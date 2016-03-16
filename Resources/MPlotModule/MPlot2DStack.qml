import QtQuick 2.0
import MPlotModule 1.0
import "Tools"
import "Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle{
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

    property int toZoom:0
    property int oldToZoom:0
    property real movingZoom:0

    color:"transparent"

    Component.onCompleted:goTim.start(10)
    onPlotNumberChanged: {lista.completed=false;stackTim.start(10)}

    Timer{id:goTim;onTriggered: completed=true}
    Timer{id:stackTim;onTriggered: lista.completed=true}

    onTimeMarkerChanged:{
        //console.log(timeMarker)
        if(completed)
            for(var i=0;i<lista.count;i++)
                lista.itemAt(i).currentTime=timeMarker
    }

    function setAbsXmax(x){
        for(var i=0;i<plotNumber;i++)
            lista.itemAt(i).setAXmax(x)
    }

    function setAbsXmin(x){
        for(var i=0;i<plotNumber;i++)
            lista.itemAt(i).setAXmin(x)
    }

    function popola(){
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

    function modify(p){
        if(p.length>0){
            var l=[]
            var plotIndex=0

            for(var i=0;i<p.length;i++){
                if(p[i]==="$Plot"){
                    //inizio un nuovo gruppo di proprietà
                    l=[];
                    continue;
                }
                if(p[i]==="&Plot")                {
                    //sono alla fine dunque modifico il modello corrispondente
                    lista.itemAt(plotIndex).modello=l
                    plotIndex++
                    continue
                }
                //inizia la lettura
                l[l.length]=p[i]

            }
        }
    }

    function separateMarkers(p){
        if(p.length>0){
            //console.log("separate")
            var l=[]
            var nome
            for(var i=0;i<p.length;i++){
                if(p[i]==="$MarkerGroup"){
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=p[i]
                    continue;
                }
                if(p[i]==="&MarkerGroup"){

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

    function separateColorMaps(m){
        if(m.length>0){
            var l=[]
            var nome
            for(var i=0;i<m.length;i++){
                if(m[i]==="$MapGroup"){
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=m[i]
                    continue;
                }
                if(m[i]==="&MapGroup"){
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

    function separateFrames(p){
        //console.log(p)
        if(p.length>0){
            var l=[]
            var nome
            for(var i=0;i<p.length;i++){
                if(p[i]==="$DefinerGroup"){
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=p[i]
                    continue;
                }
                if(p[i]==="&DefinerGroup"){
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

    function separateTracks(t){
        if(t.length>0){
            var l=[]
            var nome

            for(var i=0;i<t.length;i++){
                if(t[i]==="$TrackGroup"){
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=t[i]
                    continue;
                }
                if(t[i]==="&TrackGroup"){
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

    function separateLimits(t){
        if(t.length>0){
            var l=[]
            var nome
            //console.log(t)
            for(var i=0;i<t.length;i++){
                if(t[i]==="$Limit"){
                    l=[];
                    //leggo il nome del plot
                    i++
                    nome=t[i]
                    continue;
                }
                if(t[i]==="&Limit"){
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

    onPlotPropChanged:  if(completed)popola()
    onMarkersChanged:   if(completed)separateMarkers(markers)
    onFramesChanged:    if(completed)separateFrames(frames)
    onTracksChanged:    if(completed)separateTracks(tracks)
    onColorMapsChanged: if(completed)separateColorMaps(colorMaps)
    onLimitsChanged:    if(completed)separateLimits(limits)

    function timConnect(ind)
    {
        for(var i=0;i<plotNumber;i++)
            {
                lista.itemAt(i).setXmax(lista.itemAt(ind).xMax)
                lista.itemAt(i).setXmin(lista.itemAt(ind).xMin)
            }

    }

    Repeater{
        id:lista
        property bool completed:false
        property int con:-1
        property int toZoom:0
        anchors.fill:rootPlotStack
        model:plotNumber
        delegate:MPlot2D{
            id: plot
            clip:true
            toZoom: rootPlotStack.toZoom
            oldToZoom: rootPlotStack.oldToZoom
            movingZoom: rootPlotStack.movingZoom
            y:index*rootPlotStack.height/plotNumber
            height:rootPlotStack.height/plotNumber
            width:rootPlotStack.width
            onCurrentObjectChanged:
            {
                if(currentObject.length!==0)
                    curObj=currentObject
                //console.log("obj" , curObj)
            }

            onCurrentTimeChanged:{
                if(curTimeSelected&&completed){
                    //console.log(timeMarker,currentTime)
                    timeMarker=currentTime
                    for(var i=0;i<lista.count;i++)
                        if(i!=index)
                            lista.itemAt(i).currentTime=timeMarker
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
            //onImactiveChanged: {lista.whoLead=index;console.log("I'm active:",imactive,index,lista.whoLead)}
            onSaveTrackChanged: saveMe=[plot.name,saveTrack]
        }
    }
}





