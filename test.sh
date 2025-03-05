#!/bin/bash
set -e

# Check if the "nocache" argument is passed
if [[ "$1" == "nocache" ]]; then
    echo "-- Clearing CMake cache..."
    rm -rf build/*
fi

BUILD_VARIANT="GTests"
TARGET_ARCH="x86"
TARGET_OPTIONS="-DBUILD_VARIANT=${BUILD_VARIANT}"

docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/work \
    kpi-rover-bbb \
    bash -c "cmake -B build/${TARGET_ARCH} -H. ${TARGET_OPTIONS} && \
            cmake --build build/${TARGET_ARCH} -- -j$(nproc) && \
            cd build/${TARGET_ARCH} && ctest --output-on-failure"
