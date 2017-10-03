import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import MComponents 1.0

Rectangle {
    id:rootColorBox
    width: 100
    height: 62
    border.width: 2
    border.color: "black"
    MouseArea{
        anchors.fill:parent
        onClicked: {
            var c=colorPicker
            c.beginColor=rootColorBox.color
            DataEngine.putItemOnTop(c)
            c.visible=true
            con.target=c
        }
    }

    Connections{
        id:con
        ignoreUnknownSignals: true
        onColorSelected: {
            rootColorBox.color=target.choosedColor
            target.visible=false
            target = null
        }
        onClosing:{
            target.destroy()
        }
    }

}

