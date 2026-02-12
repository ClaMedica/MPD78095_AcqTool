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
        if (PicoFlow)
            return (layout !== undefined) ? layout.value("FFamily") : "Luxi Serif"

        if (Mac) {
            if (layout !== undefined) {
                var family = layout.value("FFamilyW")
                if (family !== "Calibri" && family !== "Luxi Serif")
                    return family
            }

            return  "Arial"
        }

        return (layout !== undefined) ? layout.value("FFamilyW") : "Calibri"

    }
    font.bold: PicoFlow ? true : false
    font.pixelSize: screenH > 0 ? (screenH * 0.01 * labelSize) : 1
    fontSizeMode: Text.Fit
    verticalAlignment: Text.AlignVCenter
    color: layout.value("textTable")
}

