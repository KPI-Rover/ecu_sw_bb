#!/bin/bash

# Ensure build directory exists
mkdir -p build/target

# Check if the "nocache" argument is passed
if [[ "$1" == "nocache" ]]; then
    echo "-- Clearing CMake cache..."
    rm -rf build/target/*
fi

# Run the Docker container with CMake commands
docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-build \
    bash -c "cmake -B build/target -H. && cmake --build build/target -- -j$(nproc)"
