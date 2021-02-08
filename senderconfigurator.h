#ifndef SENDERCONFIGURATOR_H
#define SENDERCONFIGURATOR_H

#include <QWidget>

namespace Ui {
class SenderConfigurator;
}

class AvahiBrowse;
class RPIVideoSender;

class SenderConfigurator : public QWidget
{
    Q_OBJECT

public:
    explicit SenderConfigurator(QWidget *parent = nullptr);
    ~SenderConfigurator();

    void setAvahiBrowser( AvahiBrowse* avahi );

private Q_SLOTS:
    void rpiVideoSenderFound( RPIVideoSender* vidsend );
    void rpiVideoSenderWentAway( RPIVideoSender* vidsend );

    void on_saveButton_clicked();

    void on_restartButton_clicked();

    void on_videoSenderSelection_activated(int index);

private:
    Ui::SenderConfigurator *ui;
    AvahiBrowse* m_avahi;
    RPIVideoSender* m_current;
    QVector<RPIVideoSender*> m_videoSenders;
};

#endif // SENDERCONFIGURATOR_H
