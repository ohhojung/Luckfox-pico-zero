#ifndef HB_FRAME_HEALTH_H
#define HB_FRAME_HEALTH_H
#include <stdint.h>
int hb_frame_health(const char *path, uint64_t expected_pid, uint64_t now_ms);
#endif