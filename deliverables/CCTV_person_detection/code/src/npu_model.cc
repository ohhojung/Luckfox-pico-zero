// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] YOLO 예제의 yolov5.cc의 native 속성 조회, UINT8 입력, create_mem/set_io_mem/rknn_run 흐름과 대응한다.
// [출처] 우리 변경: NpuModel 클래스, 모델 제약 검사, 예외 처리, stride 검사, BGR/검정 여백, 사람 전용 후처리.
// [출처] YOLO 예제의 yolov5.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/yolov5.cc :: init_yolov5_model / inference_yolov5_model / release_yolov5_model
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/yolov5.cc
// [출처] YOLO 예제의 main.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/main.cc :: main / letterbox / mapCoordinates
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
// RKNN 모델과 입출력 메모리 관리. 제공된 RV1106용 640×640 COCO YOLOv5 구조에 특화되어 있다.
#include "npu_model.h"
#include <opencv2/imgproc.hpp>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// RKNN의 음수 반환값을 예외로 전달한다.
// [출처] 프로젝트 예외 래퍼. YOLO 예제의 yolov5.cc은 각 API 뒤에 printf/return으로 오류를 처리한다.
static void checked(int result, const char *operation) {
    if (result < 0) throw std::runtime_error(std::string(operation) + ": " + std::to_string(result));
}
// 논리 크기와 실제 할당 크기를 함께 출력해 패딩·레이아웃 문제를 진단한다.
// [출처] YOLO 예제의 yolov5.cc dump_tensor_attr와 같은 진단 목적. 출력 포맷을 단순화하고 할당 크기·행 stride를 추가했다.
static void describe(const char *name, const rknn_tensor_attr &a) {
    std::printf("%s[%u] dims=[%u,%u,%u,%u] fmt=%d type=%d quant=%d zp=%d scale=%g "
                "size=%u size_with_stride=%u w_stride=%u\n", name, a.index,
                a.dims[0], a.dims[1], a.dims[2], a.dims[3], int(a.fmt), int(a.type),
                int(a.qnt_type), a.zp, double(a.scale), a.size, a.size_with_stride, a.w_stride);
}
// 입출력 메모리를 해제한 뒤 컨텍스트를 해제한다. 부분 초기화 실패도 정리한다.
// [출처] YOLO 예제의 yolov5.cc release_yolov5_model의 메모리 해제→컨텍스트 해제 순서를 클래스 소멸자로 재구성. 속성은 고정 멤버이므로 별도 free하지 않는다.
NpuModel::~NpuModel() {
    if (!context_) return;
    if (input_mem_) rknn_destroy_mem(context_, input_mem_);
    for (auto memory : output_mem_) if (memory) rknn_destroy_mem(context_, memory);
    rknn_destroy(context_);
}
// 모델 로드 및 레이아웃 검증 후 입출력 DMA 메모리를 한 번 할당하고 재사용한다.
// [출처] YOLO 예제의 yolov5.cc init_yolov5_model과 API 흐름 대응. 우리는 입력1/출력3 및640×640 제약을 검사하고 부분 실패는 소멸자로 정리한다.
void NpuModel::open(const std::string &path) {
    if (context_) throw std::runtime_error("Model is already open");
    std::printf("Loading RKNN model: %s\n", path.c_str());
    checked(rknn_init(&context_, const_cast<char *>(path.c_str()), 0, 0, nullptr), "rknn_init");
    rknn_sdk_version version = {};
    checked(rknn_query(context_, RKNN_QUERY_SDK_VERSION, &version, sizeof(version)), "query RKNN version");
    std::printf("RKNN runtime=%s driver=%s\n", version.api_version, version.drv_version);
    rknn_input_output_num count = {};
    checked(rknn_query(context_, RKNN_QUERY_IN_OUT_NUM, &count, sizeof(count)), "query I/O");
    if (count.n_input != 1 || count.n_output != 3)
        throw std::runtime_error("Expected the supplied COCO YOLOv5 model: 1 input, 3 outputs");
    checked(rknn_query(context_, RKNN_QUERY_NATIVE_INPUT_ATTR, &input_, sizeof(input_)), "query input");
    describe("native input", input_);
    if (input_.n_dims != 4 || input_.fmt != RKNN_TENSOR_NHWC ||
        input_.dims[0] != 1 || input_.dims[3] != 3 || input_.type != RKNN_TENSOR_INT8 ||
        input_.qnt_type != RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC || !(input_.scale > 0))
        throw std::runtime_error("Expected RV1106 quantized INT8 4D NHWC input; reconvert for rv1106 with INT8 calibration");
    height_ = input_.dims[1];
    width_ = input_.dims[2];
    if (width_ != 640 || height_ != 640)
        throw std::runtime_error("Only the supplied 640x640 COCO YOLOv5 is supported");
    // The native model is INT8. Per Rockchip's RV1106 C example, provide UINT8
    // image pixels and let set_io_mem fuse the model's normalization/quantization.
    // Do not cast image bytes to INT8 or normalize by 255 a second time on the CPU.
    // 원본 모델 속성은 INT8이지만 전달하는 영상은 UINT8이다. 중복 정규화를 하지 않는다.
    // [출처] YOLO 예제의 yolov5.cc init_yolov5_model도 native 입력 타입을 UINT8로 바꿔 정규화·양자화 융합 경로를 사용한다.
    // 원본 fmt=NHWC 대입 대신 우리는 조회된 NHWC를 검사한다.
    input_.type = RKNN_TENSOR_UINT8;
    size_t row_pixels = input_.w_stride ? input_.w_stride : width_;
    if (row_pixels < size_t(width_) || input_.size_with_stride < row_pixels * height_ * 3)
        throw std::runtime_error("Invalid model input allocation/stride");
    for (int i = 0; i < 3; ++i) {
        // 80×80, 40×40, 20×20 출력마다 셀당 255개 값을 요구한다. 임의 YOLO 모델을 지원하지 않는다.
        output_[i].index = i;
        checked(rknn_query(context_, RKNN_QUERY_NATIVE_NHWC_OUTPUT_ATTR,
                           &output_[i], sizeof(output_[i])), "query output");
        const auto &a = output_[i];
        describe("native output", a);
        const unsigned grid = 80 >> i;
        const size_t bytes = size_t(grid) * grid * 255;
        if (a.n_dims != 4 || a.fmt != RKNN_TENSOR_NHWC || a.type != RKNN_TENSOR_INT8 ||
            a.qnt_type != RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC || a.dims[0] != 1 ||
            a.dims[1] != grid || a.dims[2] != grid || a.dims[3] != 255 ||
            a.size != bytes || a.size_with_stride < bytes ||
            (a.w_stride != 0 && a.w_stride != grid) || !(a.scale > 0))
            throw std::runtime_error("Unsupported output layout: expected packed NHWC INT8 COCO heads");
        // Native NHWC is decoded with the vendor's 255-byte cell layout.
        // Allocation size may exceed logical size; do not infer 256 channels
        // from size_with_stride or decode the extra allocation bytes.
        std::printf("output[%d] logical_bytes=%u allocation_bytes=%u\n",
                    i, a.size, a.size_with_stride);
    }
    input_mem_ = rknn_create_mem(context_, input_.size_with_stride);
    if (!input_mem_ || !input_mem_->virt_addr) throw std::runtime_error("NPU input allocation failed");
    checked(rknn_set_io_mem(context_, input_mem_, &input_), "bind NPU input");
    for (int i = 0; i < 3; ++i) {
        const auto &a = output_[i];
        // 실제 할당에는 패딩 포함 크기를 쓰고 후처리에는 논리 크기를 사용한다.
        // [출처] YOLO 예제의 yolov5.cc처럼 size_with_stride로 메모리를 할당한다. 논리 size만 디코더에 넘기고 부족한 할당을 거부하는 검사는 추가했다.
        output_mem_[i] = rknn_create_mem(context_, a.size_with_stride);
        if (!output_mem_[i] || !output_mem_[i]->virt_addr)
            throw std::runtime_error("NPU output allocation failed");
        checked(rknn_set_io_mem(context_, output_mem_[i], &output_[i]), "bind NPU output");
    }
    std::printf("NPU ready: BGR 640x640, COCO person class=0, INT8 output\n");
}
// BGR 입력→비율 유지 리사이즈/여백→입력 복사→동기 추론→사람 필터/NMS→원본 좌표 복원.
// [출처] YOLO 예제의 main.cc letterbox/입력memcpy와 YOLO 예제의 yolov5.cc inference_yolov5_model을 한 함수로 묶은 구성. YOLO 예제의 main.cc와 같은 BGR/검정 여백을 사용하며 stride 검사와 사람 전용 후처리를 유지한다.
std::vector<person::Box> NpuModel::infer(const cv::Mat &bgr, float threshold) {
    if (bgr.empty() || bgr.type() != CV_8UC3) throw std::runtime_error("Expected BGR888 input");
    auto transform = person::letterbox(bgr.cols, bgr.rows, width_, height_);
    // 여백 값은 각 채널 0으로 공식 예제의 검정 여백과 같다.
    cv::Mat input(height_, width_, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat resized;
    cv::resize(bgr, resized, cv::Size(transform.resized_width, transform.resized_height));
    resized.copyTo(input(cv::Rect(transform.left, transform.top,
                                 transform.resized_width, transform.resized_height)));
    // NPU 입력의 가상 행 폭을 반영한다. CPU 영상 변환·복사가 있어 전체 경로가 zero-copy는 아니다.
    size_t stride = (input_.w_stride ? input_.w_stride : width_) * 3;
    auto destination = static_cast<unsigned char *>(input_mem_->virt_addr);
    std::memset(destination, 0, input_.size_with_stride);
    for (int y = 0; y < height_; ++y)
        std::memcpy(destination + y * stride, input.ptr(y), width_ * 3);
    // Same RV1106 zero-copy set_io_mem + rknn_run path as the vendor example.
    // 작업 완료까지 호출이 대기한다. 실패하면 이후 후처리를 실행하지 않는다.
    // [출처] YOLO 예제의 yolov5.cc inference_yolov5_model과 동일한 동기 rknn_run API. 실패 시 return -1 대신 예외로 호출자에게 전달한다.
    checked(rknn_run(context_, nullptr), "rknn_run");
    person::Head heads[3];
    for (int i = 0; i < 3; ++i) {
        heads[i] = {static_cast<int8_t *>(output_mem_[i]->virt_addr),
                    output_[i].size, int(output_[i].dims[1]),
                    int(output_[i].dims[2]), output_[i].zp, output_[i].scale};
    }
    // 출력은 이미 sigmoid를 포함한 모델 헤드라는 전제다. NMS IoU 기준은 0.45다.
    auto boxes = person::decode(heads, width_, height_, threshold, .45f);
    std::vector<person::Box> mapped;
    for (auto box : boxes)
        if (person::map_to_frame(box, transform, bgr.cols, bgr.rows)) mapped.push_back(box);
    return mapped;
}
