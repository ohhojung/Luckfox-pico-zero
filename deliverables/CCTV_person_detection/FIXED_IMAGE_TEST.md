# 고정 사진으로 카메라 활성 영향 비교하기

## 현재까지의 결과

별도 사진 프로그램은 23~24개 검출, 약 98~106ms로 성공했다. 통합 프로그램에서 사진만 입력하는 `-P`와 보조 VI만 유지하는 `-S -P`는 모두 task counter 19에서 NPU timeout이 발생했다. 이 두 옵션은 ARM 빌드와 보드 실행을 완료했지만 안정 동작에는 실패했다.

카메라 미초기화 비교에는 `-N -P` 모드를 사용했다. 카메라를 초기화하지 않고, 통합 프로그램과 동일하게 주 스레드에서 모델을 열고 별도 추론 스레드 하나와 최신 프레임 슬롯을 사용한다. **-N 보드 시험에서 최소 12회 추론 성공 후 task 97 timeout을 확인했다. 직후 별도 사진은 재부팅 없이 성공했다. 다음 비교는 THREAD_REPEAT_TEST.md의 새 -N -M 모드다.**

| 모드 | 사진 입력 | 카메라/AIQ | VI | 인코더/RTSP |
|---|---|---|---|---|
| `-P person.jpg` | 고정 사진 | 활성 | 주 VI + AI VI | 활성 |
| `-S -P person.jpg` | 고정 사진 | 활성 | AI VI만 | 비활성 |
| `-N -P person.jpg` | 고정 사진 | 이 프로그램에서 시작하지 않음 | 없음 | 비활성 |

`-N`은 `-P`가 필수이며 `-D` 또는 `-S`와 함께 사용할 수 없다. 다른 프로세스의 카메라를 자동으로 정지시키지는 않는다. 이 기능은 프로젝트에서 작성한 진단 기능이다.

## VM에서 빌드

새 패키지의 code 내용을 VM 작업 폴더에 반영한다. build.sh가 있는 폴더에서 실행한다.

```sh
export LUCKFOX_SDK_PATH=/home/student/luckfox-live/luckfox-pico
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

build.sh는 LF 줄바꿈으로 제공한다. 기본 BUILD_IMAGE_TEST=ON에서 선택한 OpenCV 이미지 IO 헤더를 실시간 프로그램에도 전달한다. 이미지 IO가 없는 빌드는 -P 및 -N 사진 시험을 시작 오류로 종료한다.

## 사용자가 보드로 전송

VM의 dist/cctv_person_npu를 Windows로 가져온다. Windows 터미널에서 그 파일이 있는 폴더로 이동한 뒤 아래 명령으로 새 이름을 사용한다. 모델은 기존 파일을 그대로 사용한다.

```bat
adb push cctv_person_npu /oem/usr/bin/cctv_person_npu_demo/cctv_person_npu_no_camera
```

이 문서의 보드 전송·실행은 사용자가 수행한다. 기존 실행 파일이나 드라이버를 교체하는 과정은 없다.

## 보드에서 시험

실행 중인 테스트 스트리머는 해당 터미널에서 Ctrl+C로 종료한다. 기본 카메라 서비스가 실행 중이면 종료하고 ps 출력에서 rkipc 및 다른 카메라 프로그램이 남아 있지 않은지 확인한다. 종료 중이거나 자동 재시작되면 해결한 후 비교한다.

```sh
killall rkipc 2>/dev/null
ps -ef
cd /oem/usr/bin/cctv_person_npu_demo
chmod +x cctv_person_npu_no_camera
./cctv_person_npu_no_camera -h
```

도움말에 -N이 있는지 확인한다. 이어 다음을 실행한다.

```sh
cat /proc/uptime
./cctv_person_npu_no_camera -N -P person.jpg -f 5 -t 0.5
```

약 20초 동안 관찰하고 Ctrl+C로 종료한다. 실패하면 자동 재시도 없이 종료한다. 동기 rknn_run 실행 중에는 Ctrl+C 후에도 드라이버 호출 복귀까지 기다릴 수 있다.

확인할 로그:

- `FIXED_IMAGE: ... VI not initialized`
- `READY NO_CAMERA: ... camera/AIQ/VI/VENC/RTSP not initialized`
- 반복되는 `PERSON ... source=fixed-image ... completed=...`

completed는 성공한 추론의 누적 횟수다. PERSON 로그는 매 추론마다 나오지 않을 수 있으며 대략 1초 간격 또는 검출 개수가 변할 때 출력된다. `-f 5`는 사진 선택 상한이며 실제 추론 완료율 보장은 아니다. 밀린 입력을 쌓지 않고 슬롯의 오래된 입력을 교체한다.

이 모드에서는 VIDEO/RTSP 송출이 없다. pts_us=0은 카메라 PTS가 없다는 표시다. result_age_ms는 사진 선택부터 결과 완료까지의 시간이며, detect_ms는 전처리·추론·후처리 시간이다. box_image는 사진 좌표다.

시험 종료 후 아래 출력을 함께 보관한다. 오류 로그는 실행 전 uptime과 비교해 이전 실행의 timeout을 구분한다.

```sh
cat /proc/uptime
dmesg | grep RKNPU | tail -n 40
```

## 결과 해석

- 반복 성공: 이 조건에서는 같은 초기화 스레드/추론 스레드/슬롯으로 동작한다. 기존 -S -P와 비교하여 카메라·ISP·VI 활성에 따른 자원 사용과 초기화 영향부터 좁힌다. 스레드 구현 전체가 무조건 안전하다는 증명은 아니다.
- timeout: 카메라가 실제로 정지했는지와 최근 드라이버 상태를 확인하고, 같은 상태에서 별도 사진 프로그램과 비교한다. 스레드·반복 실행·메모리 배치·런타임 등을 계속 조사한다. 이것만으로 스레드 버그라고 단정하지 않는다.

카메라를 생략하면 메모리 사용과 실행 타이밍도 달라진다. 어느 결과도 특정 하드웨어나 드라이버를 원인으로 확정하지 않는다.
