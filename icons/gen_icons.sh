#!/bin/sh

APP_NAME=voltfish
cd "$(dirname "$(readlink -f "$0")")"

for i in 86x86 108x108 128x128 172x172
do
	convert -resize "$i" "256x256/${APP_NAME}.png" "${i}/${APP_NAME}.png"
done
