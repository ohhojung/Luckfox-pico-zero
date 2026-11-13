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
