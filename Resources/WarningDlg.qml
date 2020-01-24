import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    id: warningDlg
    height:screenH*0.4
    width:screenW*0.4
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")
    property string testoWarning: ""

    function open()
    {
        DataEngine.putItemOnTop(this)
        visible=true
    }

    Keys.onReturnPressed: {
        warningDlg.visible = false
    }

    MLabel {
        id: lblMessage
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height*0.8
        text: testoWarning
        horizontalAlignment: Text.AlignHCenter
        labelSize: layout.value("F4")
        verticalAlignment: Text.AlignVCenter
        color : "white"
        wrapMode: Text.WordWrap
    }

    MButton {
        id:btnOk
        text: qsTr("OK")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height:screenH*grafic.valueOf("Button","height")
        width: screenW*grafic.valueOf("Button","width")
        anchors.margins: parent.height*0.02
        labelSize: layout.value("F4")
        onClicked: {
            warningDlg.visible = false

        }
    }
}
