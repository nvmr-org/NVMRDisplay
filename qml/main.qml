import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("NVMR Video Display")
    visibility: "FullScreen"
    id: mainWindow

    // private property
    property bool settingsOpen: false

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        WebForm {
            id: webPage
            pageNumber: 0
        }

        WebForm {
            id: webPage2
            pageNumber: 1
        }

        VideoForm {
            id: videoPage
        }

        SettingsPage {
            id: settingsPage
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

    Component {
        id: tabButton
        TabButton { }
    }

    Connections{
        target: videoPage
        function onOpenSettings() {
            if(settingsOpen) return

            var tab = tabButton.createObject(tabBar, {text: "Settings"})
            tabBar.addItem(tab)
            settingsOpen = true
        }
    }
}
