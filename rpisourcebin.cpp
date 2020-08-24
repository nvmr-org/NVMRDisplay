#include "rpisourcebin.h"

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.rpisource" );

RPISourceBin::RPISourceBin(QObject *parent) : VideoSource(parent)
{
    m_bin = gst_bin_new( "rpi-bin" );
    GstBin* bin = GST_BIN(m_bin);

    GstElement* udpSrc = gst_element_factory_make( "udpsrc", "udp" );
    GstCaps *caps = gst_caps_new_simple( "application/x-rtp", nullptr, nullptr );

    g_object_set( udpSrc,
                  "caps", caps,
                  NULL );
    gst_caps_unref(caps);

    GstElement* depay = gst_element_factory_make( "rtph264depay", "depay" );
    GstElement* parse = gst_element_factory_make( "h264parse", nullptr );
    GstElement* avdec = gst_element_factory_make( "avdec_h264", nullptr );

    gst_bin_add_many(bin, udpSrc, depay, parse, avdec, nullptr );

    gst_element_link_many( udpSrc, depay, parse, avdec, nullptr );

    GstPad* source_pad = gst_element_get_static_pad (avdec, "src");
    GstPad* source_ghost_pad = gst_ghost_pad_new ("src", source_pad);
    gst_pad_set_active (source_ghost_pad, TRUE);
    gst_element_add_pad (m_bin, source_ghost_pad);

    //gst_object_unref (source_ghost_pad);

//    udpsrc port=5248 \
//      ! 'application/x-rtp' \
//      ! rtph264depay
}

RPISourceBin::~RPISourceBin(){
    gst_object_unref( GST_OBJECT( m_bin ) );
}

void RPISourceBin::writeToQSettings( QSettings* settings ){
    writeCommon( settings, SourceType::RPi );
    settings->setValue( "port", m_port );
}

void RPISourceBin::readFromQSettings( QSettings* settings ){
    readVideoId( settings );

    int port = settings->value( "port" ).toInt();

    setPort( port );
}

SourceType RPISourceBin::sourceType() const {
    return SourceType::RPi;
}

GstElement* RPISourceBin::getBin(){
    return m_bin;
}

int RPISourceBin::port() const{
    return m_port;
}

void RPISourceBin::setPort( int port ){
    m_port = port;

    GstElement* udpSrc = gst_bin_get_by_name( GST_BIN(m_bin), "udp" );
    if( udpSrc == nullptr ){
        LOG4CXX_ERROR( logger, "Unable to get bin UDP?!" );
        return;
    }

    g_object_set( udpSrc,
                  "port", m_port,
                  NULL );
}
