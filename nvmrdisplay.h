#ifndef NVMRDISPLAY_H
#define NVMRDISPLAY_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QVector>
#include <QGridLayout>
#include <ostream>

#include "videosource.h"
#include "rpivideosender.h"

namespace Ui {
class NVMRDisplay;
}

class AvahiBrowse;

enum class CurrentCommand{
    NoCommand,
    ViewCamera,
    AddCamera,
    ViewWebpage
};

class NVMRDisplay : public QMainWindow
{
    Q_OBJECT

public:
    explicit NVMRDisplay(QWidget *parent = nullptr);
    ~NVMRDisplay();

    void clearVideos();

    void keyPressEvent( QKeyEvent* event ) override;

    void setAvahiBrowser( AvahiBrowse* avahi );

private:
    void playVideo( QQuickWidget* widget );
    bool isKeySpecial( QKeyEvent* event ) const;
    bool functionKeyHandled( QKeyEvent* event );

private Q_SLOTS:
    void newRPIVideoSender( RPIVideoSender* vidsend );
    void rpiVideoSenderWentAway( RPIVideoSender* vidsend );
    void rpiVideoSenderInfoChanged();

private:
    Ui::NVMRDisplay *ui;
    int m_numberEntered;
    CurrentCommand m_currentCommand;
    QVector<QQuickWidget*> m_videos;
    QGridLayout m_mainLayout;
//    QMap<int,VideoSource*> m_videoSources;
    QMap<int,QUrl> m_wepageSources;
    AvahiBrowse* m_avahiBrowse;
    QVector<RPIVideoSender*> m_rpiVideoSenders;
};

inline std::ostream& operator<<( std::ostream& stream, CurrentCommand cmd ){
    switch( cmd ){
    case CurrentCommand::NoCommand:
        stream << "NoCommand";
        break;
    case CurrentCommand::ViewCamera:
        stream << "ViewCamera";
        break;
    case CurrentCommand::AddCamera:
        stream << "AddCamera";
        break;
    case CurrentCommand::ViewWebpage:
        stream << "ViewWebpage";
        break;
    }

    return stream;
}

#endif // NVMRDISPLAY_H
