#ifndef H264BIN_H
#define H264BIN_H

#include <QObject>

#include <gst/gst.h>

class H264Bin : public QObject
{
    Q_OBJECT
public:
    explicit H264Bin(QObject *parent = nullptr);
    ~H264Bin();

private:
    GstElement* m_bin;
};

#endif // H264BIN_H
