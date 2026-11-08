#include "person_decode.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>

static void require(bool condition, const char *message) {
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
    std::vector<int8_t> data[3];
    person::Head heads[3];
    for (int i = 0; i < 3; ++i) {
        int size = 80 >> i;
        data[i].assign(size * size * 255, -128);
        heads[i] = {data[i].data(), data[i].size(), size, size, -128, 1.f / 255};
    }
    auto empty = person::decode(heads, 640, 640, .5f, .45f);
    require(empty.empty(), "empty frame must not detect people");
    // Two heavily overlapping person boxes, and a higher-scoring bicycle box.
    auto box = [&](int x, int anchor, float w, float h, int cls) {
        int8_t *p = data[0].data() + (40 * 80 + x) * 255 + anchor * 85;
        auto quant = [](float value) { return int8_t(std::round(value * 255) - 128); };
        p[0] = quant(.5f); p[1] = quant(.5f);
        p[2] = quant(w); p[3] = quant(h);
        p[4] = 127; p[5 + cls] = 127;
    };
    box(40, 0, .5f, .5f, 0); // roughly 10 x 13
    box(40, 1, std::sqrt(10.f / 16) / 2, std::sqrt(13.f / 30) / 2, 0);
    box(20, 0, .5f, .5f, 1);
    auto found = person::decode(heads, 640, 640, .5f, .45f);
    require(found.size() == 1, "NMS must merge duplicate people and exclude bicycle");
    // RKNN reports a larger allocation than the logical NHWC tensor.
    // Keep valid data packed, poison allocation tail, and expose only logical bytes.
    for (int i = 0; i < 3; ++i) {
        data[i].resize(size_t(80 >> i) * (80 >> i) * 256, 127);
        heads[i].data = data[i].data();
    }
    auto padded = person::decode(heads, 640, 640, .5f, .45f);
    require(padded.size() == found.size(), "allocation tail must not create detections");
    require(std::fabs(padded[0].left - found[0].left) < .001f &&
            std::fabs(padded[0].score - found[0].score) < .001f,
            "extra allocation must not change packed tensor decoding");
    require(std::fabs(found[0].left - 319) < .2f, "decoded x must match grid and anchor");
    require(found[0].score > .99f, "score must be objectness times class probability");
    auto t = person::letterbox(640, 360, 640, 640);
    require(t.left == 0 && t.top == 140 && t.resized_height == 360, "16:9 letterbox padding");
    person::Box full = {0, 140, 640, 500, 1};
    require(person::map_to_frame(full, t, 640, 360), "visible box");
    require(full.top == 0 && full.bottom == 360 && full.right == 640, "padding removal");
    person::Box padding = {0, 0, 30, 100, 1};
    require(!person::map_to_frame(padding, t, 640, 360), "padding-only box rejected");
    auto odd = person::letterbox(853, 479, 640, 640);
    person::Box edge = {float(odd.left), float(odd.top),
                       float(odd.left + odd.resized_width), float(odd.top + odd.resized_height), 1};
    require(person::map_to_frame(edge, odd, 853, 479), "rounded resize mapping");
    require(std::fabs(edge.right - 853) < .01 && std::fabs(edge.bottom - 479) < .01,
            "rounding must not distort source bounds");
    bool rejected = false;
    heads[0].bytes = 5;
    try { person::decode(heads, 640, 640, .5f, .45f); }
    catch (const std::invalid_argument &) { rejected = true; }
    require(rejected, "truncated tensor must fail safely");
    std::puts("PASS: person filtering, NMS, quantized decode, letterbox mapping, tensor bounds");
}
