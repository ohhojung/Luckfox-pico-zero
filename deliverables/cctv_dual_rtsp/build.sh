#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
sdk_dir=${LUCKFOX_SDK_PATH:-/home/student/luckfox-live/luckfox-pico}
toolchain=arm-rockchip830-linux-uclibcgnueabihf
compiler=${2:-$sdk_dir/tools/linux/toolchain/$toolchain/bin/$toolchain-gcc}
vendor_lib="$vendor_dir/lib/uclibc"

[ -x "$compiler" ] || { echo "ARM compiler missing: $compiler" >&2; exit 1; }
[ -f "$vendor_dir/include/rk_mpi_vi.h" ] && [ -d "$vendor_lib" ] || {
    echo "RKMPI headers or uClibc libraries missing: $vendor_dir" >&2
    exit 1
}

mkdir -p "$project_dir/dist"
"$compiler" -std=gnu11 -O2 -Wall -Wextra -Werror -pthread -DRKAIQ -DRV1106_RV1103 -DUAPI2 \
    -I"$vendor_dir/include" \
    -I"$vendor_dir/include/rkaiq" -I"$vendor_dir/include/rkaiq/uAPI2" \
    -I"$vendor_dir/include/rkaiq/common" -I"$vendor_dir/include/rkaiq/xcore" \
    -I"$vendor_dir/include/rkaiq/algos" -I"$vendor_dir/include/rkaiq/iq_parser" \
    -I"$vendor_dir/include/rkaiq/iq_parser_v2" \
    "$project_dir/src/"*.c \
    -L"$vendor_lib" -L"$vendor_lib/lib" \
    -Wl,-rpath-link,"$vendor_lib" -Wl,-rpath-link,"$vendor_lib/lib" \
    -lrockit -lrockchip_mpp -lrkaiq -lrtsp -lrga -lstdc++ -lm -ldl \
    -o "$project_dir/dist/cctv-dual-rtsp"
printf 'Built %s/dist/cctv-dual-rtsp\n' "$project_dir"
