FROM ubuntu:18.04

RUN apt update && apt install -y  --no-install-recommends  \
        build-essential \
        make \
        cmake \
        libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/src/gtest \
    && cmake CMakeLists.txt \
    && make \
    && cp *.a /usr/lib
