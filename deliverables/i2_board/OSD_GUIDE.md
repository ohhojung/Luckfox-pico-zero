# 2026-11-16 한국 시간 자동 적용

최신 패키지: packages/i2-board-20261116-osd-kst. S59cctv가 TZ=KST-9를 export한 뒤 실행한다. 시스템 시계를 변경하지 않는다. 사용자가 동일 TZ로 날짜 표시 정상 확인했다. 아래 기본 localtime 설명보다 서비스 TZ 설정이 우선한다.

# 날짜·시간 OSD

2026-11-15: 팀장님 BoardServer 7d1a857의 streamer/src/osd.c를 우리 수명주기에 맞춰 이식했다.
양쪽 영상 왼쪽 위에 YYYY-MM-DD HH:MM:SS를 연노랑 글자·검정 테두리로 표시한다.
1080p는 배율4, 540p는 배율2. 초 단위 갱신이며 H264/H265 모두 VENC에 오버레이를 붙인다.
보드 localtime 기준으로 /etc/TZ 또는 프로세스 TZ를 따른다. 현재 날짜가 틀리면 OSD도 틀리므로 먼저 date와 date -u로 확인한다.
시간 동기화나 시간대를 자동 변경하지 않는다. 초기 1970 등 미동기화 시각도 그대로 보일 수 있다.

실행 기본값은 OSD 켜짐이다. 비교 시험에서만 끄려면:

```sh
CCTV_OSD=0 /tmp/cctv-dual-rtsp-osd -a /oem/usr/share/iqfiles -F 18 -c h264
```

일반 시험:

```sh
/tmp/cctv-dual-rtsp-osd -a /oem/usr/share/iqfiles -F 18 -c h264
```

기존 S59cctv를 중지한 뒤 시험한다. 서버 시험은 -c h264를 생략해 H265로 한다.
새 패키지는 packages/i2-board-20261115-osd. 기존 설치의 자동 업그레이드용 install.sh가 아니므로 현재 파일을 백업하고 교체한다.
OSD 실패는 stderr에 기록하고 영상은 유지한다. 종료/재초기화 시 갱신 스레드를 먼저 종료한 다음 RGN을 해제하고 VENC를 정리한다.
OSD 시각이 움직여도 배경의 실제 카메라 장면이 움직이는지는 별도로 확인해야 한다.
보드에서 양쪽 글자 위치·색·초 갱신·18fps 유지·재시작을 검증해야 한다.

heartbeat JSON 형식은 mac/status/rtsp_ok/wg_ok/config_version 다섯 필드를 유지한다.
rtsp_ok는 영상 프로세스 기준, wg_ok는 IFF_UP 기준이며 프레임 건강도는 S59cctv health로 별도 확인한다.