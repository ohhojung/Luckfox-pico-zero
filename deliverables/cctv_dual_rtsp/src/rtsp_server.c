#include "rtsp_server.h"
#include <stdio.h>
bool rtsp_add_video(rtsp_demo_handle server, const char *path, rtsp_session_handle *session,
                    uint64_t relative, uint64_t ntp, bool h264)
{
    *session=rtsp_new_session(server,path);
    if (!*session || rtsp_set_video(*session,h264 ? RTSP_CODEC_ID_VIDEO_H264 : RTSP_CODEC_ID_VIDEO_H265,NULL,0)<0 ||
        rtsp_sync_video_ts(*session,relative,ntp)<0) {
        fprintf(stderr,"Cannot configure video RTSP session %s\n",path);
        return false;
    }
    return true;
}
