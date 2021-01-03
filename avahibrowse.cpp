#include "avahibrowse.h"

#include "avahi-dbus/ServerProxy.h"
#include "avahi-dbus/ServiceBrowserProxy.h"

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.AvahiBrowse" );

AvahiBrowse::AvahiBrowse(QObject *parent) : QObject(parent)
{
    m_dispatcher = DBus::Qt::QtDispatcher::create();
    m_conn = m_dispatcher->create_connection( DBus::BusType::SYSTEM );
}

void AvahiBrowse::initialize(){
    m_avahiServer = Avahi::ServerProxy::create( m_conn, "org.freedesktop.Avahi", "/" );

    DBus::Path serviceBrowsePath =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()
                ->ServiceBrowserNew( -1, 0, "_nvmr_video_sender._tcp", std::string(), 0 );

    LOG4CXX_DEBUG( logger, "Service browser path: " << serviceBrowsePath );

    m_browserProxy = Avahi::ServiceBrowserProxy::create( m_conn, "org.freedesktop.Avahi", serviceBrowsePath );

    std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> proxyTmp =
            m_browserProxy->getorg_freedesktop_Avahi_ServiceBrowserInterface();

    proxyTmp->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalFailure ) );
    proxyTmp->signal_AllForNow()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalAllForNow ) );
    proxyTmp->signal_CacheExhausted()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalCacheExhausted ) );
    proxyTmp->signal_ItemNew()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalItemNew ) );
    proxyTmp->signal_ItemRemove()->connect( sigc::mem_fun( *this, &AvahiBrowse::signalItemRemoved ) );

    proxyTmp->Start();

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

void AvahiBrowse::signalItemNew(int32_t interface,
                                int32_t protocol,
                                std::string name,
                                std::string type,
                                std::string domain,
                                uint32_t flags){
    LOG4CXX_DEBUG( logger, "New item!  name: " << name << " type: " << type );

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
            ->signal_Found()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedFound ) );
    newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()
            ->signal_Failure()->connect( sigc::mem_fun( *this, &AvahiBrowse::resolvedError ) );
    //newResolver->getorg_freedesktop_Avahi_ServiceResolverInterface()->Start();

}

void AvahiBrowse::signalItemRemoved(int32_t interface,
                                int32_t protocol,
                                std::string name,
                                std::string type,
                                std::string domain,
                                uint32_t flags){
    LOG4CXX_DEBUG( logger, "Removed item!  name: " << name << " type: " << type );

}

void AvahiBrowse::resolvedFound(int32_t interface,
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

    VideoSender newVideoSender;
    newVideoSender.name = QString::fromStdString( name );
    newVideoSender.port = port;
    newVideoSender.address = QString::fromStdString( address );

    // We are assuming that the 'name' of the video sender is unique.
    QVector<VideoSender>::iterator it =
            std::find_if( m_resolvedVideoSenders.begin(),
                          m_resolvedVideoSenders.end(),
                       [newVideoSender]( const VideoSender& fromArray ){
        return newVideoSender.name == fromArray.name;
    } );

    if( it != m_resolvedVideoSenders.end() ){
        m_resolvedVideoSenders.erase( it );
    }
    m_resolvedVideoSenders.push_back( newVideoSender );
    Q_EMIT videoSendersUpdated();
}

void AvahiBrowse::resolvedError(std::string err){
    LOG4CXX_ERROR( logger, "Resolver had error: " << err );
}

QVector<VideoSender> AvahiBrowse::getVideoSenders(){
    return m_resolvedVideoSenders;
}

VideoSender AvahiBrowse::getVideoSenderByID(QString id){
    QVector<VideoSender>::iterator it =
            std::find_if( m_resolvedVideoSenders.begin(),
                          m_resolvedVideoSenders.end(),
                       [id]( const VideoSender& fromArray ){
        return id == fromArray.name;
    } );

    if( it == m_resolvedVideoSenders.end() ){
        return VideoSender();
    }

    return *it;
}
