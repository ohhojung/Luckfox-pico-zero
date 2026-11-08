# 제조사 YOLO 추론 소스를 이용한 사진 비교

## 목적과 출처

우리 사진 프로그램도 새 프로세스 반복의 9번째에 rknn_run timeout이 발생했다. 카메라/RTSP/작업 스레드/동일 컨텍스트 재사용이 필수 조건이 아니므로 공통 추론 구현과 실행 환경을 더 분리한다.

이 패키지는 공식 사진 예제 완제품이 아니다. 프로젝트에서 작성한 사진 입력 진입점에, 사용자의 기존 VM에 있는 아래 소스를 수정 없이 직접 빌드한다.

- `example/luckfox_pico_rtsp_yolov5/src/yolov5.cc`: 모델 초기화·추론·해제
- `example/luckfox_pico_rtsp_yolov5/src/postprocess.cc`: 공식 후처리
- 제조사 `yolov5.h` 및 RKNN 헤더

[공식 저장소](https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/tree/kernel-5.10.160)

src/photo_main.cc의 파일 읽기, 사진 크기 기준 BGR letterbox, 반복 제어와 오류 로그는 프로젝트 추가 코드다. 공식 main.cc의 검정 여백/정수 절삭/입력 memcpy 흐름을 참고했다. 우리 NpuModel/person_decode/LatestFrame, 카메라/AIQ/VI/VENC/RTSP는 사용하지 않는다. 공식 소스의 저작권·라이선스 표시는 원본에 그대로 남으며 이 패키지는 그 소스를 복제하지 않는다.

## VM에서 빌드

기존 프로젝트와 다른 폴더에 압축을 푼다. vendor_photo_compare 폴더에서:

```sh
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

SDK 기본 경로는 /home/student/luckfox-live/luckfox-pico이며 LUCKFOX_SDK_PATH로 변경할 수 있다. 기존 uClibc 교차 컴파일러와 축소 OpenCV highgui 이미지 IO 탐색을 사용한다. imgcodecs 검사 실패 후 highgui 검사가 성공하면 정상이다. VM에 yolov5.h가 없다면 빌드를 중단하며 다른 헤더를 임의로 만들지 않는다.

dist에는 vendor_person_image, model/coco_80_labels_list.txt, vendor_manifest.txt가 생성된다. vendor_manifest.txt는 실제 VM에서 빌드한 공식 소스·헤더·RKNN 라이브러리 경로와 SHA256이다. 모델과 런타임 파일은 포함하거나 교체하지 않는다.

## 사용자가 보드에 전송

VM의 dist를 Windows로 가져오고 그 폴더에서:

```bat
adb push vendor_person_image /oem/usr/bin/cctv_person_npu_demo/vendor_person_image
adb push model/coco_80_labels_list.txt /oem/usr/bin/cctv_person_npu_demo/model/coco_80_labels_list.txt
```

보드에서는 기존 테스트/카메라 앱이 종료된 상태로 실행한다. 원인 비교에는 이전 실패 상태 영향을 줄이기 위해 재부팅한 뒤 rkipc를 종료하고 ps로 종료 완료를 확인한 조건을 권장한다. 재부팅이나 프로세스 종료는 자동 수행하지 않는다. 기존 모델 yolov5.rknn과 person.jpg를 사용한다.

```sh
cd /oem/usr/bin/cctv_person_npu_demo
chmod +x vendor_person_image
cat /proc/uptime
./vendor_person_image model/yolov5.rknn person.jpg 20 1000
cat /proc/uptime
dmesg | grep RKNPU | tail -n 40
```

20은 추론 횟수, 1000은 시작 시점 기준 간격(ms)이다. 모델과 입출력 버퍼는 한 번 초기화하고 재사용한다. 이미지 전처리도 한 번만 수행하고 매회 같은 바이트를 복사한다. 성공 시 VENDOR_PASS가 출력되고, 모두 성공하면 VENDOR_DONE completed=20이 나온다. 첫 오류에서 중단하며 자동 재시도하지 않는다. Ctrl+C도 지원하지만 실행 중인 동기 RKNN 호출의 복귀는 기다려야 한다.

fnv1a32는 CPU 전처리 입력 바이트의 간단한 비교값이다. NPU가 실제 읽은 메모리의 검증이나 암호학적 무결성 보증이 아니다. run_post_ms는 입력 memcpy·추론·공식 후처리 시간으로 기존 detect_ms와 측정 범위가 다르다. 사진/박스 결과 파일은 저장하지 않는다. 이 프로그램도 OpenCV 이미지 IO를 쓰므로 그 하드웨어 경로까지 배제한 시험은 아니다.

person_ge_0_5는 공식 후처리 결과에서 사람 클래스/점수0.5 이상만 센 값이다. 공식 후처리의 임계값/NMS와 우리 후처리가 다를 수 있어 검출 개수를 정확도 비교로 해석하지 않는다. 이번 목적은 rknn_run 완료 여부다.

## 결과 해석과 한계

- 공식 추론 경로에서도 timeout: 우리 NpuModel/후처리 구현만이 필수 원인은 아니다. 같은 모델·런타임·드라이버·입력/보드 환경을 더 조사한다. 하드웨어 불량이 확정되는 것은 아니다.
- 20회 성공: 해당 조건에서의 성공이며 간헐 오류 배제나 장시간 안정성 증명은 아니다. 이후 동일 조건으로 우리 사진 프로그램과 교차 비교한다.

현재 패키지는 로컬 참조 소스와 API 흐름 검토 및 ZIP 무결성 확인까지 완료했다. 전체 ARM 빌드·실제 보드 실행은 아직 수행하지 않았다. 빌드 또는 실행 실패가 발생하면 해당 로그와 vendor_manifest.txt를 보관한다. 사용자 보드/VM에 자동 접속하는 도구는 포함하지 않는다.
