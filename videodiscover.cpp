#include "videodiscover.h"

#include <QDebug>

VideoDiscover::VideoDiscover(QObject *parent) : QObject(parent)
{
    m_number = 0;

    m_testTimer.start( 5000 );

    connect( &m_testTimer, &QTimer::timeout,
             this, &VideoDiscover::timerFired );
}


void VideoDiscover::timerFired(){
    if( m_number > 3 ){
        m_testTimer.stop();
        return;
    }

    qDebug() << "New video found!";
    emit newVideoFound( QString("video %1").arg( m_number ) );
    m_number++;
}
