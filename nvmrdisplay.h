#ifndef NVMRDISPLAY_H
#define NVMRDISPLAY_H

#include <QMainWindow>
#include <ostream>

namespace Ui {
class NVMRDisplay;
}

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

private slots:
    void doOpenglStuff();

private:
    bool isKeySpecial( QKeyEvent* event ) const;

private:
    Ui::NVMRDisplay *ui;
    int m_numberEntered;
    CurrentCommand m_currentCommand;
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
