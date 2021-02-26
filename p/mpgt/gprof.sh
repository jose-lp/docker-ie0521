#!/bin/sh
./MultipartiteGraphTracker -i /tmp/result.avi -o /tmp/salida.avi -c /tmp/config.yaml -t /tmp/tracks.yaml -I

gprof MultipartiteGraphTracker gmon.out > profile-data.txt
