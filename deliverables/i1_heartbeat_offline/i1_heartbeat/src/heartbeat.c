#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <curl/curl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t stopping;
static int use_syslog;
static void stop(int sig) { (void)sig; stopping=1; }
static void report(const char *fmt, ...) {
    char message[768]; va_list ap;
    va_start(ap,fmt); vsnprintf(message,sizeof message,fmt,ap); va_end(ap);
    if(use_syslog) syslog(LOG_INFO,"%s",message);
    else { puts(message); fflush(stdout); }
}
static long long now_ms(void) {
    struct timespec t;
    if(clock_gettime(CLOCK_MONOTONIC,&t)) { perror("clock_gettime"); exit(2); }
    return (long long)t.tv_sec*1000+t.tv_nsec/1000000;
}
static int read_line(const char *path, char *out, size_t size) {
    FILE *f=fopen(path,"r");
    if(!f) return -1;
    if(!fgets(out,(int)size,f)) { fclose(f); return -1; }
    fclose(f); out[strcspn(out,"\r\n")]=0; return 0;
}
static int interface_name(const char *s) {
    size_t i,n=strlen(s);
    if(!n || n>=IF_NAMESIZE) return 0;
    for(i=0;i<n;i++) if(!((s[i]>='a'&&s[i]<='z') || (s[i]>='A'&&s[i]<='Z') ||
        (s[i]>='0'&&s[i]<='9') || s[i]=='_' || s[i]=='-' || s[i]=='.')) return 0;
    return 1;
}
static int mac_from_interface(const char *name, char mac[13]) {
    char path[128],line[128];
    snprintf(path,sizeof path,"/sys/class/net/%s/address",name);
    if(read_line(path,line,sizeof line)) return -1;
    return hb_mac(line,mac);
}
static int wg_up(const char *name) {
    char path[128],line[128]; unsigned long flags;
    snprintf(path,sizeof path,"/sys/class/net/%s/flags",name);
    if(read_line(path,line,sizeof line)) return 0;
    flags=strtoul(line,NULL,0);
    /* I1 administrative IFF_UP only; WireGuard operstate often is 'unknown'. */
    return (flags & 1UL)!=0;
}
static int rkipc_running(void) {
    DIR *d=opendir("/proc"); struct dirent *entry;
    if(!d) return 0;
    while((entry=readdir(d))!=NULL) {
        char path[512],name[128],state[256];
        if(strspn(entry->d_name,"0123456789")!=strlen(entry->d_name)) continue;
        snprintf(path,sizeof path,"/proc/%s/comm",entry->d_name);
        if(read_line(path,name,sizeof name) || strcmp(name,"rkipc")) continue;
        snprintf(path,sizeof path,"/proc/%s/stat",entry->d_name);
        if(!read_line(path,state,sizeof state)) {
            const char *end=strrchr(state,')');
            if(end && end[1]==' ' && end[2] && end[2]!='Z' && end[2]!='X') {
                closedir(d); return 1;
            }
        }
    }
    closedir(d); return 0;
}
struct response_buffer {
    char *data;
    size_t used, capacity;
};
static size_t receive_body(char *data, size_t size, size_t count, void *userdata) {
    struct response_buffer *buffer=userdata;
    size_t bytes;
    if(size && count>(size_t)-1/size) return 0;
    bytes=size*count;
    if(stopping || bytes>buffer->capacity-1-buffer->used) return 0;
    memcpy(buffer->data+buffer->used,data,bytes);
    buffer->used+=bytes;
    buffer->data[buffer->used]=0;
    return bytes;
}
static int post(const char *host, int port, const char *payload, int timeout_ms,
                int *status, char *body, size_t capacity, char *error) {
    CURL *curl=curl_easy_init();
    struct curl_slist *headers=NULL;
    struct response_buffer buffer={body,0,capacity};
    char url[128];
    CURLcode rc=CURLE_FAILED_INIT;
    long http_status=0;
    error[0]=0;
    if(!curl) { snprintf(error,CURL_ERROR_SIZE,"curl_easy_init failed"); return -1; }
    body[0]=0;
    snprintf(url,sizeof url,"http://%s:%d/v1/devices/heartbeat",host,port);
    headers=curl_slist_append(NULL,"Content-Type: application/json");
    if(!headers) { rc=CURLE_OUT_OF_MEMORY; goto done; }
#define SETOPT(option, value) do { rc=curl_easy_setopt(curl,option,value); if(rc!=CURLE_OK) goto done; } while(0)
    SETOPT(CURLOPT_ERRORBUFFER,error);
    SETOPT(CURLOPT_URL,url);
    SETOPT(CURLOPT_PROXY,""); /* VPN target must not use environment proxies. */
    SETOPT(CURLOPT_POST,1L);
    SETOPT(CURLOPT_HTTPHEADER,headers);
    SETOPT(CURLOPT_POSTFIELDS,payload);
    SETOPT(CURLOPT_POSTFIELDSIZE,(long)strlen(payload));
    SETOPT(CURLOPT_TIMEOUT_MS,(long)timeout_ms);
    SETOPT(CURLOPT_CONNECTTIMEOUT_MS,(long)timeout_ms);
    SETOPT(CURLOPT_NOSIGNAL,1L);
    SETOPT(CURLOPT_FOLLOWLOCATION,0L);
    SETOPT(CURLOPT_FAILONERROR,0L); /* Preserve HTTP 403 for application logging. */
    SETOPT(CURLOPT_WRITEFUNCTION,receive_body);
    SETOPT(CURLOPT_WRITEDATA,&buffer);
#undef SETOPT
    rc=curl_easy_perform(curl);
    if(rc==CURLE_OK) rc=curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_status);
    if(rc==CURLE_OK) *status=(int)http_status;
done:
    if(rc!=CURLE_OK && !error[0]) snprintf(error,CURL_ERROR_SIZE,"%s",curl_easy_strerror(rc));
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return rc==CURLE_OK?0:-1;
}
static int number(const char *s, long min, long max) {
    char *end; long n;
    errno=0; n=strtol(s,&end,10);
    if(errno || !*s || *end || n<min || n>max) return -1;
    return (int)n;
}
static int lock_pidfile(const char *path) {
    struct flock lock; char pid[32]; int n;
    int fd=open(path,O_CREAT|O_RDWR,0644);
    if(fd<0) return -1;
    memset(&lock,0,sizeof lock); lock.l_type=F_WRLCK; lock.l_whence=SEEK_SET;
    if(fcntl(fd,F_SETLK,&lock)<0) { close(fd); return -1; }
    n=snprintf(pid,sizeof pid,"%ld\n",(long)getpid());
    if(ftruncate(fd,0) || write(fd,pid,(size_t)n)!=n) { close(fd); return -1; }
    return fd;
}
static void usage(void) {
    puts("heartbeat [--dry-run | --send --host IPv4] [--port N] [--count N]\n"
         "  [--mac-interface eth0] [--wg-interface wg1] [--timeout-ms 3000]\n"
         "  [--pidfile /var/run/i1-heartbeat.pid] [--syslog]\n"
         "Default: dry-run once, no sockets, no server contact.\n"
         "Send mode: explicit numeric --host required, port 3000, every 5 seconds.\n"
         "--count 0 repeats until Ctrl+C. rtsp_ok checks non-zombie rkipc only.");
}
int main(int argc, char **argv) {
    const char *host=NULL,*mac_interface="eth0",*wg_interface="wg1";
    const char *pidfile="/var/run/i1-heartbeat.pid";
    int send_mode=0,mode_seen=0,port=3000,timeout_ms=3000,count=-1,lockfd=-1,i;
    unsigned long attempt=0;
    struct sigaction action;
    for(i=1;i<argc;i++) {
        if(!strcmp(argv[i],"--help")) { usage(); return 0; }
        if(!strcmp(argv[i],"--send") || !strcmp(argv[i],"--dry-run")) {
            if(mode_seen++) { fprintf(stderr,"Choose only one mode\n"); return 2; }
            send_mode=!strcmp(argv[i],"--send"); continue;
        }
        if(!strcmp(argv[i],"--syslog")) { use_syslog=1; continue; }
        if(i+1>=argc) { usage(); return 2; }
        if(!strcmp(argv[i],"--host")) host=argv[++i];
        else if(!strcmp(argv[i],"--port")) { port=number(argv[++i],1,65535); if(port<0) return 2; }
        else if(!strcmp(argv[i],"--count")) { count=number(argv[++i],0,1000000); if(count<0) return 2; }
        else if(!strcmp(argv[i],"--timeout-ms")) { timeout_ms=number(argv[++i],100,4000); if(timeout_ms<0) return 2; }
        else if(!strcmp(argv[i],"--mac-interface")) mac_interface=argv[++i];
        else if(!strcmp(argv[i],"--wg-interface")) wg_interface=argv[++i];
        else if(!strcmp(argv[i],"--pidfile")) pidfile=argv[++i];
        else { usage(); return 2; }
    }
    if(!interface_name(mac_interface) || !interface_name(wg_interface)) return 2;
    if(send_mode) {
        struct in_addr a;
        if(!host || inet_pton(AF_INET,host,&a)!=1 || a.s_addr==INADDR_ANY ||
           ntohl(a.s_addr)>=0xe0000000UL) {
            fprintf(stderr,"--send requires an explicit unicast IPv4 --host\n"); return 2;
        }
    }
    if(count<0) count=send_mode?0:1;
    memset(&action,0,sizeof action); action.sa_handler=stop; sigemptyset(&action.sa_mask);
    sigaction(SIGINT,&action,NULL); sigaction(SIGTERM,&action,NULL);
    signal(SIGPIPE,SIG_IGN);
    if(use_syslog) openlog("i1-heartbeat",LOG_PID,LOG_USER);
    if(send_mode) {
        lockfd=lock_pidfile(pidfile);
        if(lockfd<0) { report("PID_LOCK_ERROR %s: %s",pidfile,strerror(errno)); return 2; }
    }
    if(send_mode && curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK) {
        report("CURL_INIT_FAILED");
        close(lockfd);
        return 2;
    }
    report("START mode=%s interval_ms=5000 timeout_ms=%d mac_interface=%s wg_interface=%s",
           send_mode?"SEND":"DRY_RUN_NO_NETWORK",timeout_ms,mac_interface,wg_interface);
    while(!stopping && (!count || attempt<(unsigned long)count)) {
        char mac[13],payload[256],body[8192],error[CURL_ERROR_SIZE];
        int status=0,rtsp,wg; long long next=now_ms()+5000;
        ++attempt;
        if(mac_from_interface(mac_interface,mac)) {
            report("MAC_UNAVAILABLE interface=%s attempt=%lu; no request sent",mac_interface,attempt);
            if(!send_mode) { if(lockfd>=0) close(lockfd); return 2; }
        } else {
            rtsp=rkipc_running(); wg=wg_up(wg_interface);
            if(hb_payload(payload,sizeof payload,mac,rtsp,wg)<0) return 2;
            if(!send_mode) report("DRY_RUN attempt=%lu payload=%s",attempt,payload);
            else if(post(host,port,payload,timeout_ms,&status,body,sizeof body,error)) {
                if(!stopping) report("SEND_FAILED attempt=%lu reason=%s; retry next period",attempt,error);
            } else if(status==200) {
                unsigned long version;
                if(hb_config_version(body,&version)) report("HTTP_200 attempt=%lu config_version=missing_or_invalid (log only)",attempt);
                else report("HTTP_200 attempt=%lu config_version=%lu (log only)",attempt,version);
            } else if(status==400) report("HTTP_400 attempt=%lu check required fields and JSON types; retry next period",attempt);
            else if(status==403) report("HTTP_403 attempt=%lu check VPN IP/MAC registration; retry next period",attempt);
            else report("HTTP_%d attempt=%lu; retry next period",status,attempt);
        }
        if(count && attempt>=(unsigned long)count) break;
        while(!stopping && now_ms()<next) {
            struct timespec delay; long long left=next-now_ms();
            if(left<=0) break;
            delay.tv_sec=(time_t)(left/1000); delay.tv_nsec=(long)(left%1000)*1000000;
            nanosleep(&delay,NULL);
        }
    }
    report("STOP attempts=%lu",attempt);
    /* Keep the inode: unlinking can race another process acquiring its lock. */
    if(lockfd>=0) close(lockfd);
    if(send_mode) curl_global_cleanup();
    if(use_syslog) closelog();
    return 0;
}
