#!/bin/sh
# 공식 SDK 소스는 수정하지 않고 별도 build/dist에 비교 실행파일을 만든다.
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
export LUCKFOX_SDK_PATH=${LUCKFOX_SDK_PATH:-/home/student/luckfox-live/luckfox-pico}
cmake -S "$project_dir" -B "$project_dir/build" \
    -DVENDOR_ROOT="$vendor_dir" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$project_dir/dist"
cmake --build "$project_dir/build" --parallel 2
cmake --build "$project_dir/build" --target install
printf '\nBoard executable: %s/dist/vendor_person_image\n' "$project_dir"
