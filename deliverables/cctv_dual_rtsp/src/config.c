#include "config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
const struct stream_profile stream_profiles[STREAM_COUNT] = {
    {0, "/live/0", 1920, 1080, 4096, 20},
    {1, "/live/1", 960, 540, 1024, 20},
};
int parse_options(int argc, char **argv, struct stream_options *options)
{
    const char *iq_dir = NULL;
    const char *usage = "Usage: %s [-a IQ_DIR] [-F 30 [-T]] | -N\n";
    bool fixed_sensor_fps = false;
    bool test_tnr = false;
    bool skip_aiq = false;
    int option;
    while ((option = getopt(argc, argv, "a:F:TNh")) != -1) {
        if (option == 'a') {
            iq_dir = optarg;
            continue;
        }
        if (option == 'F' && strcmp(optarg, "30") == 0) {
            fixed_sensor_fps = true;
            continue;
        }
        if (option == 'T') {
            test_tnr = true;
            continue;
        }
        if (option == 'N') {
            skip_aiq = true;
            continue;
        }
        fprintf(option == 'h' ? stdout : stderr, usage, argv[0]);
        return option == 'h' ? 1 : -1;
    }
    if (optind != argc || (test_tnr && !fixed_sensor_fps) ||
        (skip_aiq && (iq_dir || fixed_sensor_fps))) {
        fprintf(stderr, usage, argv[0]);
        return -1;
    }

    *options=(struct stream_options){iq_dir,fixed_sensor_fps,test_tnr,skip_aiq};
    return 0;
}
