import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

MForm {
    id:volResDlg
    height:screenH*0.5
    width:screenW*0.5
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")

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
        width: parent.width
        height: parent.height/5
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:titleDlg
            font.family: (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: true
            font.pixelSize: screenH * 0.04
        }
        anchors.top: parent.top
    }

    MParameterEdit{
        id:parVolResVal
        anchors.verticalCenter: parent.verticalCenter
        height:parent.height*0.2
        width: parent.width*0.5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: layout.value("Margin")
        type:typTextField
        labelSize: layout.value("F4")
        viewPerc:0.75
        role:qsTr("Insert Residual Volume")
        keyboardAlfaNum: false
    }

    MButton {
        id:btnOK
        text: qsTr("Ok")
        //anchors.top:parVolResVal.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height:parent.height*0.2
        width: parent.width*0.2
        anchors.margins: parent.height*0.02
        labelSize: layout.value("F4")
        onClicked: {
            var volResVal = parVolResVal.info
            volResDlg.visible = false
            Qt.inputMethod.hide()
            mngData.valVolRes = volResVal
            mngData.analysis()
        }
    }


}




