# VTS3750 / 18fps 시험

2026-11-14: 사용자 요청으로 두 출력 목표와 GOP를 18로 변경했다. 서비스는 -F 18을 사용한다.
기본 코덱은 H.265. 이전 -F 30 인자는 새 버전에서 거부된다.

AIQ 값 12를 무조건 복사하지 않는다. 현재 AIQ 보고 fps × frame_lines / 3750으로 요청값을 계산한다.
우리 이전 로그 15×4500이면 요청18, 팀장님 유형의 12×3750이면 요청12이다.
이는 같은 센서 모드/라인 주기가 유지된다는 가정이며, 실제 frame_lines=3750 여부를 검증해야 한다.
시작 약5초 후 양쪽 프레임을 확인하고 타이밍 요청을 적용한다. 처음부터18fps 보장은 아니다.
현재 출력18fps 설정만으로 센서18fps나 균일 간격을 증명하지 않는다.

Windows PowerShell:

```powershell
adb push "D:\Projects\Luckfox\packages\i2-board-20261114-vts3750\cctv-dual-rtsp" /tmp/cctv-dual-rtsp-vts3750
```

VLC 종료 후 보드에서 기존 S59cctv가 있으면 stop한다. rkipc도 동시에 실행하면 안 된다.

```sh
/etc/init.d/S59cctv stop
ps | grep -E '[r]kipc|[c]ctv-dual'
chmod +x /tmp/cctv-dual-rtsp-vts3750
/tmp/cctv-dual-rtsp-vts3750 -a /oem/usr/share/iqfiles -F 18
```

기존 서비스가 없는 보드는 stop 오류 대신 ps로 영상 프로세스 부재를 확인하고 진행한다.
READY 두 행의 target_fps=18, AIQ_FPS의 target_vts=3750,
후속 AIQ_TIMING의 실제 frame_lines=3750 및 STREAM 약18fps를 확인한다.
줄 수가 다르면 로그를 보관하고 센서/V4L2 설정을 추가로 변경하지 않는다.
VLC로 실제 움직임·재접속·15분 이상 재생을 확인한다. 균일성은 PTS 간격 추가 측정이 필요하다.

시험 후 Ctrl+C로 종료한다. 기존 서비스가 설치돼 있었다면 S59cctv start로 복귀한다.
영구 설치에는 새 바이너리와 새 S59cctv를 함께 사용해야 한다. 기존 설치에 덮어쓰는 자동 업그레이드는 제공하지 않는다.
보드에 이미 적용됐다고 가정하지 않는다.