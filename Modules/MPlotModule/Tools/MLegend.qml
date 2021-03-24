import QtQuick 2.0

Rectangle{
    id:legend
    color:"transparent"
    //valori acquisizione
    property var valori:[0,0,0,0]
    //valori tempo
    property var tempo:0
    property var model
    property var colors
    property bool valVisible:true
    Repeater{
        model:legend.model
        delegate:
            Rectangle{
            id:legBox
            color:"transparent"
            anchors.left: legend.left
            anchors.right:legend.right
            height: 40
            y:index*(height+1)
            Rectangle{
                id:colBox
                anchors.margins: 2
                anchors.left:legBox.left
                anchors.verticalCenter: legBox.verticalCenter
                height: PicoFlow ? 10 : 15
                width: PicoFlow ? 10 : 15
                color: legend.colors!==undefined?legend.colors[index]:"transparent"
            }
            Text{
                id:txt
                anchors.margins: 2
                anchors.top:legBox.top
                anchors.left:colBox.right
                anchors.right:parent.right
                text:modelData
                color:"black"
                font.pixelSize: 14
                font.bold: true
                visible: PicoFlow
            }
            Text{
                id:txtDesktop
                anchors.margins: 2
                anchors.verticalCenter: colBox.verticalCenter
                anchors.left:colBox.right
                text:modelData
                color:"black"
                font.pixelSize: 18
                font.bold: true
                visible: !PicoFlow
            }
            Text{
                id:val
                anchors.margins: 2
                anchors.bottom:legBox.bottom
                anchors.left:colBox.right
                anchors.right:parent.right
                text:legend.valori[index]!==undefined?legend.valori[index]:"---"
                color:"black"
                font.pixelSize: 14
                font.bold: true
                visible: valVisible && PicoFlow
            }
            Text{
                id:valDesktop
                anchors.margins: 2
                anchors.top:txtDesktop.bottom
                anchors.horizontalCenter: legBox.horizontalCenter
                text:legend.valori[index]!==undefined?legend.valori[index]:"---"
                color:"black"
                font.pixelSize: 18
                font.bold: true
                visible: valVisible && !PicoFlow
            }
            Text{
                id:valTempo
                anchors.margins: 2
                anchors.top:valDesktop.bottom
                anchors.horizontalCenter: legBox.horizontalCenter
                text:legend.tempo!==undefined?legend.tempo:"---"
                color:"black"
                font.pixelSize: 18
                font.bold: true
                visible: valVisible && !PicoFlow
            }
        }

    }
}
