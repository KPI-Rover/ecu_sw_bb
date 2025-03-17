#!/bin/bash
set -e

BUILD_VARIANT="Tests"

# Ensure build directory exists
mkdir -p build/${BUILD_VARIANT}

# Check if the "nocache" argument is passed
if [[ "$1" == "nocache" ]]; then
    echo "-- Clearing CMake cache..."
    rm -rf build/${BUILD_VARIANT}/*
fi

docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-check \
    bash -c "export CC=/usr/bin/gcc && \
        export CXX=/usr/bin/g++ && \
        export LD=/usr/bin/ld && \
        export AR=/usr/bin/ar && \
        mkdir -p build/${BUILD_VARIANT} && cd build/${BUILD_VARIANT} && \
        cmake ../.. -DBUILD_VARIANT=${BUILD_VARIANT} \
            -DCMAKE_CXX_FLAGS='-O0 -g --coverage -fprofile-arcs -ftest-coverage' \
            -DCMAKE_EXE_LINKER_FLAGS='--coverage' \
        && make \
        && make -s coverage "

echo '=============================================================================================='
echo To open coverage report: xdg-open build/${BUILD_VARIANT}/coverage_report/index.html
echo '=============================================================================================='