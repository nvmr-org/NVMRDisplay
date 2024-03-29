#include "servicediscover.h"

ServiceDiscover::ServiceDiscover(QObject *parent) : QObject(parent)
{

}

void ServiceDiscover::foundRPIVideoSender(QString name, QString address, int port, std::vector<std::vector<uint8_t>> txtData){
    // Turn our txt data into a reasonable map
    QMap<QString,QString> txtMap;
    for( std::vector<uint8_t> vec : txtData ){
        QString asQstring = QString::fromLocal8Bit( (const char*)vec.data(), vec.size() );
        QStringList list = asQstring.split( '=' );
        if( list.size() == 2 ){
            txtMap.insert( list[0], list[1] );
        }
    }

    RPIVideoSender* sender = new RPIVideoSender( name,
                                                 address,
                                                 port,
                                                 this );
    m_resolvedVideoSenders.push_back( sender );

    if( txtMap.contains( "rtsp") ){
        Q_EMIT rpiVideoSenderRtspFound( txtMap.value( "rtsp" ), name, txtMap.value( "videoname" ) );
    }
}

void ServiceDiscover::foundHTTPServer( QString name, QString address, int port, std::vector<std::vector<uint8_t>> txtData ){
    QMap<QString,QString> txtMap;
    for( std::vector<uint8_t> vec : txtData ){
        QString asQstring = QString::fromLocal8Bit( (const char*)vec.data(), vec.size() );
        QStringList list = asQstring.split( '=' );
        if( list.size() == 2 ){
            txtMap.insert( list[0], list[1] );
        }
    }

    if( txtMap.contains( "jmri" ) ){
        QUrl url = "http://" +
                address
                + ":" + QString::number( port );
        Q_EMIT jmriWebserverFound( url );
    }
}

void ServiceDiscover::lostRPIVideoSender(QString name){
    QMutableVectorIterator<RPIVideoSender*> it( m_resolvedVideoSenders );
    while( it.hasNext() ){
        RPIVideoSender* vid = it.next();
        if( vid->name() == name ){
            Q_EMIT rpiVideoSenderRtspWentAway( vid->name() );
            it.remove();
            vid->deleteLater();
        }
    }
}
