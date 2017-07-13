import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

MForm {
    id:volResDlg
    color: "lightgrey"
    width: parent.width - parent.width/4
    height: parent.height - parent.height/4
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
        width: parent.width
        height: parent.height/5
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
        height:parent.height*0.2
        anchors.horizontalCenter: parent.horizontalCenter
        //anchors.right:parent.right
        anchors.margins: layout.value("Margin")
        type:typTextField
        labelSize: 5
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
        width:parent.width*0.3
        height:parent.height*0.15
        onClicked: {
            var volResVal = parVolResVal.info
            volResDlg.visible = false
            Qt.inputMethod.hide()
            mngData.valVolRes = volResVal
            mngData.analysis()
        }
    }

}




