#!/bin/bash

LIVE=1
TIMESTAMP=0
IPADDR=192.168.1.187

GST_DEBUG=1 gst-launch-1.0 \
  souphttpsrc location=http://$IPADDR/videostream.asf user-id=admin user-pw=admin is-live=$LIVE do-timestamp=$TIMESTAMP\
  ! asfdemux \
  ! h264parse \
  ! avdec_h264 \
  ! videoconvert \
  ! fpsdisplaysink sync=false

