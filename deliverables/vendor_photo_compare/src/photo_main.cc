// 사진 입력/반복/로그만 프로젝트에서 추가한 진단 진입점.
// init_yolov5_model, inference_yolov5_model, release_yolov5_model와 후처리는
// VM의 공식 예제 src/yolov5.cc, src/postprocess.cc를 수정 없이 빌드한다.
// [출처] LuckfoxTECH/luckfox_pico_rkmpi_example, kernel-5.10.160,
// example/luckfox_pico_rtsp_yolov5/src/main.cc의 BGR letterbox/memcpy 흐름 참고.
// 공식 사진 예제 완제품 또는 원본 main.cc가 아니다. 카메라/RTSP는 시작하지 않는다.
#include "yolov5.h"
#include CCTV_IMAGE_IO_HEADER
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>

static volatile std::sig_atomic_t interrupted = 0;
static void stop(int) { interrupted = 1; }
static int integer(const char *s, int lo, int hi) {
    char *end = nullptr;
    long n = std::strtol(s, &end, 10);
    if (!*s || *end || n < lo || n > hi) throw std::runtime_error("Invalid count or interval");
    return static_cast<int>(n);
}
// 공식 초기화가 성공한 경우에만 원본 해제 함수를 호출한다.
struct ModelOwner {
    rknn_app_context_t ctx = {};
    bool initialized = false, post_initialized = false;
    ~ModelOwner() {
        if (initialized) release_yolov5_model(&ctx);
        if (post_initialized) deinit_post_process();
    }
};
int main(int argc, char **argv) {
    setvbuf(stdout, nullptr, _IOLBF, 0);
    std::signal(SIGINT, stop);
    std::signal(SIGTERM, stop);
    if (argc < 3 || argc > 5) {
        std::fprintf(stderr, "Usage: %s model.rknn person.jpg [count=20] [start_interval_ms=1000]\n", argv[0]);
        return 2;
    }
    int completed = 0;
    try {
        const int count = argc > 3 ? integer(argv[3], 1, 10000) : 20;
        const int interval = argc > 4 ? integer(argv[4], 0, 60000) : 1000;
        cv::setNumThreads(1);
        cv::Mat photo = cv::imread(argv[2], 1);
        if (photo.empty()) throw std::runtime_error("Cannot read photo");
        ModelOwner owner;
        int ret = init_yolov5_model(argv[1], &owner.ctx);
        if (ret != 0) throw std::runtime_error("Vendor init_yolov5_model failed");
        owner.initialized = true;
        auto &ctx = owner.ctx;
        if (ctx.model_width != 640 || ctx.model_height != 640 || ctx.model_channel != 3 ||
            ctx.io_num.n_input != 1 || ctx.io_num.n_output != 3 ||
            !ctx.input_mems[0] || !ctx.input_mems[0]->virt_addr ||
            ctx.input_attrs[0].size_with_stride < 640 * 640 * 3 ||
            (ctx.input_attrs[0].w_stride && ctx.input_attrs[0].w_stride != 640))
            throw std::runtime_error("This comparison requires the existing packed 640x640x3 YOLOv5 model");
        if (init_post_process() != 0) throw std::runtime_error("Vendor postprocess initialization failed");
        owner.post_initialized = true;
        rknn_sdk_version version = {};
        if (rknn_query(ctx.rknn_ctx, RKNN_QUERY_SDK_VERSION, &version, sizeof(version)) == 0)
            std::printf("RKNN runtime=%s driver=%s\n", version.api_version, version.drv_version);

        // 공식 main의 비율/정수 절삭/검정 여백 방식을 사진 원본 크기에 적용한다.
        // 매회 같은 입력을 복사해 전처리 변동을 제거한다. BGR 채널 순서는 유지한다.
        const float sx = 640.0f / photo.cols, sy = 640.0f / photo.rows;
        const float scale = sx < sy ? sx : sy;
        const int width = static_cast<int>(photo.cols * scale);
        const int height = static_cast<int>(photo.rows * scale);
        if (width < 1 || height < 1) throw std::runtime_error("Unsupported image aspect ratio");
        cv::Mat resized, input(640, 640, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::resize(photo, resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        resized.copyTo(input(cv::Rect((640-width)/2, (640-height)/2, width, height)));
        unsigned hash = 2166136261u;
        for (int i=0; i<640*640*3; ++i) hash = (hash ^ input.data[i]) * 16777619u;
        std::printf("VENDOR_PHOTO_READY image=%dx%d input=BGR640x640 fnv1a32=%08x count=%d interval_ms=%d\n",
                    photo.cols, photo.rows, hash, count, interval);
        std::puts("Vendor yolov5.cc + postprocess.cc; custom photo harness; camera/RTSP disabled.");
        for (int i=0; i<count && !interrupted; ++i) {
            const auto start = std::chrono::steady_clock::now();
            std::memcpy(ctx.input_mems[0]->virt_addr, input.data, 640*640*3);
            object_detect_result_list results = {};
            ret = inference_yolov5_model(&ctx, &results);
            if (ret != 0) {
                std::fprintf(stderr, "VENDOR_INFER_FAILED run=%d completed=%d ret=%d\n", i+1, completed, ret);
                return 1;
            }
            ++completed;
            int persons = 0;
            for (int j=0; j<results.count; ++j)
                if (results.results[j].cls_id == 0 && results.results[j].prop >= .5f) ++persons;
            double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now()-start).count();
            std::printf("VENDOR_PASS run=%d completed=%d objects=%d person_ge_0_5=%d run_post_ms=%.1f\n",
                        i+1, completed, results.count, persons, ms);
            // 간격은 시작 시점 기준이다. 마지막 성공 뒤에는 기다리지 않고 종료한다.
            if (i+1 < count) {
                const auto next = start + std::chrono::milliseconds(interval);
                while (!interrupted && std::chrono::steady_clock::now() < next)
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        std::printf("VENDOR_DONE completed=%d interrupted=%d\n", completed, int(interrupted));
        return interrupted ? 130 : 0;
    } catch (const std::exception &e) {
        std::fprintf(stderr, "VENDOR_PHOTO_ERROR completed=%d: %s\n", completed, e.what());
        return 1;
    }
}
