#ifndef RPIVIDEODISPLAY_H
#define RPIVIDEODISPLAY_H

#include <QObject>
#include <QQuickWidget>

#include "rpivideosender.h"

class DisplayBin;

class RPIVideoDisplay : public QObject
{
    Q_OBJECT
public:
    explicit RPIVideoDisplay(RPIVideoSender* vidsend,
                             QObject *parent = nullptr);
    ~RPIVideoDisplay();

    int videoId() const;
    QQuickWidget* widget() const;

Q_SIGNALS:

public Q_SLOTS:
    void play();
    void stop();
    void checkRotation();

private:
    RPIVideoSender* m_videoSender;
    QQuickWidget* m_videoWidget;
    DisplayBin* m_displayBin;
    GstElement* m_pipeline;
};

#endif // RPIVIDEODISPLAY_H
