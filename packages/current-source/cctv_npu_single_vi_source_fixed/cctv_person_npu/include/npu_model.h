#pragma once

#include "person_decode.h"
#include "rknn_api.h"
#include <opencv2/core.hpp>
#include <string>
#include <vector>

class NpuModel {
public:
    NpuModel() = default;
    ~NpuModel();
    NpuModel(const NpuModel &) = delete;
    NpuModel &operator=(const NpuModel &) = delete;
    void open(const std::string &path);
    std::vector<person::Box> infer(const cv::Mat &rgb, float threshold);
private:
    rknn_context context_ = 0;
    rknn_tensor_attr input_ = {};
    rknn_tensor_attr output_[3] = {};
    rknn_tensor_mem *input_mem_ = nullptr;
    rknn_tensor_mem *output_mem_[3] = {};
    int width_ = 0, height_ = 0;
};
