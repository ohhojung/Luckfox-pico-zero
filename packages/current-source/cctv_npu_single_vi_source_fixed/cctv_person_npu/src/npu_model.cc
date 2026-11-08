#include "npu_model.h"
#include <opencv2/imgproc.hpp>
#include <cstdio>
#include <cstring>
#include <stdexcept>

static void checked(int result, const char *operation) {
    if (result < 0) throw std::runtime_error(std::string(operation) + ": " + std::to_string(result));
}
static void describe(const char *name, const rknn_tensor_attr &a) {
    std::printf("%s[%u] dims=[%u,%u,%u,%u] fmt=%d type=%d quant=%d zp=%d scale=%g "
                "size=%u size_with_stride=%u w_stride=%u\n", name, a.index,
                a.dims[0], a.dims[1], a.dims[2], a.dims[3], int(a.fmt), int(a.type),
                int(a.qnt_type), a.zp, double(a.scale), a.size, a.size_with_stride, a.w_stride);
}
NpuModel::~NpuModel() {
    if (!context_) return;
    if (input_mem_) rknn_destroy_mem(context_, input_mem_);
    for (auto memory : output_mem_) if (memory) rknn_destroy_mem(context_, memory);
    rknn_destroy(context_);
}
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
    // RGB pixels and let set_io_mem fuse the model's normalization/quantization.
    // Do not cast RGB bytes to INT8 or normalize by 255 a second time on the CPU.
    input_.type = RKNN_TENSOR_UINT8;
    size_t row_pixels = input_.w_stride ? input_.w_stride : width_;
    if (row_pixels < size_t(width_) || input_.size_with_stride < row_pixels * height_ * 3)
        throw std::runtime_error("Invalid model input allocation/stride");
    input_mem_ = rknn_create_mem(context_, input_.size_with_stride);
    if (!input_mem_ || !input_mem_->virt_addr) throw std::runtime_error("NPU input allocation failed");
    checked(rknn_set_io_mem(context_, input_mem_, &input_), "bind NPU input");
    for (int i = 0; i < 3; ++i) {
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
        output_mem_[i] = rknn_create_mem(context_, a.size_with_stride);
        if (!output_mem_[i] || !output_mem_[i]->virt_addr)
            throw std::runtime_error("NPU output allocation failed");
        checked(rknn_set_io_mem(context_, output_mem_[i], &output_[i]), "bind NPU output");
    }
    std::printf("NPU ready: RGB 640x640, COCO person class=0, INT8 output\n");
}
std::vector<person::Box> NpuModel::infer(const cv::Mat &rgb, float threshold) {
    if (rgb.empty() || rgb.type() != CV_8UC3) throw std::runtime_error("Expected RGB888 input");
    auto transform = person::letterbox(rgb.cols, rgb.rows, width_, height_);
    cv::Mat input(height_, width_, CV_8UC3, cv::Scalar(114, 114, 114));
    cv::Mat resized;
    cv::resize(rgb, resized, cv::Size(transform.resized_width, transform.resized_height));
    resized.copyTo(input(cv::Rect(transform.left, transform.top,
                                 transform.resized_width, transform.resized_height)));
    size_t stride = (input_.w_stride ? input_.w_stride : width_) * 3;
    auto destination = static_cast<unsigned char *>(input_mem_->virt_addr);
    std::memset(destination, 0, input_.size_with_stride);
    for (int y = 0; y < height_; ++y)
        std::memcpy(destination + y * stride, input.ptr(y), width_ * 3);
    // Same RV1106 zero-copy set_io_mem + rknn_run path as the vendor example.
    checked(rknn_run(context_, nullptr), "rknn_run");
    person::Head heads[3];
    for (int i = 0; i < 3; ++i) {
        heads[i] = {static_cast<int8_t *>(output_mem_[i]->virt_addr),
                    output_[i].size, int(output_[i].dims[1]),
                    int(output_[i].dims[2]), output_[i].zp, output_[i].scale};
    }
    auto boxes = person::decode(heads, width_, height_, threshold, .45f);
    std::vector<person::Box> mapped;
    for (auto box : boxes)
        if (person::map_to_frame(box, transform, rgb.cols, rgb.rows)) mapped.push_back(box);
    return mapped;
}
