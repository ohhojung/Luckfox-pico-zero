// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] 카메라/VI/VENC/RTSP API 흐름을 참고해 재구성. 공식 YOLO 예제의 단일 루프를 복사한 구현이 아니다.
// [출처] 우리 추가: 두 VI, 캡처/추론/송출 분리, 최신 프레임1칸, AI오류 격리, -D/-S, 예외/RAII 정리. 센서 설정까지 동일하지 않다.
// [출처] YOLO 예제의 main.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/main.cc :: main / letterbox / mapCoordinates
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
// [출처] YOLO 예제의 luckfox_mpi.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/luckfox_mpi.cc :: vi_dev_init / vi_chn_init / venc_init / TEST_COMM_GetNowUs
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/luckfox_mpi.cc
// [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c: 작업공간/simple_test/simple_vi_bind_venc_rtsp.c :: SIMPLE_COMM_ISP_Init / SIMPLE_COMM_ISP_Run / GetMediaBuffer0 / test_venc_init / main
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
// 실시간 CCTV 진입점. 기본 모드는 영상 송출과 사람 추론을 분리한다.
// -S/-D는 문제 분리를 위한 진단 모드이며 실제 센서 FPS를 설정하지 않는다.
#include "npu_model.h"
#include "latest_frame.h"
#ifdef CCTV_IMAGE_IO_HEADER
#include CCTV_IMAGE_IO_HEADER
#endif
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_mb.h"
#include "rk_aiq_user_api2_sysctl.h"
#include "rtsp_demo.h"
#include <opencv2/imgproc.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

namespace {
// 영상은 1080p, AI 채널은 640×360으로 고정. 모델 입력은 별도로 640×640이다.
constexpr int video_w = 1920, video_h = 1080;
constexpr int detect_w = 640, detect_h = 360;
// 시그널 핸들러는 플래그만 변경한다. 스레드 간 종료 통지는 atomic stopped가 담당한다.
volatile std::sig_atomic_t interrupted = 0;
std::atomic<bool> stopped(false);
// AI 오류는 영상 종료와 분리한다. 영상/카메라 공통 오류는 stopped로 종료한다.
std::atomic<bool> ai_failed(false);
// Ctrl+C 또는 종료 요청을 주 루프에 전달한다. 여기서 장치 API는 호출하지 않는다.
void signal_handler(int) { interrupted = 1; }
// 시간 간격 측정용 단조 시계. 날짜나 카메라 PTS와 같은 기준이라고 가정하지 않는다.
// [출처] YOLO 예제의 luckfox_mpi.cc TEST_COMM_GetNowUs와 같은 단조 시계 간격 측정 목적. 구현은 std::chrono로 작성했다.
uint64_t now_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
// MPI/AIQ 실패를 예외로 바꾸어 공통 정리 경로로 전달한다.
void check(int code, const char *operation) {
    if (code != 0) throw std::runtime_error(std::string(operation) + " failed: " + std::to_string(code));
}
// 실행 옵션. detect_fps는 추론 입력 선택 상한이며 송출 FPS와 독립적이다.
struct Options {
    std::string model;
    std::string fixed_image;
    std::string iq = "/etc/iqfiles";
    int bitrate = 4096, ai_channel = 1, detect_fps = 5;
    float confidence = .5f;
    bool camera_only = false;
    bool ai_only = false;
    bool no_camera = false;
    bool main_inference = false;
};
// 사용 가능한 옵션과 진단 모드의 범위를 출력한다.
void usage(const char *name) {
    std::printf("Usage: %s [-m model.rknn] [-a /etc/iqfiles] [-b 4096] [-f 5] [-t 0.5] [-I 1] [-P image.jpg] [-D|-S|-N] [-M]\n"
                "1080p H265 RTSP /live/0 + independent NPU person detection logs.\n"
                "-P image.jpg: fixed BGR input; camera behavior follows the selected mode\n"
                "-N: requires -P; no camera/AIQ/VI/VENC/RTSP initialization; same NPU worker/slot\n"
                "-M: requires -N -P; repeat inference on main thread, no inference worker\n"
                "-f: max AI sampling rate (1..30), not video FPS\n"
                "-I: secondary VI channel 1 or 2; main video always uses channel 0\n"
                "-D: diagnostic camera + NPU, drain main VI without VENC/RTSP\n"
                "-S: diagnostic AI VI channel only; no main VI, VENC or RTSP\n", name);
}
// 빈 문자열, 범위 초과, 잘못된 문자 및 정수 옵션의 소수 입력을 거부한다.
double number(const char *value, double low, double high, bool integer) {
    char *end = nullptr;
    double parsed = std::strtod(value, &end);
    if (!value[0] || *end || !(parsed >= low && parsed <= high) ||
        (integer && parsed != int(parsed))) throw std::runtime_error("Invalid numeric option");
    return parsed;
}
// 현재 작업 폴더가 아닌 실행 파일 위치를 기준으로 모델을 찾는다.
std::string default_model() {
    char path[4096];
    ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n <= 0 || size_t(n) >= sizeof(path) - 1) throw std::runtime_error("Cannot locate executable");
    path[n] = 0;
    std::string executable(path);
    return executable.substr(0, executable.find_last_of('/')) + "/model/yolov5.rknn";
}

// 카메라·인코더·RTSP 자원의 소유자. 성공한 초기화 단계만 표시해 부분 실패도 정리한다.
// [출처] YOLO 예제의 luckfox_mpi.cc의 분리된 vi/venc 함수와 RTSP 예제의 simple_vi_bind_venc_rtsp.c ISP 함수 역할을 통합한 프로젝트 클래스. 단계별 소유권 플래그/소멸자는 추가 설계다.
class Camera {
public:
    rk_aiq_sys_ctx_t *isp = nullptr;
    bool system = false, device = false, main_channel = false, ai_channel = false;
    bool encoder = false, receiving = false, bound = false, isp_started = false;
    int secondary = 1;
    rtsp_demo_handle rtsp = nullptr;
    rtsp_session_handle session = nullptr;
    MPP_CHN_S source = {}, destination = {};
    // 호출 전에 작업 스레드가 종료되어야 한다. 정리 API 반환값은 현재 기록하지 않는 한계가 있다.
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
    // VI 채널에 NV12/DMABUF 버퍼 3개를 설정한다. depth는 앱이 가져갈 수 있는 프레임 깊이다.
    // [출처] YOLO 예제의 luckfox_mpi.cc vi_chn_init과 SetChnAttr→EnableChn 순서·DMABUF/NV12 설정이 대응한다.
    // 원본 버퍼2/depth2 대신 버퍼3/가변depth와 FPS -1을 사용한다.
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
    // AIQ→SYS→RTSP(일반 모드)→VI→VENC 순서. 기존 카메라 앱을 자동으로 종료하지 않는다.
    // [출처] YOLO 예제의 main.cc는 RkLunch-stop.sh를 호출하지만 우리는 자동 종료하지 않는다. 기존 앱 사용 여부 검사와 예외는 추가한 정책이다.
    void start(const Options &options) {
        secondary = options.ai_channel;
        // ISP 내부 호출은 예제의 NORMAL 모드 init/prepare/start를 따른다.
        // 정상 노출 모드로 AIQ를 시작한다. 센서 원본 해상도나 FPS를 여기서 지정하지 않는다.
        // [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c SIMPLE_COMM_ISP_Init/Run의 HDR_MODE→enumStaticMetas→init→prepare→start 흐름. 우리는 NORMAL 고정, 콜백 nullptr, 단일 카메라로 제한했다.
        check(setenv("HDR_MODE", "0", 1), "set HDR_MODE");
        rk_aiq_static_info_t sensor = {};
        check(rk_aiq_uapi2_sysctl_enumStaticMetas(0, &sensor), "AIQ sensor enumeration");
        std::printf("Sensor: %s; IQ: %s\n", sensor.sensor_info.sensor_name, options.iq.c_str());
        isp = rk_aiq_uapi2_sysctl_init(sensor.sensor_info.sensor_name, options.iq.c_str(), nullptr, nullptr);
        if (!isp) throw std::runtime_error("AIQ Init failed");
        check(rk_aiq_uapi2_sysctl_prepare(isp, 0, 0, RK_AIQ_WORKING_MODE_NORMAL), "AIQ Prepare");
        check(rk_aiq_uapi2_sysctl_start(isp), "AIQ Start");
        isp_started = true;
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
            // 인코더의 HEVC 설정과 RTSP 세션의 코덱 식별자를 함께 H.265로 맞춘다.
            rtsp_set_video(session, RTSP_CODEC_ID_VIDEO_H265, nullptr, 0);
            rtsp_sync_video_ts(session, rtsp_get_reltime(), rtsp_get_ntptime());
        }


        // [출처] YOLO 예제의 luckfox_mpi.cc vi_dev_init의 장치 속성 확인→활성화→파이프 연결 절차와 대응. 실패는 반환값 대신 예외로 전달한다.
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
        // 기본 모드와 -D는 주 VI도 활성화한다. -S는 AI용 보조 VI만 사용한다.
        if (!options.ai_only) {
            channel(0, video_w, video_h, options.camera_only ? 1 : 0);
            main_channel = true;
        }
        channel(secondary, detect_w, detect_h, 1);
        ai_channel = true;

        if (options.camera_only) return;

        // 인코더에는 주 VI의 NV12를 직접 연결한다. AI 박스가 그려진 BGR 영상을 보내는 방식이 아니다.
        // [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c test_venc_init의 HEVC 분기(H265CBR/stH265Cbr)와 대응. YOLO 예제의 luckfox_mpi.cc venc_init은 RGB888 입력이므로 동일하지 않다.
        // 비트레이트/GOP/버퍼는 이 프로젝트 설정이다.
        VENC_CHN_ATTR_S enc = {};
        enc.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        enc.stRcAttr.stH265Cbr.u32BitRate = options.bitrate;
        enc.stRcAttr.stH265Cbr.u32Gop = 60;
        enc.stVencAttr.enType = RK_VIDEO_ID_HEVC;
        enc.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;
        // [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c는 HEVC에 H.264 High profile을 적용하지 않는다. 여기서도 영 초기화 값을 유지한다.
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
        // [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c main의 VI→VENC 직접 바인딩 방식. YOLO 예제의 main.cc YOLO main의 RGB888 SendFrame 방식과 다르다.
        check(RK_MPI_SYS_Bind(&source, &destination), "VI -> VENC Bind");
        bound = true;
    }
};

// 대기 공간은 한 프레임이다. 새 프레임이 오면 처리 전의 오래된 프레임을 교체한다.
// [출처] 프로젝트 추가 구조. YOLO 예제의 main.cc의 순차 루프에는 최신 프레임 슬롯·mutex·condition_variable이 없다.
struct InferenceFrame {
    cv::Mat bgr;
    uint64_t pts = 0, captured = 0;
};
using PendingFrame = LatestFrame<InferenceFrame>;
// VI 프레임 소유권 관리. 획득에 성공한 경우 반복문 종료·continue·예외 때 반환한다.
// [출처] 프로젝트 추가 RAII. YOLO 예제의 main.cc main은 반복문 끝에서 VI_ReleaseChnFrame을 직접 호출한다.
struct FrameGuard {
    int channel;
    VIDEO_FRAME_INFO_S frame = {};
    bool acquired = false;
    explicit FrameGuard(int id) : channel(id) {}
    ~FrameGuard() { if (acquired) RK_MPI_VI_ReleaseChnFrame(0, channel, &frame); }
};
// VI 버퍼의 행 간격을 반영해 복사하고 독립 BGR 영상을 만든다. 반환 후 원본 VI 버퍼와 수명이 분리된다.
// [출처] YOLO 예제의 main.cc main의 VI 매핑→OpenCV 색변환과 역할 대응. 우리는 stride를 반영해 복사하고 NV12→BGR을 명시; 원본 COLOR_YUV420sp2BGR 호출과 동일하지 않다.
cv::Mat copy_bgr(const VIDEO_FRAME_INFO_S &frame) {
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
    // UV 시작 위치는 보이는 높이가 아니라 버퍼의 가상 높이 기준으로 계산한다.
    auto uv = data + size_t(v.u32VirWidth) * v.u32VirHeight;
    for (int y = 0; y < detect_h / 2; ++y)
        std::memcpy(nv12.ptr(detect_h + y), uv + size_t(y) * v.u32VirWidth, detect_w);
    cv::Mat bgr;
    // RK_FMT_YUV420SP is NV12 (UV), not NV21 (VU). Model input follows the example BGR preprocessing.
    cv::cvtColor(nv12, bgr, cv::COLOR_YUV2BGR_NV12);
    return bgr;
}
// -D 전용: 주 VI를 계속 회수·반환해 정체를 막고 실제 회수율을 출력한다.
// [출처] 프로젝트 추가 -D 진단 함수. YOLO 예제의 main.cc/RTSP 예제의 simple_vi_bind_venc_rtsp.c에서 이 기능의 직접 대응 함수는 없다.
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
// 송출 스레드: 인코딩된 패킷을 받아 RTSP로 보내고 즉시 반환한다. NPU 완료를 기다리지 않는다.
// [출처] RTSP 예제의 simple_vi_bind_venc_rtsp.c GetMediaBuffer0의 GetStream→매핑→rtsp_tx_video→ReleaseStream 흐름과 대응. 100ms 대기, 무프레임5초 검사, 측정FPS와 예외 처리는 추가했다.
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
                std::printf("VIDEO fps=%.1f ai=%s (encoder output; not end-to-end latency)\n",
                            frames * 1000000.0 / (now - report), ai_failed.load() ? "disabled" : "enabled");
                report = now;
                frames = 0;
            }
        }
    } catch (const std::exception &error) {
        std::fprintf(stderr, "STREAM ERROR: %s\n", error.what());
        stopped.store(true);
    }
}
// [출처] 프로젝트 추가 비교 기능. 주 스레드와 작업 스레드가 같은 1회 처리 함수를 사용한다.
// 모델은 main에서 한 번 초기화하고, 이 객체는 호출하는 스레드 하나만 소유한다.
class InferenceRunner {
    uint64_t report = 0, completed = 0;
    int last_count = -1;
public:
    void step(NpuModel &model, PendingFrame &pending, float confidence, bool fixed_image, bool video_active) {
      try {
        InferenceFrame frame;
        if (!pending.take(frame, stopped) || ai_failed.load()) return;
        const cv::Mat &bgr = frame.bgr;
        uint64_t pts = frame.pts, captured = frame.captured;
        // pending.take가 잠금을 해제한 뒤에 추론한다. 캡처/송출은 완료를 기다리지 않는다.
        // 측정에는 전처리와 후처리도 포함된다. 순수 NPU 연산 시간만을 나타내지는 않는다.
        uint64_t begin = now_us();
        auto boxes = model.infer(bgr, confidence);
        uint64_t end = now_us();
        ++completed;
        if (int(boxes.size()) != last_count || end - report >= 1000000) {
            // result_age_ms는 VI 수신 이후, -N에서는 사진 선택 이후의 경과 시간이다.
            // 센서·ISP 대기나 VLC 재생 지연은 포함하지 않는다. -N의 pts_us=0은 카메라 PTS 없음이다.
            std::printf("PERSON count=%zu pts_us=%llu detect_ms=%.1f result_age_ms=%.1f",
                        boxes.size(), static_cast<unsigned long long>(pts),
                        (end - begin) / 1000.0, (end - captured) / 1000.0);
            std::printf(" source=%s input=%dx%d completed=%llu\n", fixed_image ? "fixed-image" : "camera",
                        bgr.cols, bgr.rows, static_cast<unsigned long long>(completed));
            // -P의 좌표는 사진 픽셀 기준이다. 카메라 PTS는 입력 선택 시점 표시에만 사용한다.
            // 1080p 좌표 환산은 두 VI 채널의 시야와 크롭이 같다는 전제다. 영상에 박스를 그리지는 않는다.
            for (const auto &box : boxes)
                std::printf("  person score=%.3f %s=(%d,%d,%d,%d)\n", box.score,
                            fixed_image ? "box_image" : "box1080",
                            int(box.left * (fixed_image ? 1.0f : float(video_w) / detect_w)),
                            int(box.top * (fixed_image ? 1.0f : float(video_h) / detect_h)),
                            int(box.right * (fixed_image ? 1.0f : float(video_w) / detect_w)),
                            int(box.bottom * (fixed_image ? 1.0f : float(video_h) / detect_h)));
            report = end;
            last_count = boxes.size();
        }
      } catch (const std::exception &error) {
        ai_failed.store(true);
        pending.clear();
        std::fprintf(stderr, "NPU DISABLED: %s; %s (no automatic retry)\n", error.what(),
                     video_active ? "video loop remains independent" : "diagnostic run will exit; no video stream");
        std::fprintf(stderr, "INFERENCE_STOP completed=%llu\n", static_cast<unsigned long long>(completed));
      }
    }
};
// 같은 RKNN 컨텍스트에 동시 추론하지 않는다. -M이면 이 작업 스레드를 만들지 않는다.
// [출처] 공식 YOLO 예제의 main.cc는 주 스레드에서 추론한다. 아래 작업 스레드는 프로젝트 구현이다.
void run_inference(NpuModel &model, PendingFrame &pending, float confidence, bool fixed_image, bool video_active) {
    InferenceRunner runner;
    while (!stopped.load() && !ai_failed.load())
        runner.step(model, pending, confidence, fixed_image, video_active);
}
} // namespace

// 옵션 검증→모델 로드→카메라 시작→작업 스레드→AI 캡처→스레드 합류→자원 정리 순서.
// [출처] YOLO 예제의 main.cc의 단일 루프와 다른 프로젝트 통합 흐름. 옵션/샘플링/스레드 join/공통 종료 정책은 그대로 가져온 공식 코드가 아니다.
int main(int argc, char **argv) {
    setvbuf(stdout, nullptr, _IOLBF, 0);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGPIPE, SIG_IGN);
    Options options;
    try {
        int c;
        while ((c = getopt(argc, argv, "m:a:b:f:t:I:P:hDSNM")) != -1) {
            switch (c) {
            case 'm': options.model = optarg; break;
            case 'P':
                options.fixed_image = optarg;
                if (options.fixed_image.empty()) throw std::runtime_error("Empty fixed image path");
                break;
            case 'a': options.iq = optarg; break;
            case 'b': options.bitrate = number(optarg, 128, 20000, true); break;
            case 'f': options.detect_fps = number(optarg, 1, 30, true); break;
            case 't': options.confidence = number(optarg, .01, .99, false); break;
            case 'I': options.ai_channel = number(optarg, 1, 2, true); break;
            case 'D': options.camera_only = true; break;
            case 'S': options.ai_only = true; options.camera_only = true; break;
            case 'N': options.no_camera = true; break;
            case 'M': options.main_inference = true; break;
            case 'h': usage(argv[0]); return 0;
            default: usage(argv[0]); return 2;
            }
        }
        if (optind != argc) throw std::runtime_error("Unexpected positional argument");
        if (options.main_inference && !options.no_camera)
            throw std::runtime_error("-M requires -N -P image.jpg");
        if (options.no_camera && options.fixed_image.empty())
            throw std::runtime_error("-N requires -P image.jpg");
        if (options.no_camera && options.camera_only)
            throw std::runtime_error("-N cannot be combined with -D or -S");
        if (options.model.empty()) options.model = default_model();
        cv::setNumThreads(1);
        // [출처] 프로젝트 추가 비교 시험. 사진 프로그램과 같은 imread(BGR)를 사용한다.
        // 사진은 시작 전에 한 번 읽고, 각 선택 시 독립 복사본을 전달한다.
        cv::Mat fixed_bgr;
        if (!options.fixed_image.empty()) {
#ifdef CCTV_IMAGE_IO_HEADER
            fixed_bgr = cv::imread(options.fixed_image, 1);
            if (fixed_bgr.empty()) throw std::runtime_error("Cannot read fixed image: " + options.fixed_image);
            std::printf("FIXED_IMAGE: %s %dx%d; %s; NV12 conversion bypassed\n",
                        options.fixed_image.c_str(), fixed_bgr.cols, fixed_bgr.rows,
                        options.no_camera ? "VI not initialized" : "VI acquisition/release retained");
#else
            throw std::runtime_error("Fixed image requires OpenCV image IO; rebuild with BUILD_IMAGE_TEST=ON and working image IO");
#endif
        }
        NpuModel model;
        // 모델 오류를 카메라 시작 전에 확인한다. 다른 앱이 메모리를 점유하면 여기서 할당 실패할 수 있다.
        model.open(options.model); // Fail before opening the camera if the model is incompatible.
        Camera camera;
        // [출처] 프로젝트 추가 -N 대조 시험. 모델 초기화와 작업 스레드는 유지한다.
        // start를 생략하면 AIQ/SYS/VI/VENC/RTSP를 시작하지 않으며 소멸자도 장치 정리를 호출하지 않는다.
        // 다른 프로세스의 카메라는 중지하지 않는다. 시험 전 사용자가 별도로 종료해야 한다.
        if (!options.no_camera) camera.start(options);
        const bool video_active = !options.no_camera && !options.camera_only;
        PendingFrame pending;
        InferenceRunner main_runner;
        std::thread streaming, inference;
        int result = 0;
        try {
            if (!options.no_camera && options.camera_only && !options.ai_only)
                streaming = std::thread(run_camera_drain);
            else if (video_active)
                streaming = std::thread(run_stream, std::ref(camera));
            if (!options.main_inference)
                inference = std::thread(run_inference, std::ref(model), std::ref(pending), options.confidence,
                                    !fixed_bgr.empty(), video_active);
            if (options.no_camera) {
                std::printf("READY NO_CAMERA: fixed image + latest-frame slot; inference_thread=%s; "
                            "camera/AIQ/VI/VENC/RTSP not initialized; AI <= %d fps; pts_us=0 (no camera PTS)\n",
                            options.main_inference ? "main" : "worker", options.detect_fps);
            } else if (options.ai_only) {
                std::printf("READY DIAGNOSTIC SINGLE: AI VI channel %d, %dx%d + NPU; main VI/VENC/RTSP disabled; AI <= %d fps\n",
                            options.ai_channel, detect_w, detect_h, options.detect_fps);
            } else if (options.camera_only) {
                std::printf("READY DIAGNOSTIC: camera + NPU; two VI channels; VENC/RTSP disabled; AI <= %d fps\n",
                            options.detect_fps);
            } else {
                std::printf("READY: rtsp://BOARD_WIFI_IP:554/live/0; 1080p H265 %d Kbps; AI <= %d fps\n"
                            "Person results are terminal logs; this version does not draw boxes on video.\n",
                            options.bitrate, options.detect_fps);
            }
            uint64_t next_sample = 0, last_frame = now_us();
            while (!interrupted && !stopped.load()) {
                if (ai_failed.load() && !video_active) { result = 1; break; }
                if (options.no_camera) {
                    // 카메라 수신 대신 단조 시계로 사진을 선택한다. 같은 슬롯과 추론 스레드를 쓴다.
                    // 밀린 입력을 몰아서 넣지 않고, 처리 전 오래된 사진은 LatestFrame에서 교체한다.
                    uint64_t selected = now_us();
                    if (selected < next_sample) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        continue;
                    }
                    next_sample = selected + 1000000 / options.detect_fps;
                    InferenceFrame sample;
                    sample.bgr = fixed_bgr.clone();
                    sample.captured = selected;
                    // pts는 초기값 0이다. 아래 VI 획득 경로로 진행하지 않는다.
                    pending.publish(std::move(sample));
                    // -M도 같은 슬롯에서 입력을 꺼내 같은 함수를 호출한다. 작업 스레드는 없다.
                    // 동기 추론 중에는 다음 사진을 만들지 못하므로 생산 타이밍 차이는 남는다.
                    if (options.main_inference)
                        main_runner.step(model, pending, options.confidence, true, false);
                    continue;
                }
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
                // 선택하지 않은 프레임도 FrameGuard로 반환한다. 이 제한은 센서/ISP의 처리율을 낮추지 않는다.
                if (ai_failed.load() || captured < next_sample) continue;
                next_sample = captured + 1000000 / options.detect_fps;
                // -P도 보조 VI를 계속 획득/반환하고 같은 슬롯/추론 스레드를 사용한다.
                // VI 픽셀 매핑·복사·색변환만 우회한다. 송출 영상은 계속 실제 카메라 영상이다.
                cv::Mat bgr = fixed_bgr.empty() ? copy_bgr(frame.frame) : fixed_bgr.clone();
                InferenceFrame sample;
                sample.bgr = std::move(bgr);
                sample.pts = frame.frame.stVFrame.u64PTS;
                sample.captured = captured;
                // 비어 있지 않으면 이전 대기 프레임을 교체한다. 처리중 프레임은 별도 소유한다.
                pending.publish(std::move(sample));
            }
            if (!interrupted && stopped.load()) result = 1;
        } catch (const std::exception &error) {
            std::fprintf(stderr, "AI INPUT ERROR: %s\n", error.what());
            ai_failed.store(true);
            pending.clear();
            result = 1;
            // AI 채널 획득/변환 오류도 주 영상 종료로 연결하지 않는다.
            // 단, 스레드 생성 실패 등으로 송출 스레드가 없으면 그대로 종료한다.
            if (video_active && streaming.joinable()) {
                std::fprintf(stderr, "AI capture disabled; keeping RTSP until Ctrl+C or stream error\n");
                while (!interrupted && !stopped.load())
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        stopped.store(true);
        pending.wake();
        if (streaming.joinable()) streaming.join();
        // 동기 rknn_run은 이 플래그로 취소되지 않는다. 종료 시 호출 복귀/드라이버 타임아웃을 기다릴 수 있다.
        if (inference.joinable()) inference.join();
        return (result || ai_failed.load()) ? 1 : 0;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "STARTUP ERROR: %s\n", error.what());
        return 1;
    }
}
