// [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
// [출처] YOLO 예제의 postprocess.cc의 검출 상자·양자화 정보, YOLO 예제의 main.cc의 letterbox 변환에 필요한 정보를 담는 프로젝트용 자료구조다.
// [출처] Box/Head/Letterbox 선언 자체가 특정 공식 헤더와 동일하다는 근거는 없다. 외부 출력 메모리를 소유하지 않는다.
// [출처] YOLO 예제의 postprocess.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/postprocess.cc :: anchor / process_i8_rv1106 / deqnt_affine_to_f32 / CalculateOverlap / nms / post_process
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/postprocess.cc
// [출처] YOLO 예제의 main.cc: 작업공간/luckfox_pico_rtsp_yolov5/src/main.cc :: main / letterbox / mapCoordinates
// [출처] https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
// [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace person {
// 픽셀 단위 좌상단/우하단 좌표와 검출 점수. 좌표계는 decode 후 모델, map 후 원본이다.
struct Box {
    float left, top, right, bottom, score;
};
// 외부가 소유한 출력 메모리를 읽기 전용 참조한다. bytes는 패딩 제외 논리 크기다.
// [출처] YOLO 예제의 postprocess.cc의 zp/scale와 출력 포인터 정보를 넘기기 위한 프로젝트 구조. SDK 구조체의 직접 복사가 아니다.
struct Head {
    const int8_t *data;
    size_t bytes;
    int height, width, zero_point;
    float scale;
};
// 리사이즈 크기, 여백, 원본 대비 공통 배율(두 필드에 같은 값)을 저장한다.
struct Letterbox {
    int resized_width, resized_height, left, top;
    float scale_x, scale_y;
};
// 입력/모델 크기로 전처리 변환 정보를 계산한다.
Letterbox letterbox(int width, int height, int model_width, int model_height);
// Three packed NHWC INT8 heads of the supplied COCO YOLOv5 model only.
// Keep the 80-class tensor layout; class 0 is person.
// 세 출력 헤드를 읽어 사람 클래스만 반환한다. 버퍼 소유권은 가져오지 않는다.
std::vector<Box> decode(const Head *heads, int model_width, int model_height,
                        float confidence, float nms_threshold);
// 상자를 원본 좌표로 제자리 변환하고 유효한 면적이면 true를 반환한다.
bool map_to_frame(Box &box, const Letterbox &transform, int width, int height);
} // namespace person
