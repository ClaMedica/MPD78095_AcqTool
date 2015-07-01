import QtQuick 2.0
import MPlotModule 1.0
import "Tools"
import "Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle{
    id:rootPlotRealStack

    property var plotProp:[]
    property int plotNumber
    property var markers:[]
    property int savedData
    property bool completed:false

    function saveScreen(number)
    {
        lista.itemAt(number).saveScreen()
        savedData=lista.itemAt(number).savedData
    }

    function setPageTime(time)
    {
        for(var i=0;i<plotNumber;i++)
        {
            lista.itemAt(i).modello=["pageTime",time]
        }
    }

    function separateMarkers(p){
        if(p.length>0){

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
                    {
                        //console.log(l,lista.itemAt(j).name,nome)
                        if(lista.itemAt(j).name===nome)
                            lista.itemAt(j).markerItems=l
                    }
                    continue
                }
                //inizio la lettura
                l[l.length]=p[i]
            }
        }
    }

    onMarkersChanged:   if(completed)separateMarkers(markers)
    color:"transparent"

    Component.onCompleted:goTim.start(10)

    Timer{id:goTim;onTriggered: completed=true}

    function modify(p)
    {
        if(p.length>0)
        {
            var l=[]
            var plotIndex=0

            for(var i=0;i<p.length;i++)
            {
                if(p[i]==="$Plot")
                {
                    l=[];
                    continue;
                }
                if(p[i]==="&Plot")
                {
                    lista.itemAt(plotIndex).modello=l
                    plotIndex++
                    continue
                }
                //inizia la lettura
                l[l.length]=p[i]

            }
        }
    }

    function startAll(){timCheckTcp.start()}

    function stopAll()
    {
        for(var i=0;i<lista.count;i++)
        {
            lista.itemAt(i).stop()
            timCheckTcp.stop()
        }

    }
    Timer{
        id: timCheckTcp
        interval: 2000
        repeat: false
        onTriggered:  {
            for(var i=0;i<lista.count;i++)
                lista.itemAt(i).start()
        }
    }

    onPlotPropChanged: if(completed)popola()
    function popola()
    {

        plotNumber=0
        for(var i=0;i<plotProp.length;i++)
            if(plotProp[i]==="$Plot")
                plotNumber++
        console.log("disegno ",plotNumber," plot")
        modify(plotProp);
    }

    Repeater{
        id:lista
        anchors.fill:rootPlotRealStack
        model:plotNumber
        delegate:MPlot2DRealTime{
            id: plot
            clip:true
            y:index*rootPlotRealStack.height/plotNumber
            height:rootPlotRealStack.height/plotNumber
            width:rootPlotRealStack.width
        }
    }

}





