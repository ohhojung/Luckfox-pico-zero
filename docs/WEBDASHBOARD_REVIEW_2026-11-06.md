# WebDashboard 코드 검토

확인일: 2026-11-06. 저장소 https://github.com/moonjuyoung727/WebDashboard
기준 main 커밋: 6e3dc54091173e3dec250c54b5cdd4be4c229ec1 (2026-11-04, update for Iteration1 test).
로컬 검토본: references/WebDashboard-review. 읽기 전용 검토이며 코드 수정/의존성 설치/빌드/브라우저 및 실서버 호출은 수행하지 않았다. 배포본이 이 커밋과 같은지는 미확인.

## 구조와 데이터 흐름

React19 + React Router7 + react-scripts5 기반 프런트엔드. src/pages는 로그인/회원가입, 대시보드, 기기관리, 멀티뷰, 모니터링, 이벤트, 리포트, VPN 및 설정 화면을 구성한다. 서버의 heartbeat 수신·DB·WireGuard 설정 코드는 이 저장소에 없다.

보드 → POST /v1/devices/heartbeat → 별도 서버의 last_seen/status 갱신 → 웹 GET /v1/devices + Bearer JWT → 화면 표시.
웹은 heartbeat payload를 직접 받지 않고 서버의 보드 목록 응답을 읽는다. cameraApi.toCamera는 device_id/mac/name/location/status/last_seen/firmware_version/vpn_status/vpn_ip/latest_event_time/thumbnail_url을 매핑한다. rtsp_ok/wg_ok 자체를 화면에서 직접 표시하는 매핑은 없다. offline15초 계산은 서버 반환 status에 의존하며 프런트에서 last_seen으로 재계산하지 않는다.

## 실제 API 연결 코드

- src/api/authApi.js: POST /auth/login, /auth/register, /auth/logout. 로그인 우회 코드는 주석 처리돼 있다.
- src/api/client.js: API base 연결, Bearer 토큰, 401 세션정리, error_code/message/details 해석 및 네트워크 오류 안내.
- src/api/cameraApi.js: GET /devices에5초 제한시간. registerCamera 함수는 존재하지만 현재 DeviceManage 화면에서 호출하지 않는다.
- src/hooks/useCameraList.js: 기기관리/멀티뷰/모니터링 기본 조회. 응답완료 후5초 대기하는 구조이므로 정확한 요청시작5초간격은 아니다. 숨긴탭은조회생략.
- src/utils/mac.js: 대문자 콜론없는MAC, CAM-끝4자리 생성. 현재 보드7424CA5004AF와 형식일치.
- src/utils/formatDateTime.js: 서버UTC를 Asia/Seoul로 표시, last_seen null은기록없음. 보드시간에9시간을더할필요없음.

## 연동 우선 확인사항

1. 서버 주소 불일치: .env4행 REACT_APP_API_BASE_URL=http://10.0.0.1:3000/v1. 최신프로젝트기록의 서버VPN10.0.0.100과다름. 현재운영서버기준이면환경변수를 http://10.0.0.100:3000/v1로 맞추고 개발서버재시작/배포재빌드가 필요. VPN_SERVER_IP 상수도10.0.0.1이나 API요청은환경변수사용. 이번검토에서변경하지않음.
2. 메인Dashboard.jsx46행은 DASHBOARD_POLL_MS=10000. I1의클라이언트5초조회와차이. 기타화면도응답시간+5초이므로최대화면반영20초를엄격히보장하는구조는아님.
3. 첫서버연결실패시예시보드목록, 이후실패시마지막목록유지. CameraSourceBadge로mock/stale구분되나예시online을실제연동으로오인하지않아야함. source=server 및실제last_seen갱신확인필요.
4. DeviceManage.jsx의등록/삭제/이름변경은setCameras로화면만갱신. 등록API호출은주석. 다음서버조회에서원래값으로돌아갈수있음. 팀원의수동DB등록성공과웹등록기능완료는별개.
5. VpnManage.jsx19행 VPN_API_ENABLED=false. 보드VPN관리표는목업. 로그인후 /status를통한클라이언트VPN조회와구분. /status는서버의추가계약으로connected/client_ip필요. client_ip는10.0.2.x를기대하며IPv4-mapped IPv6접두어를프런트에서정규화하지않음.
6. useWebRtcStream은WHEP SDP POST/DELETE 코드가있으나cameraApi.toCamera는webrtcUrl/lowResStreamUrl을보존하지않음. Monitoring은webrtcUrl,Multiview는lowResStreamUrl이필요하여현재목록경로만으로실제영상URL이전달되지않음. 영상은I1생존상태표시와별도후속연동.
7. apiFetch 공통타임아웃은없음. 보드목록/VPN목록은개별5초제한이있으나로그인/가입/클라이언트VPN /status는명시적제한이없어무응답시화면대기가길어질수있음.
8. useCameraList의모듈전역shared는사용자별키나로그아웃초기화가없음. 같은탭에서계정변경시이전목록이새조회전또는실패시남을수있으므로후속검증/수정대상. 코드상판단이며브라우저재현미실행.

## 목업/후속 구현 범위

이벤트/통계/리포트/일부설정에src/data의목업사용이남아있다. API파일의함수존재/화면존재를서버구현및실제연동완료로해석하지않는다. I1외기능미연결자체는I1실패를의미하지않음.

## 다음 통합 확인

서버주소확정 → 클라이언트VPN10.0.2.x와서버/status계약확인 → 실제로그인토큰 → GET /v1/devices 응답과source=server확인 → 보드last_seen갱신 → 보드전원차단/복구에따른offline/online 실측. 보드heartbeat형식은이번웹검토때문에변경할필요없음.
