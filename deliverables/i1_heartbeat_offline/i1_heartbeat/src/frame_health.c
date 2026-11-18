#include "frame_health.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/* Status is atomically published by cctv. Missing/stale/ambiguous input fails closed. */
int hb_frame_health(const char *path, uint64_t expected_pid, uint64_t now_ms)
{
    static const char *keys[]={"pid","monotonic_ms","main_frames","sub_frames","main_last_ms","sub_last_ms"};
    uint64_t values[6]={0}; unsigned seen=0; int running=0, bad=0;
    char line[256]; FILE *f=fopen(path,"r");
    if (!f) return 0;
    while (fgets(line,sizeof line,f)) {
        if (!strchr(line,'\n') && !feof(f)) {bad=1; break;}
        line[strcspn(line,"\r\n")]=0;
        char *eq=strchr(line,'='); if (!eq) {bad=1; break;}
        *eq++=0;
        if (!strcmp(line,"state")) {
            if (seen & 64) bad=1;
            seen|=64; running=!strcmp(eq,"running");
            continue;
        }
        for (unsigned i=0;i<6;i++) if (!strcmp(line,keys[i])) {
            if ((seen & (1U<<i)) || !*eq || strspn(eq,"0123456789")!=strlen(eq)) {bad=1; break;}
            errno=0; char *end; unsigned long long v=strtoull(eq,&end,10);
            if (errno || *end) {bad=1; break;}
            values[i]=v; seen|=1U<<i; break;
        }
    }
    if (ferror(f)) bad=1;
    fclose(f);
    if (bad || seen!=127 || !running || expected_pid<=1 || values[0]!=expected_pid || !values[2] || !values[3]) return 0;
    unsigned stamps[]={1,4,5};
    for (unsigned i=0;i<3;i++) {
        uint64_t stamp=values[stamps[i]];
        if (!stamp || stamp>now_ms || now_ms-stamp>5000) return 0;
    }
    if (values[4]>values[1] || values[5]>values[1]) return 0;
    return 1;
}