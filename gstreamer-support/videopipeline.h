#ifndef VIDEOPIPELINE_H
#define VIDEOPIPELINE_H

#include <QObject>

#include <gst/gst.h>

class VideoPipeline : public QObject
{
    Q_OBJECT
public:
    explicit VideoPipeline(QObject *parent = nullptr);
    ~VideoPipeline();

public Q_SLOTS:
    void play();
    void stop();

private:
    GstElement* m_pipeline;
};

#endif // VIDEOPIPELINE_H
