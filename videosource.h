#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QObject>
#include <QSettings>

#include <gst/gst.h>

enum class SourceType{
    Soospy,
    RPi
};

class VideoSource : public QObject
{
    Q_OBJECT
public:
    explicit VideoSource(QObject *parent = nullptr);

    virtual void writeToQSettings( QSettings* settings ) = 0;
    virtual void readFromQSettings( QSettings* settings ) = 0;
    virtual SourceType sourceType() const = 0;
    virtual GstElement* getBin() = 0;

    void setVideoId( int id );
    int videoId();

signals:

public slots:
protected:
    void writeCommon( QSettings* settings, SourceType type );
    void readVideoId( QSettings* settings );

private:
    int m_videoId;
};

#endif // VIDEOSOURCE_H
