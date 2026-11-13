#include "capture.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
int main(void)
{
    char dir[]="/tmp/cctv-capture-test-XXXXXX";
    assert(mkdtemp(dir));
    assert(setenv("CCTV_DUMP_DIR",dir,1)==0);
    unsigned char bytes[]={0,0,1,42};
    for (int i=0; i<205; ++i) capture_frame(0,bytes,sizeof bytes,i+1,1);
    capture_close(0);
    char path[256];
    snprintf(path,sizeof path,"%s/live-0.h264",dir);
    struct stat st;
    assert(stat(path,&st)==0 && st.st_size==800);
    capture_frame(0,bytes,sizeof bytes,300,1);
    assert(stat(path,&st)==0 && st.st_size==800);
    /* Refuse an existing output without overwriting its contents. */
    char existing[256];
    snprintf(existing,sizeof existing,"%s/live-1.h265",dir);
    FILE *f=fopen(existing,"wb"); assert(f);
    assert(fwrite("keep",1,4,f)==4); assert(fclose(f)==0);
    capture_frame(1,bytes,sizeof bytes,1,0);
    f=fopen(existing,"rb"); assert(f);
    char content[4]; assert(fread(content,1,4,f)==4); fclose(f);
    assert(content[0]=='k' && content[3]=='p');
    unlink(path); unlink(existing); rmdir(dir);
    puts("PASS capture frame limit, codec extension, no reopening and no overwrite");
    return 0;
}
