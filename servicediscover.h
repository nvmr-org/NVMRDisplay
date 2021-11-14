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
    void rpiVideoSenderRtspFound( QUrl url );
};

#endif // SERVICEDISCOVER_H
