import QtQuick 2.5
import QtQuick.Window 2.0
import MComponents 1.0

Rectangle {
    id: anaBtn
    width: 130
    height: 30
    color:layout.value("c_btn_enabled")
    border.width: anaBtn.activeFocus ? 3 : 1
    border.color: layout.value("TaskBarColor")
    radius: height*0.1
    property real labelSize:Math.max(1,layout.value("F4"))

    //Since the buttons are created on the fly,
    //we need to identify the button on which the user
    // has clicked. The id must be unique
    property string buttonId;
    signal clicked(string buttonId);

    property string myText;

    MLabel{
        text:myText
        anchors.fill: anaBtn
        labelSize: anaBtn.labelSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.bold: false
    }

    BuzzMouseArea {
        anchors.fill: parent
        onClicked: parent.clicked(parent.buttonId)

        onPressed:  anaBtn.color = layout.value("c_btn_pressed")//"lightgrey"
        onReleased: anaBtn.color = layout.value("c_btn_enabled")//"whitesmoke"
    }
}

