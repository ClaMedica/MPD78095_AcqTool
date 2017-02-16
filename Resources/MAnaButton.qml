import QtQuick 2.5
import QtQuick.Window 2.0

Rectangle {
    id: anaBtn
    width: 120
    height: 30
    color:"whitesmoke"
    border.color: "gray"
    radius: 7

    //Since the buttons are created on the fly,
    //we need to identify the button on which the user
    // has clicked. The id must be unique
    property string buttonId;
    signal clicked(string buttonId);

    property string myText;

    Text {
        id: textBtn
        text: myText
        font.family: "Courier 8 Pitch"
        font.bold: false
        font.pixelSize: anaBtn.height//*0.4
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked(parent.buttonId)

        onPressed:  anaBtn.color = "lightgrey"
        onReleased: anaBtn.color = "whitesmoke"
    }
}

