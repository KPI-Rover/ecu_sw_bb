#######################################################
# Dockerfile for the Beaglebone Blue cross-compilation
#######################################################
# Multi-Target Dockerfile for x86 & ARM Cross-Compilation
#######################################################

FROM debian:buster

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies (x86 and ARM toolchains)
RUN apt-get update -q && apt-get install -y -q \
    gcc g++ \
    gcc-8-arm-linux-gnueabihf \
    g++-8-arm-linux-gnueabihf \
    cmake \
    build-essential \
    wget \
    unzip \
    lcov \
    gcovr \
    python3 \
    libgtest-dev && \
    rm -rf /var/lib/apt/lists/*  # Clean up APT cache to reduce image size

# Build GoogleTest manually (since libgtest-dev only provides source code)
RUN cd /usr/src/googletest && \
    mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install

# Set working directory
WORKDIR /work
