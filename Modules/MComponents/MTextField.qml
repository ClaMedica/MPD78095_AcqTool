import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

TextField {
    id: rootLabel
    property real labelSize: 8
    property string safeText: ""
    signal cliccato
    onTextChanged: {
        if(acceptableInput)
            safeText = text
        else
            text = safeText
    }
    text: ""
    font.family: (layout !== undefined) ? layout.value("FFamily") : "Arial"
    font.bold: true
    font.pixelSize: screenH * 0.01 * 3
    verticalAlignment: Text.AlignVCenter
    style: TextFieldStyle {
        textColor: "black"
    }
//    MouseArea {
//        height: parent.height
//        width: parent.width
//        onClicked: parent.cliccato

//    }
}

