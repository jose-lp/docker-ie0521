#!/bin/sh
CPUPROFILE=/tmp/mpgt_prof 
./MultipartiteGraphTracker -i /tmp/result.avi -o /tmp/salida.avi -c /tmp/config.yaml -t /tmp/tracks.yaml -I
google-pprof --callgrind ./MultipartiteGraphTracker /tmp/mpgt_prof > /tmp/profiling_kcachegrind.log
kcachegrind /tmp/profiling_kcachegrind.log
