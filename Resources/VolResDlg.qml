import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

MForm {
    id:volResDlg
    color: "lightgrey"
    width: 350
    height: 200
    visible:false

    anchors.centerIn: parent

    property string titleDlg: ""

    Keys.onReturnPressed: {
        volResDlg.visible = false
    }

    onVisibleChanged: {
        if (visible)
            parVolResVal.setInfo(mngData.valVolRes)
    }

    Rectangle{
        color:"dodgerblue"
        width: 350
        height: 30
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:titleDlg
        }
        anchors.top: parent.top
    }

    MParameterEdit{
        id:parVolResVal
        anchors.verticalCenter: parent.verticalCenter
        height:parent.height*0.25
        anchors.left: label.right
        anchors.right:parent.right
        anchors.margins: layout.value("Margin")
        type:typTextField
        labelSize: 4
        viewPerc:0.75
        role:qsTr("Insert Residual Volume")
        keyboardAlfaNum: false
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
            var volResVal = parVolResVal.info
            volResDlg.visible = false
            Qt.inputMethod.hide()
            mngData.valVolRes = volResVal
            mngData.analysis()
        }
    }

}




