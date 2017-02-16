import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0
Rectangle {
    id:volResDlg
    color: "lightgrey"
    implicitWidth: 300
    implicitHeight: 150
    visible:false

    anchors.centerIn: parent

    property string titleDlg: ""
    property int volResVal: 0

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

    Row {
        id: label
        spacing:20
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        height:parent.height*0.1
        anchors.left: parent.left
        //anchors.right:parent.right
        anchors.margins: layout.value("Margin")
        focus: true
        Text{
            id: text
            text:qsTr("Insert Residual Volume")
        }
    }

    MParameterEdit{
        id:parVolResVal
        anchors.verticalCenter: parent.verticalCenter
        height:parent.height*0.2
        anchors.left: label.right
        anchors.right:parent.right
        anchors.margins: layout.value("Margin")
        type:typTextField
        model:[0,validator]
        beginInfo:"0"
        labelSize: 3
        viewPerc:0
        IntValidator {
            id:validator
            bottom:0
            top:1000
        }
    }

    Button {
        id:btnOK
        text: "Ok"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        width:parent.width*0.4
        height:parent.height*0.2
        onClicked: {
            volResVal = parVolResVal.info
            volResDlg.visible = false
            Qt.inputMethod.hide()
            mngData.valVolRes = volResVal
            mngData.analysis()
        }
    }

}




