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
    frameVisible: false
    clip:true
    selectionMode:SelectionMode.ExtendedSelection
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    itemDelegate: Item {
        anchors.leftMargin: 100
        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: styleData.textColor
            elide: styleData.elideMode
            text: styleData.value
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: false
            font.pixelSize: 14
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
        resize()
    }
    function resize()
    {
        var info = model.modelInfo(type)
        var sizeUsed=0
        for (var i=0; i<info.length; i++) {
            var column = getColumn(i)
            if(i<info.length-1)
                column.width = Math.round(info[i][1]*rootTable.width/100)
            else
                column.width = rootTable.width-sizeUsed
            sizeUsed+=column.width
        }
    }

    Rectangle{
        id:frame
        anchors.fill:rootTable
        width:rootTable.width-2
        height:rootTable.height-2
        border.width: 2
        border.color: "blue"
        color:"transparent"
    }
}




