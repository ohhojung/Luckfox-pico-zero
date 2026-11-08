# Pico Zero + IMX415 NPU 사람 감지

## 2026-09-20 동시 실행 시간 초과 분리 진단

`./cctv_person_npu -D -f 5 -t 0.5`는 카메라 두 VI 채널과 NPU를 유지하면서 VENC 생성·바인딩 및 RTSP 서버/송출을 생략한다.
1920×1080 메인 VI는 depth=1로 받아 즉시 반환하고, 기존 640×360 AI 채널은 동일한 전처리·추론 경로를 사용한다.
센서 해상도, 클록, 프레임률 제어 설정은 변경하지 않는다. 이 모드는 VLC 영상이 없으며 `READY DIAGNOSTIC`, `CAMERA main_fps`, `PERSON` 로그로 판정한다.
기존 rkipc나 다른 카메라 프로그램을 종료한 상태에서만 시험한다. 정상 동작하면 20초 정도 로그를 확인한 뒤 Ctrl+C로 종료한다.
추론 timeout이면 종료 직후 `dmesg | tail -n 60`을 수집한다. 이 진단 변경은 소스 검토만 수행했으며 ARM 빌드·보드 실행은 아직 미검증이다.
성공하면 인코더/바인딩/추가 메모리 부하 쪽을 우선 조사하되 VENC 단독 원인으로 확정하지 않는다. 메인 VI 소비 방식도 변경되기 때문이다.
실패하면 VENC·RTSP 없이도 문제가 재현된 것이지만 ISP 하드웨어 고장으로 확정할 수는 없다.
`-D` 없이 실행하면 기존 스트리밍 경로를 사용한다. 이 진단 모드는 수정 완료된 제품 기능이 아니라 원인 분리 도구다.

후속 검증: 사용자 VM에서 ARM 빌드 성공. 보드 runtime 1.6.0/driver 0.9.2에서 모델 로딩 성공 후 출력 할당 크기 검사에 실패했다.
해당 검사는 size_with_stride >= size를 허용하도록 수정했으며 후처리에는 논리 size만 전달한다.
여유 버퍼 영역을 포함한 PC 후처리 회귀 테스트는 통과했으나, 수정본 보드 추론은 아직 미검증이다.

2026-09-17: 사용자 제공 `rknn_guide.pdf` 6쪽 전체를 확인해 구성한 버전.
**Windows에서 후처리 테스트는 통과했지만, VM/보드 직접 접속·RKNN 모델 변환·ARM 빌드·NPU 실측은 아직 하지 않았다.**
Visual Studio SSH 작업공간에 자동으로 반영된 파일이 아니다.

## 가이드에 맞춘 실행 순서

1. **Ubuntu VM**: 현재 RKNN-Toolkit2·교차 컴파일러·공식 라이브러리 경로 확인.
2. **Ubuntu VM**: Rockchip YOLOv5 ONNX를 `rv1106` 대상 INT8 RKNN으로 변환하거나, 호환성 미확인인 기존 RKNN 후보를 단일 이미지로 먼저 검증.
3. **Ubuntu VM**: ARMv7/uClibc 실행 파일 교차 빌드.
4. **Pico Zero**: `cctv_person_image`로 사람 포함 이미지 한 장을 추론해 모델·런타임·후처리 검증.
5. **Pico Zero**: 이미지 추론이 성공한 뒤 `cctv_person_npu`로 IMX415 + RTSP 연동 확인.

가이드 §5.4~5.6의 C API·4D INT8·입출력 조회·메모리 바인딩 흐름과 §6의 rv1106/armv7l 대상을 따른다.
가이드의 Python RKNN은 PC 모델 변환용이고, 보드 실행 파일은 C++에서 RKNN C API를 호출한다.
Framebuffer 테스트는 화면 장치 데모용이며, 이 프로젝트의 터미널·VLC 검증에는 필요하지 않다.
문서의 `killall`, 설치 명령, 권한 변경 명령을 자동 실행하지 않는다.

## VM 환경 확인

이 `cctv_person_npu` 폴더 전체를 VM의 `/home/student/cctv_person_npu`로 옮긴 뒤:

```sh
cd /home/student/cctv_person_npu
sh tools/check_vm.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

이 스크립트는 조회만 한다. Toolkit이나 SDK를 설치하거나 변경하지 않는다.
확인된 것은 VM 홈 `/home/student` 및 공식 RKMPI 예제 경로뿐이며 **전체 SDK 경로와 Toolkit 버전은 아직 미확인**이다.
가이드의 Toolkit 1.6.0은 설치 예시다. 보드의 librknnmrt/드라이버와 맞는 버전을 확인하기 전에 최신 버전으로 임의 업그레이드하지 않는다.

## RV1106용 모델 변환 (VM)

가이드 §4.5의 Rockchip YOLOv5 `--rknpu` 내보내기 또는 §6.1의 공식 model zoo ONNX를 사용한다.
일반 Ultralytics ONNX의 단일 decoded/NMS 출력은 이 후처리와 다르다.
본 변환기는 고정 `[1,3,640,640]` ONNX 입력과 sigmoid가 적용된 3개 COCO head를 검사한다.
각 head는 NCHW `[1,255,80,80]`, `[1,255,40,40]`, `[1,255,20,20]` 또는 대응 NHWC여야 한다.

`calibration.txt`에는 실제 설치 환경을 대표하는 이미지 경로를 한 줄에 하나씩 적는다.
상대 경로는 목록 파일이 있는 폴더 기준이며, Toolkit 처리를 위해 경로에는 공백이 없어야 한다.
밝기·거리·사람 크기가 다양한 이미지를 사용한다. 단일 사진으로 정확도가 검증됐다고 판단하지 않는다.

```sh
# 실제 설치된 Toolkit2 환경을 활성화한 Ubuntu VM에서 실행한다.
python tools/convert_rv1106.py \
  --onnx /실제/경로/yolov5s.onnx \
  --dataset /실제/경로/calibration.txt \
  --output model/person_rv1106.rknn
```

`--check-only`를 추가하면 ONNX와 이미지 경로만 검사한다. ONNX 검사에는 `onnx` 패키지가 필요하다.
변환은 `target_platform=rv1106`, `asymmetric_quantized-8`, `do_quantization=True`, RGB mean=0/std=255로 고정한다.
성공하면 Toolkit 버전·대상·SHA256·미검증 상태를 기록한 같은 이름의 JSON도 생성한다.
기존 모델/메타데이터를 덮어쓰지 않는다.

**현재 번들에 있는 `model/yolov5.rknn`은 기존 폴더에서 복사한 후보이며 이번에 새로 변환한 파일이 아니다.**
SHA256: `7e72c8caa060ea15d69109e4af6c1f80005cbd1be14bbe1f6f2af9175861c44a`.
원본 ONNX·캘리브레이션 이미지·VM Toolkit이 제공되지 않아 새 RKNN을 생성하지 않았다.
이 후보의 생성 Toolkit/대상은 현재 미확인이고, 보드 단일 이미지 테스트로 호환성을 먼저 판단한다.

모델 내부 native INT8과 입력 API의 UINT8 RGB는 구분한다. 공식 RV1106 예제처럼
`rknn_set_io_mem`에 UINT8을 지정해 런타임이 정규화·양자화를 수행한다.
이미지 바이트를 INT8로 단순 캐스팅하거나 CPU에서 255로 중복 정규화하지 않는다.

## 동작

- VI 채널 0 → VENC 하드웨어 바인딩 → H.264 1920×1080, 기본 4096 Kbps → RTSP `/live/0`.
- 별도 VI 채널 1의 NV12 640×360 → CPU RGB 변환·letterbox → RV1106 NPU의 YOLOv5 → COCO `person`(class 0)만 출력.
- 기본 최대 5회/초 AI 입력. 영상 송출, AI 입력 수집, NPU 추론을 분리한다. 대기 영상은 최신 1개만 유지하여 추론이 느릴 때 큐가 누적되지 않게 한다.
- 이 버전은 사람 수·신뢰도·1080p 기준 좌표를 **터미널에 출력**한다. VLC 영상에는 박스를 그리지 않는다. 특정 인물 식별이나 얼굴 인식이 아닌 사람 객체 감지다.
- `detect_ms`는 전처리·NPU·후처리 소요 시간, `result_age_ms`는 프로그램이 VI 프레임을 받은 뒤 결과가 나온 시간이다. 센서→VLC 전체 지연이나 NPU 연산만의 시간이 아니다.
- 모델은 80클래스 출력을 유지하고 person만 필터링한다. `OBJ_CLASS_NUM`을 1로 바꾸지 않는다. 새로 변환한 모델은 빌드의 `MODEL_FILE` 또는 실행의 `-m`으로 선택한다.
- 모델 출력은 640×640 입력/3개 packed NHWC INT8 COCO YOLOv5 head만 지원한다. 맞지 않으면 오류로 종료한다. CPU 추론으로 대체하지 않는다.
- 스트림 FPS는 센서·ISP 설정에 따른다. `-f 5`는 AI 샘플링 제한이며 영상 5fps 설정이 아니다. 30fps 보장이나 SRS 충족 선언은 보드 측정 후 한다.

## VM에서 빌드

사용자 확인 공식 예제 경로: `/home/student/RKMPI/luckfox_pico_rkmpi_example`.
이 폴더 전체를 예를 들어 `/home/student/cctv_person_npu`로 복사한다.
완전한 공식 예제의 헤더, `lib/uclibc` 라이브러리, OpenCV와 보드 펌웨어에 맞는 **Linux ARM uClibc 교차 컴파일러**가 필요하다.

```sh
cd /home/student/cctv_person_npu
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

컴파일러는 명시적 접두사, 가이드의 `GCC_COMPILER`, `LUCKFOX_SDK_PATH` 아래 표준 SDK 경로, PATH 순서로 찾는다.
`LUCKFOX_SDK_PATH`는 RKMPI 예제 폴더가 아니라 전체 SDK 경로여야 한다.
컴파일러를 못 찾으면 실제 컴파일러 경로에서 마지막 `gcc`/`g++`만 제외한 접두사를 두 번째 인자로 전달한다.

```sh
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example /실제/도구체인/bin/arm-rockchip830-linux-uclibcgnueabihf-
```

자동 다운로드·보드 프로세스 종료·펌웨어 라이브러리 덮어쓰기는 하지 않는다.
완료 시 `dist/`에 이미지/스트리밍 실행 파일과 선택한 모델이 `model/yolov5.rknn` 이름으로 설치된다.
공식 예제의 kernel/라이브러리 버전이 보드와 다르면 빌드 또는 로딩 오류를 먼저 해결해야 한다.

새 모델을 선택하거나, 먼저 카메라 라이브러리가 필요 없는 이미지 실행 파일만 빌드할 수 있다.

```sh
MODEL_FILE="$PWD/model/person_rv1106.rknn" BUILD_STREAMING=OFF \
  sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
# 이미지 추론이 검증되면 BUILD_STREAMING=ON으로 다시 빌드
```

`GCC_COMPILER`는 가이드처럼 끝의 하이픈 없이 지정해도 된다.
`OpenCV core/imgproc`, `librknnmrt`는 필수다. 헤더는 vendor의 `include`, `include/opencv4`도 탐색한다.
이미지 테스트의 imread/imwrite는 imgcodecs 또는 highgui 헤더로 컴파일·링크 가능 여부를 검사한다.
이미지 입출력이 없으면 경고와 함께 이미지 테스트만 제외하고 스트리밍 타깃을 빌드한다.
이 경우 단일 이미지 검증이 완료된 것이 아니며 `build/CMakeFiles/CMakeError.log`로 원인을 확인해야 한다.
`BUILD_STREAMING=OFF`로 이미지 테스트만 요청했는데 이미지 입출력이 없으면 오류로 종료한다.
OpenCVConfig의 이전 설치 경로 경고가 남더라도 별도 헤더 탐색에 성공하면 계속 진행한다.

## 보드에서 이미지 한 장 검증

`dist/`와 사람이 포함된 테스트 이미지 `person.jpg`를 보드의 작업 폴더에 복사한다.

```sh
cd /oem/usr/bin/cctv_person_npu_demo
chmod +x cctv_person_image
./cctv_person_image model/yolov5.rknn person.jpg result.jpg 0.5
```

카메라·ISP·RTSP를 열지 않으며, 터미널 출력과 녹색 박스가 그려진 `result.jpg`를 생성한다.
빈 장면과 여러 사람이 있는 사진으로도 확인한다. `rknn_init` 실패는 모델/런타임/메모리 문제를 먼저 확인한다.
출력에 RKNN runtime·driver 버전과 native 입출력 타입·shape·stride·양자화 값이 표시된다.
특정 출력 배치가 지원되지 않으면 안전하게 실패한다. 임의로 4D/INT8 검사만 제거하지 않는다.

## 보드 실행

기존 `simple_vi_bind_venc_rtsp`와 `rkipc`가 카메라를 점유하지 않도록 정상 종료한 뒤 실행한다.
이번 온도 비교가 끝나기 전에는 기존 스트리밍을 자동으로 종료하지 않는다.
`dist` 전체를 보드 `/oem/usr/bin/cctv_person_npu_demo`로 복사한 경우:

```sh
cd /oem/usr/bin/cctv_person_npu_demo
chmod +x cctv_person_npu
./cctv_person_npu -b 4096 -f 5 -t 0.5
```

VLC: `rtsp://현재_보드_Wi-Fi_IP:554/live/0`, 캐시 비교 기준 200ms.
실행 파일 기준 상대 위치에서 모델을 찾으므로 실행 디렉터리가 달라도 된다.
다른 경로의 모델은 `-m /경로/yolov5.rknn`, IQ는 `-a /etc/iqfiles`로 지정한다.
보드에서 검증되지 않은 두 번째 VI 채널이 초기화되지 않으면 **첫 오류와 시작 로그를 확인**한다.
`-I 2`로 ISP bypass 채널을 선택할 수도 있으나, 해당 채널의 640×360 출력 지원을 별도로 검증해야 한다.
`libopencv*` 등이 없다는 로딩 오류는 보드와 같은 ABI의 라이브러리를 확인한 뒤 패키지의 `lib/`에 넣어 해결한다. 시스템 라이브러리를 임의 교체하지 않는다.

기대하는 로그 형식(실측 결과가 아닌 예시):

```text
NPU ready: RGB 640x640, COCO person class=0, INT8 output
PERSON count=1 pts_us=... detect_ms=... result_age_ms=...
  person score=0.9... box1080=(...,...,...,...)
VIDEO fps=... (encoder output; not end-to-end latency)
```

## 검증 순서

### 카메라 동시 실행 실패 분리 시험 (2026-09-23)

`-D`는 두 VI 채널을 유지하고 VENC/RTSP만 제외한다.
`-S`는 AI용 VI 채널(기본 1, 640x360)만 열고 메인 VI 및 VENC/RTSP를 제외한다.
센서 입력 해상도, ISP 모드, 모델과 추론 코드는 변경하지 않는다.
단일 AI 채널 구동 지원과 NPU 동시 실행 성공은 보드 검증이 필요하다.

```sh
./cctv_person_npu -S -f 5 -t 0.5
```

시작 시 `READY DIAGNOSTIC SINGLE`이 표시되어야 한다. VLC 영상과 main_fps 출력은 없다.
`PERSON count=0`도 추론 완료 결과다. READY만으로 추론 성공으로 판단하지 않는다.
실패 시 최초 오류와 종료 직후 `dmesg | tail -n 100`을 확인한다.
`-S`에서 성공해도 메모리/채널 구성이 함께 바뀌므로 메인 VI 자체 결함으로 단정하지 않는다.

1. 위 이미지 추론이 통과한 뒤 시작 로그에서 IMX415, 올바른 IQ 경로, NPU 초기화, 두 VI 채널 및 RTSP 시작 확인.
2. 빈 장면과 사람이 들어왔다 나간 장면에서 count/좌표 변화를 확인. 여러 사람이 있으면 각각 감지되는지 확인.
3. 4Mbps 기준 기존 스트리밍과 비교해 VIDEO fps·체감 지연·끊김을 확인. AI 부하가 높으면 `-f 2`로 입력 빈도를 낮춰 비교.
4. 이후 검출 박스 영상 표시, 이벤트 중복 억제, 사람 감지 이벤트 저장·서버 연동을 추가한다. 이 버전에서는 미구현.

호스트에서 보드 없이 후처리 검증:

```sh
cmake -S . -B build-host -DHOST_TESTS_ONLY=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

## 참고

- [Luckfox 공식 RKMPI 예제](https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example)
- 사용자 제공 `C:/Users/ohhoj/OneDrive/바탕 화면/rknn_guide.pdf`, 특히 p.5-6.
- [Rockchip RV1106 C API 예제](https://github.com/airockchip/rknn_model_zoo/blob/main/examples/yolov5/cpp/rknpu2/yolov5_rv1106_1103.cc)
- [Rockchip 모델 변환 예제](https://github.com/airockchip/rknn_model_zoo/blob/main/examples/yolov5/python/convert.py)
- 기존 워크스페이스 `simple_test/simple_vi_bind_venc_rtsp.c`와 `luckfox_pico_rtsp_yolov5`의 RKNN zero-copy 흐름을 참고했다.
- 현재 Windows 작업공간에는 공식 공통 헤더·ARM 라이브러리·교차 컴파일러가 없어, 호스트 테스트 통과를 보드 빌드나 추론 성공으로 해석하지 않는다.
