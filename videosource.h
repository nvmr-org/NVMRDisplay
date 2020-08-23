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

    virtual SourceType sourceType() const = 0;

    virtual GstElement* getBin() = 0;

signals:

public slots:
protected:
    void writeSourceType( QSettings* settings, SourceType type );
};

#endif // VIDEOSOURCE_H
