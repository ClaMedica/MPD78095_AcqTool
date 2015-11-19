import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

Rectangle {
    id:volResDlg
    color: "lightgrey"
    implicitWidth: 300
    implicitHeight: 150
    visible:false

    anchors.centerIn: parent

    property string titleDlg: ""
    property string volResVal: "0"

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
            text:titleDlg
        }

        anchors.top: parent.top

    }

    Row{
        spacing:20
        anchors.centerIn: parent
        focus: true

        Text{
            id: textVol
            text:qsTr("Insert Residual Volume")
        }

        Rectangle {
            color: "whitesmoke"
            height: textVol.height +5
            width: 50
            anchors.bottom: textVol.bottom
            focus: true

            TextInput {
                id: textInput
                text: volResVal
                validator: IntValidator { bottom:0; top: 1000}
                anchors.bottom: parent.bottom
                focus: true

                onActiveFocusChanged: {
                    console.log("focus cambiato", volResVal)
                    volResVal = ""

                }
            }
        }
    }

    Button {
        id:btnOK
        text: "Ok"

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        onClicked: {
            volResVal = textInput.text
            volResDlg.visible = false
            mngData.valVolRes = volResVal
            mngData.analysis()
        }
    }

}



