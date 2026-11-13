#include "video.h"
#include "capture.h"
#include "config.h"
#include "runtime.h"
#include <stdio.h>
#include <limits.h>
#include "rk_mpi_mb.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_vi.h"
bool start_video(struct video_stream *video)
{
    int channel = video->channel;
    VI_CHN_ATTR_S vi = {0};
    vi.stIspOpt.u32BufCount = 2;
    vi.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    vi.stSize.u32Width = video->width;
    vi.stSize.u32Height = video->height;
    vi.enPixelFormat = RK_FMT_YUV420SP;
    vi.enCompressMode = COMPRESS_MODE_NONE;
    vi.stFrameRate.s32SrcFrameRate = -1;
    vi.stFrameRate.s32DstFrameRate = -1;
    if (!check("VI SetChnAttr", channel, RK_MPI_VI_SetChnAttr(CAMERA, channel, &vi)) ||
        !check("VI EnableChn", channel, RK_MPI_VI_EnableChn(CAMERA, channel)))
        return false;
    video->vi_enabled = true;

    VENC_CHN_ATTR_S encoder = {0};
    encoder.stVencAttr.enType = video->h264 ? RK_VIDEO_ID_AVC : RK_VIDEO_ID_HEVC;
    encoder.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;
    encoder.stVencAttr.u32PicWidth = encoder.stVencAttr.u32VirWidth = video->width;
    encoder.stVencAttr.u32PicHeight = encoder.stVencAttr.u32VirHeight = video->height;
    encoder.stVencAttr.u32StreamBufCnt = 2;
    encoder.stVencAttr.u32BufSize = video->width * video->height * 3 / 2;
    if (video->h264) {
        encoder.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        VENC_H264_CBR_S *rate = &encoder.stRcAttr.stH264Cbr;
        rate->u32BitRate = video->bitrate;
        rate->u32Gop = video->fps;
        rate->u32SrcFrameRateNum = video->fps;
        rate->fr32DstFrameRateNum = video->fps;
        rate->u32SrcFrameRateDen = rate->fr32DstFrameRateDen = 1;
    } else {
        encoder.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        VENC_H265_CBR_S *rate = &encoder.stRcAttr.stH265Cbr;
        rate->u32BitRate = video->bitrate;
        rate->u32Gop = video->fps;
        rate->u32SrcFrameRateNum = video->fps;
        rate->fr32DstFrameRateNum = video->fps;
        rate->u32SrcFrameRateDen = rate->fr32DstFrameRateDen = 1;
    }
    if (!check("VENC CreateChn", channel, RK_MPI_VENC_CreateChn(channel, &encoder)))
        return false;
    video->encoder_created = true;

    VENC_RECV_PIC_PARAM_S receive = { .s32RecvPicNum = -1 };
    if (!check("VENC StartRecvFrame", channel, RK_MPI_VENC_StartRecvFrame(channel, &receive)))
        return false;
    video->receiving = true;

    MPP_CHN_S source = { .enModId = RK_ID_VI, .s32DevId = CAMERA, .s32ChnId = channel };
    MPP_CHN_S target = { .enModId = RK_ID_VENC, .s32DevId = 0, .s32ChnId = channel };
    video->bound = check("VI to VENC Bind", channel, RK_MPI_SYS_Bind(&source, &target));
    return video->bound;
}

void stop_video(const struct video_stream *video)
{
    capture_close(video->channel);
    int channel = video->channel;
    MPP_CHN_S source = { .enModId = RK_ID_VI, .s32DevId = CAMERA, .s32ChnId = channel };
    MPP_CHN_S target = { .enModId = RK_ID_VENC, .s32DevId = 0, .s32ChnId = channel };
    if (video->bound)
        check("VI to VENC UnBind", channel, RK_MPI_SYS_UnBind(&source, &target));
    if (video->receiving)
        check("VENC StopRecvFrame", channel, RK_MPI_VENC_StopRecvFrame(channel));
    if (video->encoder_created)
        check("VENC DestroyChn", channel, RK_MPI_VENC_DestroyChn(channel));
    if (video->vi_enabled)
        check("VI DisableChn", channel, RK_MPI_VI_DisableChn(CAMERA, channel));
}

bool send_frame(struct video_stream *video)
{
    // The RV1106 example retrieves a complete encoded frame in one VENC pack.
    VENC_PACK_S packet = {0};
    VENC_STREAM_S frame = { .pstPack = &packet, .u32PackCount = 1 };
    int ret = RK_MPI_VENC_GetStream(video->channel, &frame, GET_TIMEOUT_MS);
    if (ret != RK_SUCCESS) {
        if (monotonic_ms() - video->last_frame_ms < 5000)
            return true;
        fprintf(stderr, "NO_FRAME path=%s for 5 seconds (last VENC result=0x%08x)\n",
                video->path, (unsigned)ret);
        return false;
    }

    const uint8_t *data = RK_MPI_MB_Handle2VirAddr(packet.pMbBlk);
    if (data && packet.u32Len && packet.u32Len <= INT_MAX)
        capture_frame(video->channel, data, packet.u32Len, packet.u64PTS, video->h264);
    if (!data || packet.u32Len == 0 || packet.u32Len > INT_MAX ||
        rtsp_tx_video(video->session, data, (int)packet.u32Len, packet.u64PTS) < 0)
        ++video->tx_errors;
    ++video->frames;
    ++video->total_frames;
    video->last_frame_ms = monotonic_ms();
    return check("VENC ReleaseStream", video->channel,
                 RK_MPI_VENC_ReleaseStream(video->channel, &frame));
}
