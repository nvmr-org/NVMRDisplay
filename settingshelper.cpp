#include <QDebug>
#include "settingshelper.h"

SettingsHelper::SettingsHelper(QObject *parent) : QObject(parent)
{

}

void SettingsHelper::setDefaultURL( QString defaultURL ){
    m_defaultURL = defaultURL;
}

QString SettingsHelper::defaultPageForID(int id){
    QString key = QString( "url/number_%1" ).arg(id);
    return m_settings.value( key, m_defaultURL ).toString();
}

void SettingsHelper::cacheDefaultPageForID(int id, QString url){
    m_currentURLs[id] = url;
}

void SettingsHelper::saveDefaultPageForID(int id){
    QString key = QString( "url/number_%1" ).arg(id);
    m_settings.setValue(key, m_currentURLs[id] );
}

void SettingsHelper::clearDefaultPageForID(int id){
    QString key = QString( "url/number_%1" ).arg(id);
    m_settings.remove(key);
}
