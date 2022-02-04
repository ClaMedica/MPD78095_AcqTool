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
    property real yMax:10
    property real yMin:0
    signal update
    //se ci sono novità su di un marker modifico questa proprietà
    property var newMarker:[]
    property var markerChanged:[] //un marker operativo ha cambiato posizione in un plot, devo aggiornare la sua posizione
    property real markerCount:0
    property bool completed:false
    property var limDefChanged:[] //un definitore ha cambiato i limiti, devo aggiornare i limiti di spostamento dei marker analitici
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
        var mCount=0
        //console.log("Popola Marker",items)

        var ll=items.length
        for(var i=0;i<ll;i++)
            if(items[i]==="$Marker")
                mCount++
        markerCount=mCount
        if(mCount>0)
            modify(items)
    }


    onMarkerChangedChanged: {
        for(var i = 0; i < lista.count; i++)
            if (lista.itemAt(i).whoAmI === markerChanged[0])
                lista.itemAt(i).updateX(markerChanged[2])
    }

    onLimDefChangedChanged: {
        for(var i = 0; i < lista.count; i++) {
            var pp = lista.itemAt(i).defCode
            var pp1 =limDefChanged[0]
            if (lista.itemAt(i).defCode === limDefChanged[0]) {
                lista.itemAt(i).limiteDefMin = limDefChanged[2]
                lista.itemAt(i).limiteDefMax = limDefChanged[4]
            }
        }
    }

    Repeater{
        id:lista
        model:markerCount
        anchors.fill:rootMarkerStack
        delegate:
            MMarker{
            id:mar
            max:rootMarkerStack.max
            min:rootMarkerStack.min
            vMax:rootMarkerStack.vMax
            vMin:rootMarkerStack.vMin
            yMax:rootMarkerStack.yMax
            yMin:rootMarkerStack.yMin

            lineLength:rootMarkerStack.height
            anchors.bottom: lista.bottom

            //quando cambia il valore di un marker invio queste informazioni sopra
            onModifyMe: if(completed){newMarker=[whoAmI,"val",value];}
            //se ne voglio rimuovere uno invece invio solo il codice
            onDeleteMe: if(completed){newMarker=[whoAmI];}

        }
    }
}
