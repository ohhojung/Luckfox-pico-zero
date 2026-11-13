# CCTV 프로젝트 작업 기록

## C드라이브 원본 데이터 삭제 완료 — 2026-11-12

- 사용자 삭제 요청에 따라 D:\Projects\Luckfox와 모든 원본 파일의SHA256·세 Git작업상태를 재검증하고 C:\Users\ohhoj\Downloads\Luckfox의 원본 데이터를 삭제했다. 복사 이후 생긴 Codex Git기록6개도 D에 추가 보존했다. D의 .git·references·미커밋 작업은 유지된다.
- 앞으로 D:\Projects\Luckfox를 작업 폴더로 연다. 기존 C경로를 연결하는 junction은 만들지 않았다. 사용 중인 프로세스 때문에 C의 빈 루트폴더만 남을 수 있으나 원본 파일 데이터는 모두 정리했다.


## D드라이브 복사 검증 완료·경로 전환 대기 — 2026-11-12

- 사용자 요청으로 C:\Users\ohhoj\Downloads\Luckfox 전체를 D:\Projects\Luckfox로 복사했다. references·숨김.git·미커밋 파일 포함1694개 약1.21GiB의 SHA-256 일치 및 세 Git 저장소의HEAD·작업상태·remote 일치를 검증했다.
- 원본폴더를 백업이름으로 전환하는 단계에서 사용 중인 프로세스 잠금으로 Move-Item이 실패했다. 따라서 C 원본 데이터는 삭제하지 않았고 junction도 만들지 않았다. 현재는 검증된 두 사본이다.
- 이후 D:\Projects\Luckfox를 작업폴더로 열어 사용한다. C경로를 사용하는 Codex·터미널·다른 프로세스가 해제된 뒤, 양쪽 변경 여부를 재확인하고 원본 정리와 경로 전환을 완료해야 한다. 미검증 상태로 C폴더를 삭제하지 않는다.
- 검증 로그 및 SHA256목록은 C:\Users\ohhoj\AppData\Local\Temp\luckfox-migration-20261112.log 및 luckfox-migration-20261112-sha256.json에 있다. GitHub 원격·보드 설정 변경은 없다. 이 상태 기록은 두 사본에 동일하게 반영했다.


## 팀장 BoardServer 로컬 사본 최신화 — 2026-11-12

- 사용자 요청으로 references/BoardServer-review의 clean main을 origin/main에서 fast-forward했다. 6eb3ad5 → 46703b3, 30파일 변경. 원격에push하거나 우리 소스·보드에 적용하지 않았다.
- heartbeat 기본 URL 및 S60heartbeat가10.0.0.100으로 갱신됐고 팀장 코드의WG 기본은wg1 유지다. 새 streamer에는 이중H.265·시간OSD·정지감시·상태파일·앱내재초기화 및 기본비활성오디오 경로가 추가됐다. README상 /tmp 시험 단계이며 기본 두 출력18fps다.
- 팀장 docs/progress/I2.md의 최신 보고는 rkipc에 AE표기12/VTS3750 타이밍을 적용해 재부팅4회 균일18fps, 정지·10fps고정·pingpong0을 관측했다는 내용이다. 이는 팀장 보드의 보고이며 우리 보드 검증이 아니다. 우리 센서mode30 모듈·AE설정과 차이가 있으므로 표기FPS1.5배 관계나 설정을 그대로 적용하지 않는다. 우리20fps 진행 결정 유지.

## I2 두 출력20fps 기준 확정 — 2026-11-12

- 사용자가 우선20fps로 진행하고30fps 방법을 확보하면 추후 수정하도록 결정했다. 이전 메인30/서브20 준비보다 이 결정을 우선한다.
- 자체 송출기 두 채널 목표·GOP를20으로 설정하고 인코더src/dst 선언도20/20으로 맞췄다. 기존 센서30fps 요청은 약20fps ISP출력을 얻었던 조건이므로 유지하며 SENSOR_REQUEST_FPS로 별도 명명했다. 새로운 설정의 실측FPS는 보드 미검증이며 정확한20fps를 보장하는 변경으로 해석하지 않는다.
- 30fps 원인 진단은 별도 과제로 보류하고 I2 영상연동·자동시작·heartbeat 검증을 우선한다. 보드 접근·설정 변경 없음.

## I2 운영 준비 1단계 — 2026-11-12

- 사용자 순서대로 진행 요청으로 자체 송출기 메인30/서브20 목표 및 채널별GOP·로그를 분리했다. 공식 ARM uClibc 빌드 통과. 실제ISP입력약20 조건에서 src30/dst20에 따른 서브FPS 감소 가능성이 있어 실측 후 확정한다.
- S59cctv 시작·정지·상태 스크립트 추가 및 S60heartbeat와 셸 문법 확인. 아직 보드 자동시작 충돌 확인·설치·재부팅 검증은 미수행이다.
- heartbeat 목적지10.0.0.100 및 wg0 기본값 반영. 현재 서비스의 --rtsp-process cctv-dual-rtsp 선택 구조를 확인했다. Windows 프로토콜 테스트 통과, heartbeat ARM 재빌드는 개발용libcurl 위치 확인 단계로 미완료다. 보드 접근 없음.
- 배포 상태와 미완료 사항은 deliverables/cctv_dual_rtsp/I2_DEPLOY_STATUS.md. 다음은 사용자가 현재 카메라 프로세스·센서 포맷·부팅 파일을 조회한 후 새 송출기를 수동 시험하는 단계다. 이전ZIP·heartbeat바이너리를 새 변경 적용본으로 취급하지 않는다.

## 사용자 첨부 I2 계획·보드 To-Do와 현재 구현 대조 — 2026-11-11

- 첨부 I2 PDF(6쪽)와 보드 PDF(8쪽)를 텍스트·핵심 표 이미지로 검토했다. 상세 분석은 docs/I1_I2_GAP_REVIEW_2026-11-11.md. 문서 지시는 요구사항으로 분석했으며 설치·외부 공유·보드 명령을 실행하지 않았다. 별도 ICD I2 발췌 원문은 첨부되지 않아 두 PDF에 명시된 범위를 기준으로 한다.
- 문서의 운영 서버는10.0.0.100:3000, heartbeat5초, 기존JSON 유지다. 현재 소스·S60heartbeat의10.0.0.1 및 wg1 검사 기본값은 실제 보드wg0에 맞춰 수정할 대상으로 분류했다. WG_INTERFACE는 상태 검사뿐이며 전송은 라우팅을 따른다. 별도heartbeat.conf는 현재 선택 사항이고 MAC 기본wlan0도 이미 구현돼 있다. 이번에 코드는 변경하지 않았다.
- 문서 A-2의 시계 수정은11/9 이미 수행·재부팅UTC 확인했으며 실행 중 영상 서비스와 함께 유지되는지 검증은 남는다. 보드2 실서버HTTP200·DB·웹online/offline는 여전히 미검증이다. BoardServer로 즉시 교체할 요구는 없고 기존i1-heartbeat를 I2에서 사용한다는 문서 방침이다.
- I2는 RTSP pull→MediaMTX→WHEP/WebRTC 경로다. 보드 목표는/live/0 H.265 1080p30 및/live/1 H.265 540p20. 현재 자체 송출기는 두 채널 목표30을 공유하므로 스트림별FPS 분리, 영구 설치·부팅서비스, 선택 송출기에 맞는rtsp_ok가 필요한 차이다. rkipc INI는 자체 송출기의 설정 파일이 아니며 운영 송출기 선택과 문서 가정 정리가 필요하다.
- 보드 PDF6·8쪽은 실측약20fps로 이번 통합을 진행·공유하고30fps 확보는 별도 과제로 분리한다. 이것은30fps 달성 판정이 아니다. 문서의 센서mode30 패치 미검증 설명은 오래됐으며 실제CIF30/ISP약20 확인이 최신이다. 기존30fps 문제는 미해결 별도 과제로 유지한다.
- 보드에WebRTC/push 구현을 추가할 필요는 없다. MediaMTX의 메인상시pull·서브on-demand와WHEP주소 전달은 서버, 화면 재생·재연결은 클라이언트 담당이다. 녹화·NPU·마스킹·설정적용은 이번I2 범위 밖이다. 이번 작업은 분석 문서·기록 작성만 수행했다.

## 다음 확인 대상 — 작은 입력의 AIQ 멀티 통계 경로 선택 — 2026-11-11

- 첫 조사 대상은1944×1097에서 AIQ가 단일 영역 버퍼에 멀티 통계 해석기를 사용하는 선택 조건이다. 매칭된 라이브러리의 해당 분기 및 초기화 경로를 추적하는 것이 필요하며, 기존 SIGSEGV나20fps 실측 반복만으로 새 원인을 얻지는 못한다.
- 로컬 송출기는 AIQ prepare에0,0,NORMAL을 전달한다. 공식 헤더는 width/height를 내부 확인용 센서 출력 크기로 설명하며, 이 호출만으로 앱이4K를 강제한다고 판단하지 않는다. 공식/로컬 hw.c에서 DT rockchip,unite=1은 ISP_UNITE_ONE을 선택하지만 AIQ 멀티 해석기 선택까지의 전달 경로는 아직 미확인이다.
- 보드를 다시 켠 뒤 시험 전 상태 확인은 카메라 프로세스, 센서 현재 포맷, 실행 중 DT의rockchip,unite 원시값 조회로 제한한다. 이 조회는 재부팅 후 기준 확인이며 자체로20fps 원인을 확정하는 시험은 아니다. 보드 접근·설정 변경·새 실측은 하지 않았다.

## 해상도를 바꿔도 약20fps가 유지된다는 사용자 지적 재검토 — 2026-11-11

- 사용자 지적에 따라 VI 단독 원본 ISP 로그를 다시 대조했다. 1080p와540p 출력 시험 모두 Input3840×2160, 출력간격49ms다. 출력 크기에 둔감한 공통 경로의 처리 주기·readback 스케줄링을 우선 조사할 근거가 있으며, 단순 출력 해상도에 따른 연산 부족만으로 설명하지 않는다.
- 입력도 줄인1944×1097 시험은 AIQ 충돌 또는 AIQ 생략 시0프레임으로 끝났다. 따라서 작은 입력에서도20fps로 고정된다는 성공 실측은 아직 없다. 변경되지 않은4K 입력 처리량의 한계와 드라이버 처리 주기 제한은 기존 비교만으로 분리할 수 없다.
- 약20.2fps의 반복 관측은 안정된 병목을 보여주지만 코드에20fps 상수가 있거나 의도적인 제한이 있다는 증거는 아니다. 공통 처리 주기·프레임 선택·대기 경로 검토와 작은 입력 경로의 정상화가 다음 판단에 필요한 사항이다. 새 보드 시험은 하지 않았다.

## 20fps 제한 원인 및 다음 해결 우선순위 검토 — 2026-11-11

- 기존 실측을 종합하면 30fps 요청 후 센서/CIF는 33ms·30fps, ISP 두 출력은 49ms, VENC는 약20.2fps다. 10월 2일 VENC/RTSP 없는 VI 단독 1080p 및 540p도 약20fps였다. 제한 구간은 ISP/VI 출력 경로로 강하게 좁혀지며 Wi-Fi/VLC/H.265 이중 인코딩을 현재 제한의 우선 원인으로 보지 않는다.
- 1080p/540p는 출력 크기이며 ISP 입력은 3840×2160으로 유지됐다. 가장 유력한 후보는 이 4K 입력의 unite 분할·readback 처리, 메모리 이동 및 스케줄링이다. 로컬 드라이버에 해당 모드의 재처리 분기는 있으나 `X2`/`force to 2 readback` 문구만으로 정확한20fps 산식이나 하드웨어 절대 상한을 도출할 수 없다. 순수 연산량·메모리 대역폭·드라이버 대기를 아직 분리하지 못했다.
- 제조사 공식 위키 https://wiki.luckfox.com/Luckfox-Pico-Zero/ 의 ISP 5M@30fps와 제품 페이지 https://www.luckfox.com/Luckfox-Pico-Zero 의 ISP 8M@15fps를 재확인했다. 4K 입력30fps가 보장된 사양은 아니며, 실제 약20fps 관측을 제품표의15fps로 덮어쓰지 않는다.
- 1944×1097 작은 입력의 AIQ 통계 버퍼 범위 초과는 별도 확인된 장애다. 4K의20fps를 직접 설명하는 오류는 아니지만 입력 부하를 낮춰30fps를 검증하는 경로를 막는다. 해결 순서는 단일 영역 통계와 일치하는 AIQ/드라이버 설정 또는 호환 빌드를 확보한 뒤 작은 입력에서 실제 CIF→ISP→VENC FPS를 측정하는 것이다. 이 변경만으로30fps가 보장되지는 않는다.
- 자동노출에 따른15fps는 실행 중 고정30fps 요청으로 센서/CIF30까지 올렸으므로 현재 잔여20fps 제한과 구분한다. VI 버퍼2→4, VI FPS -1/-1, 출력 해상도 축소 및 TNR 처리 bypass 시험은 개선이 없었다. BAY3D 전체 블록 비활성화는 검증되지 않았다. AIQ 생략 시험은 params 대기 경고와0프레임으로 종료되어 정상 송출 대안이 되지 못했다.
- 이번 작업은 기존 로그·로컬 소스·공식 사양 재검토다. 새 보드 시험이나 설정 변경은 없으며30fps 달성은 여전히 미확인이다.

## 작은 센서 모드 AIQ 없는 첫 송출 시험 — 2026-11-11

- 사용자 `-N` 로그에서 AIQ 비활성, RAW 1944×1096, 두 RTSP READY를 확인했다. 이어 `/live/1`이 5초간 VENC 프레임을 받지 못해 `NO_FRAME`으로 종료·정리했다. 두 채널 실측 FPS가 기록되지 않았고 `/live/0`에 프레임이 들어왔는지도 로그로 확정할 수 없다.
- 다음은 같은 시험이 실행 중일 때 CIF·ISP 출력 카운터를 함께 수집해 프레임이 끊기는 구간을 확인하는 것이다.
- 사용자 수동 순차 실행에서 `Done(1)`과 같은 540p `NO_FRAME`이 재현됐다. CIF는 시작·출력 정보 없이 클록만 표시했고 ISP 출력 필터는 비어 있었다. 활성 구간의 통계를 확보하지 못해 송출기 시작과 3초 후 조회를 같은 셸 줄로 실행하도록 안내했다.
- 시작 3초 후 동시 조회 성공. ISP는 1920×1080 12비트 입력 모드, 인터럽트 0, readback 프레임 0, mainpath/selfpath 모두 프레임 0·간격 0ms를 표시했다. CIF는 클록만 표시해 센서 스트림 시작 여부 미확인이다. 드라이버 소스에서 ISP 시작 시 3A 구독 여부에 따른 파라미터 대기는 확인했으나 현 시험의 대기·실패 원인은 단정할 수 없다. 다음은 IMX415·CSI·CIF·ISP 커널 시작 로그 조회다.
- 사용자 커널 로그에서 두 `-N` 시도 모두 ISP 시작 약 1초 뒤 `waiting on params stream on event timeout` 확인. 첨부 범위에 IMX415 `s_stream: 1` 기록은 없다. 로컬 `rkisp_start_3a_run`은 params STREAMON과 첫 파라미터를 기다리며, 타임아웃 자체는 경고 후 시작 경로로 복귀한다. 따라서 이 구성에서는 `-N`으로 두 스트림의 FPS를 측정할 수 없었으며 센서/ISP 시작의 구체적인 정지 원인은 미확정이다. 다음은 단일 영역 통계와 맞는 AIQ 경로를 찾는 것이다.

## 작은 센서 모드 AIQ 없는 송출 대조 준비 — 2026-11-11

- 매칭된 라이브러리에서 `RkAiqCore::analyze`의 통계 처리 경로가 AE/AWB/AF 뒤에 `handleAdehazeStats`를 호출하는 것을 확인했다. 그 호출과 핸들러에서 알고리즘 활성 여부로 분기하는 코드는 확인되지 않았다. Dehaze 제어만으로 충돌 회피를 가정하지 않았다.
- 두 H.265 스트림에 `-N` 시험 옵션을 추가했다. AIQ 초기화를 생략하고 기존 두 VI/VENC 및 RTSP를 시작한다. 센서 1944×1097/0x3010에서 `venc_fps` 및 CIF를 확인하며 AIQ 없는 자동 노출·화질은 시험 대상 밖이다. 공식 ARM uClibc 빌드 통과, 보드에서 영상 취득과 실제 FPS는 미검증이다.

## ISP32 단일·분할 통계 버퍼 크기 대조 준비 — 2026-11-11

- 공식 ARM 컴파일러로 SDK 헤더를 컴파일해 ISP32 통계 구조체 크기 13036바이트 및 `frame_id` 오프셋 13028(`0x32e4`), ISP32 Lite는 각각 3788 및 3780바이트임을 확인했다. 로컬 드라이버는 ISP32 구조체를 16바이트 정렬한 13040바이트에 `unite_div`를 곱한다. 로컬 소스의 ISP32 입력 면적 분기에 따르면 1920×1080은 DIV1, 3840×2160은 DIV2에 해당한다. 보드의 실제 버퍼 길이는 미측정이다.
- 확인된 AIQ 명령 흐름은 V4L2 버퍼 길이의 절반을 첫 통계 포인터에 더해 두 번째 포인터를 만들고 각 포인터의 `frame_id`를 읽는다. 작은 모드에서 13040바이트 한 영역만 제공되는 경우 두 번째 읽기의 시작 오프셋은 19548바이트로 길이를 넘는다. 동일 SIGSEGV의 `r4-r5`에서 길이의 절반을 얻어 확인하도록 안내했다.
- 사용자 GDB 재현에서 같은 PC `0xa6d558fe`, `r3=0x32e4`, `r4-r5=6520`, 두 배 13040바이트를 확인했다. 충돌한 두 번째 `frame_id` 읽기는 버퍼 시작에서 19548바이트로 보고된 길이를 넘는다. 작은 센서 모드의 안정화가 먼저 필요하고, 안정화 뒤 두 출력의 30fps 달성 여부를 실측해야 한다.

## 1944×1097 AIQ 충돌 함수 확인 — 2026-11-11

- 사용자 GDB에서 `librkaiq.so`의 런타임 `.text` 범위 `0xa6ce7a80–0xa6de9370` 확보. SHA-256 대조가 끝난 SDK ELF의 `.text` 주소 `0x1ba80`을 빼 load bias `0xa6ccc000`을 구했고, 라이브러리 내부 프레임 9개를 모두 심볼에 연결했다. `xc:isp_3a_stats`에서 `RkAiqCore::handleAdehazeStats` → `RkAiqResourceTranslatorV32::translateAdehazeStats` → `translateMultiAdehazeStats` 경로다.
- ARM 디스어셈블리에서 충돌 PC `0xa6d558fe`는 ELF `0x898fe`(함수 시작+126)이며, 두 번째 통계 영역에서 값을 읽는 명령이다. 12비트 작은 입력에서 통계 버퍼 구성과 AIQ의 멀티 통계 처리 가정이 어긋나는지 확인할 근거를 얻었다. 실제 버퍼 크기·레이아웃과 근본 원인은 아직 미확인이다.

## 보드 AIQ와 공식 SDK 심볼 사본 일치 확인 — 2026-11-11

- 공식 ARM `strip --strip-all` 및 `--strip-unneeded`로 로컬 SDK `librkaiq.so`의 임시 사본을 생성했다. 두 결과 모두 1683192바이트, SHA-256 `3f8f084df73ce82871012bb1c8e4f06cf04731994eebc5e7d75d0b7822b4d48e`로 사용자 보드 파일과 일치한다. 보드 파일을 다시 복사하지 않고 로컬 원본의 심볼을 사용할 수 있다.
- 다음은 1944×1097 GDB 충돌 실행의 `info files`에서 `/oem/usr/lib/librkaiq.so`에 해당하는 `.text` 시작 주소를 얻고, 같은 실행의 백트레이스를 로컬 심볼에 대조하는 것이다. 이번에는 보드 명령을 실행하지 않았다.
- 사용자 V4L2 조회에서 1944×1097/0x3010을 재확인했다. GDB 출력을 `grep`으로 필터링한 실행은 빈 결과라 바이너리 존재, GDB 시작 여부와 실제 충돌 여부는 미확인이다. 다음은 `/tmp` 바이너리 존재를 확인하고 필터 없는 로그를 수집한다. PC의 ARM TNR Lite 바이너리는 존재를 확인했다.
- 후속 조회에서 `/bin/gdb`는 존재하지만 `/tmp/cctv-dual-rtsp-tnr-lite`가 없었다. 필터 없는 GDB 로그도 `No executable file specified`를 확인했다. PC의 기존 ARM 바이너리를 같은 경로로 다시 전송한 뒤 1944×1097 모드에서 재현하도록 안내했다.
- 재전송 후 GDB가 실행 파일을 시작했고, 센서 포맷은 1944×1097/0x3010으로 조회됐다. 송출기는 TCP 554의 `Address already in use`로 AIQ 초기화 전에 종료 코드 1을 반환했다. 이번 실행에는 AIQ 매핑이나 SIGSEGV 스택이 없다. 포트 점유 프로세스는 미확인이다.
- 후속 시험에서도 TCP 554 충돌이 반복됐고 사용자 `/proc` 조회로 수신 소켓 inode 7997을 `/proc/673/fd/51`에서 확인했다. PID 673의 실행 파일은 아직 조회되지 않았다. 기존 카메라 앱으로 확인된 경우 종료하고, 센서 포맷을 다시 설정한 뒤 GDB를 실행한다.

## 영상 전용 1944×1097 모드에서 동일 AIQ 충돌 스택 확인 — 2026-11-10

- GDB 재현에서 센서 pad 0 `1944×1097/0x3010`, VI 로그 RAW 1944×1096, 두 RTSP READY 이후 `xc:isp_3a_stats`가 `/oem/usr/lib/librkaiq.so`의 `0xa6d558fe`에서 SIGSEGV. 9월 30일 NPU 포함 시험의 `librkaiq.so` 프레임 #0~#8 주소 9개가 모두 일치한다. 이 재현은 NPU 및 실행 중 30fps 요청 없이 발생했다. GDB의 `??`로 구체적인 함수명·원인은 아직 미확인이다.
- 로컬 공식 RKMPI uClibc `librkaiq.so`는 `.symtab`을 포함한다(크기 2682768바이트, SHA-256 `ff4cb39532e145c2317ac426038a2d57c629940a91d0d8812014e12c9a3c4dc9`). 다음은 보드 파일과 동일 빌드인지 대조한 뒤 실제 로드 주소를 기준으로 함수명을 확인하는 것이다. 보드 복원 여부는 미확인이다.
- 보드 `/oem/usr/lib/librkaiq.so`는 1683192바이트, SHA-256 `3f8f084df73ce82871012bb1c8e4f06cf04731994eebc5e7d75d0b7822b4d48e`로 SDK 파일 전체와 다르다. 심볼 제거 여부와 실제 빌드 차이를 구분하기 위해 사용자에게 보드 라이브러리를 읽기 전용으로 가져오도록 안내했다. SDK `.text`는 가상 주소 `0x1ba80`, 크기 1054960, SHA-256 `52b718a6d3ef60d3500785774d58cf82f4e356ba4fdcec58d51938f32730ad1c`다.

## 1944×1097 모드에서 영상 전용 송출기 SIGSEGV 재현 — 2026-11-10

- 사용자 로그에서 센서 pad 0 `1944×1097/0x3010 SGBRG12`, 기본 IMX415 IQ, AIQ init/prepare/start, 두 VI stream-on 및 두 RTSP `READY`를 확인했다. 첫 `STREAM` 또는 실행 중 AIQ 30fps 요청이 나오기 전에 `Segmentation fault (core dumped)`로 종료했다.
- 이번 송출기는 NPU를 초기화하지 않는다. 이전 NPU 포함 시험의 SIGSEGV와 유사하나 이번 실행의 fault 스레드·라이브러리·호출 위치는 아직 미확인이다. 이 시험에서 작은 모드 FPS는 측정되지 않았다. 복원 출력 미제공이며, 다음은 최근 dmesg 수집과 기존 3864×2192/0x300e 복원 확인이다.
- 후속 `dmesg | tail -n 40`에서 센서 `s_stream: 0. 1944x1097, hdr: 0, bpp: 12` 및 CSI·CIF 종료를 확인했다. 첨부 범위에 사용자 공간 fault 위치는 없다. 이후 `wg0` 핸드셰이크 재시도 로그는 별도로 관측됐다. 이번 실행의 코어 파일 저장 여부와 복원 여부는 미확인이다.
- 사용자 조회에서 `core_pattern`은 `| /usr/bin/coredump2sdcard.sh %p %e`, `/tmp/core*`는 없음, GDB는 `/bin/gdb`로 확인했다. 스크립트의 실제 저장 위치와 코어 보존 여부는 미확인이다. 코어가 있으면 동일 바이너리와 GDB로 읽고, 없으면 작은 모드에서 송출기를 GDB로 한 번 실행해 현재 fault 스레드와 스택을 수집한다.
- 후속 스크립트는 `/mnt/sdcard/core-<PID>-<실행이름>`으로 표준입력을 저장한다. 사용자가 `$1`·`$2`를 직접 셸에 입력해 `core--`를 조회했으며 실제 파일 목록은 미조회다. GDB 재실행의 RAW 입력은 폭 3864였으며 30fps 요청 후 양쪽 VENC 약 20fps, RTSP TX 오류 0, Ctrl+C의 SIGINT만 확인했다. 이 스택은 1944×1097 SIGSEGV의 백트레이스가 아니다.
- 사용자 `ls -lht /mnt/sdcard/core-*-cctv-dual-rtsp*`에서 일치하는 코어 파일이 없었다. 다음은 카메라 앱 종료 후 1944×1097/0x3010 설정과 조회를 같은 셸에서 수행하고 GDB로 직접 재현하는 것이다.

## 1944×1097 IMX415 입력 대조 준비 — 2026-11-10

- 과거 1944×1097/0x3010 시험 로그를 재확인했다. AIQ init/prepare/start와 VI stream-on 이후 `xc:isp_3a_stats`가 `librkaiq.so` 내부에서 SIGSEGV를 냈으며 그 시험 앱은 NPU를 초기화했다. 로컬 IMX415 드라이버는 이 센서 모드에서 1920×1080 crop bounds를 제공한다.
- 이번 대조는 TNR 우회 IQ 대신 기본 IQ, NPU 없는 현재 이중 송출기 및 동일 출력 1080p/540p를 사용한다. 카메라 앱을 중지한 뒤 `/dev/v4l-subdev2` pad 0의 1944×1097/0x3010 포맷을 임시 설정하고, 실행 중 CIF 실제 입력 및 두 출력 FPS를 확인한다. 종료 후 3864×2192/0x300e 복원 절차를 안내했다. 보드 명령 미실행, 작은 입력 모드의 정상 시작 및 FPS 미확인.

## 임시 IQ의 BAY3D bypass 적용 및 이중 송출 실측 — 2026-11-10

- 보드 로그에서 `/tmp/iq-tnr-off/`의 IMX415 JSON 로드 성공. 기존 IQ 관측의 `BAY3D ON(0x2001 ...) bypass:0`과 달리 시험 IQ에서 `ON(0x2003 ...) bypass:1`을 확인했다. 로컬 ISP procfs 소스에서 enable과 bypass는 별도 비트다. 즉, 처리 우회는 적용됐으나 블록 enable은 유지됐다.
- 시험 중 AIQ 보고 30fps/2250줄, ISP read 33ms, 양쪽 ISP 출력 49ms로 관측. 7개 구간 VENC 1080p 20.16~20.38fps, 540p 20.14~20.38fps 및 RTSP TX 오류 0. TNR 처리 우회로 이 조건의 20fps 병목은 개선되지 않았다. 블록 전체 비활성화나 하드웨어 최대 성능까지 검증한 것은 아니다.

## Bayer TNR 전용 조회 mode=0 및 임시 IQ 대조 준비 — 2026-11-10

- 사용자 `-F 30 -T` 로그에서 AIQ 30fps/2250줄 및 VENC 20.17/20.37fps, `ISP_HW stage=before_tnr_request BAY3D ON`을 확인했다. V23 Lite 조회 후 `mode=0`(SDK의 INVALID)으로 보호 분기가 종료했으며 SetAttrib 호출이나 실제 TNR 전환은 없었다. 실행에서 읽은 IQ 파일 경로는 `/etc/iqfiles/imx415_CMK-OT2022-PX1_IR0147-36IRC-8M-F20.json`이다.
- 보드에서 가져온 IQ JSON을 입력으로 사용하는 `prepare_tnr_off_iq.py`를 추가했다. V23_LITE의 `TuningPara.enable` 한 값만 1→0으로 바꾼 별도 복사본을 만들며 기존 파일은 보호한다. 로컬 IMX415 IQ 백업에서 JSON 의미상 변경 한 곳 및 재실행 시 덮어쓰기 방지 검증 완료. 실제 보드에서 `-a`로 복사본 로딩, `BAY3D OFF`, VENC FPS는 미검증이다.

## Bayer TNR V23 Lite 전용 대조 및 하드웨어 상태 로깅 — 2026-11-10

- IMX415 IQ 백업의 `bayertnr_v23.Version=V23_LITE`와 공식 SDK 헤더를 확인했다. 기존 `-F 30 -T`의 공통 TNR 제어를 V23 Lite GetAttrib/SetAttrib으로 교체했다. 자동 모드 속성을 읽고 다른 값을 보존한 채 `stAuto.st3DParams.enable`만 false로 비동기 요청한다. `/proc/rkisp-unite0`의 `BAY3D` 줄을 같은 실행에서 요청 직전과 이후에 `ISP_HW`로 남긴다.
- 공식 ARM uClibc 빌드와 전용 API 심볼 링크 검증 통과. 실행 중인 보드의 라이브러리에서 함수 동작 및 BAY3D ON→OFF 전환·FPS 변화는 미검증이다.

## TNR 공통 제어 호출 후에도 VENC 약 20fps — 2026-11-10

- 사용자 `-F 30 -T` 콘솔에서 30fps 요청 후 AIQ 보고 30fps/2250줄, TNR 요청 전 VENC 20.18/19.98fps, `ISP_TNR enabled_before=0 requested=off` 및 후속 세 측정 구간을 확인했다. 요청 후 VENC는 1080p 20.16~20.39fps, 540p 20.19~20.36fps, RTSP 송신 오류 0이었다.
- 앞서 제공된 `/proc` 스냅샷은 `BAY3D ON`, `bypass:0`였고 이번 콘솔 캡처에는 같은 실행 중의 BAY3D 스냅샷이 없다. 공통 AIQ 제어의 반환 성공/조회 0은 하드웨어 비활성화 또는 실제 ON→OFF 전환 증거가 아니다. 같은 옵션을 반복하는 대신 BAY3D의 실제 상태 전환을 검증할 수 있는 전용 제어나 IQ 임시 설정을 다음 후보로 검토한다.

## TNR 대조 중 ISP 스냅샷 수신 — 2026-11-10

- 사용자 제공 스냅샷에서 ISP 두 출력이 모두 49ms(약 20fps), `BAY3D ON`, `bypass:0`이었다. 이 스냅샷에는 TNR 비활성화가 나타나지 않는다. 해당 스냅샷이 `ISP_TNR` 및 `after_tnr_request` 이후인지, AIQ 함수 반환 로그 및 같은 시점의 CIF 수신 FPS는 미제공이다. ISP Read의 66ms 한 표본만으로 센서 15fps 전환을 확정하지 않는다.
- 후속 스냅샷은 ISP read 33ms, mainpath·selfpath 49ms, `BAY3D ON`, `bypass:0`이었다. `ISP_TNR` 및 `AIQ_TIMING` 콘솔 로그가 아직 없어 비활성화 요청 실행 또는 ISP 반영 실패를 구분할 수 없다.

## ISP TNR 단독 대조 옵션 준비 — 2026-11-10

- 로컬 ISP 드라이버에서 `ISP_UNITE_ONE` 조건이 readback 처리 횟수에 영향을 주는 것을 확인했다. 이 동작이 출력 20fps의 직접 원인인지는 실측으로 분리되지 않았다. 이미 시행한 VI 버퍼·출력 해상도 시험 대신 4K 입력과 두 H.265 스트림을 유지하고 TNR만 변경하는 대조를 준비했다.
- `cctv-dual-rtsp -F 30 -T`는 기존 30fps 요청 후 첫 양쪽 VENC 측정 구간을 기록하고, 공식 AIQ `RK_MODULE_TNR` 제어로 TNR 비활성화를 한 번 요청한다. `ISP_TNR` 및 `AIQ_TIMING stage=after_tnr_request`로 구간을 구분하고 `/proc/rkisp-unite0`의 `BAY3D`로 적용을 확인한다. 기본 실행과 `-F 30`의 기존 흐름에서는 TNR을 변경하지 않는다.
- 공식 SDK ARM uClibc 교차 빌드 및 TNR 제어 함수 링크 확인. 보드 적용·FPS 비교는 미수행이다.

## AIQ 30fps 요청 후 CIF 30fps·ISP 출력 약 20fps 확인 — 2026-11-10

- 사용자 `/proc` 첨부에서 첫 CIF는 66ms/15fps, 뒤 CIF는 33ms/30fps였다. 두 번째 스냅샷에서 ISP read는 33ms, mainpath 1080p와 selfpath 540p는 각각 49ms(약 20fps), 지연 표시는 168ms였다. CSI 오류와 ISP 인터럽트 오류는 0이고 ISP read `frameloss`는 3, 두 출력 `frameloss`는 0이었다.
- CIF에서 실제 30fps를 받고도 ISP 두 출력은 약 20fps로 관측됐다. 앞선 VENC 실측과 일치하며, 과거 인코더·RTSP를 제외한 VI 단독 시험에서도 같은 구간이 관측됐다. 3840×2160 입력의 ISP `unite X2`·readback 경로를 우선 조사하되 구체적인 원인이나 하드웨어 상한은 미확정이다.

## 실행 중 AIQ 30fps 요청 후 이중 스트림 약 20fps 실측 — 2026-11-10

- 사용자 `/tmp/cctv-dual-rtsp-livefps -F 30` 로그에서 요청 전 AIQ 보고 15fps/4500줄, 두 VENC 14.59fps 확인. `AIQ_FPS stage=running requested=30` 후 네 구간 모두 AIQ 보고 30fps/2250줄, VENC `/live/0` 20.18~20.36fps 및 `/live/1` 20.18~20.38fps. 전 구간 `rtsp_tx_errors=0`, Ctrl+C까지 지속 후 자원 정리를 확인했다.
- 이전 시작 전 요청에서 관측된 5초 무프레임은 이번 실행에서 재현되지 않았다. AIQ 보고 30fps와 실제 CIF 속도는 구분한다. 이번 실행 중 CIF·ISP 스냅샷 및 VLC 실측은 미제공이므로, 다음은 `AIQ_FPS stage=running` 이후 두 `/proc` 통계를 함께 수집해 구간을 좁힌다.

## AIQ 30fps 시작 시 프레임 미수신 및 적용 시점 변경 — 2026-11-10

- 사용자 로그에서 `-F 30`의 `before_vi` 요청 및 두 `READY`를 확인했다. 이후 `/live/0`의 VENC에서 5초 동안 프레임을 받지 못해 `NO_FRAME`(`0xa004800e`)으로 정리 종료했다. `STREAM`과 실행 중 AIQ 재요청 로그는 없었고, CIF/ISP 및 센서 제어값은 이번 실패 실행에서 미조회다. 이 결과만으로 프레임이 멈춘 내부 구간은 특정할 수 없다.
- 수정본은 기존 AIQ 정책으로 시작해 두 채널의 첫 5초 실측을 남긴 뒤 한 번만 고정 30fps를 요청한다. ARM uClibc 교차 빌드와 소스 ZIP 검증 통과. 이 변경의 보드 실측은 아직 미수행이다.

## 이중 RTSP의 AIQ 30fps 비교 옵션 추가 — 2026-11-10

- `deliverables/cctv_dual_rtsp`에 선택 옵션 `-F 30`을 추가했다. AIQ의 AE 고정 프레임률을 VI 시작 전에 요청하고, 두 채널의 프레임 취득이 확인된 뒤 한 번 재요청한다. `AIQ_TIMING`에 AIQ 보고 FPS·프레임 길이, `STREAM`에 실측 VENC FPS가 나온다. 옵션 생략 시 기존 AE 정책이다.
- 공식 SDK ARM uClibc 교차 빌드 및 AE 함수 링크 확인. 보드 배포 및 `-F 30` 실측은 아직 미수행이다. 동일 조명에서 CIF의 실제 FPS와 두 채널의 VENC FPS를 각각 확인한다.

## 이중 RTSP 실행 중 IMX415 프레임 길이 확인 — 2026-11-10

- 사용자 조회에서 `/dev/v4l-subdev2`의 `exposure=3375`, `vertical_blanking=2308`, `horizontal_blanking=4936`, `pixel_rate=356800000` 확인. 현재 RAW 높이 2192와 VBLANK를 합한 VTS는 4500줄로, 과거 30fps 센서 시험의 2250줄(VBLANK 58)의 두 배다. 현재 CIF 66ms/15fps와 부합한다.
- 현재 노출 3375줄은 2250줄 프레임에 들어가지 않는다. 조도·자동노출 정책·모드 초기화 중 VTS 4500을 정한 원인은 미확인이다. 밝은 장면에서 읽기 전용으로 다시 확인하고, 필요하면 이전 시험과 같은 AIQ 고정 30fps 요청으로 대조한다. 과거 고정 요청은 CIF 30fps를 확인했으나 ISP/VI 출력은 약 20fps였다.

## 이중 RTSP 실행 중 CIF부터 약 15fps 확인 — 2026-11-10

- 사용자 첨부의 두 스냅샷에서 IMX415의 입력 모드 표시는 SGBRG10 3864×2192@30, CIF 출력은 3840×2160, `rate:66 ms`, `fps:15`였다. CSI overflow·bandwidth lack·size error 및 전체 오류는 모두 0이다. CIF와 ISP 양쪽 출력 카운터가 각각 248씩 증가했으나 스냅샷 사이의 정확한 실경과는 알 수 없어 FPS 산출에 쓰지 않았다.
- ISP read 및 1080p mainpath·540p selfpath가 모두 66ms, `frameloss:0`, ISP `ErrCnt:0`이었다. 앞선 VENC 약 15fps와 일치한다. 관측 가능한 프레임률 저하는 이미 CIF에서 나타났고, 센서의 실제 VBLANK/VTS 및 AE 적용 상태는 이번 로그에 없다. 다음은 송출 중 `/dev/v4l-subdev2`의 `vertical_blanking`, `horizontal_blanking`, `pixel_rate`, `exposure`를 읽기 전용 조회한다.

## H.265 이중 RTSP 보드 실측 — 2026-11-10

- 사용자 첨부 실행 로그에서 `/tmp/cctv-dual-rtsp`의 RTSP 554 리스너, IMX415/AIQ 시작, mainpath `/dev/video11`의 `/live/0` 1920×1080과 selfpath `/dev/video12`의 `/live/1` 960×540 준비 및 각 세션의 HEVC VPS/SPS/PPS 인식을 확인했다. 센서 RAW는 3864×2192였다.
- 약 25초 동안 5개 측정 구간에서 `/live/0`은 14.59~15.19fps, `/live/1`은 14.59~15.19fps였다. 각 구간 `rtsp_tx_errors=0`, Ctrl+C 후 채널 및 세션 정리 확인. `target_fps=30`은 인코더 설정값이고 두 스트림의 실제 VENC 취득은 약 15fps다. VLC 접속·동시 재생 출력은 제공되지 않았다.
- 두 채널에서 비슷한 속도가 관측되어 공통 센서/ISP 경로를 우선 점검하되 원인은 미확정이다. 다음 시험은 송출 중 별도 셸에서 `/proc/rkcif-mipi-lvds`와 `/proc/rkisp-unite0`을 5초 간격으로 두 번 조회하고, 같은 시간의 두 VENC 로그와 비교한다.

## H.265 이중 RTSP 전용 송출기 구현 — 2026-11-10

- SRS WC-F-001~004의 1080p, H.265, 고·저화질 동시 송출, 30fps 요구를 확인했다. ICD I1 발췌에는 heartbeat의 `rtsp_ok`가 있으며 RTSP 경로와 저화질 해상도는 이번 구현에서 정했다.
- 별도 소스 `deliverables/cctv_dual_rtsp`에서 IMX415/AIQ 하나와 VI→VENC 두 채널을 연결한다. RTSP 554번 포트의 `/live/0`은 1920×1080/4096 Kbps, `/live/1`은 960×540/1024 Kbps이며 둘 다 HEVC CBR, 인코더 목표 30fps다. 채널별 VENC 취득 FPS와 RTSP 송신 오류를 5초마다 로그로 남긴다. 사람 인식 및 이벤트·녹화·heartbeat 연동은 이번 범위 밖이다.
- 공식 RKMPI kernel-5.10.160 헤더·라이브러리와 Luckfox uClibc 도구 체인으로 셸 문법 검사 및 ARM 교차 빌드·링크가 통과했다. ELF32 ARM 및 `/lib/ld-uClibc.so.0` 인터프리터를 확인했다. 보드 실행, 핫스팟/Windows VLC 동시 재생, 실제 30fps 유지 여부는 미검증이다. 기존 heartbeat의 `rtsp_ok`는 `rkipc` 프로세스를 확인하므로 새 실행 파일의 상태 판정에 사용할 수 없다.

## 팀 1080p30 스트리밍 이슈 공유 — 2026-11-10

- 사용자 전달 메시지에서 팀원은 540 해상도에서 프레임이 잘 나오지만 1080p30에서 문제가 있어 측정 중이라고 보고했다. 실행 프로그램, 입력 모드, 코덱·비트레이트, 수신 경로 및 실측 FPS는 미공유다.
- 우리 보드의 과거 1080p H.264 4096 Kbps 단일 스트림 시험에서는 30fps 요청 후에도 AIQ 콜백 및 인코더 프레임 취득이 약 20fps였다. 이번 팀원 시험과 조건이 같은지와 원인 공유 여부는 미확인이다.

## 노트북 없이 팀 원격 테스트하는 방식 검토 — 2026-11-10

- [Luckfox Pico Zero 로그인 문서](https://wiki.luckfox.com/Luckfox-Pico-Zero/Login/)는 SSH를 지원 접속 방식으로 안내한다. 공개 SDK의 [Buildroot 설정](https://github.com/LuckfoxTECH/luckfox-pico/blob/main/sysdrv/tools/board/buildroot/luckfox_pico_defconfig)은 OpenSSH를 포함한다. Windows `adb devices -l`은 로컬 ADB 데몬 시작 실패로 장치 목록을 얻지 못했다. 현재 이미지의 SSH 리스너와 adbd TCP·인증·권한은 실기 미확인이다.
- 운영안 후보는 보드가 노트북과 독립적인 Wi-Fi/핫스팟에서 기존 AWS WireGuard 서버에 연결하고, 팀원별 VPN 피어와 SSH 공개키를 발급하는 구성이다. 보드의 관리 접속은 VPN에서만 허용하고, 테스트 계정은 비관리자이며 상태 조회 명령만 허용하는 SSH 강제 명령으로 제한한다. 팀원에서 보드로 직접 연결하려면 서버 피어 간 전달 정책 및 보드의 팀원 VPN 주소에 대한 AllowedIPs/복귀 경로를 설정해야 한다. 11월 9일 관측된 보드 peer의 AllowedIPs는 서버 `10.0.0.100/32`만 포함했다. NAT 뒤에서 수신이 필요하면 [WireGuard keepalive](https://www.wireguard.com/quickstart/) 설정을 확인한다.
- [ADB TCP](https://android.googlesource.com/platform/packages/modules/adb/+/refs/heads/main/docs/user/adb.1.md)는 현재 이미지가 지원할 경우 개발용 접속 후보지만, 셸 이외 파일 전송도 제공하므로 테스트 명령만 허용해야 하는 팀 공유에는 제한형 SSH가 적합하다. 공인 포트 공개는 운영안에 포함하지 않는다.
- WireGuard 없이도 같은 Wi-Fi에서 단말 간 통신이 허용되면 보드의 현재 `wlan0` IPv4와 TCP 5555로 ADB 접속을 시험할 수 있다. 외부망에서는 보드가 공인 서버에 먼저 SSH 역방향 터널을 열고 팀원이 SSH로 중계하는 구성도 가능하다. 두 경로 모두 이 보드에서 실기 미검증이다.
- 어댑터 단독 부팅 요청 후 Luckfox 공개 SDK 확인. `S50usbdevice`는 ADB 활성 조건에서 FunctionFS를 준비하고 `/usr/bin/adbd`를 시작한다. SDK의 `0015-make-adb-tcp-function-default-enbale.patch`는 USB 초기화 경로에서 기본 TCP 리스너도 연다. RSA 인증은 별도 패치에서 `ADBD_RSA_AUTH_ENABLE` 환경변수로 제어한다. 현재 보드 이미지에 같은 스크립트와 패치가 들어 있는지, 데이터 케이블 없이 5555가 열리는지는 미확인. 1차 시험은 상시 Wi-Fi에서 어댑터만 꽂아 부팅한 뒤 다른 단말이 현재 DHCP 주소의 5555에 `adb connect` 및 `adb shell uptime`을 실행하는 것이다. 보드 설정 변경 미수행.
- AOSP의 TCP ADB 서버는 한 포트에서 여러 연결을 받아 각각의 전송으로 등록한다. Luckfox 공개 SDK의 RSA 패치에서 처음 제시된 공개키를 확인하는 분기는 기본 `/etc/adbkey.pub`의 첫 줄을 비교하므로, 설치 이미지가 서로 다른 팀원 키를 어떻게 허용하는지는 별도 확인한다. 실제 동시 접속은 두 PC가 각각 셸을 연 상태로 `uptime`을 실행하는 방식으로 검증할 수 있으며, 이번에는 소스만 검토했다.
- 전원만 다시 연결하는 운영 검증은 USB와 노트북이 빠진 상태에서 Wi-Fi·WG 재연결, 실제 heartbeat API HTTP 200 및 서버 상태 갱신, Windows VLC 실영상 재생, 서비스·네트워크 장애 후 복구까지 확인한다. 최신 기록은 WG handshake 0, heartbeat `10.0.0.1:3000` 타임아웃 및 재부팅 표본에서 `rkipc` 미실행이다. 운영 서버 주소와 포트 계약을 먼저 확인하고 `wg_ok` 검사 대상 `wg1`과 실제 `wg0` 차이를 점검한다. 이번 검토에서 보드와 서버 설정은 변경하지 않았다.

## Wi-Fi 주소의 DHCP 여부 확인 — 2026-11-09

- ADB `ip -4 addr show dev wlan0`에서 `10.141.95.101/24`는 `forever`, `10.141.95.102/24`는 `secondary dynamic noprefixroute` 및 유효시간 2334초로 확인. 기본 경로는 핫스팟 게이트웨이 `10.141.95.250` 경유 `wlan0 src 10.141.95.102`이므로 일반 외부 트래픽의 Wi-Fi 송신 주소는 동적 할당값이다.
- `dhcpcd` 실행 확인. 조회한 `/etc/dhcpcd.conf`의 정적 IPv4 지시문 및 최근 임대 로그는 출력 없음. `.101`은 수명 표기만 확인했으며 그 설정 주체나 핫스팟 간 주소 유지 보장은 미확인. `wg0`의 설정 주소 `10.0.1.1`은 Wi-Fi 주소와 별개다.

## 현재 보드 WireGuard 부팅 스크립트와 heartbeat 경로 확인 — 2026-11-09

- ADB 읽기 결과 `/etc/init.d/S50wireguard`는 `wg-quick up/down /etc/wireguard/wg0.conf`를 호출하며 start/stop/restart/status를 제공한다. 11월 5일 원래 기록의 wg1 버전과 달리 현재 보드 설치본은 wg0다. 로컬 Git 추적 목록에 S50wireguard 파일은 없음.
- 앞선 ADB 출력에서 활성 WireGuard 인터페이스 wg0만 존재, peer AllowedIPs `10.0.0.100/32`, Endpoint `43.201.227.58:51820`, latest handshake epoch 0 확인. `ip route get 10.0.0.100`은 wg0/src 10.0.1.1이고 `10.0.0.1`은 핫스팟 게이트웨이 경유 wlan0. 핫스팟 주소는 당시 관측값.
- PID 3087 heartbeat는 5초마다 연결 타임아웃을 보고했다. 현재 heartbeat 기본 `10.0.0.1:3000`과 운영 VPN 목적지 `10.0.0.100`을 구분한다. 후속 ping 확인 결과는 미수신. 이번 확인에서 WireGuard 및 heartbeat 설정은 변경하지 않았다.

## 보드 백업 목록 확인 — 2026-11-09

- USB ADB 읽기 전용 조회로 백업 7개 항목, 총 138 KiB 확인. `/userdata` 90 KiB, `/oem` 28 KiB, `/` 20 KiB. 당시 `/userdata` 사용률 26%, 여유 약 169 MiB. 상세 경로·용도·정리 기준은 `docs/BOARD_BACKUP_INVENTORY.md`에 기록.
- `/heartbeat.conf` 존재 및 서비스 경로 `/etc/heartbeat.conf` 부재 확인. 구 `/userdata/rkipc.ini.bak`는 11월 9일 시계 백업 내 사용자 INI와 내용 동일. 이번 조회는 보드 파일 이동·삭제 없이 진행.

## 새 heartbeat 보드 교체 및 설정 파일 없이 서비스 시작 확인 — 2026-11-09

- 사용자 ADB 로그에서 새 ARM 바이너리 `/tmp/i1-heartbeat.new --dry-run` 성공. `wlan0`의 대문자 MAC `7424CA5004AF`, `rtsp_ok=false`, `wg_ok=false` 출력. `wg_ok`는 이번 실행 기본값 `wg1` 상태를 반영하므로 다른 인터페이스의 VPN 연결 여부는 미확인.
- 기존 `/usr/bin/i1-heartbeat` 및 `/etc/init.d/S60heartbeat`를 `/userdata/heartbeat-backup-20261109-090040`에 백업한 뒤 새 파일로 교체. 서비스 시작 PID 2812 확인. 이후 `/etc/heartbeat.conf` 제거 및 서비스 재시작 뒤 `heartbeat running PID=3087` 확인. 별도 설정 파일 없는 수동 서비스 시작까지 검증.
- `mv /etc/heartbeat.conf "$backup/heartbeat.conf"`는 `backup`을 정의한 서브셸 밖에서 실행되었으므로 파일이 존재했고 변수 값이 비어 있었다면 `/heartbeat.conf`로 이동했을 수 있음. 실제 이동 대상은 확인되지 않았다. 서버 HTTP200 수신과 재부팅 자동 전송은 아직 미확인.

## heartbeat 기본 전송 및 설정 파일 선택화 — 2026-11-09

- 새 소스는 인자 없이 실행하면 5초 간격으로 `10.0.0.1:3000`에 전송한다. `--send`와 주소·인터페이스 옵션은 기존 호출과 호환된다. `--dry-run`을 명시하면 기본 한 번 JSON을 출력한다.
- `S60heartbeat`는 설정 파일 없이 부팅 시 시작한다. 기본 MAC은 보드 등록과 일치하는 `wlan0`, `wg_ok` 검사 기본값은 `wg1`이다. 기존 `/etc/heartbeat.conf`가 있으면 기존 주소·인터페이스·`ENABLED`를 존중한다. 실제 전송 경로는 보드의 라우팅을 따른다.
- 문서와 전달 ZIP 갱신. Windows 프로토콜 테스트, 셸 문법 및 설정값 확인, ZIP CRC와 소스 대조 통과. 새 ARM 바이너리 빌드와 보드 부팅·전송 확인은 남아 있다.

## heartbeat 전송 기본 주소 설정 — 2026-11-09

- 사용자 요청에 따라 로컬 `i1-heartbeat` 소스와 `S60heartbeat`, `heartbeat.conf`의 기본 목적지를 `10.0.0.1:3000`으로 설정. CLI는 `--send`로 전송을 시작하며 `--host`와 `--port`로 덮어쓸 수 있다. 옵션 없는 실행은 dry-run이고 서비스 기본값은 `ENABLED=0`이다. 운영 서버의 VPN 주소 `10.0.0.100`은 11월 6일 공지에 따른 별도 값이다.
- README와 전달 ZIP도 갱신. Windows 프로토콜 테스트, 셸 문법 및 설정값 확인, ZIP 12개 항목 CRC·소스 바이트 대조 통과. 새 ARM 바이너리 빌드와 보드 적용은 미수행.

## rkipc 자체 NTP 비활성화 및 재부팅 후 UTC 확인 — 2026-11-09

- USB ADB에서 Luckfox Pico Zero 모델 확인. 변경 전 부팅 약 119초에 Windows와 보드 UTC epoch가 같은 초, 시스템 `ntpd` PID 275, `rkipc` 미실행. IMX415용 `/oem/usr/share/rkipc-800w.ini`와 `/userdata/rkipc.ini` 모두 `[network.ntp] enable = 1`, 주기 60초. `RkLunch.sh`에서 IMX415용 원본 선택과 원본 설정 복사 경로 확인. 당시 `/tmp/rkipc-factory-config.ini` 링크는 존재하지 않았음.
- 제조사 공개 `project/app/rkipc/rkipc/common/network/ntp.c`의 `rkipc_ntp_update()`에 `offset + 28800` 및 `busybox date -s` 확인. 해당 경로의 시계 덮어쓰기를 방지하도록 두 INI를 `/userdata/rkipc-clock-backup.qzDH9v/`에 백업한 뒤 `[network.ntp]`의 `enable`만 `0`으로 변경하고 `sync`. 시스템 `ntpd` 유지.
- ADB 재부팅 후 8초 시점에 시계가 2021년이었고, 약 39초에 Windows UTC와 같은 초로 보정. 이후 약 69·99·129·159초에도 동일 초 또는 PC 조회 시간 범위 내 일치. 모든 표본에서 두 INI의 `enable = 0` 유지, `ntpd` PID 279 유지. 시계가 부팅 직후부터 유지되는 RTC 문제는 별도.
- `rkipc`는 이번 재부팅 표본에서 실행되지 않았다. 확인 범위는 설정의 재부팅 유지와 시스템 UTC 자동 보정 및 관측 구간 유지까지이며, `rkipc` 동작 중 +8시간 재발 여부와 실제 영상은 후속 검증 대상이다.

## 우리 보드 시계 오차 재확인 — 2026-11-09

- 11월 5일 `date -u`에서 Windows보다 약 8시간 앞선 결과가 있었고, 11월 6일에는 같은 `ntpd` PID를 유지한 채 부팅 초기 2021년에서 정상 UTC로 자동 보정된 결과가 있었다. 자동 보정 후 `rkipc`가 시간을 다시 덮어쓰는지는 우리 보드에서 아직 확인하지 않았다.
- Windows에서 ADB 데몬을 시작한 뒤 `adb devices -l`을 실행했으나 장치 목록이 비어 있었다. 이번 점검에서는 보드에 접속하거나 시계·설정을 변경하지 않았다.
- 다음 점검은 보드와 Windows의 UTC 비교, `ntpd` 및 `rkipc` 프로세스, `/tmp/rkipc-factory-config.ini` 실제 대상과 `/userdata/rkipc.ini`의 `[network.ntp]` 값 확인이다. 우리 보드에서도 자동 보정 이후 +8시간 재발과 `rkipc` 자체 NTP 사용이 확인되면 실제 원본과 사용자 ini를 백업하고 해당 섹션의 `enable`만 0으로 조정한다. 재부팅 후 몇 분간 UTC를 비교해 초기 보정과 +8시간 재발 여부를 구분한다.

## 조장 BoardServer 저장소 검토 — 2026-11-09

- `r1n90m0d/BoardServer` main `6eb3ad5c348f6127a0096fe47ed5d25d33f33edc`(2026-11-04)를 `references/BoardServer-review`에 읽기용으로 복제. 보드용 heartbeat, 시작 스크립트, ADB 설치 스크립트, 모의 수신기와 문서 확인. 이번 검토는 정적 소스 확인이며 빌드·ADB 적용·운영 호출은 미수행.
- 조장 heartbeat는 libcurl로 `/v1/devices/heartbeat`에 5초 간격 전송. wlan0의 대문자 MAC 12자리와 status/rtsp_ok/wg_ok/config_version 포함. 연결 2초·전체 3초 제한, `HEARTBEAT_URL` 기본 `http://10.0.0.1:3000/v1/devices/heartbeat`, `HEARTBEAT_IFACE` 기본 wg1 및 libcurl 인터페이스 고정. 이는 소스 기본값이며 11월 6일 운영 서버 공지의 VPN 주소 10.0.0.100과 구분. rtsp_ok는 rkipc 프로세스, wg_ok는 인터페이스 UP을 검사.
- `deploy/install_board.sh`는 ADB 사전 검사 및 최초 1회 백업 후 factory ini와 `/userdata/rkipc.ini`의 rkipc NTP 비활성화, `dhcpcd.conf`에 `denyinterfaces wg*` 적용, `/usr/bin/heartbeat` 및 `/etc/init.d/S60heartbeat` 설치. dry-run/시리얼 선택 지원. WireGuard 키·피어 구성은 별도. 문서상 기존 적용 보드 재실행만 검증됐고 새로 재플래싱한 보드 첫 설치는 미검증.
- `docs/rkipc-clock-fix.md`는 조장 환경에서 rkipc NTP의 하드코딩된 +8시간과 부팅 시 factory ini 복사 동작을 기록. 시스템 ntpd와 구분하며 호종님 보드에도 동일한 수정이 필요하다고 단정하지 않는다.
- 우리 저장소도 heartbeat 구현을 포함하나 기본 비활성 설정 및 `/usr/bin/i1-heartbeat`를 사용. 두 버전이 동일한 `/etc/init.d/S60heartbeat` 설치 경로를 사용하므로 조장 설치 스크립트를 실행하면 기존 시작 스크립트 교체 가능. BoardServer의 Python 수신기는 200/403/무응답 모의 시험용이며 실제 AWS API·DB·웹·영상 구현은 이 스냅샷의 파일 목록에 없음.

## WebDashboard 공개 저장소 검토 — 2026-11-06

- 사용자 제공 https://github.com/moonjuyoung727/WebDashboard 의main6e3dc54091173e3dec250c54b5cdd4be4c229ec1(2026-11-04) 확인. 승인된clone으로references/WebDashboard-review 보관. 원격/로컬소스수정없음,빌드/실서버호출/화면검증미실행. 상세 docs/WEBDASHBOARD_REVIEW_2026-11-06.md.
- React프런트이며로그인/가입/BearerJWT/GET devices/대문자MAC/UTC→KST구현확인. heartbeat수신/DB는별도서버. 웹status는서버값사용,rtsp_ok/wg_ok직접표시매핑없음.
- 저장소.env API base는10.0.0.1:3000/v1로최신운영VPN10.0.0.100과차이. 실제배포env는미확인. Dashboard10초조회,다른목록기본응답완료후5초조회. 최초실패시목업/후속실패시캐시표시.
- DeviceManage등록/삭제/수정화면만반영,VPN관리VPN_API_ENABLED=false,이벤트/통계등목업잔존. WHEP훅은있지만목록매핑이영상URL누락하여현재실제영상연동완료아님. 로그인이후/status계약및일부요청타임아웃,계정간공유캐시초기화추가검토필요.

## 보드1 부팅 후 자동 UTC 보정 확인 — 2026-11-06

- 사용자 pidof ntpd275 유지, 보드 UTC2026-11-06 11:12:20 및 Windows UTC11:12:23 확인. 순차조회3초차이로 현재시각일치수준확인, 정밀3초오차로해석하지않음. 같은PID가부팅후2021시각에서현재UTC로보정됐으며이번시험중수동재시작보고없음.
- 보드1 NTP자동시작 및 네트워크연결후자동보정 확인. NTP설정/부팅스크립트수정불필요. 부팅직후부터정확한시간을갖는RTC유지요건은별개로미충족/미검증(초기2021관측); 두번째보드및정밀동기상태/장시간안정성은미검증.

## NTP 로그 날짜 전환 및 서버 패킷 수신 관측 — 2026-11-06

- 사용자 경로 default via10.32.0.28 wlan0 src10.32.0.102, DNS10.32.0.28 확인. ntpd275 시작은2021-01-01 12:04:40, wlan0 .101등록9초후/.102등록15초후. 이전date12:04:58은시작후18초였으므로시각의12:04를부팅후4분으로해석하면안됨.
- 후속로그Nov6 11:08:50에194.0.5.123로부터Unexpected origin timestamp 수신 기록. 로깅시계 날짜/시간이 바뀌고 NTP서버 패킷수신은 확인되나 이행만으로동기선택/정확도/안정상태확정불가. 해당패킷의요청응답타임스탬프불일치이며재시작필요확정아님.
- nslookup0.pool.ntp.org는DNS10.32.0.28의No answer. 해당조회실패사실이며모든DNS/전체인터넷불능확정아님. 다음은date-u/PC UTC같은시점비교,PID유지및최신로그,필요시다른설정NTP호스트조회. 설정변경/NTP재시작미실행.

## 핫스팟 연결 재부팅 후 NTP 미보정 — 2026-11-06

- 사용자 재부팅 후 wlan0 10.32.0.101/24 단일IPv4, pidof ntpd275, date -u2021-01-01 12:04:58 확인. 정상대역주소는 있으나UTC미보정. 새부팅PID이며Wi-Fi연결완료후경과시간/실제인터넷접근/현재NTP로그는미확인.
- 다음은 ip route, resolv.conf, 현재NTP로그, nslookup 가용시0.pool.ntp.org 조회. NTP설정/부팅순서를 원인확인없이 수정하지 않음. DNS성공은UDP123응답증거가 아니며VPN핸드셰이크도외부NTP연결과구분.

## NTP 자동실행 유지, wlan0 링크로컬 주소 관측 — 2026-11-06

- 사용자 pidof ntpd281, date -u2021-01-01 12:03:06. 시작로그12:00:03부터약3분 경과해도 시간보정미완료. 부팅자동실행 자체는 성공.
- 제공 로그 마지막 wlan0 IPv4는169.254.156.212(12:00:19), 정상 핫스팟 DHCP주소 기록없음. 이는 링크로컬 주소이며 정상IPv4할당/외부NTP접근문제 가능성이 우선. 현재인터페이스목록/기본경로/WPA상태를 아직 받지 않아 현상태나원인은 확정하지 않음. wg0의10.0.1.1주소존재는외부연결증거아님.
- 다음은 wpa_cli status, ip -4 addr show dev wlan0, ip route, resolv.conf 조회. 설정변경·NTP재설치·시계강제설정 없이 네트워크상태부터 진단. 기존SSID/비밀번호 재작성이나DHCP클라이언트중복시작 지시하지 않음.

## Discord 후속 확인: 상태 연동 성공 보고 및 11월 6일 서버 변경 — 2026-11-06

- 사용자 요청에 따라 현재 열린 캡스톤 #중계서버및db 채널을 아래로 스크롤하여 11월5일 첫 메시지15:47~마지막22:36 및 다음11월6일 날짜경계 확인. 이 채널의 해당일 텍스트 대화 읽기 완료. 다른 채널/첨부이미지 작은글씨 확대검증은 범위 밖. 상세 DISCORD_WORK_LOG_2026-11-05.md. 모두 팀원 채팅 보고이며 이번 직접 장비 재검증 아님.
- 아래 부분확인의 21:06 이후 미확인은 이번에 해소: 21:42~21:44 서버 HTTP500 소멸 및 정태석 성공 보고, 손우승이 DB last_seen 5초 갱신 보고. CA 적용의 정확한 변경 내용과 원시 HTTP200은 여전히 미확인.
- 21:46 웹 목록은 빈 상태. 21:50 등록 API devices.device_id NOT NULL 위반23502 공유. 21:54 이메일 대신 실제 사용자UUID를 user_id에 연결하자는 정정 후 21:55 손우승 수동변경. 21:56 문주영 보드1 목록 표시 확인. 등록 API 자동생성/자동매칭 수정완료 의미 아님.
- 21:56~22:00 정태석 보드1 전원차단→웹offline, 재로그인후offline 유지, 전원복구→online, 재차차단/복구→offline/online 반복확인 보고. 최대20초는 정태석 예상이며 실측시간 아님. 오호종 보드2의 동일시험 완료로 확대하지 않음.
- 22:01~22:36 비등록VPN IP heartbeat403 시험은 미완료. 정태석123.123.123.123 변경 보고, 서버의 remote_ip10.0.2.1 요청은 웹 가능성. 서버wg AllowedIPs10.0.1.0/24(정태석 peer), 다른peer10.0.2.0/24 공유. handshake 성공만으로 HTTP요청 전달/인증 성공 판정하지 않음. 최종403 응답 관측 없음.
- **최신11월6일12:55~12:59 공지:** AWS 운영용 계정으로 이전하여 서버 공인IP **43.201.227.58**, WireGuard 공개키 **oeicknZNeigmFxsbAhAQz+ZUbzcSiPn92rVnwgICez4=**로 변경. 서버VPN **10.0.0.100 유지**. DB엔드포인트도 변경했으나 구체값미제공. 아래3.27.114.132 및 w8K…키는 과거서버값. 보드의 새서버 설정 적용/통신은 미검증이며 이번 작업에서 설정변경하지 않음.

## Discord 11월 5일 대화 부분 확인 — 2026-11-06

- 캡스톤 서버 #중계서버및db 앱 화면에서 2026-11-05 15:47~21:06 대화 확인. 전체 날짜·채널·첨부 이미지 확인은 미완료. 상세 기록: DISCORD_WORK_LOG_2026-11-05.md. 팀원 보고와 이번 직접 장비 재검증을 구분한다.
- 김동욱 15:47: 공용 RDS postgres의 users/devices 생성, 보드 CAM-E099(MAC7424CAAAE099, VPN10.0.1.2) 및 CAM-04AF(MAC7424CA5004AF, VPN10.0.1.1) 등록·조회 완료 보고. device_id PK, mac CHAR(12) UNIQUE NOT NULL 및 형식검사, vpn_ip UNIQUE, 버전2필드 기본0, user_id NULL.
- 손우승 17:28 서버 VPN10.0.0.100/24 공지. 공인Endpoint3.27.114.132:51820은 당시 공유값. 기존 실서버 WireGuard 공개키와 일치. Windows mock10.0.0.1과 구분.
- 문주영 20:35 로그인 CONNECTION_REFUSED 후 20:55 로그인 성공 보고, 손우승 20:56 성공 로그 공유. 로그인 성공을 heartbeat/보드목록/DB 갱신 성공으로 확대하지 않음.
- 정태석 20:58 본인 보드 handshake·ping3/3·health 성공, 전원차단후 MAC/시계/자동실행 정상 보고. heartbeat HTTP500, SELF_SIGNED_CERT_IN_CHAIN 오류 및 서버→RDS CA 신뢰 문제 분석/해결안 제안. 오호종 보드 NTP 상태와 혼합하지 않음.
- 손우승 21:00 5초마다 POST 수신 보고. 보드 조회·저장 로직 관련 논의 중 문주영 21:06 웹 보드목록에서도 동일 오류 보고. 21:06 이후 해결 결과는 아직 미확인이며, CA 적용·스키마 추가·heartbeat200·DB갱신 완료로 기록하지 않음.

## 재부팅 후 ntpd 자동시작 확인, 초기 동기화 대기 관측 — 2026-11-05

- 사용자 첨부 로그: 수동start 없이 ntpd PID283 유지. 시작13:52:35, 두번째date13:53:02(2021-01-01)로 시작후27초 관측. 두date 간15초이며 이전 안내의1분후 확인이 완료된 로그는 아님. 자동실행 누락이 아니라 초기 시간 동기화 미완료 상태로 분류.
- 시작시lo만,9초후wlan0 .101 및wg0,16초후wlan0 .102 추가; NTP peer121.134.215.104 local addr .101→.102 로그. 늦게 생긴 인터페이스를 ntpd가 감지함. 네트워크 준비 전 시작했다는 이유만으로 실패 확정하거나 시작순서 변경하지 않음.
- 현재RTC/부팅시계가2021로 시작한 사실은 확인하나 원인/RTC영속성은 미확정. 다음은 서비스 재시작 없이 추가60초 대기후pidof/date/NTP로그 확인. 동기화 이후 WindowsUTC 비교. 지속실패 시 서버응답/DNS/경로 등 추가조회.

## NTP 부팅 등록 및 RTC 초기화 코드 확인 — 2026-11-05

- 사용자 S49ntp 실행권한755 및 rcS의 /etc/init.d/S??* 순차 start 실행 확인. 자동시작 등록 누락 가설은 해소. S49ntp는 S99wlan0보다 먼저 호출되지만 이것만으로 ntpd 종료 원인을 확정하지 않음.
- S99rtcinit는 hwclock 출력에1969가 포함될 때만 date -s2024-01-01 및 hwclock -w 실행, 그 외에는 메시지만 출력. 항상 RTC를 시스템시계로 읽는 코드가 아니며 약8시간 오차 원인으로 단정할 수 없음. 해당 조건이 실제 부팅에서 성립했는지는 미확인.
- grep의 NTP/시간관련 결과는 위두스크립트만. 다음은 설정변경 없이 재부팅직후 pidof/date/부팅NTP로그 및 네트워크연결후 재확인. 실패원인 확인 전 S49 이름변경/중복시작 추가/RTC쓰기 수행하지 않음.

## NTP 재수동 시작 후 UTC 재보정, 부팅 경로 진단 대기 — 2026-11-05

- 사용자 S49ntp start OK, PID2808, 보드09:43:38 UTC 및 Windows09:43:50 UTC 보고. 조회시점이 달라12초를 실제오차로 단정하지 않음. 시작로그17:43:25와 이후UTC09시대에서 약8시간 보정 재관측.
- 시작 직후 Clock Unsynchronized 로그만 있어 지속실패 증거 아님. 현재 서버목록 변경 필요성은 확인되지 않음. 부팅후 자동실행 여부/종료원인 및 RTC에서 로드되는 시간은 아직 미확정.
- 요청했던 S49ntp 실행권한과 rcS 본문은 미수신. 다음 읽기전용 조회는 ls -l S49ntp, rcS, S99rtcinit 및 date/hwclock/NTP 관련 부팅스크립트 검색. 부팅 Wi-Fi 준비전 실행/DNS 또는 후기RTC덮어쓰기 등은 아직 가설이며 임의 스크립트수정하지 않음.

## 실서버 VPN 변경 후 NTP 확인 재개 — 2026-11-05

- 최근 사용자 출력에서 wg0/10.0.1.1/24, 목적지10.0.0.1 경로dev wg0 src10.0.1.1 확인. peer w8KflU5qvlksTzCMxgZXRk78Viy23I5j4RhIVz8kCS0=, Endpoint3.27.114.132:51820, 최근handshake28초 및수신증가 확인. ping 무응답 원인은 미확정이며 이전 Windows 테스트 구성과 구분.
- 사용자 NTP 작업 우선 요청. 이전 ntpd 수동시작(PID2908) 후 약8시간 오차 보정 관측은 있으나 현재 재부팅후 자동시작/UTC는 미확인. 다음은 pidof/date와 부팅스크립트 실행권한/rcS 조회, 미실행 시 기존 S49ntp start. 설정 덮어쓰기나 시간 하드코딩 없이 진행.

## NTP 수동 시작 후 UTC 보정 관측 — 2026-11-05

- 사용자 S49ntp start OK, pidof ntpd2908 유지. /etc/default/ntpd cat 출력없음(부재/빈파일 구분 안 됨). 시작 후 date -u는2026-11-05 09:00:33 UTC로 앞선 약8시간 오차가 보정된 상태 관측. 동일시점 PC 비교/ntpq 동기선택 출력은 아직 없음.
- 로그는 ntpd4.2.8p15, /usr/sbin/ntpd -g -p /var/run/ntpd.pid, 인터페이스 리슨 정상 및 시작 시각17:00:25의 Clock Unsynchronized 메시지. 시작 직후 로그이므로 현재 지속 실패 증거로 해석하지 않음. 조건부 ntpq -pn 출력없음으로 도구 부재 여부 미확정.
- 설정파일 변경 없이 기존 서비스 시작만 수행한 사용자 결과. 다음은 Windows UTC 재비교 및 재부팅 후 직접start 없이 ntpd PID/UTC 확인(부팅직후와 네트워크연결 후 구분). 자동실행 실패 원인, RTC 유지 여부, 두번째 보드는 미확인.

## 보드 UTC 약8시간 오차 및 기존 NTP 구성 확인 — 2026-11-05

- 사용자 Windows UTC2026-11-05 08:58:23, 보드 date -u16:58:35 UTC 비교. 명령 간 시차 포함 약8시간 빠름. KST9시간 차이나 특정 RTC 원인으로 단정하지 않음.
- /usr/sbin/ntpd 존재. S49ntp는 기본 NTPD_ARGS=-g, /etc/default/ntpd가 있으면 override 후 start-stop-daemon으로 시작. /etc/ntp.conf에는0~3.pool.ntp.org iburst 및 restrict 설정 있음. 직전 pidof ntpd는 없음.
- 다음은 /etc/default/ntpd 확인, 기존 S49ntp start 후 pidof/date 및 ntpq가 있으면 -pn 확인. 설정파일 변경 없이 기존 서비스 시작으로 진단. 프로세스 실행만으로 동기화 완료 판정하지 않으며 실패 시 로그 확인. 부팅 당시 미실행 원인과 재부팅 후 시간 유지/동기화는 미확정.

## 보드1 프로세스·VPN·UTC 점검 — 2026-11-05

- 사용자 pidof: i1-heartbeat546, rkipc673, ntpd 출력없음. 현재 ntpd 프로세스 미실행이며 다른 시간동기 방식/일회성 동기 여부는 아직 미확인. 두보드 시계설정/ntpd 상시실행 체크 완료 아님.
- date -u 출력 Thu Nov5 16:57:21 UTC2026. 동일 시점의 신뢰할 수 있는 UTC 비교값이 없으므로9시간 오차나 정확성을 단정하지 않음. Windows UTC와 나란히 비교 필요.
- 실제 VPN 인터페이스는 여전히 wg1,10.0.1.1/24. peer 기존 Windows키,Endpoint10.32.0.31:51820,최근handshake31초,rx17.43KiB/tx162.61KiB,keepalive25초. wg0 변경은 적용됐다고 가정하지 않음. 실제 공인서버가 아닌 Windows 핫스팟 테스트 구성.
- wlan0 .101 forever 및 .102 dynamic 이중주소가 여전히 존재. 현재 시간점검 범위에서 임의 변경하지 않음. 다음은 /etc/init.d/S49ntp 및 NTP설정/명령 조회, Windows UTC비교. 설정 수정/시간 강제설정/서비스시작 미실행.

## 카메라 연결 후 rtsp_ok true 및 서비스 수신 확인 — 2026-11-05

- 사용자 카메라 연결 후 Windows 모의 서버 로그에서 mac7424CA5004AF/status online/rtsp_ok true/wg_ok true/config_version0의 POST 및HTTP200을2회 확인(16:56:33/38, monotonic15450.859/15455.921, 간격5.062초). 앞선 서비스 PID2680 시작 이후 후속 수신으로 수정본 서비스 전송 확인.
- 기존 rkipc 존재 판정 코드 기준 true 분기 실기 확인. 앞서 rkipc 미실행 ps 및 false 출력과 함께 양 상태 확인. 이번 메시지에는 별도 rkipc ps 출력은 없으며 true를 실제 RTSP 영상수신/인코더 정상 검증으로 확대하지 않는다. 실행 후 종료하여 다시false로 돌아가는 동일 세션 시험은 별도 미수행.
- 보드 heartbeat 로컬 시험은 JSON/주기/200/403/timeout/서버재시작복구/서비스실행/true·false 상태까지 확인. 실제 서버의 VPNIP-MAC 대조/DB/15초offline/클라이언트 화면 연동 및 장시간 안정성은 여전히 별도 검증 대상.

## ICD 수정본 시험 후 heartbeat 서비스 시작 확인 — 2026-11-05

- 사용자 /etc/init.d/S60heartbeat start에서 heartbeat started, status에서 heartbeat running PID=2680 확인. 앞선 대문자 MAC HTTP200 3회 시험 뒤 서비스 재개 상태 확인.
- 이번 메시지는 서비스 프로세스 실행 증거이며 서비스 시작 이후 대문자 MAC 지속 수신 로그는 아직 미제공. 장시간 안정성/새 버전 재부팅 검증 완료로 확대 해석하지 않음. Windows 수신창에서 약5초 주기 대문자 MAC 요청이 계속 들어오는지 확인하면 서비스 전송까지 검증 가능.

## ICD 대문자 MAC 요청 HTTP200 수신 3회 확인 — 2026-11-05

- 사용자 Windows mock 로그에서16:50:42/47/52 보드10.0.1.1의 POST 3회 및200 확인. mac7424CA5004AF/status online/rtsp_ok false/wg_ok true/config_version0로 ICD 형식 일치. monotonic15100.296/15105.281/15110.250으로 수신 간격4.985초/4.969초.
- 앞선16:48:34의400은 요청본문 미제공으로 원인 미확정. 이전 소문자 바이너리 요청 가능성은 가설일 뿐 확정 아님. 현재200 수신으로 Windows VPN/API 접근은 복구 확인되나 VirtualBox 종료 등 구체적 조치는 사용자 미보고.
- 이번 로그는 서버측 수신/응답 성공 증거. 새 바이너리의 보드 HTTP200 파싱 출력 및 /usr/bin 교체·서비스 재시작 여부는 미제공. 다음은 기존 설치 절차에서 서비스 start/status 및 지속 수신 확인. 이미 실행중이면 start는 중복 실행을 피하는 구현.

## Windows hotspot 활성화 실패: VirtualBox UDP51820 점유 확인 — 2026-11-05

- 사용자 WireGuard 로그에서 0.0.0.0:51820 bind 실패(0xc0000043) 후 터널 종료 반복. 앞선 Python bind10.0.0.1 WinError10049와 연결되는 VPN 미활성 상태.
- 승인된 읽기 전용 Windows 조회에서 UDP0.0.0.0:51820 OwningProcess27788 확인. 같은 PID의 프로그램은 VirtualBoxVM.exe(C:/Program Files/Oracle/VirtualBox/VirtualBoxVM.exe). 포트 충돌 점유 주체 확인. VM의 NAT 포트포워딩 규칙은 직접 조회하지 않아 해당 규칙 원인은 추정으로만 유지.
- 다음 안내는 Ubuntu VM 정상 종료 후 hotspot 재활성화 또는 VM을 계속 쓸 경우 VirtualBox의 호스트UDP51820 포워딩 규칙 확인·해당 규칙 조정. 어떤 프로세스도 강제종료하거나 보드/Windows 포트·키를 변경하지 않음.

## ICD 대문자 MAC 수정본 보드 dry-run 성공 — 2026-11-05

- 직전 무출력 원인은 사용자 전송 로그상 heartbeat 실행파일(35016바이트) 대신 heartbeat.conf(205바이트)를 전송한 것으로 확인. 올바른 실행파일 재전송 후 /tmp/i1-heartbeat-new --dry-run --count 1 --mac-interface wlan0에서 START/JSON/STOP 정상 출력.
- 실제 payload mac=7424CA5004AF, status=online, rtsp_ok=false, wg_ok=true, config_version=0 확인. 새 ARM 바이너리 실행 및 ICD 대문자 정규화 성공. rkipc true 판정/실제 영상 상태 확인과 구분.
- 다음은 기존 heartbeat 서비스 중지, 기존 실행파일·설정 백업, /usr/bin/i1-heartbeat 교체 후 갱신된 Windows mock 정상 모드에3회 전송 확인, 서비스 재시작. 아직 영구 실행파일 교체 및 새 HTTP200 출력은 미수신. 기존 heartbeat.conf/S60heartbeat/WireGuard 설정 유지.

## ICD 수정 바이너리 교체 전 dry-run 무출력 — 2026-11-05

- 사용자 /tmp/i1-heartbeat-new --dry-run --count 1 --mac-interface wlan0 실행에서 START/오류 출력 없이 프롬프트 복귀. 정상 성공으로 간주하지 않음. 파일 크기/형식/종료코드/전송결과 미확인.
- 다음은 보드 ls -l, wc -c, od로 ELF 매직 확인 및 재실행 직후 종료코드 확인, VM dist/heartbeat 크기와 비교. 빈 파일/다른 파일/불완전 전송 가능성은 가설. 기존 서비스 중지나 /usr/bin 실행파일 교체를 진행하지 않도록 안내.

## ICD I1 발췌본에 따른 heartbeat 형식 정합 — 2026-11-05

- 사용자 첨부 ICD Iteration1 발췌(작성2026-11-03)를 7페이지 전체 텍스트 및 관련 표 이미지로 검토, docs/ICD_I1_EXCERPT_2026-11-03.pdf에 원본 보관. 사용자 요청은 이 문서에 형식을 맞추는 것.
- MAC은 문서2페이지 대문자12자리 예시,6페이지 DB CHECK [0-9A-F]{12}로 명확하여 기존 소문자 합의/테스트를 이번 요청 기준으로 대체. protocol.c의 toupper, 테스트 기대값, README, mock 검증을 갱신. 현재 보드 MAC 전송 표기는7424CA5004AF, 서버 device_id 예시는CAM-04AF이나 heartbeat에는 device_id를 추가하지 않음.
- 필수5필드/mac,status,rtsp_ok,wg_ok,config_version 및 타입은 기존과 일치. config_version 요청0 고정,200응답 버전 로그만 유지. 5초주기/전체3초timeout/토큰없는VPN식별 유지. 선택필드3개는 보드에서 생략하고 mock은타입검증후허용.
- mock 정상응답 기본version0으로 변경(--config-version 7로 비영 파싱시험 가능). 400/403 오류를 error_code/message JSON으로 통일. 구체적인 heartbeat 오류문구/코드 문자열은 문서에 없어 BAD_REQUEST/FORBIDDEN은 시험용이며 실제서버 문자열 확정아님. 보드에 HTTP400 점검 로그 추가, 로그 자체는 ICD규정이 아님. 오류본문 message/details 추출은 미구현이며 재시도는 상태코드 기준.
- rkipc 존재/wg1UP의 기존 상태판정 유지. 발췌본의 정상여부 표현만으로 미검증 영상/핸드셰이크 검사 기능을 추가하지 않음. 서버 등록DB 인증과15초offline/클라이언트5초조회는 모의서버/보드 구현 범위밖.
- MSVC 프로토콜 테스트 통과, 로컬 HTTP200/400/403·대문자·타입·필수/선택필드 시험 통과. 첫 로컬 소켓시험은 샌드박스10013 차단, 승인된 재실행 성공. ZIP12파일 CRC/바이트일치검사 완료. 이전ZIP은 backups/heartbeat-before-icd-i1-20261105에 보존. 새 ARM빌드 및 보드전송은 미검증. 보드카메라/네트워크/서비스 설정은 변경하지 않음.
- 직전 센서 진단 후 사용자가 카메라를 연결하지 않았던 사실을 확인. 실제 연결 후 rkipc true 결과는 아직 미수신. 기존 드라이버 진단 가설을 고장확정으로 해석하지 않는다.

## 센서 모듈 로딩 후 제거 경로 확인 — 2026-11-05

- 사용자 insmod_ko.sh 전체에서 __insmod imx415.ko 호출 및 후속 __rmmod_camera_sensor 확인. 후자는 lsmod 세번째 필드가0인 센서 모듈을 rmmod한다. 따라서 현재 imx415 미표시는 초기 로딩 실패뿐 아니라 로딩 후 refcount0 제거 가능성도 있음. refcount0만으로 물리 카메라 미연결 확정 금지.
- /oem/usr/ko/imx415.ko 및 imx415.ko.before-mode30 존재 확인. 백업 파일 존재만으로 현재 모듈이 수정본이거나 원인이라고 단정하지 않음. /ko/insmod_ko.sh cat 출력은 없음.
- 다음 읽기 전용 진단: uname -r, 커널 모듈/센서 probe 관련 dmesg, /proc/rkisp-vir0 또는 /proc/rkisp0 상태. 드라이버 수동 로딩/백업 복원/스크립트 수정은 미실행.

## IMX415 sysfs 드라이버 항목 및 로그 미확인 — 2026-11-05

- 사용자 /sys/bus/i2c/drivers/*imx415* 조회는 없음, dmesg | grep -i imx415도 출력 없음. 앞선 lsmod 결과와 함께 현재 표준 imx415 이름의 드라이버 등록을 확인하지 못함. dmesg 무출력만으로 부팅 때 드라이버 로딩 시도가 없었거나 하드웨어가 고장났다고 확정하지 않음.
- 다음은 /ko, /oem/usr/ko의 존재와 insmod_ko.sh 내용, imx415 관련 모듈 파일을 조회. insmod/modprobe 실행, 시작 스크립트 수정, 설정 변경은 하지 않음. 현재 요청의 rkipc 미실행 false 판정은 확인됐고 true 판정은 미검증으로 유지.

## rkipc 기본 설정 링크 누락 및 IMX415 모듈 미표시 — 2026-11-05

- 사용자 출력: /tmp/rkipc-factory-config.ini 및 /oem/usr/share/rkipc.ini 없음. /oem/usr/share/rkipc-800w.ini 존재(16567바이트). test -f 결과 config MISSING. lsmod | grep imx415 결과 없음. luckfox-config get_csi_unite는 null byte/fdtdump 경고 후1 출력.
- 현재 파일 상태로 RkLunch.sh의 factory 설정파일 검사에서 종료되어 rkipc 실행에 도달할 수 없음. IMX415 선택 분기는 lsmod 문자열에 의존하므로 현재 모듈 목록으로는 해당 링크를 생성하지 못함. 이번 부팅 당시 상태 및 post_chk 실제 도달 로그는 없으므로 정확한 부팅 시계열 원인은 미확정.
- lsmod 미표시는 커널 내장 드라이버 가능성도 있어 센서 미인식 확정 아님. 다음은 /sys/bus/i2c/drivers의 imx415 항목, dmesg 및 /proc/config.gz가 있으면 CONFIG_VIDEO_IMX415 확인 등 읽기 전용 진단. csi_unite 변경/링크생성/모듈삽입/재부팅은 수행하거나 지시하지 않음. rkipc 미실행에 대한 heartbeat false는 일치.

## RkLunch.sh 본문 검토: rkipc 실행 전 종료 조건 — 2026-11-05

- 사용자 첨부 텍스트에서 전체 RkLunch.sh 읽음. rcS 실행 후 post_chk를 백그라운드 실행하며 마지막에 rkipc(-a iqfiles)를 시작하는 구성. 실제 rkipc 자동 시작 코드 존재 확인.
- /tmp/rkipc-factory-config.ini가 유효한 파일이 아니면 exit. /oem/usr/share/rkipc.ini가 없을 때만 센서 모듈에 따라 factory 링크를 생성하며 IMX415는 rkipc-800w.ini 선택. 기존 share/rkipc.ini가 있는 경우 본문에 factory 링크 생성 else가 없어 누락 가능성이 있으나 보드 파일 상태 미확인.
- IMX415 모듈의 lsmod 세번째 필드가1인지와 get_csi_unite 값에 따라 set_csi_unite 후 return하는 분기 존재. 이 경로는 rkipc 실행을 건너뛰고 재부팅 필요 메시지를 출력하며 스크립트 자체에서 reboot를 호출하지 않음. 사용자 환경에서 해당 분기가 실제 발생했는지는 미확정.
- 다음 읽기 전용 확인은 factory/share/800w 설정파일 ls, lsmod의 imx415, luckfox-config get_csi_unite. 설정 수정/스크립트 실행/재부팅 지시 없음. 원인을 heartbeat 오류로 단정하지 않음.

## rkipc 부팅 호출 경로 확인, 읽기 전용 진단 — 2026-11-05

- 사용자는 제조사 보드 설정을 함부로 수정하지 않기를 명확히 요청. rkipc 진단은 조회만 진행하고 제조사 스크립트 수정/수동 실행/카메라 설정 변경 없이 원인 확인한다.
- 사용자 S21appinit 출력: RkEnv.sh가 있으면 source하며 start에서 sh /oem/usr/bin/RkLunch.sh, stop에서 RkLunch-stop.sh 호출. S99luckfoxconfigload는 luckfox-config load 호출. grep에서도 S21appinit 호출 확인.
- 부팅용 시작 스크립트의 호출 구성이 확인됐으나 실제 이번 부팅에서 실행 완료 여부와 rkipc가 없는 원인은 미확정. 다음은 RkLunch.sh 전체 및 rcS/S21appinit 실행권한을 읽어 시작 조건/조기종료/로그 경로 확인. 자동 시작 설정 자체가 없다고 해석하지 않는다.

## rkipc 미실행 시 false 판정 확인 — 2026-11-05

- 사용자 ps 필터 결과 rkipc/cctv_person/simple_vi 없음. /usr/bin/i1-heartbeat --dry-run --count 1 --mac-interface wlan0 결과 rtsp_ok=false, wg_ok=true 확인. command -v rkipc는 /oem/usr/bin/rkipc.
- 사용자 grep에서 RkLunch.sh는 /userdata/rkipc.ini를 기본설정으로 강제 복사하는 행과 rockchip,unite/재부팅 관련 분기, rkipc -a /oem/usr/share/iqfiles 또는 rkipc 실행 분기가 확인됨. 전체 스크립트 실행은 아직 안내하지 않음. RkLunch-stop.sh는 killall 및 종료 확인 포함.
- 다음은 RkLunch.sh 65~165행과 기존 /userdata/rkipc.ini 존재 확인 후 기존 설정을 보존하는 실행 방법 결정. true 및 종료 후 false 전환은 아직 미검증.

## rkipc 존재 판정 실기 시험 시작 — 2026-11-05

- 사용자 요청: rkipc 실행 여부에 따른 rtsp_ok 판정 확인. 현재 코드의 /proc/PID/comm 정확한 rkipc 이름 비교 및 stat의 Z/X 제외 확인.
- 먼저 보드 ps에서 rkipc/기타 카메라 프로그램 실행 상태와 dry-run 결과 비교 안내. 기존 heartbeat 서비스는 유지하며 dry-run은 전송/PID잠금 없이 실행 가능. 실제 rkipc 시작·종료 및 true/false 전환 시험은 아직 미수행. 다른 카메라 프로세스와의 충돌을 피하기 위해 실행 상태 및 기존 rkipc 시작 스크립트를 확인한 뒤 진행한다.

## RTSP 지표 범위 재확인: rkipc 존재 판정 유지 — 2026-11-05

- 사용자는 실제 송출 실행파일을 아직 정하지 않았고 rkipc 프로세스 실행 여부만 확인하면 되는지 질문. 현재 I1 범위에서는 기존 rkipc 존재 판정이 맞으며 별도 CCTV 실행파일 선택이나 실제 영상 상태 검사는 필요하지 않음을 재확인.
- 직전 추가한 --rtsp-process/RTSP_PROCESS 기능 및 문서 변경은 적용 전 원복했다. backups/heartbeat-before-process-option-20261105의 ZIP에서 소스11파일 및 전달 ZIP을 복원하고 바이트 일치 확인. 현재 전달본은 보드에서 앞서 검증한 rkipc 고정 판정 버전. 직전 기능추가 기록은 철회된 작업 이력이다.
- rtsp_ok=false는 rkipc 부재 시 정상. 프로세스 존재와 실제 RTSP 영상 정상은 구분한다. 다음 작업은 실행파일 명칭 결정이 아니라 기존 요구사항의 미검증 항목(장시간 안정성 또는 실제 서버 연동 등)에서 선택하며, 영상 검증 범위를 임의 확대하지 않는다.

## heartbeat RTSP 프로세스 선택 기능 추가 — 2026-11-05

- 사용자 다음 단계 진행 요청에 따라 --rtsp-process 옵션과 S60heartbeat의 RTSP_PROCESS 설정 추가. 기본 rkipc 유지. 프로젝트 CMake에서 cctv_person_npu 송출 실행파일 확인했지만 현재 보드에서 실행 중인 실제 이름은 사용자에게 질문 중이며 임의 확정하지 않음.
- /proc/PID/exe의 실행파일 basename 전체를 정확히 비교하고 stat의 Z/X 상태 제외. 긴 실행파일명이 /proc/comm에서 잘리는 문제 회피. 프로세스 존재 지표이며 실제 RTSP 프레임/송출 활성/정지 여부 검증은 아님. 기존 소문자 MAC/HTTP 전송/5초 주기는 유지.
- 소스·S60heartbeat·설정 예제·README·전달 ZIP 갱신. 변경 전 ZIP은 backups/heartbeat-before-process-option-20261105에 보존. ZIP CRC/파일 바이트 일치 확인. 새 코드 ARM 빌드 및 보드 true/false 전환은 미검증. 기존 검증된 보드 서비스를 자동 교체하지 않음.

## 재부팅 시험 후 WireGuard·heartbeat 실행 확인 — 2026-11-05

- 재부팅 자동실행 시험 안내 후 사용자가 잘 된다고 보고. wg show wg1에 Endpoint10.32.0.31:51820, AllowedIPs10.0.0.1/32, 최근 핸드셰이크10초, 수신1.18KiB/송신1.75KiB, PersistentKeepalive25초 확인. /etc/init.d/S60heartbeat status는 heartbeat running PID=547 출력.
- 사용자 보고 맥락상 부팅 자동실행 시험 성공으로 기록하되, 직접 출력으로 확인된 것은 VPN 핸드셰이크 및 서비스 실행 상태다. 이번 메시지에 재부팅 직후 Windows HTTP 수신 로그는 없으며 이전 수동 HTTP200 시험과 구분한다. Endpoint는 핫스팟 DHCP 관측 주소로 고정값이 아님.
- 기본 통신/오류복구/서비스 실행 검증 진전. 장시간 CPU·메모리/누수, 현재 CCTV 프로세스에 맞는 rtsp_ok 판정, 실제 서비스 연동은 아직 미검증이며 전체 최적화 완료로 간주하지 않음.

## heartbeat HTTP403 재시도 성공 및 자동실행 설치 안내 — 2026-11-05

- 사용자 보드 --send --host 10.0.0.1 --port 3000 --mac-interface wlan0 --count 3에서 HTTP_403 attempt1~3 출력 후 STOP attempts=3 확인. 모의 서버 forbidden 응답 처리·재시도 성공. 정상200/서버중단복구/3초timeout/403 시험 완료, 실제 서비스 등록 검증과 구분.
- 기존 deliverables/i1_heartbeat/S60heartbeat 검토: /usr/bin/i1-heartbeat, /etc/heartbeat.conf 사용, PID잠금/시작·종료·상태 지원 및 syslog 출력. 기본 비활성/eth0는 배포본에서 유지하고 보드 설치 시 ENABLED=1 HOST=10.0.0.1 PORT=3000 MAC_INTERFACE=wlan0 WG_INTERFACE=wg1 지정 안내.
- 다음은 Windows 모의 서버 ok 모드 복귀, 보드 /tmp의 실행파일과 전송한 S60heartbeat를 영구 경로에 설치 후 수동start/status 및 재부팅 수신 확인. 설치·자동부팅 성공 출력은 아직 없음. S50wireguard 설치 여부 및 wg-quick 가용성도 미확인 상태이며 부팅 후 wg1 확인 필요. S60은 S99wlan0보다 먼저 시작하므로 초기 전송 실패 뒤 다음 주기 재시도는 정상 동작.

## heartbeat 3초 타임아웃 시험 성공 — 2026-11-05

- 사용자 보드 --send --host 10.0.0.1 --port 3000 --mac-interface wlan0 --count 3 출력에서 각 시도 모두 Operation timed out after 3002 milliseconds with 0 bytes received 확인. SEND_FAILED attempt1~3 뒤 STOP attempts=3 정상 종료. 앞서 안내한 Windows 모의 서버 slow 모드 시험에 대한 결과이며 전체 요청 기본3초 제한 적용 확인.
- 다음은 forbidden 모드에서 HTTP403 응답 처리 및 재시도 시험. 현재 로그에는 시각이 없어 요청 시작 간격을 별도로 실측한 것으로 간주하지 않는다. 정상200/서버재시작복구/timeout 확인 완료,403/부팅자동실행/실제서비스연동은 미검증.

## heartbeat 서버 중단·재시작 자동 복구 성공 보고 — 2026-11-05

- 직전 안내한 count0 송신 중 Windows 모의 서버 중단/재시작 시험에 대해 사용자가 성공이라고 확인. 보드 프로그램 재시작 없이 HTTP200 복귀한 것으로 보고 기록. 상세 로그 및 복구 소요 시간은 미제공.
- 다음은 mock_server --mode slow(응답6초 지연)와 보드 기본3초 제한시간 시험, 이후403 재시도 확인. 자동부팅 설치 및 실제 서비스 연동은 아직 미검증.

## heartbeat VPN HTTP 요청·응답 3회 성공 — 2026-11-05

- 사용자 Windows mock_server.py --bind 10.0.0.1 --port 3000 --mode ok에서 보드 10.0.1.1의 POST /v1/devices/heartbeat 3회 수신 및 HTTP200 확인. 수신 monotonic 6217.375, 6222.500, 6227.359로 간격 5.125초/4.859초. 약5초 주기 수신을 관측했으며 장시간 안정성 시험은 아님.
- 보드 --send --host 10.0.0.1 --port 3000 --mac-interface wlan0 --count 3에서 HTTP_200 및 config_version=7 (log only) 3회 출력, STOP attempts=3 확인. MAC7424ca5004af/status online/rtsp_ok false/wg_ok true/config_version0 payload를 Windows에서도 확인. libcurl 전송 및 응답 JSON 파싱의 실기 성공.
- 모의 서버 검증이며 실제 서비스 등록·인증·온라인 판정 검증과 구분. 다음은 count0 전송 중 테스트 서버 중단/재시작에 대한 재시도·자동 복구 시험. HTTP403/3초 timeout/부팅 자동 실행은 아직 미검증.

## heartbeat wlan0 dry-run 3회 성공 — 2026-11-05

- 사용자 보드에서 /tmp/i1-heartbeat --dry-run --count 3 --mac-interface wlan0 실행 성공. 3회 모두 mac=7424ca5004af, status=online, rtsp_ok=false, wg_ok=true, config_version=0 출력 후 STOP attempts=3 확인.
- MAC 소문자 콜론 없는 12자리 및 보드 실행 확인. rtsp_ok는 rkipc 프로세스 확인만 수행하므로 false를 실제 RTSP 영상 실패로 해석하지 않음. wg_ok는 인터페이스 UP이며 이번 dry-run은 HTTP/핸드셰이크 검증이 아님. status=online은 송신 payload 고정값으로 서버 온라인 판정 증거가 아님.
- 다음은 Windows mock_server.py를 VPN 10.0.0.1:3000에 바인딩하고 보드 10.0.1.1에 한정한 TCP 수신 허용 후 --send --mac-interface wlan0 --count 3 시험. 아직 HTTP 수신/실서버 MAC 등록/자동실행 검증 미완료.

## heartbeat 보드 dry-run: eth0 MAC 읽기 실패 — 2026-11-05

- 사용자 /tmp/i1-heartbeat --dry-run --count 3 실행에서 START 출력 후 MAC_UNAVAILABLE interface=eth0 attempt=1 확인. 실행파일 시작 및 초기 동적 라이브러리 로딩은 통과했으나 JSON 3회 출력은 미완료. eth0 부재/읽기 실패/유효하지 않은 MAC 중 실제 원인은 아직 구분하지 못함.
- 현재 네트워크는 wlan0 사용, 앞선 관측 MAC은 74:24:ca:50:04:af. 다음은 /sys/class/net 목록과 wlan0/address 확인 후 --mac-interface wlan0로 전송 없는 dry-run 시험. 이 시험을 서버 등록 MAC의 변경 승인이나 일치 확인으로 간주하지 않는다. 실제 전송 전 등록 식별자와 일치 확인 필요.

## heartbeat ARM ELF 형식 확인 — 2026-11-05

- 사용자 VM file 출력: dist/heartbeat는 ELF 32-bit LSB executable, ARM, EABI5, dynamically linked, interpreter /lib/ld-uClibc.so.0, with debug_info, not stripped. ARM/uClibc 대상 형식 확인. 실제 보드 동적 라이브러리 로딩 및 dry-run 성공은 아직 미확인.
- 다음은 실행파일을 보드 /tmp/i1-heartbeat로 전송하고 chmod +x, --dry-run --count 3 실행. debug_info/not stripped는 실행 실패를 의미하지 않으며 현재 strip 작업은 불필요.

## heartbeat 빌드 성공 보고 — 2026-11-05

- 사용자 Ubuntu ~/i1_heartbeat에서 sh build.sh 실행 결과 `Built: /home/student/i1_heartbeat/dist/heartbeat` 확인. 앞선 헤더 경로 전달 실패 뒤 빌드 성공. 실제 ELF 형식 출력 및 보드 런타임 실행 결과는 아직 없음.
- 다음은 VM의 file dist/heartbeat 확인 후 기존 전송 경로로 보드 /tmp/i1-heartbeat에 복사, chmod +x 및 --dry-run --count 3 실행. MAC eth0 식별 및 JSON 상태를 확인하며 서버 전송/자동실행 설치는 아직 미수행.

## heartbeat 첫 ARM 빌드: 헤더 경로 전달 실패 — 2026-11-05

- 사용자 Ubuntu에서 CURL_PREFIX를 단독 셸 변수 할당한 뒤 별도 명령으로 sh build.sh 실행, curl/curl.h 없음으로 컴파일 실패. 제공 명령에는 export가 없으므로 자식 셸에 경로가 전달되지 않은 것이 유력 원인. 헤더 자체는 앞선 find에서 존재 확인.
- 다음은 `export CURL_PREFIX=.../sysroot/usr` 후 sh build.sh 또는 동일 명령 앞 환경변수 할당 방식으로 재실행. 아직 성공 결과 없으며 라이브러리 재설치·소스 변경 불필요.

## SDK libcurl 개발 파일 및 보드 버전 확인 — 2026-11-05

- 사용자 Ubuntu find 출력에서 `/home/student/luckfox-live/luckfox-pico/sysdrv/source/buildroot/buildroot-2023.02.6/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr` 아래 include/curl/curl.h 및 lib/libcurl.so 존재 확인. heartbeat build.sh의 CURL_PREFIX로 이 usr 경로를 사용한다. host-cmake의 cmcurl 헤더는 선택하지 않는다.
- 보드 curl --version: curl/libcurl 8.4.0, arm-buildroot-linux-uclibcgnueabihf, OpenSSL 1.1.1v, zlib 1.2.11. SDK libcurl 빌드 디렉터리도 8.4.0. 버전/대상 계열 일치는 확인됐으나 전체 ABI 호환·ARM 링크·보드 실행은 아직 미검증.
- 다음은 Ubuntu에서 최신 i1_heartbeat 소스에 CURL_PREFIX를 지정해 sh build.sh 실행, dist/heartbeat의 ARM 형식 확인 후 보드 dry-run. 신규 라이브러리 설치나 보드 런타임 교체는 하지 않는다.

## 작업 폴더 정리 — 2026-11-05

- 사용자 요청으로 파일 분류 및 중복 정리 완료. 현재 사람 인식 소스는 deliverables/CCTV_person_detection/code, heartbeat는 deliverables/i1_heartbeat. 이중 압축 해제 폴더를 제거했으며 소스 내용과 전달 ZIP 내용은 변경하지 않음. S50wireguard는 deliverables/S50wireguard 유지.
- 루트 보드 실행파일 2개 및 진단 .ko는 artifacts/board, result.jpg는 diagnostics/imported-images, AIQ 원본 압축은 references/driver-source로 이동. tmp 전체는 references/review-workspace로 보존. ISP FPS 최초판~v5는 packages/archive/isp-fps에 보관, v6 유지.
- 삭제는 SHA256이 같은 중첩 NPU 진단 소스 4개 및 재생성 가능한 Python 캐시 1개뿐. 펌웨어/복구 백업/모델/원본 예제/별도 웹앱 보존.
- 590개 파일 이동 전후 SHA256 확인, 삭제 5개 기록은 docs/organization-2026-11-05/changes.json. 진단 보조 스크립트의 전달본 경로 참조 및 루트 README를 갱신. 과거 기록과 백업의 경로는 역사적 내용으로 유지. 소스 빌드나 보드 실행 검증을 수행한 작업은 아님.

## WireGuard 부팅 스크립트 준비 — 2026-11-05

- 사용자 요청으로 deliverables/S50wireguard 작성. /etc/wireguard/wg1.conf를 wg-quick up/down으로 적용하며 start/stop/restart/status 지원. 이미 존재하는 wg1은 start에서 유지하고 설정 재적용은 restart 사용. 개인키/Endpoint를 스크립트에 저장하지 않음.
- 보드 wg 및 ip 존재는 이전 출력으로 확인, wg-quick 및 그 인터프리터 설치는 미확인. 사용자에게 command -v wg-quick와 wg-quick --help 확인 후 설치 안내. 스크립트는 누락 명령에서 실패를 반환하며 자동 설치하지 않음.
- S50은 S99wlan0보다 먼저 실행되므로 Wi-Fi 완료를 동기 대기하지 않는다. 현재 숫자 IPv4 Endpoint 구성 기준이며 네트워크 확보 뒤 트래픽/keepalive로 핸드셰이크 시도. 핫스팟 Windows DHCP Endpoint 변경은 자동 추적하지 않음.
- 로컬 파일 작성 및 정적 검토만 완료. 보드 설치/실행/재부팅 검증은 미실행. 기존 heartbeat ZIP에는 별도 스크립트를 추가하지 않음.

## heartbeat libcurl 전환 — 2026-11-05

- 사용자 보드 출력에서 /usr/bin/curl, /usr/lib/libcurl.so 및 libcurl.so.4, libcurl.so.4.8.0 존재 확인. 런타임 파일 존재 사실이며 버전/헤더/SDK ARM 링크 환경은 아직 미확인.
- 요청에 따라 deliverables/i1_heartbeat/src/heartbeat.c의 수동 소켓 HTTP 전송을 libcurl easy API로 교체. 5초 주기, 전체 요청 기본 3초 제한, 본문8191바이트 상한, HTTP200/403 처리, 소문자 MAC 및 dry-run 유지. 프록시/리다이렉트 사용 안 함. 기존 hb_http 파서는 현재 전송 경로에서 사용하지 않음.
- build.sh에 -lcurl 및 CURL_PREFIX(대상 staging/usr) 지원 추가. Ubuntu 호스트 라이브러리를 ARM 링크에 사용하지 않음. 다음은 사용자 Ubuntu SDK의 curl/curl.h와 libcurl.so 위치 및 보드 curl --version 확인 후 ARM 빌드. 전체 프로그램 빌드/보드 실행/HTTP 통합 시험 미검증. 보드나 서버 설정 변경 없음.

## Windows hotspot VPN ping 성공, heartbeat 시험으로 진행 — 2026-11-05

- 이전Windows→보드10.0.1.1 ping3/3성공(6–80ms)으로VPN왕복통신확인. 보드→Windows10.0.0.1 ping미응답에대해Windows관리자ICMP수신허용규칙안내후사용자가“이제 된다 다음 단계”라고보고. 해당방향ping성공으로기록하며실제규칙출력/개별RTT는미수신. 새터널/키재생성필요없음.
- 다음단계는소문자MAC수정본deliverables/i1_heartbeat_offline.zip의ARM빌드및보드dry-run. 빌드/전송이이미완료됐다는사용자증거는없음. MAC기본eth0,rtsp_ok=rkipc존재,wg_ok=wg1UP. 현재실제등록MAC인터페이스는후속결과로확인하며wlan0로임의대체하지않는다.
- dry-run확인후Windows테스트수신서버와VPN내POST시험으로진행예정. ping성공을HTTP3000허용/서버API존재/heartbeat완료로간주하지않음. 아직HTTP전송·자동실행설치미실행.

## Windows–보드 WireGuard 핸드셰이크 성공, ping 미응답 — 2026-11-05

- 사용자보드 wg show wg1에서Endpoint10.32.0.31:51820,latest handshake10초전,124B수신/5.98KiB송신확인. Windows첨부화면에서도보드peer키일치,AllowedIPs10.0.1.0/24,학습Endpoint10.32.0.102:51820,최근handshake및수신2.39KiB/송신188B확인. 화면과보드출력시점은다르므로카운터를동일시점비교하지않는다. 이전핸드셰이크미성립은해소됨.
- 보드ping10.0.0.1은두차례3송신0수신. WireGuard보안연결성립과ICMP echo응답은별개이며전송카운터로ping응답성공을판정하지않는다. Windows인바운드ICMP차단이유력확인대상이지만아직원인확정아님.
- 다음사용자시험:Windows관리자PowerShell에서로컬10.0.0.1/원격10.0.1.1에한정한ICMPv4 echo(type8)수신허용규칙추가후보드ping재시험. 방화벽전체해제불필요. 실패지속시Windows실제VPN주소와보드 `ip route get 10.0.0.1`로주소/경로확인. 규칙적용및성공결과는아직미수신.

## 새 Endpoint 적용 확인, WireGuard 핸드셰이크 미성립 — 2026-11-05

- 사용자 wg show wg1:peer0+ty…의실행Endpoint10.32.0.31:51820확인,AllowedIPs10.0.0.1/32,transfer0B received/3.32KiB sent,latest handshake없음. ping10.0.0.1은3송신0수신. 설정파일미반영가설은이번실행출력으로해소. 송신카운터는Windows도착/정상복호화증거가아님.
- 다음은Windows WireGuard터널활성상태/실제공개키/peer출력,UDP51820리스너확인. 핸드셰이크가없으므로ICMP허용만으로해결된다고안내하지않음. 키불일치/UDP방화벽/핫스팟기기간차단가능성은남으며아직원인확정없음.

## Windows Endpoint 수정 후 VPN 미연결 확인 중 — 2026-11-05

- 사용자 Windows ipconfig:Wi-Fi IPv4 10.32.0.31/24,게이트웨이10.32.0.28. 보드/etc/wireguard/wg1.conf의Endpoint=10.32.0.31:51820으로파일상일치확인. peer공개키0+ty…유지,AllowedIPs10.0.0.1/32,보드주소10.0.1.1/24. 개인키는기록하지않음.
- 서버10.0.0.1 ping은응답없다고보고. 파일수정후실행중wg1에반영됐는지/latest handshake는아직미제공. 다음은wg show wg1 확인,기존peer Endpoint를wg set으로현재값에적용하고ping/handshake재확인. 파일과실행상태를구분하고Windows방화벽/공개키/핫스팟단말간통신문제를아직원인으로확정하지않는다.

## 이중 IP 추가 검색 결과 — 2026-11-05

- 사용자 /etc/init.d 목록에 S이름의백업파일없음. S40network,S21appinit,S99luckfoxconfigload 등존재. 지정범위(/etc/init.d,/etc/network,/etc/rc.local,/etc/inittab,/etc/dhcpcd.conf)의udhcpc/10.32.0.101/ifconfig wlan0/ip addr검색결과는수정된S99hciinit의인터페이스up과usb0/nfs관련행뿐. 이범위에서주소생성원인을찾지못했으며모든부팅경로배제는아님.
- 다음은/etc/network/interfaces 및S40network,S21appinit,S99luckfoxconfigload 본문으로간접호출/ifup자동DHCP경로확인. 부팅중짧게실행후종료한클라이언트또는앱내부설정가능성은가설이며실제출처미확정. 보드추가변경없음.

## DHCP 수정 후 재부팅: WPA 성공, 이중 IP 잔존 — 2026-11-05

- 재부팅후사용자출력: SSIDWifi/freq2437/WPA2-PSK/CCMP/wpa_stateCOMPLETED, MAC74:24:ca:50:04:af. Wi-Fi인증및연결확인. wpa_cli ip_address는10.32.0.101이나이값만으로DHCP출처/선호송신주소를판정하지않는다.
- wlan0에는여전히10.32.0.101/24(forever)와10.32.0.102/24(secondary dynamic noprefixroute,valid3534초/preferred3084초)가동시존재. default via10.32.0.28/src10.32.0.102/metric3004. dhcpcd manager232및wlan0 ARP .102확인; ps -ef검색에udhcpc프로세스없음. 기존중복udhcpc실행은현재목록에서해소됐으나IP중복해결은미완료.
- .101생성원인은아직미확정. 다른부팅스크립트/초기설정,init.d내실행대상백업파일,주소설정후종료한DHCP실행등을확인해야함. forever만으로정적IP확정금지. 다음은/etc/init.d목록과/etc/init.d,/etc/network,/etc/rc.local,/etc/inittab,/etc/dhcpcd.conf내udhcpc/wlan0주소설정검색. 우선전체dhcpcd종료·wlan0전체주소삭제하지않는다.

## DHCP 중복 시작 스크립트 수정 확인 — 2026-11-05

- 사용자 cat 출력으로 S99wlan0의 udhcpc 시작/종료 행 제거 확인. wpa_supplicant 시작과 기존 stop 동작은 유지됨. S99hciinit의 DHCP 복합 명령은 `ifconfig wlan0 up`으로 변경됐으며 Bluetooth 초기화 코드 유지 확인.
- 사용자 `sh -n /etc/init.d/S99wlan0` 및 S99hciinit 실행에서 오류 출력 없이 셸 복귀. 구문검사 통과로 기록하되 실제 부팅 실행·네트워크 연결 성공과 구분한다.
- 다음은 실행권한 확인/부여 및 sync/reboot 뒤 WPA COMPLETED,wlan0 IPv4한개,default via,udhcpc미실행 확인. 아직 재부팅 결과는 없고 수정 전 실행 중이던 udhcpc가 종료됐다고 가정하지 않는다.

## 핫스팟 DHCP 단일 관리 수정 안내 — 2026-11-05

- 사용자확인:S99wlan0는직접추가한파일이며원래S99hciinit만존재했다고설명. S99wlan0를삭제하지않고WPA인증실행은유지한다. 사용자가처음부터순서안내를요청하여고유날짜이름의/root백업→두파일udhcpc부분제거→sh -n구문및내용확인→sync/reboot→상태확인절차를안내. 아직사용자실행결과없음.
- 사용자는 당분간 갤럭시핫스팟 사용,고정공유기전환은나중으로결정. 현재정적IP설정하지않고dhcpcd만DHCP관리자로유지하는방향이다.
- 제공된S99hciinit전체에서Bluetooth hciattach/hciconfig 처리뒤 `ifconfig wlan0 up && udhcpc -i wlan0 > /dev/null 2>&1` 실행확인. 이줄을ifconfig wlan0 up으로만교체하며Bluetooth로직은유지한다. S99wlan0의udhcpc시작행과killall udhcpc행만제거한다. wpa_supplicant start/stop기존동작은이번최소수정범위외다.
- S41dhcpcd는설정파일로전역daemon시작, /etc/dhcpcd.conf는duid/persistent및일반DHCP옵션으로정적IP/denyinterfaces없음. 수정불필요. proxy/BPF프로세스들은동일dhcpcd의보조프로세스이며별도중복daemon으로세지않는다.
- 사용자실행안내: USB/시리얼접속에서 /root아래전용백업디렉터리에S99wlan0/S99hciinit보존(부팅스크립트폴더에S로시작하는백업을두지않음),두부분수정확인후재부팅으로기존udhcpc와잔류주소정리. 재부팅뒤wpa_stateCOMPLETED,wlan0 DHCP IPv4한개,default via,wlan0 udhcpc미실행확인. 아직수정/재부팅/성공출력미수신. dhcpcd종료또는기존주소강제삭제는실행하지않음.

## wlan0 DHCP 중복 실행 확정 — 2026-11-05

- 사용자 실제출력: udhcpc -i wlan0 두개(PID667/1136)와 dhcpcd manager 및 wlan0 BPF ARP 프로세스가 동시에존재. dhcpcd는usb0도관리한다. wlan0 IPv4는10.32.0.101/24(forever)와10.32.0.102/24(secondary dynamic,valid3226초/preferred2776초). 기본경로via10.32.0.28/src10.32.0.102/metric3004. 개별주소생성주체의완전한추적은안했으나중복관리구성자체는확인됨.
- S99wlan0 start에서 wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf 및 udhcpc -i wlan0 실행. stop은killall wpa_supplicant/udhcpc라다른인스턴스까지종료가능. S99hciinit13행에도ifconfig wlan0 up && udhcpc -i wlan0 존재. S41dhcpcd는/sbin/dhcpcd와/etc/dhcpcd.conf를사용. S99hciinit/S41dhcpcd 전체 및dhcpcd.conf는미확인.
- 현재SSID는Wifi,WPA-PSK 설정. 사용자제공비밀번호는기록하지않음. usb0실제주소172.32.0.93과링크로컬169.254.95.91; 스크립트에별도usb0주소설정도존재하므로전역dhcpcd종료/usb수정하지않는다. 현재10.32.0.0/24는이전192.168.45.x 및10.174.156.x와다른관측이며고정대역으로간주하지않는다.
- 정적IP 진행안: wlan0의udhcpc중복시작을제거하고dhcpcd 한곳에서정적설정관리, wpa_supplicant는인증용유지. 실제수정전S99hciinit/S41dhcpcd/dhcpcd.conf 전체와대상Wifi가영구사용할공유기인지확인필요. 현재주소.101/.102를충돌검증없이정적IP로선택하지않는다. 아직보드설정변경없음.

## Windows 테스트 VPN 설정 후속 — 2026-11-05

- 사용자제시 보드설정:Address10.0.1.1/24,ListenPort51820,peerPublicKey=`0+tyNFs+Cb7vym0PNzCZ0/4B0o6PVTlD53AElWh+/yo=`,AllowedIPs10.0.0.1/32,Endpoint192.168.45.193:51820. 이번텍스트에PersistentKeepalive는없음. Windows설정:Address10.0.0.1/32,ListenPort51820,peer는기존보드공개키,AllowedIPs10.0.1.0/24.
- Windows실제공개키는앱표시로아직확인되지않았으므로보드peer키오류라고단정하지않는다. 이전Ubuntu공개키+422…와별개로확인해야한다. 개인키는복사/저장하지않았다. 192.168.45.193이현재Windows LAN주소인지,보드와상호도달가능한지,설정반영/handshake성공은미검증. 서버/보드설정파일은수정하지않았다.

## heartbeat MAC 형식 정정 — 2026-11-05

- 사용자 명확화: 문서의 “대문자 콜론 없는 12자리”는 대문자와 콜론을 모두 없앤 **소문자 16진수 12자리**를 뜻한다. 이전 대문자 해석은 잘못된 과거 기록이며 앞으로 적용하지 않는다. 예: `74:24:CA:50:04:AF` → `7424ca5004af` (형식 예시이며 등록용 eth0 MAC 확정 의미 아님).
- deliverables/i1_heartbeat/src/protocol.c를 tolower로 수정. 혼합대소문자 입력과 JSON 출력의 소문자 정규화 테스트, 모의 서버의 소문자12자리 스키마, README를 함께 갱신했다. 기본 식별 인터페이스 eth0 유지.
- MSVC 프로토콜 테스트 통과. 모의 서버 스키마에서 소문자 허용/대문자·콜론 포함 거부 확인(서버 미기동). deliverables/i1_heartbeat_offline.zip 11파일 갱신 및 CRC·소스바이트 일치 확인. 이전 ZIP 해시는 이제 과거값이다. ARM 재빌드/보드 실행은 아직 미검증.

## heartbeat 공식 예제 조사 — 2026-11-04

- Luckfox 공식 위키/공식 RKMPI 예제 저장소 및 웹검색 범위에서 Pico Zero용 I1 형식 HTTP heartbeat 완제품은 찾지 못했다. SDK 전체 파일을 전수검사한 것은 아니므로 부재를 단정하지 않는다.
- 제조사 공식 기초 문서: https://wiki.luckfox.com/Core3566/Linux-Systems-Programming/7.Network-Programming/ (Core3566용 Linux 네트워크 개념 및 TCP socket/connect/send/recv 절차 설명. 열람한 페이지에는 완성 C 소스가 없으며 Pico Zero heartbeat 예제가 아님). HTTP POST 및 응답 수신의 실제 C 예제는 curl 공식 https://curl.se/libcurl/c/http-post.html 및 https://curl.se/libcurl/c/postinmemory.html. libcurl 사용에는 보드 런타임 및 ARM/uClibc 헤더/링크라이브러리 확인이 필요하며 현재 설치상태 미확인.

## WireGuard Windows 테스트 구성 검토 — 2026-11-04

- 사용자 제시 보드설정:10.0.1.1/24,listen51820,peer공개키는이전에확인한Ubuntu공개키와동일,AllowedIPs10.0.0.1/32,keepalive25,Endpoint누락. Windows테스트서버설정:10.0.0.1/32,listen51820,peer는기존보드공개키,AllowedIPs10.0.1.0/24. 실제Windows실행여부/현재공개키/두장치LAN접속상태는미확인.
- 사용자메시지에양측PrivateKey가포함됐으나프로젝트파일에는복사·기록하지않는다. Windows앱의실제공개키와보드peer일치확인,같은LAN이면보드Endpoint에Windows실제LAN IPv4:51820,Windows수신UDP51820허용이필요. 네이티브Windows서버와VirtualBoxUbuntu서버의경로를구분한다. 보드1대직접peer는10.0.1.1/32로좁히는구성을안내하며실제변경은미수행.

## 보드 Wi-Fi 연결 상태 재확인 — 2026-11-04

- 최신 wpa_cli ping=PONG, status=SCANNING, ip_address169.254.156.212, MAC74:24:ca:50:04:af. 기존제어프로세스응답확인, Wi-Fi인증미완료. ip route는default dev wlan0 scope link/src169.254.156.212이며공유기게이트웨이via가없다. 정상공유기DHCP연결로간주하지않는다.
- 다음읽기전용진단은wpa_cli list_networks와scan_results. 필요시scan요청후수초뒤결과조회. 대상SSID/활성상태/검색여부확인전비밀번호오류단정·DHCP추가실행·소켓삭제하지않는다. 현재대상SSID가과거Wifi와동일한지는미확인.
- 사용자 wpa_supplicant -B 실행은 ctrl_iface `/var/run/wpa_supplicant/wlan0` 충돌로 실패했다. 기존인스턴스 실행 또는 잔여소켓 가능성을 구분해야 하며 소켓을 즉시 삭제하거나 중복실행하지 않는다.
- 같은 출력 wlan0 MAC=`74:24:CA:50:04:AF`, IPv4=`169.254.156.212/16`, RX/TX패킷0. 링크로컬주소만 확인되어 정상DHCP주소획득/인터넷접속 성공 근거가 아니다. Wi-Fi인증상태는 wpa_cli status로 별도확인 필요. usb0=`172.32.0.93/16` 유지. 이출력에wg1은없으나인터페이스삭제/설정손실로단정하지않는다.
- 다음은 `wpa_cli -p /var/run/wpa_supplicant -i wlan0 ping`, 같은경로 status, `ip route` 확인. PONG이면기존인스턴스사용; 설정을바꾼경우에만 reconfigure후status확인. DHCP는wpa_state=COMPLETED 및현재클라이언트확인뒤처리해과거중복할당을반복하지않는다.

## Ubuntu VM 주소 확인 — 2026-11-04

- 보드 후속 출력: wg1 UP, `10.0.1.1/24`, listen51820, peer AllowedIPs10.0.0.1/32, keepalive25초. peer Endpoint/latest handshake/transfer 표시 없음.
- 공개키 대조: 보드wg1 공개키 `poOBaNdooRh43R354QVa92wZY9pQt5apeyrYtsZ2i2M=`는 Ubuntu wg0에 등록된 peer와 일치. 반대로 보드에 등록된 peer `0+tyNFs+Cb7vym0PNzCZ0/4B0o6PVTlD53AElWh+/yo=`는 현재 Ubuntu wg0 공개키 `+422n+y2hOwcuEX52+sPU14vab+bF56domo7/NsZLUI=`와 불일치한다. 로컬Ubuntu 시험 목적에서는 상대키 불일치와 Endpoint 미설정이 확인됨. 기존peer가 다른 실제서버용일 가능성은 미확정이므로 삭제·덮어쓰기하지 않았다.
- 다음은 Windows PC와 보드가 같은 LAN인지 및 Windows의실제LAN IPv4/보드라우트 확인 후 VirtualBox NAT UDP51820포워딩 또는 브리지 경로를 정한다. 공인IP 필요 여부를 미리 확정하지 않는다. 키/Endpoint/AllowedIPs 변경은 아직 미수행.
- 후속 `wg show wg0`: listening port51820, peer1개, AllowedIPs=`10.0.1.0/24`. endpoint/latest handshake/transfer 항목은 표시되지 않음. 현재 인터페이스에서 성공한 핸드셰이크 기록이 없으며 보드측Endpoint·키매핑·현재VPN IP 및 VM NAT 전달 경로는 아직 미확인이다. 보드가 클라이언트로 먼저 접속하는 구성에서는 서버측peer Endpoint 부재 자체를 설정 오류로 단정하지 않는다. /24는단일보드 /32보다넓지만 피어역할·서버할당IP 확인 전 변경하지 않는다.
- 사용자 VM `root@student-VirtualBox:/home/student/vpn`의 ip addr: enp0s3 MAC `08:00:27:09:b3:dc`, 동적 IPv4 `10.0.2.15/24`; wg0 UP, IPv4 `10.0.0.1/24`. 이 MAC은 VM의 것이며 보드 등록 MAC이 아니다.
- enp0s3 주소는 VirtualBox 기본 NAT 대역과 일치하여 NAT 모드로 추정하나 GUI 설정 자체는 미조회. VM 인터페이스에 공인 IPv4는 표시되지 않음. wg0 UP만으로 보드와 핸드셰이크/통신 성공을 선언하지 않는다. VPN 내부 heartbeat 대상은 터널이 연결되어 있으면10.0.0.1이며 외부 Endpoint와 구분한다.

## I1 heartbeat 오프라인 준비본 작성 — 2026-11-04

- 사용자 정정: 실제 서버에 접속하지 않고 가능한 작업부터 진행한다. VPN ping/handshake 및 서버 POST는 현재 실행 단계에서 제외했다. 보드·VM·외부 서버에 접속하거나 WireGuard/부팅 설정을 설치하지 않았다.
- 새 전달본 `deliverables/i1_heartbeat_offline.zip` (최상위 i1_heartbeat, 소스/스크립트/README/테스트11파일). libcurl 설치 여부를 아직 모르는 상태에서 외부 의존성 없이 준비 가능한 POSIX 소켓 C 구현을 선택했으며 보드에 libcurl이 없다고 판정한 것이 아니다. 기존 카메라/NPU 코드는 변경하지 않았다.
- 기본 실행은 dry-run 1회: eth0 MAC 대문자12자리, 비좀비rkipc 존재, wg1 IFF_UP 상태를 JSON으로 출력하며 소켓을 만들지 않는다. `--dry-run --count 3`은5초 간격3회. 서버전송은 `--send --host 숫자IPv4` 명시가 필수이며 서버주소 기본값 없음. heartbeat.conf는 ENABLED=0/HOST빈값. S60heartbeat 준비만 했고 /etc에 미설치.
- 전송 경로 구현(실행 미검증):5초 주기, 연결/송수신 전체3초 monotonic deadline,403/연결실패/timeout 재시도,200 top-level config_version 로그만,Content-Length/chunked/EOF응답 처리,PID파일잠금 및SIGTERM종료. rtsp_ok/wg_ok boolean·config_version 정수0은 첨부 기반 구현안으로 ICD v1.2 원문 대조 필요. wg_ok는 핸드셰이크 보증 아님. custom RTSP만 실행하면 I1 기준 rtsp_ok=false일 수 있다.
- Windows MSVC /W4 /WX로 protocol.c와 tests/test_protocol.c 빌드/오프라인 테스트 통과:MAC무효값,JSON정확성,config_version파싱/중복/중첩/오버플로,HTTP framing/403/chunked/keep-alive/절단/크기제한. Python 모의POST서버 구문 및 스키마 테스트 통과(import만,리스너 미기동). 프로토콜테스트는 실제 소켓 재시도/타이머 검증을 대체하지 않는다.
- 로컬WSL조회는 승인후 Ubuntu 존재 확인, 이후 Linux전체빌드·dry-run 실행 승인 요청은 사용자가 거절하여 실행하지 않았다. Git sh 구문검사도 샌드박스 접근제한으로 실패(실행 성공으로 기록하지 않음). 셸정적검토/LF확인 및ZIP CRC·원본바이트일치검증 완료. ZIP SHA256 `bcf563648240c5501f1eead6c01ab37ddd64e26297acf30b735f467cd170431d`.
- 다음 사용자 단계:VM /home/student/i1_heartbeat에서 sh build.sh로 ARM/uClibc 빌드, dist/heartbeat를 보드 /tmp/i1-heartbeat로 복사 후 chmod+x 및 `--dry-run --count 3`. ARM빌드/보드실행/서비스설치/재부팅/2대연동은 아직 미검증. 실제서버 연결은 후속 요청 전 진행하지 않는다.

## I1 보드 heartbeat 작업 시작 — 2026-11-04

- 사용자 요청으로 `bf7ee0ed-589d-4961-83dd-d71ce2fe59ba_보드.pdf`의 작업을 진행한다. 보관본 `docs/I1_BOARD_TASK_2026-11-04.pdf`, 전체 4쪽 텍스트 및 렌더 확인. 두 담당자 체크 상태가 다르므로 **오호종 절(p.2 하단–4)**을 적용한다. 정태석 절의 구현 완료 체크를 우리 구현 완료로 옮기지 않는다.
- 오호종 절에서 MAC 확인·MAC/공개키 전달·서버 재할당 후 WG 설정 변경은 체크됨. 이는 문서 보고이며 보드 실측 재검증은 아님. VPN ping/handshake·재부팅 자동 연결, HTTP 구현 및 단독/통합 시험은 미체크. 현재 보드/VM 직접 접근 없음.
- 이번 I1 범위: 2대, 11/5 단독 시험 및 부팅 자동 실행, 11/6 저녁 실제 서버 통합. 기준은 ICD v1.2라고 적혀 있으나 ICD 원문은 미제공. 기존 SRS 전체를 교체하지 않고 이번 I1 heartbeat에는 명시된 **5초 주기**를 적용한다.
- POST `http://10.0.0.1:3000/v1/devices/heartbeat`; mac(eth0 MAC 대문자/콜론 없는 12자리), status=online, rtsp_ok(rkipc 실행), wg_ok(wg1 인터페이스 상태), config_version=0. 200 본문 config_version은 로그만, 403 및 연결 실패/timeout은 종료하지 않고 다음 주기에 재시도. 연결/응답 timeout 필요. bool 필드 등의 정확한 ICD 스키마는 후속 확인 대상.
- WireGuard 요구: 서버 할당 10.0.1.x, peer AllowedIPs=10.0.0.1/32, endpoint 포트51820. 실제 endpoint 호스트/할당 주소는 추정하지 않는다. 기존 키 유지, 개인키 공유 금지. 문서의 담당자 전달 항목을 이유로 assistant가 외부 메시지를 발송하지 않는다.
- 다음 단계: 사용자 보드에서 `wg show wg1`, `ip -4 addr show dev wg1`, `ping -c 3 10.0.0.1`, libcurl 파일·curl 명령 유무, init 파일 및 관련 프로세스 읽기 전용 조회. libcurl 존재 시 개발 헤더/ARM 링크 라이브러리도 확인, 없으면 소켓 HTTP 구현. 아직 heartbeat 코드/부팅 설정을 설치하거나 재부팅하지 않았다.
- 구현 후 POST 수신용 시험 서버로 5초 주기·200 config_version·403·무응답 timeout·서버 중단/재시작 복구를 검증하고 S50wireguard 다음 S60heartbeat 자동 실행 검증. 전원 차단20초내offline은 서버/클라이언트 판정까지 포함하므로 보드 송신만으로 완료 선언하지 않는다. rtsp_ok는 I1의 rkipc 실행 지표로 유지하고 기존 custom RTSP 앱 실행과 혼동하지 않는다.

## VM에서 공식 RTSP YOLO 예제만 확인, 제조사 추론 소스 사진 비교 패키지 작성 — 2026-11-02

- 사용자 VM find 결과: /home/student/RKMPI/luckfox_pico_rkmpi_example/include/rknn 및 example/luckfox_pico_rtsp_yolov5만 표시. 지정 이름의 디렉터리 검색 범위에서 별도 사진 예제는 발견되지 않았으며 SDK 전체에 사진 예제가 절대 없다고 단정하지 않는다.
- deliverables/vendor_photo_compare.zip 별도 작성. VM의 공식 src/yolov5.cc와 src/postprocess.cc를 수정 없이 직접 컴파일하며 제조사 yolov5.h를 탐색. 우리의 NpuModel/person_decode/LatestFrame/AIQ/VI/VENC/RTSP는 제외. 사진읽기/BGR검정letterbox/반복·로그는 새 사용자측 진입점으로 공식 사진 예제 완제품이 아님을 문서·로그에 명시.
- 입력은 시작 시 한 번 전처리, 매회 공식 input_mems에 memcpy 후 inference_yolov5_model 호출. 같은 컨텍스트로 기본20회/1000ms 시작 간격, 첫 오류 중지. 입력CPU바이트FNV값 및 VENDOR_PASS/FAIL 누적 기록. 공식 후처리와우리후처리조건차이 때문에 검출수/시간을동일지표로비교하지않는다. 이미지IO의하드웨어경로까지배제한시험은아님.
- 기존 VM에서 성공한 ARM/uClibc·OpenCV highgui 헤더탐색 재사용. dist에는 vendor_person_image·공식label·사용한소스/헤더/라이브러리SHA256 manifest만 설치. 기존모델/보드런타임교체없음. 공식상위CMake(kernel-5.10.160) 온라인확인으로include경로/라이브러리연결참고; 로컬에는yolov5.h없어실제VM헤더탐색과ARM빌드는사용자확인대기.
- 패키지4파일 ZIP CRC/바이트일치/build.sh LF 확인, API흐름과로컬원본소스검토완료. ARM전체빌드/보드실행미수행. 기존CCTV전달본은변경하지않음. 보드/VM자동접근없음. 다음사용자단계는새폴더에서 sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example.

## 사진 프로그램 새 프로세스 반복도 9번째 실패 — 2026-11-02

- 사용자 기존 cctv_person_image를 매회 새 프로세스로 최대20회 순차 실행, 매회 종료 뒤 sleep1. RUN1~8 성공(23~24개,97.5~99.8ms), RUN9는 전체 입출력할당/NPU ready 이후 op18 Conv:/model.4/cv3/conv/Conv/task95/intstatus0/rknn_run -1로 실패. FAILED RUN=9에서 루프 중지. stdout/stderr를 파일로 함께 리다이렉트하여 오류행이 모델로딩행보다 앞에 보이나 실제 초기화 전 추론실패로 해석하지 않음.
- 새 커널 job0afc637a uptime1139.688~1139.928, wait6.378초/timeout6.498초, task0x5f/irq0/raw0xc0000/required0x300, softreset. uptime147/798/979/995는 이전 실패.
- 매회 프로세스·모델컨텍스트·입출력버퍼를 새로 만들어도 간헐 실패. 동일컨텍스트재사용, 통합앱슬롯, 작업스레드, 카메라/RTSP활성이 필수조건이라는 가설을 지지하지 않음. 다만 이 세션은 이전timeout들이 선행되어 잔류상태/보드환경 원인 미배제. 사진 단발 성공을 안정성검증으로 취급하지 않는다. 8/9를 통계적 신뢰도로 일반화하지 않음.
- 로컬 공식 luckfox_pico_rtsp_yolov5/src/yolov5.cc 재확인: native입력/nativeNHWC출력 조회, UINT8입력, size_with_stride create_mem, set_io_mem, 동기rknn_run 및해제는우리공통코드와같은API흐름. 이 정적 유사성은 구현정확성/보드안정성 보증아님. 다음 분리는 같은모델·런타임조건의제조사/Rockchip 사진전용예제 기준실행이며, 공식RTSP예제만비교하면카메라조건이추가됨. 공식그대로의사진예제존재/경로부터확인할예정. 런타임/드라이버 무작정업데이트하지않음. 보드자동접근없음.

## 1초 간격 주 스레드 반복도 실패 — 2026-11-02

- 첨부730403b0: 별도 사진23개/97.8ms 성공 직후 -N -M -P person.jpg -f1 -t0.5를 두 번 실행. 첫 실행은 첫 추론부터 op18/task96, INFERENCE_STOP completed0. 두 번째는23개/detect97.2~110.2ms로5회 성공 후 여섯 번째 op18/task95 실패, INFERENCE_STOP completed5.
- 커널 새 기록 uptime979.048~979.288(task96,wait6.275초/timeout6.395초), uptime995.048~995.288(task95,wait6.220초/timeout6.340초), 모두 irq0/raw0xc0000/require0x300/softreset. 두 실행에 같은job문자열4a4a3145가 출력되어도 uptime/counter가 다른 별개 실행이다. uptime147/798은 이전 실패.
- 호출간격을200ms에서1초로 늘려도 해결되지 않았고 첫 추론부터 실패한 경우도 있어, 현재 프로세스에서 연속 호출이 누적돼야만 발생한다는 설명은 성립하지 않음. 보드의 이전 실패 상태 영향은 남음. 카메라/RTSP 및 작업스레드 전환도 필수조건 아님. NPU 전체 연산시간 대비 실제사용률로 단정하지 않는다.
- 다음 비교 후보는 기존 사진 실행파일을 각기 새 프로세스로20회 순차 실행하고 첫 실패에서 멈추는 것. 매회 모델/버퍼 초기화·해제와 JPG저장까지 포함하므로 같은컨텍스트 재사용 대조에 유용하나 프로세스/할당/타이밍도 함께 바뀜. 모두 성공해도 재사용 버그 확정은 아님; 실패하면 단발 사진 안정성 가정부터 재검토한다. 새 빌드 없이 사용자 실행 가능. assistant 보드접근/소스/드라이버 변경 없음. 원본 diagnostics/2026-11-02/main-thread-one-fps-timeout.log 보관.

## 주 스레드 반복도 두 번째 추론에서 실패, 호출 간격 비교 예정 — 2026-11-02

- 사용자 cctv_person_npu_thread_compare -N -M -P person.jpg -f5 -t0.5 실행. READY inference_thread=main, 카메라 미초기화, 첫 추론 PERSON23/detect97.4ms/completed1 성공 후 op18 Conv:/model.4/cv3/conv/Conv/task95로 실패. INFERENCE_STOP completed=1로 두 번째 추론 실패 확인. 최신 -M 실제 보드 실행 확인(안정 동작 실패).
- 새 커널 job021faaa7 uptime798.568~798.808: wait6.119초/timeout elapsed6.239초, task0x5f, irq0/raw0xc0000/required0x300, softreset. uptime147 job2001a502/task97는 이전 -N 실패로 구분.
- 사용자 가설은 너무 연속으로 호출해 정지한다는 것. 현재 -f5는 시작 선택 간격 약200ms이고 첫 전체 전후처리 포함 추론97.4ms이므로 정상 첫 호출 뒤 대기 여지가 있으며 겹치는 동시 rknn_run은 없다. 작업 스레드 전환은 필수 재현조건이 아님. 반복 조건/간격 영향은 가능하지만 단순 처리량 초과, 열, 메모리/캐시, 컨텍스트 재사용, 전원관리 원인 확정 아님.
- 다음 새 비교는 같은 바이너리 -N -M -P에서 -f1로 선택 간격 약1초로 늘리는 것. 기존 사진 단발 정상 기준을 확인하고 30초 관찰; 성공해도 간격 민감성 단서일 뿐 과부하 확정이 아님. 로컬 rknpu_drv.c는 power_put_delay 기본3000ms이고 debugger delayms 노출이나 보드 실값은 이번에 미조회; 전원관리 설정 변경하지 않음. 보드접근/코드 변경 없음.

## -N 반복 실패 직후 사진 성공 및 주 스레드 반복 비교 구현 — 2026-11-02

- 사용자 후속 사진 프로그램은 PERSON count23, detect_ms97.8로 성공. dmesg는 앞선 uptime147.688~147.928/job2001a502/task97의 동일 timeout·reset 기록이며 사진의 새 오류가 아니다. 재부팅 없이 실패 직후에도 사진이 정상인 사례로 기록. 직전 다른 세션에서 사진까지 실패한 이력도 유지하며 실패 후 항상 회복/항상 불능 어느 쪽도 단정하지 않는다.
- 다음 분리를 위해 현재 전달본 main.cc에 -M 추가(-N -P 필수). 같은 바이너리에서 -M이면 추론 작업 스레드를 만들지 않고 main이 사진 clone→동일 LatestFrame publish/take→공통 InferenceRunner.step을 반복 호출한다. 모델은 여전히 main에서 한 번 초기화하고 RKNN 컨텍스트/입출력 버퍼를 재사용한다. 기존 run_inference도 같은 step을 호출하도록 공통화. NpuModel·모델·드라이버 변경 없음.
- READY inference_thread=main/worker 및 실패 시 INFERENCE_STOP completed 추가. 오류 시 정확한 성공 누적 횟수 기록, 자동 재시도 없음. main 모드는 추론 중 다음 사진 생산 불가여서 생산 타이밍/메모리 수명 차이가 남으며 순수 스레드 변수만의 비교라고 주장하지 않는다.
- 새 공통 처리 코드를 실제 소스에서 추출해 MSVC+모의 Mat/NpuModel로 주 스레드100회, 작업 스레드 호출 위치, 오류 후 재시도 금지/영상종료플래그 독립, 대기 종료 검사 통과. 기존 size_t→int와 stopped 이름 가림 경고 출력. 전체 main.cc/ARM/OpenCV/RKNN 빌드·실장치 검증 아님. 진단 소스 diagnostics/2026-11-02/runner_check.cc, 실행 도구 run_runner_check.cmd. 최신 -M 보드 미시험.
- 전달본 STATUS/README/SOURCE_MAP/FIXED_IMAGE_TEST 최신 결과 반영, THREAD_REPEAT_TEST.md 추가. ZIP 16파일 CRC·원본 바이트 일치, build.sh LF, 모델 SHA256 유지 확인. 이전 ZIP/main.cc는 backups/before-main-thread-mode 보존. 사용자 VM 빌드 후 cctv_person_npu_thread_compare로 별도 배포하여 -N -M 먼저20초, 정상 종료하면 같은 바이너리 -N으로 비교. 보드접근 없음.

## 정상 사진 기준 직후 -N 반복 추론 일부 성공 후 timeout — 2026-11-02

- 첨부4d97ec51: 재부팅 후 사진23개/105.3ms 정상 기준에 이어 동일 cctv_person_npu_no_camera -N -P person.jpg -f5 -t0.5 실행. FIXED_IMAGE/READY NO_CAMERA 확인. PERSON completed=1,3,4,6,7,11,12까지 출력되어 최소12회 성공 확인. 출력된 detect_ms97.7~137.1, 검출23~24개. 로그가 매회 출력되는 구조가 아니므로 실패가 정확히13번째인지는 미확정.
- 이후 op18 Conv:/model.4/cv3/conv/Conv, taskcounter97, intstatus0, rknn_run -1로 자동종료. 새 커널 job2001a502 uptime147.688~147.928: wait6.013초, timeout elapsed6.133초, counter0x61, irq0/raw0xc0000/required0x300 및 softreset. raw0xc0000의 상세 의미는 이번 자료로 해석하지 않음.
- 정상 기준 이후에도 카메라 미초기화 경로에서 반복 중 재현. 과거 실패 상태 잔류만으로 이번 실패를 설명할 수 없고 카메라/RTSP 동시활성은 필수 재현조건이 아님. 다만 부팅 중 rkipc 가동 이력은 있으며 모든 ISP 관련 상태가 없다는 뜻은 아님. 초기화 스레드→추론 스레드 전달은 일부 성공했지만 장시간 안정성이 검증된 것은 아님. 스레드/반복 실행/입출력 메모리·런타임·드라이버/하드웨어 원인 미확정.
- 이번 첨부에는 -N 실패 뒤 별도 사진 재실행 결과 없음. 다음은 동일 실패 상태에서 사진1회로 회복 여부 확인. 이후 원인 분리에는 동일 바이너리·모델·동일 컨텍스트 반복에서 주스레드 추론과 작업스레드 추론을 비교하는 것이 유효; 단발 사진성공과 반복실행실패만으로 스레드 결함 단정 금지. 원본 diagnostics/2026-11-02/no-camera-repeated-inference-timeout.log 보관. 보드접근/코드/드라이버 변경 없음.

## 재부팅 기준 사진 추론 복구, 같은 상태에서 -N 비교 예정 — 2026-11-02

- 첨부6372d380: 재부팅 안내 뒤 killall rkipc→sleep3→ps 확인. ps에 rkipc/사용자 스트리머 없음. MemFree72516kB, MemAvailable117984kB, CmaTotal102400kB, CmaAllocated12kB, CmaReleased102388kB, CmaFree0. 로드 rknpu srcversion은 기존 기준 0BBC54BA6B23914910050C9와 일치.
- 이어 별도 사진 프로그램 PERSON count23, detect_ms105.3으로 성공하고 셸 복귀. 두 번의 dmesg grep RKNPU 출력 없음. 해당 필터에서 오류 미관측이며 전체 커널 오류 부재를 뜻하지는 않음. CmaFree0인데도 추론 성공해 이 수치 단독 고갈 해석을 다시 배제.
- 이전 -N/task80 및 후속 사진/task96 실패 상태와 달리 재부팅 후 정상 기준 회복. 재부팅은 여러 상태를 함께 바꾸므로 NPU reset 결함 또는 특정 코드 원인 확정은 아님. 이전 -N은 실패 상태에서 수행됐을 가능성이 있어, 지금 사진 성공 직후 카메라를 시작하지 않고 동일 -N 바이너리를 20초 관찰하는 대조가 필요하다.
- 다음 사용자 시험: -N -P person.jpg -f5 -t0.5, 성공 시 completed 증가/20초 후 Ctrl+C, 실패 시 새 dmesg 확보 후 별도 사진 1회 재시험. 코드/모델/드라이버 변경 및 assistant 보드접근 없음. 첨부 원본 diagnostics/2026-11-02/reboot-photo-baseline-success.log 보관.

## -N 실패 뒤 별도 사진 프로그램도 실패, 프로세스·메모리 확인 — 2026-11-02

- 첨부2da88e32: uptime1098.57에서 ps에 rkipc/사용자 스트리머/추론 앱이 보이지 않음. [venc], [rkisp-unite0], [rknpu_power_off]는 커널 스레드 표기이며 앱 실행 증거로 취급하지 않는다. MemFree62688kB, MemAvailable118812kB, CmaTotal102400kB, CmaFree0. 이번 필터에 CmaAllocated/CmaReleased가 제외되어 실제 값 미확인; 이전 보드 이력에서 CmaFree0과 CmaAllocated12/CmaReleased102388이 함께 관측된 바 있어 CmaFree0만으로 고갈 확정 금지.
- 커널 uptime963.731/979.540에 dmabuf alloc failed args.size409600 두 건. 이전 두 출력 메모리 할당 실패와 부합하며 실패 크기 확인. uptime988.647 job176731c4 task80(0x50) wait6.391초/timeout elapsed6.511초, irq0/raw0/require0x300, softreset. 앞선 -N 실행 실패와 counter 일치.
- 같은 첨부에서 이후 cctv_person_image도 전체 할당/NPU ready 후 op18 Conv:/model.4/cv3/conv/Conv, task96, rknn_run -1로 실패. 사진 실행 이후 dmesg는 없으므로 task96 커널 상세는 미확인. 별도 사진은 모델 초기화와 추론을 같은 주 스레드에서 수행하므로 현재 오류를 작업 스레드만의 문제로 설명할 수 없음. 이전 timeout 뒤 잔류 상태 가설은 남지만 확정 아님.
- 다음은 현재 실패 상태 로그 보존 후 사용자 재부팅, rkipc 종료 완료/전체 Cma 항목/로드 rknpu srcversion·progress_diag 유무 확인 뒤 별도 사진을 먼저 한 번 실행해 정상 기준 회복 여부 확인. 아직 스레드 변경/런타임 업그레이드/드라이버 교체 안 함. assistant 보드 접근 없음.

## 카메라 미초기화 모드에서도 초기화 성공 후 추론 실패 — 2026-11-02

- 사용자 `cctv_person_npu_no_camera -N -P person.jpg -f 5 -t 0.5` 세 번 실행. 첫 두 번은 `errno:12 Cannot allocate memory / STARTUP ERROR: NPU output allocation failed`; 첫 실행 뒤 killall rkipc 수행. 세 번째는 입출력 할당/NPU ready/READY NO_CAMERA까지 성공했으나 PERSON 성공 없이 op12 Conv:/model.4/cv2/conv/Conv, taskcounter80, intstatus0, rknn_run -1로 종료했다.
- 이번 앱 자체가 카메라/AIQ/VI/VENC/RTSP를 초기화하지 않는 경로에서도 실패한 관측이다. ps·새 dmesg·현재 메모리 상태는 미제공이므로 보드 전체 카메라 정지, 잔류 상태, 커널 timeout/리셋 상세는 미확인. RTSP+NPU 동시 부하만으로 설명하지 않으며 스레드 원인으로도 확정하지 않는다. killall 직후 두 번째 실패→세 번째 할당 성공이 지연된 자원 해제 때문인지는 추정이다.
- npu_model.cc 재확인: output[0..2] 속성/크기 로그는 실제 입출력 할당 루프보다 먼저 출력한다. 마지막 output[2] 로그만으로 세 번째 출력 버퍼 할당 실패라고 특정할 수 없다. NPU ready가 전체 할당/바인딩 완료 기준이다.
- -N 옵션이 포함된 실행파일의 실제 보드 실행 확인으로 이전 보드 미시험 상태 해소(안정 동작은 실패). 다음은 재부팅/드라이버 변경 전 ps·메모리·uptime/새 커널 로그를 확보하고 동일 상태의 별도 사진 프로그램을 한 번 실행해 비교한다. 별도 사진만 성공하면 동일 바이너리 내 같은 스레드/작업 스레드 비교가 다음 분리 후보다. assistant 보드 접근 없음.

## 카메라 미초기화 + 동일 추론 스레드 대조 옵션 구현 — 2026-11-02

- 사용자 진행 승인으로 현재 전달본 code/src/main.cc에 `-N -P image.jpg` 추가. 주 스레드 model.open → 동일 run_inference 작업 스레드 1개/LatestFrame 슬롯을 유지하고 Camera.start, AIQ/SYS/VI/VENC/RTSP 초기화 및 VI 획득을 생략한다. 단조 시계로 -f 상한에 맞춰 사진 clone을 전달하며 pts=0, result_age는 사진 선택 이후 시간이다. 다른 카메라 프로세스는 자동 종료하지 않는다.
- -N은 -P 필수, -D/-S와 조합 거부. PERSON completed는 성공한 추론 누적 횟수. 진단모드 NPU 실패는 비정상 종료하고 오류 문구에 영상 없음 표시; 일반 송출의 AI 실패 후 영상 유지 정책은 유지했다. 모델/추론코어/드라이버 변경 없음.
- 기존 -P 및 -S -P의 ARM 빌드·보드 timeout 결과를 전달 문서에 반영하고, 새 -N 시험 절차와 구분했다. 카메라 미초기화 성공 시 카메라 활성 영향을 더 좁힐 수 있으나 자원·타이밍 차이도 있으므로 원인 확정은 아님.
- 로컬 분기/정리 경로 검토 및 전달 ZIP CRC·전체 파일 바이트 일치, build.sh LF, 모델 SHA256 유지 확인. 새 -N의 ARM 빌드/보드 시험은 미수행. 사용자 VM에서 빌드 후 cctv_person_npu_no_camera 별도 이름으로 전송해 실행할 예정. assistant 보드/ADB/SSH 접근 없음.
- 최신 전달본 deliverables/CCTV_person_detection.zip. 이전 ZIP은 backups/handoff-before-no-camera/CCTV_person_detection.zip에 보존. 실행 절차는 전달본 FIXED_IMAGE_TEST.md.

## 고정 사진 시험 안내 후 커널 timeout 로그 수신 — 2026-11-02

- 사용자 dmesg: uptime63.207~63.447, job9241f602, wait 약6.221초/timeout시 elapsed6.341초, taskcounter19(0x13), irq0/raw0/required0x300, soft reset 확인. 초기메모리할당실패가 아닌 작업완료대기timeout 로그다.
- 저장된 diagnostics/2026-10-30/fixed-image-stream-timeout.log와 대조하여 job9241f602/uptime/카운터/시간이 정확히 같은 기록임을 확인. 전체로그의 -P 및 FIXED_IMAGE로 고정사진+송출시험 오류를 식별했으며 새 실패로 중복집계하지 않는다. 이미 후속 -S -P의 job ca4c0aac 실패도 기록되어 같은 시험을 다시 요청하지 않는다. 다음새비교는 카메라 미초기화+동일 추론스레드/슬롯 고정사진 조건이다. 보드접근 없음.

## 고정 사진 + 단일 AI VI도 NPU timeout — 2026-10-30

- 첨부efe9c7db: cctv_person_npu_fixed_image -S -P person.jpg -f5 -t0.5. FIXED_IMAGE612×368 및 NV12변환우회, READY DIAGNOSTIC SINGLE로 AI VI1 640×360만 활성/주VI·VENC·RTSP비활성 확인. 센서 RAW3864×2192/isOnline0/isMultiIsp1 및 메모리할당 성공.
- PERSON 성공 없이 op3 Conv:/model.1/conv/Conv, task19, rknn_run -1. 진단모드 자동정리 후 셸 복귀. 새 커널job ca4c0aac uptime194초대 wait6.440초/timeout6.560초, irq0/raw0/require0x300/task0x13, softreset. uptime63의job9241f602는이전-P송출시험이므로구분.
- 현재 동일바이너리의고정사진조건에서도 인코더/RTSP/주VI와카메라픽셀변환을빼고재현. 그요소들은이번실패의필수조건이아님. 남은후보는ISP/센서/보조VI동시활성,메모리/런타임,컨텍스트초기화·추론스레드등. 동일task19두번만으로특정레이어버그확정금지.
- NPU DISABLED의 video loop remains independent는공통오류문구이며-S에서영상이송출된다는뜻아님. 다음새비교는현재앱과동일한RKNN초기화→추론스레드/슬롯을유지한카메라미초기화고정사진대조가유효. 해당전용옵션은아직구현되지않음. 원본diagnostics/2026-10-30/fixed-image-single-vi-timeout.log보관. 보드접근·설정변경없음.

## 고정 사진 입력 + 카메라 송출 시험에서도 NPU timeout — 2026-10-30

- 첨부08a89308: cctv_person_npu_fixed_image -P person.jpg -b4096 -f5 -t0.5 실행. FIXED_IMAGE 612x368 / NV12 conversion bypassed로 사진 입력 적용 확인. 모델 입출력 할당 성공 및 BGR ready, IMX415 RAW3864×2192/isOnline0/isMultiIsp1, 주/보조VI와H265시작 확인.
- PERSON 성공 출력 없이 op3 Conv:/model.1/conv/Conv, taskcounter19, rknn_run -1 발생. VIDEO14.8 ai=enabled 뒤 NPU DISABLED, VIDEO15.0 ai=disabled 두 번 지속. Ctrl+C 후 두VI streamoff/AIQ deinit 확인.
- 같은첨부 dmesg job9241f602 uptime63초대 task19(0x13), wait6.221초/timeout6.341초, irq0/raw0/require0x300, softreset. 직전사용자dmesg와같으며 앱counter와도일치. 새실행에서 고정사진도실패한근거로기록.
- 카메라 픽셀매핑/NV12복사/BGR변환을 우회했는데도 재현되어 그 경로만을 필수원인으로볼수없음. ISP/NPU동시성,메모리접근,스레드/초기화 등은남음. 버스경합 또는 드라이버원인 확정은아님. -P ARM빌드와보드실행미검증상태해소, 안정동작은실패.
- 다음 최소 비교는 같은바이너리의 -S -P로 고정사진입력을유지하고 주VI/VENC/RTSP를제외. 과거-S는카메라픽셀입력이었다는차이명시. 새코드/드라이버변경없이사용자시험. 원본 diagnostics/2026-10-30/fixed-image-stream-timeout.log 저장. assistant보드접근없음.

## 고정 사진 비교 소스 ARM 빌드 성공 — 2026-10-30

- 사용자 VM /home/student/CCTV_person_detection/CCTV_person_detection/code에서 build.sh 성공. GCC8.3.0 ARM/uClibc로 person_inference 및 cctv_person_image/cctv_person_npu 링크 완료, dist에 실행파일과 모델 설치 완료.
- OpenCV imgcodecs 헤더 검사는 실패했지만 highgui 헤더 검사 성공으로 opencv2/highgui.hpp 선택. 해당 Failed는 최종 빌드 실패가 아님. 최신 -P 옵션 소스의 ARM 빌드 대기 상태는 해소되었으며 보드 배포·-P 실행 결과는 아직 미확인.
- 다음은 사용자 직접 dist/cctv_person_npu를 보드 프로젝트 내 cctv_person_npu_fixed_image로 전송, 기존 카메라 앱 종료 후 -P person.jpg -b4096 -f5 -t0.5 실행. 보드/VM 자동접근 없음.

## 전달 스크립트 CRLF 오류 수정 — 2026-10-30

- 사용자 VM sh build.sh 실행 시 7행 set: Illegal option - 발생. 로컬 전달본과 ZIP의 build.sh 모두32줄 CRLF, 7행 set -eu\r\n 확인. Windows 텍스트 저장 과정에서 들어간 CR로 POSIX sh 옵션 해석이 실패한 원인과 부합. 컴파일/NPU 단계 오류가 아님.
- build.sh를 바이트 단위로 LF 정규화하고 전달 ZIP 갱신, CR 없음/ZIP CRC/스크립트 동일 바이트 확인. VM에서는 현재 code폴더에서 sed -i 's/\r$//' build.sh 후 기존 빌드 명령 재실행하도록 안내. 보드/VM 직접 실행 없음.

## 전체 실패 원인과 하드웨어 사양 재검토 — 2026-10-30

- 공식 PicoZero 위키/제품 페이지, Rockchip RV1106 Rev1.7 및 Sony IMX415-AAQR 자료 재확인. Zero RV1106G3/단일A7최대1.2GHz/DDR3L256MB/INT8 1TOPS/4lane CSI. G3 ISP4K15fps와 위키5M30fps를 조건별 표기로 구분; 센서 단품 최대읽기속도를 보드 실현FPS로 사용하지 않음.
- 전체 이력에서 CMA할당실패, NPU실행timeout, ISP/VI FPS제한을 분리. 이전 제조사스트리머+별도사진timeout과 -S단일VI실패가 있어 우리 입력코드·RTSP·이중VI만으로 전체 실패를 설명할 수 없음. 최상위 조사영역은 현ISP/센서환경에서 NPU동시실행(드라이버·메모리접근·공유자원)이며 구체원인 미확정.
- 코드 검토에서 명백한 VI반환후포인터사용/동일RKNN동시호출은 찾지 못함. 공식 예제도 NPU먼저초기화/명시mem_sync없음이어서 그 차이를 확정버그로 주장하지 않음. 공식 지원표SC3336와 우리IMX415차이, 작은1944×1097모드AIQ SIGSEGV, 저온46~52도 실패, 사용자420MHz동일보고 반영.
- force-to-2-readback문구는 ISP_UNITE_ONE조건으로도 출력되고 NPUload는미완료job시간을포함한다는 로컬소스 재확인. 경고=스펙초과,100%=과부하차단으로 단정하지 않음. 최근별도사진성공의동시스트리머증거부족 유지.
- docs/CCTV_FAILURE_REVIEW.md에 사양·실측·우선순위·다음비교·공식링크 기록. 다음은 미수행-P시험과 별도스트리머병행성공조건확인. 이번보드/VM/ADB접근·설정변경·앱수정없음.

## 전달본 설명 주석 표기 정리 — 2026-10-28

- 사용자 요청으로 현재 추출본 deliverables/CCTV_person_detection/CCTV_person_detection의 주석에서 [설명] 표기만 제거했다. 설명 문장과 [출처] 표기는 유지하고 실행 코드 변경이 없는지 확인했다. 전달 ZIP 재생성 및 CRC/파일 일치 확인 완료.

## 전달본 출처 표기 가독성 개선 — 2026-10-28

- 사용자 요청에 따라 현재 전달본 작업 경로 deliverables/readable-staging/CCTV_person_detection 확인. SOURCE_MAP은 예제 파일별 역할/우리 파일별 참고 함수/직접 구현한 부분으로 구성되어 있으며, 남아 있던 R1~R5 표기를 문서와 코드·헤더 주석에서 실제 예제 파일명으로 모두 교체했다. 한글 조사에 붙은 식별자도 검사했다.
- 실행 코드 줄 변경 없음 확인. 두 전달 ZIP 재생성 후 CRC 및 전체 파일 바이트 일치 검증. 보드/ARM 추가 시험 없음. 현재 전달용은 deliverables/CCTV_person_detection.zip.

## 팀장 전달 자료 검증 상태 갱신 — 2026-10-28

- 사용자 요청으로 전달본 README를 짧은 안내로 정리하고 STATUS.md에 사진 성공(97.8~105.5ms/23~24개), 카메라 입력 NPU 실패, 오류 후 VIDEO15fps 회수 유지, enabled≠추론성공, 원인 미확정 및 검증 한계를 통합했다. 날짜별 이력 없이 현재 흐름으로 작성.
- code/README.md와 SOURCE_MAP.md의 오래된 전체 보드 미검증 서술을 수정하여 기존 보드 실행 결과와 최신 -P 소스 미빌드/미시험 상태를 구분. FIXED_IMAGE_TEST.md는 미수행 시험 절차임을 명시. 이번 코드·모델 변경 없음.
- deliverables/CCTV_person_detection.zip을 최신 전달본으로 갱신하고 fixed_image ZIP도 동일 내용으로 동기화. 기존 ZIP 두 개는 backups/handoff-before-status-refresh에 보존. 두 ZIP CRC 및 파일 바이트대조 통과. 실제 팀장 전송/보드 접근 없음.

## 고정 사진 입력 비교 옵션 구현 — 2026-10-28

- 사용자 진행 요청으로 전달본 추출 디렉터리 deliverables/CCTV_person_detection/CCTV_person_detection/code의 main.cc에 -P 사진 경로 추가. 카메라/두VI/인코더/RTSP/추론스레드/최신슬롯은 유지, 보조VI 획득·반환도 유지. 선택 프레임의 픽셀매핑/NV12복사/BGR변환만 우회하고 시작 전 imread한 원본 크기 사진 clone을 전달. 기본 카메라 입력은 유지.
- FIXED_IMAGE 시작로그, PERSON source=fixed-image 또는 camera, input 크기 및 사진용 box_image 좌표 구분. 사진로딩 실패/이미지IO 미지원은 시작 전 오류. CMake에서 기존 이미지IO 헤더검사 결과를 실시간 타깃에도 전달. 모델/추론코어/드라이버 변경 없음.
- FIXED_IMAGE_TEST.md와 README/SOURCE_MAP 갱신. 새 deliverables/CCTV_person_detection_fixed_image.zip 제공, 기존 CCTV_person_detection.zip은 비교용 기존본으로 보존. ZIP CRC 및 전체 파일 바이트대조 통과. 로컬 코드 경로 검토만 수행했으며 ARM 컴파일/보드 시험은 미수행. 보드에는 사용자가 새 바이너리를 cctv_person_npu_fixed_image 별도 이름으로 전송하여 시험한다.
- 사진 로드 메모리/복사량/실행 타이밍도 달라지므로 성공·실패를 특정 원인 확정으로 해석하지 않는다. 고정 입력과 일반 입력을 동일 새 바이너리에서 -P 유무로 비교할 수 있음.

## 기존 스트리머 병행 비교 요청 후 사진 추론 성공 — 2026-10-28

- 사용자 사진 로그: PERSON count=24, detect_ms=97.8, 실패 없이 프롬프트 복귀. 마지막 박스 score0.510으로 threshold0.5 바로 위. 이전23개와 검출수 차이만으로 정확도 개선을 주장하지 않음.
- 앞서 NPU 없는 기존 스트리머를 켠 채 별도 사진 추론을 요청한 데 대한 응답. 다만 이번 출력에는 스트리머 실행명령/동시 실행 상태가 없어 병행 성공 해석은 해당 조건을 전제로 함.
- dmesg 끝은 uptime338 task22/raw0xc0000,381~382 task10/raw0x0 timeout과softreset으로 직전 두 카메라앱 실패 카운터와 일치. 이번 사진실행 실패 근거가 아니며 새 사진오류는 제공되지 않음.
- 전달본 추출 경로 deliverables/CCTV_person_detection/CCTV_person_detection/code 확인. 사진은 동일 스레드 model.open/infer, 실시간은 주 스레드open 후 작업스레드infer; 보조VI640x360 및 NV12복사/BGR변환 경로도 차이. 이것들이 오류라는 증거는 없음. 다음 코드 진단 후보는 기존 파이프라인을 유지하고 추론 입력만 고정사진으로 교체해 입력/캡처와 나머지 조건을 분리하는 것. 현재 코드 수정/보드 자동실행 없음.

## 원본 모듈 환경: 카메라 두 번 실패 후 사진 성공 — 2026-10-28

- 첨부17bedcfc: 사진23명/105.5ms → killall rkipc 및 ps grep 무출력 → cctv_person_npu -b4096 -f5 -t0.5 두 번 실행. 첫 실행 op3/task22, 두 번째 op2/task10에서 rknn_run -1. 첫 실행은 AI 비활성 뒤 VIDEO15.0fps 두 번 유지. 각 실행 Ctrl+C 후 streamoff/AIQ deinit 확인.
- 이어 사진 프로그램은23명/98.2ms 성공. 첨부상 재부팅/모듈교체 없이 이어짐. 직전 NO_PROGRESS_DIAG 환경에서의 연속시험으로 해석하며 이번 첨부 자체에는 모듈 재조회와 dmesg는 없음. 원본 환경에서도 카메라 실패 뒤 사진이 계속 실패한다는 가설은 이번 관측에서 성립하지 않음. 진단 패치의 인과관계도 미확정.
- 카메라 실행 조건 또는 카메라 앱 고유 경로가 중요한 차이. NPU 모델의 항상 재현되는 실패나 RTSP 단독 원인으로 단정하지 않음. 다음 분리는 NPU 없는 기존 스트리머를 켠 상태에서 별도 cctv_person_image 실행하여 카메라/인코더 활성 영향과 카메라 앱의 입력·스레드 경로 영향을 비교. 별도 프로세스라 메모리 배치 차이 등은 남음. 보드 실행은 사용자만 수행.

## 재부팅 뒤 사진 추론 정상 복귀 — 2026-10-28

- 사용자 첨부8eb68a49 및 사진 로그: 로드 모듈 srcversion 0BBC54BA6B23914910050C9, NO_PROGRESS_DIAG. 진단 모듈이 아닌 재부팅 기준 환경에서 BGR 사진 추론 PERSON count=23, detect_ms=105.5로 성공하고 셸로 복귀. 런타임1.6.0/드라이버0.9.2. 최신 결과 JPG의 박스 정확도는 별도 미확인.
- killall rkipc 직후 ps에는 PID638 rkipc -a /oem/usr/share/iqfiles가 여전히 존재. 종료 처리 중일 가능성이 있어 사진 실행 시 카메라 완전 정지 또는 동시 실행 여부는 확정할 수 없음.
- 재부팅과 진단 모듈 해제가 함께 일어났으므로 직전 사진 timeout을 진단 패치 또는 timeout 후 잔류 상태 중 하나로 단정하지 않음. 다음은 현재 모듈을 유지하고 rkipc 종료를 확인한 뒤 카메라+NPU 1회, 오류 발생 시 종료 후 사진 재시험으로 원본 모듈에서도 실패 후 상태가 지속되는지 비교. 보드 시험은 사용자 수행, assistant 보드 접근 없음.

## 진단 드라이버에서 사진 시험도 timeout — 2026-10-28

- 사용자 사진실행 cctv_person_image에서 입출력할당/BGR ready 뒤 op18 Conv:/model.4/cv3/conv/Conv, taskcounter95로 rknn_run -1. 새job0541afd2 uptime2484~2489,1.017~5.177초샘플 task95/task_reg0x600005f 고정,irq0/raw0/mask0x300/required0x300;6.057초wait/6.177초timeout/softreset.
- 앞선task22는카메라시험, 이번95는사진시험으로구분. 사진프로그램자체는카메라를시작하지않지만이번ps출력미제공으로타카메라프로세스부재미확인. 진단모듈교체/직전카메라timeout·reset이선행하여, 사진실패를진단패치원인또는카메라와무관한영구고장으로확정하지않는다.
- 다음은원본/oem모듈을보존한임시로드전제하에사용자재부팅후progress_diag노출여부/로드srcversion/프로세스상태확인,다른카메라앱종료뒤사진시험을먼저실행해기준복구. assistant보드조작없음. 현재는카메라동시실행만실패한다는서술을절대조건으로쓰지않는다.

## NPU 진행 카운터 정체 관측 — 2026-10-28

- 진단모듈 활성 후 새job b21e6400, uptime2282~2287: elapsed1.066/2.107/3.147/4.187/5.227초 모든 샘플에서 task22, task_reg0x1700016, irq0/raw0, mask0x300/required0x300. 약6.027초wait,6.147초timeout 뒤soft reset. 최초 약1초 이전 진행은 미관측이며22를모델22번째레이어로해석하지 않는다.
- 이번샘플기간에 task카운터진행없음/완료상태비트없음/실제마스크와요구마스크일치를확인. 단순추론속도저하보다는작업정체와부합하지만메모리/클록/ISP공유자원/드라이버원인은미확정. CPU로의IRQ전달유실만으로설명하는가설을지지하는완료raw비트도보이지않음. 첫uptime1215의task20/raw0xc0000은이전실행으로구분.
- 이번사용자는커널로그만제공;앱PERSON/VIDEO/VLC재생상태는별도미확인. 다음분리확인은진단모듈을유지하고카메라앱종료상태에서사진단독재시험(진단패치적용후정상기준확인). 보드자동실행없음.

## 진행 진단 모듈 활성 확인 — 2026-10-28

- 사용자 보드 /proc/modules: rknpu27659 참조수0 Live(O), /sys/module/rknpu/parameters/progress_diag=Y. 임시 진단 모듈 로드 및 파라미터 활성 확인. 기존/oem 모듈 파일 변경 여부를 새로 관측한 것은 아님.
- 다음 시험은 같은 cctv_person_npu -b4096 -f5 -t0.5를1회 실행하고 오류 후 종료, dmesg의RKNPU progress/timeout 로그 확보. 모듈로드 성공을NPU추론성공으로 취급하지 않는다. assistant 보드접근 없음.

## 모듈 교체 뒤 초기화 로그 수신 — 2026-10-28

- 사용자 dmesg 끝 uptime2138.867~2138.868에 non-iommu mode, Initialized RKNPU driver v0.9.2 for20230825, dev_pm_opp_set_regulators no regulator(rknpu) -19 출력. 앞의uptime1215 timeout은 이전시험이며 새모듈시험결과로 취급하지 않는다.
- 새 드라이버 초기화 경로 실행은 관측되나 DIAG_MODULE_LOADED/ORIGINAL_RESTORED 및 progress_diag값은 미제공. 원본도같은0.9.2이므로 진단모듈 활성 여부 미확정. 다음은/sys/module/rknpu/parameters/progress_diag와/proc/modules 확인. no regulator문구만으로 NPU전원부재나timeout원인을 단정하지 않는다.

## 진행 진단 모듈 메타데이터 확인 — 2026-10-28

- 사용자 VM 새 rknpu.ko .modinfo: version0.9.2, parm progress_diag(bool), srcversion C0DA7AEBC21E6E88AE31053, depends빈값, vermagic `5.10.160 mod_unload ARMv7 thumb2 p2v8`. 보드 uname5.10.160, /proc/modules `rknpu 27019 0 - Live`, 기존파일 /oem/usr/ko/rknpu.ko 확인. 참조수0은 해당 조회시점의 값이며 전체ABI호환/새모듈로드성공은 아직 미확인.
- 다음은 사용자 직접 수행으로 새모듈을/tmp/rknpu-progress-diag.ko에 전송하고 카메라/NPU앱 종료 후 강제옵션 없이 rmmod/insmod 임시교체. 원본/oem 파일은 덮어쓰지 않고 신규로드 실패 시 원본 insmod 복구. 현재까지 보드적용 확인 없음.

## NPU 진행 진단 모듈 빌드 성공 — 2026-10-28

- 사용자 첨부6809bb0a-ae67-482a-8856-066e15145dd9: VM ARM 교차 빌드에서 rknpu_job.o 포함 컴파일, modpost, rknpu.ko 최종 링크 완료, make 정상종료 및 BUILD_EXIT=0 확인. 생성경로 sysdrv/source/objs_kernel/drivers/rknpu/rknpu.ko.
- V=1 출력에 보이는 ERROR: Kernel configuration is invalid 문구는 test 실패 시 실행할 echo 조건부 명령의 표시이며 이번 빌드의 실제 오류가 아님. 빌드 성공은 보드 ABI호환/모듈적용/진단동작 성공과 구분한다.
- 다음은 VM 산출물 .modinfo(특히 vermagic/progress_diag)·해시 확인과 보드 기존 모듈 위치/현재 로딩상태 확인. 보드모듈교체/자동실행 없음.

## 진행 진단 패치 적용 및 빌드 중단 — 2026-10-28

- 사용자 VM apply.sh check에서 원본해시 일치 확인 후 apply 성공. rknpu_job.c.before-progress-diag 백업 경로 출력 확인. 패치 재적용 불필요.
- M=drivers/rknpu modules 빌드가 사용자 Ctrl+C로 중단됨. 로그는 make 디렉터리 진입 후 __sub-make Interrupt이며 컴파일 오류/성공 근거는 없음. build_status 저장줄 미실행으로 BUILD_EXIT 빈 값. 기존 rknpu.ko가 있더라도 이번 패치 산출물로 취급하지 않는다.
- 다음은 VM Bash에서 tee로 진행출력을 보며 재빌드하고 파이프 직후 PIPESTATUS[0]을 저장. 빌드가 조용하다는 사실만으로 멈춤을 단정하지 않는다. 보드 적용 없음.

## VM NPU 소스 원본 확인 — 2026-10-28

- 사용자 VM sysdrv/source/kernel/drivers/rknpu/rknpu_job.c SHA256=1235ce135a3e0a46e7339b929d8893ac67366e82261263877540fec61d886f3c, grep progress_diag 결과 없음. 준비한 진행 진단 패치의 원본 해시와 정확히 일치. VM 소스에는 해당 진단 패치가 아직 반영되지 않은 것으로 확인.
- 다음은 npu-progress-diag 패키지를 VM에서 check/apply 후 M=drivers/rknpu 모듈만 빌드하는 단계. 보드 모듈 적용/실행은 아직 수행되지 않음. 패치 소스 해시 및 ZIP 무결성 로컬 확인.

## 진행 진단 파라미터 확인 — 2026-10-28

- 사용자 보드에서 /sys/module/rknpu/parameters/progress_diag 읽기 가능 여부 검사 결과 `progress_diag parameter not found`. 현재 sysfs 경로에서 진단 파라미터가 노출·읽기 가능하지 않음. 이것만으로 모든 패치 미적용/드라이버 고장을 확정하지 않음.
- 다음 단계는 VM의 현재 rknpu_job.c 해시 및 progress_diag 문자열 확인. 준비된 packages/current-source/npu-progress-diag.zip은 원본해시 일치 시 적용하도록 구성되어 있음. 현 단계 보드모듈교체/재부팅 지시나 자동실행 없음.

## 동일 실행 커널 로그 분석 — 2026-10-28

- 첨부c85c1066-e5e3-4232-9e48-7b985d7bf7d0: uptime1208대 카메라시작,1215대 RKNPU wait 약6.153초/timeout6.273초, taskcounter20(0x14), irq0/raw0xc0000/require0x300, soft reset 확인. 앱 failed to submit은 이번에도 커널 작업완료 대기 timeout과 연결됨. raw 상위비트 의미는 제공소스만으로 미확정; IRQ유실/버스오류로 단정하지 않는다.
- 센서3864×2192/10bit, ISP420MHz, over virtual isp max resolution→2 readback, 센서VTS2250→4500 변경 관측. VENC설정1920×1080 H265/4096000bps/GOP60/fps30:30. 실제 VIDEO약15fps와 설정30fps를 구분. VTS증가가15fps와 부합하는 단서이나 단독으로 정확한 센서실측FPS를 입증하지는 않음.
- 기존 -S(VENC/RTSP없음)에서도counter20/raw0xc0000 timeout이 기록되어 있으므로 RTSP가필수원인 또는 송출분리로NPU해결이라고 말하지 않는다. 현재timeout후 영상유지실측은 앞선로그로확인, VLC시청은 미확인. 기존 준비한 progress_diag진단 모듈 활성여부부터 확인해 대기중카운터 변화를 볼 필요가 있음. 이번보드접근/파일변경없음.

## 1080p H.265 송출 중 AI 오류 격리 실측 — 2026-10-28

- 사용자 첨부355f852e-41b8-4578-a62e-00bc3cb4ceb7 로그: cctv_person_npu -b4096 -f5 -t0.5, BGR모델 초기화/IMX415 AIQ/두VI/mainpath+selfpath/HEVC VPS·SPS·PPS 생성 성공. sensor raw3864×2192, rkisp-unite0/isMultiIsp1은 관측이며 장애 원인 확정 아님.
- VIDEO fps14.8 ai=enabled 이후 RKNN failed to submit, Conv:/model.1/conv/Conv, task counter20, int status0, rknn_run -1. PERSON 성공 로그 없음. NPU DISABLED 후 VIDEO fps15.0 ai=disabled가4회 지속. 따라서 NPU 실패가 앱 전체 종료로 이어지지 않고 인코더 회수가 계속됨을 확인. VLC 실제 재생/수신 성공은 사용자 확인 전. NPU timeout/추론 실패 자체는 미해결이며 이번 커널 timeout 세부는 dmesg 미제공.
- Ctrl+C 후 RTSP삭제/VI streamoff/AIQ stop/deinit 출력. 전체 약26초 실행으로 장시간 안정성이나30fps 검증 아님. -f5가 영상5fps를 강제하지 않는 것은 실측 확인. 현재15fps 제한의 원인은 아직 확인하지 않음. 다음으로 이 실행의 dmesg 확인 필요.

## BGR 전처리 사진 추론 실기 결과 — 2026-10-28

- 사용자 보드에서 cctv_person_image model/yolov5.rknn person.jpg result.jpg 0.5 실행. NPU ready: BGR 640x640, PERSON count=23 image=612x368 detect_ms=98.3, 실패 메시지 없이 셸 프롬프트로 복귀. 런타임1.6.0/드라이버0.9.2, 세 출력 속성 조회 및 메모리 할당·추론 경로 통과를 관측.
- 이전 RGB/다른 후처리의24명과 달리 현재23개 검출. 전처리/후처리 변경에 따른 차이가 후보이나 원인 단정 불가. 결과 JPG의 실제 내용·누락/중복 박스는 아직 확인하지 않음. 전체 정확도나 실시간 카메라 병행 안정성 검증으로 확대하지 않는다. 다음 시험은1080p H.265+AI(-b4096 -f5 -t0.5), VIDEO fps/PERSON 및 오류 시 영상 유지 여부 확인.

## 1080p 복원 및 송출·추론 분리 — 2026-10-28

- 사용자 요청으로 현재 deliverables/CCTV_person_detection/code를 1920×1080 H.265 직접 VI→VENC 바인딩으로 복원. AI는640×360 보조VI, 캡처/추론/송출 분리. -f 기본5는 다시 AI 입력 선택 상한이며 영상 FPS와 무관. -I 보조VI선택, -D 두VI진단/-S AI전용진단 복원. GOP60 유지. 영상박스 없음, 사람결과 로그 출력임을 README/응답에 명시.
- include/latest_frame.h의 최신 프레임 한 칸 대기 공간 사용. 잠금 밖 추론, 오래된 대기 교체, 처리중 복사본 소유권 유지. 실행중 NPU/AI캡처 오류는 ai_failed로 격리하고 정상모드의 기존 영상 스레드는 계속 실행. 자동재시도 없음. 모델할당/초기화 실패는 여전히 시작 실패, 영상오류/Ctrl+C는 전체 종료. 공유 하드웨어 장애나 부하로 인한 FPS 하락까지 해결했다고 주장하지 않는다.
- 예제에 맞춘 BGR/검정 여백·NMS·속성조회순서는 유지; 전처리/후처리/사진코드와 헤더가 직전 검증본과 동일함 확인. ISP→SYS 순서 유지. 실제 latest_frame.h에 대한 MSVC C++ 동시성 테스트 통과(느린 소비자, 최신값, 소유권, clear/stop). 정적 송출경로 분리 확인. 전체 ARM 빌드/보드/ADB/VM 시험 없음.
- 이전 순차본은 backups/serial-before-independent-1080 보관. README 두 곳/출처 설명을 현재 흐름으로 갱신, 기존 전달 ZIP 재생성 및 CRC/바이트대조 통과. 검증 기록 diagnostics/example-alignment-check/independent_1080_verification.json. 최신 코드 기준은 전달본이며 이전 작업 원본 프로젝트는 변경하지 않음.

## 공식 예제 순차 처리로 전달본 재구성 — 2026-10-28

- 사용자 요청으로 현재 전달본 deliverables/CCTV_person_detection/code를 예제 유사 구조로 변경. 현재 수정 기준은 이 폴더이며 cctv_npu_output_fix_complete의 기존 병렬 프로젝트는 보존했다. 이전 전달본은 backups/example-alignment-20261028에 보관, 현재 CCTV_person_detection.zip도 갱신/CRC/파일별 바이트대조 확인.
- 단일 VI0 720×480, 버퍼2/depth2. 모델→RGB pool→ISP→SYS→RTSP→VI→VENC, 같은 스레드에서 획득→BGR→추론→사람박스→RGB888 SendFrame→H.265 RTSP→반환. 보조 VI/바인딩/mailbox/캡처·추론·송출 스레드 제거. H.265·4096Kbps 유지, 예제 HEVC GOP60. 기본 앱 처리율 제한 없음; -f는 이제 영상 송출도 제한한다. -S/-D는 동일 VI0에서 인코더·RTSP만 제외, -I 제거.
- NPU 출력 속성을 모두 조회한 뒤 입력·출력 메모리 할당/바인딩. BGR/검정 letterbox·크기절삭/공통scale, 점수 엄격초과, +1 IoU, NMS 이후 모델경계 제한/정수화로 맞춤. 예제와 남는 차이는 명시적 NV12→BGR, 송출 BGR→RGB, 사람필터/최대128/std::sort/영상경계검사/RAII 등. 원본과 완전 동일한 출력은 보장하지 않는다.
- Windows MSVC를 발견하여 실제 C++ 후처리 검사를 컴파일/실행, 사람필터/NMS/양자화/패딩/잘린텐서·공통scale·신뢰도경계·경계밖박스NMS 및 원본 CalculateOverlap 수치대조 통과. 좌표 형변환 C4244 경고 존재. 진단파일 diagnostics/example-alignment-check. 전달본에는 tests/tools를 다시 추가하지 않았다.
- 전체 ARM 빌드/보드/ADB/VM/VLC 시험 없음. 전처리가 바뀌어 기존 사진24명 성공은 현재 코드 검증이 아님. NPU timeout 미해결 유지. 현재720×480 비교용 구성은1080p SRS 충족 아님. README 두 곳/출처설명을 날짜 이력 없이 현재 흐름으로 갱신.

## 전달본 빌드 설명 보완 — 2026-10-27

- deliverables/CCTV_person_detection/code의 CMakeLists.txt와 build.sh에 교차 컴파일러 탐색, SDK/OpenCV 의존성, 공통 라이브러리·두 실행파일, 링크 경로, dist 설치 단계 주석 추가. 실행 설정 변경 없음.
- 같은 README에 CMakeLists/build.sh/자동 생성 Makefile의 관계와 빌드 단계 설명 추가. 삭제된 tests/tools 관련 오래된 설명 수정. 실제 빌드·보드 시험 및 기존 ZIP 갱신 없음.

## 축소 전달본 빌드 설정 정리 — 2026-10-27

- 사용자가 include/model/src만 남긴 전달 폴더 deliverables/CCTV_person_detection/code의 CMakeLists에서 HOST_TESTS_ONLY·삭제된 tests 참조·tools 변환 안내·README 및 선택 JSON 설치를 제거. 두 실행파일과 RKNN 모델 설치, ARM/SDK/OpenCV 호환 설정은 유지. build.sh의 설치 설명도 수정.
- CMake 블록 짝/소스·모델 존재/삭제 항목 참조 제거를 정적 확인. 로컬 CMake 및 ARM 빌드 환경 부재로 실제 configure/build는 미수행. 작업 원본 프로젝트와 기존 ZIP은 변경하지 않았으며 보드 시험 없음.

## 예제 구현 정밀 대조 — 2026-10-27

- 로컬 YOLO 예제 및 simple_test와 현재 소스를 재대조. 핵심 RKNN API/YOLO 수식과 모델 SHA256은 대응하나 전처리(BGR/검정 대 RGB/114), NMS(+1/경계 제한 순서), 카메라 초기화 순서, 단일 루프 대 다중 스레드/VI 구조가 다르다. 동일 구현·동일 검출 결과로 설명하지 않는다.
- IoU 산술 반례: 같은 박스에서 원본 0.4667/현재 0.4286으로 공통 임계값 0.45 가정 시 억제 여부가 다름. C++/보드 실행 결과가 아닌 수식 비교. NMS는 rknn_run 후이므로 이 차이를 NPU timeout 원인으로 단정하지 않는다.
- 상세 기록 diagnostics/2026-10-27/EXAMPLE_CONFORMANCE.md. 현재 전달본 src 네 파일은 작업 소스와 동일. 실행 코드/전달 ZIP 변경 및 보드/VM 시험 없음. H.265 실기 미검증·카메라 병행 NPU timeout 미해결 유지.

## 전달 문서 흐름 통합 — 2026-10-27

- 사용자 요청으로 전달 설명/코드 출처 주석의 작성일·버전별 변경서술 제거, 현재 H.265 코드 흐름으로 통합. 새 배포명 deliverables/CCTV_person_detection.zip 및 같은 이름 폴더 사용(날짜/버전 접미사 없음). 출처 링크/원본 저작권 연도/검증한계 유지. 보관 이력은 내부 문서에만 남김. 코드 실행문 동일성 및 ZIP 검사 통과, 보드실행 없음.

## H.265 인코딩 변경 — 2026-10-27

- 후속 요청으로 현재 작업 README·SOURCE_MAP 및 v3 전달본 설명 전체를 H.265 현재형으로 통합 정리. v2 주석만변경 문구의 중복/혼동 제거, 축소본에 없는 tools 구분, WC-F-002 표도 코드반영/검증대기로 갱신. 과거 구축/코드검토 문서는 과거기록 표시 및 최신 설명 안내. 동일 v3_h265.zip 재생성/검증, 원문/구ZIP backups/h265-docs-20261027 보관. 코드/모델 해시 불변 확인, 보드시험 없음.

- 사용자 확인 후 커스텀 main.cc의 VENC를 RK_VIDEO_ID_HEVC/VENC_RC_MODE_H265CBR/stH265Cbr로 변경, RTSP 식별자도 H265로 변경. H264E_PROFILE_HIGH 제거 후 공식 simple_test HEVC 분기와 같이 영 초기화 값 유지. help/READY/출처 주석 갱신.
- 해상도/기본4096Kbps/GOP30/채널/사람추론/모델/-D/-S 흐름 유지. backups/h265-before-20261027/main.cc 보관. 허용한 코덱 변경 외 실행문 동일성 및 ZIP CRC/내용 검사 통과. ARM 빌드·보드·VLC 검증 없음, NPU timeout 미해결.
- 최신 전달본 deliverables/CCTV_person_detection_2026-10-27_v3_h265.zip. 이전 사용자 축소 범위와 README 본문 보존 후 최신 상태 안내 추가. SRS WC-F-002는 이제 코드 반영/실기 검증 대기이며 종전 검토의 H264 미구현 판정은 변경 전 상태다.

## SRS 적합성 검토 — 2026-10-27

- docs/SRS_2026-09-20.pdf(v1.1 표기,21쪽) 전체 텍스트와 주요 표 렌더링을 검토, 현재 사람 인식 소스와 대조. diagnostics/2026-10-27/SRS_COMPLIANCE_REVIEW.md에 보드19개/비기능23개 및 타 부체계 범위 기록. 코드/보드 변경 없음.
- 전체 충족 아님. WC-F-012 사진 성공/실시간 타임아웃으로 부분. H264 단일RTSP이므로 WC-F-002 H265/003 이중송출 미충족, AI용VI는 저화질 송출아님. 이벤트자동JPG/전후MP4/존마스킹/heartbeat/자동복구 미구현. 외부VPN·서버·DB·앱·웹은 현재 코드만으로 전체 구현 여부 판단 불가.
- SRS 자체 주의: ✅가 가능성/필수성도 포함, heartbeat 수정10초 대 기존30초, PTZ/업데이트032/033 매핑변경, NF-P-003 수정25초, 요약 웹캠15/비기능18 대 본문19/23 불일치. 현재 통합코드와 SRS에 기재된 다른 시험/팀 구현을 혼동하지 않는다. AI30fps나 정확도 숫자는 SRS에 명시되지 않음.

## 팀장 전달 자료 및 한국어 주석 — 2026-10-27

- 후속 요청: 소스12개 파일 전체에 [출처] 주석 추가. 로컬 공식 yolov5.cc/postprocess.cc/main.cc/luckfox_mpi.cc 및 simple_test/simple_vi_bind_venc_rtsp.c의 실제 함수와 대조해 API흐름/알고리즘/프로젝트 변경분을 명시. 줄단위 복사 이력은 단정하지 않음. SOURCE_MAP.md에 원본 파일 SHA256과 참조 경로 수록. RKNN 모델이 로컬 공식 예제 모델과 바이트 동일함 확인.
- 전달 폴더의 사용자 편집(README 축약, tools/로그/code README 제거)을 발견해 기존 본문 및 축소 범위를 보존. 새 deliverables/CCTV_person_detection_2026-10-27_v2.zip에는 남겨둔 코드10개에 출처 주석 반영, SOURCE_MAP.md와 설치에 필요한 짧은 code/README.md 추가. tools2개는 작업 소스에만 주석 반영. 축소본 Python 테스트가 빠진 tools를 필요로 함을 안내. 기존 전달본은 그대로 보존.
- 검증: 추가 주석 제거 시 원문12개와 동일, Python AST동일, 포함된 사용자 코드와 변경 전 원문 동일, 작업 소스 Python 경로테스트2개 통과, ZIP CRC/바이트대조 통과. 보드/VM 시험 없음. 원본 backups/provenance-comments-20261027 보관.

- 사용자 요청으로 전달 범위를 사람 인식으로 축소. deliverables/CCTV_person_detection_2026-10-27.zip에 주석 포함 NPU 프로젝트(빌드 의존 README/tests/tools 포함), 기존 모델, 설명서1개와 사진 검출 로그 발췌만 구성. FPS/센서/드라이버 패치·공식예제 사본·전체 이력 제외. 이전 종합 전달본은 보존. ZIP CRC 및 내용 동일성 검사 통과. 외부 전송 없음.

- 사용자 요청으로 커스텀 NPU 프로젝트 src/include/build/tools/tests/CMake 12개 파일에 설명 주석94개 추가. 추가 주석을 제거해 원문과 같음을 검증했으며 실행문 변경 없음. 원본 backups/comment-only-20261027 보관. 공식 예제와 기존 진단 패키지는 원문 유지.
- deliverables/CCTV_team_handoff_2026-10-27에 코드·모델, 공식 로컬 예제, FPS v6/IMX415/NPU진행진단 패키지, 사진/FPS/NPU 증거 로그, 진행보고·코드설명·전달문안 정리. 성공/미해결/미검증 구분. 전체 SDK·펌웨어·실행바이너리 미포함. 실제 외부 전송 없음.
- 주석 변경 후 Python 경로 검증 테스트2개 통과. ARM 재빌드 및 보드 실행 없음. 이번 전달본 소스와 설치 바이너리의 일치는 미확인. 전달본 문서가 과거 README의 시점별 미완료 문구보다 최신 상태를 설명함.

## NPU 코드 정적 검토 — 2026-10-27

- 후속 코드 흐름 비교(실행 없음): 로컬 공식 예제는 RkLunch-stop.sh→NPU초기화→RGB송출용MB풀→ISP→SYS→RTSP→VI0→VENC, 단일 루프에서 VI획득→색변환/letterbox→동기추론→박스그리기→VENC SendFrame/GetStream→RTSP→VI반환. 우리 코드는 NPU→SYS→RTSP(기본모드)→ISP→VI0+AI VI→VENC/바인딩 후 캡처/추론/송출 분리. 공식은 추론 중 VI프레임을 보유, 우리는 독립 RGB복사 후 주 루프 끝에서 반환하며 추론과 다음 캡처가 겹칠 수 있다. 양쪽 RKNN 핵심은 native속성조회→UINT8입력→create_mem/set_io_mem→rknn_run→후처리로 동일 계열. 전처리는 공식 BGR변환/검정여백, 우리 RGB변환/114여백으로 같지 않음. 공식 직렬 앱루프도 ISP를 추론 중 정지시키지는 않으므로 하드웨어 동시부하가 없다고 해석하지 않는다. 공식 예제는 추론 반환값을 main에서 검사하지 않고 while(1)에 정상탈출이 없어, 우리 종료처리와도 다름. 이 차이들이 현재 timeout 원인인지는 미확정.

- 공식 YOLO 예제 재확인: LuckfoxTECH/luckfox_pico_rkmpi_example kernel-5.10.160에 luckfox_pico_rtsp_yolov5 존재. 공식 지원표는 RV1106/Buildroot/SC3336이며 Zero+IMX415 조합 검증으로 확대 해석하지 않는다. 로컬 luckfox_pico_rtsp_yolov5/src/main.cc도 존재하며 기본720×480, 카메라 프레임→YOLO→박스그리기→VENC→RTSP 순서 확인. 우리 커스텀 병렬 구현과 비교할 기준 예제로 활용 가능; 이번 공식 예제 실행은 하지 않음. 출처 https://github.com/LuckfoxTECH/luckfox_pico_rkmpi_example/tree/kernel-5.10.160 .

- 사용자 요청으로 로컬 cctv_npu_output_fix_complete/cctv_person_npu 소스·테스트 및 NPU 진행 진단 패치를 검토. 보고서 diagnostics/2026-10-27/CODE_REVIEW.md. 보드/ADB/VM 접근, 실행 검증, 제품 코드 변경 없음. 설치 바이너리와 소스 일치는 이번에 확인하지 않음.
- 코드 확인: NPU 예외가 공통 stopped를 설정해 영상도 종료시키는 구조. -f는 AI 프레임 선택 상한이며 센서/ISP FPS가 아님. 모델 입력은640×640고정. CPU NV12/RGB 변환·복사가 있어 전체 zero-copy는 아님. result_age_ms는 VI 수신 이후 앱 내부 시간이고 전체 스트리밍 지연이 아님.
- 최신 프레임1개 대기, 단일 추론스레드, NPU 입출력메모리 재사용 및 논리/할당 크기 구분 확인. 현재 타임아웃의 원인이 밝혀진 것은 아님. 기존 -S 재현 때문에 RTSP만의 경합으로 단정하지 않는다. 운영 개선 우선은 AI 장애와 영상 종료 분리, 종료/복구 정책 및 측정 의미 명확화.

## 케이스 파일 조사 — 2026-10-06

- 후속 물리 호환 비교: 공식 치수 이미지 직접 확인. Zero PCB65×30mm/모서리4개 Ø3mm 고정홀, Pro/Max PCB72×21mm/RJ45, Ultra PCB50×50mm/고정홀 세로간격45.10mm/RJ45. Zero의 가로65mm는 Ultra50mm보다 크고 폭30mm는 Pro21mm보다 커서 각 보드에 밀착한 케이스는 그대로 대체하기 어렵다. 큰 외함의 수정·별도 고정 가능성과 완전 호환은 구분한다.
- 근거: https://wiki.luckfox.com/assets/images/Zero-size-bf9625aa6dbaa50c5af9bb68488505b0.webp , https://www.luckfox.com/image/catalog/development-board/Luckfox-Pico-Pro/Luckfox-Pico-Pro-details-size.jpg , https://www.luckfox.com/image/catalog/development-board/Luckfox-Pico-Ultra/Luckfox-Pico-Ultra-details-size.jpg . 로컬 hardware-models/에3개 이미지 저장. Plus는 공식 제품자료에서 RJ45/배치차이 확인했지만 이번에 공식 치수 이미지 원문은 미확인.
- 사용자가 찾는 대상은 보드 형상 모델이 아니라 출력용 케이스 파일이라고 확인했다. Pico Zero 전용 및 IMX415 함께 수납하는 케이스 STL을 공개 웹에서 검색했으나 이번 조사에서 호환을 명시한 파일은 확인하지 못했다. 파일이 존재하지 않는다는 단정은 아님.
- 확인한 다른 모델용 자료: https://cults3d.com/en/3d-model/gadget/case-luckfox-pico-plus-pro 는 Plus/Pro용 유료 케이스(3MF 및 STLs.rar). https://github.com/OOHehir/luckfox-pico-case 는 Pico Ultra + LF40-480480-APK 디스플레이용이며 STL 2개와 수정 가능한 SCAD 소스 제공, 제작자가 작업 중이라고 명시.
- 위 자료는 우리 Pico Zero + IMX415 호환으로 추천하지 않는다. 케이스 후보 파일 다운로드/설계 변경/출력 검증은 수행하지 않았으며 기존 공식 보드 STEP 파일은 케이스 설계 참고용으로 유지.

## 보드 3D 모델 조사 — 2026-10-06

- 사용자 요청으로 Luckfox Pico Zero 보드의 STL/3D 모델을 조사. 공식 다운로드 문서는 3D 모델을 STEP 형식으로 안내한다: https://wiki.luckfox.com/Luckfox-Pico-RV1106/Downloads/ . 공개 검색에서 Pico Zero 전용 STL은 확인하지 못했으며, 존재하지 않는다고 단정하지 않는다.
- 제조사 공식 https://github.com/LuckfoxTECH/Luckfox-Pico-docs/blob/main/Hardware/3D%20Models/Luckfox-Pico-Step.zip 다운로드 및 압축목록 확인. 실제 `Luckfox-Pico-Step/Luckfox Pico Zero.step` 포함, 비압축 49,469,345바이트. 로컬 `hardware-models/Luckfox Pico Zero.step`에 추출. 원본 ZIP도 같은 폴더에 보존.
- 이 파일은 보드 STEP 모델이며 출력용 케이스 STL이나 IMX415 포함 조립체로 확인한 것이 아니다. 형상 시각검증/실물 치수 대조/STL 변환은 아직 수행하지 않았다. 보드 접근 없음.

## NPU 작업 재개 — 2026-10-04

- 사용자 VM rknpu_job.c 해시1235ce...6f3c가 로컬과 일치. packages/current-source/npu-progress-diag.zip 준비: 단일 rknpu_job.c, diff, 원본해시검사/백업/복원 apply.sh, VM모듈 빌드 안내. 기존 wait 절대마감/완료/리셋 논리는 유지하고 약1초마다 작업카운터/IRQ/raw/mask를 읽는 진단. 타이밍영향 가능, ARM빌드/실기 미검증. 보드 적용 없음.
- 사용자 가설 RTSP+NPU 부하에 대한 설명: 현재 -S는 mainVI/VENC/RTSP 없음. ISP/카메라+NPU 동시성은 의심되지만 RTSP경합으로 설명할 수 없음. load99~100은 미완료job 점유시간 포함. 기록상 timeout→softreset→앱 카메라정리→뒤늦은power off 순서이며 부하수치만으로 과부하 전원차단이라고 판단하지 않는다.

- 사용자20샘플 수집 분석: NPU IRQ37(ff660000.npu)는 initial4/final4 및 모든 샘플4로 증가0. 반면 ISP IRQ40 +808, IRQ41 +202, CIF IRQ43 +204. 이번 대기 중 NPU IRQ가 CPU에 집계되지 않았지만 IRQ전달 결함과 작업 미완료를 구분할 근거는 아직 없음.
- 실행중 샘플4~9 CMA allocated74712/total102400KiB, released27688KiB, MemAvailable약111992~112020KiB. 초기 할당 실패/총량 고갈 증거 없음(연속할당 가능량/순간피크까지 배제하지 않음). 온도 전체46.386~52.326도, NPU freq700MHz/power on, ISP420MHz 유지. power는 드라이버 참조상태라 전압 안정성 증명 아님.
- selfpath640x360 frame14→93이1253.86→1259.16(5.30초) 사이 증가79, 약14.9fps. rate66ms/VTS4500과 부합. Input @30fps 표시를 실측30으로 해석하지 않음. 이번 -S조건은 FPS진단 -F30과 다름.
- 새 실패는 uptime1259대 taskcounter68(0x44), raw0/irq0, 약6.37초 wait/6.49초 timeout. 첨부 상단 uptime689대 counter20/raw0xc0000은 이전 시험 로그다. 동일 실행으로 혼동하지 않음. 카메라는 NPU timeout 중에도 프레임 진행.
- 원본 diagnostics/2026-10-04/npu-state-user.log 저장. 다음은 커널 job 진행 상태/마스크를 시간대별 관측하는 최소 진단 추가 검토. VM rknpu_job.c 해시를 로컬 검토본과 비교 후 패치 준비하며, 완료 강제인정/IRQ우회/타임아웃연장 해결책은 적용하지 않는다. 보드 접근 없음.

- 로컬 소스 조사: rknpu_job.c timeout은 INT_STATUS/RAW 레지스터 직접 조회, IRQ 완료 판정은 fuzz_status와 job int_mask 비교. raw0xc0000은 요구0x300비트 없음; 제공소스에 상위비트 의미 미정의라 버스오류로 단정하지 않음. drv hrtimer의 미종료job 점유시간으로 load 계산하므로100%를 정상 연산 진행 증거로 해석하지 않음.
- 외부 issue544 및 Luckfox issue359는 같은 작성자의 유사 보고(독립2사례 아님). ISP online1 대 우리 online0 차이 존재, AXI 경합은 작성자 가설. 확인된 해결패치는 찾지 못함. 조사 보고 diagnostics/2026-10-04/NPU-INVESTIGATION.md 및 읽기전용 capture-npu-state.sh 준비. 다음 사용자가 IRQ/CMA/전원/ISP 상태 동시 수집 후 최소 진단 패치 여부 결정. 이번 조사에서 보드 접근/드라이버 변경 없음.

- 사용자 추가 확인: 현재 보드 /proc/rknpu/freq=700000000, VM PicoZero DTS &npu assigned-clock-rates700000000 및 &{/rkisp@ffa00000} rockchip,unite=<1> 유지. 사용자는 “420MHz로 바꿨을 때도 똑같았어”라고 이전 시험 결과를 명시했다. 이전 미완료 기록보다 이 사용자 보고를 우선하며, 420MHz에서도 증상 지속한 것으로 기록한다. 단 당시 보드 런타임 freq420000000 확인 여부/실행 로그/동일 실패 단계는 아직 제공되지 않아 별도 미확인으로 구분한다. 같은 클록 변경을 즉시 재권고하지 않는다. assistant 보드 접근 없음.

- 최신 사용자 -S -f5 -t0.5 재시험: NPU 입출력 할당/NPU ready, AI selfpath640x360/버퍼3 성공, main VI/VENC/RTSP 없음. PERSON 출력 없이 op3 /model.1/conv/Conv, task counter20에서 rknn_run -1. 커널 약6.055초 대기/6.175초 timeout, irq0/raw0xc0000/require0x300/task0x14 및 soft reset 확인. 과거 raw0과 차이는 기록하되 비트 의미/원인은 확정하지 않음.
- 센서3864x2192/10bit, dphy892Mbps, 시작 후 VTS4500으로 변경, ISP420MHz/force to2readback 관측. 이번 -S는 -F30 FPS진단 프로그램과 다르므로 센서 실측30fps로 취급하지 않는다. 새 첨부에 CMA 할당 실패는 없으며 실행 중 메모리 여유가 충분하다는 증명까지는 아님.
- 사진 단독 성공과 카메라 동시 timeout이 현재 상태에서도 재현됨. 다음은 미완료였던 NPU 클록 대조 준비: 사용자가 현재 /proc/rknpu/freq와 VM DTS의 npu/ISP 보존 상태 확인 후 진행. ISP420MHz와 NPU 클록은 구분. 아직420MHz 적용/펌웨어 변경 없음. 원본 diagnostics/2026-10-04/npu-single-vi-timeout.log. assistant 보드 접근 없음.

- 사용자 재시험 성공: 제공 ps에 rkipc/스트리밍/NPU 사용자 프로그램 없음(대괄호 venc/rkisp-unite0/rknpu_power_off는 커널 스레드). cctv_person_image에서 모든 입출력 할당 성공, PERSON count24/image612x368/detect_ms105.1, 셸 복귀 확인. runtime1.6.0/driver0.9.2 유지. 현재 사진 단독 추론 기준 재확인, 카메라 동시 성공을 의미하지 않음. JPG 자체 시각 검사는 이번에 하지 않았다.
- 원본 diagnostics/2026-10-02/npu-image-recheck-20261004.log에 저장(파일명 날짜가 실제 재시험 날짜). 다음 사용자 시험은 기존 cctv_person_npu -S -f5 -t0.5로 카메라 단일 VI+NPU 동시 실행 비교. 에러 시 반복 대신 종료 직후 dmesg 확보. ADB 접근 없음.

- 사용자 사람 인식 NPU 작업 재개 요청. 현재 성공 범위는 카메라 미실행 상태의 이미지 추론(24명 검출, 약98~100ms)이며 카메라/ISP 동시 NPU 추론은 약6초 job timeout으로 미해결이다. VENC/RTSP 없는 -D/-S 및 제조사 카메라+사진 프로그램에서도 실패 이력이 있다.
- 이후 센서 mode30 모듈 설치와 FPS v6 시험을 했지만, 변경된 센서 모드에서 NPU 재검증한 결과는 아직 없다. NPU420MHz 비교는 준비 이력만 있고 적용 성공 기록은 없다. 마지막 확인 NPU700MHz와 ISP420MHz를 구분한다.
- 재개 첫 단계는 사용자가 카메라 프로세스 상태를 확인하고 이미지 단독 추론 기준을 재확인하는 것. 결과 후 현재 센서 모드에서 동시 실행 비교를 진행한다. 이번 기록 작업은 로컬만 수행, ADB/보드 접근 없음.

## 사용자 제공 네트워크 주소 확인 — 2026-10-04

- 후속 사용자 재조회에서도 wlan0 MAC `74:24:ca:50:04:af`로 동일함을 확인했다. 사용자가 두 조회 사이 수행한 작업을 `reboot`라고 명시하여, 이번 재부팅 전후 MAC 유지가 확인됐다. Wi-Fi 변경 전후 비교는 아직 미확인이다.
- 사용자 실행 `cat /sys/class/net/wlan0/address` 및 `ifconfig` 결과: wlan0 MAC `74:24:ca:50:04:af`(두 출력 일치), IPv4 `192.168.45.194/24`, UP/RUNNING. usb0 MAC `ae:c7:a3:7f:40:3b`, IPv4 `172.32.0.93/16`로 별도 인터페이스임을 확인.
- 보드 식별용 MAC 검토 기준은 wlan0 값. 이번 재부팅에서 유지된 사실과 공장 영구 MAC 여부·펌웨어 재설치 후 유지 여부는 구분한다.
- wlan0 IP는 현재 관측값이며 고정/예약 여부와 할당 방식은 이번 출력만으로 확인되지 않는다. SSID·인터넷/VPN 연결·RTSP 재생 성공도 이번 출력으로 판단하지 않는다. assistant의 직접 보드 접근 없이 사용자 결과만 기록했다.

## 10/4 회의 안건 검토 — 2026-10-04

- 사용자가 제공한 `KakaoTalk_20261004_163038910.jpg` 회의 안건을 검토했다. 이미지의 이전 회의 결론: 보드 식별자는 MAC 주소로 통일, 카메라와 보드를 프로젝트에서 1:1로 매핑. 이는 전체 하드웨어 지원 한계를 검증한 결과가 아니다. 사용할 네트워크 인터페이스와 MAC 영속성·소유권 인증 방법은 추가 합의 대상이다.
- 이미지의 오늘 논의 사항: VPN/로그인 순서, MediaMTX 중계 및 브라우저 재생, 고/저화질 스트림, 보드6시간·서버 장기 녹화, 이벤트 임계값/전송/미확인 알림, ICD 단계별 작성과 시험기간 일정. 회의에서 최종 합의한 것으로 기록하지 않는다.
- 공식 MediaMTX 문서 확인: H.265 WebRTC 재생은 브라우저/환경 제약이 있어 'PC·안드로이드 100% 지원'으로 표현 불가. RTSP→WebRTC/HLS 전달과 영상 코덱 재인코딩을 구분하고, 필요한 코덱 변환은 별도 처리 및 비용 검증 대상이다.
- 회의 전달 시 최신 실측을 우선: 10/2 기록에서 센서/CIF30fps, ISP/VI 약20fps이며 VENC/네트워크 없는 VI 단독에서도 재현. 카메라 미실행 시 이미지 NPU 추론 성공과 카메라/ISP 동시 실행 timeout 이력은 구분한다. 신규 실기 시험은 하지 않았다.
- 설계 검토 제안: 영상 파일은 파일/객체 저장소, DB는 메타데이터; 6시간 용량은 비트레이트·실제 여유공간으로 산정; 네트워크 복구 시 파일 재전송 ACK/중복 방지/삭제 정책 합의. 10초 heartbeat와 WireGuard keepalive/handshake는 별개. 사용자 오프라인과 카메라 오프라인, 알림 전달 여부와 읽음 상태를 구분한다.

## 최신 첨부 SRS의 보드 통신 요구사항 정리 — 2026-10-02

- 사용자 제공 `C:/Users/ohhoj/Downloads/c41f62fb-eb58-4653-a728-9912ada27dfd_SRS.pdf`(26쪽)를 읽고 보드의 송수신 데이터 표 초안을 작성했다. 이번 작업은 문서 분석이며 보드 접근·실기 시험·인터페이스 구현은 하지 않았다.
- 이번 첨부본 §3.1 기준: WC-F-022 로컬 보관 기본값 **6시간**, 네트워크 장애 중 저장 및 복구 후 미전송분 일괄 전송; WC-F-023 이벤트 JPG/MP4 서버 업로드 추가. 과거 SRS의 기본 30일과 구분한다.
- 관리 ID가 변경됨: WC-F-031 Wi-Fi 변경 시 VPN 연결 유지, WC-F-032 **10초 heartbeat**, WC-F-033 부팅 시 NTP 동기화 및 실패 재시도, WC-F-034 WireGuard handshake 상태 보고. WC-F-040 프로세스 장애 감지, WC-F-041 시스템 레벨 장애 복구. 이번 첨부 §3.1 고유 WC-F ID는 21개다.
- 비기능 기준은 NF-P-001 영상 지연 1.5초 이내, NF-P-002 이벤트 알림 5초 이내, NF-P-003 상태 갱신 10초 이내. 문서의 검증 표시를 새로운 실측 결과로 취급하지 않는다. WC-F-004의 과거 네트워크 병목 추정과 최신 로컬 ISP/VI 약20fps 실측을 구분한다.
- 통신 표는 SRS의 서버 RTSP pull 및 B↔S↔C 구조 기준. 보드↔클라이언트 직접 통신은 확정 요구로 추가하지 않는다. 데이터 필드·응답·재전송 식별자·설정 버전 등은 ICD 협의용 제안이며 확정 API가 아니다.
- 추가 협의: 사람 감지 이벤트 분류(person 별도/움직임 하위), 수동 촬영·녹화 실행 위치(보드/서버), 오디오 스트림 사양, 등록 식별자(MAC/시리얼/외부 등록코드) 매핑, VPN 최초 프로비저닝 경로, 미업로드 파일 만료·용량 부족 정책. 프라이버시 마스킹은 보드에서 처리하며 AI 박스 없는 원본 보존과 마스킹 제외 원본 노출을 혼동하지 않는다.

## 보드 접근 및 시험 담당 원칙 — 2026-10-02

- 사용자 명시 지시: 보드 시험은 사용자가 직접 수행한다. assistant는 임의로 ADB를 통해 보드 시험이나 명령을 실행하지 않는다. 조회를 포함한 ADB 접근도 사용자의 별도 명시 요청이 있을 때만 수행한다.
- 앞으로는 로컬 소스/기록 작업과 실행 명령 안내를 제공하고, 사용자가 전달한 보드 실행 결과를 분석한다. “올렸어”, “다음으로”, “어떻게 해?”는 보드 직접 실행 허가로 해석하지 않는다. 과거 직접 ADB 접근 허용으로 해석한 내용보다 이 지시가 우선한다.
- 직전 assistant가 실행한 VI 단독 1080p/540p 시험은 모두 정상 종료했다. 이 지시 이후 보드 접근 없이 로컬 기록만 갱신했다.

## VI 버퍼 비교 준비 — 2026-10-02

- v6 사용자 실측 완료: VI_RATE_CONFIG src=-1/dst=-1, 버퍼2/depth1, 기존 illegal param Src/Dst0/0 경고 없음. 첫5초19.39fps, 이후20.32/20.31fps, get_failures0, 400프레임/error0/정상 종료. 경고 제거로30fps 미달 문제는 해결되지 않았다. 원본 diagnostics/2026-10-02/vi-v6-rate-minus1.log 저장. 보드 접근 없음.
- 다음은 VI 버퍼/FPS 옵션 재빌드 반복을 종료하고 ISP unite/readback 구간 분석. 로컬 rkisp.c 992~1029에서 is_multi_overflow/unite_div가 sw_rd_cnt 및 rdbk_times에 영향을 주는 코드 확인. 해당 분기가 실기20fps의 원인이라는 증명은 아님. 사용자에게 v6 실행 중5초 간격 CIF/ISP 누적 카운터 두 스냅샷 요청하여 단일 시점 rate표시 외 실제 진행량 비교 예정. 커널/IQ/클록 변경은 하지 않음.

- 다음 대조 revision6 준비: 로컬 제조사 example/vi/sample_multi_vi.c 317~318행의 VI 설정에 따라 stFrameRate Src/Dst를 -1/-1로 명시. v5의 초기화값0/0 경고를 제거하는 대조이며20fps 원인이라는 확정은 아님. 버퍼2/depth1/1080p/-F30 유지. v5 대비 기능 변경은 VI 두 필드 설정뿐(진단 출력/버전 표시 추가). packages/current-source/isp_fps_test_source_v6.zip 무결성/구성 확인, VM ARM빌드/보드실행 미완료. ADB 접근 없음. 사용자가 v6를 별도 이름으로 전송/실행한다. 경고 수정 후에도20fps면 해당 설정 반복을 멈추고 ISP unite/readback 경로 분석으로 진행.

- 후속 사용자 실측 완료: v5 -V 1920x1080/-F30/-c400, -B2 안정 구간20.26/20.24fps, -B4는20.22/20.20fps. 각각 mb pool create success MBCnt2/4로 풀 생성 확인, depth1 동일, get_failures0, VI_TEST_DONE400/error0 및 정상 정리 로그 확인. 버퍼2→4 증가로 FPS 개선 없음. 이번 첨부에는 실행 중 proc 통계가 없어 ISP 드라이버 큐 적용값까지 확정하지 않는다.
- 공통 AIQ 실행 중 fps30/2250줄/594MHz, sensor raw3864x2192, sensor is united1/isMultiIsp1. VI Src/Dst0/0 경고도 계속 존재하며 원인 여부 미확정. 종료 시 accept failed는 정상 프레임 완료 이후 관측되어 이번 FPS 제한의 근거로 사용하지 않는다.
- 원본 diagnostics/2026-10-02/vi-v5-buffers-2-vs-4.log 보관. ADB 접근 없음. 다음 -B4 실행 중 ISP/CIF 통계와 커널 로그를 사용자에게 받아 readback 경로 및 실제 큐 상태를 확인한다.

- 로컬 소스 revision5에 -B 2..4 추가. 기본2 유지, VI depth1(-V) 유지. 같은 실행파일에서 -B2/-B4로 버퍼 수만 비교하도록 준비했다. 센서/IQ/클록/해상도는 동일 조건으로 유지한다.
- packages/current-source/isp_fps_test_source_v5.zip 제공. 소스/ZIP 검토만 완료했으며 VM 빌드 및 실기 시험은 미완료. 이번 작업에서 ADB 접근이나 보드 명령 실행은 하지 않았다. 사용자가 빌드/전송/시험을 수행하고 결과를 전달한다.

## VI 단독 실측 완료 — 2026-10-02

- 사용자 업로드한 /oem/usr/bin/simple_vi_bind_venc_rtsp_fps_v4 확인. 기존 fps 실행파일은 별도 이름으로 보존. 다른 카메라 프로세스가 없는 상태에서 ADB로 -V -F30 시험을 실행했다.
- 1920x1080 -c600: revision4 확인, 첫 5초 VI19.38fps 이후20.20~20.25fps, get_failures0, 600프레임 error0/종료코드0. 실행 5초 후 CIF30fps/33ms/오류0, ISP mainpath1080p49ms/delay166ms/core420MHz/unite X2 관측.
- 960x540 -c400: 첫 구간19.34fps 이후20.19~20.21fps, get_failures0, 400프레임 error0/종료코드0. ISP 입력은3840x2160@30 그대로, 출력960x540/49ms/delay173ms. 출력 해상도 감소만으로 FPS 개선 없음.
- 관측 결론: VENC/RTSP가 없는 VI 단독 경로에서도 약20fps가 재현되어 인코딩/네트워크/VLC 없이 발생하는 제한임. ISP/VI 버퍼·처리·unite readback 경로를 다음 분석 대상으로 좁힌다. 순수 ISP 연산 성능의 절대 한계라고 확정하지 않는다. 두 시험 모두 정상 종료했으며 모듈/IQ/클록 영구 설정 변경 없음.
- 원본 로그와 실행 중 CIF/ISP 통계를 diagnostics/2026-10-02/vi-only-v4* 파일6개로 저장. 다음은 VI 버퍼 개수 및 ISP unite/readback 처리 경로를 근거와 함께 검토한다.

## 최신 요약 및 다음 진단 준비 — 2026-10-02

- 지금까지의 확인 사항과 원본 모듈 복원 방법을 diagnostics/2026-10-02/FPS-CHECKPOINT.md에 정리했다. 아래 과거 기록의 “미설치/ADB 연결 없음” 등은 당시 상태이며 현재는 새 mode30 모듈 설치와 CIF 30fps 수신까지 확인되었다. ISP/VENC 출력은 약 20fps로 미해결이다.
- 원본 모듈 백업은 PC와 보드에 보존되어 있다. 현재 모듈은 영구 경로에 교체되어 있으므로 단순 재부팅으로 원복되지 않는다. 이번 저장/소스 준비 작업에서는 보드 설정을 변경하지 않았다.
- isp_fps_test revision4에 -V VI 단독 진단 추가: VENC/RTSP 없이 GetChnFrame/ReleaseChnFrame 즉시 반복, 5초 실측, -c600 자동 종료, 10초 무프레임 오류 종료. 버퍼 개수2 유지, 취득을 위해 depth1 사용. 기존 rev3 ZIP 유지, 새 packages/current-source/isp_fps_test_source_v4.zip 준비. VM ARM 빌드 및 보드 실측은 아직 미완료.
- 다음은 같은 1920x1080/-F30 조건에서 VI 단독 FPS와 실행 중 CIF/ISP proc 통계 비교. VI20이면 ISP/VI, VI30이면 후단 연결/버퍼/송출 영향 우선 확인. 단일 결과만으로 원인 확정하지 않는다.

## IMX415 모듈 호환성 비교 — 2026-10-02

- 새모듈 -F30 사용자실측: AE_RESULT30/2250줄/8800/594MHz로변경, VBLANK58/exposure2025/gain45/link446MHz/pixel_rate356.8MHz; 실제VENC19.89~20.00 유지. 원본보다AE20→30 변화만으로출력30성공이라판단하지않음.
- ADB로 카메라앱 종료상태 확인 후 같은1080p4096/-F30 조건 -c400 자동종료 시험 수행. 첫조회는 종료후라 재시험에서 실행5초후통계저장. CIF Output rate33ms/fps30/readout32ms/frameamount78, irq모든오류0; ISP IspRead rate33ms, Output mainpath1920x1080 rate49ms/delay175ms/frameless0, unite X2 readback/BigMode ON, core420MHz; VENC SrcFr30/TarFr30/RealFps*10=200/cb간격약49.9ms/오류0. 센서-CIF수신은30 통계, ISP출력~20으로구간좁힘. ISP순수처리성능과후단버퍼/역압/드롭 등 정확한원인은아직미확정. 보드전체최대20으로단정하지않음.
- 시험정상자동종료, 파일 diagnostics/2026-10-02/mode30-pipeline-live.log 및 mode30-cif.txt/mode30-isp.txt/mode30-venc.txt 보존. 추가설정/클록/IQ/모듈교체없음. 다음ISP/VI출력경로와후단영향분리 검토.

- 사용자 교체/재부팅 후 모듈 imx415 Live(O), 센서 exposure/gain제어 로그 확인. ADB재조회 설치파일SHA256 0e869dddd062169a8ade507681ad367779bde1cd42cf42517dedbea5cc191cd5로 신규일치, 보드 .before-mode30 백업928567138080b5097fb0d35eb2882481f90106f5cdc57fcf725f70f5d76fbcf7로원본일치. 부팅시rkipc -a /oem/usr/share/iqfiles 자동실행 PID635 관측, 3864x2192/10bit/HDR0 stream on. 현cur_vts4500은rkipc 실행중관측이며30fps진단결과아님. 다음rkipc정상종료 후 rev3 -F30 실측. 아직30fps달성/화질안정성은 미검증; 설치변경은 재부팅해도유지되며 원본복원필요.

- 사용자 새ko /tmp전송성공 후 일반rmmod 실패 Resource temporarily unavailable; 조건부insmod는 실행되지 않음. 직접ADB조회: 알려진카메라앱 없음, fuser 영상/서브디바이스/media 점유출력없음, imx415 refcnt1/holders비어있음. kernel카메라경로 참조잔존 가능성이 있으나 정확한주체는 미확정. 부팅스크립트 /oem/usr/ko/insmod_ko.sh 49행 imx415.ko 로드, 이후CIF/ISP/rockit로드 확인. 새ko로드실패가아니며 호환성오류로판단하지않음. 보드원본SHA256928567...bcf7, /tmp신규0e869...1cd5 일치확인. 강제언로드/파일교체/재부팅안함. 다음 대안은 원본보드내백업+모듈파일교체 후재부팅으로로드(임시로드와달리복원전까지지속되는변경임).

- 다음 시험은 /tmp/imx415-mode30.ko 임시로드로 진행하도록 안내. 원본 /oem/usr/ko/imx415.ko 및 부팅설정 보존. 카메라앱 종료 후 일반 rmmod 성공시에만 새insmod, 실패시 원본insmod 복원 시도. 사용중 오류에서 강제언로드하지 않음. 2026-10-02 현재 ADB연결 없음으로 실행 미완료. 새모듈SHA256 0e869dddd062169a8ade507681ad367779bde1cd42cf42517dedbea5cc191cd5, 백업원본928567138080b5097fb0d35eb2882481f90106f5cdc57fcf725f70f5d76fbcf7 재확인. 모듈로드 결과 및 재등록센서 확인 후 스트리밍시험 예정.

- 새 루트 imx415.ko(366544bytes)와 보드 원본백업 backups/imx415-before-mode30-20261001/imx415.ko(26844bytes)를 ELF직접 파싱 비교. 양쪽 ELF32 little-endian ARM, flags0x05000000, vermagic `5.10.160 mod_unload ARMv7 thumb2 p2v8 ` 일치. 이름imx415/장치alias/depends 일치, 외부참조 심볼집합 동일, this_module크기384 일치.
- 두 파일 모두 __versions CRC항목 없음: 심볼버전 CRC검증 통과라고 해석하지 않음. 원본 intree=Y가 새파일에는 없음(모듈 빌드방식 메타데이터 차이). 새파일에 디버그섹션254141bytes 있어 파일크기증가의 주요부분 설명. 실제 로드 및30fps 스트리밍 안정성 미검증.
- supported_modes의 첫 항목은 원본3864x2192/10bit/NO_HDR20fps, 새파일30fps; 다음20fps와 순서교환 확인, 그 외8개항목동일. 정적검사에서명백한호환불일치없음. 보드 ADB 현재장치목록없음으로 실기적용/현재상태재검증하지 않음. 기존원본백업은유지, 파일교체/재부팅/언로드없음. 상세 diagnostics/2026-10-02/module-compatibility.json, 재현스크립트compare_modules.py.

## FPS 시험 복원 기준 보존 — 2026-10-01

- 사용자 요청에 따라 backups/fps-baseline-20261001/RESTORE-NOTES.md에 센서형식/컨트롤/ISP/NPU/스트리밍/IQ/복원 절차 기록. VBLANK808은 시험 전 관측값이며 드라이버 default58과 구분. 노출2250/게인46은 자동제어 순간값으로 수동 고정 복원 대상으로 취급하지 않는다. -F15는 시험설정, 원래 AE fixed/value는 미조회.
- 기존 pre-npu-clock-20260930 백업을 보존하고 이번 PC소스/전체컨텍스트 스냅샷과 해시를 별도 저장. 이번 작업은 PC 기록 보존이며 보드 접속/변경/전체디스크 추가백업은 하지 않음. VBLANK1808 적용 결과 아직 미수신.

## ISP FPS 비교 실측 — 2026-10-01

- 사용자 다운로드완료 후 PC확인: 루트에 새로 온 파일은 .imx415.ko.cmd(340bytes, 링크명령 기록)이며 실제 새imx415.ko는 아직 없음. 기존mode30-check내ko는 앞서 확인한20fps구버전이므로 사용하지 않음. ADB연결 ac6e3deb24cfa546 확인, 보드kernel5.10.160, imx415로드됨, 파일 /oem/usr/ko/imx415.ko 발견. 원본을 backups/imx415-before-mode30-20261001/imx415.ko로 읽기백업 및SHA256대조. 보드모듈교체/언로드는 하지 않음. 다음은 VM의 숨김.cmd가 아닌 imx415.ko 실파일 수신.

- 사용자 VM M=drivers/media/i2c modules 빌드 성공(BUILD_EXIT0). CC[M] imx415.o, MODPOST, CC imx415.mod.o, LD[M] imx415.ko를 명시적으로 확인. 같은 대상의mia1321/mis5001/sc3336도 빌드됐으나 적용대상은IMX415만. 새모듈 PC수신/ELF검증/보드백업 및 적용은 아직 미완료.

- imx415-mode30-build-check.tar.gz 수신/안전추출 diagnostics/2026-10-01/mode30-check. FIT내부해시검사 통과(서명검증 아님), NPU700MHz/standalone DTB일치/기존보드백업과 DTB속성차이0. 커널payload SHA256 cd737a73bd24bd6e42fb9ffcd6ca6db94533c22d13f74ee8b264e020867a014c로 기존과 동일.
- 핵심: .config CONFIG_VIDEO_IMX415=m. 첨부소스는30fps패치이나 ELF정적분석 imx415.o 및 imx415.ko supported_modes 첫항목은3864x2192/0x300e/10000:200000=20fps로 기존값. ko SHA256 ff68036a77b61909a21ff570688bded9a8a8031da8ee6828d44bc2895aa91e5f. 즉 kernel빌드 성공이 센서모듈 갱신을 의미하지 않았음. boot플래시로 패치적용 불가, 다음 동일구성으로 M=drivers/media/i2c modules 빌드 후 새ko검증. 모듈교체/언로드/보드설치 아직 안함. verification.json/module-verification.json 저장.

- 사용자 VM DTS백업 .before-mode30-1790842363284477948 생성 및 &npu700MHz 변경 성공. kernel빌드 Running build_kernel succeeded/BUILD_EXIT0, FIT boot.img 생성 및 output/image 설치 확인. 다만 제시로그에는 DTC는 있으나 CC imx415.o가 없어 드라이버 변경 포함 여부 아직 미검증(증분빌드/비활성config/모듈 등 가능). 원본 diagnostics/2026-10-01/mode30-kernel-build.log. 다음 새boot/DTB/.config/드라이버소스 및 존재하는imx415.o/ko/.cmd/빌드로그 수집 후 코드포함 및 보드백업DTB와 비교. 보드플래시 미실행.

- 사용자 defconfig diff는 CONFIG_NET_FOU=y 한 줄 추가로 확인. 센서FPS 설정과 구분하여 사용자 네트워크 변경 유지. 다음 VM단계: 현DTS를 별도 백업하고 &npu 블록의420MHz만 마지막보드확인값700MHz로 복원, ISP unite1/공유CRU/NET_FOU/Wi-Fi변경 유지 후 bash build.sh kernel. 아직DTS복원/빌드/플래시 미실행. 생성이미지는 기존보드백업과 DTB/커널설정 차이를 검토 후 적용할 것.

- 사용자 VM 빌드전 상태: 활성BoardConfig PicoZero EMMC Buildroot RV1106 IPC 일치. DTS &npu는420000000 유지, &cru 700MHz 값 유지, 말미 &{/rkisp@ffa00000} rockchip,unite=<1> 명시 확인. git diff --stat에 imx415.c 8줄(4+/4-), DTS7줄 외 defconfig1줄추가 및 project/app/wifi_app/wifi/librkwifibt.so 변경 존재. 커널 디렉터리 git이 SDK상위 저장소를 보고함. defconfig추가 내용 미확인; 다음 해당diff 확인 후 NPU실험변경 분리 및 kernel빌드 진행. Wi-Fi 바이너리를 임의복원하지 않음. 원본 diagnostics/2026-10-01/vm-pre-mode30-build-state.log.

- 사용자 VM에서 imx415-mode30-test/apply.sh check 및 apply 성공. 적용전 원본해시 a3a847eafb1b8a675af60ed436a73977229c5e6c98c6fb6c8e3a1064bf2d9831, 패키지수정본01d58e9cdac2341501a45c346ebce5d9868c839c95d9135ce9844a362d881c8a 확인. 백업 /home/student/luckfox-live/luckfox-pico/sysdrv/source/kernel/drivers/media/i2c/imx415.c.before-mode30 생성. 소스적용만 완료, 빌드/보드설치/30fps실측 미완료. 다음은 활성BoardConfig 및 PicoZero DTS의 NPU/ISP설정을 조회하여 이전420MHz 변경과 분리.

- VM imx415.c SHA256 a3a847eafb1b8a675af60ed436a73977229c5e6c98c6fb6c8e3a1064bf2d9831로 PC검토소스와 일치 확인(실행커널 일치 증명 아님). packages/current-source/imx415-mode30-test.zip 준비: 4lane표 첫20fps/다음30fps 항목 전체를 교환해 기존891M 레지스터모드 우선선택. 패치와 원본해시 검사/백업/복원 apply.sh 포함. 수정본 SHA256 01d58e9cdac2341501a45c346ebce5d9868c839c95d9135ce9844a362d881c8a. 정적 대조로 두 항목 순서 외 코드 동일 확인; ARM빌드/실기검증 미완료. 기존 NPU420/ISP unite 관련 VM DTS변경은 유지되므로 빌드/플래시 전 보드백업과 별도 비교 필수. 현재 소스 적용만 안내하며 보드 플래시하지 않음.

- -F30 후속 사용자controls: VBLANK58(min58/default58), exposure2224(max2242), HBLANK4936, gain47, link446MHz/pixel_rate356.8MHz. VTS2192+58=2250으로 AE_RESULT2250과 일치. 조회시 스트림 실행중 여부는 로그상 불명확(앞선 실행은 종료됨). 센서 제어값도 변경 확인되어 VBLANK항상808 가설 폐기. dmesg s_stream은3864x2192/hdr0/bpp10만 표시, cur_best_fit 없음으로 모드번호 확정 불가. 같은 AIQ클록/HTS에서30fps는1500줄을 요구하여 현 높이2192보다 작음; 현 타이밍 그대로 VBLANK만 감소하여30fps 달성 불가. 센서의 다른 타이밍모드 선택 검토 필요, 실행커널/VM소스 일치 확인 전 드라이버패치/플래시 보류.

- revision3 -F30: 설정fixed/value30이나 실행중 AE_RESULT fps20/frame_lines2250/line_pixels8800/pixel_clock396MHz, 재요청 후 동일. VENC20.23~20.40, SOF20.25~20.60fps. 이번controls 첨부 없음. 현재 모드에서 AIQ결과20fps 제한 관측이며 보드전체 절대상한으로 해석하지 않음. 로컬 imx415.c 4lane표는 동일3864x2192/10bit/NO_HDR에서 첫20fps, 다음30fps; find_best_fit은 크기거리 동률이면 첫항목 유지하여20fps선택과 부합. 실기커널과 일치 미확인, 다음 dmesg cur_best_fit/s_stream 확인. max_fps숫자만 변경하지 않음. 원본 diagnostics/2026-10-01/fps-v3-request30-ae20.log.

- revision3 -F10 실측: before_vi는 AE15/3000 초기값, running_initial 이후 AE fps10/frame_lines4500/line_pixels8800/pixel_clock396MHz로 변경. 설정fixed1/value10, enabled1/mode1. VENC 첫15.25 이후15.00, SOF 첫15.05 이후15.00fps. FPS요청이 완전히 무시되거나 항상20 고정이라는 가설 수정: 하향 제어가 실측에 영향을 줌. 다만 AE10과 실측15는 불일치. -F15의 비율과 달라 단순1.5배 보정이나30fps 가능 확정 불가. 이번 센서controls 출력은 첨부 없음. 다음은rev3 -F30의 AE값/실측/실행중controls 비교. 원본 diagnostics/2026-10-01/fps-v3-ae10-measured15.log.

- revision3 -F15 보드 실행 성공: AE enabled1/mode1/run_interval0, 설정fixed1/value15. AE_RESULT는 시작전/실행중/재적용후 모두 fps15, frame_lines3000, line_pixels8800, pixel_clock_mhz396. 실제 VENC20.09~20.21fps, AIQ콜백19.90~20.60fps. AIQ조회 내부계산396e6/(8800*3000)=15와 프레임집계20 사이 불일치 확인. frame_lines3000은 이전 VBLANK808+height2192와 일치. 과거 센서 pixel_rate356.8MHz와 이번AIQ396MHz는 다른 시점/의미일 수 있어 곧바로 클록오류 확정하지 않음. 다음 동일rev3 -F10 비교로 AIQ계산/VBLANK/실측이 요청에 따라 함께 변하는지 확인 예정. 소스/펌웨어 변경 없이 비교. 원본 diagnostics/2026-10-01/fps-v3-ae15-measured20.log.

- rkaiq-rv1106-review.tar.gz를 작업공간에서 수신, tmp/rkaiq-review에 안전 추출. 제공 헤더의 Uapi_ExpQueryInfo_t에 Fps/LinePeriodsPerField/PixelPeriodsPerLine/PixelClockFreqMHZ 확인. ELF32 ARM 공유라이브러리의 queryExpResInfo/getExpSwAttr 정의된 동적심볼 확인. 보드 라이브러리와 동일성은 미검증.
- 진단 revision3 준비: AE enable/opmode/run_interval 및 위 AE조회결과 출력. -F생략 IQ-only 실행에도 읽기 진단 활성화, -F지정일 때만 기존 재요청 유지. -a를 필수인수로 수정하여 공백/붙임 모두 수용. 센서/VI/VENC/IQ원본/펌웨어 변경 없음. packages/current-source/isp_fps_test_source_v3.zip 제공. 로컬 API/심볼/압축 구성 검토 완료, ARM빌드/보드실측은 미검증.

- VM RKAIQ검색 결과 확보: RV1106 uClibc 배포 위치 /home/student/luckfox-live/luckfox-pico/media/isp/release_camera_engine_rkaiq_rv1106_arm-rockchip830-linux-uclibcgnueabihf 에 include/rkaiq, lib/librkaiq.so 및 .a, 서버바이너리 존재. out/media/output에 복사본, RV1126 및 RK3588용 별도 배포 존재. 찾던 내부 구현소스는 아직 발견 안 됨; rkaiq_3A_server.c 등 응용소스와 구분. 원본 diagnostics/2026-10-01/vm-rkaiq-paths.log.
- 다음은 VM의 해당 RV1106 배포 헤더+공유라이브러리를 rkaiq-rv1106-review.tar.gz로 읽기전용 수집하여 PC에서 지원 API/구조체 및 라이브러리 심볼 점검. 보드실행 librkaiq.so와 동일성은 아직 미확인. 라이브러리 교체나 펌웨어 플래시는 하지 않는다.

- 사용자 Ubuntu VM에서 /home/student/luckfox-live/luckfox-pico 아래 SensorHw.cpp/rk_aiq_user_api2_ae.cpp/RkAiqAeHandle.cpp 정확한 이름 find 결과 없음. 앞선 보드에서 실행한 동일 검색과 구분. SDK내 해당 이름의 일반파일이 검색되지 않았다는 관측이며 RKAIQ전체소스 부재 확정 아님(다른 이름/압축/심볼릭링크 가능). 다음은 RKAIQ디렉터리/라이브러리/압축파일 위치 조사. 보드설정 추가 변경 없음.

- 보드 /tmp IMX415시험JSON 확인: isFpsFix1/FpsValue15, SHA256 b7cc1243315c93fd57e743d8c7982c3e39c4d90e1f6a9cb0f4f22a0c1be785d6로 PC사본과 일치, /tmp/imx415*에는 JSON하나만 존재(451241bytes). 전송오류/같은이름BIN 가능성 제외. IQ 설정과 런타임 요청 양쪽에서20fps 유지한 관측이나 내부 실제 적용경로는 미확정.
- Luckfox 엔지니어 공식 포럼 t=2375(2025-12-22)는 단일 sub_scene IQ에서 invalid main scene len!이 나타나도 실제 사용에 영향 없다고 설명. 이번 경고만을 근거로 IQ호환성 문제로 판단하지 않음. 다음은 VM SDK내 SensorHw.cpp 및 rk_aiq_user_api2_ae.cpp 등 AIQ소스 존재 확인 후 FPS→VBLANK 경로 검토. https://forums.luckfox.com/viewtopic.php?t=2375 . 기존 IQ/커널 변경 없음.

- 사용자 -a/tmp/ 실행: init success의 IQ경로 /tmp//imx415_CMK-OT2022-PX1_IR0147-36IRC-8M-F20.json 확인, VBLANK808, venc_dequeue_fps19.93~20.11 및 aiq_sof_fps19.80~20.31 유지. requested_isp_fps0은 -F 생략 표시이며 IQ자동정책을 증명하지 않음. invalid main scene len!이 초기화에 출력됨(원본IQ 실행에도 있던 경고). 경로선택 성공과 수정항목 실제 적용은 구분; 보드파일의 isFpsFix/해시 및 대응 .bin 유무 확인이 다음 단계. 원본 diagnostics/2026-10-01/iq-tmp-fps20.log. IQ호환성 오류가 FPS원인이라는 결론은 아직 없음.

- -a/tmp/iq-fps15-test 수정 후 경로 적용 확인. AIQ가 해당 경로 IMX415 .json/.bin access failed로 초기화 실패, g_aiq_ctx NULL로 스트리밍 시작 전 종료. 따라서 IQ15 실측 아직 없음. 파일 누락/중첩 전송/임시경로 소실 여부는 미확정. PC시험JSON 존재 확인 후 보드 디렉터리 생성 및 JSON 단일 파일을 정확한 전체목적경로로 재전송하도록 안내. 센서 인식 하드웨어 고장으로 단정하지 않음.

- IQ15 첫 시험 정정: 사용자 명령 -a /tmp/iq-fps15-test였으나 로그의 ISP IQ file path 및 init success 모두 /etc/iqfiles 원본을 가리킴. 실제 수정IQ 시험은 성립하지 않음. VBLANK808/약20fps를 수정IQ 효과 없음의 증거로 사용하지 않는다. 원인은 진단 소스 getopt optstr의 a::(선택 인수)이며 앞서 공백을 넣어 안내한 오류. 기존 바이너리에서 -a/tmp/iq-fps15-test로 붙여 실행해야 함. 시험 README 수정, 원본 로그 diagnostics/2026-10-01/iq-fps15-path-not-applied.log 보관. 소스/보드IQ 변경 없음.

- 다음 분리시험용 IQ 사본 준비: packages/current-source/iq-fps15-test/. 기존 백업 IMX415 JSON에서 normal/day/scene_isp32/ae_calib/CommCtrl/AecFrameRateMode/isFpsFix 0→1만 변경(한 문자 차이); 기존 FpsValue15 유지. JSON 파싱 확인. 보드 /tmp/iq-fps15-test로 별도 전송 후 -a 해당경로, -F 생략으로 초기 IQ 설정과 런타임 API 요청을 분리 비교 예정. 원본 /etc/iqfiles 및 드라이버 변경 없음, 보드 실행/효과 미검증. 종료 후 -a/-F 생략하여 기준 정책 재시작 및 실측확인.

- 사용자 정지 상태 대조: ps -ef에서 rkipc/simple_vi_bind_venc_rtsp_fps/cctv_person_npu 등 알려진 카메라 앱 없음. VBLANK1808 set_exit=0, 조회1808 성공. 이전 실행 중 set후808과 대비되어 카메라 실행 경로의 재설정 가능성이 높아짐. AIQ 또는 드라이버 중 실제 쓰기 주체는 아직 미확정; 정지 중 조회는 컨트롤 캐시일 수 있어 센서 레지스터 적용 증명은 아님. 마지막 확인값1808이며808 복원 출력 미수신. 원본 diagnostics/2026-10-01/vblank-stream-stopped.log.

- 사용자 실행 중 VBLANK1808 set 명령 후 오류 출력 없이 즉시 get은808 반환. 1808 유지 확인 실패. set 종료코드/중간 센서값은 미확보라 자동제어 재설정인지 드라이버 적용 문제인지 미확정. 이 상태에서15fps 센서 주기 시험이 성립했다고 취급하지 않음. 다음은 카메라 앱 정상종료 후 다른 카메라 프로세스 유무 확인, 정지 상태에서1808 설정/종료코드/조회 후808 복원하여 대조(정지 상태 get은 센서 레지스터 실측 아님).

- revision2 -F15 실행 중 사용자 센서 상태:3864x2192 SGBRG10, VBLANK808(범위58~30575), exposure2250(max2992), HBLANK4936, link_frequency446MHz, pixel_rate356800000. VTS는 드라이버 계산상2192+808=3000줄. /proc/rkisp-unite0: 입력3840x2160@20fps, IRQ3808/ErrCnt0, ISP core198MHz/aclk339428572/hclk148.5MHz. Input @fps는 소스상 sensor->fi.interval 기반 설정 표시이며 독립 실측 아님. 누락된 Output 통계는 추정하지 않는다.
- 다음 제안: 동일 스트림 실행 중 VBLANK를808→1808(VTS4000)로 임시 변경하고 MEASURED 및 VBLANK 유지 여부 확인 후808 복원. 줄 주기가 그대로이고 현재20fps가 센서 주기라면 약15fps 예상하는 조건부 시험. AE가 재설정할 수 있으며 아직 실행 결과 없음.30fps 가능 여부/원인 확정 아님.

- revision2 보드 실행 확인: -F15의 running_before_reapply/after_reapply/delayed 모두 fixed=1,value=15.00, 재적용 set_ret=0. 실제 aiq_sof_fps20.60/20.11/20.09/19.88 및 venc_dequeue_fps20.20/20.11/20.09/20.08 유지. 설정 덮어쓰기/시작 전 적용 시점만으로 설명하는 가설은 약화됨. getter의 설정 저장은 센서 타이밍 적용 증명이 아님. 이 결과로 revision2 보드 미검증 기록 갱신.
- 로컬 제공 IMX415 드라이버 확인: V4L2_CID_VBLANK에서 height+ctrl값으로 VTS 레지스터0x3024~0x3026 기록. g_frame_interval은 현재 실측 대신 mode->max_fps 반환. 다음은 -F15 실행 중 보드의 센서 format/controls 및 /proc/rkisp* 상태 확보, 필요 시 기본 실행과 VBLANK 비교. 원인은 아직 AE→센서 전달/센서 타이밍/ISP 경로 중 미확정이며 커널 변경 없음.

- 후속 -F15 사용자 실측3회: aiq_sof_fps20.61/20.11/20.09, venc_dequeue_fps20.21/20.11/20.09. 하향 요청도 약20fps 유지. 보드 성능 상한20fps의 증거가 아니라 현재 AE 요청이 실측에 반영되지 않는 상태로 판단. 설정 적용 시점/덮어쓰기/AE 동작 경로 등 원인은 미확정.
- 진단 소스 revision2 준비: VI 시작 전 및 실행 중 AE fixed/value readback, 첫 SOF 관측 약2초 후 메인 스레드에서 동일 FPS 요청 한 번 재적용, 이후5초 간격 readback. 인코더/VI 속도 및 센서 모드 유지하여 시점 변경 비교. 기존 VI0/0 경고도 이번 대조에서는 유지. packages/current-source/isp_fps_test_source_v2.zip 제공; ARM 빌드/보드 동작 미검증. 기존 ZIP은 revision1이며 새 버전은 별도 파일.

- 사용자 보드 실행 로그로 simple_vi_bind_venc_rtsp_fps 빌드/배포 및 실행 확인. 1920x1080/H264/4096Kbps 기본 실행의 5초 집계3회: aiq_sof_fps19.60~20.10, venc_dequeue_fps19.80~19.91. -F30은 AIQ 요청 accepted를 출력했으나 aiq_sof_fps19.63~20.24, venc_dequeue_fps19.82~19.85로 약20fps 유지. 앞선 준비 단계의 보드 미검증 기록을 갱신한다.
- 관측: 두 조건 모두 센서 raw3864x2192, VI 프레임레이트0/0 illegal param 경고 후 스트리밍 진행. 이번 코드의 인코더 FPS 필드는 변경하지 않았다. AIQ 콜백과 인코더 취득 모두 약20fps이므로 이번30fps 요청으로 실제 출력 증가가 확인되지 않음. VLC 표시 FPS/중복 프레임 여부 또는 보드 절대 최대 FPS를 증명하는 결과는 아님.
- 추정 및 다음 비교: AIQ 요청이 스트림 시작 후 덮어써지는지, 센서/ISP 모드에서 제한되는지 아직 미확정. 동일 바이너리 -F15로 하향 요청의 효과를 먼저 비교할 예정(미실행). VI0/0 경고가20fps 원인이라는 근거는 없음.
- 원본 로그 보관: diagnostics/2026-10-01/isp-fps-baseline-vs30.log. 이번 기록 작업에서 보드/VM 설정이나 프로그램 소스는 변경하지 않음.

## 작업공간 정리 — 2026-10-01

- 사용자 요청으로 루트 압축파일7개를 용도별 이동. 삭제 및 압축 내용 변경 없음, 이동 전후 SHA256 전부 일치. 루트 README.md에 폴더 안내 및 각 파일 상태 기록. 이동 전후 절대 경로/크기/해시는 docs/organization-2026-10-01/moves.json.
- 현재 소스 스냅샷: packages/current-source/cctv_npu_single_vi_source_fixed.zip 및 isp_fps_test_source.zip. 이전 -D 소스: packages/archive/cctv_camera_npu_diag_complete.zip.
- 드라이버 수신 압축: references/driver-source/camera_isp_source_review.tar.gz 및 rknpu_source_review.tar.gz. 부팅 이미지 검토 압축: diagnostics/2026-09-30/incoming/npu420-build-check.tar.gz 및 npu420-isp-preserved.tar.gz. 검증/보드 적용 완료를 의미하는 이동이 아님.
- 소스 폴더 cctv_npu_output_fix_complete/cctv_person_npu, isp_fps_test, 제조사 example/simple_test/luckfox_pico_rtsp_yolov5, IMAGES, backups, 기존 diagnostics, tmp, profile-webapp은 기존 경로 유지. 현재 작업 소스와 ZIP은 자동 동기화되지 않음. 과거 기록/채팅의 루트 압축 경로는 이 절과 README의 새 경로로 대응한다. 보드/VM 파일은 변경하지 않음.

## 스트리밍 FPS 변경 체감 보고 — 2026-09-30

- 사용자 전달 대안: 추가 디버깅 후1080p20+540p20 이중 스트림 검토, 1080p만 제공하면 클라이언트 디코딩 시 끊길 수 있다는 담당자 의견. 실제 디코더 병목 확인 결과가 아니며 클라이언트 사양/동시 재생 수/코덱/가속 여부 미확인. 저해상도 선택지는 합리적이나20fps 확정이나 이중 인코딩 안정성 검증 완료로 취급하지 않는다. 로컬 파일 재생 대조와 수신 통계로 네트워크/디코딩/표시 병목 구분 필요.

- 사용자 요청으로 ISP/AIQ FPS 변경 비교용 별도 샘플 isp_fps_test/simple_vi_bind_venc_rtsp_fps.c 준비. SDK example/common/isp3.x/sample_comm_isp.c의 AE get/setExpSwAttr 패턴으로 -F1..30 고정 FPS 요청 추가. 인코더 FPS 필드는 변경하지 않으며 옵션 생략 시 AIQ 정책 유지. 5초마다 MEASURED aiq_sof_fps(콜백 수), venc_dequeue_fps(인코더 프레임 취득 수) 출력. 중복 프레임/실제 센서 주기/VLC 표시 FPS 증명은 아니며 송신 지연 영향 가능. 오류 반환 검사, 종료 대기 단축 포함.
- isp_fps_test_source.zip에 build.sh/README/source3개 LF 개행 및 CRC 확인. 기존 샘플과 NPU 소스 변경 없음. ARM 헤더/컴파일러가 로컬에 없어 빌드와 보드 실측은 미검증. VM /home/student/isp_fps_test에서 build.sh 실행, 별도 실행파일로 기존 정책→-F30을 동일 환경 비교 예정. AIQ 고정 요청은 센서 모드나 ISP 성능 한계 초과를 보장하지 않음.

- 최신 사용자 전달 관측: 해상도 변경은 되지만20→30fps 설정 후에도 움직임이 느리게 보이며 ISP단20fps/인코더30fps 표시 확인했다고 함. 원본 실행 명령/코드/상태 출력 미수신이라 ISP값이 Input 설정값인지 Output rate 실측인지 미확정. 인코더 프레임 중복 또는 FPS 메타데이터만 변경했는지도 미확정. ISP 실측이20fps라면 인코더 설정30만으로 새로운 장면30장/s를 얻을 수 없으며 센서/AIQ의 실제 프레임 주기 및 노출, VI/ISP 제한 설정을 먼저 조사해야 함. 원본 ISP Input/Output rate 및 인코더 FPS 변경 코드 확보가 다음 단계.

- 공식 제품 상세 추가: ISP8M@15fps 및3840x2160 입력15FPS 명시, Video Encoding5M@30FPS. 위키는 ISP5M@30fps로 다른 해상도 조건 표기. 따라서 사양을 단일 최대 FPS로 환산하지 않고 8M입력15/5M입력30/5M인코딩30을 출처별 구분한다. 기존 3864x2192 raw 경로 약20fps 관측이 있어 공식8M15 값을 현 펌웨어의 절대 상한으로 적용하지 않는다.

- 공식 Pico Zero 제품/위키 재확인: RV1106G3, ISP Max input5M@30fps, 제품 페이지 Video Encoding5M@30FPS. 이는 조건부 해상도별 처리 사양이며 모든 해상도에서 절대30fps 한계 또는 IMX415/Wi-Fi/NPU 동시30fps 보장을 뜻하지 않는다. 출처 https://wiki.luckfox.com/Luckfox-Pico-Zero/ 및 https://www.luckfox.com/Luckfox-Pico-Zero . 기존 통합 프로그램 약20fps 실측과 사양 구분, simple_vi_bind_venc_rtsp의 실제 FPS는 별도 미측정.

- 사용자가 전달한 테스트 담당자 보고: 프레임/해상도를 바꿔 송출 중 표시 수치는 바뀌지만 실제 프레임 증가로는 보이지 않음. 현재 실행 프로그램·명령·변경 코드·설정 전후 값·수신 경로는 미확인. 실제 FPS 증가/메타데이터만 변경/수신 병목 중 어느 것인지 미확정.
- 로컬 simple_vi_bind_venc_rtsp는 -w/-h를 VI/VENC 크기에 적용하며 FPS CLI는 없음. cctv_person_npu의 -f는 AI 샘플링 상한. 이전 통합 프로그램 VIDEO 약20fps는 과거 특정 실행 관측이며 이번 실행 측정값으로 취급하지 않는다. 다음은 실행 명령 및 FPS 변경 위치 확인 후 송신 프레임 수/시간과 수신 표시 프레임 수/시간 비교.

## NPU 진단 재개 — 2026-09-30

- 후속 사용자 grep: Pico Zero DTS는 rv1106.dtsi 및 rv1106-luckfox-pico-zero-ipc.dtsi 포함. rv1106*.dts* 전체 rockchip,unite 검색 결과는 다른 보드 rv1106g-evb1-v11-4k.dts의 rockchip,unite-en만 존재. 원래 실행 보드의 /rkisp@ffa00000 rockchip,unite=<1>와 다른 속성이므로 대체하지 않는다. 다음 사용자 수동 수정은 최상위 Pico Zero DTS 끝에 &{/rkisp@ffa00000} { rockchip,unite = <1>; };로 기존 보드 속성을 명시적으로 보존한 뒤 kernel 재빌드, 새 FIT와 기존 백업의 DTB 차이가 NPU 클록 하나뿐인지 재검증. 수정/재빌드 결과는 아직 미수신.

- npu420-build-check.tar.gz 수신 및 diagnostics/2026-09-30/npu420-check에 검증용 추출. FIT 직접 파싱 및 FDT/kernel/resource 각 SHA256 내부 해시 확인 모두 통과(서명 검증 의미 아님). 새boot.img 3860992bytes, SHA25613e80ac530050fec87435cae593d316f971900cadee9c536ae0b1c79755682d7, 기존32MiB boot 파티션보다 작음. 별도 DTB와 FIT 내부 DTB 속성 일치. 커널 payload SHA256cd737a73bd24bd6e42fb9ffcd6ca6db94533c22d13f74ee8b264e020867a014c로 기존 보드 백업과 완전히 동일.
- 중요: 기존 보드 백업 DTB와 새 DTB의 속성 차이는2개: /npu@ff660000 assigned-clock-rates700000000→420000000 및 /rkisp@ffa00000 rockchip,unite=<1> 속성 삭제. VM의 .before-npu420와의 한 줄 diff가 보드 실행 설정과 한 줄 차이를 보장하지 않았음. 새이미지는 NPU만 변경한 대조군이 아니므로 보드 적용 보류. 실제 소스/포함 DTSI의 unite 설정 및 기존 변경 이력 확인 필요. 삭제의 실제 동작 영향/원인은 아직 미확정. verification.json에 상세 검증 저장. 보드 쓰기·재부팅은 수행하지 않음.

- 사용자 Ubuntu kernel 빌드 성공 확인: 종료코드0 및 Running build_kernel succeeded. DTB 재생성, 커널/FDT/resource 포함 FIT boot.img 생성, sysdrv/source/objs_kernel/boot.img → sysdrv/out/image_uclibc_rv1106/boot.img → output/image 설치 로그 확인. 전체 firmware 빌드 없이 boot.img가 생성됨. DTB와 boot.img를 Windows에서 검증할 예정이며 FIT 내부420MHz/파일크기/기존 이미지와 호환성 및 복구 절차는 아직 미검증. 보드 플래시/재부팅/420MHz 실측은 아직 수행하지 않음.

- 사용자 VM 확인: .BoardConfig.mk → project/cfg/BoardConfig_IPC/BoardConfig-EMMC-Buildroot-RV1106_Luckfox_Pico_Zero-IPC.mk. build.sh 도움말 kernel(-build kernel), firmware(-pack all the image we need to boot up system) 및 build_kernel 함수 확인. 다음 사용자 수동 단계는 bash build.sh kernel 및 로그 확보. 전체 all/clean/firmware나 보드 쓰기는 아직 지시·수행하지 않음. 커널 빌드 성공 및 생성 이미지 경로/포장 단계는 후속 출력으로 확인해야 함.

- 최신 사용자 VM diff 확인: rv1106g-luckfox-pico-zero.dts.before-npu420 원본과 현재 .dts의 차이는 &npu assigned-clock-rates 700000000→420000000 한 줄뿐. 원본 백업 존재 및 해당 원본의700MHz 값 확인. 공유 &cru 변경 없음. Ubuntu 소스 수정 완료이며 빌드/이미지 생성/보드 적용 완료는 아님. 다음은 활성 .BoardConfig.mk와 SDK build.sh의 빌드 옵션 확인; 보드 마지막 실측은700MHz 유지. 앞선 Ubuntu 백업 미확인 기록은 이 diff로 갱신됨.

- 사용자 복구용 저장 요청에 따라 backups/pre-npu-clock-20260930 생성. 보드 boot(/dev/mmcblk0p4,33554432bytes), env(p1), idblock(p2), uboot(p3)를 ADB로 읽어 PC에 백업; 4개 모두 보드 원본과 PC SHA256 일치 확인. 실행 중 파일시스템 전체/eMMC 전체 백업은 아니며 부팅 실패 시 실제 복구 도구/절차는 아직 검증하지 않음.
- 보드 cctv_person_npu_demo 전체(모델/실행 파일/테스트 이미지 포함), IQ 파일, 보드 상태 기록 백업. 확인 당시 NPU700MHz, 센서3864x2192/10bit 유지. 펌웨어 쓰기나420MHz 적용 없음. Ubuntu 소스 수정 여부는 사용자 확인 전까지 미확인; .before-npu420 백업 존재/내용도 아직 확인되지 않음. 이후 이미지 적용 전 Ubuntu 원본과 복구 수단 확인이 필요함.

- 사용자 VM 소스 조회 확인: rv1106g-luckfox-pico-zero.dts 39~54줄 &cru 활성화, PLL_CPLL 및 CLK_500M_SRC에700MHz 지정; 57~59줄 &npu assigned-clock-rates=<700000000>. 다음 소스 비교 변경은 &npu의700000000만420000000으로 바꾸고 공유 &cru는 유지. 아직 사용자 수정·빌드·보드 적용은 미확인. 활성 .BoardConfig.mk 및 SDK build.sh의 kernel/firmware 옵션 확인 필요.

- 사용자 작업 분담 확정: Ubuntu VM 작업은 사용자가 수동 수행. SSH 주소를 요청하거나 VM 직접 접속을 전제로 하지 않는다. Windows 소스 준비/ADB 보드 진단은 Codex가 수행하고 필요한 VM 명령과 결과 항목을 제공한다.
- 새로운 비교 근거: Luckfox 공식 포럼 https://forums.luckfox.com/viewtopic.php?t=2452 의 Pico Zero+IMX415 유사 오류에서 엔지니어가 NPU 클록 하향을 제안, 원작성자가420MHz로 동작했다고 보고. 외부 사례이며 현재 보드 해결 확인/원인 확정은 아님. 별도 GitHub 이슈544의 AXI contention 주장은 사용자 가설로 취급.
- 직접 보드 확인: /proc/rknpu/freq=700000000. 임시 debugfs 마운트로 aclk_npu_root=700MHz, parent clk_500m_src=700MHz 확인. 가능한 부모 clk_500m_src/clk_300m_src/clk_pvtpll_0/clk_pvtpll_1, pvtpll_1은420MHz. clk_rate/clk_parent 디버그 노드는 읽기 전용이고 devfreq 항목 없음. /proc/rknpu/freq에420000000 임시 설정 시도는 Bad address/exit1로 실패했고 이후700000000 유지 확인; 시험 후 debugfs를 언마운트함. 현재 SDK 소스의 &npu/&cru 확인 후 NPU만420MHz로 낮춘 비교 빌드를 사용자가 수행하는 방향. 공유 CRU/전압/부팅 이미지는 아직 변경하지 않음.

- 후속 메모리 분리 시험: 카메라/NPU 종료 시 전용 CMA 사용12KiB. -S 실행 중 CmaAllocated74712KiB / 총102400KiB, NPU 할당 합17340KiB(14028+12+1200+1600+400+100), 카메라 CIF raw 버퍼10260KiB×4 확인. 해당 실행은 NPU ready 이후 timeout으로 종료하므로 초기 CMA 할당 실패와 구분한다.
- 기본 rkipc만 실행 시 전용 CMA85180KiB 사용. 여기에 관측된 NPU17340KiB를 더하면102520KiB로 총량102400KiB를120KiB 초과한다. 이전 rkipc+사진의409600byte 할당 실패는 이 용량 부족과 일치한다. 부팅 CMA를 임의 변경하지 않았다.
- 제조사 simple_vi_bind_venc_rtsp를 출력640x360/1024Kbps로 실행하고 별도 cctv_person_image로 저장된 사진 추론: NPU ready 성공, 총CMA75480KiB 상태에서도 op2 /model.0/conv/conv/Conv, counter13, 약6초 timeout 재현. 센서 원본은3864x2192 유지. 자체 카메라 RGB 변환/실시간 입력 코드 없이도 재현되며 해당 코드만의 문제로 설명할 수 없음. 출력 축소를 최종 영상 품질 설정으로 적용한 것은 아니다.
- gdb에서 사진 프로그램 rknn_run 직전에 중단(모델/NPU 메모리 유지)→제조사 카메라640x360 시작→약3초 후 정상 종료→동일 모델 추론 재개 시험 성공(PERSON count24, 정상 종료). detect_ms4908.4는 디버거 중단과 카메라 시험 시간이 포함되어 추론 성능 수치로 사용하지 않는다. 이 한 번의 시험이 모든 메모리 손상 가능성을 배제하지 않으나, 카메라 종료 후 동일 NPU 컨텍스트에서 추론 가능함을 확인.
- 후속 로그는 diagnostics/2026-09-30/memory-*.log 및 camera-cycle-gdb.log, dmesg-memory.txt. 종료 후 rkipc/simple_vi_bind_venc_rtsp/cctv_person_npu 미실행 확인. 다음 진단용 재빌드를 위해 사용자에게 기존 Ubuntu VM SSH 주소 요청; 아직 접속 정보 미수신. 현재 결과만으로 ISP/NPU 드라이버·런타임·공유 하드웨어 중 정확한 원인을 확정하지 않음.

- Windows ADB로 연결된 보드에 직접 명령 실행 가능 확인. Linux 5.10.160, IMX415 및 기존 /oem/usr/bin/cctv_person_npu_demo 실행 파일 확인. 부팅 중 rkipc 실행 상태였으며 TERM 후 카메라 해제 및 종료 확인.
- 센서 active 1944x1097/0x3010(12bit) 설정 및 G_FMT 확인 성공. -S 시작 시 커널 s_stream도 1944x1097 HDR0 bpp12 유지. 앱 sensor raw는 1944x1096, ISP 약339MHz이며 force to 2 readback 경고는 유지됨.
- 작은 모드 -S는 PERSON 출력 전에 비정상 종료. 반복 실행 exit139 확인. 보드 gdb로 Thread xc:isp_3a_stats의 SIGSEGV 및 /oem/usr/lib/librkaiq.so 내부 backtrace 확보. 충돌 위치는 확인했으나 라이브러리 심볼이 없어 구체적 원인은 미확정. 작은 모드에서 NPU 성공 여부를 판단할 수 없음.
- 기존 3864x2192/0x300e(10bit)로 복원한 -S는 exit1, rknn_run -1 재현. 이번 op2 /model.0/conv/conv/Conv, task counter12로 과거 op3/counter19와 구분.
- 카메라 종료 후 사진 단독 추론 exit0 재확인. 기본 rkipc와 사진 추론 동시 시험은 NPU output allocation failed, 커널 CMA 100pages(409600bytes) 할당 실패/-12 확인. 이는 초기 할당 실패이며 -S의 추론 timeout과 같은 원인으로 단정하지 않는다. 최초 별도 ADB 셸 background 기동 시도는 rkipc 실행 확인이 안 되어 동시 시험으로 계산하지 않음; 같은 셸에서 PID 생존 확인 후 수행한 시험만 유효.
- 시험 후 센서 기존3864x2192/10bit 복원, 진단 앱 종료. 로그는 diagnostics/2026-09-30/에 보관. 다음은 작은 모드 AIQ 충돌의 심볼/호환성 확인 또는 기존 모드에서 카메라·NPU 동시 실행의 메모리 사용/드라이버 경로 분리이며, 센서 해상도 축소만으로 해결됐다고 판단하지 않음. Wi-Fi/VLC 사람 감지 동시 검증은 아직 미완료.

## 저장 공간 및 RAM 확인 — 2026-09-28

- 사용자 제공 df -h: / 전체5.8G, 사용248.7M, 가용5.4G(4% 사용); /userdata 전체237.9M, 가용169.2M(26% 사용); /oem 전체487.6M, 가용440.0M(7% 사용). 현재 각 저장 파티션에 여유 있음. 루트의 여유 공간을 별도 /userdata·/oem 파티션의 여유로 간주하지 않는다.
- 사용자 제공 free -m: RAM total145MB, used22MB, free79MB, buff/cache43MB, available116MB, swap0. 현재 가용 RAM은 충분한 편이나 측정 당시 카메라/NPU 실행 여부는 미확인. 실행 중 메모리 부족 여부나 기존 NPU timeout 원인은 이 결과만으로 판정하지 않는다. /tmp·/run·/dev/shm은 tmpfs로 RAM을 사용한다.

## NPU 분리 시험 시작 실패 — 2026-09-23

- 보드 TRY_FMT pad0 width1944 height1097 code0x3010 반환 일치(SGBRG12_1X12) 확인. 현재 실행 드라이버에서 작은12bit 모드 best-fit 선택 가능하며 실제 active 설정/스트리밍/NPU 성공은 미검증. 다음 비교는 카메라 앱 종료 상태에서 active1944x1097/12bit 설정→G_FMT 확인→-S 실행, 시작 sensor raw 및 커널 s_stream으로 유지 여부 확인. AIQ prepare(0,0)가 다시 모드를 선택할 수 있어 변경 유지 보장하지 않음. 실패 시 기존3864x2192/0x300e로 복원 가능.

- 보드 code0x3010 ENUM_FRAME_SIZE는 ioctl 제목만 출력, 크기 없음. 제공 imx415_enum_frame_sizes는 전역 index의 bus_fmt 불일치 시 EINVAL 반환하여 첫10bit 모드에서12bit 열거가 중단될 수 있음. 12bit 미지원으로 판단하지 않는다. 다음은 --try-subdev-fmt pad0/code0x3010/1944x1097로 드라이버 best-fit 반환 확인; TRY 분기는 active cur_mode 변경 없이 try-format만 저장함을 소스에서 확인. 성공도 실제 센서/AIQ 스트리밍 검증은 아님.

- camera_isp_source_review.tar.gz 수신 및 tmp/camera_isp_review에 검토용 추출. IMX415 소스 4lane supported_modes의 10bit는3864x2192이나 12bit에는1944x1097 30fps NO_HDR/HDR_X2 존재(1453줄 이후); crop selection은1920x1080 지원. 2lane 표는 별도로1284x720 12bit 모드 존재. 실제 보드 배선/소스와 실행 드라이버 동일성 미확인이므로 적용 가능성은 보드12bit ENUM_FRAME_SIZE로 확인해야 함. 이전10bit 조회만으로 센서 모든 모드에 작은 해상도 없다고 일반화하지 않는다.
- isp_params_v32.c:4772 인근에서 hw->unite==ISP_UNITE_ONE이면 is_multi_overflow를 true로 설정하며 해당 경고를 출력. 따라서 경고 문자열만으로 현재 입력 해상도 초과/자원 부족이 timeout 원인이라고 판단할 수 없음. ISP 모드 강제 우회 패치는 하지 않는다. 다음 보드 읽기 전용 조회: 0x3010 SGBRG12 크기 및1944x1097 프레임 간격. AIQ 시작 시 모드 재선택 가능성이 있어 수동 set-fmt만으로 변경 유지 보장하지 않음.

- 최신 power/load 모니터링: uptime2069~2100초 off/0%, 2101 on/0%, 2102 on/7%, 2103~2108 on/99~100%, 2109~2110 on/0%, 2111 이후 off/0%. 작업 중 power 참조가 유지됨을 확인; NPU 미기동/조기 전원 참조 해제 가설은 지지되지 않음. 실제 전압/클록 안정성은 이 표시로 검증 불가. load는 busy시간 기반이므로 연산 진행·성공 증거가 아님. 이번 실행 앱/dmesg와 시간대 대조는 미수행, 기존 약6초 timeout 패턴과 부합. 다음은 IMX415 및 ISP 드라이버의 full-resolution/readback 선택 경로 확인이며 센서 저해상도 모드 존재를 가정하지 않는다.

- 최신 보드 조회: /proc/rknpu/version v0.9.2, freq700000000, power off(카메라/NPU 앱 미실행 상태). /sys/class/devfreq 루프 출력 없음; 노출된 devfreq 디렉터리 미확인이지 주파수 제어 고장 의미 아님. debugger 소스에서 power는 power_refcount 기준이며 freq 읽기는 power_get→clk_get_rate→power_put 수행. 주파수 읽기도 일시 전원 참조를 건드리므로 반복 모니터링에서는 제외한다. 다음은 -S 실행 중 별도 ADB 셸에서 power/load만 초당 읽어 비교. load는 작업 busy 시간 기반 통계이지 정상 연산 진행의 증거가 아님.

- rknpu_source_review.tar.gz를 작업공간에서 읽어 tmp/rknpu_review/rknpu에 검토용 추출. 헤더 버전0.9.2로 보드 표시와 같지만 동일 바이너리 빌드는 미확인. job.c에서 require mask는 마지막 task의 int_mask에서 오며 IRQ STATUS를 fuzz 처리 후 비교하여 완료 판정. raw0xc0000은 비트18/19이나 의미 이름/하드웨어 오류 정의는 제공 소스에 없어 특정 원인으로 해석 불가. timeout은 submit args에서 전달받아 사용, 만료 시 레지스터 출력 후 soft reset. task counter19는 하드웨어 작업 상태 레지스터의 마스크 값이지 성공 프레임 수가 아님.
- rv1106 설정은 단일 IRQ, 32비트 DMA mask. bw_priority_addr/length가0이며 대역폭 우선순위 get/set은 base가 없으면 EINVAL을 반환하므로 이 API로 우선순위를 바꾸는 해결책은 현재 소스로 뒷받침되지 않음. 즉시 적용할 드라이버 수정 근거는 발견하지 못함. 다음은 보드가 노출하는 rknpu debug/proc 및 devfreq의 버전/주파수/정책을 읽어 동작 설정 조사. 임의 timeout 증가나 모듈 교체는 하지 않음.

- 사용자 VM find 결과: /home/student/luckfox-live/luckfox-pico/sysdrv/source/kernel/drivers/rknpu 에 rknpu_job.c/rknpu_drv.c 및 include 헤더 등 소스 존재 확인. sysdrv/out/kernel_drv_ko/rknpu.ko와 objs_kernel의 5.10.160 모듈도 존재하지만 실행 중 보드 드라이버와 동일 빌드인지 미확인. 다음은 해당 소스 디렉터리를 묶어 전달받아 상태 레지스터/timeout/플랫폼 설정 검토. 모듈 교체나 재빌드 지시는 아직 하지 않는다.

- 최신 -S 보드 시험(ce1b05f1 / 2d6956ac): READY DIAGNOSTIC SINGLE, AI VI1 640x360만 초기화 성공. main VI/VENC/RTSP 없는 상태에서도 op3 /model.1/conv/Conv task counter19에서 rknn_run -1, PERSON 없음. 커널 1395초 구간에서 약6.01초 대기 후 timeout/soft reset, irq0 raw0xc0000 require0x300. 앞의 636초 timeout은 이전 -D 이력이며 혼동하지 않는다. raw0xc0000 의미는 드라이버 정의 확인 전 추정하지 않는다.
- 센서3864x2192 HDR0 10bit 및 force to 2 readback/ISP420MHz 유지. 채널 수 축소만으로 해결되지 않았으며 sensor/ISP/NPU 동시 경로와 드라이버/메모리 접근 후보를 조사한다. -S ARM 빌드/배포 및 채널 시작은 사용자 로그로 확인, 추론 성공은 아님. 다음은 VM SDK의 rknpu 드라이버 파일 위치 확인 후 상태 비트와 동시 실행 관련 코드 대조. 펌웨어/클록/센서 모드 임의 변경은 하지 않는다.

- 배포 ZIP 오류 확인 및 수정: 사용자 VM 폴더는 올바른 /home/student/cctv_person_npu이며 build.sh 자체가 없음. 이전 ZIP 생성 필터가 build로 시작하는 파일명까지 제외해 build.sh를 누락한 것이 원인(사용자 압축 해제 오류 아님). 디렉터리만 제외하도록 수정하여 cctv_npu_single_vi_source_fixed.zip 생성, build.sh/CMakeLists/src/model 포함 및 ZIP CRC 확인. 이전 ZIP 대신 수정 ZIP 사용. VM 빌드는 아직 미완료.

- 최신 배포본 카메라 종료 상태 이미지 재시험 성공: PERSON count24, image612x368, detect_ms100.3. 현재 모델/실행 파일의 단독 추론 성공 확인. -D 동시 실행 실패와 구분한다.
- 다음 분리 시험용 `-S` 추가: AI용 VI 채널 기본1(640x360)만 생성, main VI 및 drain 스레드/VENC/RTSP 생략. 기존 -D 및 일반 실행 경로 유지. 센서 입력/ISP 설정/모델 변경 없음. 소스 분기 및 정리 경로 검토, 배포 ZIP 검증만 수행하며 ARM 빌드·보드 단일 채널 지원·추론 성공은 미검증. `cctv_npu_single_vi_source.zip` 전체 소스를 VM에 반영해 빌드 후 cctv_person_npu만 교체하여 `-S -f 5 -t 0.5` 실행 예정.

- 최신 97921fb4 커널 로그: -D 실패와 일치하는 task counter19(0x13), 약 6.56초 대기, timeout 6000000us, irq/raw status0, job timeout 후 soft reset 확인. 센서는 3864x2192 HDR0 10bit, ISP clk420MHz, `over virtual isp max resolution, force to 2 readback` 관측. readback과 timeout의 인과관계는 미확정.
- 종료 후 ps에는 rkipc/simple_vi_bind_venc_rtsp/cctv_person_npu가 보이지 않음. [venc], [rkisp-unite0]는 커널 스레드다. meminfo는 MemAvailable118800kB, CmaTotal102400kB, CmaAllocated12kB, CmaReleased102388kB, CmaFree0. 종료 후 상태이며 실행 중 부족을 배제하지 않고 CmaFree0 단독으로 CMA 고갈이라고 판단하지 않는다. 이번 제공 커널 구간에는 새 할당 실패/OOM 기록이 없다.

- 최신 f4f9e248 첨부: `-D -f 5 -t 0.5`에서 초기화 및 CAMERA main_fps=20.3 확인 후 RKNN failed to submit (op id3 /model.1/conv/Conv, task counter19, int status0), `NPU ERROR: rknn_run: -1`로 종료. PERSON 출력 없음. VENC/RTSP를 생성하지 않는 조건에서도 추론 실행 실패가 재현되어 해당 앱의 인코딩/RTSP 경로만으로 설명할 수 없다. 센서/ISP/VI와 NPU 동시 실행 조건의 문제는 계속 조사하며 정확한 원인이나 커널 timeout은 이번 앱 로그만으로 확정하지 않는다. 다음은 종료 직후 dmesg tail100, ps -ef, meminfo 확보. 종료 과정의 accept failed를 최초 원인으로 해석하지 않는다.

- 후속 첨부 a1809e7c 로그에서는 3개 출력 할당 및 NPU ready 성공, IMX415 AIQ init/prepare/start와 mainpath/selfpath stream on 성공, `READY DIAGNOSTIC: camera + NPU; two VI channels; VENC/RTSP disabled; AI <= 5 fps` 확인. 이번 실행의 초기 메모리 할당 실패는 재현되지 않았으나 회복 원인은 미확인. 로그 끝에 AF motor wait 경고가 있으며 PERSON 출력·추론 성공·장시간 안정성은 아직 확인되지 않았다. 다음은 실행을 유지해 후속 PERSON 또는 NPU 오류 확인, 정체 시 별도 터미널의 dmesg 확인.

- 사용자 ADB 로그에서 cctv_person_npu와 cctv_person_image 두 실행 파일 전송 성공. 첫 명령의 쉼표로 image만 실패했으나 별도 재전송 성공. 보드 chmod 후 `./cctv_person_npu -D -f 5 -t 0.5` 실행 확인.
- 두 차례 모두 output[1] 속성/크기 출력 후 `rknn_create_mem`의 errno 12 Cannot allocate memory 및 STARTUP ERROR: NPU output allocation failed. NPU ready 및 카메라 초기화 이전 실패로, 기존 rknn_run timeout과 구분한다. VENC 제외 효과는 아직 검증하지 못했다.
- 사이에 사용자가 `killall rkipc` 실행했으나 재시험도 동일. 실제 프로세스 종료 완료·자동 재시작·다른 앱 점유·현재 메모리 상태는 미확인. rkipc 종료만으로 자원 해제가 완료됐다고 가정하지 않는다.
- 다음은 보드 `ps -ef`, `/proc/meminfo`, `dmesg` 마지막 100줄로 현재 프로세스와 메모리 할당 실패 근거 확인. 메모리 풀 부족/단편화/잔류 점유는 후보이며 아직 확정하지 않는다.

## WireGuard 설정 검토 — 2026-09-22

- 최신 사용자 비교 결과: Wi-Fi 직접 RTSP 접속은 정상이라고 확인. VPN/서버 경유에서는 위에서 아래로 느리게 갱신되는 증상이 보고됨. 보드 영상 출력과 VLC 기본 재생이 가능한 대조 결과이며 VPN/서버 경유 경로를 우선 조사한다. 양쪽 비트레이트·TCP 옵션 등 모든 조건의 동일성은 별도 확인되지 않았으므로 특정 서버 프로세스, MTU, 대역폭 또는 보드의 VPN 처리 중 하나로 원인을 확정하지 않는다. 사용자는 서버 접속 권한이 없다고 설명했다.
- 최신 보드 `[root@luckfox] wg show`: wg0, listen 51820, 서버 peer endpoint `54.180.26.70:51820`, AllowedIPs `10.0.0.0/24`, latest handshake 1분 27초 전, 누적 received 11.93 MiB / sent 512.65 MiB, keepalive 25초 확인. 최근 VPN handshake 및 누적 송수신은 확인됐으나 누적 바이트로 현재 속도·패킷 손실·영상 수신 성공을 판단하지 않는다. 서버 SSH 출력은 아직 제공되지 않았다.
- 최신 사용자 설명: 서버는 GCP에서 아마 Linux를 사용 중이라고 함. 사용자 추정이며 OS·클라우드 인스턴스 정보는 미확인. VLC의 실제 실행 위치와 보드→서버→PC 구성, 서버가 단순 VPN 라우터인지 RTSP 중계 프로그램을 실행하는지는 확인 필요. 앞서 제공된 Endpoint와 GCP 인스턴스의 대응도 미검증이다. 다음은 서버 SSH에서 `uname -s`, `wg show`, IPv4 forwarding 상태 및 영상 관련 프로세스를 읽기 전용으로 확인한다.
- 사용자는 영상 사양을 낮추는 것보다 서버 컴퓨터에서 할 수 있는 진단을 요청했다. 이후 진단은 서버 VLC의 수신/디코딩/표시 통계 및 오류 로그, 실제 RTSP 접속 프로세스 확인을 우선한다. 서버 OS와 서버에서의 VLC 직접 재생/중계 구성은 미확인이다. 보드가 관측한 Lavf User-Agent와 서버 주소만으로 특정 중계 또는 NAT 구성을 확정하지 않는다.
- 사용자 화면 증상 확정: '화면이 위에서 아래로 천천히 갱신됨'. 영상은 표시되나 정상적인 전체 화면 갱신이 아닌 상태로 보고됨. 전송 경고와의 연관은 후보이며 센서 스캔 방식이나 네트워크 병목으로 확정하지 않는다. `-b 2048` 적용 결과는 아직 없으므로 동일 TCP 재생 조건에서 낮은 비트레이트 비교를 우선 안내한다.
- 최신 사용자 정정: 서버 쪽에 화면이 나오지만 '스캔 하는 것 마냥' 보인다고 보고했다. 따라서 완전 무영상으로 취급하지 않는다. 화면이 위→아래로 느리게 갱신되는지, 줄/블록으로 깨지는지, 색·밝기 띠가 움직이는지와 서버 측 표시 프로그램은 아직 미확인이다. 직전 `-b 2048` 안내의 실제 적용 여부도 미확인. 구체적 화면 증상을 확인하기 전 대역폭·센서·디코더 중 원인을 확정하지 않는다.
- 후속 로그: `rtsp_tx_video`의 `client 10.0.0.1 will lost video packet` 경고가 다수 반복된 뒤 peer closed / delete client 확인. 송신 처리의 패킷 누락 경고와 네트워크상의 실측 패킷 손실은 구분한다. 송신 정체·수신 측 처리 지연 등은 후보이며 원인은 미확정이다. 현재 실제 실행 비트레이트는 미확인. 다음 비교는 실행 중 샘플 정상 종료 후 `-b 2048`로 전송량을 낮추고 동일 TCP 재생 조건에서 경고/영상 변화를 확인하는 것이다.
- 최신 세션 1234567D: DESCRIBE에 H264 SDP/SPS/PPS 응답, SETUP의 `RTP/AVP/TCP;unicast;interleaved=0-1` 및 서버 TCP 수락, PLAY 200 OK 확인. 이후 `rtp over tcp send interlaced frame to 10.0.0.1 failed: Connection reset by peer`, peer closed 발생. TCP 협상 성공 후 영상 데이터 송신 단계에서 연결 리셋이 관측됐으며, VLC 자체/중계/네트워크 중 리셋 원천은 미확정이다. 이 문구의 interlaced만으로 인터레이스 영상 문제로 해석하지 않는다. 사용자는 VLC 사용을 확인했으나 보드에 보이는 User-Agent는 계속 Lavf62.3.100이다. 다음은 VLC 오류 로그 및 동일 실행 상태의 Wi-Fi 직접 RTSP 재생 비교이며 UDP 포트 문제로 단정하지 않는다.
- 최신 추가 로그: 세션 1234567C에서 `PLAY rtsp://10.0.0.3:554/live/0` CSeq 4에 200 OK(18:28:04), 30초 뒤 OPTIONS CSeq 5에도 200 OK 확인. 재생 제어 요청 수락 및 해당 시점까지 제어 연결 유지는 확인되나 실제 영상 패킷 송출/수신/디코딩은 미확인이다. User-Agent는 `Lavf62.3.100`으로, FFmpeg/libavformat 기반 접속을 시사하며 사용자가 실제 사용하는 플레이어/서버 중계 프로그램은 미확인. SETUP 요청·응답의 Transport가 없어 UDP/TCP 여부도 미확정이다.
- 최신 RTSP 로그: OPTIONS에 `RTSP/1.0 200 OK`, CSeq 5, Session 1234567B 응답 후 `peer closed`, `delete client 65 from 10.0.0.1` 확인. 보드 RTSP 제어 연결과 요청 수신은 확인됐으나 제공된 구간에 DESCRIBE/SETUP/PLAY 또는 영상 수신 증거는 없다. 보드가 관측한 상대 주소는 VPN 서버 주소이며 서버 자체 접속인지 서버 NAT를 거친 PC인지는 미확정이다. 영상 전송 문제를 확정하지 않고 VLC의 RTSP/TCP 시도 및 접속 시작부터의 로그로 다음 분리 확인을 안내한다.
- 후속 사용자가 영상 실행/접속이 안 되는 것 같다고 보고했다. 샘플 실행 실패인지 VLC 접속 실패인지, 현재 접속 URL·실행 로그·VPN handshake는 아직 미확인이다. 다음은 실행 로그와 기존 카메라 프로세스, `wg show`, 현재 wlan0 주소 확인으로 영상 생성과 VPN 경로를 분리한다. 옵션 없는 실행은 가능하지만 기존 기록상 기본 비트레이트 10×1024 Kbps이고 `-b 4096`에서 끊김 해소를 보고했으므로 이후 재시험은 기존 성공 설정을 우선한다. 비트레이트를 이번 실패 원인으로 확정하지 않는다.
- 사용자가 보드용 설정 후보 제시: Address `10.0.0.3/24`, ListenPort `51820`, 서버 Peer의 AllowedIPs `10.0.0.0/24`, Endpoint `54.180.26.70:51820`, PersistentKeepalive `25`. 실제 적용·서버 공개키 일치·서버의 보드 Peer 등록·handshake는 미검증이다. Wi-Fi 주소와 VPN 주소는 별개다.
- 서버의 해당 보드 Peer에는 보드 공개키와 AllowedIPs `10.0.0.3/32`가 필요하다. 다른 VPN 클라이언트에서 서버를 경유하는 접근은 서버 forwarding/방화벽 설정도 필요하다.
- 사용자 메시지에 개인키가 포함되어 교체 및 서버 측 보드 공개키 갱신을 권고한다. 개인키 값은 이 문서에 저장하지 않는다.

## WPA 재실행 시 제어 소켓 충돌 — 2026-09-22

- 최신 사용자 wlan0 출력: IPv4 `192.168.45.194`, 마스크 `255.255.255.0`, MAC `74:24:CA:50:04:AF`, `UP BROADCAST RUNNING MULTICAST`, RX/TX 패킷 및 오류 0 확인. 현재 인터페이스 활성화/IP 할당은 확인됐으며 WPA status·RTSP 재생은 이번 출력만으로 검증되지 않았다. 현재 주소를 고정 IP로 취급하지 않는다.
- 최신 사용자 `ps -ef` 출력에서 PID 598의 `wpa_supplicant -B -i wlan0 -c /data/wpa_supplicant.conf -d` 실행 확인. 현재 실행 인스턴스의 설정 경로는 `/data/wpa_supplicant.conf`이며 앞서 수동 실행에 사용한 `/etc/wpa_supplicant.conf`와 다르다(두 파일의 내용/링크 관계는 미확인). 기존 인스턴스의 status/IP 확인을 우선하며, reconfigure는 해당 인스턴스의 `/data` 설정을 다시 읽는다. PID는 관측 당시 값이며 재사용하지 않는다.
- 사용자 출력: `ip link set wlan0 up` 뒤 `wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf` 실행 시 `ctrl_iface exists and seems to be in use` 및 제어 인터페이스 초기화 실패. 기존 프로세스 실행 또는 종료 후 소켓 잔존 중 어느 경우인지는 아직 미확인이다.
- 다음 확인은 `wpa_cli -i wlan0 ping`과 `status`. PONG 응답 시 기존 인스턴스를 사용하며 필요한 경우 reconfigure/reassociate를 수행한다. 응답이 없으면 `ps -ef`에서 프로세스를 확인한 뒤 판단하며 소켓을 먼저 삭제하지 않는다. 현재 인증 상태·IP·DHCP 동작은 미확인이다.

## 핫스팟 IP 변경 보고 — 2026-09-20

- 최신 사용자 선택: 갤럭시 핫스팟은 당분간 두 주소를 시도해 사용하는 방식으로 두고, 일반 Wi-Fi 공유기에서 같은 IP를 유지하는 방법을 우선 안내한다. 핫스팟 중복 IP 진단은 현재 필수 대기 작업이 아니다. 공유기 DHCP 주소 예약(보드 wlan0 MAC 기준)을 안내했으며 공유기 모델·실제 예약 IP·적용 성공은 아직 미확인이다. 예약 후 보드 재부팅 전후 wlan0 주소와 RTSP 재생을 확인해야 한다.
- 사용자 정정: 보드를 껐다 켤 때 RTSP 접속 주소가 `.101` 또는 `.102`로 달라진다는 의미다. 핫스팟 제공 장치는 **갤럭시 휴대폰**으로 확인(모델/One UI 버전 미확인). 한 시점의 이중 IP 관측과 재부팅 간 주소 변화는 별개로 모두 유지한다. 갤럭시의 DHCP 예약 지원은 확인되지 않았으며 보드 고정 IP도 아직 설정하지 않았다.
- 최신 사용자 출력에서 wlan0에 `10.174.156.101/24`(valid/preferred_lft forever)와 `10.174.156.102/24`(secondary dynamic noprefixroute, valid_lft 2314초/preferred_lft 1864초)가 동시에 존재함을 확인했다. 기본 게이트웨이는 `10.174.156.15`, 해당 /24 경로의 선호 출발지(src)는 `.102`, metric 3004. `.101`의 forever만으로 수동 고정 IP라고 단정하지 않는다. DHCP 클라이언트가 무기한 주소로 커널에 등록한 경우도 구분해야 한다.
- 같은 시점 `ps | grep -E 'udhcpc|dhcpcd'` 출력은 비어 있다. 현재 DHCP 중복 실행은 확인되지 않았으며, 기본 ps의 표시 범위/명령 잘림 및 주소를 남기고 종료된 프로세스 가능성은 미확정. 다음은 과거 사용 가능했던 `ps -ef` 및 `/etc/network/interfaces`, `/etc/dhcpcd.conf`, `/etc/init.d`의 wlan0/DHCP 설정 조회. 주소 삭제·프로세스 종료는 아직 수행하지 않는다. 현재 RTSP 두 주소별 접속 성공 여부는 미검증이다.
- 사용자가 핫스팟/WPA 연결 시 RTSP 주소의 마지막 자리가 1과 2로 바뀐다고 보고했다. 이번 보고의 전체 IP·변경 시점·동시 주소 존재 여부는 미확인이다.
- 과거 wlan0에 `.101`/`.102` 동시 할당 및 udhcpc 중복/dhcpcd 관측이 있어, DHCP 재할당과 중복 관리를 구분해야 한다. 현재 `ip -4 addr show dev wlan0`, `ip route`, `ps`의 DHCP 관련 출력 확인 전 원인 확정 또는 프로세스 종료·고정 IP 설정은 하지 않는다.

## 진행 단계 — 2026-09-20 실시간 통합 시험 재개

- 최신 ENUM_FRAME_INTERVAL 출력: 요청 pad0/code0x300e/3864x2192에서 20fps와 30fps 항목이 반복 나열됨. 현재 실측 FPS나 서로 다른 해상도라는 의미는 아니다. 이 출력만으로 HDR/bit depth별 세부 모드 대응은 확정하지 않는다.
- 다음 분리 시험용 `-D` 옵션을 현재 소스 `cctv_npu_output_fix_complete/cctv_person_npu/src/main.cc`에 추가했다. 두 VI 채널과 NPU는 유지하고 VENC/RTSP 생성·바인딩·송출을 생략한다. 메인 VI depth=1로 별도 스레드에서 프레임을 받아 반환하며 AI 경로는 유지한다. 센서 모드/클록 변경 없음. 소스 검토 및 ZIP 무결성 확인 대상이며 ARM 컴파일/보드 실행은 아직 미검증. 사용자 VM에서 재빌드 후 `-D -f 5 -t 0.5`로 확인해야 한다. 성공해도 메인 VI 소비 방식·메모리 부하도 달라졌으므로 VENC 자체 원인으로 단정하지 않는다.
- 최신 센서 해상도 조회: code=0x300e ENUM_FRAME_SIZE 결과 5개 모두 3864x2192 고정. 현재 드라이버가 해당 10비트 형식으로 더 작은 해상도를 노출하지 않는 것으로 확인. 센서 하드웨어 전체의 지원 한계나 12비트 모드까지 일반화하지 않는다. 프레임 간격 조회는 옵션과 인자가 별도 명령으로 입력되어 실행되지 않았으며, 필수 인자를 같은 명령에 포함해 재조회 필요. 별도로 입력한 `pad=0,code=0x300e,width=3864,height=2192`는 셸 변수 할당으로 센서 설정 변경이 아니다.
- 최신 센서 조회: /sys/class/video4linux/v4l-subdev2/name=`m00_b_imx415 4-0037`. G_FMT pad0=3864x2192, mbus=0x300e SGBRG10_1X10. ENUM_MBUS_CODE에서 0x300e 5회와 0x3010 SGBRG12_1X12 5회가 표시됨. 중복 개수를 지원 해상도 수로 해석하지 않는다.
- 센서 controls: exposure 2250 (4..2992), vertical_blanking 808 (58..30575), horizontal_blanking 4936 고정 read-only, analogue_gain 69, link_frequency index1=446000000Hz, pixel_rate=356800000 read-only. 스트리밍 종료 상태의 조회이며 실제 FPS 측정이 아니다. 지원 해상도는 아직 미확인. 다음은 code=0x300e의 ENUM_FRAME_SIZE와 현재 해상도의 ENUM_FRAME_INTERVAL 조회; 설정 변경은 하지 않았다.
- 최신 대조 결과: 기존 스트리밍 종료 후 동일 cctv_person_image/model/person.jpg에서 다시 24명 검출 성공, detect_ms=98.3. 스트리밍 없음 성공 → 스트리밍 있음 timeout → 종료 후 성공의 비교를 확인했다. 영구적 NPU 고장이나 모델 실행 자체 불가로 단정할 근거는 없으며 동시 실행 조건의 문제를 조사한다.
- 사용자 보드 `uname -r`=5.10.160, `command -v v4l2-ctl`=/usr/bin/v4l2-ctl 확인. 다음은 기존 로그의 센서 노드 /dev/v4l-subdev2 이름·현재 포맷·지원 mbus 코드·컨트롤 읽기 전용 조회. 지원 센서 해상도와 ISP 입력 부하를 조사하되 아직 모드/드라이버/클록 설정은 변경하지 않는다.
- 최신 분리 시험: 기존 simple_vi_bind_venc_rtsp 실행 중 별도 cctv_person_image로 동일 person.jpg 추론을 시도한 결과 3개 출력 할당과 NPU ready 이후 실패. op id 2 Conv:/model.0/conv/conv/Conv, task counter 13, irq/raw status=0, 약 6.15초 대기 후 timeout/soft reset. 이후에도 IMX415 gain 로그가 계속되어 카메라는 동작 중이었다. 새 보조 VI/카메라 RGB 변환/통합 스레드 없이도 재현되므로 그 경로만의 문제라는 설명은 배제된다. ISP/VENC/메모리·드라이버 중 직접 원인은 아직 미확정이고 공통 NPU 코드까지 완전히 배제한 것은 아니다.
- 다음 비교: 기존 스트리밍을 정상 종료한 뒤 동일 이미지 추론을 다시 실행하여 timeout 이후 독립 동작 회복 여부 확인. 센서 모드 조사에 사용할 v4l2-ctl 유무와 커널 버전을 함께 확인한다. 프레임률/비트레이트만 낮추는 반복 시험이나 runtime 임의 교체는 아직 수행하지 않는다.
- 최신 커널 첨부 `c7f5e8c0-3b20-4fce-92f4-f3c03797573a/붙여넣은 텍스트.txt`: 약 166초 및 261초 시점의 두 NPU job timeout 확인. timeout=6000000us, 실제 대기 약 6.46/6.60초, irq/raw status=0, task counter=22/20, 이후 soft reset 및 앱의 ISP/VENC 종료. 새 로그 범위에 CMA 할당 실패는 없고 종료 후 MemAvailable=118312kB, CmaAllocated=12kB. 종료 후 수치로 실행 중 메모리 압박을 배제하지 않는다.
- 같은 로그에서 센서 3864x2192 및 `over virtual isp max resolution, force to 2 readback` 반복 확인. 인코더 출력은 1920x1080/4096000bps/GOP30. 센서 입력과 출력 해상도가 다르며 readback 경로가 보이나 NPU 시간 초과 원인으로 확정하지 않는다. 이번 첨부 범위에 CSI FIFO overflow는 보이지 않는다.
- 다음 분리 시험: 기존 `/oem/usr/bin/simple_vi_bind_venc_rtsp -b 4096`를 한 보드 터미널에서 실행하고, 별도 보드 터미널에서 이미 성공한 cctv_person_image를 같은 person.jpg로 실행. 카메라 영상 전처리·보조 VI·새 통합 스레드와 독립적인 입력으로 동시 실행 문제를 비교한다. rkipc/통합 앱은 중복 실행하지 않는다. 시험 결과는 아직 미확인이다.
- 최신 실행 로그 `ec186ec0-3d7c-44fe-b829-fa3a90b1eb6a/붙여넣은 텍스트.txt`: `-b 4096 -f 5 -t 0.5`에서 NPU 입출력 할당·바인딩 성공, RTSP 554/live/0 생성, IMX415 IQ 선택 및 AIQ init/prepare/start 성공, mainpath/video11과 selfpath/video12 stream on 성공. READY 이후 H264 SPS/PPS 및 VIDEO fps=20.1 출력. VLC 수신 여부는 아직 미확인이며 20.1은 시작 직후 약 5초 구간 인코더 출력 측정이다.
- 같은 실행에서 PERSON 로그 없이 RKNN `failed to submit`, op id 3 Conv:/model.1/conv/Conv, int status 0 및 `rknn_run: -1` 발생. 이전 CMA 출력 버퍼 할당 실패와 다른 추론 실행 단계 오류. 원인 확정 전이며 커널 dmesg 필요. run task counter 22를 성공한 추론 22회로 해석하지 않는다. NPU 스레드가 stopped를 설정하여 앱이 RTSP·VI·AIQ를 종료했고 종료 중 accept failed가 출력됨.
- 단독 이미지 성공과 동시 실행 실패 차이로 ISP/VENC/NPU 동시 자원 사용 및 드라이버 경로를 조사한다. 같은 보드/센서의 유사 사용자 보고 https://github.com/airockchip/rknn-toolkit2/issues/544 를 찾았지만 버전·설정 차이가 있고 보고자의 버스 경합 가설은 이 보드의 확정 원인이 아니다. 자동 출력된 업데이트 권고만으로 runtime/toolkit을 교체하지 않는다. 다음은 종료 직후 dmesg tail 120과 meminfo 확인.

- 사용자가 구축 문서 작성 이후 다음 단계 진행을 요청했다. 단일 이미지 NPU 성공을 바탕으로 IMX415 + NPU + RTSP 통합 프로그램을 Wi-Fi/Windows VLC에서 시험하는 단계로 이동한다.
- 새 실시간 실행 결과는 아직 없다. 먼저 보드 현재 wlan0 주소와 카메라 앱 점유를 확인한다. 과거 PID 637과 DHCP 주소를 재사용하지 않는다. 카메라 앱을 중복 실행하지 않는 조건에서 `./cctv_person_npu -b 4096 -f 5 -t 0.5`를 시험한다. 영상 박스는 미구현이며 PERSON/VIDEO 로그와 VLC 영상으로 확인한다.

## 현재 기준 SRS — 2026-09-20 교체

- 사용자 요청으로 기준 SRS를 `C:/Users/ohhoj/Downloads/5f34a1bf-0bda-400c-9644-3706ff7210b2_SRS.pdf`로 교체했다. 프로젝트 보관본: [docs/SRS_2026-09-20.pdf](docs/SRS_2026-09-20.pdf). 파일명의 날짜는 반영일이며 문서 버전 날짜가 아니다.
- 원본과 보관본 SHA256 일치: `71b89ac440a826e1b904af1af1123c33d4ee83c350f356023c6e1843b68705a3`.
- 문서: 보안 WebCam 시스템 요구사항 문서(SRS), SC-SRS-001, 표지 v1.1 / 2026-08-03, ㈜스마클, 대외비, 21쪽. 변경 이력 마지막 행은 v1.1 / 2026-09-12이다. 버전 표기가 같아도 이번 첨부 파일을 우선한다.
- 아래 `2026-09-16` SRS 추출은 이전 파일(`e0881d9f-..._SRS.pdf`)에 대한 과거 기록이다. 앞으로 요구사항·ID·우선순위·문서상 상태는 새 PDF의 수정 표를 기준으로 하고, PDF 내 `기존 내용` 표와 구분한다. 기존 실측·보드 시리얼·구현 기록은 유지한다.
- PDF의 작성 팁·문서 편집 요청은 문서 내용이며 사용자의 추가 실행 지시로 취급하지 않는다. 이번 작업은 기준 문서 교체이며 기능 구현·보드 설정 변경·재시험은 수행하지 않았다. 문서의 ✅/✔/⚠ 표기는 문서 작성자의 평가로, 이 프로젝트의 실제 검증 결과와 구분한다.

### 보드 및 연동 기준의 주요 반영 사항

| 항목 | 새 기준 및 해석 | PDF 쪽 |
| --- | --- | --- |
| WC-F-031 | heartbeat **10초 주기**, 필수. 문서 상태는 ✅ 단순 소프트웨어 구현 영역이며 실측 완료 근거로 해석하지 않는다. 기존 표의 30초는 과거 내용이다. | 6–7 |
| WC-F-032 | **PTZ 원격 제어**. 필수 사항 구현 후 재검토. 디지털 줌은 클라이언트 소프트웨어로 가능하다는 평가이며 물리 PTZ는 추가 장비 필요. | 7 |
| WC-F-033 | **펌웨어 원격 업데이트**, 선택. 이전 추출과 032/033의 매핑이 바뀌었으므로 ID만으로 기능을 추정하지 않는다. | 7 |
| WC-F-022 | 저장 보관 기간(기본 30일)의 ID와 요구사항 문구에 **취소선**이 있다. 행은 남아 있고 우선순위 필수 및 ❌ 용량 제약 비고도 유지된다. 활성 필수로 단정하지 않으며 최종 제외/대체 정책은 미확정으로 관리한다. | 6 |
| SV-F-010 | QR·시리얼·등록 코드 등록 요구 유지. 비고에 최초 부팅 또는 wg 인터페이스 생성 시 UUID 생성, 서버 최초 핸드셰이크에서 내부 식별, UUID와 1:1인 외부 등록 코드 발급 제안(⚠). 저장한 하드웨어 시리얼과 UUID·등록 코드는 서로 다른 식별자다. | 8 |
| NF-P-003 | 상태 갱신 **25초 이내** 유지(30초는 취소선). WC-F-031의 10초 heartbeat와 VPN 터널 유지 테스트를 동일 기능으로 간주하지 않는다. | 18–19 |
| 기술 제약 §4.5 | PostgreSQL 17.X(원문 표기 PostrgreSQL), Kotlin(버전 미정 표기), React.js, Express.js / Node.js 24.11. 보드 런타임 요구로 확대 적용하지 않는다. | 20 |

- 보드 요구사항 상세는 §3.1(p.4–7), VPN은 §3.4(p.11), 비기능은 §4(p.18–20)를 참조한다. 영상/감지/장애 복구의 요구와 보드 단일 이미지 NPU 성공 기록을 구분한다.
- APP/WEB 요구사항 문구 및 구현 상태도 새 PDF의 §3.5–3.6(p.11–18)를 기준으로 한다. 이번 보드 중심 요약은 전체 시스템 요구사항 목록을 대체하지 않는다.
- p.21 요약 건수는 상세 항목과 불일치한다(예: 웹캠 15건 표기, 상세 WC-F ID는 취소선 항목 포함 19개). 구현 범위는 요약 건수 대신 상세 표와 취소선 상태로 판단한다.

## 장비 식별 정보 — 2026-09-20 저장

- 보드: **Luckfox Pico Zero** (연결 카메라: **IMX415**).
- **보드 시리얼 번호: `ac6e3deb24cfa546`**.
- OTP 원본 CPU ID(16바이트): `4d4152383133000000000000000b2520`.
- 조회 경로: `/proc/device-tree/serial-number`.
- 검증: `/sys/bus/nvmem/devices/rockchip-otp0/nvmem`의 오프셋 10부터 16바이트를 읽고 공식 시리얼 생성 방식으로 계산한 값이 위 시리얼과 일치함.
- 사용자 요청으로 저장한 이 보드의 식별 정보이며, 이후 같은 보드 작업에서 참조한다.

## 최신 확인 — 2026-09-20: 보드 시리얼 조회

- **최신 대조 성공:** 사용자가 `od` 및 `dd | hexdump`로 읽은 OTP 오프셋 10의 16바이트는 `4d 41 52 38 31 33 00 00 00 00 00 00 00 0b 25 20`이며 두 출력이 일치한다. 공식 `rockchip_set_serialno()` 방식(홀수 인덱스 8바이트에 seed 0의 crc32_no_comp, 그 결과를 seed로 짝수 인덱스 8바이트 계산)을 적용한 결과 하위 `24cfa546`, 상위 `ac6e3deb`, 최종 `ac6e3deb24cfa546`으로 장치 트리 값과 정확히 일치했다. 따라서 현재 시리얼과 OTP CPU ID의 계산 관계를 확인했다. 아래 원시 데이터/관계 미확인은 이 대조 전 이력이다. 제조사 라벨 일치·전 기기 무중복·펌웨어 변경 후 보고 값 불변까지 검증한 것은 아니다.
- 후속 사용자 출력: `rockchip-otp0/nvmem`은 128바이트, 읽기 전용(`-r--r--r--`)으로 표시되며 `of_node`는 `otp@ff3d0000`을 가리킨다. 실제 바이트 내용은 아직 미확인. 공식 SDK `sysdrv/source/kernel/arch/arm/boot/dts/rv1106.dtsi`에서 `otp_id: id@a`의 `reg = <0x0a 0x10>` 확인: CPU ID 영역은 오프셋 10부터 16바이트. 이 영역을 읽어 기존 시리얼과 생성 관계를 대조하는 단계이며 실제 일치는 아직 미검증.
- 사용자 제공 출력: `/proc/device-tree/serial-number` 값은 `ac6e3deb24cfa546`. 현재 펌웨어가 장치 트리에 제공하는 장치 시리얼로 확인했다. 출력 뒤 쉘 프롬프트가 붙은 것은 줄바꿈이 없기 때문이며 프롬프트는 시리얼에 포함되지 않는다.
- `/sys/bus/nvmem/devices/`에 `rockchip-otp0`가 표시됨. 이는 장치 이름이며 시리얼 값 자체가 아니다. OTP 원시 데이터와 위 시리얼의 생성 관계, 제조사 라벨 번호와 일치 여부, 재플래시 후 유지 여부는 이번 출력만으로 검증하지 않았다.
- 공식 Luckfox SDK main 소스 추가 확인: `sysdrv/source/uboot/u-boot/common/fdt_support.c`가 U-Boot `serial#`를 루트 `serial-number` 속성으로 전달한다. `arch/arm/mach-rockchip/board.c`의 `rockchip_set_serialno()`는 설정에 따라 vendor storage SN을 사용하고, 기존 `serial#`가 없으면 eFuse/OTP CPU ID로 CRC32 기반 시리얼을 생성한다(해당 드라이버 설정이 없으면 random cpuid 대체 경로도 존재). 현재 보드의 실제 펌웨어 설정 및 선택된 경로는 미확인이다. 따라서 현재 보고되는 장치 시리얼임은 확인되나, 출력만으로 OTP 원본·영구 불변·전 보드 간 무중복을 보장하지 않는다.

## 최신 상태 — 2026-09-18: 구축 문서 작성

- 사용자 요청으로 실시간 스트리밍 시험 전에 구축 방식과 검증 결과를 `CCTV_NPU_IMPLEMENTATION.md`에 정리했다. 현재 로컬 소스와 실행 로그를 대조했으며 장비/VM 환경, 파일 역할, 이미지 및 실시간 처리 구조, 메모리 할당, 모델 출처, 빌드·배포, 오류 해결, 검증 범위와 남은 작업을 포함한다.
- 현재 확정 상태: 두 실행 파일 ARM 빌드 성공; 보드에서 612×368 사진의 인물 24명에 대응하는 검출 24개와 결과 JPG 저장 확인; detect_ms=98.0은 전처리·NPU·후처리 단일 측정. 실시간 카메라+NPU+RTSP 동시 실행은 아직 시작하지 않았다. 사용자 요청은 현재 문서화이며 이 턴에서 실시간 시험을 진행하지 않았다.
- 현재 소스의 `tools/convert_rv1106.py`는 importlib.metadata 의존성이 남아 Python 3.7 base에서 그대로 실행할 수 없다. 확인 도구 수정과 변환 도구를 구분한다. 새 모델 변환 시 Toolkit 호환 Python 환경 확인이 필요하며 실제 모델 변환은 미수행이다.
- 아래 2026-09-17 항목은 작업 이력이다. JPG 저장·감지 성공 뒤에도 남아 있는 과거 미확인/실패 문장은 최신 상태보다 우선하지 않는다.

## 최신 작업 — 2026-09-17: RKNN 가이드 반영

이 절이 NPU 구현의 최신 상태이며, 아래 장비·Wi-Fi·스트리밍 관측 기록은 계속 적용한다.

- 최신 이미지 확인: 사용자가 `C:/Users/ohhoj/Downloads/person.jpg`와 `result.jpg`를 첨부. 원본은 6열×4행의 인물 24명 모음이며 결과 24개 박스가 각 인물과 대응함을 육안 확인. 뚜렷한 누락/중복은 보이지 않지만 일부 박스는 인접 칸으로 넘어감(특히 좌하단). 라벨이 상단에서 잘리거나 겹치는 표시 문제도 있음. 해당 이미지에서 감지·좌표 복원·결과 JPG 저장을 확인했으며 일반 장면 정확도나 실시간 성능 검증은 아님. 과거 JPG 저장 미확인 상태는 해소됨.
- 다음 단계: 기존 카메라 앱을 함께 실행하지 않는 조건에서 보드 `cctv_person_npu -b 4096 -f 5 -t 0.5` 실행, Wi-Fi IP로 Windows VLC 수신 및 터미널 PERSON/VIDEO 로그 확인. 현재 스트리밍 버전은 VLC 박스 표시 미구현. rkipc 완전 종료 여부는 아직 새 관측이 없어, 카메라 점유 오류가 나면 실제 상태부터 확인한다.

- **최신 결과: 보드 단일 이미지 NPU 추론 및 후처리 실행 성공.** rkipc 종료 요청 및 CMA 점유 감소 후 재시험에서 3개 출력 메모리 할당, `NPU ready`, `PERSON count=24 image=612x368 detect_ms=98.0` 출력. 세 번째 출력은 `[1,20,20,255]`, logical=102000/allocation=102400. 이전 메모리 할당 실패는 이번 재시험에서 재현되지 않았다.
- 검출 score 약 0.698~0.906, 박스가 대략 6열×4행에 분포. 원본 person.jpg 및 result.jpg를 아직 보지 못했으므로 실제 인원 일치·박스 정확성·모델 정확도는 미검증. detect_ms=98.0은 한 이미지의 전처리+추론+후처리 시간이고, 모델 초기화·파일 읽기/저장·영상 지연이나 지속 FPS가 아니다.
- 마지막 MPP 버퍼/채널 로그는 결과 JPG 저장 경로 실행 중 나온 것으로 코드와 일치한다. 사용자 출력에 쉘 복귀·종료 코드·저장 이미지 확인이 없어 JPG 저장 완료는 별도 확인 필요. 다음은 결과 이미지 박스 확인 후 실시간 RTSP+NPU 동시 검증. rkipc 프로세스 완전 종료 여부도 아직 미확인이다.

- 최신 첨부 `8fd9be05-757e-4445-9236-696070899063/pasted-text.txt`: 종료 요청 후 확인 맥락에서 PID 637 rkipc가 ps에 여전히 표시됨. 상태(State) 정보가 없어 실행/종료 대기/좀비 여부 미확정. CmaAllocated는 이전 85180kB에서 12kB로 감소, CmaReleased는 102388kB로 증가, CmaFree는 여전히 0. MemAvailable=113212kB. 이 펌웨어의 CmaFree 단독으로 CMA 소진을 판정하면 안 됨. rkipc 자원 정리와 일치하는 메모리 변화이나 종료 완료와 NPU 재시험 성공은 아직 미확인. 첨부에 새 이미지 실행 결과는 없음. 다음은 `/proc/637/status`와 단일 이미지 재시험 결과 확인; 강제 종료·펌웨어 변경은 불필요하게 진행하지 않는다.

- 최신 첨부 `4e1b54d7-4cfb-4aa6-8345-6fe3466b0f93/pasted-text.txt`: 실패 후 MemAvailable=111848kB, CmaTotal=102400kB, CmaAllocated=85180kB, CmaReleased=17220kB, CmaFree=0. 커널에 `rk-dma-heap-cma: alloc failed, req-size: 100 pages, ret: -12`, `RKNPU: dmabuf alloc failed, args.size = 409600`가 있어 CMA 경로 할당 실패를 확인. 실패 후 메모리 값이므로 추론 중 피크 사용량으로 해석하지 않는다.
- 최신 ps에서 PID 637 `rkipc -a /oem/usr/share/iqfiles` 실행 확인. 이전 rkipc 부재 관측은 과거 이력이다. 부팅 시 커널 로그에 IMX415 3864x2192 입력과 H.265 3840x2160, 최종 설정 8192000bps/15fps가 보인다(현재 실측 fps 아님). rkipc의 카메라/인코더 점유가 CMA 압박의 유력 요인이나 전체 점유량 귀속은 미확정. 다음은 이 rkipc만 정상 종료 요청 후 메모리 및 단일 이미지 재시험 비교. 임의 CMA 증설·모델 재변환은 아직 하지 않는다.

- 최신 보드 재실행: 출력 크기 검사 통과. output[0] logical=1632000/allocation=1638400, output[1] `[1,40,40,255]` logical=408000/allocation=409600 확인. 두 번째 출력의 `rknn_create_mem` 단계에서 `failed to allocate fd ... errno: 12 Cannot allocate memory`, `NPU output allocation failed` 발생. 첫 출력 바인딩 이후까지 진행했지만 `rknn_run` 전이므로 추론 성공 아님.
- 원인 미확정: 시스템 RAM 부족, 하드웨어용 메모리 풀 부족/단편화, 다른 실행 프로세스 점유 등을 보드 관측으로 구분해야 한다. 최신 프로세스 목록·/proc/meminfo·커널 로그가 필요하다. 과거 스트리밍 프로세스가 지금도 실행 중이라고 가정하지 않으며, 메모리 설정 변경이나 일괄 kill은 하지 않는다.

- 출력 메모리 검사 수정본 후속: 사용자 VM 로그에서 두 실행 파일 ARM 재빌드 및 dist 설치 성공 확인. `CCTV_IMAGE_IO_imgcodecs_hpp - Failed`는 선택적 이미지 IO 방식 검사 실패이며, 이어진 `CCTV_IMAGE_IO_highgui_hpp - Success`로 대체 방식 선택 완료. 전체 빌드 실패가 아니다. 수정본의 보드 재실행 결과는 아직 대기 중.

- 최신 보드 이미지 테스트: 배포 및 실행 성공, JPG 로딩 후 `rknn_init`과 속성 조회 성공. runtime `1.6.0 (2de554906@2024-01-17T14:53:41)`, driver `0.9.2`. 입력 NHWC INT8 `[1,640,640,3]`, zp=-128, scale=0.00392157, size=1228800. 첫 출력 `[1,80,80,255]`, size=1632000, size_with_stride=1638400, w_stride=0. 앱의 두 크기 동일 조건에 걸려 `rknn_run` 전에 실패했다. NPU 추론 성공으로 기록하지 않는다.
- 수정: 기존 vendor native NHWC 255 채널 읽기 방식을 유지하고, 실제 버퍼는 size_with_stride로 할당하되 논리 size 이상인지 검사한다. 명시적 비일치 w_stride는 거부하고 후처리에는 논리 size만 전달한다. 단순히 크기 비율로 256 채널이라고 추정하지 않는다. 수정 후 ARM 재빌드 및 보드 결과 대기.
- 수정 후 PC CTest 1/1 통과. 추가 할당 영역을 높은 값으로 채워도 논리 크기 내 packed 데이터의 검출 결과가 동일한지 회귀 검사했다. 이 테스트는 실제 NPU 출력 배치 검증을 대체하지 않는다. 전체 소스 `cctv_npu_output_fix_complete.zip`으로 제공한다.

- **최신 확인: 사용자 제공 VM 로그에서 ARM 빌드 및 dist 설치 성공.** `person_inference`, `cctv_person_image`, `cctv_person_npu` 컴파일·링크 완료. `/home/student/cctv_person_npu/dist`에 두 실행 파일, `model/yolov5.rknn`, README 설치됨. 아래 소스 누락 및 OpenCV 필수 모듈 오류는 해결된 과거 이력이다. OpenCV 경로 경고는 남지만 highgui 이미지 IO 검사와 최종 빌드가 성공했다.
- 아직 보드 배포·실행·NPU 감지 성공은 미확인이다. 다음은 dist 내용과 사람이 포함된 JPG를 보드 작업 폴더에 복사하여 `cctv_person_image` 실행 로그 확인. 그 후 Wi-Fi/VLC 스트리밍 동시 검증. 현재 DHCP IP나 SSH 전송 가능 여부는 새로 확인된 바 없다.

- 최신 VM 재빌드 로그: OpenCV core/imgproc/highgui 탐색 성공, highgui.hpp의 imread/imwrite 컴파일·링크 검사 성공. 이미지 타깃 제외 없이 사용 가능하다. 이전 include 경고는 남지만 구성 진행됨. 이후 `src/main.cc`, `src/npu_model.cc` 미발견으로 CMake 생성 단계 실패. 파일 누락 또는 복사 배치 문제이며 원인은 미확정, ARM 프로그램 빌드 완료 아님.
- 로컬 소스에는 위 파일이 존재함을 확인했다. 혼동을 줄이기 위해 패치가 아닌 전체 묶음 `cctv_person_npu_complete.zip`을 재생성하고 15개 파일 CRC, 필수 소스 및 OpenCV 수정 포함을 확인했다. ZIP은 최상위 `cctv_person_npu/` 폴더를 포함하므로 VM `/home/student`에 풀어야 한다.

- VM 빌드 후속: 사용자 로그에서 ARM GCC/G++ 8.3.0 컴파일러 동작 확인. CMake는 vendor OpenCV 4.10.0의 include 경로 부재 경고와 필수 imgcodecs 누락으로 구성 실패했다. ARM 소스 컴파일까지 진행되지 않았다.
- 로컬 소스 폴더가 현재 `cctv_person_npu_source/cctv_person_npu`에 있는 것을 확인했다. 이 폴더의 CMake를 core/imgproc 필수, imgcodecs/highgui 선택으로 수정하고 vendor include 경로 탐색을 보완했다. 이미지 IO는 헤더별 컴파일·링크 검사로 선택하고, 제공되지 않으면 경고 후 이미지 타깃만 제외한다(이미지 전용 빌드는 실패 처리). 이것은 단일 이미지 검증 완료를 의미하지 않는다. 이미지 main은 선택된 헤더를 사용하도록 수정했다. VM 재빌드 결과는 미확인이다.

- 사용자 제공 `C:/Users/ohhoj/OneDrive/바탕 화면/rknn_guide.pdf` 6쪽을 확인했다. RV1106/RV1103의 RKNN C API, INT8 모델, ARMv7/uClibc 교차 빌드, 단일 이미지 검증 후 카메라 연동 순서로 Windows 작업공간의 `cctv_person_npu`를 수정했다.
- 사용자 VM은 Ubuntu이며 Visual Studio SSH로 사용한다. 알려준 홈은 `/home/student`, RKMPI 예제는 `/home/student/RKMPI/luckfox_pico_rkmpi_example`이다. 전체 Luckfox SDK/컴파일러 경로와 Toolkit 버전은 미확인이다. Codex가 Visual Studio 또는 VM에 접속해 확인한 것이 아니다.
- 추가 소스: `cctv_person_image` 단일 이미지 추론 및 선택적 결과 JPG, `tools/convert_rv1106.py` RV1106 INT8 변환/ONNX 출력 구조 검사, `tools/check_vm.sh` 읽기 전용 환경 조회. 스트리밍 프로그램은 IMX415 → H.264 1080p/4096Kbps RTSP와 별도 사람 감지 경로를 구현했다. 기본 AI 입력은 최대 5회/초이며 보드 실측 성능이 아니다.
- 결과는 사람 수·신뢰도·좌표를 터미널에 출력한다. VLC 영상 위 박스 표시는 아직 구현하지 않았다. 원본 vendor 예제는 수정하지 않았다.
- 검증: Windows MSVC 후처리 C++ 테스트 CTest 1/1 통과, Python 변환 입력 경로 검증 unittest 2/2 통과, 변환 스크립트 구문 검사 통과. Python 테스트는 샌드박스 임시 폴더 접근 제한 때문에 승인 후 동일 테스트를 재실행하여 통과했다.
- 미검증: VM Toolkit 실행, 보드 NPU/카메라 동시 실행, 성능·정확도·온도. ARM 교차 빌드는 위 최신 사용자 로그로 성공 확인. 사용자에게 배포 성공 또는 SRS 사람 감지 완료로 설명하지 않는다.
- 번들 `model/yolov5.rknn`은 기존 예제 모델을 복사한 후보이며 새로 변환한 모델이 아니다. SHA256 `7e72c8caa060ea15d69109e4af6c1f80005cbd1be14bbe1f6f2af9175861c44a`. 생성 Toolkit 및 대상 호환성은 미확인이다. 새 모델 변환에는 원본 ONNX와 대표 캘리브레이션 이미지, VM Toolkit이 필요하다.
- 다음 단계: VM으로 소스 폴더를 옮겨 `sh tools/check_vm.sh /home/student/RKMPI/luckfox_pico_rkmpi_example` 결과 확인 → 맞는 도구체인으로 빌드 → 보드에서 단일 이미지 검증 → Wi-Fi/VLC 스트리밍 동시 검증. 카메라 뒷면 발열은 관찰 기록만 유지하고, 사용자가 추후 스트리밍 온도 변화를 알려주기로 했다.
- VM 조회 후속(사용자 출력): x86_64, 작업 위치 `/home/student/cctv_person_npu`; cmake/make 있음. PATH에서 `/home/student/luckfox-live/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++` 발견. `LUCKFOX_SDK_PATH` 및 `GCC_COMPILER`는 미설정이지만 PATH 탐색으로 컴파일러 선택 가능하다. SDK 루트는 해당 경로에서 추정할 수 있으나 SDK 전체 내용은 확인하지 않았다.
- 같은 조회에서 vendor의 `include/rknn/rknn_api.h`, `lib/uclibc/librknnmrt.so`, OpenCVConfig.cmake 존재 확인. 활성 Python은 Conda base `/home/student/miniconda3/bin/python3`, 3.7.3. 스크립트가 `importlib.metadata`에서 실패했으므로 RKNN 설치·import 확인까지 도달하지 않았다. Toolkit 미설치라고 단정하지 않는다. 로컬 확인 스크립트는 해당 Python의 pip 정보 조회와 별도 RKNN import로 수정했다. 보드 C++ 빌드에는 PC Python Toolkit이 필요하지 않으며 기존 후보 모델로 빌드를 먼저 진행할 수 있다. ARM 빌드 결과는 아직 대기 중.

## 현재 기준 — 2026-09-16 업데이트

이 절을 현재 상태의 기준으로 사용한다. 아래 2026-09-14 기록은 과거 이력이며, 특히 색 편향 해소·네트워크 미연결 등의 표기는 현재 상태와 구분한다. CCTV 작업을 시작하거나 진단을 이어가기 전에 이 파일을 먼저 읽는다.

- 사용자 재확인: 보드는 **Luckfox Pico Zero**, 카메라는 **IMX415**. 모델을 반복해서 묻지 않는다.
- 목표 환경: 보드를 **Wi-Fi 핫스팟**으로 연결하고 Windows **VLC Media Player**에서 영상을 본다. USB 테스트를 최종 Wi-Fi 성능 검증으로 대체하지 않는다.
- 보드는 최소 구성의 임베디드 Linux이며 사용자가 `netstat`이 없다고 명시했다. 해당 명령을 재안내하지 않는다. 필요하면 사용 가능한 명령을 확인하거나 `/proc/net/tcp` 등으로 점검한다.
- `/etc/iqfiles`의 현재 목록(사용자 제공):
  - `imx415_CMK-OT2022-PX1_IR0147-36IRC-8M-F20.json`
  - `mia1321_MIA1321_30IRC-F16.json`
  후속 시작 로그에서 `/etc/iqfiles/imx415_CMK-OT2022-PX1_IR0147-36IRC-8M-F20.json` 선택 및 AIQ init/prepare/start 성공을 확인했다. 실제 렌즈·IR 필터 등 모듈 적합성 및 실행 중 AWB 정상 동작은 별도 검증이 필요하다. 과거 IQ 경로 수정을 반복하지 않는다.
- 핫스팟 SSID: `Wifi`, 2437 MHz, 사용자 제공 신호 -52 dBm. 보드 `ip route`의 wlan0 출발지 주소는 `10.174.156.102`, 게이트웨이는 `10.174.156.15`였다.
- PC Wi-Fi `10.174.156.31`에서 보드 `10.174.156.102:554`로 TCP 연결 성공을 직접 확인했다. VLC 영상 수신도 사용자 확인. 현재 성공 URL은 `rtsp://10.174.156.102:554/live/0`이다. DHCP 주소이므로 영구 고정 주소로 가정하지 않는다.
- 이전 `.101:554` 접속은 실패했다. 과거 이중 IP 이력이 있으므로 `.101`이 제거되었다거나 주소 변경의 정확한 원인을 확정하지 않는다. Ping 실패만으로 RTSP 통신 차단을 단정하지 않는다.
- 테스트 예제: `simple_test/simple_vi_bind_venc_rtsp.c`. 기본 1920×1080, H.264, 비트레이트 10×1024 Kbps, GOP 60. 사용자가 `-b 4096` 추가 후 끊김이 없어졌다고 보고했다. 현재 비교 기준은 4Mbps이며 VLC 캐시 200ms를 권장했으나 실제 유지값은 미확인. 네트워크 부하 영향이 의심되며 패킷 손실 원인은 확정하지 않았다.
- 지연: ffplay에서 약 2초로 느꼈고, 이후 VLC에서는 수백 ms 정도로 체감. VLC 캐시 100ms와 200ms의 차이는 잘 느끼지 못함. 정밀 측정값이 아니다.
- 현재 화질(사용자 정정): 주황빛은 카메라를 이리저리 옮기며 조명이 달라질 때 관찰한 현상이고, 사용자는 문제가 아닌 것 같다고 판단했다. 고정 구도에서의 색 편향 재발로 취급하지 않고 색 관련 추가 진단·AWB 고정·IQ 수정은 우선 중단한다. 고정 설치에서도 조명은 달라질 수 있으므로 실제 설치 환경에서 지속적인 이상이 재현될 때 다시 확인한다. 약간의 노이즈는 이전 보고이며 해소 여부는 별도 미확인. 끊김 순간 화면 깨짐은 없다고 확인됨.
- 온도 이력: 초기 `70742`(약 70.7°C), 후속 측정 `56485`(약 56.5°C). 후속 출력에서 thermal_zone0은 `soc-thermal`로 확인됨. passive trip은 75°C·85°C, critical trip은 115°C, 각 hysteresis는 2°C. 이 값은 커널 설정이며 부품 권장 동작 온도나 실제 스로틀링 동작 검증이 아니다. 장시간 온도 추이는 미확인.
- 다음 진단은 모델과 IQ 파일 목록을 다시 묻지 말고, 기존 rkipc 동시 점유 및 IQ 경로 해결 이력을 반영하여 현재 실행 프로세스·AIQ 로그, 일정한 조명에서 색 변화, 온도 추이를 필요한 범위에서 확인한다.
- 진단 재개(2026-09-16): 4Mbps와 VLC 캐시 200ms를 비교 조건으로 두었다. 첨부된 `ps -ef`에서 PID 2688 `/oem/usr/bin/simple_vi_bind_venc_r...` 1개가 확인되며 rkipc 및 별도 rkaiq 사용자 프로세스는 보이지 않는다. CMD가 잘려 전체 인자는 미확인. 샘플 시작 시각과 ps 시각이 모두 03:25라 시작 직후일 수 있고, 후속 56.5°C를 장시간 안정 온도로 간주하지 않는다. `[rkisp-unite0]`, `[venc]`, `[ircut wq]` 등은 커널 스레드이며 중복 카메라 앱으로 해석하지 않는다.
- 같은 ps에 `udhcpc -i wlan0` 두 개(PID 864, 1252)와 dhcpcd의 wlan0 관련 프로세스가 보인다. DHCP 중복 관리 가능성을 추후 확인하되 이중 IP·색 편향 원인으로 확정하거나 현재 연결 중인 프로세스를 임의 종료하지 않는다.
- 색 진단 이력: 샘플 시작 로그에서 IMX415 선택·IQ 경로·AIQ init/prepare/start 성공을 확인했다. 이후 사용자가 조명 변화 상황이었다고 정정하여 색 관련 진단은 우선 종료한다. AWB 고정·IQ 수정은 수행하지 않았다. 직접 연결된 보드 터미널은 없으므로 보드 관측은 사용자 명령 출력으로 확인한다.
- 시작 로그 후속 확인: 사용자 첨부 `f1477bbf-a292-47cb-8923-d48326e3f009/pasted-text.txt`에는 `-b 4096`, H.264 1920×1080, 센서 `m00_b_imx415 4-0037`, AIQ v5.0x5.0/ISP HW 32, IQ 선택 및 init/prepare/start 성공, ISP stream on·VI→VENC bind·SPS/PPS 생성이 나타난다. 센서 입력은 3864×2192로 1080p는 출력 해상도이며 센서 1080p 모드를 의미하지 않는다.
- 로그 경고: `invalid main scene len!` 이후 초기화·스트림은 성공한다. `s32SrcFrameRate(0) s32DstFrameRate(0)` 경고가 있어 VI 프레임률 초기화 점검 후보이며 실제 fps는 미확인. `AF:E:the time to wait motor move is too long, force stable`가 각 실행에서 보인다. AF 모터 대기 관련 경고로, AWB 오류·IR-CUT 고장·실제 AF 모터 존재를 이것만으로 확정하지 않는다. 첫 실행은 사용자 Ctrl+C로 정상 종료 후 재실행되어, 종료 과정의 `accept failed`를 스트리밍 중 접속 장애로 취급하지 않는다.
- 후속 기준: 4Mbps 스트리밍을 유지하여 고정 설치 환경에서 장시간 끊김·온도·화질 안정성을 확인하는 단계. 이전 정상/주황빛 비교 화면 요청은 사용자 정정에 따라 더 이상 필수 대기 항목이 아니다. 장시간 검증 완료로 기록하지 않는다.
- 카메라 발열(사용자 요청으로 관찰만 기록): **IMX415 카메라 뒷면에 발열이 있음.** 온도 실측 및 정상/이상 판정은 하지 않았다. 추가 발열 진단은 진행하지 않으며, 앞서 제안한 스트리밍 종료 후 냉각 비교 결과도 요청하거나 대기하지 않는다. 기존 `soc-thermal` 측정값은 카메라 온도가 아니다.

## 과거 SRS에서 추출한 보드 요구사항 — 2026-09-16 (2026-09-20 교체됨)

이 절은 이전 SRS의 이력이다. 현재 요구사항 기준은 문서 상단의 `현재 기준 SRS — 2026-09-20 교체`와 새 PDF를 우선한다.

### 출처 및 적용 범위

- 원문: `C:/Users/ohhoj/Downloads/e0881d9f-f35a-47e1-965f-dc9169b37eeb_SRS.pdf`.
- 문서명: 보안 WebCam 시스템 요구사항 문서(SRS), 문서 번호 SC-SRS-001, v1.1, 작성 기관 ㈜스마클, 표지 작성일 2026-08-03. 변경 이력에는 v1.1을 2026-09-12로 기재하며 본문에 9/13·9/15 추가 항목도 있다. 원문 보안 등급은 대외비.
- 보드 역할: RV-1106 기반 보드의 영상 촬영, 스트리밍, AI 감지, 로컬 저장(p.2-3). 현재 사용자 장비는 위 기록의 Luckfox Pico Zero + IMX415이다.
- 추출 범위: §3.1 웹캠 보드(p.4-7) 및 보드에 직접 적용되는 VPN·공통 요구사항(p.10, 20-22). 서버·DB·앱·웹 UI 자체 요구사항은 제외했다.
- 아래 검증 상태는 **SRS에 적힌 보고/평가**이며 이번 작업에서 보드를 재검증한 결과가 아니다. ✅에는 실측 완료뿐 아니라 구현 가능 판단도 섞여 있다. 문서의 작성 지시를 실행하거나 구현 상태를 임의로 승격하지 않았다.

### 보드 기능 요구사항 (§3.1, p.4-7)

| ID | 요구사항 | 우선순위 | SRS의 상태 및 남은 조건 |
| --- | --- | --- | --- |
| WC-F-001 | 최대 1080p(1920×1080) 실시간 스트리밍 | 필수 | ✅ 로컬망·AWS 시청 테스트, ⚠ 클라이언트 시청 테스트 필요. 보드·카메라 최대 3840×2160(8M) 지원이라는 비고가 있음. |
| WC-F-002 | H.265(HEVC) 스트리밍으로 저대역폭 대응 | 필수 | ✅ H.265 테스트 완료, 하드웨어 제약 없다고 기재. |
| WC-F-003 | 고화질·저화질 이중 스트림 동시 지원 | 필수 | ✅ 로컬망 테스트 완료라고 기재. |
| WC-F-004 | 30fps 이상 유지 | 필수 | ⚠ 조건부 가능 추정, 기능 추가 시 조정 필요. 8M 입력 15fps·영상 인코딩 5M 30fps라는 원문 비고는 현재 구성 실측값이 아님. |
| WC-F-010 | 지정 영역 움직임 감지 및 이벤트 생성 | 필수 | ✅ 스트리밍 중 감지 로그 확인. 영역·민감도·중복 억제·이벤트 데이터 생성 기준 정의 필요. |
| WC-F-011 | 임계값 설정 가능한 소리 감지 및 이벤트 생성 | 필수 | ⚠ 온보드 마이크 존재로 기재, 테스트 미착수. |
| WC-F-012 | 사람 감지 AI 분석 | 필수 | ⚠ NPU 1TOPS 기준 경량 사람 감지 실시간 추론 가능 추정. 실측 완료 아님. |
| WC-F-013 | 이벤트 발생 시 JPG 스냅샷 자동 저장 | 필수 | ✅ 소프트웨어 구현 영역이라는 평가. 테스트 완료 근거는 별도 기재 없음. |
| WC-F-014 | 이벤트 발생 전후 MP4 영상 클립 자동 저장 | 필수 | ✅ 소프트웨어 구현 영역이라는 평가. 전후 저장 시간·버퍼 정책 미정. |
| WC-F-015 | 프라이버시 존을 감지 및 녹화에서 제외 | 필수 | ⚠ 고정 영역 마스킹 샘플 존재, NPU 기반 동적 마스킹은 추가 개발 필요라는 비고. 동적 마스킹을 별도 확정 필수 요구로 확대 해석하지 않는다. |
| WC-F-020 | 이벤트 MP4·JPG 로컬 저장 | 필수 | ✅ 소프트웨어 구현 영역이라는 평가. |
| WC-F-021 | 용량 초과 시 오래된 파일 자동 삭제 | 필수 | ✅ 소프트웨어 구현 영역이라는 평가. |
| WC-F-022 | 보관 기간 설정, 기본 30일 | 필수 | ❌ 저장 용량 제약으로 표시. 8GB eMMC에서 1080p HEVC 약 9시간이라는 문서 추정, MicroSD 확장·기간 조정·서버/DB 처리 검토 필요. 비트레이트·녹화 방식에 따른 계산 근거는 원문에 없음. |
| WC-F-030 | 부팅 시 VPN 자동 연결 및 서버 프로비저닝 | 필수 | ✅ 부팅 시 WireGuard 인터페이스 설정·up 구현, 서버 자동 핸드셰이크 테스트 미착수라고 기재. 전체 요구 완료로 간주하지 않는다. |
| WC-F-031 | 30초마다 서버 heartbeat 전송 | 필수 | ✅ 실측 완료라고 기재. |
| WC-F-032 | 펌웨어 원격 업데이트 | 선택 | 검증 상태 미기재. |
| WC-F-033 | PTZ 원격 제어(상하좌우 회전·줌) | 필수 항목 구현 후 재검토 | ⚠ 디지털 PTZ(크롭 위치 이동·줌) 가능 추정. 원문 아래의 과거 표에는 필수로 남아 있음. 물리 회전 장치 구현 완료 의미 아님. |
| WC-F-040 | 영상 서비스 장애 감지 | 필수 | 검증 상태 미기재. |
| WC-F-041 | 영상 서비스 자동 복구 | 필수 | 검증 상태 미기재. |

### 보드 관련 VPN 연동 (§3.4, p.10)

- VPN-F-001(필수): WireGuard 터널 구성. SRS에는 보드↔중계서버 VPN 연결 성공으로 기재.
- VPN-F-002(필수): 보드 부팅 시 VPN 자동 연결. 자동 quick up 구현, 서버 프로비저닝 테스트 예정으로 기재.
- VPN-F-006(필수): 연결 끊김 시 자동 재연결. 원문 검증 상태 미기재.
- VPN-F-007(필수): 기기별 VPN IP 자동 할당·관리. 보드 적용을 위한 VPN/서버 연동 요구이며, 보드 단독 구현 책임으로 간주하지 않는다.

### 보드에 적용되는 비기능 요구사항 (§4, p.20-22)

| ID | 기준 | SRS의 상태 및 해석 |
| --- | --- | --- |
| NF-P-001 | 영상 스트리밍 지연 3초 이내 | ⚠ 보드→VPN 터널→Wi-Fi 환경 안드로이드 앱에서 약 1.8초, 로컬망 테스트라는 비고. 현재 Windows VLC 체감 지연과 측정 환경이 다름. |
| NF-P-002 | 이벤트 알림 전송 지연 5초 이내 | ⚠ 움직임 감지 로그는 즉시 출력, 객체 인식 등 전체 이벤트 지연은 추후 테스트. 서버·클라이언트까지 포함하는 연동 기준. |
| NF-P-003 | 보드 상태 갱신 25초 이내 | 수정 표에서 30초를 취소하고 25초로 변경. ✅ 근거에는 25초 VPN 터널 유지 테스트라고 기재. 이것이 상태 갱신이나 heartbeat 검증과 같은지는 확인 필요. |
| NF-S-001 | 모든 통신 구간 WireGuard 암호화(필수) | ✅ 보드↔중계서버 연결 및 Wireshark·libpcap 암호화 교차 검증 완료라고 기재. 모든 구간의 이번 재검증 의미는 아님. |
| NF-S-004 | 프라이버시 존 마스킹을 보드에서 처리(필수) | 검증 상태 미기재. |
| NF-S-006 | 보드↔중계서버↔클라이언트 각 구간 암호화 여부를 클라이언트에서 확인(선택, 9/13 추가) | ⚠ libpcap 분석 확인, 결과의 클라이언트 전달은 구현 가능 추정. 보드 측 분석·결과 제공 연동 범위에 관련. |
| NF-S-007 | ARIA-256-GCM 암호화(선택, 9/13 추가) | 초기 구성도에 있으나 SRS에 포함되지 않았던 내용이라는 비고. WireGuard 외 추가 암호화의 적용 위치·설계·검증은 미정. |
| NF-A-002 | VPN 단절 후 자동 재연결 60초 이내 | 검증 상태 미기재. |
| NF-A-003 | 보드 전원 복구 후 VPN 자동 연결 60초 이내 | 검증 상태 미기재. |
| NF-C-003 | 영상 파일 MP4(H.264/H.265) | ✅ H.264·H.265 스트리밍 검증이라고 기재. 스트리밍 검증만으로 MP4 저장 검증까지 완료한 것으로 보지 않는다. |
| NF-C-004 | JPG 스냅샷 1920×1080 | 검증 상태 미기재. |

### 향후 판단 시 유지할 구분

- SRS의 H.265·이중 스트림·VPN·heartbeat 검증 기록과 현재 `simple_vi_bind_venc_rtsp -b 4096` 단일 스트림 테스트를 구분한다. 현재 샘플 기본 코덱은 H.264이며 `-b`만으로 H.265가 되지 않는다.
- WC-F-031의 heartbeat 30초와 NF-P-003의 상태 갱신 25초는 서로 다른 표의 기준이다. VPN keepalive·heartbeat·상태 갱신을 동일 기능으로 간주하거나 주기를 임의 변경하지 않는다.
- WC-F-022의 30일 보관 요구는 삭제하지 않고 용량 제약이 있는 미해결 요구로 유지한다. 문서의 8GB·9시간 추정은 실제 저장 공간 및 연속/이벤트 녹화 정책을 확인한 뒤 재산정해야 한다.
- 원문 p.22 요약에는 웹캠 기능 15건으로 되어 있으나, §3.1의 고유 WC-F ID는 장애 감지·복구를 포함해 **19건**이다. 위 추출은 상세 표의 19건을 기준으로 했다.
- 현재 Wi-Fi/VLC 테스트만으로 SRS의 VPN 경유 지연·30fps·장시간 안정성·저장·자동 복구 등의 충족을 선언하지 않는다.

기록일: 2026-09-14 (Asia/Seoul)

## 후속 업데이트 (2026-09-14)

- 최종 무선 영상 확인: 사용자가 `rtsp://10.174.156.102:554/live/0`으로 핫스팟 경유 RTSP 영상 재생 성공을 보고했다. 아래 핫스팟 RTSP 실패/미해결 표기는 이전 이력이다. `.101` 접속 실패 원인과 이중 IP 생성 원인은 확정하지 않았다.
- 현재 성공 기준: rkipc 종료 후 `simple_vi_bind_venc_rtsp` 단독 실행, IMX415 IQ 경로 정상, USB 및 핫스팟 RTSP 재생 성공, 현재 화면의 오렌지색 편향 해소. `.102`는 앞선 출력에서 DHCP 동적 주소였으므로 재접속/재부팅 후 주소를 다시 확인한다. 자동 실행 및 재부팅 후 재현성은 아직 미확인.

- 최신 영상 확인: 사용자가 오렌지색 편향이 없어졌다고 보고했고 USB RTSP 재생 스크린샷에서도 강한 오렌지색 편향은 보이지 않았다. 현재 `simple_vi_bind_venc_rtsp` 실행 조건에서 색 편향 해소 및 실제 영상 수신 확인. 다른 조명 조건, RKIPC 실행 시 색상, 재부팅 후 재현성은 아직 미확인. 아래 색 편향 미해결 표기는 이전 이력이다.
- USB RTSP `rtsp://172.32.0.93:554/live/0` 재생 성공. 핫스팟 경유 영상은 실패 상태로, 핫스팟 접속 완료와 기기 간 RTSP 통신 성공을 구분한다. PC Wi-Fi는 `10.174.156.31/24`, 보드 wlan0은 `.101/24`와 `.102/24`; 원인은 아직 확정되지 않았다.
- 샘플 로그에서 AIQ init/prepare/start, ISP stream on, H.264 SPS/PPS, RTSP 554 및 `/live/0` 생성 확인. 최초 실행은 Ctrl+C(signal 2)로 종료된 뒤 포트 점검을 해서 리스너가 없었고, 이후 재실행하여 USB 영상을 확인했다.

- 사용자가 핫스팟 연결 완료를 보고했다. 아래 WPA 미연결 상태는 이전 이력이다. 최신 ps에는 wlan0의 DHCP/ARP 관련 프로세스가 보이지만 정확한 현재 IP는 별도 확인해야 한다.
- 온도 측정값은 56485~58861(약 56.5~58.9°C). 측정 간격과 장기 안정성은 미확인.
- 최신 `ps -ef`에 PID 637 `rkipc -a /oem/usr/share/iqfiles`가 실행 중임을 확인. 따라서 위 온도를 카메라 미실행 상태의 기본 온도로 해석하지 않는다. 앞선 단순 ps 검색의 빈 결과는 전체 프로세스를 반영하지 못했다.
- dmesg 마지막 노출 설정 기록은 5초 후에도 동일했다. 제공된 끝부분에는 과열/실패 메시지가 없지만 전체 하드웨어 정상이나 종료 문제 해결을 보장하지 않는다.
- 사용자는 상태가 괜찮다고 판단되면 카메라 설정으로 진행하길 요청했다. 다음 단계는 rkipc 점유를 해제하고 이전 성공 샘플로 영상 및 색 편향을 확인하는 것이다.

사용자가 아래 두 작업을 같은 CCTV 프로젝트의 구성 작업으로 지정했다. 이후 이 작업공간에서 CCTV를 진행할 때 카메라와 무선 네트워크 작업을 함께 참고한다. 이 문서는 참조 대화의 기록을 정리한 것으로, 현재 보드를 새로 검사한 결과는 아니다.

## 원본 작업

- Connect WPA to hotspot — thread://01a09e01-204a-7652-87f2-d40f8eb8745c?hostId=local
- 정리 imx415 연결 문제 해결 과정 — thread://01a09df3-6174-7233-bcda-9a9881ddd1b0?hostId=local

## IMX415 카메라 및 RTSP

- 이전 정리 기준 보드: Luckfox Pico Zero. 센서: `m00_b_imx415 4-0037`.
- 영상 경로: IMX415 → MIPI CSI → RKCIF → RKISP → VI → VENC → RTSP.
- 장치 기록: RKCIF `/dev/media0`, RKISP `/dev/media1`, ISP mainpath `/dev/video11`.
- IQ 파일: `/oem/usr/share/iqfiles/imx415_CMK-OT2022-PX1_IR0147-36IRC-8M-F20.json`.
- 초기 AIQ 실패 원인은 IQ 파일 경로 구조 오류였다. `/etc/iqfiles` 디렉터리 안에 `iqfiles` 링크가 중첩되어 프로그램이 JSON을 읽지 못했다.
- 기존 디렉터리를 `/etc/iqfiles.bak`으로 옮기고 `/etc/iqfiles → /oem/usr/share/iqfiles` 링크로 수정한 뒤 기본 경로에서도 AIQ init/prepare/start 성공이 기록됐다. 이는 과거 수행 내역이며 무조건 재실행할 명령이 아니다.
- RKIPC 설정 기록: `enable_aiq = 1`, `init_form_ini = 0`. JSON으로 초기화 성공했으며 이 장애 해결에 별도 BIN은 필요하지 않았다.
- Bayer 포맷은 센서부터 ISP까지 `SGBRG10_1X10`으로 일치. 센서 3864×2192, ISP 입력 3840×2160, 크롭 `(12,16)/3840x2160`.
- IR-CUT 장치 `m00_b_ircut`이 `/dev/v4l-subdev3`에 등록됨. `gpio36` 부재만으로 IR-CUT 고장을 단정하지 않는다.
- 최신 결과: 사용자가 rkipc를 종료하지 않았음을 확인했고, 종료 후 `/oem/usr/bin/simple_vi_bind_venc_rtsp` 단독 실행에서 AIQ 초기화, ISP 스트림, VI→VENC 연결, H.264 SPS/PPS 생성, RTSP 서버 시작 성공이 기록됐다.
- `invalid main scene len!` 이후에도 위 초기화가 성공하므로 이 메시지 자체를 치명적 실패로 취급하지 않는다. rkipc와 샘플의 동시 카메라 점유를 우선 확인한다.
- 샘플 RTSP: 포트 `554`, 경로 `/live/0`. 과거 IP `192.168.45.194`는 현재 주소로 보장되지 않는다.
- 미해결: 실제 영상의 오렌지색 편향 잔존 여부와 원인. AWB/CCM/IQ 모듈 적합성 및 IR-CUT은 확인 후보일 뿐 원인 확정 아님. 재부팅 후 IQ 링크 유지 및 펌웨어 반영도 미확인.

## WPA 핫스팟 연결

- 인터페이스 `wlan0`, 설정 파일 `/etc/wpa_supplicant.conf`, 제어 소켓 디렉터리 `/var/run/wpa_supplicant`.
- 대상 SSID `Wifi`. 스캔에서 2437 MHz(2.4 GHz), WPA2-PSK-CCMP, 신호 -57로 검색됨. 비밀번호는 기록하지 않는다.
- 설정 논의: `ctrl_interface=/var/run/wpa_supplicant`, `ap_scan=1`, `update_config=1`, `key_mgmt=WPA-PSK`. 일반 문자열 PSK에는 큰따옴표 사용.
- 중복 실행 시 제어 소켓 충돌 발생. 기존 wpa_cli 응답이 있으면 실행 중인 인스턴스를 확인하고 reconfigure/reassociate를 사용하며 소켓을 함부로 삭제하지 않는다.
- 최신 네트워크 결과: reconfigure/reassociate는 OK지만 `wpa_state=SCANNING` 지속, 네트워크 0 `Wifi`가 `[TEMP-DISABLED]`. 원인은 아직 미확정.
- `udhcpc`는 discover를 반복했으며 인증 완료(`COMPLETED`)나 IP 할당 성공 기록이 없다. wpa_supplicant 실행 성공을 Wi-Fi 접속 성공으로 보지 않는다.
- 미수행 후속 점검: wpa_cli 이벤트의 `CTRL-EVENT-SSID-TEMP-DISABLED ... reason=...` 확인으로 실패 원인 구분.

## 가장 최근 보드 상태 및 다음 작업의 전제

- 핫스팟 작업 말미에 사용자가 심한 발열과 reboot/poweroff 무응답을 보고했다.
- 당시 답변은 전원 공급 연결을 모두 분리하고 식힌 뒤 모델·전원 방식/전압·부팅 로그를 확인하는 것이었다. 실제 분리 수행, 보드 회복, 원인 및 손상 여부는 확인되지 않았다.
- 후속 하드웨어 작업은 보드 상태 확인을 먼저 진행한다. 카메라 스트림 초기화 성공과 Wi-Fi 연결 실패, 색 편향 미확인을 구분해서 이어간다.


## 2026-11-12 I1/I2 integration continuation
- User confirmed rkipc terminated, no rkipc/cctv/rkaiq process in ps.
- Sensor currently 3864x2192 MEDIA_BUS_FMT_SGBRG10_1X10 (0x300e); keep this mode, output target20fps on both paths.
- Full OEM RkLunch received: S21appinit calls RkLunch, which runs post_chk asynchronously. Preserve module/network/config initialization. Launch handoff must occur at the original rkipc launch point, not merely by init-script numbering.
- Prepared marker-gated OEM handoff, S59 service, dual-frame health helper, backup/install/rollback scripts in deliverables/i2_board.
- Heartbeat default process cctv-dual-rtsp, wg0 binding, destination 10.0.0.100:3000/v1/devices/heartbeat. rtsp_ok remains process-only; frame health is separate.
- Native libcurl client loopback tests passed: dry-run no traffic,403->200 retry, JSON schema, missing interface no fallback. Health helper rejects stale/empty/wrong PID/recovering/malformed state. Shell syntax and OEM patch recognizer passed.
- Do not operate user's board or VM directly. User executes adb and board commands. Board deployment, reboot, actual20fps and server/MediaMTX verification remain pending.

- Final build: ARM streamer and heartbeat succeeded. ARM heartbeat qemu loopback tests passed; ELF NEEDED only libc.so.0 (libcurl statically linked). Streamer QEMU hits SIGSEGV during vendor hardware probing after missing Rockchip efuse/device-tree; no board runtime success claimed. Package packages/i2-board-20261112 and tar.gz prepared with SHA256SUMS.


## 2026-11-13 모듈화 및 검증
- 사용자 요청으로 streaming/heartbeat를 기능별 .c/.h로 분리. 실행 옵션·20fps 출력·5초 heartbeat·wg0 바인딩·초기화/정리 순서 유지.
- streaming: src/main,config,pipeline,isp,video,rtsp_server,status,runtime. 기존 dual_rtsp.c는 src 모듈로 대체. heartbeat: 기존 main/protocol에 platform,transport,runtime,config 분리.
- 기존 소스와 바이너리는 artifacts/refactor-20261113에 백업. 기존 packages/i2-board-20261112 유지, 새 패키지는 packages/i2-board-20261113-modular 및 tar.gz.
- 검증 통과: streamer ARM -Wall/-Wextra/-Werror 빌드, CLI 옵션/재시도 파싱/진단 조합/프로파일 회귀 검사, heartbeat native/ARM 빌드, 기존 protocol 테스트, native 및 qemu-arm 실제 HTTP 403→200/JSON/dry-run/없는 인터페이스 우회 금지, 상태 검사, 패키지 SHA256 검사.
- 보드나 VM에 직접 접속하지 않음. 새 모듈화 바이너리는 보드에서 실행 미검증. 이전 사용자가 보낸 11/12 로그는 약2분간 두 스트림 20.14~20.58fps 및 Ctrl+C 정리를 확인한 것으로, 새 바이너리 검증과 구분.
- 모듈별 역할/변경 지점/빌드 명령은 docs/MODULE_GUIDE.md. 기존 서버503 및 I2 통합 검증 미완료 상태는 이번 모듈화로 해결된 것이 아님.


## 2026-11-13 보드 시험: 미해결 장애

사용자가 영구 설치 후 S59cctv running, health의 양쪽 최근 프레임, S60heartbeat running을 보고했습니다. 재부팅 직후 명령 실행 여부는 설치 대화 흐름에 따른 보고이며 독립 재검증하지 않았습니다. heartbeat 실행은 실서버 HTTP200을 입증하지 않습니다.

모듈화 버전에서 양쪽 약19.5~19.8fps 로그와 프레임 카운터 증가를 확인했습니다. VPN으로 live/1을 받는 VLC에서 demux 데이터·디코딩·표시 카운터가 증가했지만 사용자는 화면 정지를 보고했습니다. 영상 내용이 실제로 갱신되는지는 미확인입니다.

이후 state=failed, attempt=3, cctv stopped 확인. 로그에 NO_FRAME과 재시도, 커널에 rkisp_stream_stop 및 rkisp_isp_sd_s_stream timeout이 반복됩니다. 재초기화 후 영상 복구 실패는 확인됐고, 최초 원인은 미확정입니다. VLC 종료나 VPN을 원인으로 단정하지 않습니다. 종료 이후 WireGuard handshake 응답도 기록됐습니다.

다음 시험: 실패 로그를 보관하고 재부팅 후 VLC 미접속 상태에서 프레임·health 유지 확인, 이어 접속/종료 전후 비교. 해당 시험 결과는 아직 수신하지 않았습니다. 이 커밋은 개발 진행 상태이며 안정화 완료본이 아닙니다.
