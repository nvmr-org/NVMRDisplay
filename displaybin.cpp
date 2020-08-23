#include <QQuickItem>

#include <log4cxx/logger.h>

#include "displaybin.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.displaybin" );

DisplayBin::DisplayBin(QObject *parent) : QObject(parent)
{
    m_bin = gst_bin_new( "disp-bin" );
    GstBin* bin = GST_BIN( m_bin );

    GstElement* convert = gst_element_factory_make( "videoconvert", nullptr );
    GstElement* sinkBin = gst_element_factory_make("glsinkbin", nullptr);
    m_qml = gst_element_factory_make ("qmlglsink", "qmlsink");

    g_object_set (sinkBin, "sink", m_qml, nullptr);

    g_object_set( m_qml, "sync", false, nullptr );
    g_object_set( sinkBin, "sync", false, nullptr );

    gst_bin_add_many (bin, convert, sinkBin, nullptr);
    if( !gst_element_link (convert, sinkBin ) ){
        LOG4CXX_ERROR( logger, "convert->sink" );
    }

    GstPad* sink_pad = gst_element_get_static_pad (convert, "sink");
    GstPad* sink_ghost_pad = gst_ghost_pad_new ("sink", sink_pad);
    gst_pad_set_active (sink_ghost_pad, TRUE);
    gst_element_add_pad (m_bin, sink_ghost_pad);
    //gst_object_unref (sink_ghost_pad);
}

DisplayBin::~DisplayBin(){
    gst_object_unref( m_bin );
}

GstElement* DisplayBin::bin(){
    return m_bin;
}

void DisplayBin::linkToQQuickWidget(QQuickWidget *widget){
    QQuickItem* quickRoot = widget->rootObject();
    g_object_set( m_qml,
                  "widget", quickRoot->findChild<QQuickItem*>( "videoItem" ),
                  nullptr );
}
