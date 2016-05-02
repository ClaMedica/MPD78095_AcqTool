import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

Rectangle {
    id:exitDlg
    color: "lightgrey"
    implicitWidth: 300
    implicitHeight: 150
    visible:false

    anchors.centerIn: parent

    Keys.onReturnPressed: {
        volResDlg.visible = false
    }

    Rectangle{
        color:"dodgerblue"
        implicitWidth: 300
        implicitHeight: 25
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:qsTr("close")
        }

        anchors.top: parent.top

    }

    Row{
        spacing:20
        anchors.centerIn: parent
        focus: true

        Text{
            id: text
            text:qsTr("Do you want save changes?")
        }

    }

    Button {
        id:btnYes
        text: qsTr("Yes")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 2*width + 30

        onClicked: {
            exitDlg.visible = false
            mngData.toSave = "yes"
            mngData.exitFromReview()
        }
    }

    Button {
        id:btnNo
        text: qsTr("No")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: width + 20

        onClicked: {
            exitDlg.visible = false
            mngData.toSave = "no"
            mngData.exitFromReview()
        }
    }

    Button {
        id:btnCancel
        text: qsTr("Cancel")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10

        onClicked: {
            exitDlg.visible = false
            mngData.toSave = ""
        }
    }

}

