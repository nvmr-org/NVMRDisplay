#ifndef VIDEODISCOVER_H
#define VIDEODISCOVER_H

#include <QObject>
#include <QTimer>

class VideoDiscover : public QObject
{
    Q_OBJECT
public:
    explicit VideoDiscover(QObject *parent = nullptr);

signals:
    void newVideoFound( QString video );

private slots:
    void timerFired();

private:
    QTimer m_testTimer;
    int m_number;
};

#endif // VIDEODISCOVER_H
