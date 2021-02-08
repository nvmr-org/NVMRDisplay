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
}

void SenderConfigurator::videoSendersUpdated(){
    LOG4CXX_DEBUG( logger, "Video senders updated" );
//    QVector<VideoSender> senders = m_avahi->getVideoSenders();

//    ui->videoSenderSelection->clear();
//    ui->videoSenderSelection->addItem( "" );
//    for( VideoSender sender : senders ){
//        ui->videoSenderSelection->addItem( sender.name );
//    }
}

void SenderConfigurator::on_videoSenderSelection_activated(const QString &arg1)
{
    // User has selected a new VideoSender to configure
    m_videoSender.close();

//    VideoSender vs = m_avahi->getVideoSenderByID( arg1 );

//    LOG4CXX_DEBUG( logger, "Going to query information for video sender "
//                   << arg1.toStdString()
//                   << " with address "
//                   << vs.address.toStdString()
//                   << ":"
//                   << vs.port );

//    if( vs.address.isEmpty() ){
//        return;
//    }

//    QString toOpen = "ws://" + vs.address + ":" + QString::number( vs.port );
//    m_videoSender.open( toOpen );
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

    VideoSenderMessage videoSenderMsg( doc.object() );
    ui->uuidLabel->setText( videoSenderMsg.configuration().uuid() );
    ui->videoName->setText( videoSenderMsg.configuration().videoSettings().name() );
    ui->heightSpin->setValue( videoSenderMsg.configuration().videoSettings().height() );
    ui->widthSpin->setValue( videoSenderMsg.configuration().videoSettings().width() );
    ui->ptSpin->setValue( videoSenderMsg.configuration().videoSettings().pt() );
    ui->configInterval->setValue( videoSenderMsg.configuration().videoSettings().configInterval() );
    ui->idSpin->setValue( videoSenderMsg.configuration().videoSettings().id() );

    ui->ipAddr->setText( videoSenderMsg.configuration().networkSettings().udpHost() );
    ui->ipPort->setValue( videoSenderMsg.configuration().networkSettings().udpPort() );
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

void SenderConfigurator::on_saveButton_clicked()
{
    VideoSenderMessage saveMessage;

    saveMessage.setCommand( "set" );
    saveMessage.mutuable_configuration().mutable_videoSettings()
            .setId( ui->idSpin->value() )
            .setPt( ui->ptSpin->value() )
            .setWidth( ui->widthSpin->value() )
            .setHeight( ui->heightSpin->value() )
            .setName( ui->videoName->text() )
            .setFramerate( ui->framerateSpin->value() )
            .setConfigInterval( ui->configInterval->value() );
    saveMessage.mutuable_configuration().mutable_networkSettings()
            .setUdpHost( ui->ipAddr->text() )
            .setUdpPort( ui->ipPort->value() );

    QJsonDocument doc( saveMessage.jsonObj() );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}

void SenderConfigurator::on_restartButton_clicked()
{

    VideoSenderMessage saveMessage;

    saveMessage.setCommand( "restart" );

    QJsonDocument doc( saveMessage.jsonObj() );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}
