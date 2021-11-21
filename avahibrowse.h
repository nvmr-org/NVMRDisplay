#ifndef AVAHIBROWSE_H
#define AVAHIBROWSE_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <dbus-cxx-qt.h>

#include "avahi-dbus/ServerProxy.h"
#include "avahi-dbus/ServiceBrowserProxy.h"
#include "avahi-dbus/ServiceResolverProxy.h"

#include "rpivideosender.h"
#include "servicediscover.h"

class AvahiBrowse : public ServiceDiscover
{
    Q_OBJECT
public:
    explicit AvahiBrowse(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    void initialize();

private:
    void signalFailure( std::string str );
    void signalAllForNow();
    void signalCacheExhausted();
    void signalVideoSenderNew(int32_t,int32_t,std::string,std::string,std::string,uint32_t);
    void signalVideoSenderRemoved(int32_t,int32_t,std::string,std::string,std::string,uint32_t);
    void signalHTTPNew(int32_t,int32_t,std::string,std::string,std::string,uint32_t);
    void signalHTTPRemoved(int32_t,int32_t,std::string,std::string,std::string,uint32_t);

    void resolvedVideoSenderFound(int32_t interface,
               int32_t protocol,
               std::string name,
               std::string type,
               std::string domain,
               std::string host,
               int32_t aprotocol,
               std::string address,
               uint16_t port,
               std::vector<std::vector<uint8_t>> txt,
               uint32_t flags);
    void resolvedVideoSenderError( std::string err );

    void resolvedHTTPFound(int32_t interface,
               int32_t protocol,
               std::string name,
               std::string type,
               std::string domain,
               std::string host,
               int32_t aprotocol,
               std::string address,
               uint16_t port,
               std::vector<std::vector<uint8_t>> txt,
               uint32_t flags);
    void resolvedHTTPError( std::string err );

private:
    std::shared_ptr<DBus::Connection> m_conn;
    std::shared_ptr<DBus::Dispatcher> m_dispatcher;
    std::shared_ptr<Avahi::ServerProxy> m_avahiServer;
    std::shared_ptr<Avahi::ServiceBrowserProxy> m_browserProxyVideoSender;
    std::shared_ptr<Avahi::ServiceBrowserProxy> m_browserProxyJMRIHttp;
    QMap<QString,std::shared_ptr<Avahi::ServiceResolverProxy>> m_nameToResolver;
};

#endif // AVAHIBROWSE_H
