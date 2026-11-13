#include "config.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
static int parse(int argc, char **argv, struct stream_options *options)
{
    optind=0;
    opterr=0;
    return parse_options(argc,argv,options);
}
int main(void)
{
    struct stream_options o;
    char *defaults[]={"streamer",NULL};
    assert(parse(1,defaults,&o)==0);
    assert(!o.iq_dir && !o.fixed_sensor_fps && !o.test_tnr && !o.skip_aiq);
    assert(!o.h264);
    char *avc[]={"streamer","-c","h264",NULL};
    assert(parse(3,avc,&o)==0 && o.h264);
    char *hevc[]={"streamer","-c","h265",NULL};
    assert(parse(3,hevc,&o)==0 && !o.h264);
    char *badcodec[]={"streamer","-c","jpeg",NULL};
    assert(parse(3,badcodec,&o)==-1);
    char *production[]={"streamer","-a","/oem/usr/share/iqfiles","-F","30",NULL};
    assert(parse(5,production,&o)==0);
    assert(!strcmp(o.iq_dir,"/oem/usr/share/iqfiles") && o.fixed_sensor_fps && !o.skip_aiq);
    /* Retries reset getopt and must get exactly the same options. */
    assert(parse(5,production,&o)==0 && o.fixed_sensor_fps);
    char *tnr[]={"streamer","-F","30","-T",NULL};
    assert(parse(4,tnr,&o)==0 && o.test_tnr);
    char *noaiq[]={"streamer","-N",NULL};
    assert(parse(2,noaiq,&o)==0 && o.skip_aiq);
    char *invalid[]={"streamer","-N","-F","30",NULL};
    assert(parse(4,invalid,&o)==-1);
    char *invalid_tnr[]={"streamer","-T",NULL};
    assert(parse(2,invalid_tnr,&o)==-1);
    char *invalid_fps[]={"streamer","-F","20",NULL};
    assert(parse(3,invalid_fps,&o)==-1);
    char *help[]={"streamer","-h",NULL};
    assert(parse(2,help,&o)==1);
    assert(STREAM_COUNT==2 && PORT==554 && SENSOR_REQUEST_FPS==30);
    assert(!strcmp(stream_profiles[0].path,"/live/0") && stream_profiles[0].width==1920 && stream_profiles[0].height==1080);
    assert(!strcmp(stream_profiles[1].path,"/live/1") && stream_profiles[1].width==960 && stream_profiles[1].height==540);
    assert(stream_profiles[0].fps==20 && stream_profiles[1].fps==20);
    assert(stream_profiles[0].bitrate==4096 && stream_profiles[1].bitrate==1024);
    return 0;
}
