#ifndef SERVICEDISCOVER_H
#define SERVICEDISCOVER_H

#include <QObject>
#include <QHostAddress>

#include "rpivideosender.h"

class ServiceDiscover : public QObject
{
    Q_OBJECT
public:
    explicit ServiceDiscover(QObject *parent = nullptr);

signals:
    void rpiVideoSenderFound( RPIVideoSender* sender );
    void rpiVideoSenderWentAway( RPIVideoSender* sender );
    void jmriWebserverFound( QHostAddress address,  uint16_t port );

};

#endif // SERVICEDISCOVER_H
