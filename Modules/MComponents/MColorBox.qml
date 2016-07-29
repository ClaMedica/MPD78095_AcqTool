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
            var c
            if(colorPicker===undefined){
                c=Qt.createQmlObject('import MComponents 1.0; MColorPicker{}',rootApp)
                c.build()
            }
            else
                c=colorPicker
            DataEngine.putItemOnTop(c)
            c.beginColor=rootColorBox.color
            c.visible=true
            con.target=c
        }
    }

    Connections{
        id:con
        ignoreUnknownSignals: true
        onColorSelected: {
            rootColorBox.color=target.choosedColor
            if(colorPicker===undefined)
                target.erase()
            else
                target.visible=false
            DataEngine.putItemOnTop(rootColorBox)
        }
        onClosing:target.destroy()
    }

}

