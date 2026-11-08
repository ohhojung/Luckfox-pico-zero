// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] 별도 사진 시험 진입점은 프로젝트용 작성 부분. YOLO 예제의 main.cc main의 검출 상자·문자 그리기와 같은 OpenCV API를 사용한다.
// [출처] 공식 YOLO 예제의 main.cc는 실시간 VI 입력이고 이 파일은 imread 파일 입력이다. imread/imwrite 진입점을 특정 공식 파일에서 가져온 이력은 확인되지 않았다.
// [출처] YOLO 예제의 main.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/main.cc :: main / letterbox / mapCoordinates
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
// 정지 이미지 검증용 실행 파일. 카메라/AIQ/RTSP를 시작하지 않고 모델 경로와 후처리를 분리 검증한다.
#include "npu_model.h"
#include CCTV_IMAGE_IO_HEADER
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>

// 인자: 모델, 입력 사진, 선택 출력 사진, 선택 신뢰도. 사진 검출 성공이 실시간 동시 실행 성공을 보증하지 않는다.
// [출처] 프로젝트 추가 파일 입력 실행 흐름. YOLO 예제의 main.cc의 카메라·RTSP 초기화 및 루프를 옮긴 코드가 아니다.
int main(int argc, char **argv) {
    if (argc < 3 || argc > 5) {
        std::fprintf(stderr, "Usage: %s model.rknn image.jpg [result.jpg] [confidence=0.5]\n", argv[0]);
        return 2;
    }
    try {
        float threshold = .5f;
        if (argc == 5) {
            char *end = nullptr;
            threshold = std::strtof(argv[4], &end);
            if (!argv[4][0] || *end || !(threshold > 0 && threshold < 1))
                throw std::runtime_error("confidence must be between 0 and 1");
        }
        cv::setNumThreads(1);
        // OpenCV 파일 입력의 BGR을 예제와 같은 추론 입력으로 전달한다.
        cv::Mat bgr = cv::imread(argv[2], 1);
        if (bgr.empty()) throw std::runtime_error("Cannot read input image");
        NpuModel model;
        model.open(argv[1]);
        // 시간 측정은 모델 로드를 제외하고 전처리·NPU 실행·후처리를 포함한다.
        auto begin = std::chrono::steady_clock::now();
        auto boxes = model.infer(bgr, threshold);
        double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
        std::printf("PERSON count=%zu image=%dx%d detect_ms=%.1f\n", boxes.size(), bgr.cols, bgr.rows, ms);
        // 복원된 원본 사진 좌표로 검출 박스와 점수를 표시한다.
        // [출처] YOLO 예제의 main.cc main처럼 rectangle/putText로 표시하되 사람 고정 라벨, snprintf, 다른 두께/글꼴 크기를 사용한다. 좌표는 infer에서 복원된다.
        for (const auto &b : boxes) {
            std::printf("person score=%.3f box=(%d,%d,%d,%d)\n", b.score,
                        int(b.left), int(b.top), int(b.right), int(b.bottom));
            cv::rectangle(bgr, cv::Point(int(b.left), int(b.top)),
                          cv::Point(int(b.right), int(b.bottom)), cv::Scalar(0, 255, 0), 2);
            char label[48];
            std::snprintf(label, sizeof(label), "person %.2f", b.score);
            cv::putText(bgr, label, cv::Point(int(b.left), std::max(16, int(b.top) - 5)),
                        cv::FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(0, 255, 0), 1);
        }
        // 출력 경로가 있으면 결과 사진을 저장하고 실패 시 오류로 처리한다.
        if (argc >= 4 && !cv::imwrite(argv[3], bgr))
            throw std::runtime_error("Cannot save result image");
        // No camera, RKAIQ, RTSP, framebuffer, or rkipc control in this executable.
        return 0;
    } catch (const std::exception &e) {
        std::fprintf(stderr, "IMAGE TEST FAILED: %s\n", e.what());
        return 1;
    }
}
