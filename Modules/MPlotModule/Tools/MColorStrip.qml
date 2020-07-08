import QtQuick 2.0
import MPlotModule 1.0

Rectangle{   
    id:rootStrip

    property var colorList

    color:"transparent"

    function toCol(v)
    {
        var opa=1
        if(v>=0 && v<=25)
            return Qt.rgba(0,v/25,1,opa)
        if(v>25 && v<=50)
            return Qt.rgba(0,1,1-(v-25)/25,opa)
        if(v>50 && v<=75)
            return Qt.rgba((v-50)/25,1,0,opa)
        if(v>75 && v<=100)
            return Qt.rgba(1,1-(v-75)/25,0,opa)
        return "transparent"
    }

    Repeater{
        id:listStrip
        anchors.fill:rootStrip
        model:colorList
        delegate: Rectangle{
            anchors.left:rootStrip.left
            anchors.right:rootStrip.right
            height:rootStrip.height/listStrip.count
            y:index*height
            color:toCol(modelData)
        }
    }

}
