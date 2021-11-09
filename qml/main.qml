import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("NVMR Video Display")
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

        WebForm {
            id: webPage2
        }

        VideoForm {
            id: videoPage
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Panel 1")
        }
        TabButton {
            text: qsTr("Panel 2")
        }
        TabButton {
            text: qsTr("Video")
        }
    }
}
