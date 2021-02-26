#!/bin/sh

#cd ~ && git clone https://github.com/Itseez/opencv.git && \ 
cd ~ && git clone https://github.com/Itseez/opencv.git && \ 
    cd opencv && \
    git checkout 2.4.13.5 && \
    mkdir -p build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \ 
    -D INSTALL_C_EXAMPLES=ON \ 
    -D INSTALL_PYTHON_EXAMPLES=ON \ 
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \ 
    -D BUILD_EXAMPLES=OFF .. && \
    make -j4 && \
    make install && \ 
    ldconfig

