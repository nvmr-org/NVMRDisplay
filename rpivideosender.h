#ifndef RPIVIDEOSENDER_H
#define RPIVIDEOSENDER_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <memory>

#include "rpisourcebin.h"
#include "nvmr-json/videosendermessage.h"

class RPIVideoSender : public QObject
{
    Q_OBJECT
public:
    RPIVideoSender( QString name, QString address, int port, QObject* parent = nullptr );

    bool isValid() const;

    VideoSource* getVideoSource();
    QString name() const;
    int videoId() const;
    const std::shared_ptr<const VideoSenderMessage> lastValidInfo() const;

    void setNewSettings( VideoSenderConfiguration settings );
    void requestRestart();
    QString ipAddress() const;

Q_SIGNALS:
    void videoSourceChanged();

private Q_SLOTS:
    void binaryMessageRx( const QByteArray& binaryMsg );
    void queryData();
    void websocketError( QAbstractSocket::SocketError err );
    void connectToRPI();
    void startStreamingVideo( int portNumber );

private:
    QString m_name;
    QWebSocket m_videoSender;
    QTimer m_reconnectTimer;
    QString m_websocketAddr;
    QString m_ipAddr;
    RPISourceBin m_videoSource;
    std::shared_ptr<VideoSenderMessage> m_lastValidVideoInfo;
};


#endif // RPIVIDEOSENDER_H
