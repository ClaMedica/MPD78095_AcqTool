import QtQuick 2.2
import MPlotModule 1.0
import "../Models"

//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle{

    id:rootMarkerStack

    property var items:[]//["$Mark","popUp","Marker 1","color","green","&Mark"]

    property real max:width
    property real min:0
    property real vMax:10
    property real vMin:0
    signal update
    //se ci sono novità su di un marker modifico questa proprietà
    property var newMarker:[]

    property real markerCount
    property bool completed:false
    color:"transparent"

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
            var markerIndex=0

            for(var i=0;i<it.length;i++)
            {
                if(it[i]==="$Marker")
                {
                    l=[];
                    index=0;
                    continue;
                }
                if(it[i]==="&Marker")
                {
                    //console.log("marker index",markerIndex,l)
                    lista.itemAt(markerIndex).modello=l
                    markerIndex++
                    continue
                }
                //inizia la lettura
                l[index]=it[i]
                index++
            }
        }
    }

    onItemsChanged: popola()

    function popola()
    {
        if(items.length>0){
        var mCount=0
        //console.log("Popola Marker",items)

        var ll=items.length
        for(var i=0;i<ll;i++)
            if(items[i]==="$Marker")
                mCount++
        markerCount=mCount
        //console.log("popolato con ",markerCount," marker")
        if(markerCount>0)
            modify(items)
        }
    }

    Repeater{
        id:lista
        model:markerCount
        anchors.fill:rootMarkerStack
        delegate:
            MMarker{
            id:mar
            max: rootMarkerStack.max
            min: rootMarkerStack.min
            vMax:rootMarkerStack.vMax
            vMin:rootMarkerStack.vMin
            lineLength:rootMarkerStack.height
            anchors.bottom: lista.bottom
            //quando cambia il valore di un marker invio queste informazioni sopra
            onModifyMe: if(completed){newMarker=["code",fullCode,"val",value];}
            //se ne voglio rimuovere uno invece invio solo il codice
            onDeleteMe: if(completed){newMarker=["code",fullCode];}
        }
    }
}
