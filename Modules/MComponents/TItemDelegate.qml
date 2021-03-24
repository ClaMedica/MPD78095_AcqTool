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
    itemData.row%2?DataEngine.ir2:DataEngine.ir0:
    itemData.row%2?DataEngine.ir3:DataEngine.ir1
    Behavior on color {
        ColorAnimation {
            duration: 200
        }
    }
    Rectangle{
        id:recSel
        anchors.fill:parent
        opacity: 0.5
        visible: itemData.selected
        color:DataEngine.sr
    }
    MLabel{
        anchors.fill:parent
        anchors.margins: 2
        color: DataEngine.TASKBAR_COLOR
        elide: itemData.elideMode
        text: {
            if (itemData.value!==undefined && (colonna[itemData.column] === "BirthDate" || colonna[itemData.column] === "TestDate" || colonna[itemData.column] === "Date")){
                Qt.formatDate(new Date(itemData.value), "dd/MM/yyyy")
            }
            else if (colonna[itemData.column] === "Arrangement"/* || colonna[itemData.column] === "ArrangDesc"*/) {
                itemData.value===undefined?"none":qsTranslate("ProtocolContext",itemData.value)
            }
            else
                itemData.value===undefined?"none":itemData.value
        }
        labelSize:layout.value("F4")
        horizontalAlignment: Text.AlignHCenter        
    }


}

