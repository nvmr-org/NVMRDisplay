#!/bin/bash

gst-launch-1.0 udpsrc multicast-group=224.1.1.3 auto-multicast=true port=8230 \
  ! 'application/x-rtp,width=1280,height=720' \
  ! rtph264depay \
  ! avdec_h264 \
  ! autovideosink sync=false

