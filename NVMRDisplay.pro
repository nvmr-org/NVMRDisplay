#-------------------------------------------------
#
# Project created by QtCreator 2020-08-17T18:31:34
#
#-------------------------------------------------

QT       += core gui quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NVMRDisplay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 gstreamer-plugins-base-1.0 gstreamer-video-1.0 liblog4cxx

SOURCES += \
        main.cpp \
        nvmrdisplay.cpp \
    videowidget.cpp \
    h264bin.cpp \
    rpisourcebin.cpp \
    videopipeline.cpp \
    videosource.cpp \
    displaybin.cpp \
    soospysourcebin.cpp

HEADERS += \
        nvmrdisplay.h \
    videowidget.h \
    h264bin.h \
    rpisourcebin.h \
    videopipeline.h \
    videosource.h \
    displaybin.h \
    soospysourcebin.h

FORMS += \
        nvmrdisplay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    quickwidget/video.qml

RESOURCES += \
    qmlwidgets.qrc
