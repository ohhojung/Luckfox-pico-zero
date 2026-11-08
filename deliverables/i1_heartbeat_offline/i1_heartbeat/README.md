# I1 heartbeat - 서버 없이 준비하는 단계

기준: 2026-11-04 첨부 보드 작업 문서의 오호종 절(p.2 하단–4).
2026-11-05 제공된 「ICD — Iteration 1 적용 범위 발췌」(작성일 2026-11-03), 1장 및 3.4절을 기준으로 요청·응답 형식을 맞췄다. 문서의 [TBR] 항목은 실제 서버와 최종 확인이 필요하다.
이번 묶음은 카메라/NPU 소스·WireGuard 키·네트워크 설정을 변경하지 않는다.

## 지금 확인할 것

1. ZIP의 `i1_heartbeat` 폴더를 VM `/home/student/i1_heartbeat`에 둔다.
2. VM에서 아래 명령으로 기존 ARM/uClibc 컴파일러를 사용한다.

```sh
cd /home/student/i1_heartbeat
sh build.sh
```

기존 컴파일러가 PATH에 없으면 `/home/student/luckfox-live/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc`를 찾는다.
다른 경로이면 `CC=/실제/컴파일러/경로 sh build.sh`로 지정한다.
2026-11-05 보드의 curl 및 /usr/lib/libcurl.so.4 런타임 존재를 확인하여 HTTP 전송을 libcurl로 변경했다. 빌드에는 **대상 ARM/uClibc용 curl 헤더와 링크 라이브러리**가 필요하다. Ubuntu 호스트용 libcurl 개발 패키지를 ARM 링크에 사용하면 안 된다.

컴파일러의 기본 검색 경로에 없다면 SDK에서 먼저 찾는다:

```sh
find /home/student/luckfox-live/luckfox-pico -path '*/curl/curl.h' -o -name 'libcurl.so*'
```

헤더가 `<staging>/usr/include/curl/curl.h`, 라이브러리가 `<staging>/usr/lib/libcurl.so`에 있는 경우 `CURL_PREFIX=<staging>/usr sh build.sh`로 지정한다. 꺾쇠 부분은 실제 검색 결과로 바꾼다. 대상 libc/아키텍처와 보드 런타임의 호환성은 실제 빌드 및 보드 실행으로 확인해야 한다. `libcurl.so.4.8.0` 파일명만으로 curl 제품 버전을 판정하지 않는다. 보드 `curl --version`으로 버전을 확인한다.

네이티브 시험 빌드에는 호스트용 curl 개발 헤더/라이브러리가 필요하다. 빌드 스크립트가 패키지를 자동 설치하지 않는다.

3. 생성된 `dist/heartbeat`를 기존 파일 전송 방법으로 보드의 `/tmp/i1-heartbeat`에 복사한다. VM에서 만든 ARM 실행파일을 VM에서 실행하지 않는다.
4. 보드에서 다음을 실행한다. 이 명령은 소켓을 만들지 않고 상태를 읽어 JSON만 출력한다.

```sh
chmod +x /tmp/i1-heartbeat
/tmp/i1-heartbeat --dry-run --count 3
```

약 5초 간격으로 3번 출력하고 종료한다. 기본값은 `--dry-run --count 1`이다.

```text
START mode=DRY_RUN_NO_NETWORK interval_ms=5000 timeout_ms=3000 mac_interface=eth0 wg_interface=wg1
DRY_RUN attempt=1 payload={"mac":"...","status":"online","rtsp_ok":false,"wg_ok":false,"config_version":0}
...
STOP attempts=3
```

- MAC은 `/sys/class/net/eth0/address`에서 읽어 **콜론 없는 대문자 12자리**로 만든다. 예: `02:AB:CD:12:EF:34` → `02ABCD12EF34`. 이전에 합의한 소문자 형식은 이번 ICD 정합 요청으로 대체했다. 문서 예시 및 DB CHECK가 대문자를 명시한다. 저장된 보드 시리얼을 MAC으로 대체하지 않는다.
- `MAC_UNAVAILABLE`이면 eth0 존재/값부터 확인한다. 서버 등록 MAC이 eth0 기준이므로 wlan0로 임의 대체하지 않는다. 다른 인터페이스를 사용하기로 합의한 경우에만 `--mac-interface`를 지정한다.
- `rtsp_ok`는 좀비 상태가 아닌 `rkipc` 프로세스 존재 여부다. 현재 custom RTSP/NPU 프로그램만 실행 중이면 영상이 나와도 false일 수 있다. 이 I1 지표는 실제 영상 수신 검증이 아니다.
- `wg_ok`는 wg1의 IFF_UP 플래그다. WireGuard의 operstate가 unknown인 정상 상황을 false로 처리하지 않는다. 서버 핸드셰이크 성공을 의미하지 않는다.
- 필드 타입은 mac/status 문자열, rtsp_ok/wg_ok JSON boolean, config_version 정수0으로 구현했다. ICD 발췌본의 필수 필드와 일치한다. 실서버 구현과의 통합 시험은 남아 있다.
- 두 보드 각각 확인하고 MAC 마지막4자리가 다른지 비교한다. 이 패키지 자체가 두 보드를 검사했다고 간주하지 않는다.

## 구현된 후속 전송 경로 (지금 실행하지 않음)

- `--send`와 명시적인 숫자 IPv4 `--host`가 둘 다 있어야 전송한다. 실제 서버 주소는 소스/설정의 기본값에 넣지 않았다.
- 경로 `/v1/devices/heartbeat`, 기본 포트3000, 5초 시작 간격. 겹치는 요청 없음.
- 연결·쓰기·읽기 전체를 libcurl의 CURLOPT_TIMEOUT_MS=3000으로 제한한다. DNS 조회는 없으며 숫자 IPv4만 받는다. `--timeout-ms` 허용 범위100–4000ms.
- 200의 최상위 비음수 정수 config_version은 로그만 남기며 보드의 config_version=0을 변경하지 않는다. 누락/잘못된 JSON도 로그를 남기고 계속한다.
- 403, 다른 HTTP 상태, 연결 실패, 응답 지연, 잘못된 응답은 다음5초 주기에 재시도한다. 실패 뒤 즉시 반복 폭주하지 않는다.
- HTTP 전송과 Content-Length/chunked 응답 처리는 libcurl에 맡긴다. 본문 수신 콜백은 8191바이트를 초과하면 요청을 중단한다. 이전 수동 HTTP 파서의 헤더8KiB/전체16KiB 제한은 현재 전송 경로에 적용되지 않는다. 환경변수 프록시와 리다이렉트를 사용하지 않는다. 인증·TLS는 이 문서의 VPN 내부 HTTP 범위에 없다.
- 전송 모드에서는 fcntl PID 파일 잠금으로 동일 PID 파일의 중복 실행을 막는다. SIGTERM/Ctrl+C로 종료한다. 전송 중에는 libcurl 요청 종료까지 최대 설정 제한시간만큼 기다릴 수 있다.
- 카메라/WireGuard를 시작·종료하거나 설정을 변경하지 않는다. API 응답에 따른 원격 설정 적용도 하지 않는다.

## 부팅 스크립트 (준비만, 미설치)

- `S60heartbeat`와 `heartbeat.conf` 포함. 기본 `ENABLED=0`, `HOST`는 빈 값이다.
- 나중에 `/usr/bin/i1-heartbeat`, `/etc/heartbeat.conf`, `/etc/init.d/S60heartbeat`에 설치하고 대상 호스트를 명시한 후에만 활성화한다. 현재는 `/etc`에 복사하거나 ENABLED를 바꾸지 않는다.
- 서비스 로그는 syslog(`i1-heartbeat`)로 보낸다. 보드 syslog 서비스/로그 조회 방식과 init 실행 순서는 실제 환경에서 확인해야 한다.
- S50wireguard 다음 순서라는 이름만으로 터널 준비를 보장하지 않는다. 늦은 네트워크 준비에도 재시도하는 방식이며 재부팅 실측이 필요하다.

## 서버 없는 검증

Linux VM에서 다음 테스트는 컴파일러와 표준 C 라이브러리만 사용하며 네트워크 호출이 없다.

```sh
sh tests/run_offline.sh
```

Windows MSVC 실행용은 `tests/run_offline_windows.cmd`다.
검증 범위: MAC 형식/무효주소, 정확한 JSON과 boolean, config_version 파싱·중복키·중첩값·오버플로 거부, HTTP Content-Length/keep-alive 종료 판단/chunked/403, 수신 중간 절단과 크기 제한.

## 로컬 수신 시험 도구 (나중 단계, 현재 미실행)

`tests/mock_server.py`는 POST를 처리하는 Python 표준 라이브러리 서버다. 기본 bind는127.0.0.1:13000이며 가져오기(import)만으로 서버가 시작되지 않는다.
시험 진행 요청 후 VM 내에서 native 실행파일과 함께 쓸 수 있다.

```sh
sh build.sh --native
python3 tests/mock_server.py --mode ok
```

다른 VM 터미널에서만 다음 명령으로 loopback 테스트한다.

```sh
./build/native/heartbeat --send --host 127.0.0.1 --port 13000 --count 5 --pidfile /tmp/i1-heartbeat-test.pid
```

VM의 eth0 MAC과 상태는 보드의 값이 아니다. eth0가 없는 VM은 실제 VM 인터페이스를 확인해 시험용 `--mac-interface`를 지정한다.
`--mode forbidden`, `--mode slow`(6초 지연), `--mode chunked`로403·timeout·chunked를 시험한다. 서버 중단/재시작 시에도 heartbeat가 계속 재시도하는지는 별도 관찰한다.
기본 loopback 서버는 외부 보드에서 접속할 수 없다. 보드 연동용 bind/방화벽 변경은 후속 단계에서 실제 네트워크를 확인한 뒤 진행한다.

기존 `hb_http` 파서와 해당 단위 테스트는 회귀 확인용으로 남아 있으나, 현재 heartbeat 전송에서는 호출하지 않는다. 이 테스트 통과는 libcurl 전송 검증을 의미하지 않는다.

## 과거 검증 상태 (libcurl 전환 당시; 최신 상태는 아래 참조)

- Windows MSVC에서 프로토콜 테스트 `/W4 /WX` 빌드 및 실행 통과. 네트워크 호출 없이 수행.
- 셸 스크립트는 정적 검토 및 LF 줄바꿈 확인. 로컬 셸 구문 검사 실행은 샌드박스에서 차단되어 미검증. Python 시험 서버의 구문/요청 스키마 테스트 통과, 서버 미기동.
- libcurl 전환 후 전체 Linux 프로그램 빌드 및 HTTP 통합 시험: 미검증. 로컬 WSL 추가 실행 승인 요청이 거절되어 실행하지 않았다.
- ARM 빌드, 보드 dry-run, 실제 타이머/소켓 timeout/복구, 자동 실행,2대 동시 연동: 미검증.
- 보드/외부 서버 접속, heartbeat 전송, VPN 변경, 부팅 설정 설치는 수행하지 않았다.
- 전원 차단20초 이내offline은 서버/클라이언트 판정까지 포함하는 통합시험이다.

## ICD I1 형식 반영 (2026-11-05)

전송 예시 (boolean 값은 실행 상태에 따라 달라짐):

```json
{"mac":"7424CA5004AF","status":"online","rtsp_ok":false,"wg_ok":true,"config_version":0}
```

- POST /v1/devices/heartbeat, Content-Type: application/json, 5초 주기. 요청 전체 제한시간3초 유지.
- 보드 인증은 서버에서 출발지 VPN IP로 수행. JWT, device_id, serial_no, mac_no를 heartbeat 요청에 추가하지 않는다.
- device_id CAM-04AF는 이 MAC의 서버 등록 식별자 예시이며 heartbeat 요청 필드가 아니다.
- storage_used_bytes, firmware_version, latency_ms는 선택 필드로 현재 보드는 생략한다. 모의 서버는 문서의 타입으로 제공하면 허용한다.
- 200 응답의 config_version은 로그만 남긴다. 보드 적용 버전은 I1에서0 유지.
- 모의 서버의 정상 응답 기본값은 {"config_version":0}. 비영 응답 파싱 시험은 --config-version 7로 선택 가능.
- 모의 서버 400/403 오류도 {"error_code":"...","message":"..."} JSON을 사용한다. details는 선택이라 생략한다. 구체적인 heartbeat error_code/message 문자열은 발췌본에 정의되지 않아 BAD_REQUEST/FORBIDDEN은 시험용 값이다.
- 보드는 HTTP400/403에서 원인 점검 안내를 기록하고 다음 주기에 재시도한다. 서버 오류 본문의 message/details를 해석하거나 그대로 출력하는 기능은 없다. 로그 문구는 ICD 표준이 아니다.
- mock_server.py는 실제 VPN IP/MAC 등록 DB 검증을 구현하지 않는다. forbidden은403 강제 시험 모드다.
- rtsp_ok=rkipc 비좀비 프로세스 존재, wg_ok=wg1 UP 판정은 기존 I1 보드 시험 정의로 유지한다. 실제 스트림 수신/최근 핸드셰이크의 보장은 아니다. 발췌본의 '정상 여부'와 실제 서비스 판정 기준은 통합 시 확인한다.
- 15초 초과 offline 계산과 클라이언트5초 재조회는 서버/클라이언트 담당이며 보드에 추가하지 않는다.

이전 소문자 바이너리로 HTTP200/403/3초timeout/서버중단복구 및 서비스 실행을 확인했다. 이번 대문자 변경 후 C 프로토콜 테스트는 통과했으나 ARM 재빌드/보드 재전송은 필요하다. 서버 등록 MAC도 7424CA5004AF 형식으로 맞춘다. 보드 네트워크/카메라/부팅 설정은 이 변경에서 수정하지 않았다.

검증: Windows MSVC 프로토콜 테스트 통과. `python tests/test_mock_server.py`의 로컬 HTTP200/400/403 및 필수/선택 필드·대문자·자료형 검사 통과. 테스트는127.0.0.1만 사용하고 자식 서버를 종료한다. 실제 보드 VPN 테스트를 대체하지 않는다.
