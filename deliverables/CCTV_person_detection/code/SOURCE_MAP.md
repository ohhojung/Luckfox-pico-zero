# 공식 예제에서 참고한 부분과 직접 구현한 부분

이 문서는 각 파일이 어떤 예제를 참고했는지 설명합니다. 예제 전체를 그대로 복사한 코드는 아닙니다. API 호출 순서와 계산식을 참고했고, 사람 검출과 독립 영상 송출에 맞게 구조를 변경했습니다. 공식 예제와 성능·결과가 같다는 의미는 아닙니다.

## 참고한 예제 한눈에 보기

| 예제 파일 | 참고한 역할 |
|---|---|
| YOLO 예제 `yolov5.cc` | 모델 초기화, NPU 메모리 연결, 추론, 자원 해제 |
| YOLO 예제 `postprocess.cc` | NPU 출력 해석, 검출 점수 계산, 중복 박스 제거 |
| YOLO 예제 `main.cc` | 입력 크기 조정과 여백 처리, 좌표 복원, 결과 표시 |
| YOLO 예제 `luckfox_mpi.cc` | 카메라 장치·채널과 인코더 설정 |
| RTSP 예제 `simple_vi_bind_venc_rtsp.c` | ISP 시작, 카메라와 인코더 직접 연결, RTSP 전송 |

여기서 ‘YOLO 예제’는 `luckfox_pico_rtsp_yolov5/src` 폴더의 예제를 뜻합니다. 아래의 ‘우리 파일’은 전달본 `code` 폴더 기준입니다.

## 우리 파일별 설명

### 카메라와 송출 — `src/main.cc`

**참고:** YOLO 예제 `luckfox_mpi.cc`의 `vi_dev_init`, `vi_chn_init`, `venc_init`과 RTSP 예제의 `SIMPLE_COMM_ISP_Init`, `SIMPLE_COMM_ISP_Run`, `test_venc_init`, `GetMediaBuffer0`, `main`.

카메라 장치·채널을 설정하는 순서와 ISP 시작 절차를 참고했습니다. 주 영상은 RTSP 예제처럼 VI에서 VENC로 직접 연결하고, 인코딩 결과를 받아 RTSP로 전송한 후 반환합니다.

**직접 구성한 부분:** 1080p 주 채널과 640×360 AI 채널, 송출·추론 스레드 분리, 최신 프레임 전달, AI 오류 후 송출 유지, 진단 옵션입니다. 공식 YOLO 예제는 한 반복문에서 카메라 입력과 추론을 처리하므로 우리 구조와 다릅니다. `-P` 고정 사진 입력도 프로젝트에서 추가한 기능입니다.

### 모델 실행 — `src/npu_model.cc`

**참고:** YOLO 예제 `yolov5.cc`의 `init_yolov5_model`, `inference_yolov5_model`, `release_yolov5_model`과 `main.cc`의 `letterbox`.

입출력 속성을 조회하고 메모리를 할당·연결한 뒤 동기 방식으로 추론하는 흐름을 참고했습니다. 입력은 BGR 영상의 비율을 유지해 크기를 조절하고 검정 여백을 채웁니다.

**직접 구성한 부분:** 모델과 메모리를 관리하는 클래스, 지원 모델 검사, 행 간격을 고려한 입력 복사, 오류 처리입니다.

### 사람 검출 결과 처리 — `src/person_decode.cc`

**참고:** YOLO 예제 `postprocess.cc`의 `process_i8_rv1106`, `deqnt_affine_to_f32`, `CalculateOverlap`, `nms`, `post_process`와 `main.cc`의 `mapCoordinates`.

정수 출력을 실제 값으로 복원하는 식, 앵커를 이용한 박스 계산, 겹치는 박스를 제거하는 NMS, 원본 영상 좌표 복원을 참고했습니다. 원본과 같이 IoU 면적 계산에 폭·높이 +1을 적용하고, NMS 후 좌표를 제한·정수화합니다.

**직접 구성한 부분:** 사람 클래스만 선택하고 최대 128개로 제한합니다. 원본의 정렬 인덱스 대신 `std::sort`와 박스 목록을 사용하며, 영상 경계 제한과 면적이 없는 박스 제외 처리를 추가했습니다. 점수가 같은 박스의 선택 순서까지 원본과 같다고 보장하지 않습니다.

### 사진 시험 — `src/image_main.cc`

**참고:** YOLO 예제 `main.cc`의 추론 결과 표시 역할과 `rectangle`, `putText` 사용 방식.

**직접 구성한 부분:** 사진을 읽고 추론한 뒤 결과 파일을 저장하는 별도 실행 프로그램입니다. 파일 읽기·쓰기 진입점을 공식 실시간 예제에서 그대로 가져온 것은 아닙니다. 모델 실행과 후처리는 우리 공통 코드를 사용합니다.

### 최신 프레임 전달 — `include/latest_frame.h`

**직접 구현:** 공식 순차 예제에 대응하는 구조가 없습니다. 대기 프레임을 한 개만 보관하고, 새 프레임이 오면 처리 전의 이전 프레임을 교체합니다. 추론 중인 프레임은 별도로 보유하며 잠금을 해제한 상태에서 추론합니다.

### 자료구조와 빌드 설정

`include/npu_model.h`, `include/person_decode.h`는 모델·메모리·검출 박스·여백 정보를 표현하는 프로젝트용 선언입니다. 데이터의 역할은 위 모델 실행 및 후처리 예제를 참고했습니다.

`CMakeLists.txt`와 `build.sh`는 프로젝트용 빌드 구성입니다. 외부 SDK의 ARM/uClibc 헤더·라이브러리를 연결하고 OpenCV 이미지 입출력 지원을 검사해 두 실행 파일을 만듭니다. Makefile은 CMake가 선택한 생성기에 따라 자동 생성되며 직접 관리하지 않습니다.

## 예제와 비교할 때 주의할 차이

- 카메라 입력은 NV12→BGR 변환을 명시합니다. 원본의 `COLOR_YUV420sp2BGR`와 동일한 호출은 아닙니다.
- 주 영상은 추론을 거치지 않고 인코더로 전달합니다. 검출 박스는 로그에만 표시합니다.
- 지원 모델은 입력 640×640, 출력 80/40/20 격자와 255채널 형태로 제한합니다.
- 현재 신뢰도 기본값은 0.5, NMS 기준은 0.45, 사람 최대 개수는 128입니다. 원본 관련 헤더가 로컬에 없어 모든 기본 상수가 같다고 단정하지 않습니다.
- 오류 검사와 부분 초기화 정리를 추가했습니다. 동기 NPU 호출은 Ctrl+C로 즉시 취소되지 않을 수 있습니다.

## 참고 파일 경로와 대조 정보

공식 예제 URL은 kernel-5.10.160 브랜치 기준이며 특정 커밋을 확정한 링크가 아닙니다. 실제 대조한 로컬 파일은 아래 해시로 식별합니다. 외부 소스 사본은 전달본 크기를 줄이기 위해 포함하지 않습니다.

- **YOLO 예제의 yolov5.cc**
  - 경로: `luckfox_pico_rtsp_yolov5/src/yolov5.cc`
  - 참고 함수: `init_yolov5_model / inference_yolov5_model / release_yolov5_model`
  - SHA256: `e0bdeae11890505675d205b9104d63c737117020c270a4f065439de6d266fd49`
  - https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/yolov5.cc
- **YOLO 예제의 postprocess.cc**
  - 경로: `luckfox_pico_rtsp_yolov5/src/postprocess.cc`
  - 참고 함수: `anchor / process_i8_rv1106 / deqnt_affine_to_f32 / CalculateOverlap / nms / post_process`
  - SHA256: `fe7a6e852d2df2a2b088302371a24e520a2fb83553b1cc5aa709b3b0f9d6b6fb`
  - https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/postprocess.cc
- **YOLO 예제의 main.cc**
  - 경로: `luckfox_pico_rtsp_yolov5/src/main.cc`
  - 참고 함수: `main / letterbox / mapCoordinates`
  - SHA256: `b292427effe5c1f14151c683e4d66473baaa5af8c97ab5dc898630977d58fead`
  - https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/main.cc
- **YOLO 예제의 luckfox_mpi.cc**
  - 경로: `luckfox_pico_rtsp_yolov5/src/luckfox_mpi.cc`
  - 참고 함수: `vi_dev_init / vi_chn_init / venc_init / TEST_COMM_GetNowUs`
  - SHA256: `90e54eb2b300c8ce78e37fe6b2f2c74e48535a5779caa2c59f53b9ea7ce054c3`
  - https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/blob/kernel-5.10.160/example/luckfox_pico_rtsp_yolov5/src/luckfox_mpi.cc
- **RTSP 예제의 simple_vi_bind_venc_rtsp.c**
  - 경로: `simple_test/simple_vi_bind_venc_rtsp.c`
  - 참고 함수: `SIMPLE_COMM_ISP_Init / SIMPLE_COMM_ISP_Run / GetMediaBuffer0 / test_venc_init / main`
  - SHA256: `8ce4aa5813d3e4c5f22baacdc9e06552a17c0713d2d39cdee150d6e48d66927f`
  - 로컬 SDK 예제. 이번 작업에서 정확한 upstream 커밋/URL을 확인하지 않아 링크를 추정하지 않음.

YOLO 예제의 yolov5.cc의 원본 헤더는 Rockchip 2023, YOLO 예제의 postprocess.cc는 Rockchip 2021 저작권과 Apache-2.0 안내를 포함합니다. YOLO 예제의 luckfox_mpi.cc는 Luckfox team 작성 표기가 있습니다. 이 출처 설명이 외부 원본의 저작권 표시를 대체하거나 프로젝트 전체의 라이선스를 새로 결정하지는 않습니다.

## 모델

포함된 model/yolov5.rknn은 로컬 공식 예제의 model/yolov5.rknn과 바이트가 동일합니다. 이 프로젝트에서 새로 학습하거나 새로 변환했다고 주장하지 않습니다. 원 학습 데이터·가중치 계보·변환 설정 전체는 소스 대조만으로 확인되지 않았습니다.

모델 SHA256: `7e72c8caa060ea15d69109e4af6c1f80005cbd1be14bbe1f6f2af9175861c44a`


## 검증 범위

기존 C++ 후처리 및 최신 프레임 대기 공간의 호스트 검사는 통과했습니다. BGR 사진 추론 성공과 통합 프로그램의 카메라 입력 NPU 실패·오류 후 영상 회수 지속을 사용자 보드 로그에서 확인했습니다. -P 소스의 ARM 빌드 및 -P/-S -P 보드 timeout을 확인했습니다. -N의 반복 성공 후 timeout을 보드에서 확인했습니다. 새 -M 옵션은 모의 모델 호스트 검사까지 완료했으며 전체 ARM 빌드·보드 시험은 아직 필요합니다. 실제 VLC 수신과 장시간 안정성은 별도 확인이 필요합니다. 자세한 결과와 한계는 상위 STATUS.md를 참조합니다.

이 전달본에는 include/src/model 및 빌드·설명 파일만 포함합니다. 테스트와 도구는 포함하지 않습니다.

고정 사진 진단(-P)은 프로젝트 추가 기능이다. main.cc의 옵션 처리·사진 로드·선택 프레임 대체·로그 구분과 CMake 이미지 IO 정의 전달을 추가했다. 공식 예제 동일 구현이 아니다.


카메라 미초기화 진단(-N)은 프로젝트 추가 기능이다. Camera.start와 VI 획득, 송출 스레드를 생략하고 단조 시계에 따라 고정 사진을 기존 슬롯에 전달한다. 모델 초기화 위치와 추론 스레드, NpuModel 구현은 유지한다. 공식 예제에서 복사한 기능이 아니다.

-M 진단은 프로젝트 추가 기능이다. 기존 추론/보고/오류 처리 코드를 InferenceRunner.step으로 공통화하여 main 또는 작업 스레드에서 호출한다. 공식 예제의 주 스레드 호출과 역할은 비슷하지만 진단 옵션·슬롯·제어 흐름을 복사한 것은 아니다.
