# Imagen base ubuntu 18.04
FROM ubuntu:18.04

# Mantainer  jose-lp: joselp1695@gmail.com
LABEL maintainer="jose-lp"

# Instalación de paquetes
RUN apt update && apt install -y  --no-install-recommends  \
        build-essential \
        make \
        cmake \
        libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Configuracipon de gtest
RUN cd /usr/src/gtest \
    && cmake CMakeLists.txt \
    && make \
    && cp *.a /usr/lib
