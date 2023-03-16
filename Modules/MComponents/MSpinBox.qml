import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import MComponents 1.0

Rectangle{
    id: rootSpinBox
    property real value: 0.0
    property real maximumValue:  10.0
    property real minimumValue: -10.0
    property real step: 1.0
    property int decimals: 1
    property string suffix: "a.u."
    property real labelSize: 3

    signal clicked

    function changeValue(newValue)
    {
        if(newValue > maximumValue)
            newValue = maximumValue
        if(newValue < minimumValue)
            newValue = minimumValue

        value = newValue
        rootSpinBox.clicked()
    }

    MButton {
        id: btnMinus
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: (height < rootSpinBox.width*0.2) ? height : rootSpinBox.width*0.2
        text: "-"
        labelSize: rootSpinBox.labelSize
        onClicked: changeValue(value - step)
        onPressed: timIncDec.startDec()
        onReleased: timIncDec.stop()
    }

    MLabel {
        id: lblValue
        anchors.top: parent.top
        anchors.left: btnMinus.right
        anchors.right: btnPlus.left
        anchors.bottom: parent.bottom
        text: value.toFixed(decimals) + suffix
        labelSize: rootSpinBox.labelSize
        horizontalAlignment: Text.AlignHCenter
    }

    MButton {
        id: btnPlus
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: (height < rootSpinBox.width*0.2) ? height : rootSpinBox.width*0.2
        text: "+"
        labelSize: rootSpinBox.labelSize
        onClicked: changeValue(value + step)
        onPressed: timIncDec.startInc()
        onReleased: timIncDec.stop()
    }

    Timer{
        id: timIncDec
        property bool dir: true
        function startInc() { dir = true; start() }
        function startDec() { dir = false; start() }
        interval: 200
        repeat: true
        onTriggered:
        {
            if(dir)
                changeValue(value + step)
            else
                changeValue(value - step)
        }
    }

}
