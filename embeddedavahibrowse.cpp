#include <QDebug>
#include <QtEndian>
#include "embeddedavahibrowse.h"
#include "avahi-qt/qt-watch.h"
#include "avahi-common/error.h"

EmbeddedAvahiBrowse::EmbeddedAvahiBrowse(QObject *parent) : ServiceDiscover(parent)
{
    QTimer::singleShot( 0, this, &EmbeddedAvahiBrowse::initialize );
}

void EmbeddedAvahiBrowse::initialize(){
    AvahiServerConfig config;
    int error;

    /* Do not publish any local records */
    avahi_server_config_init(&config);
    config.publish_hinfo = 0;
    config.publish_addresses = 0;
    config.publish_workstation = 0;
    config.publish_domain = 0;

    /* Allocate a new server */
    m_server = avahi_server_new(avahi_qt_poll_get(), &config, NULL, NULL, &error);

    /* Free the configuration data */
    avahi_server_config_free(&config);

    /* Check wether creating the server object succeeded */
    if (!m_server) {
        fprintf(stderr, "Failed to create server: %s\n", avahi_strerror(error));
        goto fail;
    }

    /* Create the service browser */
    if (!(m_jmriBrowse = avahi_s_service_browser_new(m_server,
                                           AVAHI_IF_UNSPEC,
                                           AVAHI_PROTO_INET,
                                           "_http._tcp",
                                           NULL,
                                           (AvahiLookupFlags)0,
                                           &EmbeddedAvahiBrowse::jmri_browse_callback,
                                           this))) {
        fprintf(stderr, "Failed to create service browser: %s\n", avahi_strerror(avahi_server_errno(m_server)));
        fflush(stderr);
        goto fail;
    }

    if (!(m_videoSenderBrowse = avahi_s_service_browser_new(m_server,
                                           AVAHI_IF_UNSPEC,
                                           AVAHI_PROTO_INET,
                                           "_nvmr_video_sender._tcp",
                                           NULL,
                                           (AvahiLookupFlags)0,
                                           &EmbeddedAvahiBrowse::videosender_browse_callback,
                                           this))) {
        fprintf(stderr, "Failed to create service browser: %s\n", avahi_strerror(avahi_server_errno(m_server)));
        fflush(stderr);
        goto fail;
    }


    qDebug() << "initialized";

fail:
    return;
}

void EmbeddedAvahiBrowse::jmri_browse_callback(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
        void* userdata){
    qDebug() << "browse callback event: " << event;
    EmbeddedAvahiBrowse* avahiBrowse = static_cast<EmbeddedAvahiBrowse*>( userdata );
    avahiBrowse->jmri_browse_cb( b, interface, protocol, event, name, type, domain, flags );
}

void EmbeddedAvahiBrowse::jmri_browse_cb(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags){
    /* Called whenever a new services becomes available on the LAN or is removed from the LAN */

    switch (event) {

        case AVAHI_BROWSER_FAILURE:
            qDebug() << "browser failure: " << avahi_strerror(avahi_server_errno(m_server));
            return;

        case AVAHI_BROWSER_NEW:
        qDebug() << "new serivce " << name << " of type " << type << " in domain " << domain;

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_s_service_resolver_new(m_server,
                                               interface,
                                               protocol,
                                               name,
                                               type,
                                               domain,
                                               AVAHI_PROTO_UNSPEC,
                                               (AvahiLookupFlags)0,
                                               &EmbeddedAvahiBrowse::jmri_resolve_callback,
                                               this)))
                fprintf(stderr, "Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_server_errno(m_server)));

            break;

        case AVAHI_BROWSER_REMOVE:
            fprintf(stderr, "(Browser) REMOVE: service '%s' of type '%s' in domain '%s'\n", name, type, domain);
            fflush(stderr);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            qDebug() <<  "(Browser) " << (event == AVAHI_BROWSER_CACHE_EXHAUSTED ? "CACHE_EXHAUSTED" : "ALL_FOR_NOW");
            break;
    }
}

void EmbeddedAvahiBrowse::jmri_resolve_callback(
            AvahiSServiceResolver *r,
            AVAHI_GCC_UNUSED AvahiIfIndex interface,
            AVAHI_GCC_UNUSED AvahiProtocol protocol,
            AvahiResolverEvent event,
            const char *name,
            const char *type,
            const char *domain,
            const char *host_name,
            const AvahiAddress *address,
            uint16_t port,
            AvahiStringList *txt,
            AvahiLookupResultFlags flags,
        void* userdata){
    EmbeddedAvahiBrowse* avahiBrowse = static_cast<EmbeddedAvahiBrowse*>( userdata );
    avahiBrowse->jmri_resolve_cb( r, interface, protocol, event, name, type, domain, host_name, address, port, txt, flags );
}

void EmbeddedAvahiBrowse::jmri_resolve_cb(
    AvahiSServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
        AvahiLookupResultFlags flags){
    /* Called whenever a service has been resolved successfully or timed out */
    std::vector<std::vector<uint8_t>> txtData;
    AvahiStringList* current = txt;

    while( current ){
        std::vector<uint8_t> vec;
        vec.reserve( current->size );
        for( size_t x = 0; x < current->size; x++ ){
            vec.push_back( current->text[x] );
        }
        txtData.push_back( vec );
        current = current->next;
    }

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
                    name, type, domain, avahi_strerror(avahi_server_errno(m_server)));
            break;

        case AVAHI_RESOLVER_FOUND: {
            if( address->proto == AVAHI_PROTO_INET6 ) break;

            QHostAddress hostAddress = QHostAddress( qFromBigEndian( address->data.ipv4.address ) );
            qDebug() << "service " << name << " found at address " << host_name << " type " << type;
            foundHTTPServer( QString(name),
                             hostAddress.toString(),
                             port,
                             txtData );
        }
    }

    avahi_s_service_resolver_free(r);
}

void EmbeddedAvahiBrowse::videosender_browse_callback(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
        void* userdata){
    qDebug() << "browse callback event: " << event;
    EmbeddedAvahiBrowse* avahiBrowse = static_cast<EmbeddedAvahiBrowse*>( userdata );
    avahiBrowse->videosender_browse_cb( b, interface, protocol, event, name, type, domain, flags );
}

void EmbeddedAvahiBrowse::videosender_browse_cb(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags){
    /* Called whenever a new services becomes available on the LAN or is removed from the LAN */

    switch (event) {

        case AVAHI_BROWSER_FAILURE:
            qDebug() << "browser failure: " << avahi_strerror(avahi_server_errno(m_server));
            return;

        case AVAHI_BROWSER_NEW:
        qDebug() << "new serivce " << name << " of type " << type << " in domain " << domain;

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_s_service_resolver_new(m_server,
                                               interface,
                                               protocol,
                                               name,
                                               type,
                                               domain,
                                               AVAHI_PROTO_UNSPEC,
                                               (AvahiLookupFlags)0,
                                               &EmbeddedAvahiBrowse::videosender_resolve_callback,
                                               this)))
                fprintf(stderr, "Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_server_errno(m_server)));

            break;

        case AVAHI_BROWSER_REMOVE:
            fprintf(stderr, "(Browser) REMOVE: service '%s' of type '%s' in domain '%s'\n", name, type, domain);
            fflush(stderr);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            qDebug() <<  "(Browser) " << (event == AVAHI_BROWSER_CACHE_EXHAUSTED ? "CACHE_EXHAUSTED" : "ALL_FOR_NOW");
            break;
    }
}

void EmbeddedAvahiBrowse::videosender_resolve_callback(
            AvahiSServiceResolver *r,
            AVAHI_GCC_UNUSED AvahiIfIndex interface,
            AVAHI_GCC_UNUSED AvahiProtocol protocol,
            AvahiResolverEvent event,
            const char *name,
            const char *type,
            const char *domain,
            const char *host_name,
            const AvahiAddress *address,
            uint16_t port,
            AvahiStringList *txt,
            AvahiLookupResultFlags flags,
        void* userdata){
    EmbeddedAvahiBrowse* avahiBrowse = static_cast<EmbeddedAvahiBrowse*>( userdata );
    avahiBrowse->videosender_resolve_cb( r, interface, protocol, event, name, type, domain, host_name, address, port, txt, flags );
}

void EmbeddedAvahiBrowse::videosender_resolve_cb(
    AvahiSServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
        AvahiLookupResultFlags flags){
    /* Called whenever a service has been resolved successfully or timed out */
    std::vector<std::vector<uint8_t>> txtData;
    AvahiStringList* current = txt;

    while( current ){
        std::vector<uint8_t> vec;
        vec.reserve( current->size );
        for( size_t x = 0; x < current->size; x++ ){
            vec.push_back( current->text[x] );
        }
        txtData.push_back( vec );
        current = current->next;
    }

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
                    name, type, domain, avahi_strerror(avahi_server_errno(m_server)));
            break;

        case AVAHI_RESOLVER_FOUND: {
            if( address->proto == AVAHI_PROTO_INET6 ) break;

            QHostAddress hostAddress = QHostAddress( qFromBigEndian( address->data.ipv4.address ) );
            qDebug() << "service " << name << " found at address " << host_name << " type " << type;
            foundRPIVideoSender( QString(name),
                                 hostAddress.toString(),
                                 port,
                                 txtData );
        }
    }

    avahi_s_service_resolver_free(r);
}
