#include "soospysourcebin.h"
#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.soospysource" );

SoospySourceBin::SoospySourceBin(QObject *parent) : VideoSource(parent)
{
    m_bin = gst_bin_new( "soospy-bin" );
    GstBin* bin = GST_BIN(m_bin);

    GstElement* soupSrc = gst_element_factory_make( "souphttpsrc", "soupsrc" );
    GstElement* asfdemux = gst_element_factory_make( "asfdemux", "asf" );
    GstElement* parse = gst_element_factory_make( "h264parse", "h264" );
    GstElement* avdec = gst_element_factory_make( "avdec_h264", nullptr );

    if( soupSrc == nullptr || asfdemux == nullptr || parse == nullptr || avdec == nullptr ){
        LOG4CXX_ERROR( logger, "something null" );
    }

    g_object_set( soupSrc, "is-live", 1, nullptr );
    g_object_set( soupSrc, "do-timestamp", 0, nullptr );

    g_signal_connect( asfdemux, "pad-added", G_CALLBACK(&SoospySourceBin::pad_added_handler_cb), this );

    gst_bin_add_many(bin, soupSrc, asfdemux, parse, avdec, nullptr );

    // Note: can't link asfdemux to parse, as pad doesn't always exist.
    gst_element_link( soupSrc, asfdemux );
    gst_element_link( parse, avdec );

    //gst_element_link_many( soupSrc, asfdemux, parse, avdec, nullptr );

    GstPad* source_pad = gst_element_get_static_pad (avdec, "src");
    GstPad* source_ghost_pad = gst_ghost_pad_new ("src", source_pad);
    gst_pad_set_active (source_ghost_pad, TRUE);
    gst_element_add_pad (m_bin, source_ghost_pad);
}

SoospySourceBin::~SoospySourceBin(){
    gst_object_unref( GST_OBJECT( m_bin ) );
}

void SoospySourceBin::writeToQSettings( QSettings* settings ){
    writeCommon( settings, SourceType::Soospy );

    settings->setValue( "location", m_location );
    settings->setValue( "user-id", m_userId );
    settings->setValue( "user-password", m_userPassword );
}

void SoospySourceBin::readFromQSettings(QSettings *settings){
    readVideoId( settings );

    QString location = settings->value( "location" ).toString();
    QString userId = settings->value( "user-id" ).toString();
    QString userPassword = settings->value( "user-password" ).toString();

    setLocation( location );
    setUserId( userId );
    setUserPassword( userPassword );
}

SourceType SoospySourceBin::sourceType() const{
    return SourceType::Soospy;
}

GstElement* SoospySourceBin::getBin(){
    return m_bin;
}

QString SoospySourceBin::location() const{
    return m_location;
}

void SoospySourceBin::setLocation( QString location ){
    m_location = location;

    std::string stdLocation = m_location.toStdString();
    GstElement* soup = gst_bin_get_by_name( GST_BIN( m_bin ), "soupsrc" );
    g_object_set( soup, "location", stdLocation.c_str(), nullptr );
}

QString SoospySourceBin::userId() const{
    return m_userId;
}

void SoospySourceBin::setUserId( QString id ){
    m_userId = id;

    std::string stdId = m_userId.toStdString();
    GstElement* soup = gst_bin_get_by_name( GST_BIN( m_bin ), "soupsrc" );
    g_object_set( soup, "user-id", stdId.c_str(), nullptr );
}

QString SoospySourceBin::userPassword() const{
    return m_userPassword;
}

void SoospySourceBin::setUserPassword( QString str ){
    m_userPassword = str;

    std::string stdPass = m_userPassword.toStdString();
    GstElement* soup = gst_bin_get_by_name( GST_BIN( m_bin ), "soupsrc" );
    g_object_set( soup, "user-pw", stdPass.c_str(), nullptr );
}

void SoospySourceBin::pad_added_handler_cb (GstElement *src, GstPad *pad, void *data){
    SoospySourceBin* soospy = static_cast<SoospySourceBin*>( data );
    soospy->pad_added_handler( src, pad );
}

void SoospySourceBin::pad_added_handler( GstElement* src, GstPad* pad ){
    QString elementName( gst_element_get_name( src ) );
    if( elementName == "asf" ){
        gchar* name = gst_pad_get_name( pad );
        QString padName( name );
        g_free( name );
        LOG4CXX_DEBUG( logger, "Padd added, name is " << padName.toStdString() );
        if( padName.startsWith( "video_" ) ){
            GstElement* h264 = gst_bin_get_by_name( GST_BIN( m_bin), "h264" );
            if( !gst_element_link( src, h264 ) ){
                LOG4CXX_ERROR( logger, "Unable to link asf to h264" );
            }
        }
    }
}
