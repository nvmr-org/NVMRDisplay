#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QObject>
#include <QSettings>

class SettingsHelper : public QObject
{
    Q_OBJECT
public:
    explicit SettingsHelper(QObject *parent = nullptr);

    void setDefaultURL( QString defaultURL );

    Q_INVOKABLE QString defaultPageForID(int id);
    Q_INVOKABLE void cacheDefaultPageForID(int id, QString url);
    Q_INVOKABLE void saveDefaultPageForID(int id);
    Q_INVOKABLE void clearDefaultPageForID(int id);

Q_SIGNALS:

private:
    QSettings m_settings;
    QString m_defaultURL;
    QMap<int, QString> m_currentURLs;
};

#endif // SETTINGSHELPER_H
