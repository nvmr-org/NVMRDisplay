#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QNetworkInterface>
#include <QQmlApplicationEngine>
#include <QtGlobal>
#include <QTimer>
#include <memory>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#ifndef Q_OS_ANDROID
#include <qtwebengineglobal.h>
#include "avahibrowse.h"
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/spi/location/locationinfo.h>
#include <dbus-cxx/utility.h>
#else
#include "embeddedavahibrowse.h"
#endif

static void printObjectsAndChildren( QObject* inObj ){
    qDebug() << "In obj name: " << inObj->objectName();
    for( QObject* obj : inObj->children() ){
        qDebug() << "Name: " << obj->objectName();
    }
}

static std::unique_ptr<ServiceDiscover> createServiceDiscover(){
#ifdef Q_OS_ANDROID
    return std::make_unique<EmbeddedAvahiBrowse>();
#else
    return std::make_unique<AvahiBrowse>();
#endif
}

#ifndef Q_OS_ANDROID
static void dbus_logger(const char* logger_name,
                        const struct SL_LogLocation* location,
                        const enum SL_LogLevel level,
                        const char* log_string){
    log4cxx::LoggerPtr logger = log4cxx::LogManager::getLogger( logger_name );
    log4cxx::spi::LocationInfo inf( location->file,
                                    location->function,
                                    location->line_number);

    switch( level ){
    case SL_TRACE:
        logger->trace( log_string, inf );
        break;
    case SL_DEBUG:
        logger->debug( log_string, inf );
        break;
    case SL_INFO:
        logger->info( log_string, inf );
        break;
    case SL_WARN:
        logger->warn( log_string, inf );
        break;
    case SL_ERROR:
        logger->error( log_string, inf );
        break;
    case SL_FATAL:
        logger->fatal( log_string, inf );
        break;
    }
}
#endif

static void init_logging(){
#ifndef Q_OS_ANDROID
    log4cxx::xml::DOMConfigurator::configure( "logconfig.xml" );

    DBus::set_logging_function( dbus_logger );
#endif
}

int main(int argc, char *argv[])
{
#ifndef Q_OS_ANDROID
    QtWebEngine::initialize();
#endif /* Q_OS_ANDROID */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    init_logging();

    QGuiApplication app(argc, argv);
    app.setApplicationName( "VideoDisplay" );
    app.setOrganizationName( "NVMR" );

    qmlRegisterType<ServiceDiscover>("org.nvmr.videodisplay", 1, 0, "ServiceDiscover");

    std::unique_ptr<ServiceDiscover> sd = createServiceDiscover();
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    QSettings settings;

    QString noJMRI = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    noJMRI.append(QDir::separator());
    noJMRI.append("no-jmri.html");
    QFile(":/no-jmri.html").copy(noJMRI);
    context->setContextProperty(QStringLiteral("initialUrl"),
                                settings.value( "url", "file://" + noJMRI ));
    context->setContextProperty(QStringLiteral("serviceDiscover"), sd.get() );

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        qDebug() << "Object created: " << objUrl;
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
