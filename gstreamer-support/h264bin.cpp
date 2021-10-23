#include "h264bin.h"

H264Bin::H264Bin(QObject *parent) : QObject(parent)
{
    m_bin = gst_bin_new( "h264-bin" );

    GstElement* source = gst_element_factory_make ("h264parse", "parse1");
    GstElement* decode = gst_element_factory_make("avdec_h264", "decode");
    GstElement* sink = gst_element_factory_make("fakesink", "fdsink" );

    gst_bin_add_many(GST_BIN(m_bin), source, decode, sink, nullptr );

    gst_element_link( source, decode );
    gst_element_link( decode, sink );
}

H264Bin::~H264Bin(){
    gst_object_unref( GST_OBJECT( m_bin ) );
}
