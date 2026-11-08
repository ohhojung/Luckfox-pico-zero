// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] YOLO 예제의 yolov5.cc이 관리하는 RKNN 컨텍스트/속성/입출력 메모리 역할을 C++ 소유 클래스에 모았다.
// [출처] NpuModel 선언, 복사 금지, open/infer 인터페이스는 프로젝트용 작성 부분. rknn_api.h의 SDK 타입을 사용하며 SDK 헤더를 재작성한 것이 아니다.
// [출처] YOLO 예제의 yolov5.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/yolov5.cc :: init_yolov5_model / inference_yolov5_model / release_yolov5_model
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/yolov5.cc
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
#pragma once

#include "person_decode.h"
#include "rknn_api.h"
#include <opencv2/core.hpp>
#include <string>
#include <vector>

// RKNN 자원의 단일 소유자. 복사를 금지하고 소멸자로 정리한다. infer 동시 호출은 지원하지 않는다.
// [출처] YOLO 예제의 yolov5.cc의 함수형 컨텍스트 관리 역할을 묶은 자체 인터페이스. 공식 클래스 이름이나 선언을 그대로 가져온 것이 아니다.
class NpuModel {
public:
    NpuModel() = default;
    ~NpuModel();
    NpuModel(const NpuModel &) = delete;
    NpuModel &operator=(const NpuModel &) = delete;
    // 한 번만 열 수 있다. 실패 시 예외를 던지며 이미 할당된 자원은 소멸자가 정리한다.
    void open(const std::string &path);
    // 입력은 CV_8UC3 BGR, 반환은 입력 영상 좌표의 사람 상자다.
    std::vector<person::Box> infer(const cv::Mat &bgr, float threshold);
private:
    // 0/null 초기값으로 부분 초기화의 정리 여부를 구분한다.
    rknn_context context_ = 0;
    rknn_tensor_attr input_ = {};
    rknn_tensor_attr output_[3] = {};
    rknn_tensor_mem *input_mem_ = nullptr;
    rknn_tensor_mem *output_mem_[3] = {};
    int width_ = 0, height_ = 0;
};
