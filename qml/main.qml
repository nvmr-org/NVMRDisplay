import QtQuick 2.0
import QtQuick.Controls 2.2

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
            id: webPage0
            pageNumber: 0
        }

        WebForm {
            id: webPage1
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
            id: panel0Button
            text: qsTr("Panel 1")
        }
        TabButton {
            id: panel1Button
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

    Connections{
        target: webPage0
        function onUrl_titleChanged(text) {
            panel0Button.text = text
        }
    }

    Connections{
        target: webPage1
        function onUrl_titleChanged(text) {
            panel1Button.text = text
        }
    }
}
