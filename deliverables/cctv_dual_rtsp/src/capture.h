#ifndef CCTV_CAPTURE_H
#define CCTV_CAPTURE_H
#include <stdint.h>
#include <stddef.h>
void capture_frame(int channel, const uint8_t *data, size_t size, uint64_t pts, int h264);
void capture_close(int channel);
#endif
