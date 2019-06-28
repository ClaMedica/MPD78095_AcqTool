import QtQuick 2.4
import Managers 1.0
import MPlotModule 1.0


Rectangle {
    id: rootPlot
    property var traksToDraw:[]
    property int xMin: 0
    property int yMin: 0
    property int xMax: 100
    property int yMax: 100

    property var tracksColors: []
    property var tracksWidth: [2,2,2,2,2,2,2]
    property string nome: "nome"

    property string udmY: "left"
    property string udmX: "bottom"

    property double xPoint: 10
    property double yPoint: 30

    property string colorEsterno: "darkGrey"
    property string colorTesti: "black"
    property string colorLabelAssi: "white"
    property string colorGriglia: "grey"

    property var polygonYpoints: [0]
    property var polygonXpoints: [0]
    property int numPunti: 0
    property var bandeColore: ["lightgreen", "peachpuff"]
    property int numBande: 1

    property int labelSize: layout.value("F4")-1//grafic.valueOf("Nomogrammi","labelSize")
    property int dimPoint: grafic.valueOf("Nomogrammi","dimPoint")
    border.width: 1

    color: "lightgrey"
    border.color: "black"

    function drawTracks()
    {
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
                plotter.drawTrack(l);
                trackIndex++
                continue
            }
            //inizia la lettura
            l[index]=traksToDraw[i]
            index++
        }
    }


    function updateY(v)
    {
        var valPer=v/yMax
        point.y =(rCenter.height*(1-valPer))-point.height/2
    }

    onYPointChanged: updateY(yPoint)

    function updateX(v)
    {
        var valPer=v/xMax
        point.x =(rCenter.width*valPer)-point.width/2
    }

    onXPointChanged: updateX(xPoint)

    function calculateXarea(vl)
    {
        if (vl.length > 1)
        {
            var banda = 0
            var newListPoints = []
            for(var i=0;i<vl.length;i++)
            {
                if (vl[i] !== "$End")
                {
                    var valPer=vl[i]/xMax
                    newListPoints[newListPoints.length] = plotter.width*valPer
                }
                else
                {
                    listaAree.itemAt(banda).xpoints = newListPoints
                    newListPoints=[]
                    banda++
                }
            }
        }

    }

    onPolygonXpointsChanged: calculateXarea(polygonXpoints)

    function calculateYarea(vl)
    {
        if (vl.length > 1)
        {
            var banda = 0
            var newListPoints = []
            for(var i=0;i<vl.length;i++)
            {
                if (vl[i] !== "$End")
                {
                    var valPer=vl[i]/yMax
                    newListPoints[newListPoints.length] = plotter.height*valPer
                }
                else
                {
                    listaAree.itemAt(banda).ypoints = newListPoints
                    newListPoints=[]
                    banda++
                }
            }
        }
    }

    onPolygonYpointsChanged: calculateYarea(polygonYpoints)

    Rectangle{
        id: rRightMargin
        color: colorEsterno
        width: grafic.valueOf("Nomogrammi","margin")/3
        anchors.top: rootPlot.top
        anchors.bottom: rootPlot.bottom
        anchors.right: rootPlot.right

    }

    Rectangle{
        id: rTopMargin
        color: colorEsterno
        height: grafic.valueOf("Nomogrammi","margin")
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right

        Text {
            id: text1
            text: nome
            color: colorTesti
            x: rootPlot.width/2 - nome.length*4
            y: 10
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        text1.font.family = layout.value("FFamily")
                    else
                        text1.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    text1.font.family = layout.value("FFamilyW")
                else
                    text1.font.family = "Calibri"
            }
            font.pixelSize: screenH * 0.01 * labelSize
        }
    }

    Rectangle{
        id: rLeftMargin
        color: colorEsterno
        width: grafic.valueOf("Nomogrammi","margin")
        anchors.bottom: rootPlot.bottom
        anchors.top: rootPlot.top
        anchors.left: rootPlot.left
        Text {
            id: text2
            text: udmY
            color: colorTesti
            x: 5
            y: rootPlot.height/2 + udmY.length*4
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        text2.font.family = layout.value("FFamily")
                    else
                        text2.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    text2.font.family = layout.value("FFamilyW")
                else
                    text2.font.family = "Calibri"
            }
            font.pixelSize: screenH * 0.01 * labelSize
            transform: Rotation {
                angle: -90
            }
        }
    }

    Rectangle{
        id: rBottomMargin
        color: colorEsterno
        height: grafic.valueOf("Nomogrammi","margin")
        anchors.bottom: rootPlot.bottom
        anchors.left: rootPlot.left
        anchors.right: rootPlot.right
        Text {
            id: text3
            text: udmX
            color: colorTesti
            x: rootPlot.width/2 - udmX.length*4
            y: 30
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        text3.font.family = layout.value("FFamily")
                    else
                        text3.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    text3.font.family = layout.value("FFamilyW")
                else
                    text3.font.family = "Calibri"
            }
            font.pixelSize: screenH * 0.01 * labelSize
        }
    }


    DVAxes{ //Y AXIS LABELS
        id: yAxisLabels
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: yAxisNotches.left
        anchors.left: rLeftMargin.right
        topMargin: rTopMargin.height// 40//30
        bottomMargin: rBottomMargin.height//45//50
        major:rootPlot.yMax/10 +1
        minor:0
        max:rootPlot.yMax
        min:rootPlot.yMin
        // Y Axis General properties
        format:            AxisSettings.EFORMAT_INT
        decimalsMaj:       1
        //Y Proprietà del testo Major
        fontMajFamily:     "Luxi Serif"
        fontMajColor:      colorLabelAssi
        fontMajSize:       8
        textMajDist:       2
        textMajVisible:    true
        //Y Proprietà del testo Minor
        textMinDist:       0
        textMinVisible:    true
        // Y Axis unit of measure
        unitOfMeasureRotation:       -90
        unitOfMeasureVisible:        false
    }

    PlotGrid2D{ //Y AXIS NOTECHES
        id: yAxisNotches
        visible: true
        anchors.top: rTopMargin.bottom
        anchors.bottom: rBottomMargin.top
        anchors.right: rCenter.left
        width: 7
        anchors.margins:0

        lineColor: "white"
        lineWidth:  plotter_grid.lineWidth
        stippleEn: false

        lineColorSub: "white"
        lineWidthSub: plotter_grid.lineWidthSub
        stippleEnSub: false

        xMax: 1
        xMin: 0
        yMax: rootPlot.yMax
        yMin: rootPlot.yMin

        numXlines: 0
        numXlinesSub: 0
        numYlines: plotter_grid.numYlines
        numYlinesSub: plotter_grid.numYlinesSub

        clip: true
    }

    DHAxes{  //X AXIS LABELS
        id: xAxisLabels
        anchors.bottom: rootPlot.bottom
        anchors.top: xAxisNotches.bottom
        anchors.right: rootPlot.right
        anchors.left: rootPlot.left
        leftMargin: rLeftMargin.width//50
        rightMargin: rRightMargin.width//20
        major: rootPlot.xMax/100 +1
        minor: 0
        max: plotter.xMax
        min: plotter.xMin
        // X Axis General properties
        format:             AxisSettings.EFORMAT_FLOAT
        decimalsMaj:        1
        fontMajColor:      colorLabelAssi
        fontMajSize:       8
        textMajDist:       2
        textMajVisible:    true
        unitOfMeasureVisible:        false
    }

    PlotGrid2D{ //X AXIS NOTECHES
        id: xAxisNotches
        visible: true
        anchors.top: rCenter.bottom
        anchors.right: rRightMargin.left
        anchors.left: rLeftMargin.right
        height: 7
        anchors.margins:0

        lineColor: "white"
        lineWidth: plotter_grid.lineWidth
        stippleEn: false

        lineColorSub: "white"
        lineWidthSub:  plotter_grid.lineWidthSub
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
        color: rootPlot.color

        anchors.bottom: rBottomMargin.top
        anchors.top: rTopMargin.bottom
        anchors.left: rLeftMargin.right
        anchors.right: rRightMargin.left

        Repeater{
            id:listaAree
            model: numBande
            PolygonItem {
                id:area
                anchors.fill: plotter
                clip: true
                npunti: numPunti
                colore: bandeColore[index]
            }
        }

        //prima la griglia
        PlotGrid2D{
            id: plotter_grid
            visible: true
            anchors.fill: parent
            anchors.margins:0
            //non funziona
            stippleEn: true
            stippleEnSub: true

            xMax: rootPlot.xMax
            xMin: rootPlot.xMin
            yMax: rootPlot.yMax
            yMin: rootPlot.yMin

            clip: true

            lineColor: colorGriglia
            lineColorSub:colorGriglia

            numXlines:   rootPlot.xMax/100 + 1
            numYlines:   rootPlot.yMax/10 + 1
            numXlinesSub:0// 2
            numYlinesSub:0// 1
        }

        //tracce
        Plot2DStationary {
            id: plotter
            anchors.fill: parent
            anchors.margins: 2
            clip: true
            plotName: rootPlot.nome
            trackColors:rootPlot.tracksColors
            lineWidth: rootPlot.tracksWidth
            xMax: rootPlot.xMax
            xMin: rootPlot.xMin
            yMax: rootPlot.yMax
            yMin: rootPlot.yMin

            xAbsoluteMax: rootPlot.xMax
            xAbsoluteMin: rootPlot.xMin
            yAbsoluteMax: rootPlot.yMax
            yAbsoluteMin: rootPlot.yMin
        }

        Rectangle{
            id: point
            color: "blue"
            border.color: "darkblue"
            height: dimPoint*10
            width: dimPoint*10
        }


        Rectangle{
            id:lineaO
            height:1
            width:rCenter.width
            anchors.left:rCenter.left
            y: point.y + point.height/2
            x: 0
            color: "slateblue"
            opacity: 1
        }

        Rectangle{
            id:lineaV
            height: rCenter.height
            width: 1
            anchors.bottom: rCenter.bottom
            y: 0
            x: point.x + point.width/2
            color: "slateblue"
            opacity: 1
        }
    }

    ShaderEffectSource {
        id: sourceImg
        width: rootPlot.width
        height: rootPlot.height
        sourceItem: rootPlot
        recursive: true
        live: false
    }

    function saveImgNomogramma() {
        if (PicoFlow)
            rootPlot.grabToImage(
                    function(result) {mngData.getGrabbedImage(result, nome); },
                    Qt.size(400,300)
                    )
        else
           mngData.saveImg(sourceImg,nome)
    }
}

