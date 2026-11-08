# CCTV 사람 인식 전달 자료

Luckfox Pico Zero + IMX415에서 YOLOv5/RKNN 사람 검출과 H.265 스트리밍을 통합한 소스 및 검증 자료다. 사진 검출은 성공했지만 실시간 카메라 입력의 NPU 오류는 아직 해결되지 않았다. 정상 동작 완료본으로 전달하는 자료가 아니다.

## 먼저 볼 파일

| 파일 | 내용 |
|---|---|
| [STATUS.md](STATUS.md) | 실측 결과, enabled/disabled 의미, 문제점과 검증 한계 |
| [code/README.md](code/README.md) | 코드 구조, 옵션, CMake와 빌드 방법 |
| [code/SOURCE_MAP.md](code/SOURCE_MAP.md) | 공식 예제 참조 범위와 프로젝트 고유 구현 |
| [FIXED_IMAGE_TEST.md](FIXED_IMAGE_TEST.md) | 고정 사진 비교 결과와 새 카메라 미초기화 시험 절차 |
| [THREAD_REPEAT_TEST.md](THREAD_REPEAT_TEST.md) | 같은 바이너리의 주 스레드·작업 스레드 반복 추론 비교 |
| code/src, code/include | 주석을 포함한 C++ 구현 |
| code/model/yolov5.rknn | 로컬 공식 예제와 동일한 모델 |

## 현재 구조

```text
IMX415 → ISP/VI ┬→ VI0 1920×1080 NV12 → H.265 인코더 → RTSP
               └→ AI VI 640×360 → BGR 복사 → 최신 프레임 1개 → NPU → 사람 로그
```

영상은 추론 완료를 기다리지 않도록 분리했고, NPU 실행 실패 후에도 인코더 결과 회수가 약 15fps로 지속되는 로그를 확인했다. 다만 실제 VLC 수신 상태와 장시간 안정성은 이 결과만으로 확인할 수 없다. 송출 영상에 사람 박스는 표시하지 않는다.

사진 추론은 약 98~106ms에 23~24개 검출이 확인되었다. 실시간 프로그램에서는 NPU 작업 실패가 재현되어 원인을 조사 중이다. ai=enabled는 추론 성공이 아니라 활성 상태를 뜻한다.

고정 사진 입력 -P 및 -S -P도 보드에서 timeout이 확인되었다. 카메라를 시작하지 않는 -N -P도 최소 12회 성공 후 timeout이 발생했고, 직후 별도 사진은 성공했다. 새 -N -M -P는 같은 모델·버퍼를 재사용하며 주 스레드에서 반복 추론한다. 새 -M 소스의 ARM 빌드와 보드 시험은 아직 필요하다. 진단 모듈·펌웨어·보드 실행파일·개발용 테스트 도구는 포함하지 않는다. 모델을 새로 학습하거나 변환한 작업은 아니다.
