#ifndef CCTV_RTSP_SERVER_H
#define CCTV_RTSP_SERVER_H
#include <stdbool.h>
#include <stdint.h>
#include "rtsp_demo.h"
/* Called only on the pipeline thread, like rtsp_tx_video in video.c. */
bool rtsp_add_video(rtsp_demo_handle server, const char *path, rtsp_session_handle *session,
                    uint64_t relative, uint64_t ntp);
#endif
