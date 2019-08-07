import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.1

import "qrc:/Forms"
import "qrc:/Components"
import MComponents 1.0

BuzzTableView{
    id:rootTable
    property bool completed:false
    property var roles:[]
    property string type:"default"
    height:300
    width:500

    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    itemDelegate: Item {
        anchors.leftMargin: 100
        Text {
            id: text1
            anchors.verticalCenter: parent.verticalCenter
            color: styleData.selected ? "#000" : "#000"
            elide: styleData.elideMode
            text: (styleData.value !== undefined) ? styleData.value : ""
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        text1.font.family = layout.value("FFamily")
                    else
                        text1.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    text1.font.family = layout.value("FFamilyW")
                else
                    text1.font.family = "Calibri"
            }
            font.bold: false
            font.pixelSize: rootTable.height * grafic.valueOf("TableRis","size")
        }
    }

    rowDelegate: Rectangle{
        color:styleData.row%2?"#E0E0E0":"#FFFFFF"
        height:
        {
            var dim = rootTable.height *0.06
            styleData.selected?dim*1.5:dim
        }
        Behavior on height {NumberAnimation {duration: 200}}
    }

    headerDelegate: Rectangle{
        color: "#E0E0E0"
        height: rootTable.height*0.1
        Text {
            id:text
            anchors.verticalCenter: parent.verticalCenter
            color: styleData.selected ? "#000" : "#000"
            text: styleData.value
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        text.font.family = layout.value("FFamily")
                    else
                        text.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    text.font.family = layout.value("FFamilyW")
                else
                    text.font.family = "Calibri"
            }
            font.bold: true
            font.pixelSize:rootTable.height *0.045
        }
    }
    function populate(){//al caricamento della tabella popolo con le colonne
        if(model===undefined)
            return
        var info = model.modelInfo(type)
        var sizeUsed=0
        for (var i=0; i<info.length; i++) {
            var column = addColumn( Qt.createQmlObject(
                                       "import QtQuick.Controls 1.4; TableViewColumn {}",
                                       this) )
            column.role  = info[i][0]
            roles[roles.length]=column.role
            if(i<info.length-1)
                column.width = Math.round(info[i][1]*rootTable.width/100)
            else
                column.width = rootTable.width-sizeUsed
            sizeUsed+=column.width
            column.title = info[i][2]
            column.movable = false
            column.resizable = false
        }
    }
}




