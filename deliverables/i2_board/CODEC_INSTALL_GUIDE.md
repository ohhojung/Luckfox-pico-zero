# 설치 미리보기와 코덱 전환 (2026-11-13)

## 현재 적용 범위

새 패키지: `packages/i2-board-20261113-codec/` 및 같은 이름의 tar.gz.
기본 H.265, 메인 1920×1080 / 서브 960×540, 출력 목표 각 20fps이다.
`-F 30`은 센서 요청이며 출력 목표 30fps를 뜻하지 않는다.
이번 수정은 로컬 소스·빌드·패키지에 적용되며 보드 설치나 재부팅을 자동 수행하지 않는다.
H.264의 실제 보드 재생과 장시간 안정성은 별도 검증해야 한다.

## 설치 전 미리보기

아래 명령은 새 패키지를 보드에 풀고 해당 폴더에서 실행한다.

```sh
sh install.sh --dry-run
```

코덱 및 네트워크 보정을 포함한 계획:

```sh
sh install.sh --dry-run --codec h264 --fix-network
```

`--dry-run`은 체크섬과 옵션을 확인하고 변경 계획·일부 설치 차단 조건을 출력한다.
파일 생성·수정, 서비스 중지·시작, 네트워크 재시작은 하지 않는다.
모든 실제 설치 사전 검사를 대체하지 않으며, BLOCKER가 표시되면 실제 설치가 가능하다는 뜻이 아니다.
옵션은 하이픈 두 개의 `--dry-run`이다. `-dry-run`, 알 수 없는 옵션, 잘못된 코덱은 실패한다.
**구버전 install.sh는 dry-run을 지원하지 않으므로 해당 인자를 붙여 실행하지 않는다.**

## 새 보드 설치

VLC를 종료하고 rkipc/cctv 프로세스를 정상 종료한 상태에서 진행한다.

```sh
sh install.sh --codec h265
```

선택적 보정까지 적용하려면:

```sh
sh install.sh --codec h265 --fix-network
```

`--fix-network`는 factory INI와 `/userdata/rkipc.ini`의 `[network.ntp] enable`만 0으로 설정하고,
`/etc/dhcpcd.conf` 전역에 `denyinterfaces wg*`를 추가한다. 시스템 NTP 전체를 끄는 옵션이 아니다.
WireGuard 키·주소·Endpoint는 바꾸지 않고, 네트워크 서비스를 즉시 재시작하지 않는다.
예상한 파일이나 NTP 섹션이 없으면 중단한다. 네트워크 보정 파일도 설치 백업에 포함된다.
기존 rkipc INI의 `[video.0]`, `[video.1]` 코덱은 바꾸지 않는다. 우리 프로그램은 해당 설정을 읽지 않는다.

설치 출력의 BACKUP 경로를 기록한 후 수동 재부팅하고 영상·heartbeat·네트워크를 확인한다.

## 이미 설치된 보드

`/userdata/cctv-i2.enabled`가 있으면 기존 정책대로 전체 재설치를 거부한다.
기존 백업을 덮어쓰지 않기 위한 제한이며, 이번 버전은 자동 업그레이드 설치를 제공하지 않는다.
현재 정상 서비스가 돌아가는 보드에는 우선 `/tmp`에서 새 실행 파일을 시험한다.

Windows PowerShell에서:

```powershell
adb push "D:\Projects\Luckfox\packages\i2-board-20261113-codec\cctv-dual-rtsp" /tmp/cctv-dual-rtsp-codec
```

보드에서 한 줄씩:

```sh
/etc/init.d/S59cctv stop
chmod +x /tmp/cctv-dual-rtsp-codec
/tmp/cctv-dual-rtsp-codec -a /oem/usr/share/iqfiles -F 30 -c h264
```

VLC에서 보드의 Wi-Fi 또는 VPN IP로 `/live/0`, `/live/1`을 각각 확인한다.
로그 두 READY 행이 모두 `codec=H264`인지 확인하고 영상이 움직이는지 확인한다.
H.265 비교는 Ctrl+C로 종료한 후 동일 명령의 `-c h264`를 `-c h265`로 바꿔 실행한다.
시험 종료 후 Ctrl+C, `/etc/init.d/S59cctv start`로 기존 서비스를 다시 시작한다.

전체 재설치는 기록된 이전 백업의 `rollback.sh`로 복구하고 재부팅한 후 새 설치 절차를 따른다.
현재 보드가 정상이라면 검증 없이 전체 재설치할 필요는 없다.

## 새 버전 설치 후 서비스 코덱 변경

새 바이너리와 새 S59cctv 둘 다 설치된 경우에만 아래 명령을 사용한다.

```sh
/etc/init.d/S59cctv stop
printf 'h264\n' > /etc/cctv-codec
/etc/init.d/S59cctv start
```

H.265로 돌아갈 때는 같은 순서에서 `h264`를 `h265`로 바꾼다.
`/etc/cctv-codec`가 없으면 H.265가 기본이며, 유효하지 않은 값이면 시작을 거부한다.
이 설정은 두 채널의 인코더와 RTSP SDP에 함께 적용된다. VLC도 재연결해야 한다.
H.265를 기대하는 서버 중계·웹 재생과의 호환성은 코덱 변경 후 별도 확인한다.

## 복구

설치 출력의 실제 BACKUP 경로를 사용한다.

```sh
sh /userdata/cctv-i2-backup-실제값/rollback.sh
```

새 서비스 중지 후 기존 실행 파일·스크립트·코덱 설정을 복구한다.
`--fix-network`를 썼다면 저장된 INI·dhcpcd 설정도 복구한다. 이후 수동 재부팅한다.

## 진단용 원본 저장

`CCTV_DUMP_DIR`에 기존의 빈 디렉터리를 지정하면 RTSP 전송 전 압축 데이터를 저장한다.
채널당 최대 200프레임 또는 16MiB이며 파일 확장자는 선택 코덱에 맞춘다.
기존 파일을 덮어쓰지 않고 프로세스 내 복구 시에도 새로 녹화하지 않는다.
변수를 지정하지 않으면 저장하지 않는다. 재생 카운터 증가만으로 영상 내용의 변화를 보장할 수는 없다.
