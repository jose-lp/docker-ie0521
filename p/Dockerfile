
FROM ubuntu:16.04 as builder

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt full-upgrade -y && \
    apt install -f -y --no-install-recommends \
    build-essential \
    make \
    curl \
    wget \
    cmake \ 
    ca-certificates \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/*
    
RUN apt update && apt full-upgrade -y && \
    apt install -f -y --no-install-recommends \
    python-dev \
    python-numpy \
    libcanberra-gtk-module \
    libcv-dev \
    libcv2.4 \
    libhighgui2.4 \
    opencv-data \
    libgtk2.0-dev \
    flex \
    autoconf \
    automake \
    libtool \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/*

#########################
# Instalación de OpenCV #
#########################

WORKDIR /tmp

RUN curl -LO https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/cuda-repo-ubuntu1604_8.0.61-1_amd64.deb \
    && dpkg -i cuda-repo-ubuntu1604_8.0.61-1_amd64.deb \
    && apt update && apt full-upgrade -y \
    && apt install -f -y --no-install-recommends \
	cuda \
	nvidia-cuda-toolkit \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/* \
    && rm -rf /tmp/*
      
###########################
# Instalación de yaml-cpp #
###########################

RUN cd /tmp \
	&& curl -LO https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.6.0.tar.gz \
	&& tar -xf yaml-cpp-0.6.0.tar.gz \
	&& cd ./yaml-cpp-yaml-cpp-0.6.0 && mkdir build && cd build \
	&& cmake -D CMAKE_BUILD_TYPE=RELEASE \
    	   -D CMAKE_INSTALL_PREFIX=/usr/local ..\
	&& make -j8 \
	&& make install \
	&& rm -rf /tmp/*

############################
# Instalación de Boost c++ #
############################

RUN cd /tmp \ 
	&& curl -LO https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz \ 
	&& tar -xf ./boost_1_67_0.tar.gz \
	&& cd boost_1_67_0/ \
	&& ./bootstrap.sh --prefix=/usr/local \
	&& ./b2 \
	&& ./b2 install \
	&& rm -rf /tmp/*

###########################
# Instalación de OpenMPI  #
###########################

RUN cd /tmp \
	&& curl -LO https://github.com/open-mpi/ompi/archive/v2.1.3.tar.gz \
	&& tar -xf v2.1.3.tar.gz \
	&& cd ompi-2.1.3 \
	&& ./autogen.pl \
	&& ./configure --prefix=/usr/local \
	&& make all install \
	&& rm -rf /tmp/*
		
#########################
# Instalación de OpenCV #
#########################

RUN cd /tmp \
	&& curl -LO https://github.com/opencv/opencv/archive/2.4.13.5.tar.gz \
	&& tar -xf  2.4.13.5.tar.gz \
	&& cd ./opencv-2.4.13.5 \
	&& sed -i '/set(NVCC_FLAGS_EXTRA ${NVCC_FLAGS_EXTRA} -gencode arch=compute_${CMAKE_MATCH_2},code=sm_${CMAKE_MATCH_1}/c\set(NVCC_FLAGS_EXTRA ${NVCC_FLAGS_EXTRA} -D_FORCE_INLINES -gencode arch=compute_${CMAKE_MATCH_2},code=sm_${CMAKE_MATCH_1})' ./cmake/OpenCVDetectCUDA.cmake \
	&& mkdir build && cd build \
	&& cmake -D CMAKE_BUILD_TYPE=Release \
	   -D CMAKE_INSTALL_PREFIX=/usr/local \
	   -D COMPILE_GPU = 1 \
	   -D WITH_CUDA=ON \
           -D ENABLE_FAST_MATH=1 \
           -D CUDA_FAST_MATH=1 \
           -D WITH_CUBLAS=1 .. \
	&& make -j8 \
	&& make install \
	&& ldconfig \
	&& rm -rf /tmp/*
	
	
WORKDIR /src

COPY . ./

	# Construccion de VideoJoiner 
RUN cd /src/videojoiner \
	&& make \
	&& mv VideoJoiner /usr/bin \ 
	# Construccion de ValidadorRastreo
	&& cd /src/validador \
	&& make \
	&& mv ValidadorRastreo /usr/bin \
	# Construccion de AnotadorRastreo 
	&& cd /src/anotador_gtk2 \
	&& make \
	&& mv AnotarRastreo /usr/bin \
	# Construccion de MultipartiteGraphTracker 
	&& cd /src/mpgt \
	&& make \
	&& mv MultipartiteGraphTracker /usr/bin \
	&& rm -r /src 

WORKDIR /mpgt
	

