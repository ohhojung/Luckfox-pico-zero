# RV1106 NPU 진행 로그 진단

목적: timeout 때의 단일 카운터만 보지 않고 대기 중 약1초 간격으로 task 상태/IRQ/raw/실제 interrupt mask/요구 mask를 비교한다. 해결 패치가 아닌 관측 패치이다.

원본 소스 SHA256: 1235ce135a3e0a46e7339b929d8893ac67366e82261263877540fec61d886f3c

변경: rknpu_job.c만 변경. 동기 작업의 대기를1초 이하 구간으로 나누되 각 원래 wait의 절대 마감시각을 유지한다. 기존 wait_count/continue_wait, 완료판정, reset 및 레지스터 쓰기 경로는 유지한다. IRQ_LOCK 안에서 작은 레지스터 스냅샷을 복사하고 출력은 lock 밖에서 수행한다. RV1106 단일코어 레지스터 구성에만 적용한다. progress_diag 모듈 파라미터는 기본 true이며 false일 때 원래 wait 호출로 돌아간다. 폴링으로 강제 성공처리하지 않는다.

주의: 추가 wakeup, MMIO read, printk가 타이밍에 영향을 줄 수 있다. 해당 진단 모듈로 성공해도 근본 해결이라고 판단하지 않는다. reset 동시 동작을 포함한 실기 안전성/ARM빌드는 아직 검증하지 않았다. 원본 모듈은 보존해야 한다.

## VM에서 소스 적용

```sh
cd /home/student/npu-progress-diag
sh apply.sh check
sh apply.sh apply
```

적용 전에 원본해시를 확인하고 rknpu_job.c.before-progress-diag를 보존한다. 다른 수정이 있으면 덮어쓰지 않는다.

## VM에서 모듈만 빌드

```sh
cd /home/student/luckfox-live/luckfox-pico
make -C sysdrv/source/kernel \
  O=/home/student/luckfox-live/luckfox-pico/sysdrv/source/objs_kernel \
  ARCH=arm \
  CROSS_COMPILE=/home/student/luckfox-live/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf- \
  M=drivers/rknpu modules -j2 > /home/student/npu-progress-build.log 2>&1
build_status=$?
cat /home/student/npu-progress-build.log
echo "BUILD_EXIT=$build_status"
```

현재 모듈용 빌드 설정/산출물이 준비된 기존 SDK를 사용한다. 빌드 결과와 새 sysdrv/source/objs_kernel/drivers/rknpu/rknpu.ko를 검토한 다음 보드 배포를 결정한다. 아직 insmod/rmmod, 모듈 덮어쓰기, reboot 또는 boot flash를 하지 않는다. 이번 단계는 VM 소스 적용/컴파일까지만이다.

## 복원

`sh apply.sh restore`는 VM 소스만 복원한다. 이미 빌드한 모듈이나 보드 파일은 바뀌지 않는다. 원본 모듈 복원은 별도 절차이다.
