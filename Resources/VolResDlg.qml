import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

MForm {
    id:volResDlg
    height:screenH*0.4
    width:screenW*0.4
    anchors.centerIn: parent
    visible: false
    color : layout.value("BackgroundColor")

    property string titleDlg: ""

    function open()
    {
        DataEngine.putItemOnTop(this)
        visible=true
    }

    function okbutton() {
        var volResVal = parVolResVal.info
        volResDlg.visible = false
        Qt.inputMethod.hide()
        mngData.valVolRes = volResVal
        mngData.analysis()
    }

    Keys.onReturnPressed: {
        okbutton()
    }

    onVisibleChanged: {
        if (visible)
            parVolResVal.setInfo(mngData.valVolRes)
    }

    Rectangle{
        color:"dodgerblue"
        width: parent.width
        height: parent.height/5
        anchors.top: parent.top
        MLabel {
            id: lblMessage
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: titleDlg
            horizontalAlignment: Text.AlignHCenter
            labelSize: layout.value("F4")
            verticalAlignment: Text.AlignVCenter
            color : "white"
        }
    }

    MParameterEdit{
        id:parVolResVal
        anchors.verticalCenter: parent.verticalCenter
        height:screenH*grafic.valueOf("Parameter","height")
        width: screenW*grafic.valueOf("Parameter","width")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: layout.value("Margin")
        type:typTextField
        labelColor: "white"
        labelSize: layout.value("F4")
        viewPerc:0.75
        role:qsTr("Insert Residual Volume")
        keyboardAlfaNum: false
    }

    MButton {
        id:btnOK
        text: qsTr("OK")
        //anchors.top:parVolResVal.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height:screenH*grafic.valueOf("Button","height")
        width: screenW*grafic.valueOf("Button","width")
        anchors.margins: parent.height*0.02
        labelSize: layout.value("F4")
        onClicked: okbutton()
    }


}




