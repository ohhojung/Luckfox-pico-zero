#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

#include "rtsp_demo.h"
#ifdef RV1126_RV1109
#include <rk_aiq_user_api_camgroup.h>
#include <rk_aiq_user_api_imgproc.h>
#include <rk_aiq_user_api_sysctl.h>
#else
#include <rk_aiq_user_api2_camgroup.h>
#include <rk_aiq_user_api2_imgproc.h>
#include <rk_aiq_user_api2_sysctl.h>
#include <rk_aiq_user_api2_ae.h>
#endif

#include "rk_debug.h"
#include "rk_defines.h"
#include "rk_mpi_adec.h"
#include "rk_mpi_aenc.h"
#include "rk_mpi_ai.h"
#include "rk_mpi_ao.h"
#include "rk_mpi_avs.h"
#include "rk_mpi_cal.h"
#include "rk_mpi_ivs.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_rgn.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_tde.h"
#include "rk_mpi_vdec.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_vo.h"
#include "rk_mpi_vpss.h"

#define MAX_AIQ_CTX 8
static rk_aiq_sys_ctx_t *g_aiq_ctx[MAX_AIQ_CTX];
rk_aiq_working_mode_t g_WDRMode[MAX_AIQ_CTX];
#include <stdatomic.h>
static atomic_int g_sof_cnt = 0;
static atomic_bool g_should_quit = false;

static RK_S32 g_s32FrameCnt = -1;
static RK_U32 g_u32Bitrate = 10 * 1024;
static int g_isp_fps = 0; /* 0 keeps the existing AIQ policy. */
static atomic_bool quit = false;
static bool g_vi_only = false;
static unsigned g_vi_buffers = 2;
static atomic_int g_worker_error = 0;

#ifdef RKAIQ
static void fps_readback(const char *stage) {
	Uapi_ExpSwAttrV2_t attr;
	memset(&attr, 0, sizeof(attr));
	int ret = rk_aiq_user_api2_ae_getExpSwAttr(g_aiq_ctx[0], &attr);
	if (ret != 0) {
		printf("FPS_DIAG %s get_ret=%d\n", stage, ret);
		return;
	}
	printf("FPS_DIAG %s fixed=%d value=%.2f requested=%d (configuration, not measured FPS)\n",
	       stage, (int)attr.stAuto.stFrmRate.isFpsFix,
	       (double)attr.stAuto.stFrmRate.FpsValue, g_isp_fps);
	printf("AE_STATE %s enabled=%u mode=%d run_interval=%u\n", stage,
	       (unsigned)attr.Enable, (int)attr.AecOpType, (unsigned)attr.AecRunInterval);
	Uapi_ExpQueryInfo_t info;
	memset(&info, 0, sizeof(info));
	ret = rk_aiq_user_api2_ae_queryExpResInfo(g_aiq_ctx[0], &info);
	if (ret != 0) {
		printf("AE_RESULT %s query_ret=%d\n", stage, ret);
		return;
	}
	printf("AE_RESULT %s fps=%.3f frame_lines=%.3f line_pixels=%.3f pixel_clock_mhz=%.3f (AIQ reported, not measured FPS)\n",
	       stage, (double)info.Fps, (double)info.LinePeriodsPerField,
	       (double)info.PixelPeriodsPerLine, (double)info.PixelClockFreqMHZ);
}

static void fps_reapply(void) {
	Uapi_ExpSwAttrV2_t attr;
	memset(&attr, 0, sizeof(attr));
	int ret = rk_aiq_user_api2_ae_getExpSwAttr(g_aiq_ctx[0], &attr);
	if (ret == 0) {
		attr.stAuto.stFrmRate.isFpsFix = true;
		attr.stAuto.stFrmRate.FpsValue = g_isp_fps;
		ret = rk_aiq_user_api2_ae_setExpSwAttr(g_aiq_ctx[0], attr);
	}
	printf("FPS_DIAG reapply_after_stream set_ret=%d requested=%d\n", ret, g_isp_fps);
}
#endif

rtsp_demo_handle g_rtsplive = NULL;
static rtsp_session_handle g_rtsp_session;

static void sigterm_handler(int sig) {
	fprintf(stderr, "signal %d\n", sig);
	quit = true;
}

RK_U64 TEST_COMM_GetNowUs() {
	struct timespec time = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &time);
	return (RK_U64)time.tv_sec * 1000000 + (RK_U64)time.tv_nsec / 1000; /* microseconds */
}

static void *GetMediaBuffer0(void *arg) {
	(void)arg;
	printf("========%s========\n", __func__);
	void *pData = RK_NULL;
	int loopCount = 0;
	int s32Ret;
	RK_U64 report_us = 0;
	unsigned report_frames = 0;
	int report_sof = 0;

	VENC_STREAM_S stFrame;
	stFrame.pstPack = malloc(sizeof(VENC_PACK_S));

	while (!quit) {
		s32Ret = RK_MPI_VENC_GetStream(0, &stFrame, 1000);
		if (s32Ret == RK_SUCCESS) {
			if (g_rtsplive && g_rtsp_session) {
				pData = RK_MPI_MB_Handle2VirAddr(stFrame.pstPack->pMbBlk);
				rtsp_tx_video(g_rtsp_session, pData, stFrame.pstPack->u32Len,
				              stFrame.pstPack->u64PTS);
				rtsp_do_event(g_rtsplive);
			}

			RK_U64 nowUs = TEST_COMM_GetNowUs();
			if (!report_us) {
				report_us = nowUs;
				report_sof = atomic_load(&g_sof_cnt);
			} else {
				++report_frames;
				if (nowUs - report_us >= 5000000) {
					int sof = atomic_load(&g_sof_cnt);
					double seconds = (nowUs - report_us) / 1000000.0;
					printf("MEASURED window=%.2fs aiq_sof_fps=%.2f venc_dequeue_fps=%.2f requested_isp_fps=%d\n",
					       seconds, (sof - report_sof) / seconds,
					       report_frames / seconds, g_isp_fps);
					report_us = nowUs;
					report_sof = sof;
					report_frames = 0;
				}
			}

			RK_LOGD("chn:0, loopCount:%d enc->seq:%d wd:%d pts=%lld delay=%lldus\n",
			        loopCount, stFrame.u32Seq, stFrame.pstPack->u32Len,
			        stFrame.pstPack->u64PTS, nowUs - stFrame.pstPack->u64PTS);

			s32Ret = RK_MPI_VENC_ReleaseStream(0, &stFrame);
			if (s32Ret != RK_SUCCESS) {
				RK_LOGE("RK_MPI_VENC_ReleaseStream fail %x", s32Ret);
			}
			loopCount++;
		}

		if ((g_s32FrameCnt >= 0) && (loopCount > g_s32FrameCnt)) {
			quit = true;
			break;
		}

		usleep(10 * 1000);
	}
	printf("\n======exit %s=======\n", __func__);
	free(stFrame.pstPack);
	return NULL;
}

/* Drain VI immediately: no encoding, RTSP, image copies or artificial sleep. */
static void *GetViFrames(void *arg) {
	int channel = *(RK_S32 *)arg;
	unsigned total = 0, frames = 0, failures = 0;
	int sof_start = atomic_load(&g_sof_cnt);
	RK_U64 start = TEST_COMM_GetNowUs();
	RK_U64 last_frame = start;
	while (!quit) {
		VIDEO_FRAME_INFO_S frame;
		memset(&frame, 0, sizeof(frame));
		int rc = RK_MPI_VI_GetChnFrame(0, channel, &frame, 1000);
		RK_U64 now = TEST_COMM_GetNowUs();
		if (rc == RK_SUCCESS) {
			rc = RK_MPI_VI_ReleaseChnFrame(0, channel, &frame);
			if (rc != RK_SUCCESS) {
				fprintf(stderr, "VI release failed: %x\n", rc);
				g_worker_error = 1;
				quit = true;
				break;
			}
			++total;
			++frames;
			last_frame = now;
		} else {
			++failures;
			fprintf(stderr, "VI get failed: %x\n", rc);
			if (now - last_frame >= 10000000) {
				fprintf(stderr, "VI test stopped: no frames for 10 seconds\n");
				g_worker_error = 1;
				quit = true;
			}
		}
		if (now - start >= 5000000) {
			int sof = atomic_load(&g_sof_cnt);
			double seconds = (now - start) / 1000000.0;
			printf("VI_MEASURED window=%.2fs aiq_sof_fps=%.2f vi_dequeue_fps=%.2f get_failures=%u requested_isp_fps=%d\n",
			       seconds, (sof - sof_start) / seconds, frames / seconds,
			       failures, g_isp_fps);
			start = now;
			sof_start = sof;
			frames = failures = 0;
		}
		if (g_s32FrameCnt >= 0 && total >= (unsigned)g_s32FrameCnt)
			quit = true;
	}
	printf("VI_TEST_DONE frames=%u error=%d\n", total, atomic_load(&g_worker_error));
	return NULL;
}

static RK_S32 test_venc_init(int chnId, int width, int height, RK_CODEC_ID_E enType) {
	printf("========%s========\n", __func__);
	VENC_RECV_PIC_PARAM_S stRecvParam;
	VENC_CHN_ATTR_S stAttr;
	memset(&stAttr, 0, sizeof(VENC_CHN_ATTR_S));

	if (enType == RK_VIDEO_ID_AVC) {
		stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
		stAttr.stRcAttr.stH264Cbr.u32BitRate = g_u32Bitrate;
		stAttr.stRcAttr.stH264Cbr.u32Gop = 60;
	} else if (enType == RK_VIDEO_ID_HEVC) {
		stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
		stAttr.stRcAttr.stH265Cbr.u32BitRate = g_u32Bitrate;
		stAttr.stRcAttr.stH265Cbr.u32Gop = 60;
	} else if (enType == RK_VIDEO_ID_MJPEG) {
		stAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
		stAttr.stRcAttr.stMjpegCbr.u32BitRate = g_u32Bitrate;
	}

	stAttr.stVencAttr.enType = enType;
	stAttr.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;
	if (enType == RK_VIDEO_ID_AVC)
		stAttr.stVencAttr.u32Profile = H264E_PROFILE_HIGH;
	stAttr.stVencAttr.u32PicWidth = width;
	stAttr.stVencAttr.u32PicHeight = height;
	stAttr.stVencAttr.u32VirWidth = width;
	stAttr.stVencAttr.u32VirHeight = height;
	stAttr.stVencAttr.u32StreamBufCnt = 2;
	stAttr.stVencAttr.u32BufSize = width * height * 3 / 2;
	stAttr.stVencAttr.enMirror = MIRROR_NONE;

	int rc = RK_MPI_VENC_CreateChn(chnId, &stAttr);
	if (rc != RK_SUCCESS) return rc;

	memset(&stRecvParam, 0, sizeof(VENC_RECV_PIC_PARAM_S));
	stRecvParam.s32RecvPicNum = -1;
	rc = RK_MPI_VENC_StartRecvFrame(chnId, &stRecvParam);
	if (rc != RK_SUCCESS) {
		RK_MPI_VENC_DestroyChn(chnId);
		return rc;
	}

	return 0;
}

int vi_dev_init() {
	printf("%s\n", __func__);
	int ret = 0;
	int devId = 0;
	int pipeId = devId;

	VI_DEV_ATTR_S stDevAttr;
	VI_DEV_BIND_PIPE_S stBindPipe;
	memset(&stDevAttr, 0, sizeof(stDevAttr));
	memset(&stBindPipe, 0, sizeof(stBindPipe));
	// 0. get dev config status
	ret = RK_MPI_VI_GetDevAttr(devId, &stDevAttr);
	if (ret == RK_ERR_VI_NOT_CONFIG) {
		// 0-1.config dev
		ret = RK_MPI_VI_SetDevAttr(devId, &stDevAttr);
		if (ret != RK_SUCCESS) {
			printf("RK_MPI_VI_SetDevAttr %x\n", ret);
			return -1;
		}
	} else {
		printf("RK_MPI_VI_SetDevAttr already\n");
	}
	// 1.get dev enable status
	ret = RK_MPI_VI_GetDevIsEnable(devId);
	if (ret != RK_SUCCESS) {
		// 1-2.enable dev
		ret = RK_MPI_VI_EnableDev(devId);
		if (ret != RK_SUCCESS) {
			printf("RK_MPI_VI_EnableDev %x\n", ret);
			return -1;
		}
		// 1-3.bind dev/pipe
		stBindPipe.u32Num = 1;
		stBindPipe.PipeId[0] = pipeId;
		ret = RK_MPI_VI_SetDevBindPipe(devId, &stBindPipe);
		if (ret != RK_SUCCESS) {
			printf("RK_MPI_VI_SetDevBindPipe %x\n", ret);
			return -1;
		}
	} else {
		printf("RK_MPI_VI_EnableDev already\n");
	}

	return 0;
}

int vi_chn_init(int channelId, int width, int height) {
	int ret;
	// VI init
	VI_CHN_ATTR_S vi_chn_attr;
	memset(&vi_chn_attr, 0, sizeof(vi_chn_attr));
	vi_chn_attr.stIspOpt.u32BufCount = g_vi_buffers;
	vi_chn_attr.stIspOpt.enMemoryType =
	    VI_V4L2_MEMORY_TYPE_DMABUF; // VI_V4L2_MEMORY_TYPE_MMAP;
	vi_chn_attr.stSize.u32Width = width;
	vi_chn_attr.stSize.u32Height = height;
	vi_chn_attr.enPixelFormat = RK_FMT_YUV420SP;
	vi_chn_attr.enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_AFBC_16x16;
	vi_chn_attr.u32Depth = g_vi_only ? 1 : 0;
	/* Match vendor example/vi/sample_multi_vi.c VI frame-rate settings. */
	vi_chn_attr.stFrameRate.s32SrcFrameRate = -1;
	vi_chn_attr.stFrameRate.s32DstFrameRate = -1;
	printf("VI_RATE_CONFIG src=-1 dst=-1 (configuration, not measured FPS)\n");
	printf("VI_CONFIG requested_buffers=%u depth=%u\n", g_vi_buffers,
	       (unsigned)vi_chn_attr.u32Depth);
	ret = RK_MPI_VI_SetChnAttr(0, channelId, &vi_chn_attr);
	if (ret == RK_SUCCESS) ret = RK_MPI_VI_EnableChn(0, channelId);
	if (ret) {
		printf("ERROR: create VI error! ret=%d\n", ret);
		return ret;
	}

	return ret;
}

static XCamReturn SIMPLE_COMM_ISP_SofCb(rk_aiq_metas_t *meta) {
	g_sof_cnt++;
	if (g_sof_cnt <= 2)
		printf("=== %u ===\n", meta->frame_id);
	return XCAM_RETURN_NO_ERROR;
}

static XCamReturn SIMPLE_COMM_ISP_ErrCb(rk_aiq_err_msg_t *msg) {
	if (msg->err_code == XCAM_RETURN_BYPASS)
		g_should_quit = true;
	return XCAM_RETURN_NO_ERROR;
}

RK_S32 SIMPLE_COMM_ISP_Init(RK_S32 CamId, rk_aiq_working_mode_t WDRMode, RK_BOOL MultiCam,
                            const char *iq_file_dir) {
	if (CamId >= MAX_AIQ_CTX) {
		printf("%s : CamId is over 3\n", __FUNCTION__);
		return -1;
	}
	// char *iq_file_dir = "iqfiles/";
	setlinebuf(stdout);
	if (iq_file_dir == NULL) {
		printf("SIMPLE_COMM_ISP_Init : not start.\n");
		g_aiq_ctx[CamId] = NULL;
		return 0;
	}

	// must set HDR_MODE, before init
	g_WDRMode[CamId] = WDRMode;
	char hdr_str[16];
	snprintf(hdr_str, sizeof(hdr_str), "%d", (int)WDRMode);
	setenv("HDR_MODE", hdr_str, 1);

	rk_aiq_sys_ctx_t *aiq_ctx;
	rk_aiq_static_info_t aiq_static_info;
#ifdef RV1126_RV1109
	rk_aiq_uapi_sysctl_enumStaticMetas(CamId, &aiq_static_info);

	printf("ID: %d, sensor_name is %s, iqfiles is %s\n", CamId,
	       aiq_static_info.sensor_info.sensor_name, iq_file_dir);

	aiq_ctx =
	    rk_aiq_uapi_sysctl_init(aiq_static_info.sensor_info.sensor_name, iq_file_dir,
	                             SIMPLE_COMM_ISP_ErrCb, SIMPLE_COMM_ISP_SofCb);

	if (MultiCam)
		rk_aiq_uapi_sysctl_setMulCamConc(aiq_ctx, true);
#else
	rk_aiq_uapi2_sysctl_enumStaticMetas(CamId, &aiq_static_info);

	printf("ID: %d, sensor_name is %s, iqfiles is %s\n", CamId,
	       aiq_static_info.sensor_info.sensor_name, iq_file_dir);

	aiq_ctx =
	    rk_aiq_uapi2_sysctl_init(aiq_static_info.sensor_info.sensor_name, iq_file_dir,
	                             SIMPLE_COMM_ISP_ErrCb, SIMPLE_COMM_ISP_SofCb);

	if (MultiCam)
		rk_aiq_uapi2_sysctl_setMulCamConc(aiq_ctx, true);
#endif

	g_aiq_ctx[CamId] = aiq_ctx;
	return 0;
}

RK_S32 SIMPLE_COMM_ISP_Run(RK_S32 CamId) {
	if (CamId >= MAX_AIQ_CTX || !g_aiq_ctx[CamId]) {
		printf("%s : CamId is over 3 or not init\n", __FUNCTION__);
		return -1;
	}
#ifdef RV1126_RV1109
	if (rk_aiq_uapi_sysctl_prepare(g_aiq_ctx[CamId], 0, 0, g_WDRMode[CamId])) {
		printf("rkaiq engine prepare failed !\n");
		g_aiq_ctx[CamId] = NULL;
		return -1;
	}
	printf("rk_aiq_uapi_sysctl_init/prepare succeed\n");
	if (rk_aiq_uapi_sysctl_start(g_aiq_ctx[CamId])) {
		printf("rk_aiq_uapi_sysctl_start  failed\n");
		return -1;
	}
	printf("rk_aiq_uapi_sysctl_start succeed\n");
#else
	if (rk_aiq_uapi2_sysctl_prepare(g_aiq_ctx[CamId], 0, 0, g_WDRMode[CamId])) {
		printf("rkaiq engine prepare failed !\n");
		g_aiq_ctx[CamId] = NULL;
		return -1;
	}
	printf("rk_aiq_uapi2_sysctl_init/prepare succeed\n");
	if (rk_aiq_uapi2_sysctl_start(g_aiq_ctx[CamId])) {
		printf("rk_aiq_uapi2_sysctl_start  failed\n");
		return -1;
	}
	printf("rk_aiq_uapi2_sysctl_start succeed\n");
#endif
	return 0;
}

RK_S32 SIMPLE_COMM_ISP_Stop(RK_S32 CamId) {
	if (CamId >= MAX_AIQ_CTX || !g_aiq_ctx[CamId]) {
		printf("%s : CamId is over 3 or not init g_aiq_ctx[%d] = %p\n", __FUNCTION__,
		       CamId, g_aiq_ctx[CamId]);
		return -1;
	}
#ifdef RV1126_RV1109
	printf("rk_aiq_uapi_sysctl_stop enter\n");
	rk_aiq_uapi_sysctl_stop(g_aiq_ctx[CamId], false);
	printf("rk_aiq_uapi_sysctl_deinit enter\n");
	rk_aiq_uapi_sysctl_deinit(g_aiq_ctx[CamId]);
	printf("rk_aiq_uapi_sysctl_deinit exit\n");
#else
	printf("rk_aiq_uapi2_sysctl_stop enter\n");
	rk_aiq_uapi2_sysctl_stop(g_aiq_ctx[CamId], false);
	printf("rk_aiq_uapi2_sysctl_deinit enter\n");
	rk_aiq_uapi2_sysctl_deinit(g_aiq_ctx[CamId]);
	printf("rk_aiq_uapi2_sysctl_deinit exit\n");
#endif

	g_aiq_ctx[CamId] = NULL;
	return 0;
}

static RK_CHAR optstr[] = "?a:w:h:c:I:e:b:F:VB:";
static void print_usage(const RK_CHAR *name) {
	printf("Usage example:\n");
	printf("\t%s -I 0 -w 1920 -h 1080 (rtsp://ip/live/0)\n", name);
	printf("\t-w | --width: VI width, Default:1920\n");
	printf("\t-h | --heght: VI height, Default:1080\n");
	printf("\t-a | --aiq: iq file path, Default:/etc/iqfiles\n");
	printf("\t-c | --frame_cnt: frame number of output, Default:-1\n");
	printf("\t-I | --camid: camera ctx id, Default 0. "
	       "0:rkisp_mainpath,1:rkisp_selfpath,2:rkisp_bypasspath\n");
	printf("\t-e | --encode: encode type, Default:h264, Value:h264, h265\n");
	printf("\t-b | --bitrate: set bitrate (Kbps), Default: 10Mbps\n");
	printf("\t-F: AIQ fixed FPS request (1..30); omitted keeps existing policy. Not a measured FPS.\n");
	printf("\t-V: VI-only FPS measurement; no encoder or RTSP server.\n");
	printf("\t-B: VI buffer count (2..4), default 2; VI depth unchanged.\n");
}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IOLBF, 0);
	RK_S32 s32Ret = RK_FAILURE;
	RK_U32 u32Width = 1920;
	RK_U32 u32Height = 1080;
	// RK_CHAR *pOutPath = NULL;
	RK_CODEC_ID_E enCodecType = RK_VIDEO_ID_AVC;
	RK_CHAR *pCodecName = "H264";
	RK_S32 s32chnlId = 0;
	char *iq_dir = "/etc/iqfiles";
	int c;
	int ret = -1;
	bool sys_ready = false, vi_ready = false, dev_ready = false;
	bool venc_ready = false, bound = false;
	MPP_CHN_S stSrcChn = {0}, stDestChn = {0};

	while ((c = getopt(argc, argv, optstr)) != -1) {
		switch (c) {
		case 'B': {
			char *end = NULL;
			long count = strtol(optarg, &end, 10);
			if (!optarg[0] || *end || count < 2 || count > 4) {
				fprintf(stderr, "-B requires an integer in 2..4\n");
				return 2;
			}
			g_vi_buffers = (unsigned)count;
			break;
		}
		case 'V':
			g_vi_only = true;
			break;
		case 'a':
			if (optarg)
				iq_dir = optarg;
			break;
		case 'w':
			u32Width = atoi(optarg);
			break;
		case 'h':
			u32Height = atoi(optarg);
			break;
		case 'I':
			s32chnlId = atoi(optarg);
			break;
		case 'c':
			g_s32FrameCnt = atoi(optarg);
			break;
		case 'e':
			if (!strcmp(optarg, "h264")) {
				enCodecType = RK_VIDEO_ID_AVC;
				pCodecName = "H264";
			} else if (!strcmp(optarg, "h265")) {
				enCodecType = RK_VIDEO_ID_HEVC;
				pCodecName = "H265";
			} else {
				printf("ERROR: Invalid encoder type.\n");
				return -1;
			}
			break;
		case 'b':
			g_u32Bitrate = atoi(optarg);
			break;
		case 'F': {
			char *end = NULL;
			long fps = strtol(optarg, &end, 10);
			if (!optarg[0] || *end || fps < 1 || fps > 30) {
				fprintf(stderr, "-F requires an integer in 1..30\n");
				return 2;
			}
			g_isp_fps = (int)fps;
			break;
		}
		case '?':
		default:
			print_usage(argv[0]);
			return -1;
		}
	}

	printf("#Mode:%s\n", g_vi_only ? "VI_ONLY" : "VI_VENC_RTSP");
	if (!g_vi_only) printf("#CodecName:%s\n", pCodecName);
	printf("#Resolution: %dx%d\n", u32Width, u32Height);
	printf("#CameraIdx: %d\n\n", s32chnlId);
	printf("#Frame Count to save: %d\n", g_s32FrameCnt);

	signal(SIGINT, sigterm_handler);
	signal(SIGTERM, sigterm_handler);

	if (iq_dir) {
#ifdef RKAIQ
		printf("ISP IQ file path: %s\n\n", iq_dir);
		if (SIMPLE_COMM_ISP_Init(0, RK_AIQ_WORKING_MODE_NORMAL, 0, iq_dir) != 0)
			return 1;
		if (SIMPLE_COMM_ISP_Run(0) != 0) {
			SIMPLE_COMM_ISP_Stop(0);
			return 1;
		}
		if (g_isp_fps) {
			/* Same AE frame-rate controls used by SDK isp3.x sample_comm_isp.c. */
			Uapi_ExpSwAttrV2_t attr;
			memset(&attr, 0, sizeof(attr));
			int fps_ret = rk_aiq_user_api2_ae_getExpSwAttr(g_aiq_ctx[0], &attr);
			if (fps_ret == 0) {
				attr.stAuto.stFrmRate.isFpsFix = true;
				attr.stAuto.stFrmRate.FpsValue = g_isp_fps;
				fps_ret = rk_aiq_user_api2_ae_setExpSwAttr(g_aiq_ctx[0], attr);
			}
			if (fps_ret != 0) {
				fprintf(stderr, "AIQ FPS request failed: %d\n", fps_ret);
				SIMPLE_COMM_ISP_Stop(0);
				return 1;
			}
			printf("AIQ fixed FPS request accepted: %d (verify MEASURED logs; not a guarantee)\n", g_isp_fps);
			fps_readback("before_vi");
		}
#endif
	}

	// init rtsp
	if (!g_vi_only) {
	g_rtsplive = create_rtsp_demo(554);
	if (!g_rtsplive) goto __FAILED;
	g_rtsp_session = rtsp_new_session(g_rtsplive, "/live/0");
	if (!g_rtsp_session) goto __FAILED;
	if (enCodecType == RK_VIDEO_ID_AVC) {
		rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
	} else if (enCodecType == RK_VIDEO_ID_HEVC) {
		rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
	} else {
		printf("not support other type\n");
		return -1;
	}
	rtsp_sync_video_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());
	}

	if (RK_MPI_SYS_Init() != RK_SUCCESS) {
		RK_LOGE("rk mpi sys init fail!");
		goto __FAILED;
	}

	sys_ready = true;
	if (vi_dev_init() != RK_SUCCESS) goto __FAILED;
	dev_ready = true;
	if (vi_chn_init(s32chnlId, u32Width, u32Height) != RK_SUCCESS) goto __FAILED;
	vi_ready = true;

	// venc  init
	if (!g_vi_only) {
	if (test_venc_init(0, u32Width, u32Height, enCodecType) != RK_SUCCESS)
		goto __FAILED;
	venc_ready = true;

	// bind vi to venc
	stSrcChn.enModId = RK_ID_VI;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = s32chnlId;

	stDestChn.enModId = RK_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = 0;
	printf("====RK_MPI_SYS_Bind vi0 to venc0====\n");
	s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
	if (s32Ret != RK_SUCCESS) {
		RK_LOGE("bind 0 ch venc failed");
		goto __FAILED;
	}
	bound = true;
	}

	pthread_t main_thread;
	if (pthread_create(&main_thread, NULL,
	                   g_vi_only ? GetViFrames : GetMediaBuffer0, &s32chnlId) != 0)
		goto __FAILED;

#ifdef RKAIQ
	/* Wait for callbacks, then compare startup settings with a running stream.
	 * Keep this in the main thread: the streaming thread continues draining VENC. */
	RK_U64 first_sof_us = 0;
	RK_U64 last_fps_diag_us = 0;
	bool fps_reapplied = false;
	printf("FPS_DIAG revision=6 live_reapply_enabled=%d\n", !!(iq_dir && g_isp_fps));
#endif
	while (!quit) {
		usleep(500000);
#ifdef RKAIQ
		if (!quit && iq_dir) {
			RK_U64 now = TEST_COMM_GetNowUs();
			if (!first_sof_us && atomic_load(&g_sof_cnt) > 0)
				first_sof_us = now;
			if (!fps_reapplied && first_sof_us && now - first_sof_us >= 2000000) {
				fps_readback("running_initial");
				if (g_isp_fps) {
					fps_reapply();
					fps_readback("running_after_reapply");
				}
				fps_reapplied = true;
				last_fps_diag_us = TEST_COMM_GetNowUs();
			} else if (fps_reapplied && now - last_fps_diag_us >= 5000000) {
				fps_readback("running_delayed");
				last_fps_diag_us = now;
			}
		}
#endif
	}
	pthread_join(main_thread, NULL);
	ret = atomic_load(&g_worker_error) ? 1 : 0;

__FAILED:
	if (g_rtsplive)
		rtsp_del_demo(g_rtsplive);

	if (bound) {
	s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
	if (s32Ret != RK_SUCCESS) {
		RK_LOGE("RK_MPI_SYS_UnBind fail %x", s32Ret);
	}
	}

	if (vi_ready) {
	s32Ret = RK_MPI_VI_DisableChn(0, s32chnlId);
	RK_LOGE("RK_MPI_VI_DisableChn %x", s32Ret);
	}

	if (venc_ready) {
	s32Ret = RK_MPI_VENC_StopRecvFrame(0);
	if (s32Ret != RK_SUCCESS) {
		ret = 1;
	}

	s32Ret = RK_MPI_VENC_DestroyChn(0);
	if (s32Ret != RK_SUCCESS) {
		RK_LOGE("RK_MPI_VDEC_DestroyChn fail %x", s32Ret);
	}
	}

	if (dev_ready) {
	s32Ret = RK_MPI_VI_DisableDev(0);
	RK_LOGE("RK_MPI_VI_DisableDev %x", s32Ret);
	}
	RK_LOGE("test running exit:%d", s32Ret);
	if (sys_ready) RK_MPI_SYS_Exit();

#ifdef RKAIQ
	SIMPLE_COMM_ISP_Stop(0);
#endif

	return ret;
}
