# 주 스레드와 작업 스레드의 반복 추론 비교

## 비교 목적과 현재 결과

카메라를 시작하지 않는 기존 -N 모드에서 최소 12회 추론 성공 후 task counter 97 timeout이 발생했다. 그 직후 재부팅 없이 별도 사진 프로그램은 23개/97.8ms로 성공했다. 따라서 실패 뒤 NPU가 항상 계속 사용 불능이 되는 것은 아니다. 기존 커널 job 2001a502는 -N 실패 기록이며 이후 사진의 새 오류가 아니다.

단발 사진 성공과 반복 추론 실패만으로 작업 스레드가 원인이라고 할 수 없다. 새 -M 옵션은 같은 프로그램에서 주 스레드가 모델을 한 번 초기화한 뒤 반복 추론하도록 한다. 카메라/AIQ/VI/VENC/RTSP를 시작하지 않는다.

| 명령 옵션 | 모델 초기화 | 추론 실행 | 모델/입출력 버퍼 |
|---|---|---|---|
| -N -P person.jpg | 주 스레드 | 작업 스레드 하나 | 한 번 할당 후 재사용 |
| -N -M -P person.jpg | 주 스레드 | 주 스레드 | 한 번 할당 후 재사용 |

두 모드 모두 같은 InferenceRunner.step, NpuModel.infer, 사진 clone, 최신 프레임 슬롯 및 -f 선택 상한을 사용한다. -M에서는 슬롯에 넣은 사진을 주 스레드가 곧바로 꺼내 처리한다. -M 없이만 작업 스레드를 만든다. 주 스레드 추론 중에는 다음 사진을 준비할 수 없으므로 생산 타이밍과 메모리 수명 차이는 남는다. 이를 완전히 한 변수만 바뀌는 실험이라고 해석하지 않는다.

## 빌드 및 배포

새 패키지의 code 내용을 VM에 반영하고 build.sh가 있는 폴더에서 실행한다.

```sh
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

dist/cctv_person_npu를 Windows로 가져온 뒤 파일이 있는 폴더에서 새 이름으로 전송한다. 기존 바이너리와 모델·드라이버는 보존한다.

```bat
adb push cctv_person_npu /oem/usr/bin/cctv_person_npu_demo/cctv_person_npu_thread_compare
```

보드 전송과 시험은 사용자가 수행한다. -M은 -N -P와 함께만 사용할 수 있다. 카메라 송출 모드에서 주 스레드 추론을 강제로 사용하는 옵션이 아니다.

## 주 스레드 시험

기존 카메라/스트리밍 앱이 종료됐고 사진 단발 추론이 성공한 상태에서 시작한다. 그 사이 재부팅했다면 rkipc 종료를 확인하고 사진 프로그램을 먼저 실행해 기준을 다시 확인한다.

```sh
cd /oem/usr/bin/cctv_person_npu_demo
chmod +x cctv_person_npu_thread_compare
./cctv_person_npu_thread_compare -h
cat /proc/uptime
./cctv_person_npu_thread_compare -N -M -P person.jpg -f 5 -t 0.5
```

READY에 inference_thread=main이 표시되어야 한다. 약 20초 동안 PERSON completed 증가 여부를 관찰한 뒤 Ctrl+C로 종료한다. 오류 발생 시 자동 종료하며, INFERENCE_STOP completed는 오류 이전에 성공한 추론의 정확한 누적 횟수다. 이 횟수는 RKNN task counter와 다르다. 성공 종료 전 PERSON 로그는 출력 간격 때문에 최종 횟수보다 적을 수 있다.

```sh
cat /proc/uptime
dmesg | grep RKNPU | tail -n 40
```

## 작업 스레드 비교

주 스레드 시험이 정상 종료되면 같은 바이너리에서 -M만 빼고 같은 시간 동안 비교한다. 실패했다면 커널 로그를 먼저 보관하고 사진 단발 실행으로 현재 상태를 확인한다. 사진도 실패하면 재부팅 후 정상 기준을 복구한 뒤 비교하며 재부팅 사실을 기록한다.

```sh
./cctv_person_npu_thread_compare -N -P person.jpg -f 5 -t 0.5
```

이 경우 READY에 inference_thread=worker가 표시된다. 20초 후 Ctrl+C 또는 자동 오류 종료 뒤 uptime과 dmesg를 보관한다. 기존 no_camera 바이너리와 직접 비교하는 것보다 새 바이너리의 -M 유무를 비교하면 코드 버전 차이를 줄일 수 있다.

## 해석과 검증 범위

- 두 모드 모두 실패: 작업 스레드로 넘기는 것만이 필수 원인은 아니다. 반복 실행, 메모리 접근, 런타임·드라이버 등의 영향을 더 분리한다.
- 주 스레드만 성공: 스레드 전환과 슬롯 생산 타이밍·메모리 수명 차이가 조사 대상이다. 한 번의 성공으로 원인을 확정하지 않는다.
- 둘 다 성공: 이번 시간 동안은 재현되지 않은 결과다. 전체 스트리밍 문제 해결 또는 장시간 안정성 검증은 아니다.

새 공통 처리 코드를 추출해 호스트에서 모의 모델로 주 스레드 100회 호출, 작업 스레드 호출 위치, 오류 후 재시도 금지, 오류와 영상 종료 플래그 분리, 대기 중 종료를 검사했다. 실제 OpenCV/RKNN·ARM 전체 빌드나 보드 하드웨어 검증은 아니며 새 -M 모드는 보드 시험 전이다. 모델과 NpuModel의 전처리·NPU API·후처리는 변경하지 않았다.
