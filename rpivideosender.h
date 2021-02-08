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

Q_SIGNALS:
    void videoSourceChanged();

private Q_SLOTS:
    void binaryMessageRx( const QByteArray& binaryMsg );
    void queryData();
    void websocketError( QAbstractSocket::SocketError err );
    void connectToRPI();

private:
    QString m_name;
    QWebSocket m_videoSender;
    QTimer m_reconnectTimer;
    QString m_websocketAddr;
    RPISourceBin m_videoSource;
    std::shared_ptr<VideoSenderMessage> m_lastValidVideoInfo;
};


#endif // RPIVIDEOSENDER_H
