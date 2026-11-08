#!/bin/sh
# Read-only report. Run in the Ubuntu VM, not on the camera board.
set -u
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
printf 'VM architecture: '; uname -m
printf 'VM working directory: '; pwd
printf 'Vendor root: %s\n' "$vendor_dir"
printf 'LUCKFOX_SDK_PATH: %s\n' "${LUCKFOX_SDK_PATH:-not set}"
printf 'GCC_COMPILER: %s\n' "${GCC_COMPILER:-not set}"
printf '\nTools:\n'
for tool in cmake make python3 arm-rockchip830-linux-uclibcgnueabihf-g++; do
    command -v "$tool" || true
done
printf '\nVendor files:\n'
for item in include/rknn/rknn_api.h lib/uclibc/librknnmrt.so lib/uclibc/lib/cmake/opencv4/OpenCVConfig.cmake; do
    if [ -f "$vendor_dir/$item" ]; then printf 'OK %s\n' "$item"; else printf 'MISSING %s\n' "$item"; fi
done
if [ -n "${LUCKFOX_SDK_PATH:-}" ]; then
    compiler="$LUCKFOX_SDK_PATH/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++"
    if [ -x "$compiler" ]; then "$compiler" --version | head -n 1; else printf 'SDK compiler not found: %s\n' "$compiler"; fi
fi
printf '\nActive Python / Toolkit2 (no installation):\n'
python3 -c 'import sys; print(sys.version); print(sys.executable); from importlib.metadata import version; print("rknn-toolkit2=" + version("rknn-toolkit2")); from rknn.api import RKNN; print("RKNN import OK")' 2>&1 || true
