#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
if [ "${1:-}" = "--native" ]; then
    compiler=${CC:-cc}
    output_dir="$project_dir/build/native"
else
    compiler=${CC:-arm-rockchip830-linux-uclibcgnueabihf-gcc}
    if ! command -v "$compiler" >/dev/null 2>&1; then
        compiler="${LUCKFOX_SDK_PATH:-/home/student/luckfox-live/luckfox-pico}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc"
    fi
    output_dir="$project_dir/dist"
fi
if ! command -v "$compiler" >/dev/null 2>&1; then
    printf 'Compiler not found: %s\nSet CC to the full path of the target compiler.\n' "$compiler" >&2
    exit 1
fi
mkdir -p "$output_dir"
# CURL_PREFIX must be the target staging/usr, never the host Ubuntu /usr.
if [ -n "${CURL_PREFIX:-}" ]; then
    set -- "-I$CURL_PREFIX/include" "-L$CURL_PREFIX/lib" "-Wl,-rpath-link,$CURL_PREFIX/lib"
else
    set --
fi
"$compiler" -std=c99 -O2 -Wall -Wextra -Werror \
    "$@" "$project_dir/src/"*.c -lcurl -lrt \
    -o "$output_dir/heartbeat"
printf 'Built: %s/heartbeat\nThe program sends by default. Use --dry-run to print the payload.\n' "$output_dir"
