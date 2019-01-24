import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    id: exitDlg
    height:screenH*grafic.valueOf("Dialog","height")
    width:screenW*grafic.valueOf("Dialog","width")
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")

    function open()
    {
        DataEngine.putItemOnTop(this)
        visible=true
    }

    Keys.onReturnPressed: {
        exitDlg.visible = false
    }

    MLabel {
        id: lblMessage
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height*0.8
        text: qsTr("Do you want save changes?")
        horizontalAlignment: Text.AlignHCenter
        labelSize: grafic.valueOf("Dialog","labelSize")
        verticalAlignment: Text.AlignVCenter
        color : "white"
        wrapMode: Text.WordWrap
    }

    MButton {
        id:btnYes
        text: qsTr("Yes")
        anchors.top:lblMessage.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.width/3.1
        anchors.margins: parent.height*0.02
        labelSize: grafic.valueOf("Button","labelSize")
        onClicked: {
//            if (PicoFlow) restartBt()
            exitDlg.visible = false
            mngData.toSave = "yes"
            mngData.exitFromReview()
        }
    }

    MButton {
        id:btnNo
        text: qsTr("No")
        anchors.top:lblMessage.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: parent.width/3.1
        anchors.margins: parent.height*0.02
        labelSize: grafic.valueOf("Button","labelSize")
        onClicked: {
//            if (PicoFlow) restartBt()
            exitDlg.visible = false
            mngData.toSave = "no"
            mngData.exitFromReview()
        }
    }

    MButton {
        id:btnCancel
        text: qsTr("Cancel")
        anchors.top:lblMessage.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width/3.1
        anchors.margins: parent.height*0.02
        labelSize: grafic.valueOf("Button","labelSize")
        onClicked: {
            exitDlg.visible = false
            mngData.toSave = ""
        }
    }
}
