#include "nvmrdisplay.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QTimer>

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <gst/gst.h>

#include "avahibrowse.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.nvmrdisplaymain" );

static void initialize_logging(){
    QString configLocation =
                QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ).first();

    QDir d( configLocation );
    if( !d.exists() ){
        d.mkpath( "." );
    }

    QString logconfigFile = configLocation + "/logconfig.xml";
    log4cxx::xml::DOMConfigurator::configure( logconfigFile.toStdString() );
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName( "NVMR" );

    initialize_logging();

    LOG4CXX_INFO( logger, "NVMR Display starting up" );

    gst_init( nullptr, nullptr );

    // Register qmlglsink(needed for reasons)
    GstElement *sink = gst_element_factory_make ("qmlglsink", NULL);
    gst_object_unref(sink);

    AvahiBrowse br;
    QTimer::singleShot( 0, &br, &AvahiBrowse::initialize );

    NVMRDisplay w;
    w.setAvahiBrowser( &br );
    {
        QSettings settings;
        if( settings.value( "fullscreen", "true" ).toBool() ){
            w.setWindowState( Qt::WindowFullScreen );
        }
    }
    w.show();

    return a.exec();
}
