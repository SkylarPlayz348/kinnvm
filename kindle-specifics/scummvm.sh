#!/bin/sh
# Name: ScummVM
# Author: ScummVM Dev Team + Others
# DontUseFBInk

DIR="/mnt/us/scummvm/"

export GST_PLUGIN_SYSTEM_PATH=/usr/lib/gstreamer-0.10
export LD_LIBRARY_PATH="/mnt/us/scummvm/libs"
exec "$DIR/bin/scummvm" \
    --config="$DIR/scummvm.ini" \
    --extrapath="$DIR/share/scummvm" \
    "$@" >> "$DIR/scummvm.log" 2>&1
