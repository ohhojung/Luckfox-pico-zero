#include "npu_model.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_mb.h"
#include "rk_aiq_user_api2_sysctl.h"
#include "rtsp_demo.h"
#include <opencv2/imgproc.hpp>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

namespace {
constexpr int video_w = 1920, video_h = 1080;
constexpr int detect_w = 640, detect_h = 360;
volatile std::sig_atomic_t interrupted = 0;
std::atomic<bool> stopped(false);
void signal_handler(int) { interrupted = 1; }
uint64_t now_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
void check(int code, const char *operation) {
    if (code != 0) throw std::runtime_error(std::string(operation) + " failed: " + std::to_string(code));
}
struct Options {
    std::string model;
    std::string iq = "/etc/iqfiles";
    int bitrate = 4096, ai_channel = 1, detect_fps = 5;
    float confidence = .5f;
    bool camera_only = false;
    bool ai_only = false;
};
void usage(const char *name) {
    std::printf("Usage: %s [-m model.rknn] [-a /etc/iqfiles] [-b 4096] [-f 5] [-t 0.5] [-I 1] [-D|-S]\n"
                "1080p H264 RTSP /live/0 + independent NPU person detection logs.\n"
                "-f: max AI sampling rate (1..30), not video FPS\n"
                "-I: secondary VI channel 1 or 2; main video always uses channel 0\n"
                "-D: diagnostic camera + NPU, drain main VI without VENC/RTSP\n"
                "-S: diagnostic AI VI channel only; no main VI, VENC or RTSP\n", name);
}
double number(const char *value, double low, double high, bool integer) {
    char *end = nullptr;
    double parsed = std::strtod(value, &end);
    if (!value[0] || *end || !(parsed >= low && parsed <= high) ||
        (integer && parsed != int(parsed))) throw std::runtime_error("Invalid numeric option");
    return parsed;
}
std::string default_model() {
    char path[4096];
    ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n <= 0 || size_t(n) >= sizeof(path) - 1) throw std::runtime_error("Cannot locate executable");
    path[n] = 0;
    std::string executable(path);
    return executable.substr(0, executable.find_last_of('/')) + "/model/yolov5.rknn";
}

class Camera {
public:
    rk_aiq_sys_ctx_t *isp = nullptr;
    bool system = false, device = false, main_channel = false, ai_channel = false;
    bool encoder = false, receiving = false, bound = false, isp_started = false;
    int secondary = 1;
    rtsp_demo_handle rtsp = nullptr;
    rtsp_session_handle session = nullptr;
    MPP_CHN_S source = {}, destination = {};
    ~Camera() {
        // Threads must be joined before any media resource is released.
        if (rtsp) rtsp_del_demo(rtsp);
        if (bound) RK_MPI_SYS_UnBind(&source, &destination);
        if (ai_channel) RK_MPI_VI_DisableChn(0, secondary);
        if (main_channel) RK_MPI_VI_DisableChn(0, 0);
        if (receiving) RK_MPI_VENC_StopRecvFrame(0);
        if (encoder) RK_MPI_VENC_DestroyChn(0);
        if (device) RK_MPI_VI_DisableDev(0);
        if (isp_started) rk_aiq_uapi2_sysctl_stop(isp, false);
        if (isp) rk_aiq_uapi2_sysctl_deinit(isp);
        if (system) RK_MPI_SYS_Exit();
    }
    void channel(int id, int width, int height, int depth) {
        VI_CHN_ATTR_S a = {};
        a.stIspOpt.u32BufCount = 3;
        a.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
        a.stSize.u32Width = width;
        a.stSize.u32Height = height;
        a.enPixelFormat = RK_FMT_YUV420SP;
        a.enCompressMode = COMPRESS_MODE_NONE;
        a.u32Depth = depth;
        a.stFrameRate.s32SrcFrameRate = -1;
        a.stFrameRate.s32DstFrameRate = -1;
        check(RK_MPI_VI_SetChnAttr(0, id, &a), "VI SetChnAttr");
        check(RK_MPI_VI_EnableChn(0, id), "VI EnableChn");
    }
    void start(const Options &options) {
        secondary = options.ai_channel;
        // Refuse to compete with rkipc / another camera application. Never kill it.
        check(RK_MPI_SYS_Init(), "SYS Init");
        system = true;
        if (RK_MPI_VI_GetDevIsEnable(0) == RK_SUCCESS)
            throw std::runtime_error("Camera already active. Stop rkipc/simple_vi_bind_venc_rtsp first");
        if (!options.camera_only) {
            rtsp = create_rtsp_demo(554);
            if (!rtsp) throw std::runtime_error("RTSP port 554 unavailable");
            session = rtsp_new_session(rtsp, "/live/0");
            if (!session) throw std::runtime_error("RTSP session creation failed");
            rtsp_set_video(session, RTSP_CODEC_ID_VIDEO_H264, nullptr, 0);
            rtsp_sync_video_ts(session, rtsp_get_reltime(), rtsp_get_ntptime());
        }

        // Match the known-good simple_vi_bind_venc_rtsp normal-mode initialization.
        check(setenv("HDR_MODE", "0", 1), "set HDR_MODE");
        rk_aiq_static_info_t sensor = {};
        check(rk_aiq_uapi2_sysctl_enumStaticMetas(0, &sensor), "AIQ sensor enumeration");
        std::printf("Sensor: %s; IQ: %s\n", sensor.sensor_info.sensor_name, options.iq.c_str());
        isp = rk_aiq_uapi2_sysctl_init(sensor.sensor_info.sensor_name, options.iq.c_str(), nullptr, nullptr);
        if (!isp) throw std::runtime_error("AIQ Init failed");
        check(rk_aiq_uapi2_sysctl_prepare(isp, 0, 0, RK_AIQ_WORKING_MODE_NORMAL), "AIQ Prepare");
        check(rk_aiq_uapi2_sysctl_start(isp), "AIQ Start");
        isp_started = true;

        VI_DEV_ATTR_S dev = {};
        int ret = RK_MPI_VI_GetDevAttr(0, &dev);
        if (ret == RK_ERR_VI_NOT_CONFIG) check(RK_MPI_VI_SetDevAttr(0, &dev), "VI SetDevAttr");
        else check(ret, "VI GetDevAttr");
        check(RK_MPI_VI_EnableDev(0), "VI EnableDev");
        device = true;
        VI_DEV_BIND_PIPE_S pipe = {};
        pipe.u32Num = 1;
        pipe.PipeId[0] = 0;
        check(RK_MPI_VI_SetDevBindPipe(0, &pipe), "VI BindPipe");
        // Diagnostic mode reads/releases main frames instead of binding VENC.
        if (!options.ai_only) {
            channel(0, video_w, video_h, options.camera_only ? 1 : 0);
            main_channel = true;
        }
        channel(secondary, detect_w, detect_h, 1);
        ai_channel = true;

        if (options.camera_only) return;

        VENC_CHN_ATTR_S enc = {};
        enc.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        enc.stRcAttr.stH264Cbr.u32BitRate = options.bitrate;
        enc.stRcAttr.stH264Cbr.u32Gop = 30;
        enc.stVencAttr.enType = RK_VIDEO_ID_AVC;
        enc.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;
        enc.stVencAttr.u32Profile = H264E_PROFILE_HIGH;
        enc.stVencAttr.u32PicWidth = enc.stVencAttr.u32VirWidth = video_w;
        enc.stVencAttr.u32PicHeight = enc.stVencAttr.u32VirHeight = video_h;
        enc.stVencAttr.u32StreamBufCnt = 3;
        enc.stVencAttr.u32BufSize = video_w * video_h * 3 / 2;
        check(RK_MPI_VENC_CreateChn(0, &enc), "VENC CreateChn");
        encoder = true;
        VENC_RECV_PIC_PARAM_S recv = {};
        recv.s32RecvPicNum = -1;
        check(RK_MPI_VENC_StartRecvFrame(0, &recv), "VENC StartRecvFrame");
        receiving = true;
        source.enModId = RK_ID_VI;
        source.s32DevId = source.s32ChnId = 0;
        destination.enModId = RK_ID_VENC;
        destination.s32DevId = destination.s32ChnId = 0;
        check(RK_MPI_SYS_Bind(&source, &destination), "VI -> VENC Bind");
        bound = true;
    }
};

struct PendingFrame {
    std::mutex mutex;
    std::condition_variable changed;
    cv::Mat rgb;
    uint64_t pts = 0, captured = 0;
};
struct FrameGuard {
    int channel;
    VIDEO_FRAME_INFO_S frame = {};
    bool acquired = false;
    explicit FrameGuard(int id) : channel(id) {}
    ~FrameGuard() { if (acquired) RK_MPI_VI_ReleaseChnFrame(0, channel, &frame); }
};
cv::Mat copy_rgb(const VIDEO_FRAME_INFO_S &frame) {
    const auto &v = frame.stVFrame;
    if (v.enPixelFormat != RK_FMT_YUV420SP || v.enCompressMode != COMPRESS_MODE_NONE ||
        v.u32Width != detect_w || v.u32Height != detect_h ||
        v.u32VirWidth < v.u32Width || v.u32VirHeight < v.u32Height)
        throw std::runtime_error("Unexpected AI channel format or stride");
    auto data = static_cast<unsigned char *>(RK_MPI_MB_Handle2VirAddr(v.pMbBlk));
    if (!data) throw std::runtime_error("VI buffer mapping failed");
    cv::Mat nv12(detect_h * 3 / 2, detect_w, CV_8UC1);
    for (int y = 0; y < detect_h; ++y)
        std::memcpy(nv12.ptr(y), data + size_t(y) * v.u32VirWidth, detect_w);
    auto uv = data + size_t(v.u32VirWidth) * v.u32VirHeight;
    for (int y = 0; y < detect_h / 2; ++y)
        std::memcpy(nv12.ptr(detect_h + y), uv + size_t(y) * v.u32VirWidth, detect_w);
    cv::Mat rgb;
    // RK_FMT_YUV420SP is NV12 (UV), not NV21 (VU). Model input is RGB, not BGR.
    cv::cvtColor(nv12, rgb, cv::COLOR_YUV2RGB_NV12);
    return rgb;
}
void run_camera_drain() {
    try {
        uint64_t last_frame = now_us(), report = last_frame;
        unsigned frames = 0;
        while (!stopped.load()) {
            FrameGuard frame(0);
            if (RK_MPI_VI_GetChnFrame(0, 0, &frame.frame, 100) == RK_SUCCESS) {
                frame.acquired = true;
                last_frame = now_us();
                ++frames;
            } else if (now_us() - last_frame > 5000000) {
                throw std::runtime_error("No main VI frame for 5 seconds");
            }
            uint64_t now = now_us();
            if (now - report >= 5000000) {
                std::printf("CAMERA main_fps=%.1f (diagnostic; VENC/RTSP disabled)\n",
                            frames * 1000000.0 / (now - report));
                report = now;
                frames = 0;
            }
        }
    } catch (const std::exception &error) {
        std::fprintf(stderr, "CAMERA DRAIN ERROR: %s\n", error.what());
        stopped.store(true);
    }
}
void run_stream(Camera &camera) {
    try {
        VENC_PACK_S pack = {};
        VENC_STREAM_S stream = {};
        stream.pstPack = &pack;
        uint64_t last_frame = now_us(), report = last_frame;
        unsigned frames = 0;
        while (!stopped.load()) {
            int ret = RK_MPI_VENC_GetStream(0, &stream, 100);
            if (ret == RK_SUCCESS) {
                auto data = static_cast<uint8_t *>(RK_MPI_MB_Handle2VirAddr(pack.pMbBlk));
                if (!data) {
                    RK_MPI_VENC_ReleaseStream(0, &stream);
                    throw std::runtime_error("VENC buffer mapping failed");
                }
                rtsp_tx_video(camera.session, data, pack.u32Len, pack.u64PTS);
                check(RK_MPI_VENC_ReleaseStream(0, &stream), "VENC ReleaseStream");
                last_frame = now_us();
                ++frames;
            } else if (now_us() - last_frame > 5000000) {
                throw std::runtime_error("No encoded frame for 5 seconds");
            }
            rtsp_do_event(camera.rtsp);
            uint64_t now = now_us();
            if (now - report >= 5000000) {
                std::printf("VIDEO fps=%.1f (encoder output; not end-to-end latency)\n",
                            frames * 1000000.0 / (now - report));
                report = now;
                frames = 0;
            }
        }
    } catch (const std::exception &error) {
        std::fprintf(stderr, "STREAM ERROR: %s\n", error.what());
        stopped.store(true);
    }
}
void run_inference(NpuModel &model, PendingFrame &pending, float confidence) {
    try {
        uint64_t report = 0;
        int last_count = -1;
        while (!stopped.load()) {
            cv::Mat rgb;
            uint64_t pts, captured;
            {
                std::unique_lock<std::mutex> lock(pending.mutex);
                pending.changed.wait_for(lock, std::chrono::milliseconds(100), [&] {
                    return stopped.load() || !pending.rgb.empty();
                });
                if (stopped.load()) break;
                if (pending.rgb.empty()) continue;
                rgb = pending.rgb;
                pending.rgb.release();
                pts = pending.pts;
                captured = pending.captured;
            }
            uint64_t begin = now_us();
            auto boxes = model.infer(rgb, confidence);
            uint64_t end = now_us();
            if (int(boxes.size()) != last_count || end - report >= 1000000) {
                std::printf("PERSON count=%zu pts_us=%llu detect_ms=%.1f result_age_ms=%.1f\n",
                            boxes.size(), static_cast<unsigned long long>(pts),
                            (end - begin) / 1000.0, (end - captured) / 1000.0);
                for (const auto &box : boxes)
                    std::printf("  person score=%.3f box1080=(%d,%d,%d,%d)\n", box.score,
                                int(box.left * video_w / detect_w), int(box.top * video_h / detect_h),
                                int(box.right * video_w / detect_w), int(box.bottom * video_h / detect_h));
                report = end;
                last_count = boxes.size();
            }
        }
    } catch (const std::exception &error) {
        std::fprintf(stderr, "NPU ERROR: %s\n", error.what());
        stopped.store(true);
    }
}
} // namespace

int main(int argc, char **argv) {
    setvbuf(stdout, nullptr, _IOLBF, 0);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGPIPE, SIG_IGN);
    Options options;
    try {
        int c;
        while ((c = getopt(argc, argv, "m:a:b:f:t:I:hDS")) != -1) {
            switch (c) {
            case 'm': options.model = optarg; break;
            case 'a': options.iq = optarg; break;
            case 'b': options.bitrate = number(optarg, 128, 20000, true); break;
            case 'f': options.detect_fps = number(optarg, 1, 30, true); break;
            case 't': options.confidence = number(optarg, .01, .99, false); break;
            case 'I': options.ai_channel = number(optarg, 1, 2, true); break;
            case 'D': options.camera_only = true; break;
            case 'S': options.ai_only = true; options.camera_only = true; break;
            case 'h': usage(argv[0]); return 0;
            default: usage(argv[0]); return 2;
            }
        }
        if (optind != argc) throw std::runtime_error("Unexpected positional argument");
        if (options.model.empty()) options.model = default_model();
        cv::setNumThreads(1);
        NpuModel model;
        model.open(options.model); // Fail before opening the camera if the model is incompatible.
        Camera camera;
        camera.start(options);
        PendingFrame pending;
        std::thread streaming, inference;
        int result = 0;
        try {
            if (options.camera_only && !options.ai_only)
                streaming = std::thread(run_camera_drain);
            else if (!options.camera_only)
                streaming = std::thread(run_stream, std::ref(camera));
            inference = std::thread(run_inference, std::ref(model), std::ref(pending), options.confidence);
            if (options.ai_only) {
                std::printf("READY DIAGNOSTIC SINGLE: AI VI channel %d, %dx%d + NPU; main VI/VENC/RTSP disabled; AI <= %d fps\n",
                            options.ai_channel, detect_w, detect_h, options.detect_fps);
            } else if (options.camera_only) {
                std::printf("READY DIAGNOSTIC: camera + NPU; two VI channels; VENC/RTSP disabled; AI <= %d fps\n",
                            options.detect_fps);
            } else {
                std::printf("READY: rtsp://BOARD_WIFI_IP:554/live/0; 1080p H264 %d Kbps; AI <= %d fps\n"
                            "Person results are terminal logs; this version does not draw boxes on video.\n",
                            options.bitrate, options.detect_fps);
            }
            uint64_t next_sample = 0, last_frame = now_us();
            while (!interrupted && !stopped.load()) {
                FrameGuard frame(options.ai_channel);
                int ret = RK_MPI_VI_GetChnFrame(0, options.ai_channel, &frame.frame, 100);
                if (ret != RK_SUCCESS) {
                    if (now_us() - last_frame > 5000000)
                        throw std::runtime_error("No AI-channel frame for 5 seconds; check secondary VI support");
                    continue;
                }
                frame.acquired = true;
                uint64_t captured = now_us();
                last_frame = captured;
                // Drain VI continuously; only copy/convert selected frames. Pending queue has one slot.
                if (captured < next_sample) continue;
                next_sample = captured + 1000000 / options.detect_fps;
                cv::Mat rgb = copy_rgb(frame.frame);
                {
                    std::lock_guard<std::mutex> lock(pending.mutex);
                    pending.rgb = rgb;
                    pending.pts = frame.frame.stVFrame.u64PTS;
                    pending.captured = captured;
                }
                pending.changed.notify_one();
            }
            if (!interrupted && stopped.load()) result = 1;
        } catch (const std::exception &error) {
            std::fprintf(stderr, "CAPTURE ERROR: %s\n", error.what());
            result = 1;
        }
        stopped.store(true);
        pending.changed.notify_all();
        if (streaming.joinable()) streaming.join();
        if (inference.joinable()) inference.join();
        return result;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "STARTUP ERROR: %s\n", error.what());
        return 1;
    }
}
