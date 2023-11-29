import QtQuick 2.0
import MPlotModule 1.0
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "Tools"
import "Models"


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

    //proprieta' che mi dice la posizione x di un nuovo marker operativo aggiunto
    property real newOpMarkerPos:-1
    // proprieta' che ha il marker operativo spostato in un altro plot e che deve essere aggiornato in ogni plot
    property var markerTochange:[]
    //proprieta' che mi dice la posizione di un nuovo definitore aggiunto
    property var newDefinerPos
    //proprieta' che mi dice la posizione x di un nuovo marker analitico aggiunto
    property real newAnMarkerPos:-1
    property var anMarkerChNames:[]
    // proprieta' che ha il deinitore di cui mi interessano i nuovi limiti per gestire gli spostamenti dei marker analitici
    property var newLimitiDef:[]
    //selezione del definatore su altri plot
    property var defToSelect:[]

    //markermanager
    property alias markerItems:mark.items

    //framesManager
    property alias fraItems:fra.frames

    //time cursor
    property real currentTime:0
    property bool curTimeSelected:curTime.selected

    property var traksToDraw:[]

    //copy Plot
    property var toCopyPlot: []
    property bool setToCopy: false

    //Colormap
    property var colorMap:[]

    property bool firstPlot: false
    property int numPlot: 1

    property int stepMovePlot: -1

    id: rootPlot
    focus: false

    Component.onCompleted: {
//        stepMovePlot = (plotter.yAbsoluteMax - plotter.yAbsoluteMin)/10
//        console.log(" STEP PLOT PLOTTER ", stepMovePlot)
    }

    function setAXmin(x) { zoomer.xAbsoluteMin = x; zoomer.restore(); curTime.value = x }
    function setAXmax(x) { zoomer.xAbsoluteMax = x; zoomer.restore(); }
    function setAYmin(y) { zoomer.yAbsoluteMin = y; zoomer.restore(); }
    function setAYmax(y) { zoomer.yAbsoluteMax = y; zoomer.restore(); }


    //button:
    // - zoomIn
    // - zoomOut
    // - zoomReset
    function zoomFromButton(button){
        if (button === "zoomIn")
            zoomer.zoomButton(0.5)
        if (button === "zoomOut")
            zoomer.zoomButton(-0.5)
        if (button === "zoomReset"){
            zoomer.restore()
        }
    }

    onXMaxChanged:
    {
       // console.log("zzooommmm")
    }

    function setXmin(x)
    {
        zoomer.xMin=x;
    }

    function setXmax(x)
    {
         zoomer.xMax = x;
    }

    function addMarkerOp()
    {
        zoomer.newOpMarker = true
    }

    function stopAddMarkerOp()
    {
        zoomer.newOpMarker = false
    }

    function addDefiner()
    {
        zoomer.newDefiner = true
    }

    function stopAddDefiner()
    {
        zoomer.newDefiner = false
    }

    function addMarkerAn()
    {
        zoomer.newAnMarker = true
    }

    function stopAddMarkerAn()
    {
        zoomer.newAnMarker = false
    }

    onMarkerTochangeChanged: {
        mark.markerChanged = markerTochange
    }

    onNewLimitiDefChanged: {
        mark.limDefChanged = newLimitiDef
    }

    onDefToSelectChanged: {
        fra.toselect = defToSelect
    }

    PlotModel {
        id: modP
        onEnableTimeCursorChanged: curTime.modello = ["visible",modP.enableTimeCursor]
    }

    //onCurTimeSelectedChanged: console.log(name,curTimeSelected)
    onCurrentTimeChanged: if(!curTime.selected) curTime.modello = ["val", currentTime]

    onNewLimitsChanged:
    {
        //console.log(newLimits)
        if(newLimits.length === 4)
        {//devono esserci esattamente 4 elementi
            rRightMarginBottom.posScale = 0;
            setAXmin(newLimits[0])
            setAXmax(newLimits[1])
            setAYmin(newLimits[2])
            setAYmax(newLimits[3])
        }
    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore

        if((modello.length % 2) === 0 && modello.length >= 2)
        {
            console.log("modello - ", modello)
            for(var i = 0; i < modello.length; i += 2)
                modP.setProperty(modello[i], modello[i + 1])
        }
    }

    border.width: 1

    color: modP.backgroundColor
    border.color: modP.borderColor

    onTraksToDrawChanged: {

        if(traksToDraw.length > 0) {
            //console.log(name,traksToDraw)
            var l = []
            var index = 0
            var trackIndex = 0

            for(var i = 0; i < traksToDraw.length; i++)
            {
                if(traksToDraw[i] === "$Track")
                {
                    l = [];
                    index = 0;
                    continue;
                }
                if(traksToDraw[i] === "&Track")
                {
                    //console.log("draw this",l)
                    plotter.drawTrack(l);
                    trackIndex++
                    continue
                }
                //inizia la lettura
                l[index] = traksToDraw[i]
                index++
            }

            legend.model = plotter.trackInfo("names")
            //console.log(plotter.trackInfo("colors"))
            legend.colors = plotter.trackInfo("colors")
            drawed = true
        }
        else
            plotter.clearGraph()
    }

    Rectangle {
        id: rRightMargin
        color: "transparent"
        width: modP.rightMargin
        anchors.top: rootPlot.top
        anchors.right: rootPlot.right
        height: rootPlot.height/2

        MLegend {
            id:legend
            anchors.fill: parent
        }

        MouseArea{
            id: mousePlot
            anchors.fill: rRightMargin
            acceptedButtons: Qt.LeftButton
            enabled: !PicoFlow
            propagateComposedEvents: true
            onClicked: {
                toCopyPlot = [parent.width]
            }
        }
    }

    Rectangle {
        id: rRightMarginBottom
        color: "transparent"
        width: modP.rightMargin
        anchors.bottom: rootPlot.bottom
        anchors.right: rootPlot.right
        height: rootPlot.height/3
        visible: !PicoFlow && !setToCopy
        property var btnSize: rootPlot.height*numPlot*0.015
        property var posScale: 0

//        Button {
//            id: btnUp
//            anchors.top: parent.top
//            anchors.horizontalCenter: parent.horizontalCenter
//            width: rRightMarginBottom.btnSize
//            height: rRightMarginBottom.btnSize
//            style: ButtonStyle {
//                background: Image {
//                    source: "qrc:/Image/frecciasu.jpg"
//                    fillMode: Image.Stretch
//                }
//            }
//            onClicked: {
//                if (rRightMarginBottom.posScale < 10) {
//                    rRightMarginBottom.posScale++
//                    var valPlot = (plotter.yAbsoluteMax - plotter.yAbsoluteMin)/10
//                    if (stepMovePlot === -1)
//                        stepMovePlot = valPlot
//                    zoomer.yAbsoluteMax = yMax - stepMovePlot//valPlot
//                    zoomer.yAbsoluteMin = yMin - stepMovePlot//valPlot
//                    zoomer.setZoomX(zoomer.xMin,zoomer.xMax)
//                }
//            }
//        }
//        Button {
//            id: btnDown
//            anchors.top: btnUp.bottom
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.topMargin: rRightMarginBottom.btnSize/4
//            width: rRightMarginBottom.btnSize
//            height: rRightMarginBottom.btnSize
//            style: ButtonStyle {
//                background: Image {
//                    source: "qrc:/Image/frecciagiu.jpg"
//                    fillMode: Image.Stretch
//                }
//            }
//            onClicked: {
//                if (rRightMarginBottom.posScale > 0) {
//                    rRightMarginBottom.posScale--
//                    var valPlot = (plotter.yAbsoluteMax - plotter.yAbsoluteMin)/10
//                    if (stepMovePlot === -1)
//                        stepMovePlot = valPlot
//                    zoomer.yAbsoluteMax = yMax + stepMovePlot//valPlot
//                    zoomer.yAbsoluteMin = yMin + stepMovePlot//valPlot
//                    zoomer.setZoomX(zoomer.xMin,zoomer.xMax)
//                }
//            }
//        }
        Button {
            id: btnMore
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: rRightMarginBottom.btnSize/2
            width: rRightMarginBottom.btnSize
            height: rRightMarginBottom.btnSize
            style: ButtonStyle {
                background: Image {
                    source: "qrc:/Image/scale-up.png"
                    fillMode: Image.Stretch
                }
            }
            onClicked: {
               if (stepMovePlot === -1)
                    stepMovePlot = plotter.yAbsoluteMax/10
                var newScaleYmax= (plotter.yAbsoluteMax/2 - stepMovePlot)*2
                var newScaleYmin = (yMin * newScaleYmax)/yMax
                if (newScaleYmax > 0) {
                    zoomer.yAbsoluteMax = newScaleYmax
                    zoomer.yAbsoluteMin = newScaleYmin
                    zoomer.setZoomX(zoomer.xMin,zoomer.xMax)
                }
            }
        }
        Button {
            id: btnLess
            anchors.top: btnMore.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: rRightMarginBottom.btnSize/4
            width: rRightMarginBottom.btnSize
            height: rRightMarginBottom.btnSize
            style: ButtonStyle {
                background: Image {
                    source: "qrc:/Image/scale-down.png"
                    fillMode: Image.Stretch
                }
            }
            onClicked: {
                if (stepMovePlot === -1)
                    stepMovePlot = plotter.yAbsoluteMax/10
                var newScaleYmax= (plotter.yAbsoluteMax/2 + stepMovePlot)*2
                var newScaleYmin = (yMin * newScaleYmax)/yMax
                zoomer.yAbsoluteMax = newScaleYmax
                zoomer.yAbsoluteMin = newScaleYmin
                zoomer.setZoomX(zoomer.xMin,zoomer.xMax)
            }
        }
    }

    Timer{
        id:delTim
        interval: 1500
        onTriggered:
            clicright.visible = false
    }

    Rectangle {
        id: rTopMargin
        color: "transparent"
        height: modP.topMargin
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right
    }

    Rectangle {
        id: rLeftMargin
        color: "transparent"
        width: modP.leftMargin
        anchors.bottom: rootPlot.bottom
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left

    }

    Rectangle {
        id: rBottomMargin
        color: "transparent"
        height: modP.bottomMargin
        anchors.bottom: rootPlot.bottom
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right
    }

    Rectangle {
        id:plotTitle
        anchors.bottom: parent.bottom
        anchors.top: parent.top
      //  width:5
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

    DVAxes { //Y AXIS LABELS
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

    PlotGrid2D { //Y AXIS NOTECHES
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

    DHAxes {  //X AXIS LABELS
        id: xAxisLabels
        color:"transparent"
        anchors.bottom: rootPlot.bottom
        anchors.top: rCenter.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        major: modP.regolarStep ? plotter.xMax : modP.xGridLines
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
        textMinVisible:    true//modP.xALMinTextVisible
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

        regolarStep: modP.regolarStep
    }

    PlotGrid2D { //X AXIS NOTECHES
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

    Rectangle {
        id: rCenter
        color: rootPlot.color

        anchors.bottom: rBottomMargin.top
        anchors.top: rTopMargin.bottom
        anchors.left: rLeftMargin.right
        anchors.right: rRightMargin.left

        //prima la griglia
        PlotGrid2D {
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

            numXlines:   modP.regolarStep ? 0 : modP.xGridLines
            numYlines:   modP.yGridLines
            numXlinesSub:modP.regolarStep ? 0 : modP.xSubGridLines
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
//        MCursorY
//        {
//            id:horMark

//            visible: true
//            min:2
//            max:rCenter.height-2
//            lineLength: rCenter.width
//            vMax:plotter.yMax
//            vMin:plotter.yMin
//            colore:"green"
//            anchors.left: rCenter.left
//            value: 0
//        }

        //cursore tempo
        MCursorX {
            id:curTime
            min:1               //margini
            max:rCenter.width-1 //margini
            lineLength: rCenter.height
            vMax:plotter.xMax
            vMin:plotter.xMin
            anchors.bottom: rCenter.bottom
            deselectOnRelease: true
            onValueChanged:
            {
                legend.valori = plotter.trackInfo("values",value)
                var valueRoundes = Math.round(value)
                var min = Math.floor(valueRoundes/60)
                var minString = min < 10 ? "0"+min : min
                var sec = valueRoundes - (min*60)
                var secString = sec < 10 ? "0"+sec : sec
                var string = "%1:%2"
                legend.tempo = string.arg(minString).arg(secString)
                if(curTime.selected)
                    rootPlot.currentTime = value
            }
            modello:["key","Time","popUp","Time"]
        }

        //zoomer
        MZoomer {
            property var oldZoom:[]
            id:zoomer
            anchors.fill:parent
            anchors.margins: 2
            //onClicked: parent.focus=true
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
            enableYZoom: false

            onNewOpMarkerPos: {
                rootPlot.newOpMarkerPos = xpos
            }
            onNewDefinerPos: {
                rootPlot.newDefinerPos = pos
            }
            onNewAnMarkerPos: {
                rootPlot.anMarkerChNames = modP.tName
                rootPlot.newAnMarkerPos = xpos
            }
        }

//        //analisi a frame
        MFrameStack {
            id:fra
            anchors.fill:parent
            xMin:plotter.xMin
            xMax:plotter.xMax
            yMin:plotter.yMin
            yMax:plotter.yMax
            inFirstPlot: firstPlot
            onZoomThisChanged: {
                zoomer.oldZoom = zoomThis
                zoomer.setZoom(zoomThis)
            }
            onNewFrameChanged: if(completed)
                                   currentObject = newFrame
        }

        //Markers
        MMarkerStack {
            id:mark
            //focus:false
            anchors.fill: parent
            anchors.margins: 2
            vMax:plotter.xMax
            vMin:plotter.xMin
            yMax:plotter.yMax
            yMin:plotter.yMin

            countUpDown: rRightMarginBottom.posScale

            onNewMarkerChanged:if(completed)currentObject=newMarker
        }

    }
}



