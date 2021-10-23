#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QNetworkInterface>
#include <QQmlApplicationEngine>
#include <QtGlobal>
#ifndef Q_OS_ANDROID
#include <qtwebengineglobal.h>
#endif
#include <QTimer>
#include "videodiscover.h"

static QQmlApplicationEngine* local_engine;

static void printObjectsAndChildren( QObject* inObj ){
    qDebug() << "In obj name: " << inObj->objectName();
    for( QObject* obj : inObj->children() ){
        qDebug() << "Name: " << obj->objectName();
    }
}

int main(int argc, char *argv[])
{
#ifndef Q_OS_ANDROID
    QtWebEngine::initialize();
#endif /* Q_OS_ANDROID */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<VideoDiscover>("org.nvmr.videodisplay", 1, 0, "VideoDiscover");

    VideoDiscover vidDiscover;
    QQmlApplicationEngine engine;
    local_engine = &engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("initialUrl"),
                                "http://example.com");
    context->setContextProperty(QStringLiteral("videoDiscover"), &vidDiscover);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        qDebug() << "Object created: " << objUrl;
//        QTimer::singleShot( 100, startedUp );
    }, Qt::QueuedConnection);
    engine.load(url);


//    qDebug() << "root objects size: " << engine.rootObjects().size();
//    for( QObject* obj : engine.rootObjects() ){
//        printObjectsAndChildren( obj );
//    }

    return app.exec();
}
