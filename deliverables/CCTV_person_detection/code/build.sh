#!/bin/sh
# [출처] 아래 출처는 원본과의 API·알고리즘 대응을 나타낸다. 파일 전체가 원본의 복사본이라는 의미는 아니다.
# [출처] 프로젝트의 CMake configure/build/install을 묶은 스크립트. 특정 공식 build.sh를 복사한 것으로 확인되지 않았다.
# [출처] SDK 및 RKMPI 경로와 ARM 컴파일러는 외부 빌드 환경이며, 인자/출력 dist 구성은 이 프로젝트의 선택이다.
# [출처] 정확한 대조본 SHA256, 분류 기준과 모델 출처는 같은 프로젝트의 SOURCE_MAP.md 참조.
# Ubuntu VM에서 ARM 교차 빌드한다. 첫 인자는 RKMPI 예제 경로, 두 번째는 선택적 컴파일러 접두사다.
set -eu
# 실패한 명령이나 미정의 변수에서 중단한다. 작업 경로는 스크립트 위치를 기준으로 한다.
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
vendor_dir=${1:-/home/student/RKMPI/luckfox_pico_rkmpi_example}
model_file=${MODEL_FILE:-$project_dir/model/yolov5.rknn}
streaming=${BUILD_STREAMING:-ON}
# 1단계: CMake가 SDK/컴파일러/소스를 확인하고 build에 빌드 규칙을 생성한다.
# -S는 소스 폴더, -B는 빌드 폴더, -D는 CMake 설정값이다.
# 두 번째 인자가 있으면 컴파일러 접두사를 직접 전달하고 없으면 CMake의 탐색을 사용한다.
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
# 빌드 후 실행 파일·모델을 dist에 설치한다. 보드 전송이나 실행은 하지 않는다.
# 2단계: 생성된 빌드 규칙에 따라 최대 2개 작업을 병렬로 컴파일·링크한다.
cmake --build "$project_dir/build" --parallel 2
# 3단계: install 규칙으로 결과물을 dist에 모은다. VM 시스템 디렉터리에 설치하는 것이 아니다.
cmake --build "$project_dir/build" --target install
printf '\nBoard package: %s/dist\n' "$project_dir"
