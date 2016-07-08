import QtQuick 2.0
import MPlotModule 1.0
import QtQuick.Controls 1.1
import "Tools"
import "Models"

//da decommentare per debug con Test


Rectangle {

    property string name: modP.name
    property var modello:[]
    property int savedData
    property alias mod: modP
    property real elapsed:plotter.time
    property alias  markerItems: markerStack.items

    property bool running:false
    PlotModel{
        id:modP
    }

    function start() { plotter.start(); running=true;console.log(modP.serverPort,"plot start")}
    function stop() { plotter.stop(); running=false;console.log(modP.serverPort,"plot stop")}
    function saveScreen() { savedData=plotter.saveData()}
    function enableTrack(__index, __en){plotter.enableTrack(__index, __en)}

    onModelloChanged:
    {
        var restart=false
        //questo modello carica una stringa composta dalla role e poi dal valore
        if(running)
        {
            restart=true;
        plotter.stop()
        }
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modP.setProperty(modello[i],modello[i+1])
            //console.log("modello - ",modello)
        }
        if(restart)
        plotter.start()
    }

    id: rootPlotReal

    color: mod.backgroundColor
    border.color: mod.borderColor
    border.width: 1


    Rectangle{
        id: rRightMargin
        color: "transparent"
        width: modP.rightMargin
        anchors.top: rootPlotReal.top
        anchors.bottom: rootPlotReal.bottom
        anchors.right: rootPlotReal.right
    }

    Rectangle{
        id: rTopMargin
        color: "transparent"
        height: modP.topMargin
        anchors.top: rootPlotReal.top
        anchors.left: rootPlotReal.left
        anchors.right: rootPlotReal.right
    }

    Rectangle{
        id: rLeftMargin
        color: "transparent"
        width: modP.leftMargin
        anchors.bottom: rootPlotReal.bottom
        anchors.top: rootPlotReal.top
        anchors.left: rootPlotReal.left

    }

    Rectangle{
        id: rBottomMargin
        color: "transparent"
        height: modP.bottomMargin
        anchors.bottom: rootPlotReal.bottom
        anchors.left: rootPlotReal.left
        anchors.right: rootPlotReal.right

        Label{
            id: lblTcp
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: plotter.serverAddress + ":" + plotter.serverPort + "-" + plotter.socketState
            color: "lightgray"
        }
    }

    DVAxes{ //Y AXIS LABELS
        id: yAxisLabels
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: yAxisNotches.left
        anchors.left: parent.left
        major:modP.yGridLines
        minor:modP.ySubGridLines
        max:plotter.yMax
        min:plotter.yMin
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
        yMax: plotter.yMax
        yMin: plotter.yMin

        numXlines: 0
        numXlinesSub: 0
        numYlines: plotter_grid.numYlines
        numYlinesSub: plotter_grid.numYlinesSub

        clip: true
    }

    DHAxes{  //X AXIS LABELS
        id: xAxisLabels
        anchors.bottom: parent.bottom
        anchors.top: xAxisNotches.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        major:modP.xGridLines
        minor:modP.xSubGridLines
        max: (plotter.time > plotter.pageTime) ? plotter.time : plotter.pageTime
        min: (plotter.time > plotter.pageTime) ? plotter.time - plotter.pageTime : 0
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

        xMax: plotter.xMax
        xMin: plotter.xMin
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
        color: modP.backgroundColor

        anchors.bottom: rBottomMargin.top
        anchors.top: rTopMargin.bottom
        anchors.left: rLeftMargin.right
        anchors.right: rRightMargin.left

        PlotGrid2D{
            id: plotter_grid
            visible: true
            anchors.fill: parent
            anchors.margins:0
            stippleEn: false
            stippleEnSub: false

            xMax: plotter.xMax
            xMin: plotter.xMin
            yMax: plotter.yMax
            yMin: plotter.yMin

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

        Plot2DRealTime {
            id: plotter
            anchors.fill: parent
            anchors.margins: 2
            clip: true
            trackColors:modP.tColor
            //trackNames:modP.tName
            lineWidth: modP.tThick
            maxTracks: modP.maxTracks

            samplingFrq: modP.samplingFrq
            pageTime: modP.pageTime

            xMax: modP.xAbsoluteMax
            xMin: modP.xAbsoluteMin
            yMax: modP.yAbsoluteMax
            yMin: modP.yAbsoluteMin

            xAbsoluteMax: modP.xAbsoluteMax
            xAbsoluteMin: modP.xAbsoluteMin
            yAbsoluteMax: modP.yAbsoluteMax
            yAbsoluteMin: modP.yAbsoluteMin

            serverPort: modP.serverPort // server port
            serverAddress: modP.serverAddress // server address
            onLastSampleChanged:horMark.value=lastSample[1]
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
            colore:"black"
            anchors.left: rCenter.left
            remoteControl: true
            lock:true
        }
        //Markers
        MMarkerStack{
            id: markerStack
            anchors.fill: parent
            anchors.margins: 2
            vMax:(plotter.time > plotter.pageTime) ? plotter.time : plotter.pageTime
            vMin:(plotter.time > plotter.pageTime) ? plotter.time - plotter.pageTime : 0
            //onNewMarkerChanged:if(completed)currentObject=newMarker
        }


    }
}
