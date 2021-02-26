#!/bin/sh
valgrind --tool=callgrind ./MultipartiteGraphTracker -i /tmp/result.avi -o /tmp/salida.avi -c /tmp/config.yaml -t /tmp/tracks.yaml -I
