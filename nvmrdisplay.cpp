#include "nvmrdisplay.h"
#include "ui_nvmrdisplay.h"
#include "rpisourcebin.h"
#include "displaybin.h"
#include "soospysourcebin.h"
#include "avahibrowse.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QRunnable>
#include <QLayout>
#include <QPushButton>
#include <QTabBar>

#include <log4cxx/logger.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.nvmrdisplay" );

NVMRDisplay::NVMRDisplay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NVMRDisplay),
    m_numberEntered( 0 ),
    m_currentCommand( CurrentCommand::NoCommand ),
    m_mainLayout( nullptr )
{
    int defaultVideo = 0;
    int defaultUrl = 0;

    ui->setupUi(this);

    ui->centralWidget->setLayout( &m_mainLayout );
    ui->tabWidget->tabBar()->hide();

    // Load all of our video sources from the config file
    QSettings settings;
    settings.beginGroup( "video" );
    defaultVideo = settings.value( "default" ).toInt();
    int size = settings.beginReadArray( "videos" );
    for( int x = 0; x < size; x++ ){
        settings.setArrayIndex( x );
        QString videoType = settings.value( "video-type" ).toString();
        VideoSource* vidSrc = nullptr;
        if( videoType == "soospy" ){
            vidSrc = new SoospySourceBin( this );
        }else if( videoType == "RPi" ){
            vidSrc = new RPISourceBin( this );
        }else{
            continue;
        }

        vidSrc->readFromQSettings( &settings );
        //m_videoSources[ vidSrc->videoId() ] = vidSrc;
        LOG4CXX_DEBUG( logger, "read video ID " << vidSrc->videoId() );
    }
    settings.endArray();
    settings.endGroup();

    // Load all of our URL sources from the config file
    settings.beginGroup( "web" );
    defaultUrl = settings.value( "default" ).toInt();
    size = settings.beginReadArray( "urls" );
    for( int x = 0; x < size; x++ ){
        bool ok;
        settings.setArrayIndex( x );
        int urlId = settings.value( "url-id" ).toInt( &ok );
        if( !ok ){
            continue;
        }
        m_wepageSources[ urlId ] = settings.value( "url" ).toUrl();
    }
    settings.endArray();
    settings.endGroup();

    // TODO load the default video and the default URL
}

NVMRDisplay::~NVMRDisplay()
{
    delete ui;
}

void NVMRDisplay::keyPressEvent(QKeyEvent *event){
    if( functionKeyHandled( event ) ){
        return;
    }

    if( m_currentCommand == CurrentCommand::NoCommand &&
            isKeySpecial( event ) ){
        LOG4CXX_TRACE( logger, "Got special key" );
        if( event->key() == Qt::Key_Asterisk ){
            m_currentCommand = CurrentCommand::ViewCamera;
        }else if( event->key() == Qt::Key_Plus ){
            m_currentCommand = CurrentCommand::AddCamera;
        }else if( event->key() == Qt::Key_Period ){
            m_currentCommand = CurrentCommand::ViewWebpage;
        }
    }else if( m_currentCommand != CurrentCommand::NoCommand &&
              isKeySpecial( event ) ){
        // This is not a key we are allowed to press at the moment!
        // TODO need to alert the user and reset?
        m_numberEntered = 0;
        return;
    }

    if( event->key() >= Qt::Key_0 &&
            event->key() <= Qt::Key_9 &&
            m_currentCommand != CurrentCommand::NoCommand ){
        m_numberEntered *= 10;
        m_numberEntered += (event->key() - Qt::Key_0);
    }

    if( event->key() == Qt::Key_Enter ){
        LOG4CXX_DEBUG( logger, "Do command " << m_currentCommand << " with number entered " << m_numberEntered );

        switch( m_currentCommand ){
        case CurrentCommand::ViewCamera:
        {
            // Show only the one camera
            RPIVideoDisplay* displayToUse = nullptr;
            for( RPIVideoDisplay* display : m_availableRPI ){
                if( display->videoId() == m_numberEntered ){
                    displayToUse = display;
                    break;
                }
            }

            if( !displayToUse ){
                break;
            }

            // Remove anything that is currently playing
            for( RPIVideoDisplay* display : m_currentlyShowingVideos ){
                display->stop();
                display->widget()->setVisible( false );
            }
            m_currentlyShowingVideos.clear();

            m_currentlyShowingVideos.push_back( displayToUse );
            displayToUse->widget()->setVisible( true );
            displayToUse->play();
            break;
        }
        case CurrentCommand::AddCamera:
        {
            RPIVideoDisplay* displayToUse = nullptr;
            for( RPIVideoDisplay* display : m_availableRPI ){
                if( display->videoId() == m_numberEntered ){
                    displayToUse = display;
                    break;
                }
            }

            if( !displayToUse ){
                // Invalid video entered - do nothing
                break;
            }

            // Remove the first video if we have too many videos
            if( m_currentlyShowingVideos.size() > 3 ){
                m_currentlyShowingVideos[0]->stop();
                m_currentlyShowingVideos[0]->widget()->setVisible(false);
            }

            m_currentlyShowingVideos.push_back( displayToUse );
//            m_mainLayout.addWidget( displayToUse->widget() );
            displayToUse->widget()->setVisible( true );
            displayToUse->play();

            break;
        }
        }

        m_currentCommand = CurrentCommand::NoCommand;
        m_numberEntered = 0;
    }
}

bool NVMRDisplay::isKeySpecial( QKeyEvent* evt ) const {
    switch( evt->key() ){
    case Qt::Key_Asterisk:
    case Qt::Key_Backslash:
    case Qt::Key_Plus:
    case Qt::Key_Period:
        return true;
    default:
        return false;
    }
}



bool NVMRDisplay::functionKeyHandled( QKeyEvent* event ){
    if( event->key() == Qt::Key_F1 ){
        ui->tabWidget->setCurrentIndex( 0 );
        return true;
    }else if( event->key() == Qt::Key_F2 ){
        ui->tabWidget->setCurrentIndex( 1 );
        return true;
    }else if( event->key() == Qt::Key_F3 ){
        ui->tabWidget->setCurrentIndex( 2 );
        return true;
    }

    return false;
}

void NVMRDisplay::setAvahiBrowser(AvahiBrowse *avahi){
    m_avahiBrowse = avahi;
    ui->configuratorWidget->setAvahiBrowser( avahi );

    connect( avahi, &AvahiBrowse::rpiVideoSenderFound,
             this, &NVMRDisplay::newRPIVideoSender );
    connect( avahi, &AvahiBrowse::rpiVideoSenderWentAway,
             this, &NVMRDisplay::rpiVideoSenderWentAway );
}

void NVMRDisplay::newRPIVideoSender( RPIVideoSender* vidsend ){
    RPIVideoDisplay* rpiDisp = new RPIVideoDisplay( vidsend, this );
    m_availableRPI.push_back( rpiDisp );

    m_mainLayout.addWidget( rpiDisp->widget() );
    rpiDisp->widget()->setVisible(false);
    rpiDisp->widget()->sizePolicy().setHorizontalStretch(1);
    rpiDisp->widget()->sizePolicy().setVerticalStretch(1);

    LOG4CXX_DEBUG( logger, "Adding new RPI video sender" );

    connect( vidsend, &RPIVideoSender::videoSourceChanged,
             this, &NVMRDisplay::rpiVideoSenderInfoChanged );
}

void NVMRDisplay::rpiVideoSenderWentAway( RPIVideoSender* vidsend ){
    std::remove_if( std::begin( m_availableRPI ),
                    std::end( m_availableRPI ),
                    [vidsend]( RPIVideoDisplay* disp ){
        return disp->videoId() == vidsend->videoId();
    });

    // TODO remove video if playing

//    m_mainLayout.removeWidget( vidsend->widget() );
}

void NVMRDisplay::rpiVideoSenderInfoChanged(){
}
