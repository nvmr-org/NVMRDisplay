#ifndef EMBEDDEDAVAHIBROWSE_H
#define EMBEDDEDAVAHIBROWSE_H

#include <QObject>

#include "rpivideosender.h"
#include "avahi-core/core.h"
#include "avahi-core/lookup.h"

class EmbeddedAvahiBrowse : public QObject
{
    Q_OBJECT
public:
    explicit EmbeddedAvahiBrowse(QObject *parent = nullptr);

signals:
    void rpiVideoSenderFound( RPIVideoSender* sender );
    void rpiVideoSenderWentAway( RPIVideoSender* sender );
    void jmriWebserverFound( QHostAddress address,  uint16_t port );

public slots:
    void initialize();

private:
    static void browse_callback(
        AvahiSServiceBrowser *b,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
        void* userdata);
    void browse_cb(
            AvahiSServiceBrowser *b,
            AvahiIfIndex interface,
            AvahiProtocol protocol,
            AvahiBrowserEvent event,
            const char *name,
            const char *type,
            const char *domain,
            AVAHI_GCC_UNUSED AvahiLookupResultFlags flags);

    static void resolve_callback(
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
    void resolve_cb(
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
    AvahiSServiceBrowser* m_serviceBrowse;
};

#endif // EMBEDDEDAVAHIBROWSE_H
