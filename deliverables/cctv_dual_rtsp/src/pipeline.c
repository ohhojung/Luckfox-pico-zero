#include "pipeline.h"
#include "config.h"
#include "runtime.h"
#include "video.h"
#include "isp.h"
#include "status.h"
#include "rtsp_server.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int run_once(int argc, char **argv, unsigned restart_attempt)
{
    struct stream_options options;
    int parsed=parse_options(argc,argv,&options);
    if (parsed) return parsed>0 ? EXIT_SUCCESS : EXIT_FAILURE;
    const char *iq_dir=options.iq_dir;
    bool fixed_sensor_fps=options.fixed_sensor_fps;
    bool test_tnr=options.test_tnr;
    bool skip_aiq=options.skip_aiq;

    setvbuf(stdout, NULL, _IOLBF, 0);
    struct sigaction action = { .sa_handler = stop };
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) || sigaction(SIGTERM, &action, NULL)) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    struct video_stream videos[STREAM_COUNT]={0};
    const size_t video_count=STREAM_COUNT;
    for (size_t i=0; i<video_count; ++i) {
        const struct stream_profile *p=&stream_profiles[i];
        videos[i]=(struct video_stream){.channel=p->channel,.path=p->path,.width=p->width,
            .height=p->height,.bitrate=p->bitrate,.fps=p->fps,.h264=options.h264};
    }
    publish_status("starting", videos, restart_attempt);
    alarm(30);
    int status = EXIT_FAILURE;
    bool isp_started = false, system_started = false, device_enabled = false;
    rk_aiq_sys_ctx_t *isp = NULL;
    rtsp_demo_handle rtsp = create_rtsp_demo(PORT);
    if (!rtsp) {
        fprintf(stderr, "Cannot start RTSP on port %d. Check for an existing camera service.\n", PORT);
        publish_status("failed", videos, restart_attempt);
        return EXIT_FAILURE;
    }

    if (skip_aiq) {
        printf("DIAGNOSTIC aiq=disabled\n");
    } else if (!isp_start(iq_dir,&isp,&isp_started)) {
        goto cleanup;
    }
    if (!check("SYS Init", CAMERA, RK_MPI_SYS_Init()))
        goto cleanup;
    system_started = true;
    if (RK_MPI_VI_GetDevIsEnable(CAMERA) == RK_SUCCESS) {
        fprintf(stderr, "Camera already active. Stop the existing camera program first.\n");
        goto cleanup;
    }

    VI_DEV_ATTR_S device = {0};
    int ret = RK_MPI_VI_GetDevAttr(CAMERA, &device);
    if (ret == RK_ERR_VI_NOT_CONFIG)
        ret = RK_MPI_VI_SetDevAttr(CAMERA, &device);
    if (!check("VI ConfigureDev", CAMERA, ret) ||
        !check("VI EnableDev", CAMERA, RK_MPI_VI_EnableDev(CAMERA)))
        goto cleanup;
    device_enabled = true;
    VI_DEV_BIND_PIPE_S pipe = { .u32Num = 1, .PipeId = { CAMERA } };
    if (!check("VI SetDevBindPipe", CAMERA, RK_MPI_VI_SetDevBindPipe(CAMERA, &pipe)))
        goto cleanup;

    uint64_t relative = rtsp_get_reltime(), ntp = rtsp_get_ntptime();
    for (size_t i = 0; i < video_count; ++i) {
        struct video_stream *video = &videos[i];
        if (!rtsp_add_video(rtsp,video->path,&video->session,relative,ntp,video->h264))
            goto cleanup;
        if (!start_video(video))
            goto cleanup;
    }

    uint64_t last_report_ms = monotonic_ms();
    uint64_t last_status_ms = 0;
    bool fps_requested = false;
    bool tnr_requested = false;
    for (size_t i = 0; i < video_count; ++i) {
        videos[i].last_frame_ms = last_report_ms;
        printf("READY port=%d path=%s codec=%s size=%ux%u target_fps=%u bitrate_kbps=%u\n",
               PORT, videos[i].path, videos[i].h264 ? "H264" : "H265", videos[i].width, videos[i].height, videos[i].fps, videos[i].bitrate);
    }
    // All RTSP calls run on this thread. Short per-channel waits keep both sessions serviced.
    while (!stopping) {
        alarm(15);
        if (rtsp_do_event(rtsp) < 0 && !stopping) {
            fprintf(stderr, "RTSP event loop failed\n");
            goto cleanup;
        }
        for (size_t i = 0; i < video_count && !stopping; ++i)
            if (!send_frame(&videos[i]))
                goto cleanup;

        uint64_t now = monotonic_ms();
        if (now - last_status_ms >= 1000) {
            bool healthy = videos[0].total_frames && videos[1].total_frames &&
                now - videos[0].last_frame_ms < 2000 && now - videos[1].last_frame_ms < 2000;
            publish_status(healthy ? "running" : (videos[0].total_frames && videos[1].total_frames ? "degraded" : "starting"), videos, restart_attempt);
            last_status_ms = now;
            limit_log();
        }
        if (now - last_report_ms >= 5000) {
            bool both_streaming = videos[0].frames > 0 && videos[1].frames > 0;
            if (fixed_sensor_fps) {
                const char *stage = "baseline";
                if (tnr_requested)
                    stage = "after_tnr_request";
                else if (fps_requested)
                    stage = "after_request";
                report_sensor_timing(isp, stage);
            }
            for (size_t i = 0; i < video_count; ++i) {
                struct video_stream *video = &videos[i];
                double fps = video->frames * 1000.0 / (now - last_report_ms);
                printf("STREAM path=%s venc_fps=%.2f rtsp_tx_errors=%u\n",
                       video->path, fps, video->tx_errors);
                video->frames = 0;
                video->tx_errors = 0;
            }
            last_report_ms = now;
            // Start with the normal sensor policy and measure both channels before changing AE.
            if (fixed_sensor_fps && !fps_requested && both_streaming && !stopping) {
                if (!request_sensor_fps(isp))
                    goto cleanup;
                fps_requested = true;
            } else if (test_tnr && !tnr_requested && both_streaming && !stopping) {
                report_bay3d("before_tnr_request");
                if (!disable_temporal_nr(isp))
                    goto cleanup;
                tnr_requested = true;
            } else if (tnr_requested) {
                report_bay3d("after_tnr_request");
            }
        }
    }
    status = EXIT_SUCCESS;

cleanup:
    alarm(15);
    publish_status(stopping ? "stopping" : "recovering", videos, restart_attempt);
    for (size_t i = video_count; i > 0; --i)
        stop_video(&videos[i - 1]);
    if (device_enabled)
        check("VI DisableDev", CAMERA, RK_MPI_VI_DisableDev(CAMERA));
    isp_stop(isp,isp_started);
    if (system_started)
        check("SYS Exit", CAMERA, RK_MPI_SYS_Exit());
    rtsp_del_demo(rtsp);
    alarm(0);
    publish_status(stopping ? "stopped" : "failed", videos, restart_attempt);
    return status;
}
