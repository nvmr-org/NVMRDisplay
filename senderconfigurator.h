#ifndef SENDERCONFIGURATOR_H
#define SENDERCONFIGURATOR_H

#include <QWidget>

namespace Ui {
class SenderConfigurator;
}

class SenderConfigurator : public QWidget
{
    Q_OBJECT

public:
    explicit SenderConfigurator(QWidget *parent = nullptr);
    ~SenderConfigurator();

private:
    Ui::SenderConfigurator *ui;
};

#endif // SENDERCONFIGURATOR_H
