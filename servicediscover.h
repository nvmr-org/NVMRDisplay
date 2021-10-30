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

signals:
    void rpiVideoSenderFound( RPIVideoSender* sender );
    void rpiVideoSenderWentAway( RPIVideoSender* sender );
    void jmriWebserverFound( QUrl url );

};

#endif // SERVICEDISCOVER_H
