# revision6: VI 프레임레이트 기본값 대조

제조사 example/vi/sample_multi_vi.c의 VI 설정(317~318행)에 따라 stFrameRate.s32SrcFrameRate와 s32DstFrameRate를 모두 -1로 명시한다. 이전 v5는 memset의 0/0이 그대로 남아 illegal param 경고가 발생했다. 이것이 20fps의 원인이라는 증거는 아직 없으며 이번 시험으로 확인한다.

다른 조건은 v5 -B2 시험과 동일하다. -F30은 기존 AIQ 요청이며 VI -1/-1과 별개이다. VI 설정값 자체는 실측 FPS가 아니다.

VM: sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
보드 전송 시 기존 파일을 보존하도록 simple_vi_bind_venc_rtsp_fps_v6 이름을 쓴다.

```sh
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps_v6 -V -B 2 -w 1920 -h 1080 -F 30 -c 400
```

VI_RATE_CONFIG, illegal param 경고 유무, VI_MEASURED, VI_TEST_DONE을 비교한다. 여전히20fps라면 이 경고 수정으로 문제가 해결되지 않는 것이며 VI 버퍼/FPS 옵션 반복 대신 ISP unite/readback 분석으로 진행한다.

로컬 v5 대비 소스 차이와 ZIP 구성 확인만 완료했다. VM ARM 빌드 및 보드 실행은 사용자 수행 대기 중이다.

---

# revision5: VI 버퍼 수 비교

-B 2 또는 -B 4로 VI 버퍼 개수만 바꾼다. 기본2, depth1(-V)은 동일하다. 센서/IQ/클록은 변경하지 않는다.

같은 revision5 실행파일로 다음 두 시험을 순서대로 실행한다. 각 시험은 400프레임 후 종료한다.
```sh
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps_v5 -V -B 2 -w 1920 -h 1080 -F 30 -c 400
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps_v5 -V -B 4 -w 1920 -h 1080 -F 30 -c 400
```

VI_CONFIG의 requested_buffers는 요청값이다. 실제 적용과 출력 속도는 실행 중 다른 셸의 `cat /proc/rkisp-unite0` 및 VI_MEASURED로 확인한다. 메모리 할당 오류가 있으면 해당 조건의 FPS 비교는 성립하지 않는다. 다음 시험 전에 첫 시험이 종료되어야 한다.

revision5는 로컬 소스 검토/ZIP 확인만 완료, ARM 빌드/보드 검증 전이다. 보드 명령은 사용자만 실행한다. 아래 revision4 준비 당시 기록 이후, 실기에서 1080p와540p 모두 약20.2fps가 확인되었다.

---

# revision4: VI 단독 FPS 비교

`-V` 옵션을 추가했다. VENC/RTSP 없이 VI 프레임을 즉시 반환하며 5초마다 `VI_MEASURED`를 출력한다. VLC 연결용 모드가 아니다. 기존 옵션은 아래 설명을 참고한다.

VM에서:
```sh
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

보드에서 다른 카메라 앱을 종료한 다음:
```sh
./simple_vi_bind_venc_rtsp_fps -V -w 1920 -h 1080 -F 30 -c 600
```

600프레임 후 종료하며, 10초간 취득하지 못하면 오류 종료한다. 실행 중 다른 보드 셸에서 다음 통계도 확인한다.
```sh
cat /proc/rkcif-mipi-lvds
cat /proc/rkisp-unite0
```

버퍼 개수는 기존2, VI depth는 사용자 취득에 필요한1이다. 프레임 복사/인코딩/송출은 하지 않는다. `-V`를 빼면 기존 스트리밍 경로로 비교할 수 있다. 소스 검토만 완료했고 ARM 빌드와 실기 검증은 대기 중이다.

---

# AIQ FPS comparison for Luckfox Pico Zero + IMX415

This is a separate copy of simple_test/simple_vi_bind_venc_rtsp.c.
The original program and NPU programs are unchanged.

## Revision 3: AE result readback (2026-10-01)

Adds AE_STATE (enable, mode, run interval) and AE_RESULT (AIQ-reported FPS,
frame lines, line pixels, pixel clock MHz) using queryExpResInfo. These are
AIQ-reported values, not physical timing measurements; compare with MEASURED
and the sensor VBLANK query. Readbacks also run without -F, so IQ-only testing
is observable. No AE settings are written when -F is omitted.
The supplied RV1106 SDK headers declare the fields/API and the supplied ARM
shared library exports the query function. Board library identity is unverified.
The -a argument is now required: both -a/tmp and -a /tmp work.
Sensor mode, VI rate, encoder rate and firmware are unchanged.
ARM build and board execution of revision 3 remain to be tested.

First run revision 3 with the original IQ and -F15, then Ctrl+C and test the
modified IQ without -F. Keep the illumination and client connection unchanged:

```sh
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps -w 1920 -h 1080 -b 4096 -F 15
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps -a/tmp -w 1920 -h 1080 -b 4096
```

These commands are separate runs, not concurrent. Capture 20 seconds per run.

## Revision 2: live FPS request diagnostics (2026-10-01)

Baseline, -F30 and -F15 all measured about 20 FPS on the board with revision 1.
Revision 2 reads back AE settings before VI startup and during streaming.
With -F, about two seconds after observing the first SOF callback, the main
thread reapplies the request once, then reads the settings every five seconds.
Look for FPS_DIAG revision=2, set_ret and fixed/value as well as MEASURED.
Readback is configuration only, not proof of a changed sensor frame rate.
Run -F15 first for at least 20 seconds; the first measurement includes the
transition. No encoder/VI rate, sensor mode, kernel or IQ changes in revision 2.
The existing VI 0/0 warning is retained to isolate the request timing change.
Revision 2 has not yet been ARM-built or board-tested.

## Build in the Ubuntu VM

Extract isp_fps_test into /home/student, then:

```sh
cd /home/student/isp_fps_test
sh build.sh /home/student/RKMPI/luckfox_pico_rkmpi_example
```

Optional second argument: absolute ARM uClibc gcc path.
Copy dist/simple_vi_bind_venc_rtsp_fps to Windows, then use adb push to
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps on the board.
Do not replace the existing streaming executable.

## Compare on the board

Stop other camera applications first. Use the same scene, illumination,
sensor mode, bitrate, VLC settings and network route for both runs.

```sh
chmod +x /oem/usr/bin/simple_vi_bind_venc_rtsp_fps
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps -w 1920 -h 1080 -b 4096
```

Wait for at least three MEASURED lines, then Ctrl+C. Next:

```sh
/oem/usr/bin/simple_vi_bind_venc_rtsp_fps -w 1920 -h 1080 -b 4096 -F 30
```

Same RTSP path: rtsp://CURRENT_BOARD_WIFI_IP:554/live/0.
While running, capture /proc/rkisp* and the sensor get-subdev-fmt output as well.

## What changes

- New -F 1..30 requests fixed FPS through AIQ AE stAuto.stFrmRate,
  using the API pattern in example/common/isp3.x/sample_comm_isp.c.
- Omission preserves the existing AIQ policy (baseline).
- Encoder source/destination FPS fields are not changed.
- Every ~5 seconds: aiq_sof_fps counts AIQ metadata callbacks and
  venc_dequeue_fps counts successfully dequeued encoded frames over elapsed
  monotonic time. Callback rate is not a calibrated physical sensor FPS.
  Encoder count does not detect duplicate pictures or prove VLC display FPS.
  RTSP send blocking can affect the dequeue rate; compare identical clients.
- AIQ request errors stop startup rather than silently claiming success.
- Stream wait timeout reduced to 1 second so Ctrl+C need not wait 200 seconds.

## Interpretation and limitations

AIQ acceptance is not proof that the requested FPS is achieved. Existing sensor
mode, exposure requirements and ISP processing limits still apply. Fixing FPS
may shorten allowable exposure and change brightness/noise. The program does
not select a smaller sensor mode or modify firmware, clocks, or IQ JSON files.
The 1944x1097 12-bit sensor mode remains a separate investigation.

ARM compilation, linking and board operation require validation in the VM/board.
Local source review and archive integrity checks do not replace that validation.
