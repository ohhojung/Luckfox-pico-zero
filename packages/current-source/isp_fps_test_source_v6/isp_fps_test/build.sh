#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
compiler=${2:-/home/student/luckfox-live/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc}
[ -x "$compiler" ] || { echo "ARM compiler missing: $compiler" >&2; exit 1; }
[ -f "$vendor_dir/include/rkaiq/uAPI2/rk_aiq_user_api2_ae.h" ] || {
    echo "Missing AIQ AE header in vendor directory: $vendor_dir" >&2; exit 1;
}
mkdir -p "$project_dir/dist"
"$compiler" -std=gnu11 -O2 -Wall -pthread -DRKAIQ -DRV1106_RV1103 -DUAPI2 \
    -I"$vendor_dir/include" \
    -I"$vendor_dir/include/rkaiq" -I"$vendor_dir/include/rkaiq/uAPI2" \
    -I"$vendor_dir/include/rkaiq/common" -I"$vendor_dir/include/rkaiq/xcore" \
    -I"$vendor_dir/include/rkaiq/algos" -I"$vendor_dir/include/rkaiq/iq_parser" \
    -I"$vendor_dir/include/rkaiq/iq_parser_v2" \
    "$project_dir/simple_vi_bind_venc_rtsp_fps.c" \
    -L"$vendor_dir/lib/uclibc" -L"$vendor_dir/lib/uclibc/lib" \
    -Wl,-rpath-link,"$vendor_dir/lib/uclibc" \
    -Wl,-rpath-link,"$vendor_dir/lib/uclibc/lib" \
    -lrockit -lrockchip_mpp -lrkaiq -lrtsp -lrga -lstdc++ -lm -ldl \
    -o "$project_dir/dist/simple_vi_bind_venc_rtsp_fps"
echo "Built: $project_dir/dist/simple_vi_bind_venc_rtsp_fps"
