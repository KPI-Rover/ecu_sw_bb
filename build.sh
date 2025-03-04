#!/bin/bash

# Check if the "nocache" argument is passed
if [[ "$1" == "nocache" ]]; then
    echo "-- Clearing CMake cache..."
    rm -rf build/*
fi

# Run the Docker container with CMake commands
docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/work \
    kpi-rover-bbb \
    bash -c "cmake -B build -H. && cmake --build build -- -j$(nproc)"
