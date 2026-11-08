# 공식 RKMPI 헤더·라이브러리를 사용하는 RV1106/uClibc 교차 빌드 설정.
# include, src, model만 참조하며 실행파일과 모델을 설치한다.
cmake_minimum_required(VERSION 3.10)
# 실시간 프로그램과 사진 프로그램의 생성 여부. 삭제한 tests 폴더와는 무관하다.
option(BUILD_STREAMING "Also build IMX415 RTSP integration (requires MPI/AIQ)" ON)
option(BUILD_IMAGE_TEST "Build image test when OpenCV image IO is available" ON)
# VM용 실행파일 대신 보드용 ARM/uClibc 실행파일을 만든다.
# 컴파일러는 CROSS_COMPILE → GCC_COMPILER → LUCKFOX_SDK_PATH → PATH 순서로 찾는다.
# CMake가 컴파일러를 검사하는 project()보다 먼저 설정해야 한다.
set(CROSS_COMPILE "" CACHE STRING "ARM uClibc compiler prefix, including trailing hyphen")
if(NOT CROSS_COMPILE AND DEFINED ENV{GCC_COMPILER})
    string(REGEX REPLACE "-$" "" _compiler_prefix "$ENV{GCC_COMPILER}")
    set(CROSS_COMPILE "${_compiler_prefix}-")
endif()
if(NOT CROSS_COMPILE AND DEFINED ENV{LUCKFOX_SDK_PATH})
    set(_prefix "$ENV{LUCKFOX_SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-")
    if(EXISTS "${_prefix}g++")
        set(CROSS_COMPILE "${_prefix}")
    endif()
endif()
if(NOT CROSS_COMPILE)
    find_program(_compiler arm-rockchip830-linux-uclibcgnueabihf-g++)
    if(_compiler)
        string(REGEX REPLACE "g\\+\\+$" "" CROSS_COMPILE "${_compiler}")
    endif()
endif()
if(NOT CROSS_COMPILE)
    message(FATAL_ERROR "Set LUCKFOX_SDK_PATH to the SDK (not RKMPI example), or -DCROSS_COMPILE=/path/arm-rockchip830-linux-uclibcgnueabihf-")
endif()
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "ARM compiler does not exist: ${CMAKE_CXX_COMPILER}")
endif()
project(cctv_person_npu LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VENDOR_ROOT "" CACHE PATH "Complete luckfox_pico_rkmpi_example directory")
# SDK/라이브러리는 외부 경로에서 사용하고 모델만 dist/model로 복사한다.
set(MODEL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/model/yolov5.rknn" CACHE FILEPATH "RV1106 INT8 YOLOv5 model to package")
if(NOT EXISTS "${MODEL_FILE}")
    message(FATAL_ERROR "MODEL_FILE missing: place the RV1106 RKNN model in model/yolov5.rknn or set -DMODEL_FILE=/path/to/model.rknn")
endif()
if(NOT EXISTS "${VENDOR_ROOT}/include/rknn/rknn_api.h")
    message(FATAL_ERROR "VENDOR_ROOT must contain include/rknn/rknn_api.h")
endif()
# This target uses the Buildroot/uClibc firmware ABI, like the supplied sample.
set(VENDOR_LIB "${VENDOR_ROOT}/lib/uclibc")
set(OpenCV_DIR "${VENDOR_LIB}/lib/cmake/opencv4")
# 축소 OpenCV의 highgui에 이미지 입출력이 있을 수 있어 imgcodecs를 필수로 강제하지 않는다.
# [출처] 프로젝트 환경 호환 처리. VM의 축소 OpenCV에서 imgcodecs가 없었던 문제에 맞춰 선택 컴포넌트와 후속 컴파일 검사를 구성했다.
find_package(OpenCV REQUIRED COMPONENTS core imgproc OPTIONAL_COMPONENTS imgcodecs highgui)
# Vendor headers may live outside the prefix recorded in OpenCVConfig.cmake.
find_path(CCTV_OPENCV_INCLUDE opencv2/core.hpp
    PATHS "${VENDOR_ROOT}/include" "${VENDOR_ROOT}/include/opencv4"
          "${VENDOR_LIB}/include/opencv4" ${OpenCV_INCLUDE_DIRS}
    NO_DEFAULT_PATH)
if(NOT CCTV_OPENCV_INCLUDE)
    message(FATAL_ERROR "OpenCV headers missing: locate opencv2/core.hpp and set -DCCTV_OPENCV_INCLUDE to its parent include directory")
endif()
list(APPEND OpenCV_INCLUDE_DIRS "${CCTV_OPENCV_INCLUDE}")
if(BUILD_IMAGE_TEST)
    # imread/imwrite를 쓸 수 있는 헤더를 컴파일·링크로 확인한다.
    # 시험 코드를 보드에서 실행하는 검사는 아니다. 사진 기능에 필요한 호환성 검사다.
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES ${OpenCV_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${OpenCV_LIBS})
    set(CMAKE_REQUIRED_FLAGS "-Wl,-rpath-link,${VENDOR_LIB}:${VENDOR_LIB}/lib")
    # Some reduced OpenCV builds expose image IO through highgui instead.
    foreach(_header imgcodecs.hpp highgui.hpp highgui/highgui.hpp)
        string(MAKE_C_IDENTIFIER "CCTV_IMAGE_IO_${_header}" _probe)
        unset(${_probe} CACHE)
        check_cxx_source_compiles("#include <opencv2/${_header}>\nint main() { cv::Mat m = cv::imread(\"probe.jpg\", 1); return cv::imwrite(\"probe-out.jpg\", m) ? 0 : 1; }" ${_probe})
        if(${_probe})
            set(CCTV_IMAGE_IO_HEADER "opencv2/${_header}")
            break()
        endif()
    endforeach()
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_FLAGS)
    if(NOT CCTV_IMAGE_IO_HEADER)
        if(NOT BUILD_STREAMING)
            message(FATAL_ERROR "Image test requested alone, but OpenCV imread/imwrite compile/link checks failed. See CMakeError.log")
        endif()
        message(WARNING "OpenCV image IO unavailable: cctv_person_image skipped; RTSP/NPU target remains enabled. See CMakeError.log for image IO checks.")
        set(BUILD_IMAGE_TEST OFF)
    else()
        message(STATUS "Image IO header: ${CCTV_IMAGE_IO_HEADER}")
    endif()
endif()
find_package(Threads REQUIRED)
# NPU 실행과 사람 후처리를 공통 정적 라이브러리로 묶어 두 프로그램에서 공유한다.
add_library(person_inference STATIC src/npu_model.cc src/person_decode.cc)
# SDK 헤더의 RV1106/AIQ 분기 및 큰 파일 관련 ABI 설정을 선택한다.
target_compile_definitions(person_inference PUBLIC RV1106_1103 RKAIQ UAPI2
    _LARGEFILE_SOURCE _LARGEFILE64_SOURCE _FILE_OFFSET_BITS=64)
target_compile_options(person_inference PRIVATE -Wall -Wextra -O2)
# [출처] 외부 SDK/예제의 헤더 경로를 참조할 뿐, 해당 라이브러리를 이 프로젝트에서 구현한 것은 아니다.
target_include_directories(person_inference PUBLIC include ${OpenCV_INCLUDE_DIRS}
    "${VENDOR_ROOT}/include" "${VENDOR_ROOT}/include/rknn"
    "${VENDOR_ROOT}/include/rkaiq" "${VENDOR_ROOT}/include/rkaiq/uAPI2"
    "${VENDOR_ROOT}/include/rkaiq/common" "${VENDOR_ROOT}/include/rkaiq/xcore"
    "${VENDOR_ROOT}/include/rkaiq/algos" "${VENDOR_ROOT}/include/rkaiq/iq_parser"
    "${VENDOR_ROOT}/include/rkaiq/iq_parser_v2")
set(_libraries rknnmrt)
# 사진 프로그램에는 RKNN/OpenCV를, 실시간 프로그램에는 MPI·인코더·AIQ·RTSP 등도 연결한다.
# NO_DEFAULT_PATH로 VM의 x86 라이브러리가 잘못 선택되는 것을 피한다.
if(BUILD_STREAMING)
    list(APPEND _libraries rockit rockchip_mpp rkaiq rtsp rga)
endif()
foreach(name ${_libraries})
    find_library(LIB_${name} NAMES ${name} PATHS "${VENDOR_LIB}" "${VENDOR_LIB}/lib" NO_DEFAULT_PATH)
    if(NOT LIB_${name})
        message(FATAL_ERROR "Missing ARM library ${name} in ${VENDOR_LIB}; use the complete vendor repository matching the firmware")
    endif()
endforeach()
target_link_libraries(person_inference PUBLIC "${LIB_rknnmrt}" ${OpenCV_LIBS} Threads::Threads m dl)
set(_targets)
# image_main.cc는 사진용, main.cc는 카메라/스트리밍용 독립 실행 진입점이다.
if(BUILD_IMAGE_TEST)
    add_executable(cctv_person_image src/image_main.cc)
    target_compile_definitions(cctv_person_image PRIVATE CCTV_IMAGE_IO_HEADER="${CCTV_IMAGE_IO_HEADER}")
    target_link_libraries(cctv_person_image PRIVATE person_inference)
    list(APPEND _targets cctv_person_image)
endif()
if(BUILD_STREAMING)
    add_executable(cctv_person_npu src/main.cc)
    # -P 고정 사진 비교 옵션도 사진 시험에서 검증한 이미지 IO 헤더를 사용한다.
    if(CCTV_IMAGE_IO_HEADER)
        target_compile_definitions(cctv_person_npu PRIVATE CCTV_IMAGE_IO_HEADER="${CCTV_IMAGE_IO_HEADER}")
    endif()
    target_link_libraries(cctv_person_npu PRIVATE person_inference
        "${LIB_rockit}" "${LIB_rockchip_mpp}" "${LIB_rkaiq}" "${LIB_rtsp}" "${LIB_rga}")
    list(APPEND _targets cctv_person_npu)
endif()
foreach(target ${_targets})
    # 컴파일 경고와 최적화를 적용한다. rpath-link는 링크 시 의존 라이브러리 검색 경로다.
    # INSTALL_RPATH는 설치 후 실행파일 옆 lib를 검색하게 하지만 라이브러리를 복사하지는 않는다.
    target_compile_options(${target} PRIVATE -Wall -Wextra -O2)
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
    set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS
        " -Wl,-rpath-link,${VENDOR_LIB}:${VENDOR_LIB}/lib")
endforeach()
install(TARGETS ${_targets} RUNTIME DESTINATION .)
# build.sh가 설치 경로를 dist로 지정한다. 생성된 실행파일과 모델만 이곳에 모은다.
install(FILES "${MODEL_FILE}" DESTINATION model RENAME yolov5.rknn)
# Board firmware libraries are deliberately not overwritten. Only stage model + executable.
