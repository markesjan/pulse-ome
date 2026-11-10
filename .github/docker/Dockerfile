FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    ca-certificates \
    git \
    ninja-build \
    cmake \
    clang-format \
    && add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
    gcc-15 \
    g++-15 \
    && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-15
ENV CXX=g++-15
