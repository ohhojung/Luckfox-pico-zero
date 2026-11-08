#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace person {
struct Box {
    float left, top, right, bottom, score;
};
struct Head {
    const int8_t *data;
    size_t bytes;
    int height, width, zero_point;
    float scale;
};
struct Letterbox {
    int resized_width, resized_height, left, top;
    float scale_x, scale_y;
};
Letterbox letterbox(int width, int height, int model_width, int model_height);
// Three packed NHWC INT8 heads of the supplied COCO YOLOv5 model only.
// Keep the 80-class tensor layout; class 0 is person.
std::vector<Box> decode(const Head *heads, int model_width, int model_height,
                        float confidence, float nms_threshold);
bool map_to_frame(Box &box, const Letterbox &transform, int width, int height);
} // namespace person
