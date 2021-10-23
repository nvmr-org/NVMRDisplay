#ifndef SOOSPYSOURCEBIN_H
#define SOOSPYSOURCEBIN_H

#include <QObject>
#include <gst/gst.h>

#include "videosource.h"

class SoospySourceBin : public VideoSource
{
    Q_OBJECT
    Q_PROPERTY( QString location READ location WRITE setLocation )
    Q_PROPERTY( QString userId READ userId WRITE setUserId )
    Q_PROPERTY( QString userPassword READ userPassword WRITE setUserPassword )
public:
    explicit SoospySourceBin(QObject *parent = nullptr);
    ~SoospySourceBin();

    virtual void writeToQSettings( QSettings* settings );
    virtual void readFromQSettings( QSettings* settings );
    virtual SourceType sourceType() const;
    virtual GstElement* getBin();

    QString location() const;
    void setLocation( QString location );
    QString userId() const;
    void setUserId( QString id );
    QString userPassword() const;
    void setUserPassword( QString str );

private:
    static void pad_added_handler_cb (GstElement *src, GstPad *pad, void *data);
    void pad_added_handler( GstElement* src, GstPad* pad );

private:
    GstElement* m_bin;
    QString m_location;
    QString m_userId;
    QString m_userPassword;
};

#endif // SOOSPYSOURCEBIN_H
