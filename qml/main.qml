import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Tabs")
    visibility: "FullScreen"
    id: mainWindow

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        WebForm {
            id: webPage
        }

        VideoForm {
            id: videoPage
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Panel")
        }
        TabButton {
            text: qsTr("Video")
        }
    }
}
