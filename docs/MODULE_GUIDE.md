# 보드 프로그램 모듈 안내

2026-11-13: 기존 실행 동작을 유지하며 소스를 기능별로 분리했습니다. 보드에서 실행되는 프로그램은 여전히 스트리밍과 heartbeat 두 개입니다. 모듈은 컴파일 때 하나의 실행 파일로 연결되며 보드에 .c/.h 파일을 배포할 필요는 없습니다.

## 스트리밍: deliverables/cctv_dual_rtsp/src

| 파일 | 책임 | 수정하는 경우 |
| --- | --- | --- |
| config.c / config.h | 두 스트림 기본값, 실행 옵션 파싱 | 해상도·비트레이트·출력FPS·RTSP 경로 변경 |
| main.c | 프로세스 잠금, 최대3회 재시도 | 프로그램 실행·재시도 정책 변경 |
| pipeline.c / pipeline.h | 초기화→프레임 처리→정리의 실행 순서 | 기능을 실행 흐름에 연결 |
| isp.c / isp.h | AIQ 초기화·종료, 센서FPS 요청, TNR 진단 | 센서·노출·IQ 관련 수정 |
| video.c / video.h | VI/VENC 연결, 인코더 프레임 취득·전달, 자원 해제 | 인코더·버퍼·프레임 처리 변경 |
| rtsp_server.c / rtsp_server.h | RTSP 영상 세션·타임스탬프 설정 | 세션 설정 변경 |
| status.c / status.h | 원자적 상태 파일 기록 | 서비스 상태 항목 변경 |
| runtime.c / runtime.h | 종료 신호, 단조시계, 오류 출력, 로그 제한 | 공통 실행 지원 변경 |

main → pipeline → config / isp / video / rtsp_server / status 순서로 책임을 나눴습니다. runtime은 공통 보조 모듈입니다. 스트림별 런타임 상태는 video_stream에 있고, 매 재시도마다 새로 초기화합니다. 모든 RTSP 호출은 기존처럼 pipeline의 한 스레드에서 실행합니다. video.c는 취득한 패킷을 전송하고 즉시 VENC에 반환합니다.

기본 출력은 H.265 1080p·540p 각20fps입니다. 센서 요청30은 인코더 출력20과 별개입니다. config.c의 프로파일 값을 바꿔도 하드웨어가 그 FPS를 낸다는 보장은 없습니다. 센서FPS 요청은 기존과 같이 첫 프레임 측정 이후에 적용합니다. 오류가 발생한 초기화도 같은 역순 정리 경로를 통과합니다.

## Heartbeat: deliverables/i1_heartbeat_offline/i1_heartbeat/src

| 파일 | 책임 |
| --- | --- |
| config.h | 목적지·포트·인터페이스·프로세스·주기 기본값 |
| heartbeat.c | CLI 인자 검사, 5초 실행 루프, HTTP 결과 처리 |
| platform.c / platform.h | Linux MAC·WG UP·카메라 프로세스 조회 |
| transport.c / transport.h | libcurl POST, 인터페이스 바인딩, 응답 수신·시간 제한 |
| protocol.c / protocol.h | 기존 JSON·MAC 정규화·응답 파싱 |
| runtime.c / runtime.h | 로그, 종료 신호, 단조시계, PID 파일 잠금 |

transport는 종료 플래그를 인자로 받아 실행 루프 전역변수에 직접 의존하지 않습니다. platform과 runtime은 libcurl 헤더에 의존하지 않습니다. protocol은 기존 오프라인 테스트를 그대로 사용합니다.

기본값 변경 시 주의: S60heartbeat가 명령행 인자로 값을 전달하므로 서비스 운영 기본값은 S60heartbeat도 함께 확인해야 합니다. 선택적인 heartbeat.conf가 있으면 서비스 값을 덮어쓸 수 있습니다. rtsp_ok는 프로세스 생존이며 영상 건강 검사와 다릅니다.

## 빌드·테스트

기존 build.sh 명령과 실행 파일 이름을 유지했습니다. build.sh가 src/*.c를 함께 빌드하므로 새 .c 모듈도 자동 포함됩니다. 테스트 전용 .c는 tests에 둡니다. 헤더에는 선언과 타입만 두고 구현 파일을 #include하지 않습니다.

- 스트리밍 ARM: sh deliverables/cctv_dual_rtsp/build.sh VENDOR_DIR ARM_GCC
- 스트리밍 옵션 회귀: sh deliverables/cctv_dual_rtsp/tests/run_local.sh
- heartbeat native: sh deliverables/i1_heartbeat_offline/i1_heartbeat/build.sh --native
- heartbeat ARM: CC=ARM_GCC CURL_PREFIX=TARGET_CURL_PREFIX sh deliverables/i1_heartbeat_offline/i1_heartbeat/build.sh
- 프로토콜: sh deliverables/i1_heartbeat_offline/i1_heartbeat/tests/run_offline.sh
- HTTP·상태 검사: python3 deliverables/i2_board/test_local.py HEARTBEAT_BINARY

이전 소스·바이너리 백업은 artifacts/refactor-20261113, 기존 보드 시험 패키지는 packages/i2-board-20261112입니다. 모듈화 빌드는 새 packages/i2-board-20261113-modular에 만듭니다. 이전 보드의 약2분20fps 로그는 이전 바이너리의 결과이며 새 빌드의 보드 검증으로 대체할 수 없습니다.

## 이번 변경 검증 결과

스트리밍 ARM 빌드 및 옵션·프로파일 회귀 검사, heartbeat native/ARM 빌드·프로토콜·루프백 HTTP 테스트, 상태 검사, 패키지 체크섬 검사를 통과했습니다. 새 빌드의 보드 영상 시험은 아직 수행하지 않았습니다.
