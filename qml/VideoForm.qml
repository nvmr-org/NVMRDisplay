import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import org.nvmr.videodisplay 1.0

//        Video {
//            id: video1
//            anchors.fill: parent;
//            source: "rtsp://192.168.1.41:8554/test"
//            autoPlay: true

//            focus: true
//        }

Page {
    width: 600
    height: 400

    Row {
        id: layout
        anchors.fill: parent
        spacing: 2
        Rectangle {
            id: mainVideo
            color: "red"
            width: parent.width * .8
            height: parent.height
        }
        Rectangle {
            color: "green"
            width: parent.width * .2
            height: parent.height
            Column {
                id: otherVideos
            }
        }
    }

    function videoClicked(video){
        console.log( "clicked video " + video.text )

        if( video.parent != mainVideo ){
            // Move the main video to the side
            for (var i = 0; i < mainVideo.children.length; i++)
            {
                var videoToReparent = mainVideo.children[i]
                videoToReparent.anchors.fill = undefined
                videoToReparent.width = otherVideos.width
                videoToReparent.parent = otherVideos
            }

            video.parent = mainVideo
            video.width = video.parent.width
            video.anchors.fill = video.parent
        }
    }

    Connections {
        target: serviceDiscover
        function onRpiVideoSenderRtspFound( url ){
            console.log( "new video! " + url )

            var component = Qt.createComponent("qrc:/qml/NVMRVideo.qml");
            var video = component.createObject(otherVideos);
//            video.text = bbb
            video.videoSource = url

            video.onVideoClicked.connect(videoClicked)
        }
    }

}
