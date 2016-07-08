import QtQuick 2.0
import MPlotModule 1.0

Rectangle {

    id:rootColMap
    property real min:0
    property real max:1

    property var colorMap:[]/*:[[0,10,20,30,40,50,60,70,80,90,100],[2,5,75]]*/
    property bool completed
    property real visCount:1
    Component.onCompleted:complete.start(10)
    Timer{id:complete;onTriggered:completed=true;}
    color:"transparent"
    clip:true

    onColorMapChanged:popola()
    onMaxChanged: popola()
    onMinChanged: popola()

    function popola()
    {
        if(completed)
        {
        var l=colorMap.length
        var from=l*min
        var to=l*max
        visCount=to-from
        //console.log(rootColMap.width/rootColMap.visCount)
        }
    }


    Repeater{
        id:listMap
        anchors.fill:rootColMap
        model:colorMap
        delegate: MColorStrip{
            anchors.top:rootColMap.top
            anchors.bottom: rootColMap.bottom
            width:rootColMap.width/rootColMap.visCount
            x:-colorMap.length*min*width+index*width
            colorList:modelData
        }
    }
}
