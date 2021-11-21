#include "avahibrowse.h"

#include "avahi-dbus/ServerProxy.h"
#include "avahi-dbus/ServiceBrowserProxy.h"

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.AvahiBrowse" );

AvahiBrowse::AvahiBrowse(QObject *parent) : ServiceDiscover(parent)
{
    m_dispatcher = DBus::Qt::QtDispatcher::create();
    m_conn = m_dispatcher->create_connection( DBus::BusType::SYSTEM );
    QTimer::singleShot( 500, this, &AvahiBrowse::initialize );
}

void AvahiBrowse::initialize(){
    m_avahiServer = Avahi::ServerProxy::create( m_conn, "org.freedesktop.Avahi", "/" );

    DBus::Path serviceBrowsePath =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()
                ->ServiceBrowserNew( -1, 0, "_nvmr_video_sender._tcp", std::string(), 0 );

    LOG4CXX_DEBUG( logger, "Service browser path: " << serviceBrowsePath );

    m_browserProxyVideoSender = Avahi::ServiceBrowserProxy::create( m_conn, "org.freedesktop.Avahi", serviceBrowsePath );

    std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> proxyTmp =
            m_browserProxyVideoSender->getorg_freedesktop_Avahi_ServiceBrowserInterface();

    proxyTmp->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalFailure) );
    proxyTmp->signal_AllForNow()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalAllForNow ) );
    proxyTmp->signal_CacheExhausted()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalCacheExhausted ) );
    proxyTmp->signal_ItemNew()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalVideoSenderNew ) );
    proxyTmp->signal_ItemRemove()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalVideoSenderRemoved ) );

    proxyTmp->Start();

    // Create a new resolver for JMRI
    DBus::Path serviceBrowsePathJMRI =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()
                ->ServiceBrowserNew( -1, 0, "_http._tcp", std::string(), 0 );

    m_browserProxyJMRIHttp = Avahi::ServiceBrowserProxy::create( m_conn, "org.freedesktop.Avahi", serviceBrowsePathJMRI );

    std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> proxyTmp2 =
            m_browserProxyJMRIHttp->getorg_freedesktop_Avahi_ServiceBrowserInterface();

    proxyTmp2->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalFailure) );
    proxyTmp2->signal_AllForNow()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalAllForNow ) );
    proxyTmp2->signal_CacheExhausted()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalCacheExhausted ) );
    proxyTmp2->signal_ItemNew()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalHTTPNew ) );
    proxyTmp2->signal_ItemRemove()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalHTTPRemoved ) );

    proxyTmp2->Start();

}

void AvahiBrowse::signalFailure(std::string str){
    LOG4CXX_DEBUG( logger, "Avahi failure: " << str );
}

void AvahiBrowse::signalAllForNow(){
    LOG4CXX_DEBUG( logger, "All for now" );
}

void AvahiBrowse::signalCacheExhausted(){
    LOG4CXX_DEBUG( logger, "Cache exhausted" );
}

void AvahiBrowse::signalVideoSenderNew(int32_t interface,
                                int32_t protocol,
                                std::string name,
                                std::string type,
                                std::string domain,
                                uint32_t flags){
    LOG4CXX_DEBUG( logger, "New video resolver item!  name: " << name << " type: " << type );

    if( m_nameToResolver.find( QString::fromStdString( name ) ) != m_nameToResolver.end() ){
        return;
    }

    LOG4CXX_DEBUG( logger, "Getting new resolver for " << name << ":" << type << ":" << domain );
    DBus::Path serviceResolverPath =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()
            ->ServiceResolverNew(-1, 0, name, type, domain, 0, 0 );

    LOG4CXX_DEBUG( logger, "Service resolver path = " << serviceResolverPath );
    std::shared_ptr<Avahi::ServiceResolverProxy> newResolver =
            Avahi::ServiceResolverProxy::create( m_conn, "org.freedesktop.Avahi", serviceResolverPath );
    m_nameToResolver[ QString::fromStdString( name ) ] = newResolver;

    newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()
            ->signal_Found()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedVideoSenderFound ) );
    newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()
            ->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedVideoSenderError ) );
    //newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()->Start();

}

void AvahiBrowse::signalVideoSenderRemoved(int32_t interface,
                                int32_t protocol,
                                std::string name,
                                std::string type,
                                std::string domain,
                                uint32_t flags){
    LOG4CXX_DEBUG( logger, "Removed item!  name: " << name << " type: " << type );
    RPIVideoSender* toremove = nullptr;
    QString nameAsQStr = QString::fromStdString( name );

//    QVector<RPIVideoSender*>::iterator it;
//    for( it = m_resolvedVideoSenders.begin();
//         it != m_resolvedVideoSenders.end();
//         it++ ){
//        if( (*it)->name() == nameAsQStr ){
//            toremove = *it;
//            break;
//        }
//    }

//    if( toremove ){
//        m_resolvedVideoSenders.erase( it );
//        Q_EMIT rpiVideoSenderWentAway( toremove );
//        toremove->deleteLater();
//    }
}

void AvahiBrowse::resolvedVideoSenderFound(int32_t interface,
                        int32_t protocol,
                        std::string name,
                        std::string type,
                        std::string domain,
                        std::string host,
                        int32_t aprotocol,
                        std::string address,
                        uint16_t port,
                        std::vector<std::vector<uint8_t> > txt,
                        uint32_t flags){
    LOG4CXX_DEBUG( logger, "Found!  name: "
                   << name
                   << " type: "
                   << type
                   << " host: "
                   << host
                   << " address: "
                   << address
                   << " port: "
                   << port );

    foundRPIVideoSender( QString::fromStdString( name ),
                         QString::fromStdString( address ),
                         port,
                         txt );

//    RPIVideoSender* sender = new RPIVideoSender( QString::fromStdString( name ), QString::fromStdString( address ), port, this );
//    m_resolvedVideoSenders.push_back( sender );
//    Q_EMIT rpiVideoSenderFound( sender );

//    Q_EMIT rpiVideoSenderRtspFound( QString("rtsp://%1:8554/rpi-video").arg(address.c_str()) );

//    VideoSender newVideoSender;
//    newVideoSender.name = QString::fromStdString( name );
//    newVideoSender.port = port;
//    newVideoSender.address = QString::fromStdString( address );

//    // We are assuming that the 'name' of the video sender is unique.
//    QVector<VideoSender>::iterator it =
//            std::find_if( m_resolvedVideoSenders.begin(),
//                          m_resolvedVideoSenders.end(),
//                       [newVideoSender]( const VideoSender& fromArray ){
//        return newVideoSender.name == fromArray.name;
//    } );

//    if( it != m_resolvedVideoSenders.end() ){
//        m_resolvedVideoSenders.erase( it );
//    }
//    m_resolvedVideoSenders.push_back( newVideoSender );
//    Q_EMIT videoSendersUpdated();
}

void AvahiBrowse::resolvedVideoSenderError(std::string err){
    LOG4CXX_ERROR( logger, "Resolver had error: " << err );
}

void AvahiBrowse::signalHTTPNew(int32_t interface,
                                int32_t protocol,
                                std::string name,
                                std::string type,
                                std::string domain,
                                uint32_t flags){
    LOG4CXX_DEBUG( logger, "New HTTP item!  name: " << name << " type: " << type );

//    if( m_nameToResolver.find( QString::fromStdString( name ) ) != m_nameToResolver.end() ){
//        return;
//    }

    LOG4CXX_DEBUG( logger, "Getting new resolver for " << name << ":" << type << ":" << domain );
    DBus::Path serviceResolverPath =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()
            ->ServiceResolverNew(-1, 0, name, type, domain, 0, 0 );

    LOG4CXX_DEBUG( logger, "Service resolver path = " << serviceResolverPath );
    std::shared_ptr<Avahi::ServiceResolverProxy> newResolver =
            Avahi::ServiceResolverProxy::create( m_conn, "org.freedesktop.Avahi", serviceResolverPath );
//    m_nameToResolver[ QString::fromStdString( name ) ] = newResolver;

    newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()
            ->signal_Found()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedHTTPFound ) );
    newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()
            ->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedHTTPError ) );
}

void AvahiBrowse::signalHTTPRemoved(int32_t interface,
                                    int32_t protocol,
                                    std::string name,
                                    std::string type,
                                    std::string domain,
                                    uint32_t flags){
    LOG4CXX_DEBUG( logger, "Removed item!  name: " << name << " type: " << type );
}

void AvahiBrowse::resolvedHTTPFound(int32_t interface,
           int32_t protocol,
           std::string name,
           std::string type,
           std::string domain,
           std::string host,
           int32_t aprotocol,
           std::string address,
           uint16_t port,
           std::vector<std::vector<uint8_t>> txt,
                                    uint32_t flags){
    LOG4CXX_DEBUG( logger, "Found HTTP server!  name: "
                   << name
                   << " type: "
                   << type
                   << " host: "
                   << host
                   << " address: "
                   << address
                   << " port: "
                   << port );

    foundHTTPServer( QString::fromStdString( name ),
                     QString::fromStdString( address ),
                     port,
                     txt );

}
void AvahiBrowse::resolvedHTTPError( std::string err ){
    LOG4CXX_ERROR( logger, "Resolver had error: " << err );
}
