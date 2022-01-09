import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import QtQml 2.12
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

    // private property
    property int timesPressed: 0;
    signal openSettings()

    Timer {
        id: pressedTimer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            timesPressed = 0
        }
    }

    Row {
        id: layout
        anchors.fill: parent
        spacing: 2
        Rectangle {
            id: mainVideo
            color: "#000000"
            width: parent.width * .8
            height: parent.height
        }
        Rectangle {
            id: allVideos
            color: "#c2c2c2"
            width: parent.width * .2
            height: parent.height
            Column {
                id: otherVideos
                width: parent.width
                Text{
                    text: "All Videos"
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.underline: true
                    fontSizeMode: Text.HorizontalFit
                    font.pointSize: 52

                    MouseArea{
                        width: parent.width
                        onPressed: {
                            timesPressed++
                            if( timesPressed > 3 ){
                                console.log("oepn the settings")
                                openSettings()
                                timesPressed = 0
                                pressedTimer.stop()
                                return
                            }
                            pressedTimer.start()
                        }

                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                    }
                }
            }
        }
    }

    function videoClicked(video){
        console.log( "clicked video " + video.videoName )

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
        function onRpiVideoSenderRtspFound( url, name ){
            console.log( "new video! " + url )

            var component = Qt.createComponent("qrc:/qml/NVMRVideo.qml");
            var video = component.createObject(otherVideos);
            video.videoName = name
            video.videoSource = url

            video.onVideoClicked.connect(videoClicked)
        }
    }

}
