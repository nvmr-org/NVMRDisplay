#ifndef RPISOURCEBIN_H
#define RPISOURCEBIN_H

#include <QObject>
#include <gst/gst.h>

#include "videosource.h"

class RPISourceBin : public VideoSource
{
    Q_OBJECT
    Q_PROPERTY( int port READ port WRITE setPort )
public:
    explicit RPISourceBin(QObject *parent = nullptr);
    ~RPISourceBin();

    virtual void writeToQSettings( QSettings* settings );
    virtual void readFromQSettings( QSettings* settings );
    virtual SourceType sourceType() const;
    virtual GstElement* getBin();

    int port() const;

    void setPort( int port );

signals:

public slots:
private:
    GstElement* m_bin;
    int m_port;
};

#endif // RPISOURCEBIN_H
