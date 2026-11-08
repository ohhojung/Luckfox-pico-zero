cmake_minimum_required(VERSION 3.10)
# 비교 프로그램 전용. 제조사 코드와 라이브러리는 외부 VENDOR_ROOT에서 읽는다.
set(BUILD_IMAGE_TEST ON)
set(BUILD_STREAMING OFF)
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

project(vendor_photo_compare LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VENDOR_ROOT "" CACHE PATH "Existing luckfox_pico_rkmpi_example")
set(EXAMPLE_DIR "${VENDOR_ROOT}/example/luckfox_pico_rtsp_yolov5")
foreach(_file src/yolov5.cc src/postprocess.cc model/coco_80_labels_list.txt)
    if(NOT EXISTS "${EXAMPLE_DIR}/${_file}")
        message(FATAL_ERROR "Missing vendor file ${EXAMPLE_DIR}/${_file}")
    endif()
endforeach()
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
find_library(RKNN_LIBRARY NAMES rknnmrt PATHS "${VENDOR_LIB}" "${VENDOR_LIB}/lib" NO_DEFAULT_PATH)
find_path(YOLO_INCLUDE_DIR yolov5.h PATHS
    "${EXAMPLE_DIR}/include" "${EXAMPLE_DIR}/src" "${VENDOR_ROOT}/include/rknn"
    "${VENDOR_ROOT}/include" "${VENDOR_ROOT}/common" "${VENDOR_ROOT}/utils" NO_DEFAULT_PATH)
if(NOT YOLO_INCLUDE_DIR OR NOT RKNN_LIBRARY)
    message(FATAL_ERROR "Vendor yolov5.h or librknnmrt missing. Set YOLO_INCLUDE_DIR to the directory containing the vendor header.")
endif()
add_executable(vendor_person_image src/photo_main.cc
    "${EXAMPLE_DIR}/src/yolov5.cc" "${EXAMPLE_DIR}/src/postprocess.cc")
target_compile_definitions(vendor_person_image PRIVATE RV1106_1103
    _LARGEFILE_SOURCE _LARGEFILE64_SOURCE _FILE_OFFSET_BITS=64
    CCTV_IMAGE_IO_HEADER="${CCTV_IMAGE_IO_HEADER}")
target_include_directories(vendor_person_image PRIVATE "${YOLO_INCLUDE_DIR}"
    "${EXAMPLE_DIR}/include" "${VENDOR_ROOT}" "${VENDOR_ROOT}/include"
    "${VENDOR_ROOT}/include/rknn" "${VENDOR_ROOT}/common" "${VENDOR_ROOT}/utils"
    ${OpenCV_INCLUDE_DIRS})
target_link_libraries(vendor_person_image PRIVATE "${RKNN_LIBRARY}" ${OpenCV_LIBS} Threads::Threads m dl)
set_target_properties(vendor_person_image PROPERTIES INSTALL_RPATH "$ORIGIN/lib"
    LINK_FLAGS "-Wl,-rpath-link,${VENDOR_LIB}:${VENDOR_LIB}/lib")
install(TARGETS vendor_person_image RUNTIME DESTINATION .)
install(FILES "${EXAMPLE_DIR}/model/coco_80_labels_list.txt" DESTINATION model)
# 실제 VM에서 사용한 출처와 라이브러리 해시를 남긴다. 모델/런타임은 교체하지 않는다.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/vendor_manifest.txt" "Custom photo harness with unmodified external vendor sources.\n")
foreach(_file "${EXAMPLE_DIR}/src/yolov5.cc" "${EXAMPLE_DIR}/src/postprocess.cc"
              "${YOLO_INCLUDE_DIR}/yolov5.h" "${RKNN_LIBRARY}")
    file(SHA256 "${_file}" _hash)
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/vendor_manifest.txt" "${_hash}  ${_file}\n")
endforeach()
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vendor_manifest.txt" DESTINATION .)
