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
    font.family: (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
    font.bold: true
    font.pixelSize: screenH * 0.01 * labelSize
    fontSizeMode: Text.Fit
    verticalAlignment: Text.AlignVCenter
    color: "black"
}

