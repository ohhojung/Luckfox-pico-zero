#ifndef CCTV_VIDEO_H
#define CCTV_VIDEO_H
#include <stdbool.h>
#include <stdint.h>
#include "rtsp_demo.h"
struct video_stream {
    int channel;
    bool h264;
    const char *path;
    unsigned width;
    unsigned height;
    unsigned bitrate;
    unsigned fps;
    uint64_t total_frames;
    uint64_t last_pts, pts_min_delta, pts_max_delta;
    unsigned pts_samples, pts_non_increasing;
    bool have_pts;
    rtsp_session_handle session;
    bool vi_enabled;
    bool encoder_created;
    bool receiving;
    bool bound;
    unsigned frames;
    unsigned tx_errors;
    uint64_t last_frame_ms;
};

bool start_video(struct video_stream *video);
void stop_video(const struct video_stream *video);
bool send_frame(struct video_stream *video);
#endif
