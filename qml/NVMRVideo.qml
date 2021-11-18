import QtQuick 2.0
import QtMultimedia 5.12

Item {
    width: 200;
    height: width
    property string videoName
    property url videoSource

//    Rectangle{
//        width: parent.width
//        height: parent.height
//        anchors.fill: parent
//        color: "blue";
//    }
    Video {
        id: video1
        anchors.fill: parent;
        source: parent.videoSource
        autoPlay: true
        playbackRate: 1.5
        focus: true
    }

    signal videoClicked(var the_object)

//    Text{
//        text: parent.text
//    }

    MouseArea {
        width: parent.width
        height: parent.height
        anchors.fill: parent
        onClicked: function(){ videoClicked(this.parent) }
    }

}
