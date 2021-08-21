#include "senderconfigurator.h"
#include "ui_senderconfigurator.h"

#include "avahibrowse.h"
#include "nvmr-json/videosendermessage.h"

#include <QJsonDocument>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.SenderConfigurator" );

SenderConfigurator::SenderConfigurator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SenderConfigurator),
    m_current( nullptr )
{
    ui->setupUi(this);
    ui->videoSenderSelection->addItem( "" );
}

SenderConfigurator::~SenderConfigurator()
{
    delete ui;
}

void SenderConfigurator::setAvahiBrowser(AvahiBrowse *avahi){
    m_avahi = avahi;

    connect( m_avahi, &AvahiBrowse::rpiVideoSenderFound,
             this, &SenderConfigurator::rpiVideoSenderFound );
    connect( m_avahi, &AvahiBrowse::rpiVideoSenderWentAway,
             this, &SenderConfigurator::rpiVideoSenderWentAway );
}

void SenderConfigurator::rpiVideoSenderFound( RPIVideoSender* sender ){
    m_videoSenders.push_back( sender );
    ui->videoSenderSelection->addItem( sender->name() );
}

void SenderConfigurator::rpiVideoSenderWentAway( RPIVideoSender* sender ){
    m_videoSenders.removeAll( sender );
    int idx = ui->videoSenderSelection->findText( sender->name() );
    ui->videoSenderSelection->removeItem( idx );
    ui->videoSenderSelection->setCurrentIndex( 0 );
}

void SenderConfigurator::on_saveButton_clicked()
{
    VideoSenderConfiguration config;
    config.mutable_videoSettings()
            .setId( ui->idSpin->value() )
            .setPt( ui->ptSpin->value() )
            .setWidth( ui->widthSpin->value() )
            .setHeight( ui->heightSpin->value() )
            .setName( ui->videoName->text() )
            .setFramerate( ui->framerateSpin->value() )
            .setConfigInterval( ui->configInterval->value() );
    config.mutable_networkSettings()
            .setUdpHost( ui->ipAddr->text() )
            .setUdpPort( ui->ipPort->value() )
            .setBroadcast( ui->broadcastEnabled->isChecked() );

    if( !m_current ){
        return;
    }

    m_current->setNewSettings( config );
}

void SenderConfigurator::on_restartButton_clicked()
{
    if( !m_current ){
        return;
    }

    m_current->requestRestart();
}

void SenderConfigurator::on_videoSenderSelection_activated(int index)
{
    if( index == 0 ){
        m_current = nullptr;
        ui->uuidLabel->setText( "" );
        ui->videoName->setText( "" );
        ui->heightSpin->setValue( 0 );
        ui->widthSpin->setValue( 0 );
        ui->ptSpin->setValue( 0 );
        ui->configInterval->setValue( 0 );
        ui->idSpin->setValue( 0 );
        ui->framerateSpin->setValue( 0 );
        ui->systemIpAddr->setText( "" );

        ui->ipAddr->setText( "" );
        ui->ipPort->setValue( 0 );
        return;
    }

    m_current = m_videoSenders[ index - 1 ];

    const std::shared_ptr<const VideoSenderMessage> lastMsg = m_current->lastValidInfo();
    if( !lastMsg ) return;
    ui->uuidLabel->setText( lastMsg->configuration().uuid() );
    ui->videoName->setText( lastMsg->configuration().videoSettings().name() );
    ui->heightSpin->setValue( lastMsg->configuration().videoSettings().height() );
    ui->widthSpin->setValue( lastMsg->configuration().videoSettings().width() );
    ui->ptSpin->setValue( lastMsg->configuration().videoSettings().pt() );
    ui->configInterval->setValue( lastMsg->configuration().videoSettings().configInterval() );
    ui->idSpin->setValue( lastMsg->configuration().videoSettings().id() );
    ui->framerateSpin->setValue( lastMsg->configuration().videoSettings().framerate() );
    ui->systemIpAddr->setText( m_current->ipAddress() );

    ui->ipAddr->setText( lastMsg->configuration().networkSettings().udpHost() );
    ui->ipPort->setValue( lastMsg->configuration().networkSettings().udpPort() );
    ui->broadcastEnabled->setChecked( lastMsg->configuration().networkSettings().broadcast() );
}
