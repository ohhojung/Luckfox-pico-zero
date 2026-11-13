#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include "platform.h"
#include <dirent.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_line(const char *path, char *out, size_t size) {
    FILE *f=fopen(path,"r");
    if(!f) return -1;
    if(!fgets(out,(int)size,f)) { fclose(f); return -1; }
    fclose(f); out[strcspn(out,"\r\n")]=0; return 0;
}
int interface_name(const char *s) {
    size_t i,n=strlen(s);
    if(!n || n>=IF_NAMESIZE) return 0;
    for(i=0;i<n;i++) if(!((s[i]>='a'&&s[i]<='z') || (s[i]>='A'&&s[i]<='Z') ||
        (s[i]>='0'&&s[i]<='9') || s[i]=='_' || s[i]=='-' || s[i]=='.')) return 0;
    return 1;
}
int mac_from_interface(const char *name, char mac[13]) {
    char path[128],line[128];
    snprintf(path,sizeof path,"/sys/class/net/%s/address",name);
    if(read_line(path,line,sizeof line)) return -1;
    return hb_mac(line,mac);
}
int wg_up(const char *name) {
    char path[128],line[128]; unsigned long flags;
    snprintf(path,sizeof path,"/sys/class/net/%s/flags",name);
    if(read_line(path,line,sizeof line)) return 0;
    flags=strtoul(line,NULL,0);
    /* I1 administrative IFF_UP only; WireGuard operstate often is 'unknown'. */
    return (flags & 1UL)!=0;
}
int rtsp_running(const char *process_name) {
    DIR *d=opendir("/proc"); struct dirent *entry;
    if(!d) return 0;
    while((entry=readdir(d))!=NULL) {
        char path[512],name[128],state[256];
        if(strspn(entry->d_name,"0123456789")!=strlen(entry->d_name)) continue;
        snprintf(path,sizeof path,"/proc/%s/comm",entry->d_name);
        if(read_line(path,name,sizeof name) || strcmp(name,process_name)) continue;
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
