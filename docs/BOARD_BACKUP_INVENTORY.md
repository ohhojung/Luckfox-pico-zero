# 보드 실제 용량 재확인 — 2026-11-14

사용자가 제공한 df -k 및 du -sk 출력 기준이다. 이번 명령이 조회한 백업 8개 합계는 365 KiB이며 보드 전체를 재귀 검색한 총량은 아니다.

| 경로 | 사용량 KiB |
|---|---:|
| /userdata/cctv-i2-backup-ocSNaW | 50 |
| /userdata/cctv-upgrade-backup-rBHOpy | 181 |
| /userdata/heartbeat-backup-20261109-090040 | 38 |
| /userdata/rkipc-clock-backup.qzDH9v | 35 |
| /userdata/rkipc.ini.bak | 17 |
| /root/wifi-backup-20261105-012713 | 12 |
| /etc/iqfiles.bak | 4 |
| /oem/usr/ko/imx415.ko.before-mode30 | 28 |

교체 직전 백업 rBHOpy의 디렉터리 존재와 사용량이 확인됐다. 내부 파일 목록/해시는 아직 확인하지 않았으므로 완전한 복구 가능성까지 입증한 것은 아니다. 새 버전 영구 설치·재부팅 검증 전 보관한다.

| 마운트 | 사용률 | Available KiB | 약 MiB |
|---|---:|---:|---:|
| / | 6% | 5536652 | 5406.9 |
| /userdata | 26% | 172930 | 168.9 |
| /oem | 9% | 440940 | 430.6 |
| /tmp (tmpfs) | 2% | 73200 | 71.5 |

현재 출력에 용량 부족은 없다. 조회한 /userdata 백업 합계는 321 KiB이며 전체 365 KiB를 삭제해도 약0.36 MiB에 불과하므로 지금은 보관 권장.
/heartbeat.conf와 /etc/wireguard/wg0.conf.bak-*는 이번 du 결과에 출력되지 않았다. 2>/dev/null로 오류가 숨겨져 있으므로 부재 확정 대신 존재 미확인으로 기록한다.
보드 파일 삭제나 이동은 하지 않았다. 아래의 생성 미확인/과거 용량 문구는 당시 이력이며 이번 관측을 우선한다.

---
# 최신 정리 — 2026-11-14

이 목록은 과거 보드 조회와 사용자가 보낸 출력으로 확인한 기록이다. 오늘 보드 전체를 재조회한 목록은 아니므로 현재 존재·크기·누락 여부는 별도 확인이 필요하다. 이번 작업에서 보드 파일은 삭제하지 않았다.

## 11월 9일 이후 추가 항목

| 경로 | 확인 상태 | 용도·보관 기준 |
|---|---|---|
| /userdata/cctv-i2-backup-ocSNaW/ | 11월13일 사용자 ls로 존재 확인 | I2 설치 이전 RkLunch, 존재했던 영상·heartbeat 실행 파일 및 서비스·설정과 rollback.sh. 실제 포함 파일은 ls로 확인 필요. 원래 부팅 동작 복구용으로 보관. |
| /userdata/cctv-i2-backup-*/ | 추가 생성 여부 미확인 | install.sh가 실제 설치마다 생성. ocSNaW 외 항목은 결과를 받지 못했으므로 있다고 단정하지 않음. |
| /userdata/cctv-upgrade-backup-XXXXXX/ | 생성 명령만 안내, 결과 미수신 | VTS3750 교체 직전 cctv-dual-rtsp, S59cctv, cctv-codec 또는 codec-was-absent를 저장하는 용도. XXXXXX는 실제 이름이 아님. 성공 확인 전 교체 진행하지 않음. |
| /etc/wireguard/wg0.conf.bak-날짜-시간 | 생성 명령만 안내, 결과 미수신 | 잘못 붙은 PersistentKeepalive 수정 전 백업. 개인키 포함 가능. 복구 시 잘못된 줄도 복원될 수 있으므로 그대로 덮어쓰지 않음. |

기존 install.sh는 실제로 존재한 파일만 백업한다. 백업 디렉터리 존재만으로 모든 실행 파일이 포함됐다고 판단하지 않는다.
--fix-network 사용 시 새 설치 백업 내부 network/ 및 network-files에 factory INI, userdata INI, dhcpcd.conf를 저장하지만, 우리 보드에서 해당 옵션 적용은 아직 확인되지 않았다.

## 임시 진단 파일 (복구 백업과 구분)

- /tmp/cctv-capture-20cZkZ/live-0.h265: 200프레임, 로그상 5,030,084바이트 저장 확인.
- /tmp/cctv-capture-20cZkZ/live-1.h265: 200프레임, 로그상 1,217,150바이트 저장 확인.
- /tmp/cctv-dual-rtsp-* 및 /tmp/i2-board-*: 시험 실행 파일·전송 패키지. 현재 실행 중인 파일은 정리 대상에서 제외한다.
- /tmp/aiq-gdb.log 등 진단 로그: 장애 분석에 필요하면 먼저 PC에 보관한다.
- 위 /tmp 파일은 재부팅 이후 없어졌을 수 있다. 현재 존재를 확인하지 않았으며 영구 백업으로 의존하지 않는다.

## 정리 우선순위

1. df -k / /userdata /oem /tmp 로 실제 부족한 파티션부터 확인한다.
2. /userdata/rkipc.ini.bak는 11월9일 시계 백업 내 INI와 동일했음. 현재도 동일한지 확인 후 우선 정리 후보.
3. 사용하지 않는 /heartbeat.conf, /etc/iqfiles.bak와 과거 임시 영상·패키지를 검토한다. IQ 백업은 링크 구조이므로 링크 대상 디렉터리를 삭제하지 않는다.
4. heartbeat 구버전은 새 버전 부팅·서버 수신 확인 후 정리한다.
5. 카메라 드라이버, Wi-Fi 초기화, I2 설치 이전 백업은 PC 복사본·복구 필요성을 확인하고 최소 한 세트 보관한다.
6. 가장 최근 교체 직전 백업은 새 버전 재부팅·영상·heartbeat 검증 전 삭제하지 않는다.

아래 138 KiB 합계와 파티션 여유는 11월9일의 과거 측정값이다. 현재 총량으로 사용하지 않는다.
보드 밖 D:/Projects/Luckfox/backups, artifacts/refactor-20261113 등은 별도 PC 백업이므로 이 보드 용량 합계에 넣지 않는다.

---
# Luckfox 보드 백업 목록

2026-11-09에 USB ADB로 접속한 Luckfox Pico Zero 한 대를 조회했다. `/`, `/userdata`, `/oem`, `/tmp`에서 백업 이름 패턴과 프로젝트 이력에 나온 경로를 확인했다. 크기는 `du -sk`가 보고한 디스크 사용량이며 디렉터리 자체의 사용량도 포함한다. 보드에서 백업 위치와 크기만 조회했다.

| 보드 경로 | 파티션 | 사용량 | 내용과 정리 기준 |
| --- | --- | ---: | --- |
| `/userdata/heartbeat-backup-20261109-090040/` | `/userdata` | 38 KiB | 이전 `i1-heartbeat` 실행 파일과 `S60heartbeat`. 새 버전의 부팅 자동 전송과 서버 수신을 확인할 때까지 보관. |
| `/userdata/rkipc-clock-backup.qzDH9v/` | `/userdata` | 35 KiB | 시계 변경 전 `rkipc-800w.ini`, `rkipc.ini`. 원본과 사용자 INI를 같이 복원할 때 사용. |
| `/userdata/rkipc.ini.bak` | `/userdata` | 17 KiB | 예전 사용자 INI. 이번 조회에서 `/userdata/rkipc-clock-backup.qzDH9v/rkipc.ini`와 내용이 동일함을 `cmp`로 확인. `/userdata` 정리 시 첫 후보. |
| `/oem/usr/ko/imx415.ko.before-mode30` | `/oem` | 28 KiB | 30fps 시험 전 IMX415 원본 모듈. PC에도 `backups/imx415-before-mode30-20261001/imx415.ko` 보관 이력이 있다. 정리 전 복사본의 해시 일치 여부와 복구 필요성을 확인. |
| `/root/wifi-backup-20261105-012713/` | `/` | 12 KiB | `S99hciinit`, `S99wlan0` 이전본. Wi-Fi 초기화 스크립트 복원용. |
| `/etc/iqfiles.bak/` | `/` | 4 KiB | 기존 IQ 경로 구조의 백업. 내부 항목은 `/oem/usr/share/iqfiles`를 가리키는 심볼릭 링크다. 현재 `/etc/iqfiles`도 같은 대상을 가리키므로 정리 시 정확한 `.bak` 경로를 확인. |
| `/heartbeat.conf` | `/` | 4 KiB | 루트에 남아 있는 이전 heartbeat 설정. 서비스가 읽는 `/etc/heartbeat.conf`는 없음을 확인했다. 설정을 다시 쓸 가능성이 없으면 정리 후보. |

총 7개 항목, **138 KiB**. 파티션별로 `/userdata` 90 KiB, `/oem` 28 KiB, `/` 20 KiB다.

| 파티션 | 확인 당시 사용률 | 남은 공간 |
| --- | ---: | ---: |
| `/` | 6% | 약 5.3 GiB |
| `/userdata` | 26% | 약 169 MiB |
| `/oem` | 9% | 약 431 MiB |

용량 부족이 생기면 `df -k / /userdata /oem`으로 부족한 파티션부터 확인한다. `/userdata` 용량을 확보할 때는 위 표의 `/userdata` 항목이 대상이다. 백업 전체가 138 KiB이므로 큰 용량이 필요하면 해당 파티션에서 더 큰 파일을 확인한다. `/usr/bin/tdbbackup`과 이름에 `backup`이 들어간 Samba 파일은 설치된 프로그램 구성 파일이다.
