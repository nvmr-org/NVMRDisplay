#ifndef DISPLAYBIN_H
#define DISPLAYBIN_H

#include <QObject>
#include <QQuickWidget>
#include <gst/gst.h>

class DisplayBin : public QObject
{
    Q_OBJECT
public:
    explicit DisplayBin(QObject *parent = nullptr);
    ~DisplayBin();

    GstElement* bin();

    void linkToQQuickWidget( QQuickWidget* widget );

signals:

public slots:
private:
    GstElement* m_bin;
    GstElement* m_qml;
};

#endif // DISPLAYBIN_H
