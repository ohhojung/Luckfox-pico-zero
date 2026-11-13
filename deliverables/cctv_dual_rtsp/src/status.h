#ifndef CCTV_STATUS_H
#define CCTV_STATUS_H
#include "video.h"
void publish_status(const char *state, struct video_stream *v, unsigned attempt);
#endif
