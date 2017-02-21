import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

TextField {
    id: rootLabel
    property real labelSize: 8
    property string safeText: ""
    onTextChanged: {
        if(acceptableInput)
            safeText = text
        else
            text = safeText
    }
    text: ""
    font.family: (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
    font.bold: true
    font.pixelSize: screenH * 0.01 * labelSize
    verticalAlignment: Text.AlignVCenter
    style: TextFieldStyle {
        textColor: "black"
    }
}

