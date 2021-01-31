#ifndef SENDERCONFIGURATOR_H
#define SENDERCONFIGURATOR_H

#include <QWidget>
#include <QWebSocket>

namespace Ui {
class SenderConfigurator;
}

class AvahiBrowse;

class SenderConfigurator : public QWidget
{
    Q_OBJECT

public:
    explicit SenderConfigurator(QWidget *parent = nullptr);
    ~SenderConfigurator();

    void setAvahiBrowser( AvahiBrowse* avahi );

private Q_SLOTS:
    void videoSendersUpdated();

    void on_videoSenderSelection_activated(const QString &arg1);

    void binaryMessageRx( const QByteArray& binaryMsg );

    void queryData();

    void websocketError( QAbstractSocket::SocketError err );

    void on_saveButton_clicked();

    void on_restartButton_clicked();

private:
    Ui::SenderConfigurator *ui;
    AvahiBrowse* m_avahi;
    QWebSocket m_videoSender;
};

#endif // SENDERCONFIGURATOR_H
