#include "npu_model.h"
#include CCTV_IMAGE_IO_HEADER
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>

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
        cv::Mat bgr = cv::imread(argv[2], 1); // color BGR, including reduced OpenCV builds
        if (bgr.empty()) throw std::runtime_error("Cannot read input image");
        cv::Mat rgb;
        cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
        NpuModel model;
        model.open(argv[1]);
        auto begin = std::chrono::steady_clock::now();
        auto boxes = model.infer(rgb, threshold);
        double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
        std::printf("PERSON count=%zu image=%dx%d detect_ms=%.1f\n", boxes.size(), rgb.cols, rgb.rows, ms);
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
        if (argc >= 4 && !cv::imwrite(argv[3], bgr))
            throw std::runtime_error("Cannot save result image");
        // No camera, RKAIQ, RTSP, framebuffer, or rkipc control in this executable.
        return 0;
    } catch (const std::exception &e) {
        std::fprintf(stderr, "IMAGE TEST FAILED: %s\n", e.what());
        return 1;
    }
}
