#include "person_decode.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace person {
static const int anchors[3][6] = {
    {10, 13, 16, 30, 33, 23},
    {30, 61, 62, 45, 59, 119},
    {116, 90, 156, 198, 373, 326}
};
static float clamp(float value, float low, float high) {
    return std::max(low, std::min(high, value));
}
Letterbox letterbox(int w, int h, int mw, int mh) {
    if (w <= 0 || h <= 0 || mw <= 0 || mh <= 0)
        throw std::invalid_argument("invalid image dimensions");
    float scale = std::min(float(mw) / w, float(mh) / h);
    Letterbox result;
    result.resized_width = std::max(1, int(std::round(w * scale)));
    result.resized_height = std::max(1, int(std::round(h * scale)));
    result.left = (mw - result.resized_width) / 2;
    result.top = (mh - result.resized_height) / 2;
    result.scale_x = float(result.resized_width) / w;
    result.scale_y = float(result.resized_height) / h;
    return result;
}
static float overlap(const Box &a, const Box &b) {
    float intersection = std::max(0.f, std::min(a.right, b.right) - std::max(a.left, b.left)) *
                         std::max(0.f, std::min(a.bottom, b.bottom) - std::max(a.top, b.top));
    float area = (a.right - a.left) * (a.bottom - a.top) +
                 (b.right - b.left) * (b.bottom - b.top) - intersection;
    return area > 0.f ? intersection / area : 0.f;
}
std::vector<Box> decode(const Head *heads, int mw, int mh, float confidence, float nms) {
    if (!heads || mw <= 0 || mh <= 0 || !(confidence > 0 && confidence < 1) ||
        !(nms > 0 && nms < 1))
        throw std::invalid_argument("invalid decoder arguments");
    std::vector<Box> candidates;
    for (int branch = 0; branch < 3; ++branch) {
        const Head &head = heads[branch];
        const int stride = 8 << branch;
        if (!head.data || head.width != mw / stride || head.height != mh / stride ||
            mw % stride || mh % stride || !(head.scale > 0) ||
            head.bytes < size_t(head.width) * head.height * 255)
            throw std::invalid_argument("unsupported YOLOv5 output shape/stride");
        for (int y = 0; y < head.height; ++y) {
            for (int x = 0; x < head.width; ++x) {
                for (int a = 0; a < 3; ++a) {
                    const int8_t *p = head.data + (y * head.width + x) * 255 + a * 85;
                    auto value = [&](int i) { return (int(p[i]) - head.zero_point) * head.scale; };
                    if (value(4) < confidence) continue;
                    int best = 0;
                    for (int c = 1; c < 80; ++c)
                        if (p[5 + c] > p[5 + best]) best = c;
                    float score = value(4) * value(5);
                    if (best != 0 || score < confidence) continue;
                    float cx = (value(0) * 2 - .5f + x) * stride;
                    float cy = (value(1) * 2 - .5f + y) * stride;
                    float bw = std::pow(value(2) * 2, 2) * anchors[branch][a * 2];
                    float bh = std::pow(value(3) * 2, 2) * anchors[branch][a * 2 + 1];
                    Box box = {clamp(cx - bw / 2, 0, float(mw)),
                               clamp(cy - bh / 2, 0, float(mh)),
                               clamp(cx + bw / 2, 0, float(mw)),
                               clamp(cy + bh / 2, 0, float(mh)), score};
                    if (box.right > box.left && box.bottom > box.top) candidates.push_back(box);
                }
            }
        }
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const Box &a, const Box &b) { return a.score > b.score; });
    std::vector<Box> result;
    for (const Box &box : candidates) {
        bool suppressed = false;
        for (const Box &chosen : result)
            if (overlap(box, chosen) > nms) { suppressed = true; break; }
        if (!suppressed) result.push_back(box);
        if (result.size() == 128) break;
    }
    return result;
}
bool map_to_frame(Box &b, const Letterbox &t, int w, int h) {
    b.left = clamp((b.left - t.left) / t.scale_x, 0, float(w));
    b.right = clamp((b.right - t.left) / t.scale_x, 0, float(w));
    b.top = clamp((b.top - t.top) / t.scale_y, 0, float(h));
    b.bottom = clamp((b.bottom - t.top) / t.scale_y, 0, float(h));
    return b.right > b.left && b.bottom > b.top;
}
} // namespace person
