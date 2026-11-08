# CCTV 프로젝트 작업 지침

- 이 작업공간에서 CCTV·카메라·RTSP·Wi-Fi·보드 관련 요청을 다룰 때는 먼저 `CCTV_PROJECT_CONTEXT.md`를 읽는다. 최신 날짜의 현재 상태를 우선하고 과거 이력과 구분한다.
- 사용자가 확인한 장비는 Luckfox Pico Zero + IMX415이다. 이미 기록된 모델이나 설정을 불필요하게 다시 묻지 않는다.
- 보드에 `netstat`이 없다. 일반 데스크톱 Linux 도구가 설치되어 있다고 가정하지 않는다.
- 프로젝트의 실제 검증 환경은 Wi-Fi 핫스팟 연결과 Windows VLC Media Player이다.
- 새로 확인한 중요한 상태·설정·테스트 결과는 `CCTV_PROJECT_CONTEXT.md`에 반영한다. 관측 사실과 추정을 구분하고 DHCP 주소를 고정값으로 취급하지 않는다.
