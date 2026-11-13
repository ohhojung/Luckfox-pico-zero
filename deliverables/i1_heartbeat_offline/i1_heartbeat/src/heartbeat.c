#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include "config.h"
#include "platform.h"
#include "runtime.h"
#include "transport.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

static int number(const char *s, long min, long max) {
    char *end; long n;
    errno=0; n=strtol(s,&end,10);
    if(errno || !*s || *end || n<min || n>max) return -1;
    return (int)n;
}
static void usage(void) {
    puts("heartbeat [--dry-run | --send] [--host IPv4] [--port N] [--count N]\n"
         "  [--mac-interface wlan0] [--wg-interface wg0] [--timeout-ms 3000]\n"
         "  [--rtsp-process rkipc|cctv-dual-rtsp]\n"
         "  [--pidfile /var/run/i1-heartbeat.pid] [--syslog]\n"
         "Default: send to " DEFAULT_HEARTBEAT_HOST ":3000 every 5 seconds.\n"
         "--dry-run prints one payload by default without network access.\n"
         "--count 0 repeats until Ctrl+C. rtsp_ok checks the selected non-zombie process.");
}
int main(int argc, char **argv) {
    const char *host=DEFAULT_HEARTBEAT_HOST,*mac_interface=DEFAULT_MAC_INTERFACE,*wg_interface=DEFAULT_WG_INTERFACE;
    const char *pidfile="/var/run/i1-heartbeat.pid";
    const char *rtsp_process=DEFAULT_RTSP_PROCESS;
    int send_mode=1,mode_seen=0,port=DEFAULT_HEARTBEAT_PORT,timeout_ms=DEFAULT_TIMEOUT_MS,count=-1,lockfd=-1,i;
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
        else if(!strcmp(argv[i],"--rtsp-process")) rtsp_process=argv[++i];
        else if(!strcmp(argv[i],"--pidfile")) pidfile=argv[++i];
        else { usage(); return 2; }
    }
    if(!interface_name(mac_interface) || !interface_name(wg_interface)) return 2;
    if(strcmp(rtsp_process,"rkipc") && strcmp(rtsp_process,"cctv-dual-rtsp")) {
        fprintf(stderr,"--rtsp-process must be rkipc or cctv-dual-rtsp\n"); return 2;
    }
    if(send_mode) {
        struct in_addr a;
        if(inet_pton(AF_INET,host,&a)!=1 || a.s_addr==INADDR_ANY ||
           ntohl(a.s_addr)>=0xe0000000UL) {
            fprintf(stderr,"--host must be a unicast IPv4 address\n"); return 2;
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
        int status=0,rtsp,wg; long long next=now_ms()+HEARTBEAT_INTERVAL_MS;
        ++attempt;
        if(mac_from_interface(mac_interface,mac)) {
            report("MAC_UNAVAILABLE interface=%s attempt=%lu; no request sent",mac_interface,attempt);
            if(!send_mode) { if(lockfd>=0) close(lockfd); return 2; }
        } else {
            rtsp=rtsp_running(rtsp_process); wg=wg_up(wg_interface);
            if(hb_payload(payload,sizeof payload,mac,rtsp,wg)<0) return 2;
            if(!send_mode) report("DRY_RUN attempt=%lu payload=%s",attempt,payload);
            else if(post(host,port,wg_interface,payload,timeout_ms,&status,body,sizeof body,error,&stopping)) {
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
