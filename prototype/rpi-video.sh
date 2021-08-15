#!/bin/bash

width=1280
height=720

#  ! jpegenc snapshot=TRUE ! filesink location=/tmp/foo.jpg

gst-launch-1.0 udpsrc multicast-group=224.1.1.3 auto-multicast=true port=8230 \
  ! "application/x-rtp,width=$width,height=$height" \
  ! rtph264depay \
  ! avdec_h264 \
  ! autovideosink sync=false

