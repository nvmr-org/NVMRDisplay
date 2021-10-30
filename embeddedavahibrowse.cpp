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
    if (!(m_serviceBrowse = avahi_s_service_browser_new(m_server,
                                           AVAHI_IF_UNSPEC,
                                           AVAHI_PROTO_UNSPEC,
                                           "_http._tcp",
                                           NULL,
                                           (AvahiLookupFlags)0,
                                           &EmbeddedAvahiBrowse::browse_callback,
                                           this))) {
        fprintf(stderr, "Failed to create service browser: %s\n", avahi_strerror(avahi_server_errno(m_server)));
        fflush(stderr);
        goto fail;
    }

    qDebug() << "initialized";

fail:
    return;
}

void EmbeddedAvahiBrowse::browse_callback(
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
    avahiBrowse->browse_cb( b, interface, protocol, event, name, type, domain, flags );
}

void EmbeddedAvahiBrowse::browse_cb(
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
                                               &EmbeddedAvahiBrowse::resolve_callback,
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

void EmbeddedAvahiBrowse::resolve_callback(
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
    avahiBrowse->resolve_cb( r, interface, protocol, event, name, type, domain, host_name, address, port, txt, flags );
}

void EmbeddedAvahiBrowse::resolve_cb(
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
    QStringList qTxt;
    AvahiStringList* current = txt;

    while( current ){
        qTxt.append( QString::fromLocal8Bit( (const char*)current->text, current->size ) );
        current = current->next;
    }

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
                    name, type, domain, avahi_strerror(avahi_server_errno(m_server)));
            break;

        case AVAHI_RESOLVER_FOUND: {
            if( address->proto == AVAHI_PROTO_INET6 ) break;

            qDebug() << "service " << name << " found at address " << host_name << " type " << type;
            for( const QString& str : qTxt ){
                if( str.startsWith( "jmri" ) ){
                    QUrl url = "http://" +
                            QHostAddress( qFromBigEndian( address->data.ipv4.address ) ).toString()
                            + ":" + QString::number( port );
                    emit jmriWebserverFound( url );
                }
            }
        }
    }

    avahi_s_service_resolver_free(r);
}
