import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

TextField {
    id: rootLabel
    property real labelSize: 8
    property bool onlyNumber: false
    property string oldText: ""

    IntValidator { id: intval  }
    RegExpValidator { id: regexp; regExp: /""/ }

    signal clicked

    onOnlyNumberChanged:
    {
        if (onlyNumber)
            rootLabel.validator= intval
        else
            rootLabel.validator = regexp
    }
    onTextChanged: {
        if (oldText !== "" && oldText !== text)
            rootLabel.clicked()

        oldText = text
    }


    text: ""
    font.family:
    {
        if (PicoFlow)
            return (layout !== undefined) ? layout.value("FFamily") : "Luxi Serif"

        return (layout !== undefined) ? layout.value("FFamilyW") : "Calibri"
    }
    font.bold: true
    font.pixelSize: screenH > 0 ? (Mac ? screenH * 0.01 * (labelSize-1) : screenH * 0.01 * labelSize) : 1
    verticalAlignment: Text.AlignVCenter
    style: TextFieldStyle {
        textColor: "black"
    }
}

