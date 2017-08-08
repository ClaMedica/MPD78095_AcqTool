import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    id: exitDlg
    color: "lightgrey"
    implicitWidth: 350
    implicitHeight: 150
    visible: false

    anchors.centerIn: parent

    Keys.onReturnPressed: {
        volResDlg.visible = false
    }

    Rectangle {
        color: "dodgerblue"
        implicitWidth: 350
        implicitHeight: 25
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:qsTr("close")
        }
        anchors.top: parent.top
    }

    Row {
        spacing:20
        anchors.centerIn: parent
        focus: true
        Text{
            id: text
            text:qsTr("Do you want save changes?")
        }
    }

    BuzzButton {
        id:btnYes
        text: qsTr("Yes")
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: (parent.width - 4*10) / 3
        onClicked: {
            exitDlg.visible = false
            mngData.toSave = "yes"
            mngData.exitFromReview()
        }
    }

    BuzzButton {
        id:btnNo
        text: qsTr("No")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: (parent.width - 4*10) / 3
        onClicked: {
            exitDlg.visible = false
            mngData.toSave = "no"
            mngData.exitFromReview()
        }
    }

    BuzzButton {
        id:btnCancel
        text: qsTr("Cancel")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        width: (parent.width - 4*10) / 3
        onClicked: {
            exitDlg.visible = false
            mngData.toSave = ""
        }
    }
}
