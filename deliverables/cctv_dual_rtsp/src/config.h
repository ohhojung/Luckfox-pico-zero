#ifndef CCTV_CONFIG_H
#define CCTV_CONFIG_H
#include <stdbool.h>
enum { CAMERA=0, PORT=554, SENSOR_REQUEST_FPS=30, GET_TIMEOUT_MS=10, STREAM_COUNT=2 };
struct stream_profile { int channel; const char *path; unsigned width, height, bitrate, fps; };
extern const struct stream_profile stream_profiles[STREAM_COUNT];
struct stream_options { const char *iq_dir; bool fixed_sensor_fps, test_tnr, skip_aiq; };
/* Returns 0 to run, 1 for help, -1 for invalid options. Caller resets getopt. */
int parse_options(int argc, char **argv, struct stream_options *options);
#endif
