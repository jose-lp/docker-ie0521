#/bin/bash

xhost local:root

docker run -ti --net host \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /home/$USER/docker/mpgt:/mpgt \
    -e DISPLAY=unix$DISPLAY \
    mpgt


