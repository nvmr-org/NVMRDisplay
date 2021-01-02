#include "senderconfigurator.h"
#include "ui_senderconfigurator.h"

SenderConfigurator::SenderConfigurator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SenderConfigurator)
{
    ui->setupUi(this);
}

SenderConfigurator::~SenderConfigurator()
{
    delete ui;
}
