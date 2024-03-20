import QtQuick 2.0
import MPlotModule 1.0
import "Tools"
import "Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle {
    id:rootPlotStack

    property var plotProp:[]
    property int plotNumber
    property var markers:[]
    property var frames:[]
    property var colorMaps:[]
    property var limits:[]

    property var curObj:[]
    property bool completed:false
    property var tracks:[["doppler"]]
    property real timeMarker:0
    property var saveMe:[]
    property bool timeConnected:true
    property bool newOpMarker: false //true se e' stato inserito un nuovo marker operativo
    property real opMarkerPos: -1 //posizione nuovo marker operativo inserito
    property bool newDefiner: false //true se e' stato inserito un nuovo definitore
    property var definerPos //posizione nuovo defintore
    property bool newAnMarker: false //true se e' stato inserito un nuovo marker analitico
    property var anMarkerPos //posizione nuovo marker analitico
    property var anMarkChNames:[]


    //copy Plot
    property var toCopyPlot: []
    property bool setToCopy: false

    color:"transparent"

    Component.onCompleted:goTim.start(10)
    onPlotNumberChanged: {lista.completed=false;stackTim.start(10)}

    Timer{id:goTim;onTriggered: completed=true}
    Timer{id:stackTim;onTriggered: lista.completed=true}

//    onTimeMarkerChanged: {
//        //console.log(timeMarker)
//        if(completed)
//            for(var i = 0; i < lista.count; i++)
//                lista.itemAt(i).currentTime = timeMarker
//    }

    function setAbsXmax(x) {
        for(var i = 0; i < plotNumber; i++)
            lista.itemAt(i).setAXmax(x)
    }

    function setAbsXmin(x) {
        for(var i = 0; i < plotNumber; i++)
            lista.itemAt(i).setAXmin(x)
    }

    function popola() {
        //conto quanti plot devo fare e riempo il repeater di plot vergini
        plotNumber=0
        var pn=0
        for(var i=0;i<plotProp.length;i++)
            if(plotProp[i]==="$Plot")
                pn++
        plotNumber=pn
        //console.log(plotProp)
        //adesso vado a vedere come modificare i vari plot in base alle caratteristiche
        modify(plotProp);
    }

    function modify(p) {
        if(p.length>0) {
            var l = []
            var plotIndex = 0

            for(var i = 0; i < p.length; i++) {
                if(p[i] === "$Plot") {
                    //inizio un nuovo gruppo di proprietà
                    l =[];
                    continue;
                }
                if(p[i] === "&Plot") {
                    //sono alla fine dunque modifico il modello corrispondente
                    lista.itemAt(plotIndex).modello = l
                    plotIndex++
                    continue
                }
                //inizia la lettura
                l[l.length] = p[i]
            }
        }
    }

    function separateMarkers(p) {
        if(p.length > 0) {
            //console.log("separate")
            var l=[]
            var nome
            for(var i=0;i<p.length;i++) {
                if(p[i]==="$MarkerGroup") {
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=p[i]
                    continue;
                }
                if(p[i]==="&MarkerGroup") {

                    for(var j=0;j<plotNumber;j++)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).markerItems=l
                    continue
                }
                //inizio la lettura
                l[l.length]=p[i]
            }
        }
    }

    function separateColorMaps(m) {
        if(m.length>0) {
            var l=[]
            var nome
            for(var i=0;i<m.length;i++) {
                if(m[i]==="$MapGroup") {
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=m[i]
                    continue;
                }
                if(m[i]==="&MapGroup") {
                    for(var j=0;j<plotNumber;j++)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).colorMap=l
                    continue
                }
                //inizio la lettura
                l[l.length]=m[i]
            }
        }
    }

    function separateFrames(p) {
        //console.log(p)
        if(p.length>0) {
            var l=[]
            var nome
            for(var i=0;i<p.length;i++) {
                if(p[i]==="$DefinerGroup") {
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=p[i]
                    continue;
                }
                if(p[i]==="&DefinerGroup") {
                    //console.log("group",l)
                    for(var j=0;j<plotNumber;j++)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).fraItems=l
                    continue
                }
                //inizio la lettura
                l[l.length]=p[i]
            }
        }
    }

    function separateTracks(t) {
        if(t.length>0) {
            var l=[]
            var nome

            for(var i=0;i<t.length;i++) {
                if(t[i]==="$TrackGroup") {
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=t[i]
                    continue;
                }
                if(t[i]==="&TrackGroup") {
                    for(var j=0;j<plotNumber;j++)
                    {
                        //console.log(lista.itemAt(j).name,nome,l)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).traksToDraw=l
                    }
                    continue
                }
                //inizio la lettura
                l[l.length]=t[i]
            }
        }
    }

    function separateLimits(t) {
        if(t.length>0) {
            var l=[]
            var nome
            //console.log(t)
            for(var i=0;i<t.length;i++) {
                if(t[i]==="$Limit") {
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=t[i]
                    continue;
                }
                if(t[i]==="&Limit") {
                    for(var j=0;j<plotNumber;j++)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).newLimits=l
                    continue
                }
                //inizio la lettura
                l[l.length]=t[i]
            }
        }
    }


    //button:
    // - zoomIn
    // - zoomOut
    // - zoomReset
    function zoomFromButton(button){
        for(var i = 0; i < plotNumber; i++) {
            lista.itemAt(i).zoomFromButton(button)
        }
    }

    onPlotPropChanged:  if(completed)popola()
    onMarkersChanged:   if(completed)separateMarkers(markers)
    onFramesChanged:    if(completed)separateFrames(frames)
    onTracksChanged:    if(completed)separateTracks(tracks)
    onColorMapsChanged: if(completed)separateColorMaps(colorMaps)
    onLimitsChanged:    if(completed)separateLimits(limits)

    onNewOpMarkerChanged: {
        if (newOpMarker) { //avviso i plot di ascoltare l'evento inserimento marker
            for(var i = 0; i < plotNumber; i++)
                lista.itemAt(i).addMarkerOp()
            newOpMarker = false
        }
    }
    onNewDefinerChanged: {
        if (newDefiner) { //avviso i plot di ascoltare l'evento inserimento marker
            for(var i = 0; i < plotNumber; i++)
                lista.itemAt(i).addDefiner()
            newDefiner = false
        }
    }
    onNewAnMarkerChanged: {
        if (newAnMarker) { //avviso i plot di ascoltare l'evento inserimento marker analitico
            for(var i = 0; i < plotNumber; i++)
                lista.itemAt(i).addMarkerAn()
            newAnMarker = false
        }
    }

    onSetToCopyChanged: {
        for(var i = 0; i < plotNumber; i++)
            lista.itemAt(i).setToCopy = setToCopy
    }

    function timConnect(ind)
    {
        for(var i = 0; i < plotNumber; i++) {
            lista.itemAt(i).setXmax(lista.itemAt(ind).xMax)
            lista.itemAt(i).setXmin(lista.itemAt(ind).xMin)
        }
    }

    Repeater {
        id:lista
        property bool completed:false
        property int con:-1
        anchors.fill:rootPlotStack
        model:plotNumber
        delegate:MPlot2D {
            id: plot
            clip:true
            y:index*rootPlotStack.height/plotNumber
            height:rootPlotStack.height/plotNumber
            width:rootPlotStack.width
            firstPlot: index === 0
            numPlot: plotNumber
            onCurrentObjectChanged:
            {
                if(currentObject.length!==0) {
                    if (currentObject.length === 2) {//sto selezionando un definitore, devo selezionare lo stesso negli altri plot
                        for(var j=0;j<lista.count;j++){
                            lista.itemAt(j).defToSelect=currentObject
                        }
                    }
                    else {
                        curObj=currentObject
                        if (currentObject.length === 3) //sto modificando un marker operativo, devo aggiornare la posizione negli altri plot
                            for(var i=0;i<lista.count;i++){
                                lista.itemAt(i).markerTochange=curObj
                            }

                        if (currentObject.length === 9) //sto modificando un definitore, devo aggiornare i limti del definitore relativi ai marker analitici
                            for(i=0;i<lista.count;i++){
                                lista.itemAt(i).newLimitiDef=curObj
                            }
                    }
                }
            }

            onCurrentTimeChanged: {
                if(curTimeSelected&&completed) {
                    //console.log(timeMarker,currentTime)
                    timeMarker=currentTime
                    for(var i=0;i<lista.count;i++)
                        if(i!==index){
                            lista.itemAt(i).currentTime=timeMarker
                           //console.log(timeMarker,currentTime, i)
                        }
                }
            }

            onXMaxChanged:
            {
                //console.log(index,lista.con++)
                if(lista.completed && timeConnected)
                        timConnect(index)
            }

            onXMinChanged:
            {
                //console.log(index,lista.con++)
                if(lista.completed && timeConnected)
                        timConnect(index)
            }
//            onNewXmaxChanged:
//            {
//                if(lista.whoLead===-1 || lista.whoLead==index || lista.itemAt(index).imactive)
//                {
//                    lista.whoLead=index
//                    if(lista.completed && timeConnected)
//                        timConnect()
//                }
//            }
//            onNewXminChanged:
//            {
//                if(lista.whoLead===-1 || lista.whoLead==index || lista.itemAt(index).imactive)
//                {
//                    if(lista.completed && timeConnected)
//                        timConnect()
//                }

//            }


            onNewOpMarkerPosChanged:
            {
                opMarkerPos = newOpMarkerPos
                for(var i = 0; i < plotNumber; i++)
                    lista.itemAt(i).stopAddMarkerOp()
            }
            onNewDefinerPosChanged:
            {
                definerPos = newDefinerPos
                for(var i = 0; i < plotNumber; i++)
                    lista.itemAt(i).stopAddDefiner()

            }
            onNewAnMarkerPosChanged:
            {
                anMarkChNames = anMarkerChNames
                anMarkerPos = newAnMarkerPos
                for(var i = 0; i < plotNumber; i++)
                    lista.itemAt(i).stopAddMarkerAn()
            }

            onToCopyPlotChanged: {
                    if (rootPlotStack.toCopyPlot !== plot.toCopyPlot)
                        rootPlotStack.toCopyPlot = plot.toCopyPlot
            }
            //onImactiveChanged: {lista.whoLead=index;console.log("I'm active:",imactive,index,lista.whoLead)}
            onSaveTrackChanged: saveMe=[plot.name,saveTrack]
        }
    }
}





