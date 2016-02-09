import QtQuick 2.0
import MPlotModule 1.0
import "Tools"
import "Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0
Rectangle {

    property string name: modP.name
    property var modello:[]
    property real xMax: zoomer.xMax
    property real xMin: zoomer.xMin
    property real yMax: zoomer.yMax
    property real yMin: zoomer.yMin
    property var newLimits:[]

    property var saveTrack:[]
    property bool drawed:false

    //zoomer
    property real zoomSpeed:modP.zoomSpeed
    property bool enableZoom: true

    //proprietà che mi dice se c'è una modifica ad un oggetto
    //c'è il codice dell'oggetto oltre alle proprietà cambiate
    property var currentObject:[]

    //markermanager
    property alias markerItems:mark.items

    //framesManager
    property alias fraItems:fra.frames

    //time cursor
    property real currentTime:0
    property bool curTimeSelected:curTime.selected

    property var traksToDraw:[]

    //Colormap
    property var colorMap:[]

    id: rootPlot

    function setAXmin(x){zoomer.xAbsoluteMin=x;zoomer.checkLimits();zoomer.restore();curTime.value=x}
    function setAXmax(x){zoomer.xAbsoluteMax=x;zoomer.checkLimits();zoomer.restore();}
    function setAYmin(y){zoomer.yAbsoluteMin=y;zoomer.checkLimits();zoomer.restore();}
    function setAYmax(y){zoomer.yAbsoluteMax=y;zoomer.checkLimits();zoomer.restore();}

    onXMaxChanged:
    {
       // console.log("zzooommmm")

    }

    function setXmin(x)
    {
        zoomer.xMin=x;
        //zoomer.txMin=x;
    }

    function setXmax(x)
    {
         zoomer.xMax=x;
        //zoomer.txMax=x;
    }

    PlotModel{
        id:modP
        onEnableTimeCursorChanged:curTime.modello=["visible",modP.enableTimeCursor]
    }

    //onCurTimeSelectedChanged: console.log(name,curTimeSelected)
    onCurrentTimeChanged: if(!curTime.selected)curTime.modello=["val",currentTime]

    onNewLimitsChanged:
    {

        //console.log(newLimits)
        if(newLimits.length===4)
        {//devono esserci esattamente 4 elementi
            setAXmin(newLimits[0])
            setAXmax(newLimits[1])
            setAYmin(newLimits[2])
            setAYmax(newLimits[3])
        }
    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore

        if(modello.length%2===0 && modello.length>=2)
        {
            console.log("modello - ",modello)
            for(var i=0;i<modello.length;i+=2)
                modP.setProperty(modello[i],modello[i+1])

        }
    }

    border.width: 1

    color: "whitesmoke"//Qt.rgba(0.1, 0.1, 0.1)
    border.color: "black"

    onTraksToDrawChanged:{

        if(traksToDraw.length>0){
            //console.log(name,traksToDraw)
            var l=[]
            var index=0
            var trackIndex=0


            for(var i=0;i<traksToDraw.length;i++)
            {
                if(traksToDraw[i]==="$Track")
                {
                    l=[];
                    index=0;
                    continue;
                }
                if(traksToDraw[i]==="&Track")
                {
                    //console.log("draw this",l)
                    plotter.drawTrack(l);
                    trackIndex++
                    continue
                }
                //inizia la lettura
                l[index]=traksToDraw[i]
                index++
            }


            legend.model=plotter.trackInfo("names")
            //console.log(plotter.trackInfo("colors"))
            legend.colors=plotter.trackInfo("colors")
            drawed=true
        }
        else
            plotter.clearGraph()

    }

    Rectangle{
        id: rRightMargin
        color: "transparent"
        width: modP.rightMargin
        anchors.top: rootPlot.top
        anchors.bottom: rootPlot.bottom
        anchors.right: rootPlot.right

        MLegend{
            id:legend
            anchors.fill: parent
        }

    }

    Rectangle{
        id: rTopMargin
        color: "transparent"
        height: modP.topMargin
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right
    }

    Rectangle{
        id: rLeftMargin
        color: "transparent"
        width: modP.leftMargin
        anchors.bottom: rootPlot.bottom
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left

    }

    Rectangle{
        id: rBottomMargin
        color: "transparent"
        height: modP.bottomMargin
        anchors.bottom: rootPlot.bottom
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right
    }

    Rectangle{
        id:plotTitle
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        width:5
        anchors.left: parent.left

//        gradient: Gradient {
//            GradientStop { position: 0.0; color: "darkblue" }
//            GradientStop { position: 1.0; color: "black" }
//        }

   }
//    Text{
//        id:titl
//        anchors.horizontalCenter: plotTitle.horizontalCenter
//        anchors.verticalCenter: plotTitle.verticalCenter

//        font.family: modP.yAFontUOMFamily
//        color: modP.yAFontUOMColor
//        font.pointSize: modP.yAFontUOMSize
//        text: modP.name
//        rotation: -90
//    }

    DVAxes{ //Y AXIS LABELS
        id: yAxisLabels
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: yAxisNotches.left
        anchors.left: plotTitle.right
        major:modP.yGridLines
        minor:modP.ySubGridLines
        max:zoomer.yMax
        min:zoomer.yMin
        topMargin: modP.topMargin
        bottomMargin: modP.bottomMargin

        // Y Axis General properties
        format:            modP.yALFormat
        decimalsMaj:       modP.yALDecimalsMaj
        decimalsMin:       modP.yALDecimalsMin
        dateTimeformatMaj: modP.yALDateTimeformatMaj
        dateTimeformatMin: modP.yALDateTimeformatMin
        //Y Proprietà del testo Major
        fontMajFamily:     modP.yALMajFontFamily
        fontMajColor:      modP.yALMajFontColor
        fontMajSize:       modP.yALMajFontSize
        textMajDist:       modP.yALMajTextDist
        textMajVisible:    modP.yALMajTextVisible
        opacityMaj:        modP.yALMajOpacity
        //Y Proprietà del testo Minor
        fontMinFamily:     modP.yALMinFontFamily
        fontMinColor:      modP.yALMinFontColor
        fontMinSize:       modP.yALMinFontSize
        textMinDist:       modP.yALMinTextDist
        textMinVisible:    modP.yALMinTextVisible
        opacityMin:        modP.yALMinOpacity
        // Y Axis unit of measure
        unitOfMeasure:               modP.yAUOM
        unitOfMeasureVisible:        modP.yAUOMVisible
        fontUOMFamily:               modP.yAFontUOMFamily
        fontUOMColor:                modP.yAFontUOMColor
        fontUOMSize:                 modP.yAFontUOMSize
        textUOMDist:                 modP.yATextUOMDist
        opacityUOM:                  modP.yAOpacityUOM
        unitOfMeasureRotation:       modP.yAUOMRotation
    }

    PlotGrid2D{ //Y AXIS NOTECHES
        id: yAxisNotches
        visible: true
        anchors.top: rTopMargin.bottom
        anchors.bottom: rBottomMargin.top
        anchors.right: rCenter.left
        width: modP.yNotchLenght
        anchors.margins:0

        lineColor: plotter_grid.lineColor
        lineWidth: plotter_grid.lineWidth
        stippleEn: false

        lineColorSub: plotter_grid.lineColorSub
        lineWidthSub: plotter_grid.lineWidthSub
        stippleEnSub: false

        xMax: 1
        xMin: 0
        yMax: zoomer.yMax
        yMin: zoomer.yMin

        numXlines: 0
        numXlinesSub: 0
        numYlines: plotter_grid.numYlines
        numYlinesSub: plotter_grid.numYlinesSub

        clip: true
    }

    DHAxes{  //X AXIS LABELS
        id: xAxisLabels
        color:"transparent"
        anchors.bottom: parent.bottom
        anchors.top: xAxisNotches.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        major:modP.xGridLines
        minor:modP.xSubGridLines
        max: plotter.xMax
        min: plotter.xMin
        leftMargin: modP.leftMargin
        rightMargin: modP.rightMargin

        // X Axis General properties
        format:             modP.xALFormat
        decimalsMaj:        modP.xALDecimalsMaj
        decimalsMin:        modP.xALDecimalsMin
        dateTimeformatMaj:  modP.xALDateTimeformatMaj
        dateTimeformatMin:  modP.xALDateTimeformatMin
        //X Proprietà del testo Major
        fontMajFamily:     modP.xALMajFontFamily
        fontMajColor:      modP.xALMajFontColor
        fontMajSize:       modP.xALMajFontSize
        textMajDist:       modP.xALMajTextDist
        textMajVisible:    modP.xALMajTextVisible
        opacityMaj:        modP.xALMajOpacity
        //X Proprietà del testo Minor
        fontMinFamily:     modP.xALMinFontFamily
        fontMinColor:      modP.xALMinFontColor
        fontMinSize:       modP.xALMinFontSize
        textMinDist:       modP.xALMinTextDist
        textMinVisible:    modP.xALMinTextVisible
        opacityMin:        modP.xALMinOpacity
        // X Axis unit of measure
        unitOfMeasure:               modP.xAUOM
        unitOfMeasureVisible:        modP.xAUOMVisible
        fontUOMFamily:               modP.xAFontUOMFamily
        fontUOMColor:                modP.xAFontUOMColor
        fontUOMSize:                 modP.xAFontUOMSize
        textUOMDist:                 modP.xATextUOMDist
        opacityUOM:                  modP.xAOpacityUOM
        unitOfMeasureRotation:       modP.xAUOMRotation
    }

    PlotGrid2D{ //X AXIS NOTECHES
        id: xAxisNotches
        visible: true
        anchors.top: rCenter.bottom
        anchors.right: rRightMargin.left
        anchors.left: rLeftMargin.right
        height: modP.xNotchLenght
        anchors.margins:0

        lineColor: plotter_grid.lineColor
        lineWidth: plotter_grid.lineWidth
        stippleEn: false

        lineColorSub: plotter_grid.lineColorSub
        lineWidthSub: plotter_grid.lineWidthSub
        stippleEnSub: false

        xMax: zoomer.xMax
        xMin: zoomer.xMin
        yMax: 1
        yMin: 0

        numXlines: plotter_grid.numXlines
        numXlinesSub: plotter_grid.numXlinesSub
        numYlines: 0
        numYlinesSub: 0
        clip: true
    }

    Rectangle{
        id: rCenter
        color: rootPlot.color

        anchors.bottom: rBottomMargin.top
        anchors.top: rTopMargin.bottom
        anchors.left: rLeftMargin.right
        anchors.right: rRightMargin.left
        //prima la griglia
        PlotGrid2D{
            id: plotter_grid
            visible: true
            anchors.fill: parent
            anchors.margins:0
            stippleEn: false
            stippleEnSub: false

            xMax: zoomer.xMax
            xMin: zoomer.xMin
            yMax: zoomer.yMax
            yMin: zoomer.yMin

            clip: true

            lineColor: modP.gridLineColor
            lineWidth: modP.gridLineWidth

            lineColorSub:modP.subGridLineColor
            lineWidthSub:modP.subGridLineWidth

            numXlines:   modP.xGridLines
            numYlines:   modP.yGridLines
            numXlinesSub:modP.xSubGridLines
            numYlinesSub:modP.ySubGridLines
        }

        //tracce
        Plot2DStationary {
            id: plotter
            anchors.fill: parent
            anchors.margins: 2
            clip: true
            plotName: modP.name
            trackColors:modP.tColor
            //trackNames:modP.tName
            lineWidth: modP.tThick
            maxTracks: modP.maxTracks

            xMax: zoomer.xMax
            xMin: zoomer.xMin
            yMax: zoomer.yMax
            yMin: zoomer.yMin

            xAbsoluteMax: zoomer.xAbsoluteMax
            xAbsoluteMin: zoomer.xAbsoluteMin
            yAbsoluteMax: zoomer.yAbsoluteMax
            yAbsoluteMin: zoomer.yAbsoluteMin
        }

        //cursore Y
        MCursorY
        {
            id:horMark

            visible: true
            min:2
            max:rCenter.height-2
            lineLength: rCenter.width
            vMax:plotter.yMax
            vMin:plotter.yMin
            colore:"green"
            anchors.left: rCenter.left
            value: 0
        }

        //cursore tempo
        MCursorX{
            id:curTime
            min:2               //margini
            max:rCenter.width-2 //margini
            lineLength: rCenter.height
            vMax:plotter.xMax
            vMin:plotter.xMin
            anchors.bottom: rCenter.bottom
            deselectOnRelease: true
            onValueChanged:
            {
                legend.valori=plotter.trackInfo("values",value)
                if(curTime.selected)rootPlot.currentTime=value
            }
            modello:["key","Time","popUp","Time"]

        }

        //zoomer
        MZoomer{
            property var oldZoom:[]
            id:zoomer
            anchors.fill:parent
            anchors.margins: 2
            onClicked: parent.focus=true
            state: "idle"
            enabled: rootPlot.enableZoom

            xMax: modP.xAbsoluteMax
            xMin: modP.xAbsoluteMin
            yMax: modP.yAbsoluteMax
            yMin: modP.yAbsoluteMin

            xAbsoluteMax: modP.xAbsoluteMax
            xAbsoluteMin: modP.xAbsoluteMin
            yAbsoluteMax: modP.yAbsoluteMax
            yAbsoluteMin: modP.yAbsoluteMin
            zoomInc: modP.zoomSpeed

        }

        //analisi a frame
        MFrameStack{
            id:fra
            anchors.fill:parent
            xMin:plotter.xMin
            xMax:plotter.xMax
            yMin:plotter.yMin
            yMax:plotter.yMax
            onZoomThisChanged: {
                zoomer.oldZoom=zoomThis
                zoomer.setZoom(zoomThis)
                parent.focus=true
            }
            onNewFrameChanged: if(completed)currentObject=newFrame

        }


        //Markers
        MMarkerStack{
            id:mark
            anchors.fill: parent
            anchors.margins: 2
            vMax:plotter.xMax
            vMin:plotter.xMin
            yMax:plotter.yMax
            yMin:plotter.yMin

            onNewMarkerChanged:if(completed)currentObject=newMarker
        }


        Keys.onPressed: {

            switch(event.key)
            {
            case Qt.Key_H:zoomer.state="wheeling h";break;
            case Qt.Key_Z:zoomer.state="zooming";break;
            case Qt.Key_V:zoomer.state="wheeling v";break;
            case Qt.Key_E:zoomer.restore();break;
            case Qt.Key_J:zoomer.setZoomAll(0,25,-3,500);break;
            case Qt.Key_S:saveTrack=[xMin,xMax];break;
            case Qt.Key_M:curTime.modello=["val",(zoomer.xMax+zoomer.xMin)/2];break;
            case Qt.Key_O:horMark.visible=!horMark.visible;break;
            default:console.log("MPlot2D: Key not recognized")
            }

        }
        Keys.onReleased: if(zoomer.state!=="zooming")zoomer.state="idle"

    }

}




