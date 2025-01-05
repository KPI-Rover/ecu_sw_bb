#######################################################
# Dockerfile for the Beaglebone blue cross compilation
#######################################################

FROM debian:buster

RUN apt-get update -q && \
    DEBIAN_FRONTEND=noninteractive && \
    apt-get install -y -q \
    gcc-8-arm-linux-gnueabihf \
    g++-8-arm-linux-gnueabihf \
    cmake

RUN ln -s /usr/bin/arm-linux-gnueabihf-g++-8 /usr/bin/arm-linux-gnueabihf-g++ && \
    ln -s /usr/bin/arm-linux-gnueabihf-gcc-8 /usr/bin/arm-linux-gnueabihf-gcc && \
    ln -s /usr/bin/arm-linux-gnueabihf-cpp-8 /usr/bin/arm-linux-gnueabihf-cpp
    
ENV AS=/usr/bin/arm-linux-gnueabihf-as \
    AR=/usr/bin/arm-linux-gnueabihf-ar \
    CC=/usr/bin/arm-linux-gnueabihf-gcc \
    CPP=/usr/bin/arm-linux-gnueabihf-cpp \
    CXX=/usr/bin/arm-linux-gnueabihf-g++ \
    LD=/usr/bin/arm-linux-gnueabihf-ld

    RUN echo "alias ll='ls -alF --color=auto'" >> /etc/bash.bashrc

WORKDIR /build
