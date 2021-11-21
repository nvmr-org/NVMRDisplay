#ifndef SERVICEDISCOVER_H
#define SERVICEDISCOVER_H

#include <QObject>
#include <QUrl>

#include "rpivideosender.h"

class ServiceDiscover : public QObject
{
    Q_OBJECT
public:
    explicit ServiceDiscover(QObject *parent = nullptr);

Q_SIGNALS:
    void rpiVideoSenderFound( RPIVideoSender* sender );
    void rpiVideoSenderWentAway( RPIVideoSender* sender );
    void jmriWebserverFound( QUrl url );
    void rpiVideoSenderRtspFound( QUrl url, QString name );

protected:
    void foundRPIVideoSender( QString name, QString address, int port, std::vector<std::vector<uint8_t>> txtData );
    void foundHTTPServer( QString name, QString address, int port, std::vector<std::vector<uint8_t>> txtData );

private:
    QVector<RPIVideoSender*> m_resolvedVideoSenders;
};

#endif // SERVICEDISCOVER_H
