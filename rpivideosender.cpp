#include "rpivideosender.h"
#include "nvmr-json/videosendermessage.h"

#include <QJsonDocument>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.RPIVideoSender" );

RPIVideoSender::RPIVideoSender( QString name, QString address, int port, QObject* parent ) :
    QObject( parent ),
    m_name( name )
{
    connect( &m_videoSender, &QWebSocket::binaryMessageReceived,
             this, &RPIVideoSender::binaryMessageRx );
    connect( &m_videoSender, &QWebSocket::connected,
             this, &RPIVideoSender::queryData );
    connect( &m_videoSender, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
             this, &RPIVideoSender::websocketError );

    QString toOpen = "ws://" + address + ":" + QString::number( port );
    m_websocketAddr = toOpen;
    connectToRPI();
}

bool RPIVideoSender::isValid() const {
    return m_videoSender.isValid();
}

void RPIVideoSender::binaryMessageRx( const QByteArray& binaryMsg ){
    QJsonDocument doc = QJsonDocument::fromJson( binaryMsg );
    if( doc.isNull() ){
        LOG4CXX_ERROR( logger, "Docment is null!" );
        return;
    }

    if( logger->isDebugEnabled() ){
        std::string debugDoc = doc.toJson().toStdString();
        LOG4CXX_DEBUG( logger, "Got document: " << debugDoc );
    }

    m_lastValidVideoInfo = std::shared_ptr<VideoSenderMessage>( new VideoSenderMessage( doc.object() ) );
    m_videoSource.setPort( m_lastValidVideoInfo->configuration().networkSettings().udpPort() );
    m_videoSource.setAddress( m_lastValidVideoInfo->configuration().networkSettings().udpHost() );

    Q_EMIT videoSourceChanged();
}

void RPIVideoSender::queryData(){
    m_reconnectTimer.stop();

    VideoSenderMessage senderMsg;
    senderMsg.setCommand( "query" );
    QJsonDocument doc( senderMsg.jsonObj() );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}

void RPIVideoSender::websocketError( QAbstractSocket::SocketError err ){
    LOG4CXX_ERROR( logger, "Websocket error: " << err );
    m_videoSender.close();
    m_reconnectTimer.start( 10000 );
}

void RPIVideoSender::connectToRPI(){
    m_videoSender.open( m_websocketAddr );
}

QString RPIVideoSender::name() const {
    return m_name;
}

int RPIVideoSender::videoId() const {
    return m_lastValidVideoInfo->configuration().videoSettings().id();
}

VideoSource* RPIVideoSender::getVideoSource() {
    return &m_videoSource;
}

const std::shared_ptr<const VideoSenderMessage> RPIVideoSender::lastValidInfo() const{
    return m_lastValidVideoInfo;
}

void RPIVideoSender::setNewSettings(VideoSenderConfiguration settings){
    VideoSenderMessage vidsendMsg;
    vidsendMsg.setCommand( "set" );

    vidsendMsg.mutuable_configuration() = settings;
    QJsonDocument doc( vidsendMsg.jsonObj() );

    m_lastValidVideoInfo = std::shared_ptr<VideoSenderMessage>( new VideoSenderMessage( doc.object() ) );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}

void RPIVideoSender::requestRestart() {
    VideoSenderMessage msg;
    msg.setCommand( "restart" );

    QJsonDocument doc( msg.jsonObj() );
    m_videoSender.sendBinaryMessage( doc.toJson() );
}
