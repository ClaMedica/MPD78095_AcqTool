import QtQuick 2.0
import MPlotModule 1.0

Rectangle {
    property var frames:[]
    property real xMin:0
    property real xMax:1
    property real yMin:0
    property real yMax:1
    property var newFrame:[]
    property int frameCount

    property var zoomThis
    id:rootFrameStack
    color:"transparent"
    clip:true
    property bool completed
    Component.onCompleted:complete.start(10)
    Timer{id:complete;onTriggered: {completed=true;popola()}}

    function getItem(it,index,field)
    {
        var v=it[field+index*nFields]
        return v
    }

    function modify(it)
    {
        //console.log("modify",it)
        if(it.length>0)
        {
            var l=[]
            var index=0
            var windowIndex=0

            for(var i=0;i<it.length;i++)
            {
                if(it[i]==="$Definer")
                {
                    l=[];
                    index=0;
                    continue;
                }
                if(it[i]==="&Definer")
                {
                    listFra.itemAt(windowIndex).modello=l
                    windowIndex++
                    continue
                }
                //inizia la lettura
                l[index]=it[i]
                index++
            }
        }
    }

    onFramesChanged: if(completed)popola()

    function popola()
    {

        frameCount=0
        var fc=0
        //console.log("popolaFrame",frames)
        for(var i=0;i<frames.length;i++)
            if(frames[i]==="$Definer")
                fc++

        frameCount=fc
        //console.log("popolato con ",frameCount," frame")
        if(frameCount>0)
            modify(frames)
    }
    Repeater{
        id:listFra
        onItemAdded:listFra.itemAt(index).opacity=1
        model:frameCount
        delegate:
            MFrame{
            id:me
            rec:rootFrameStack
            vMinX:rootFrameStack.xMin
            vMaxX:rootFrameStack.xMax
            vMinY:rootFrameStack.yMin
            vMaxY:rootFrameStack.yMax
            onZoomMeChanged: rootFrameStack.zoomThis=zoomMe
            onModifyMe: rootFrameStack.newFrame=[whoAmI,
                                                 "xMin",xMin,
                                                 "xMax",xMax,
                                                 "yMin",yMin,
                                                 "yMax",yMax]
        }
    }
}
