import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

Text {
    id: rootLabel
    property real labelSize: 8
    height: 100
    width: 200
    text: ""
    font.family:
    {
        if (PicoFlow) {
            if (layout !== undefined)
                rootLabel.font.family = layout.value("FFamily")
            else
                rootLabel.font.family ="Luxi Serif"
        }
        else if (Mac) {
            if (layout !== undefined) {
                var family = layout.value("FFamilyW")
                if (family !== "Calibri" && family !== "Luxi Serif")
                    rootLabel.font.family = family
                else
                    rootLabel.font.family ="Arial"
            }
            else
                rootLabel.font.family ="Arial"
        }
        else if (layout !== undefined)
            rootLabel.font.family = layout.value("FFamilyW")
        else
            rootLabel.font.family = "Calibri"
    }
    font.bold: PicoFlow ? true : false
    font.pixelSize: screenH * 0.01 * labelSize
    fontSizeMode: Text.Fit
    verticalAlignment: Text.AlignVCenter
    color: layout.value("textTable")
}

