#include "embeddedavahibrowse.h"
#include "avahi-qt/qt-watch.h"
#include "avahi-common/error.h"

EmbeddedAvahiBrowse::EmbeddedAvahiBrowse(QObject *parent) : QObject(parent)
{

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
        goto fail;
    }

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

            fprintf(stderr, "(Browser) %s\n", avahi_strerror(avahi_server_errno(m_server)));
            return;

        case AVAHI_BROWSER_NEW:
            fprintf(stderr, "(Browser) NEW: service '%s' of type '%s' in domain '%s'\n", name, type, domain);

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
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            fprintf(stderr, "(Browser) %s\n", event == AVAHI_BROWSER_CACHE_EXHAUSTED ? "CACHE_EXHAUSTED" : "ALL_FOR_NOW");
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

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
                    name, type, domain, avahi_strerror(avahi_server_errno(m_server)));
            break;

        case AVAHI_RESOLVER_FOUND: {
            char a[AVAHI_ADDRESS_STR_MAX], *t;

            fprintf(stderr, "(Resolver) Service '%s' of type '%s' in domain '%s':\n", name, type, domain);

//                avahi_address_snprint(a, sizeof(a), address);
//                t = avahi_string_list_to_string(txt);
//                fprintf(stderr,
//                        "\t%s:%u (%s)\n"
//                        "\tTXT=%s\n"
//                        "\tcookie is %u\n"
//                        "\tis_local: %i\n"
//                        "\twide_area: %i\n"
//                        "\tmulticast: %i\n"
//                        "\tcached: %i\n",
//                        host_name, port, a,
//                        t,
//                        avahi_string_list_get_service_cookie(txt),
//                        !!(flags & AVAHI_LOOKUP_RESULT_LOCAL),
//                        !!(flags & AVAHI_LOOKUP_RESULT_WIDE_AREA),
//                        !!(flags & AVAHI_LOOKUP_RESULT_MULTICAST),
//                        !!(flags & AVAHI_LOOKUP_RESULT_CACHED));
//                avahi_free(t);
        }
    }

    avahi_s_service_resolver_free(r);
}
