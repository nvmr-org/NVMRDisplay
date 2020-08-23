import QtQuick 2.0
import org.freedesktop.gstreamer.GLVideoItem 1.0

Item {
    GstGLVideoItem {
            id: video
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            visible: true
        }
}
