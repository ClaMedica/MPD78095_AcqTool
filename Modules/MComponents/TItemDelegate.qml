import QtQuick 2.5
import MComponents 1.0
import Database 1.0

Rectangle{
    property var itemData:Item{
        property bool selected:false
        property int column:0
        property int row:0
        property int elideMode:0
        property string value:""
    }
    property var colonna: []
    opacity:0
    Component.onCompleted: opacity=1
    Component.onDestruction: opacity=0
    Behavior on opacity {NumberAnimation {duration: 200}}
    color:itemData.column%2?
    itemData.row%2?layout.value("ir2"):layout.value("ir0"):
    itemData.row%2?layout.value("ir3"):layout.value("ir1")

    Behavior on color {
        ColorAnimation {
            duration: 200
        }
    }
    border.color: "gainsboro"
    border.width: 1
    Rectangle{
        id:recSel
        anchors.fill:parent
        opacity: PicoFlow ? 0.7: layout.value("opacita")//0.5
        visible: itemData.selected
        color: PicoFlow ? "darkblue" : layout.value("sr")
    }

    MLabel{
        anchors.fill:parent
        anchors.margins: 2
        //color: layout.value("textTable")
        elide: itemData.elideMode
        text: {
            if (itemData.selected)
                color = "white"
            else if (PicoFlow)
                color = "black"
            else
                color = layout.value("textTable")
            if (itemData.value!==undefined && (colonna[itemData.column] === "BirthDate" || colonna[itemData.column] === "TestDate" || colonna[itemData.column] === "Date")){
                Qt.formatDate(new Date(itemData.value), "dd/MM/yyyy")
            }
            else if (colonna[itemData.column] === "Arrangement"/* || colonna[itemData.column] === "ArrangDesc"*/) {
                itemData.value===undefined?"none":qsTranslate("ProtocolContext",itemData.value)
            }
            else
                itemData.value===undefined?"none":itemData.value
        }
        labelSize:{
            if (itemData.selected || PicoFlow)
                layout.value("F4")
            else
                layout.value("F4")-1
        }
        horizontalAlignment: Text.AlignHCenter        
    }

}

