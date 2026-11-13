# i2-modular 사용 설명서

작성일: 2026-11-13 · 대상: Luckfox Pico Zero + IMX415 · 보드2 CAM-04AF

이 문서는 보드에서 이중 H.265 영상을 실행하고, heartbeat를 확인하며, 정상 동작 확인 후 부팅 자동 실행으로 설치하는 절차를 설명합니다. 명령은 한 줄씩 실행합니다. `PS ...>`와 `[root@luckfox ...]#` 같은 프롬프트는 입력하지 않습니다.

> **현재 알려진 장애(2026-11-13):** 보드에서 약20fps 프레임 생성은 확인했으나, 이후 NO_FRAME·ISP 정지 타임아웃과 세 번 재시도 후 종료가 발생했습니다. 원인과 복구 방안은 미확정이며 안정화 완료본이 아닙니다. 자세한 기록은 저장소의 deliverables/cctv_dual_rtsp/I2_DEPLOY_STATUS.md를 참고하세요.

## 1. 구성과 기본 설정

모듈화란 C 소스를 역할별 파일로 나눈 것입니다. 보드에서 실행되는 프로그램은 기존처럼 두 개이며, 각 모듈을 따로 실행하지 않습니다.

| 구성 | 하는 일 |
| --- | --- |
| cctv-dual-rtsp | 카메라 → ISP → H.265 인코더 → RTSP 메인·서브 영상 제공 |
| heartbeat | MAC·카메라 프로세스·VPN 인터페이스 상태를 서버에 5초마다 전송 |
| S59cctv | 설치된 영상 프로그램 시작·정지·상태·프레임 건강 검사 |
| S60heartbeat | 설치된 heartbeat 시작·정지·상태 확인 |
| check-health.sh | 상태 파일의 PID·시각·양쪽 프레임 갱신 검사 |
| install.sh / rollback.sh | 영구 설치·기존 파일 백업 / 이전 상태 복구 |
| patch-rklunch.awk | OEM 초기화 후 기존 rkipc 대신 새 서비스로 시작을 연결 |
| SHA256SUMS / BUILD.txt | 파일 무결성 / 빌드 정보 |

| 항목 | 현재 값 |
| --- | --- |
| 메인 | H.265, 1920×1080, 목표20fps, GOP20, CBR4096Kbps |
| 서브 | H.265, 960×540, 목표20fps, GOP20, CBR1024Kbps |
| RTSP | 포트554, /live/0 및 /live/1, 인증 설정 없음 |
| 운영 실행 옵션 | -a /oem/usr/share/iqfiles -F 30 |
| heartbeat 목적지 | http://10.0.0.100:3000/v1/devices/heartbeat |
| 전송 주기·타임아웃 | 5초·3초 |
| MAC 조회 / 송신 인터페이스 | wlan0 / wg0 |
| 보드 VPN 식별자 | 계획값10.0.1.1, MAC7424CA5004AF. 실제 VPN 연결 상태는 별도 확인 |

인코더 목표20fps와 센서 요청30은 별개입니다. 실제 프레임과 비트레이트는 실행·수신 결과로 확인합니다. 보드는 영상을 서버에 push하지 않으며, 서버 MediaMTX가 보드의 RTSP에 접속해 pull합니다. 서버 주소·로그인·VPN 구성은 이 패키지가 설치하지 않습니다.

## 2. 파일 위치와 명령 실행 장소

| 장소 | 경로 / 용도 |
| --- | --- |
| Windows 배포 폴더 | D:\Projects\Luckfox\packages\i2-board-20261113-modular |
| Windows 압축 파일 | D:\Projects\Luckfox\packages\i2-board-20261113-modular.tar.gz |
| 보드 임시 시험 폴더 | /tmp/i2-modular |
| 소스 작업 폴더 | D:\Projects\Luckfox |

- `adb` 명령: Windows PowerShell에서 실행합니다.
- `./cctv-dual-rtsp`, `sh install.sh` 등: `adb shell`로 들어간 보드 셸에서 실행합니다.
- 빌드 명령: PC의 Ubuntu/WSL에서 실행합니다. 보드 셸에서 빌드하지 않습니다.
- `/tmp`는 임시 저장소입니다. 재부팅 후 사라질 수 있으므로 필요하면 다시 전송합니다.

## 3. 임시 시험: 보드로 전송

먼저 Windows PowerShell에서 보드 연결을 확인합니다.

```powershell
adb devices
```

보드 한 대가 `device`로 표시된 상태에서 아래를 실행합니다. 여러 장치가 보이면 대상 장치부터 선택해야 합니다.

```powershell
adb push 'D:\Projects\Luckfox\packages\i2-board-20261113-modular' /tmp/i2-modular
adb shell
```

이제 보드 셸에서 실행합니다.

```sh
cd /tmp/i2-modular
ls
sha256sum -c SHA256SUMS
chmod +x cctv-dual-rtsp heartbeat
```

목록에 실행 파일과 install.sh가 있어야 하고, 체크섬은 전부 `OK`여야 합니다. 반복 전송 후 파일이 보이지 않으면 `ls /tmp/i2-modular`로 하위 폴더가 생겼는지 확인하고 실제 패키지 폴더로 이동합니다. `FAILED`가 나오면 실행·설치하지 말고 파일을 다시 전송합니다.

## 4. 기존 카메라와 충돌 해소

보드에서 실행 중인 카메라 프로그램을 확인합니다.

```sh
ps | grep -E '[r]kipc|[c]ctv-dual'
```

실행 방식에 맞는 종료 방법 하나를 사용합니다.

| 현재 실행 방식 | 종료 방법 |
| --- | --- |
| 다른 셸에서 직접 실행한 cctv-dual-rtsp | 해당 셸에서 Ctrl+C |
| 설치된 S59cctv 서비스 | 아래 서비스 stop 명령 |
| 기존 rkipc | 아래 killall 명령 |

서비스로 실행 중일 때:

```sh
/etc/init.d/S59cctv stop
```

rkipc가 실행 중일 때:

```sh
killall rkipc
```

다시 `ps` 명령으로 종료를 확인한 뒤 새 프로그램을 실행합니다. 두 송출기를 동시에 실행하면 포트554 또는 카메라 자원 충돌이 발생합니다. 종료가 안 되면 오류 로그를 확인하며, 강제 종료를 반복하지 않습니다.

## 5. heartbeat 미리 보기와 스트리밍 실행

보드의 패키지 폴더에서 한 줄씩 실행합니다.

```sh
export LD_LIBRARY_PATH=/oem/usr/lib:/oem/lib
./heartbeat --dry-run
```

예시 payload:

```json
{"mac":"7424CA5004AF","status":"online","rtsp_ok":false,"wg_ok":true,"config_version":0}
```

이 값은 예시입니다. `rtsp_ok`는 카메라 프로세스 생존 여부, `wg_ok`는 wg0의 UP 여부에 따라 바뀝니다. dry-run은 네트워크 요청을 보내지 않습니다. 따라서 dry-run 성공은 서버 연결 성공이 아닙니다.

보드의 현재 Wi-Fi IP를 확인합니다.

```sh
ip -4 addr show dev wlan0
```

영상 프로그램을 실행합니다.

```sh
./cctv-dual-rtsp -a /oem/usr/share/iqfiles -F 30
```

콘솔이 돌아오지 않고 로그가 계속 나오는 것이 정상입니다. 종료하려면 Ctrl+C를 누릅니다.

같은 Wi-Fi에 연결한 PC의 VLC 창 두 개에서 각각 아래 주소를 엽니다. `BOARD_WIFI_IP`를 방금 확인한 실제 IP로 바꿉니다. Wi-Fi IP는 DHCP 주소이므로 과거 값을 그대로 사용하지 않습니다.

```text
rtsp://BOARD_WIFI_IP:554/live/0
rtsp://BOARD_WIFI_IP:554/live/1
```

두 화면에서 움직임이 동시에 갱신되는지 60초 이상 확인합니다. VLC 코덱 정보와 통계에서 해상도·코덱·수신 상태도 확인합니다. 인코더 로그만으로 네트워크 수신·브라우저 재생 성공을 판정하지 않습니다.

### 처음에 15fps가 나오는 이유

현재 코드는 기존 AIQ 정책으로 시작한 뒤, 두 채널에서 프레임을 받은 첫 약5초 측정 구간 이후 센서에 고정30fps를 요청합니다. 첫 프레임 직후 요청하도록 변경하는 안은 논의만 했으며 아직 적용하지 않았습니다.

```text
AIQ_TIMING stage=baseline reported_fps=15.00 frame_lines=4500
STREAM path=/live/0 venc_fps=14.xx rtsp_tx_errors=0
AIQ_FPS stage=running requested=30
AIQ_TIMING stage=after_request reported_fps=30.00 frame_lines=2250
STREAM path=/live/0 venc_fps=20.xx rtsp_tx_errors=0
```

위는 이전 시험에 근거한 예상 흐름입니다. `reported_fps=30`은 AIQ 보고값이고, `venc_fps`는 인코더 프레임 취득률입니다. 매 순간20.00fps를 보장하는 것은 아닙니다. 10~15초 후에도 baseline만 보이거나 오류가 발생하면 이후 로그를 함께 확인합니다. `-F 20`은 현재 지원하지 않습니다. `-F`를 생략하면 고정 센서FPS 요청을 하지 않습니다.

### 로그 읽는 법

| 로그 | 의미 |
| --- | --- |
| READY | 스트림 구성 완료. 실제 영상 수신 성공을 뜻하지 않음 |
| STREAM ... venc_fps | 약5초 구간의 인코더 취득 FPS |
| rtsp_tx_errors=0 | 해당 구간에서 집계한 전송 호출 오류가0. 클라이언트 시청 여부는 별개 |
| NO_FRAME | 해당 채널에서5초간 프레임을 받지 못함 |
| RECOVERY | 정리 후 파이프라인 재시도. 최초 실행 포함 최대3회 |

## 6. 영구 설치와 재부팅

임시 시험에서 양쪽 영상이 정상인지 먼저 확인합니다. 아래 설치는 보드의 기존 프로그램·부팅 스크립트를 변경합니다. 단순 실행 시험만 하려면 이 절차는 진행하지 않아도 됩니다.

1. 수동 스트리밍을 Ctrl+C로 종료합니다.
2. 4절처럼 기존 카메라 프로세스가 모두 종료됐는지 확인합니다.
3. 보드 패키지 폴더에서 설치합니다.

```sh
cd /tmp/i2-modular
sh install.sh
```

`BACKUP=/userdata/cctv-i2-backup-XXXXXX`로 출력된 실제 경로를 기록합니다. XXXXXX는 예시이며 매번 다릅니다.

설치 스크립트는 다음을 수행합니다.

- 패키지 체크섬·프로세스·지원하는 RkLunch 구조를 검사합니다.
- 기존 RkLunch.sh, 카메라/heartbeat 실행 파일과 서비스, 선택 설정 파일을 백업합니다.
- 기존 heartbeat 서비스를 정지하고 새 파일을 설치합니다.
- 기존 `/etc/heartbeat.conf`가 있으면 백업 후 제거하여 새 서비스 기본값을 사용합니다.
- S21appinit의 OEM 초기화 기능을 유지하고, RkLunch의 두 rkipc 시작 지점에 새 서비스 분기를 추가합니다.
- `/userdata/cctv-i2.enabled`로 전환을 표시합니다. OEM 초기화가 끝나면 `/tmp/cctv-oem-ready`를 만들고 S59cctv를 시작합니다.

설치가 성공했다면 수동으로 재부팅합니다.

```sh
reboot
```

다시 연결한 뒤 아래 명령을 한 줄씩 실행합니다.

```sh
/etc/init.d/S59cctv status
/etc/init.d/S59cctv health
cat /tmp/cctv-dual-rtsp.status
grep STREAM /tmp/cctv-dual-rtsp.log
/etc/init.d/S60heartbeat status
logread | grep i1-heartbeat
```

`status`는 프로세스 생존, `health`는 PID와 최근 양쪽 프레임 갱신을 확인합니다. 부팅 직후에는 초기화 중일 수 있으므로 잠시 뒤 다시 확인합니다. 실제 VLC 재생도 다시 확인해야 합니다.

`Already enabled`가 나오면 설치 스크립트를 반복 실행하지 않습니다. 이 설치기는 기존 활성 설치를 바로 덮어쓰는 업데이트 도구가 아닙니다. 보관한 이전 백업으로 복구·재부팅한 뒤 새 설치를 진행하는 방식이며, 업데이트할 때에는 복구하려는 기준 버전을 먼저 확인합니다.

## 7. 서비스 관리와 서버 연동 확인

아래 명령은 영구 설치 이후에만 사용합니다. 목적에 맞는 명령 하나를 실행합니다.

```sh
/etc/init.d/S59cctv start
/etc/init.d/S59cctv stop
/etc/init.d/S59cctv restart
/etc/init.d/S59cctv health
/etc/init.d/S60heartbeat restart
/etc/init.d/S60heartbeat status
```

`start → stop → restart`를 시험 순서처럼 모두 실행할 필요는 없습니다. 상태 파일과 영상 로그는 `/tmp`에 있고, 서비스 영상 로그는 약1MiB를 넘으면 잘라냅니다. 장시간 시험 증거가 필요하면 별도 보관해야 합니다. 무프레임 재시도와 타임아웃은 커널 장애 복구나 무한 재시작을 보장하지 않습니다.

서버 연결은 보드에서 다음을 확인합니다.

```sh
wg show wg0
ip route get 10.0.0.100
logread | grep i1-heartbeat
```

WireGuard 출력에서 최근 handshake와 전송량을 확인합니다. PrivateKey 또는 설정 파일 전체를 공유하지 않습니다. `wg_ok=true`만으로 handshake 성공을 판단하지 않습니다. heartbeat는 wg0로 송신을 제한하므로 wg0가 없으면 Wi-Fi 경로로 우회하지 않습니다.

로그의 `HTTP_200`은 서버 응답 성공입니다. `HTTP_403`이면 서버의 보드VPN IP/MAC 등록과 wlan0 MAC을 대조합니다. 타임아웃은 서버 실행·VPN·라우팅을 확인합니다. 서버에서 수신할 보드2 주소는 계획상 아래와 같습니다.

```text
rtsp://10.0.1.1:554/live/0
rtsp://10.0.1.1:554/live/1
```

MediaMTX 설정, 웹 로그인, MAC 등록, WebRTC 재생, HTTP503 원인 해결은 이 패키지 외의 서버·웹 통합 작업입니다. 보드 실행 성공과 전체 I2 완료를 구분합니다.

## 8. 복구

설치 실패 또는 이전 버전 복귀 시, 설치 중 출력된 실제 백업 경로를 사용합니다. 아래 XXXXXX를 그대로 입력하면 안 됩니다.

```sh
sh /userdata/cctv-i2-backup-XXXXXX/rollback.sh
reboot
```

백업 디렉터리 안의 rollback.sh가 새 서비스를 정지하고 이전 파일을 복원하며 전환 표시를 제거합니다. 새로 설치되어 이전에는 없던 대상 파일은 삭제합니다. 백업 파일은 삭제하지 않습니다. 재부팅 후 이전 카메라·heartbeat 상태를 확인합니다. 설치가 실패했다고 무조건 재부팅하지 말고, 백업 경로와 실패 지점을 먼저 확인합니다.

## 9. 소스 수정과 빌드

### 수정할 파일

저장소 루트는 `D:\Projects\Luckfox`입니다.

| 변경 목적 | 소스 위치 |
| --- | --- |
| 두 스트림 해상도·FPS·비트레이트 | deliverables/cctv_dual_rtsp/src/config.c |
| 카메라·포트·센서 요청 상수, 옵션 타입 | deliverables/cctv_dual_rtsp/src/config.h |
| 초기화·프레임 처리·정리 순서 | deliverables/cctv_dual_rtsp/src/pipeline.c |
| 센서·노출·IQ·TNR | deliverables/cctv_dual_rtsp/src/isp.c |
| VI·VENC·버퍼·프레임 취득 | deliverables/cctv_dual_rtsp/src/video.c |
| RTSP 세션 구성 | deliverables/cctv_dual_rtsp/src/rtsp_server.c |
| 상태 기록 / 공통 실행 지원 | deliverables/cctv_dual_rtsp/src/status.c / runtime.c |
| heartbeat 기본값 | deliverables/i1_heartbeat_offline/i1_heartbeat/src/config.h |
| heartbeat CLI·전송 주기 루프 | 같은 src/heartbeat.c |
| 보드 상태 조회 / HTTP / JSON | 같은 src/platform.c / transport.c / protocol.c |
| 서비스 실행 옵션 | deliverables/cctv_dual_rtsp/S59cctv 및 deliverables/i1_heartbeat_offline/i1_heartbeat/S60heartbeat |

S60heartbeat가 CLI 옵션을 넘기므로 heartbeat config.h만 수정하면 서비스 실행값이 바뀌지 않을 수 있습니다. 해상도·센서값 변경은 실제 하드웨어 검증이 필요합니다. 새 .c 파일은 src에, 테스트 .c 파일은 tests에 둡니다. build.sh는 src/*.c를 함께 빌드합니다. 구조 설명은 저장소의 docs/MODULE_GUIDE.md를 참고합니다.

### 현재 PC의 WSL 빌드 예시

아래는 이미 준비된 D 드라이브의 ARM 도구·SDK 라이브러리·curl staging을 사용하는 명령입니다. 다른 PC에서는 실제 경로에 맞춰야 합니다. WSL Ubuntu 안에서 한 줄씩 실행합니다.

```sh
cd /mnt/d/Projects/Luckfox
build_root="$PWD/artifacts/i2-build"
arm_cc="$build_root/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc"
sh deliverables/cctv_dual_rtsp/build.sh "$build_root/vendor/luckfox_pico_rkmpi_example-kernel-5.10.160" "$arm_cc"
CC="$arm_cc" CURL_PREFIX="$build_root/curl-staging" sh deliverables/i1_heartbeat_offline/i1_heartbeat/build.sh
```

ARM용 curl 헤더·라이브러리를 사용해야 합니다. Ubuntu 호스트의 /usr/lib를 ARM 링크 경로로 지정하지 않습니다.

로컬 회귀 검증:

```sh
sh deliverables/cctv_dual_rtsp/tests/run_local.sh
sh deliverables/i1_heartbeat_offline/i1_heartbeat/tests/run_offline.sh
sh deliverables/i1_heartbeat_offline/i1_heartbeat/build.sh --native
python3 deliverables/i2_board/test_local.py deliverables/i1_heartbeat_offline/i1_heartbeat/build/native/heartbeat
```

검증 후 패키지를 다시 만듭니다.

```sh
sh deliverables/i2_board/package.sh
```

생성 위치는 packages/i2-board-20261113-modular 및 같은 이름의 tar.gz입니다. 패키지 생성은 기존 ARM dist 파일을 복사하는 작업이므로 반드시 ARM 빌드가 먼저 성공해야 합니다. 소스만 바꾸거나 native만 빌드한 뒤 패키지를 만들면 새 ARM 코드가 반영되지 않습니다. 파일 내용을 수동 변경하면 SHA256SUMS도 달라지므로 package.sh로 다시 생성합니다.

## 10. 자주 발생하는 문제

| 증상 | 확인 순서 |
| --- | --- |
| Address already in use | rkipc 또는 다른 RTSP 송출기가 포트554를 사용하는지 확인 |
| 처음 약15fps | baseline 구간인지, 이후 requested=30 및 after_request가 나오는지 확인 |
| 계속15fps | 실행 인자 -F30 여부와 이후 AIQ 오류 로그 확인. 센서 형식을 임의 변경하지 않음 |
| NO_FRAME / RECOVERY | 해당 경로·AIQ/VI/VENC 로그 확인. 반복 실패는 무한 재시작하지 않음 |
| cctv frames not healthy | 프로세스 상태와 상태 파일 PID·양쪽 프레임·로그 확인 |
| cctv deferred until OEM initialization completes | OEM 초기화 완료 표시가 없는 상태. RkLunch 초기화 오류를 확인하며 표시 파일을 수동 생성해 건너뛰지 않음 |
| Another service command is running | 다른 start/stop 진행 여부 확인. 진행 중 잠금 디렉터리를 삭제하지 않음 |
| MAC_UNAVAILABLE | wlan0 존재와 주소 확인 |
| heartbeat HTTP403 | 실제 VPN IP·wlan0 MAC과 서버 등록 대조 |
| 웹 HTTP503 | 서버/API 연결 점검. 로그인 오류만으로 보드 인코더를 변경하지 않음 |
| Unrecognized RkLunch / Refusing symlink | 설치기가 예상과 다른 보드 구성을 감지한 것. 스크립트를 억지로 통과시키지 말고 실제 구성을 대조 |
| /tmp 실행 파일 없음 | 재부팅으로 삭제됐을 수 있음. PowerShell에서 다시 전송 |

`-N`은 AIQ를 생략하는 진단 옵션, `-T`는 TNR 진단 옵션입니다. 정상 운영 명령에는 사용하지 않습니다. 현재 확인된 센서 모드는3864×2192 SGBRG10이며, 작은 입력 모드로 바꾸는 시험은 이 설명서 범위가 아닙니다.

## 11. 검증 상태와 완료 체크리스트

- [x] 모듈화 스트리밍 ARM 빌드, 실행 옵션 회귀 검사
- [x] heartbeat native·ARM 빌드, 프로토콜·루프백 HTTP·상태 검사
- [x] 이전 바이너리: 보드 로그에서 약2분간 두 채널20.14~20.58fps 및 정상 정리 확인
- [x] 모듈화 버전: 사용자 제공 baseline 로그에서 두 경로 초기 프레임 생성 확인
- [ ] 모듈화 버전: 설정 적용 이후 양쪽 약20fps 로그와 VLC 동시 수신 확인
- [ ] 영구 설치·재부팅 후 자동 실행 및 영상 확인
- [ ] 실서버 heartbeat200·DB last_seen·웹 online/offline 확인
- [ ] VPN RTSP → MediaMTX → 웹 H.265 재생 확인
- [ ] 전원 차단·복구 후 새로고침 없는 재생 복귀

단계별 결과를 구분해서 기록합니다. 사람 인식·오디오·녹화·마스킹과30fps 확보는 이번 패키지 범위에 포함하지 않습니다. 기존 시험 패키지 packages/i2-board-20261112와 모듈화 전 백업 artifacts/refactor-20261113은 별도로 보존되어 있습니다.
