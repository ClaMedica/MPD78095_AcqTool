import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0

MForm {
    id:volResDlg
    height:screenH*grafic.valueOf("Dialog","height")
    width:screenW*grafic.valueOf("Dialog","width")
    anchors.centerIn: parent
    visible: false
    border.color: layout.value("textTable")
    border.width: 5

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
        color:layout.value("THC2")
        width: parent.width
        height: parent.height/5
        anchors.top: parent.top
        border.color: volResDlg.border.color //layout.value("textTable")
        border.width: volResDlg.border.width
        MLabel {
            id: lblMessage
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: titleDlg
            horizontalAlignment: Text.AlignHCenter
            labelSize: layout.value("F4")
            verticalAlignment: Text.AlignVCenter
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
        labelColor: layout.value("textTable")
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




