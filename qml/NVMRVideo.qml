import QtQuick 2.0

Item {
    width: 200;
    height: width
    property string text

    Rectangle{
        width: parent.width
        height: parent.height
        anchors.fill: parent
        color: "blue";
    }

    signal videoClicked(var the_object)

    Text{
        text: parent.text
    }

    MouseArea {
        width: parent.width
        height: parent.height
        anchors.fill: parent
        onClicked: function(){ videoClicked(this.parent) }
    }

}
