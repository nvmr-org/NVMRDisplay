#include "videosource.h"

VideoSource::VideoSource(QObject *parent) : QObject(parent)
{

}

void VideoSource::writeSourceType( QSettings* settings, SourceType type ){
    QString videoType;

    if( type == SourceType::Soospy ){
        videoType = "soospy";
    }else{
        videoType = "RPi";
    }

    settings->setValue( "video-type", videoType );
}
