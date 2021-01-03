#include "senderconfigurator.h"
#include "ui_senderconfigurator.h"

#include "avahibrowse.h"
#include "nvmr-json/videosendermessage.h"

#include <QJsonDocument>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.SenderConfigurator" );

SenderConfigurator::SenderConfigurator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SenderConfigurator)
{
    ui->setupUi(this);

    connect( &m_videoSender, &QWebSocket::binaryMessageReceived,
             this, &SenderConfigurator::binaryMessageRx );
    connect( &m_videoSender, &QWebSocket::connected,
             this, &SenderConfigurator::queryData );
    connect( &m_videoSender, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
             this, &SenderConfigurator::websocketError );
}

SenderConfigurator::~SenderConfigurator()
{
    delete ui;
}

void SenderConfigurator::setAvahiBrowser(AvahiBrowse *avahi){
    m_avahi = avahi;
    connect( m_avahi, &AvahiBrowse::videoSendersUpdated,
             this, &SenderConfigurator::videoSendersUpdated );
}

void SenderConfigurator::videoSendersUpdated(){
    LOG4CXX_DEBUG( logger, "Video senders updated" );
    QVector<VideoSender> senders = m_avahi->getVideoSenders();

    ui->videoSenderSelection->clear();
    ui->videoSenderSelection->addItem( "" );
    for( VideoSender sender : senders ){
        ui->videoSenderSelection->addItem( sender.name );
    }
}

void SenderConfigurator::on_videoSenderSelection_activated(const QString &arg1)
{
    // User has selected a new VideoSender to configure
    m_videoSender.close();

    VideoSender vs = m_avahi->getVideoSenderByID( arg1 );

    LOG4CXX_DEBUG( logger, "Going to query information for video sender "
                   << arg1.toStdString()
                   << " with address "
                   << vs.address.toStdString()
                   << ":"
                   << vs.port );

    if( vs.address.isEmpty() ){
        return;
    }

    QString toOpen = "ws://" + vs.address + ":" + QString::number( vs.port );
    m_videoSender.open( toOpen );
}

void SenderConfigurator::binaryMessageRx( const QByteArray& msg ){
    QJsonDocument doc = QJsonDocument::fromJson( msg );
    if( doc.isNull() ){
        LOG4CXX_ERROR( logger, "Docment is null!" );
        return;
    }

    if( logger->isDebugEnabled() ){
        std::string debugDoc = doc.toJson().toStdString();
        LOG4CXX_DEBUG( logger, "Got document: " << debugDoc );
    }
}

void SenderConfigurator::queryData(){
    LOG4CXX_TRACE( logger, "Querying data" );

    VideoSenderMessage senderMsg;
    senderMsg.setCommand( "query" );
    QJsonDocument doc( senderMsg.jsonObj() );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}

void SenderConfigurator::websocketError(QAbstractSocket::SocketError err ){
    LOG4CXX_ERROR( logger, "websocket error: " << err );
}
