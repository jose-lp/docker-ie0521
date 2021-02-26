#!/bin/bash

# Uso: bash mfx2mp4.sh video dest_path

if [ $# -ne 2 ]; then
	echo 'Uso: bash mfx2mp4.sh video dest_path'
	exit 0
fi

codec=copy
format=mp4
ext=mp4
segment='-t 3'		# en segundos, cadena vacía para el video completo
md_ext=metadata
loglevel=quiet

# Uncomment next line to debug
#set -x

filename=$(basename "$1" .MXF)
ffmpeg -loglevel $loglevel -i $1 -f ffmetadata $2/$filename.$md_ext
#sed -i 's/\\;/;/g' $2/${1%.*}.$mdext
#cp ${1%.*}M01.XML $2
ffmpeg -loglevel $loglevel -i $1 -vcodec $codec -an -f $format $segment $2/$filename.$ext
