#ifndef EMBEDDEDAVAHIBROWSE_H
#define EMBEDDEDAVAHIBROWSE_H

#include <QObject>

#include "rpivideosender.h"
#include "avahi-core/core.h"
#include "avahi-core/lookup.h"
#include "servicediscover.h"

class EmbeddedAvahiBrowse : public ServiceDiscover
{
    Q_OBJECT
public:
    explicit EmbeddedAvahiBrowse(QObject *parent = nullptr);

public Q_SLOTS:
    void initialize();

private:
    static void jmri_browse_callback(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
        void* userdata);
    void jmri_browse_cb(
            AvahiSServiceBrowser *b,
            AvahiIfIndex interface,
            AvahiProtocol protocol,
            AvahiBrowserEvent event,
            const char *name,
            const char *type,
            const char *domain,
            AVAHI_GCC_UNUSED AvahiLookupResultFlags flags);
    static void jmri_resolve_callback(
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
            void* userdata);
    void jmri_resolve_cb(
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
        AvahiLookupResultFlags flags);

    static void videosender_browse_callback(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
        void* userdata);
    void videosender_browse_cb(
            AvahiSServiceBrowser *b,
            AvahiIfIndex interface,
            AvahiProtocol protocol,
            AvahiBrowserEvent event,
            const char *name,
            const char *type,
            const char *domain,
            AVAHI_GCC_UNUSED AvahiLookupResultFlags flags);
    static void videosender_resolve_callback(
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
            void* userdata);
    void videosender_resolve_cb(
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
        AvahiLookupResultFlags flags);

private:
    AvahiServer* m_server;
    AvahiSServiceBrowser* m_jmriBrowse;
    AvahiSServiceBrowser* m_videoSenderBrowse;
};

#endif // EMBEDDEDAVAHIBROWSE_H
