#!/bin/sh

sudo apt update
sudo apt upgrade

sudo apt -y --allow-unauthenticated install libyaml-cpp-dev libyaml-doc

#Debian puede que no use la nueva version del api de libyaml-cpp, si falla compilacion mejor instalar como modulo desde codigo fuente

# Install Open CV - Warning, this takes absolutely forever
RUN cd ~ && git clone https://github.com/Itseez/opencv.git && \ 
    cd opencv && \
    git checkout 2.4.13.5 && \
    cd ~ && git clone https://github.com/Itseez/opencv_contrib.git && \
    cd opencv_contrib && \
    git checkout 2.4.13.5 && \
    cd ~/opencv && mkdir -p build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \ 
    -D INSTALL_C_EXAMPLES=ON \ 
    -D INSTALL_PYTHON_EXAMPLES=ON \ 
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \ 
    -D BUILD_EXAMPLES=OFF .. && \
    make -j4 && \
    make install && \ 
    ldconfig

