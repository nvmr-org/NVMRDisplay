import QtQuick 2.0
import QtMultimedia 5.12
import QtQuick.Controls 2.15
import QtQml 2.15

Item {
    id: mainItem
    width: 200;
    height: width
    property string videoName
    property url videoSource

    GroupBox {
        title: mainItem.videoName
        anchors.fill: parent;
        label: Label {
                x: parent.leftPadding
                width: parent.availableWidth
                text: parent.title
                color: "#ffffff"
                elide: Text.ElideRight
        }

        Video {
            id: video1
            anchors.fill: parent;
            source: mainItem.videoSource
            autoPlay: true
            playbackRate: qt.platform == "android" ? 1.5 : 1.0
            focus: true
        }
    }

    signal videoClicked(var the_object)

    MouseArea {
        width: parent.width
        height: parent.height
        anchors.fill: parent
        onClicked: function(){ videoClicked(this.parent) }
    }

}
