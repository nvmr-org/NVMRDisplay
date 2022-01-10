import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import QtQml 2.12
import org.nvmr.videodisplay 1.0

Page {
    width: 600
    height: 400

    // private properties
    property int timesPressed: 0;
    property int quadVideoNumber: 0

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
            visible: false
        }

        Rectangle {
            id: quadVideo
            color: "#0000ff"
            width: parent.width * .8
            height: parent.height
            visible: true

            Grid{
                columns:  2
                width: parent.width
                height: parent.height
                Rectangle {
                    id: quad1
                    color: "#ff0000"
                    width: parent.width / 2
                    height: parent.height / 2
                }

                Rectangle {
                    id: quad2
                    color: "#00ff00"
                    width: parent.width / 2
                    height: parent.height / 2
                }

                Rectangle {
                    id: quad3
                    color: "#ff00ff"
                    width: parent.width / 2
                    height: parent.height / 2
                }

                Rectangle {
                    id: quad4
                    color: "#ffff00"
                    width: parent.width / 2
                    height: parent.height / 2
                }
            }
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

                Row{
                    id: videoStyle
                    width: parent.width
                    Text{
                        text: "Multi"
                        width: parent.width / 2
                        horizontalAlignment: Text.AlignHCenter
                        fontSizeMode: Text.HorizontalFit
                        font.pointSize: 20

                        MouseArea{
                            width: parent.width
                            onPressed: {
                                if( quadVideo.visible ) return
                                console.log("multi video")
                                quadVideo.visible = true
                                mainVideo.visible = false
                                var newSingleVideo

                                for (var i = 0; i < mainVideo.children.length; i++)
                                {
                                    var videoToReparent = mainVideo.children[i]
                                    videoToReparent.anchors.fill = undefined
                                    videoToReparent.width = otherVideos.width
                                    videoToReparent.parent = otherVideos
                                    newSingleVideo = videoToReparent
                                }

                                videoClicked( newSingleVideo )
                            }

                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                        }
                    }
                    Text{
                        text: "Single"
                        width: parent.width / 2
                        horizontalAlignment: Text.AlignHCenter
                        fontSizeMode: Text.HorizontalFit
                        font.pointSize: 20

                        MouseArea{
                            width: parent.width
                            onPressed: {
                                if( mainVideo.visible ) return
                                console.log("single video")
                                quadVideo.visible = false
                                mainVideo.visible = true
                                quadVideoNumber = 0
                                var newSingleVideo

                                for (var i = 0; i < quad1.children.length; i++)
                                {
                                    var videoToReparent = quad1.children[i]
                                    videoToReparent.anchors.fill = undefined
                                    videoToReparent.width = otherVideos.width
                                    videoToReparent.parent = otherVideos
                                    newSingleVideo = videoToReparent
                                }

                                for (var i = 0; i < quad2.children.length; i++)
                                {
                                    var videoToReparent = quad2.children[i]
                                    videoToReparent.anchors.fill = undefined
                                    videoToReparent.width = otherVideos.width
                                    videoToReparent.parent = otherVideos
                                }

                                for (var i = 0; i < quad3.children.length; i++)
                                {
                                    var videoToReparent = quad3.children[i]
                                    videoToReparent.anchors.fill = undefined
                                    videoToReparent.width = otherVideos.width
                                    videoToReparent.parent = otherVideos
                                }

                                for (var i = 0; i < quad4.children.length; i++)
                                {
                                    var videoToReparent = quad4.children[i]
                                    videoToReparent.anchors.fill = undefined
                                    videoToReparent.width = otherVideos.width
                                    videoToReparent.parent = otherVideos
                                }

                                videoClicked(newSingleVideo)
                            }

                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 5
                    color: "#ff0000"
                }
            }// end column
        }// end rectangle
    }

    function videoClicked(video){
        if(!video) return
        console.log( "clicked video " + video.videoName )

        if( mainVideo.visible && video.parent != mainVideo ){
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
        }else if( quadVideo.visible &&
                 video.parent != quad1 &&
                 video.parent != quad2 &&
                 video.parent != quad3 &&
                 video.parent != quad4 ){
            addToQuadVideo(video)
        }
    }

    function addToQuadVideo(video){
        var correctQuad
        if( quadVideoNumber == 0 ){
            correctQuad = quad1
        }else if( quadVideoNumber == 1 ){
            correctQuad = quad2
        }else if( quadVideoNumber == 2 ){
            correctQuad = quad3
        }else if( quadVideoNumber == 3 ){
            correctQuad = quad4
        }

        quadVideoNumber++
        if( quadVideoNumber == 4 ){
            quadVideoNumber = 0
        }

        for (var i = 0; i < correctQuad.children.length; i++)
        {
            var videoToReparent = correctQuad.children[i]
            videoToReparent.anchors.fill = undefined
            videoToReparent.width = otherVideos.width
            videoToReparent.parent = otherVideos
        }

        video.parent = correctQuad
        video.width = video.parent.width
        video.anchors.fill = video.parent
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
