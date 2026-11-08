#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
model_file=${MODEL_FILE:-$project_dir/model/yolov5.rknn}
streaming=${BUILD_STREAMING:-ON}
if [ "$#" -gt 1 ]; then
    cmake -S "$project_dir" -B "$project_dir/build" \
        -DVENDOR_ROOT="$vendor_dir" -DCROSS_COMPILE="$2" \
        -DMODEL_FILE="$model_file" -DBUILD_STREAMING="$streaming" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$project_dir/dist"
else
    cmake -S "$project_dir" -B "$project_dir/build" \
        -DVENDOR_ROOT="$vendor_dir" \
        -DMODEL_FILE="$model_file" -DBUILD_STREAMING="$streaming" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$project_dir/dist"
fi
cmake --build "$project_dir/build" --parallel 2
cmake --build "$project_dir/build" --target install
printf '\nBoard package: %s/dist\n' "$project_dir"
