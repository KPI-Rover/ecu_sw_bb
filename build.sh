#!/bin/bash



BUILD_VARIANT="Target"

TARGET_OPTIONS="-DBUILD_VARIANT=${BUILD_VARIANT}"

# Ensure build directory exists
mkdir -p build/${BUILD_VARIANT}

# Check if the "nocache" argument is passed
if [[ "$1" == "nocache" ]]; then
    echo "-- Clearing CMake cache..."
    rm -rf build/${BUILD_VARIANT}/*
fi


# Run the Docker container with CMake commands
docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-build \
    bash -c "cmake -B build/${BUILD_VARIANT} -H. -H. ${TARGET_OPTIONS} \
        && cmake --build build/${BUILD_VARIANT} -- -j$(nproc)"
