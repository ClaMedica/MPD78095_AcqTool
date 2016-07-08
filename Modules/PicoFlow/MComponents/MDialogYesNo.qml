import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import MComponents 1.0
Rectangle {
    id: rootDialogYesNo

    property string message: "Message"
    property string yesText:"Yes"
    property string noText:"No"
    signal accepted
    signal rejected

    MLabel {
        id: lblMessage
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height*0.7
        text: message
        horizontalAlignment: Text.AlignLeft        
        labelSize: 4
        verticalAlignment: Text.AlignTop
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
        labelSize: 3
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
        labelSize: 3
        onClicked:rejected()
    }
}
