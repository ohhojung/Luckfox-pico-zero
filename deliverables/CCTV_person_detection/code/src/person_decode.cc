// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] YOLO 예제의 postprocess.cc의 RV1106 NHWC 헤드 해석·앵커·역양자화·YOLOv5 좌표 식 및 IoU/NMS와 대응한다.
// [출처] 우리 변경: 사람 클래스만 반환, Box 벡터, std::sort, 경계 검사, 최대128개, 공통 배율 letterbox 역변환. 원본 함수의 그대로 복사는 아니다.
// [출처] YOLO 예제의 postprocess.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/postprocess.cc :: anchor / process_i8_rv1106 / deqnt_affine_to_f32 / CalculateOverlap / nms / post_process
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/postprocess.cc
// [출처] YOLO 예제의 main.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/main.cc :: main / letterbox / mapCoordinates
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
// 하드웨어에 의존하지 않는 YOLOv5 후처리. 제공 모델의 앵커/80클래스/출력 배치를 전제로 한다.
#include "person_decode.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace person {
// 세 해상도의 헤드에 대응하는 표준 YOLOv5 앵커 폭·높이 쌍이다.
// [출처] YOLO 예제의 postprocess.cc anchor[3][6]과 값이 완전히 일치하는 YOLOv5 앵커 표다. 자체 학습으로 얻은 값으로 표시하지 않는다.
static const int anchors[3][6] = {
    {10, 13, 16, 30, 33, 23},
    {30, 61, 62, 45, 59, 119},
    {116, 90, 156, 198, 373, 326}
};
// 좌표를 영상 경계 안으로 제한한다.
// [출처] 경계 제한이라는 일반 연산. YOLO 예제의 postprocess.cc clamp는 int 반환/조건식, 여기는 float 반환/std::min·max로 작성했다.
static float clamp(float value, float low, float high) {
    return std::max(low, std::min(high, value));
}
// 비율 유지 리사이즈 크기와 중앙 여백을 계산한다. 예제처럼 크기를 절삭하고 공통 배율을 사용한다.
// [출처] YOLO 예제의 main.cc letterbox의 비율 유지/중앙 여백 계산과 대응. 원본과 같은 정수 절삭·공통 scale을 사용하며 유효 크기를 검사한다.
Letterbox letterbox(int w, int h, int mw, int mh) {
    if (w <= 0 || h <= 0 || mw <= 0 || mh <= 0)
        throw std::invalid_argument("invalid image dimensions");
    float scale = std::min(float(mw) / w, float(mh) / h);
    Letterbox result;
    result.resized_width = std::max(1, int(w * scale));
    result.resized_height = std::max(1, int(h * scale));
    result.left = (mw - result.resized_width) / 2;
    result.top = (mh - result.resized_height) / 2;
    result.scale_x = scale;
    result.scale_y = scale;
    return result;
}
// 두 검출 상자의 교집합/합집합 비율(IoU)을 계산한다.
// [출처] YOLO 예제의 postprocess.cc CalculateOverlap/nms와 같은 IoU 목적. 원본과 같이 폭·높이에 +1을 더해 면적을 계산한다.
static float overlap(const Box &a, const Box &b) {
    float intersection = std::max(0.f, std::min(a.right, b.right) - std::max(a.left, b.left) + 1.f) *
                         std::max(0.f, std::min(a.bottom, b.bottom) - std::max(a.top, b.top) + 1.f);
    float area = (a.right - a.left + 1.f) * (a.bottom - a.top + 1.f) +
                 (b.right - b.left + 1.f) * (b.bottom - b.top + 1.f) - intersection;
    return area > 0.f ? intersection / area : 0.f;
}
// 세 NHWC INT8 헤드에서 사람 후보를 만들고 점수 순서로 겹친 상자를 제거한다.
// [출처] YOLO 예제의 postprocess.cc process_i8_rv1106/post_process의 NHWC 해석을 사람 전용으로 재작성. 원본 다중 클래스 결과/별도 정렬 인덱스 대신 Box 벡터를 사용한다.
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
                    // 셀당 3앵커×85값: 좌표4+객체확률1+클래스80. 할당 끝 패딩을 채널로 해석하지 않는다.
                    // [출처] YOLO 예제의 postprocess.cc process_i8_rv1106의 hw_offset와 같은 셀255/앵커85 배치. 할당 패딩을 추가 채널로 세지 않는다.
                    const int8_t *p = head.data + (y * head.width + x) * 255 + a * 85;
                    // 실수 값 = (양자화 정수 - zero point) × scale.
                    // [출처] YOLO 예제의 postprocess.cc deqnt_affine_to_f32와 같은 (q-zp)*scale 식을 lambda로 표현했다.
                    auto value = [&](int i) { return (int(p[i]) - head.zero_point) * head.scale; };
                    // [출처] 원본 qnt_f32_to_affine: INT8 범위 제한 후 정수 절삭.
                    int gate = int(clamp(confidence / head.scale + head.zero_point, -128.f, 127.f));
                    if (int(p[4]) < gate) continue;
                    int best = 0;
                    for (int c = 1; c < 80; ++c)
                        if (p[5 + c] > p[5 + best]) best = c;
                    // COCO 클래스0이 사람이다. 최고 클래스가 사람이며 객체확률×사람확률이 기준을 초과해야 한다.
                    // [출처] YOLO 예제의 postprocess.cc의 객체확률×최고클래스확률 점수 방식에 사람(best==0) 제한 추가. 원본과 같은 limit_score>threshold 조건이다.
                    float score = value(4) * value(5);
                    if (best != 0 || score <= confidence) continue;
                    // YOLOv5의 그리드·앵커 복원 식으로 모델 입력 좌표계의 중심과 크기를 계산한다.
                    // [출처] YOLO 예제의 postprocess.cc process_i8_rv1106과 동일한 YOLOv5 중심(2p-0.5+grid)*stride 및 크기(2p)^2*anchor 식이다.
                    float cx = (value(0) * 2 - .5f + x) * stride;
                    float cy = (value(1) * 2 - .5f + y) * stride;
                    float bw = value(2) * 2.f;
                    bw = bw * bw * anchors[branch][a * 2];
                    float bh = value(3) * 2.f;
                    bh = bh * bh * anchors[branch][a * 2 + 1];
                    // 원본처럼 경계를 자르지 않은 좌표로 NMS를 수행한다.
                    Box box = {cx - bw / 2, cy - bh / 2,
                               cx + bw / 2, cy + bh / 2, score};
                    if (box.right > box.left && box.bottom > box.top) candidates.push_back(box);
                }
            }
        }
    }
    // 높은 점수부터 남기는 NMS. 반환 상자는 최대 128개로 제한한다.
    // [출처] YOLO 예제의 postprocess.cc quick_sort_indice_inverse+nms와 목적 대응. std::sort와 선택 상자 순회로 다시 작성; 사람만 처리하며 최대128개로 제한한다.
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
    // [출처] post_process처럼 NMS 이후 모델 경계 제한 및 정수 좌표 변환.
    for (auto &box : result) {
        box.left = int(clamp(box.left, 0, float(mw)));
        box.top = int(clamp(box.top, 0, float(mh)));
        box.right = int(clamp(box.right, 0, float(mw)));
        box.bottom = int(clamp(box.bottom, 0, float(mh)));
    }
    return result;
}
// 여백을 빼고 공통 배율로 나눠 원본 좌표로 복원한다. 여백에만 있던 상자는 제외한다.
// [출처] YOLO 예제의 main.cc mapCoordinates의 여백 제거/배율 복원과 대응. 공통 배율과 정수 절삭을 따르고 원본 영상 경계 제한/면적0 거부를 추가한다.
bool map_to_frame(Box &b, const Letterbox &t, int w, int h) {
    b.left = clamp(int((b.left - t.left) / t.scale_x), 0, float(w));
    b.right = clamp(int((b.right - t.left) / t.scale_x), 0, float(w));
    b.top = clamp(int((b.top - t.top) / t.scale_y), 0, float(h));
    b.bottom = clamp(int((b.bottom - t.top) / t.scale_y), 0, float(h));
    return b.right > b.left && b.bottom > b.top;
}
} // namespace person
