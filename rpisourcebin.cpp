#include "rpisourcebin.h"

RPISourceBin::RPISourceBin(QObject *parent) : QObject(parent)
{
    m_bin = gst_bin_new( "rpi-bin" );

    GstElement* udpSrc = gst_element_factory_make( "udpsrc", "udp" );
    GstCaps *caps = gst_caps_new_simple( "application/x-rtp", nullptr, nullptr );
    gst_caps_unref(caps);

    g_object_set( udpSrc,
                  "port", 5248,
                  "caps", caps,
                  NULL );

    GstElement* depay = gst_element_factory_make( "rtph264depay", "depay" );

    gst_bin_add_many(GST_BIN(m_bin), udpSrc, depay, nullptr );

    gst_element_link( udpSrc, depay );

//    udpsrc port=5248 \
//      ! 'application/x-rtp' \
//      ! rtph264depay
}

RPISourceBin::~RPISourceBin(){
    gst_object_unref( GST_OBJECT( m_bin ) );
}
