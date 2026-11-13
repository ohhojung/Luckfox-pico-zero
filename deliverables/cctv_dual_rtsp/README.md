# Luckfox Pico Zero H.265 이중 RTSP

IMX415 한 대를 AIQ로 초기화하고, ISP에서 나온 두 NV12 영상 경로를 각각 하드웨어 HEVC 인코더에 연결합니다. RTSP 서버 한 개가 554번 포트에서 두 세션을 동시에 제공합니다. 이번 실행 파일의 범위는 영상 송출입니다.

| 화질 | RTSP 경로 | 해상도 | CBR 기본값 | 인코더 목표 |
| --- | --- | --- | --- | --- |
| 고화질 | `/live/0` | 1920×1080 | 4096 Kbps | 20fps |
| 저화질 | `/live/1` | 960×540 | 1024 Kbps | 20fps |

heartbeat는 기본적으로 `cctv-dual-rtsp` 프로세스의 생존 여부를 `rtsp_ok`에 반영합니다. 실제 양쪽 프레임 갱신은 `S59cctv health`로 별도 확인합니다. 사람 감지·오디오·녹화는 이번 범위에서 제외합니다. 통합 설치·복구 절차는 `../i2_board/README.md`를 참고하세요.

이번 I2는 사용자 결정에 따라 두 출력 모두 20fps 기준으로 진행합니다. 센서의 고정 30fps 요청(-F 30)은 기존 약20fps ISP 출력 확보 조건으로 유지하며 인코더 입력/출력 선언은 모두20으로 맞춥니다. 정확한 실측20 유지 여부는 보드에서 검증합니다. 출력30fps 확보는 별도 과제입니다.

## Ubuntu VM에서 빌드

기존 Luckfox SDK의 Buildroot용 ARM uClibc GCC와 [RKMPI 예제의 kernel-5.10.160 브랜치](https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/tree/kernel-5.10.160)를 사용합니다.

셸에서 이 폴더로 이동한 뒤 실행합니다.

```sh
sh build.sh
```

기존 VM 경로를 기본으로 사용합니다. 경로가 다르면 `sh build.sh /RKMPI/예제/경로 /ARM/GCC/실행파일/경로` 형식으로 지정하세요. 빌드 결과는 `dist/cctv-dual-rtsp`입니다.

## 보드 및 Windows VLC에서 시험

1. 실행 파일을 Windows로 옮긴 뒤 PowerShell에서 보드에 전송합니다.

   ```powershell
   adb push .\cctv-dual-rtsp /tmp/cctv-dual-rtsp
   adb shell
   ```

2. 보드에서 기존 `rkipc`나 RTSP 데모가 켜져 있으면 기존 서비스의 종료 절차로 먼저 중지합니다. 그 뒤 아래 명령을 실행합니다. 기본 IQ 경로는 `/etc/iqfiles`이며, 보드에서 다른 경로를 쓰면 실행 인자로 `-a /oem/usr/share/iqfiles`처럼 지정할 수 있습니다.

   ```sh
   chmod +x /tmp/cctv-dual-rtsp
   ip -4 addr show dev wlan0
   /tmp/cctv-dual-rtsp
   ```

3. 보드와 같은 핫스팟에 연결한 Windows VLC에서 아래 두 URL을 각각 열고, 두 화면에서 움직임이 동시에 갱신되는지 확인합니다. `BOARD_WIFI_IP`는 실행 직전에 확인한 보드의 현재 주소를 사용합니다. 서버에서 RTSP를 당겨올 때는 서버가 접근할 수 있는 보드 VPN 주소에 같은 포트와 경로를 사용합니다.

   ```text
   rtsp://BOARD_WIFI_IP:554/live/0
   rtsp://BOARD_WIFI_IP:554/live/1
   ```

4. 보드 콘솔의 `STREAM` 로그가 5초마다 각 경로의 인코더 취득 FPS와 RTSP 전송 오류 수를 보여줍니다. 두 채널을 동시에 재생하면서 여러 구간을 비교하세요. 인코더 목표는 20fps입니다. 한 채널에서 5초간 프레임이 없으면 파이프라인을 정리하고 재초기화하며 최초 포함 최대 3회 시도합니다. 초기화·루프·정리가 멈추면 타임아웃으로 종료합니다. 커널 복구나 무한 재시작을 보장하지 않습니다. 정상 종료는 보드 콘솔에서 Ctrl+C를 누릅니다.

## 센서 30fps 요청 비교 시험

`/tmp/cctv-dual-rtsp -F 30`으로 실행하면 기존 AIQ 정책으로 시작합니다. 두 채널에서 프레임을 받은 5초 측정 구간을 `STREAM`으로 출력한 다음 AIQ 자동 노출 설정에 고정 30fps를 한 번 요청합니다. `AIQ_FPS stage=running` 이후 로그를 같은 조명의 기준 구간과 비교하세요. 옵션 없는 실행에서는 기존 AIQ 정책을 계속 사용합니다.

`AIQ_TIMING`은 AIQ가 보고한 값입니다. 송출을 켠 채 다른 보드 셸에서 `cat /proc/rkcif-mipi-lvds`의 `Output Info`에 있는 `fps`와 `rate`를 확인하고, 송출기의 `STREAM` 로그에서 두 채널의 `venc_fps`를 확인하세요. 밝기가 낮으면 고정 프레임률 요청에 따른 영상 밝기도 확인하세요.

## ISP TNR 비교 시험

`/tmp/cctv-dual-rtsp -F 30 -T`는 30fps 요청 뒤 두 스트림의 5초 실측을 한 번 출력하고, IMX415 IQ에 맞는 Bayer TNR V23 Lite 전용 API로 자동 모드의 TNR만 끄도록 요청합니다. `ISP_TNR api=v23_lite` 앞뒤의 `STREAM`을 비교하세요. 현재 실행에서만 적용하며 어두운 장면에서 노이즈가 늘 수 있습니다.

`ISP_HW`는 같은 송출기 실행에서 `/proc/rkisp-unite0`의 `BAY3D` 줄을 요청 직전과 요청 이후에 기록합니다. `BAY3D ON`에서 `OFF`로 바뀌어야 실제 비활성화가 확인됩니다. `BAY3D`가 켜져 있으면 AIQ 요청 로그만으로 비활성화를 판단할 수 없습니다.

보드의 전용 API가 `mode=0`을 반환하면 위 실행 중 변경 시험은 중단됩니다. 이 경우 보드의 현재 IMX415 IQ JSON을 PC로 가져온 뒤 `python prepare_tnr_off_iq.py <현재-IQ-JSON> <시험-출력-폴더>`로 임시 복사본을 만듭니다. 원래 이름의 시험 JSON만 보드 `/tmp/iq-tnr-off/`에 올리고 송출기를 `-a /tmp/iq-tnr-off -F 30`으로 실행합니다. 시작 로그의 IQ 경로, 실행 중 `/proc/rkisp-unite0`의 `BAY3D`, 양쪽 `STREAM`을 확인하세요. 기존 설정으로 돌아갈 때는 송출기를 종료하고 `-a` 없이 다시 실행합니다.

구현은 [Luckfox RKMPI RTSP 예제](https://wiki.luckfox.com/Luckfox-Pico-Pi/MPI/)의 인코더 취득·송출 방식과 [RV1106 rkipc 영상 코드](https://github.com/LuckfoxTECH/luckfox-pico/blob/main/project/app/rkipc/rkipc/src/rv1106_ipc/video/video.c)의 VI→VENC 채널 구성을 참고했습니다.

## 작은 센서 모드에서 AIQ 없이 FPS 측정

`-N`은 AIQ 초기화를 생략하고 두 VI·VENC 채널과 RTSP를 시작하는 시험 옵션입니다. 카메라 서비스를 중지하고 센서를 1944×1097/`0x3010`으로 설정한 뒤 `/tmp/cctv-dual-rtsp -N`을 실행합니다. `DIAGNOSTIC aiq=disabled`와 각 `STREAM`의 `venc_fps`를 확인합니다. AIQ 자동 노출과 화질 튜닝이 빠지므로 화면이 어둡거나 색이 달라질 수 있습니다. `NO_FRAME`이 나오면 해당 로그와 CIF 통계를 수집합니다.

## 2026-11-13 모듈화

기능별 소스를 src/로 분리했습니다. 기존 빌드 명령·실행 옵션은 유지합니다. 파일별 역할과 수정 지점은 저장소의 docs/MODULE_GUIDE.md를 참고하세요.
