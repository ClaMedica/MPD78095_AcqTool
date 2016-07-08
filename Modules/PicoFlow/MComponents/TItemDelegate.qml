import QtQuick 2.5
import MComponents 1.0
Rectangle{
    property var itemData:Item{
        property bool selected:false
        property int column:0
        property int row:0
        property int elideMode:0
        property string value:"ciao"
    }

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
    Text {
        anchors.fill:parent
        anchors.margins: 2
        color: DataEngine.TASKBAR_COLOR
        elide: itemData.elideMode
        text: itemData.value===undefined?"none":itemData.value
        font.bold: true
        font.pixelSize: layout.dimensionOf("F2")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

}

