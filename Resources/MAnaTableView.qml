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
            anchors.verticalCenter: parent.verticalCenter
            color: styleData.selected ? "#000" : "#000"
            elide: styleData.elideMode
            text: styleData.value
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: false
            font.pixelSize: screenH * 0.015
        }
    }

    rowDelegate: Rectangle{
        color:styleData.row%2?"#E0E0E0":"#FFFFFF"
        height:
        {
            var dim = screenH * 0.025
            styleData.selected?dim*1.5:dim
        }
        Behavior on height {NumberAnimation {duration: 200}}
    }

    headerDelegate: Rectangle{
        color: "#E0E0E0"
        height: 35
        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: styleData.selected ? "#000" : "#000"
            elide: styleData.elideMode
            text: styleData.value
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: true
            font.pixelSize:screenH * 0.02
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




