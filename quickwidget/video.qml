import QtQuick 2.0
import org.freedesktop.gstreamer.GLVideoItem 1.0

Item {

    Rectangle {
        color: "#000000"
        width: parent.width
        height: parent.height

        GstGLVideoItem {
            id: video
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            visible: true
        }
    }

}
