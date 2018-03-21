import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    id: exitDlg
    height:screenH*0.4
    width:screenW*0.4
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")

    Keys.onReturnPressed: {
        volResDlg.visible = false
    }

    MLabel {
        id: lblMessage
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height*0.7
        text: qsTr("Do you want save changes?")
        horizontalAlignment: Text.AlignHCenter
        labelSize: layout.value("F4")
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
        labelSize: layout.value("F4")
        onClicked: {
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
        labelSize: layout.value("F4")
        onClicked: {
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
        labelSize: layout.value("F4")
        onClicked: {
            exitDlg.visible = false
            mngData.toSave = ""
        }
    }
}
