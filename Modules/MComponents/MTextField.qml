import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

TextField {
    id: rootLabel
    property real labelSize: 8
    property bool onlyNumber: false

    IntValidator { id: intval  }
    RegExpValidator { id: regexp; regExp: /""/ }
    onOnlyNumberChanged:
    {
        if (onlyNumber)
            rootLabel.validator= intval
        else
            rootLabel.validator = regexp
    }

    text: ""
    font.family:
    {
        if (PicoFlow)
            if (layout !== undefined)
                rootLabel.font.family = layout.value("FFamily")
            else
                rootLabel.font.family ="Luxi Serif"
        else if (layout !== undefined)
            rootLabel.font.family = layout.value("FFamilyW")
        else
            rootLabel.font.family = "Calibri"
    }
    font.bold: true
    font.pixelSize: Mac ? screenH * 0.01 * (labelSize-1) : screenH * 0.01 * labelSize
    verticalAlignment: Text.AlignVCenter
    style: TextFieldStyle {
        textColor: "black"
    }
}

