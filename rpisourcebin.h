#ifndef RPISOURCEBIN_H
#define RPISOURCEBIN_H

#include <QObject>
#include <gst/gst.h>

class RPISourceBin : public QObject
{
    Q_OBJECT
public:
    explicit RPISourceBin(QObject *parent = nullptr);
    ~RPISourceBin();

signals:

public slots:
private:
    GstElement* m_bin;
};

#endif // RPISOURCEBIN_H
