#!/bin/bash

width=1280
height=720

#  ! jpegenc snapshot=TRUE ! filesink location=/tmp/foo.jpg

gst-launch-1.0 udpsrc port=8230 \
  ! "application/x-rtp,width=$width,height=$height" \
  ! rtph264depay \
  ! avdec_h264 \
  ! autovideosink sync=false

exit 0

gst-launch-1.0 udpsrc port=8230 \
  ! "application/x-rtp,width=$width,height=$height,framerate=1/24,encoding-name=JPEG" \
  ! rtpjpegdepay \
  ! jpegdec \
  ! queue \
  ! fpsdisplaysink 
#  ! autovideosink

