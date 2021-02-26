#!/bin/sh

sudo apt update
sudo apt upgrade

sudo apt -y --allow-unauthenticated install libopencv-video-dev libcv-dev libcv2.4 libhighgui-dev libhighgui2.4 libopencv-core-dev libopencv-dev libopencv-highgui-dev \
                                            libopencv-imgproc-dev libopencv-photo-dev libopencv-stitching-dev libopencv-ts-dev opencv-data libopencv-calib3d-dev \
                                            libopencv-contrib-dev libopencv-features2d-dev libopencv-gpu-dev libopencv-ml-dev libopencv-objdetect-dev \
					    libgtk2.0-dev
                                            #libyaml-cpp-dev libyaml-doc

#Debian puede que no use la nueva version del api de libyaml-cpp, si falla compilacion mejor instalar como modulo desde codigo fuente
cd /tmp/
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build
cd build
#cmake -DBUILD_SHARED_LIBS=ON ..
cmake ..
make -j 4
sudo make install
