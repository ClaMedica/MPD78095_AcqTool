import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import MComponents 1.0
Rectangle {
    id: rootDialogYesNo
    height:screenH*0.4
    width:screenW*0.4
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")
    property string message: "Message"
    property string yesText:"Yes"
    property string noText:"No"
    signal accepted
    signal rejected

    function open()
    {
        DataEngine.putItemOnTop(this)
        visible=true
    }
    onAccepted: visible=false
    onRejected: visible=false

    MLabel {
        id: lblMessage
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height*0.7
        text: message
        horizontalAlignment: Text.AlignHCenter
        labelSize: layout.value("F4")
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }

    MButton {
        id: btnYes
        anchors.top:lblMessage.bottom
        anchors.left:parent.left
        anchors.bottom: parent.bottom
        width: parent.width/2.1
        anchors.margins: parent.height*0.01
        text: qsTr("Yes")
        labelSize: layout.value("F4")
        onClicked:accepted()
    }

    MButton {
        id: btnNo
        anchors.top:lblMessage.bottom
        anchors.right:parent.right
        anchors.bottom: parent.bottom
        width: parent.width/2.1
        anchors.margins: parent.height*0.01
        text: qsTr("No")
        labelSize: layout.value("F4")
        onClicked:rejected()
    }
}
