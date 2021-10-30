#!/bin/bash

IP_ADDR=$1

gst-launch-1.0 rtspsrc location=rtsp://$1:8554/rpi-video latency=0 buffer-mode=auto\
 ! rtph264depay \
 ! avdec_h264 \
 ! autovideosink
