#include "videosource.h"

VideoSource::VideoSource(QObject *parent) :
    QObject(parent),
    m_videoId( 1 )
{

}

void VideoSource::writeCommon( QSettings* settings, SourceType type ){
    QString videoType;

    if( type == SourceType::Soospy ){
        videoType = "soospy";
    }else{
        videoType = "RPi";
    }

    settings->setValue( "video-type", videoType );
    settings->setValue( "video-id", m_videoId );
}

void VideoSource::setVideoId( int id ){
    m_videoId = id;
}

int VideoSource::videoId(){
    return m_videoId;
}

void VideoSource::readVideoId(QSettings *settings){
    bool ok;
    int value = settings->value( "video-id" ).toInt( &ok );
    if( !ok ){
        return;
    }

    m_videoId = value;
}
