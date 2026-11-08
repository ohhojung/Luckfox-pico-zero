# Luckfox CCTV 작업 폴더

정리일: 2026-11-05. 장비: Luckfox Pico Zero + IMX415.
작업 재개 시 [프로젝트 기록](CCTV_PROJECT_CONTEXT.md)의 최신 날짜를 먼저 확인한다.
폴더명이나 ZIP 존재는 보드 검증 완료를 뜻하지 않는다.

## 현재 작업과 전달 파일

| 용도 | 편집할 폴더 또는 파일 | 전달용 압축 |
| --- | --- | --- |
| 사람 인식 / RTSP | [deliverables/CCTV_person_detection](deliverables/CCTV_person_detection/) | [CCTV_person_detection.zip](deliverables/CCTV_person_detection.zip) |
| heartbeat (libcurl) | [deliverables/i1_heartbeat_offline/i1_heartbeat](deliverables/i1_heartbeat_offline/i1_heartbeat/) | [i1_heartbeat_offline.zip](deliverables/i1_heartbeat_offline.zip) |
| 제조사 사진 추론 비교 | [deliverables/vendor_photo_compare](deliverables/vendor_photo_compare/) | [vendor_photo_compare.zip](deliverables/vendor_photo_compare.zip) |

편집은 위 폴더에서 한다. ZIP은 전달 당시 스냅샷이며 소스를 고친 뒤 자동 갱신되지 않는다.
각 소스의 README에 ARM/uClibc 교차 빌드 방법과 필요한 SDK·라이브러리가 있다. 보드·서버 연동 시험과 WireGuard 부팅 스크립트 이력, NPU 반복 추론 상태는 프로젝트 기록을 확인한다.

## Git 저장소 범위

Git에는 전달 소스·테스트·문서와 [별도 시험 소스](packages/current-source/)를 담는다. 카메라 빌드에 필요한 `yolov5.rknn` 모델도 함께 관리한다. 아래의 전달 ZIP, 장비 실행파일·펌웨어, 제조사 참고본, 외부에서 받은 PDF, 원시 진단 로그, 백업은 기존 작업 폴더의 로컬 자료다. 새로 복제한 저장소에서 빌드할 때는 각 소스의 README에 나온 Luckfox SDK와 제조사 라이브러리를 별도로 준비한다.

## 자료 위치

| 폴더 / 문서 | 내용 |
| --- | --- |
| [docs](docs/) | 보관 SRS, I1 보드 작업 문서, 실패 검토, 파일 정리 기록 |
| [CCTV_NPU_IMPLEMENTATION.md](CCTV_NPU_IMPLEMENTATION.md) | NPU 구현 설명 (작성 당시 상태) |
| [packages/current-source](packages/current-source/) | ISP FPS v6, IMX415, NPU 드라이버 진단 등 별도 시험 소스 |
| [packages/archive](packages/archive/) | 이전 소스 ZIP. ISP FPS v1–v5는 isp-fps/에 보관 |
| [artifacts/board](artifacts/board/) | 받은 보드 실행파일·커널 모듈. 최신 소스와 일치한다고 가정하지 않음 |
| [diagnostics](diagnostics/) | 날짜별 시험 로그·검증 코드. 루트의 result.jpg는 imported-images/로 이동 |
| [references/driver-source](references/driver-source/) | 받은 드라이버·AIQ 원본 압축 |
| [references/review-workspace](references/review-workspace/) | 이전 tmp/의 추출본·PDF 렌더·회로도·기타 검토 자료 |
| [hardware-models](hardware-models/) | 기구 STEP 및 보드 크기 자료 |
| [backups](backups/) | 복구용 및 과거 수정 전 백업. 보존 |
| [IMAGES](IMAGES/) | 기존 펌웨어 이미지. 적용 대상으로 자동 선택하지 않음 |

## 경로를 유지한 참고 프로젝트

- `example/`, `simple_test/`: 제조사 예제와 기존 산출물. 내부 빌드 경로를 유지했다.
- `luckfox_pico_rtsp_yolov5/`: 기존 YOLOv5 참고 예제.
- `cctv_npu_output_fix_complete/`: 이전 병렬 NPU 작업본. 현재 전달본과 혼동하지 않는다.
- `profile-webapp/`: 별도 웹앱 자료. 내용과 위치를 유지했다.

## 정리 기록

[2026-11-05 변경 기록](docs/organization-2026-11-05/changes.json)에 이동 전후 경로·SHA256과 삭제 사유를 기록했다.
전달 폴더의 이중 압축 해제 경로를 줄이고, 구버전 ZIP·흩어진 바이너리·검토 자료를 분류했다.
동일한 NPU 진단 소스 중복 4개와 Python 캐시 1개만 삭제했다.
과거 프로젝트 기록·백업의 경로 표기는 당시 이력으로 보존한다. 현재 위치는 이 안내서와 변경 기록을 따른다.
