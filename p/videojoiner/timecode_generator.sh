#!/bin/sh

#Usage timecode_generator Video.MXF

ffmpeg -i $1 -f ffmetadata metadata.txt
