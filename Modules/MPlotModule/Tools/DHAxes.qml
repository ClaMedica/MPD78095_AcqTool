import QtQuick 2.2
import MPlotModule 1.0
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle {
    id:axis

    property int    major:          3
    property int    minor:          3
    property real   max:            10
    property real   min:            0

    property int    format:             AxisSettings.EFORMAT_TIME
    property int    decimalsMaj:        1
    property int    decimalsMin:        2
    property string dateTimeformatMaj:  "mm:ss.zzz"
    property string dateTimeformatMin:  "ss.zzz"

    //Proprietà del testo Major
    property string fontMajFamily:     "Luxi Serif"
    property color  fontMajColor:      "white"
    property int    fontMajSize:       12
    property int    textMajDist:       0
    property bool   textMajVisible
    property real   opacityMaj:        1.0
    //Proprietà del testo Minor
    property string fontMinFamily:     "Luxi Serif"
    property color  fontMinColor:      "white"
    property int    fontMinSize:       12
    property int    textMinDist:       10
    property bool   textMinVisible
    property real   opacityMin:        1.0

    //Unit of measure
    property string unitOfMeasure:     "[mm:ss.zzz]"
    property bool   unitOfMeasureVisible:    true
    property string fontUOMFamily:     "Luxi Serif"
    property color  fontUOMColor:      "white"
    property int    fontUOMSize:       12
    property int    textUOMDist:       10
    property real   opacityUOM:        1.0
    property int    unitOfMeasureRotation: 0

    property real leftMargin:50
    property real rightMargin:50

    property bool regolarStep: false
    //Proprietà dell'oggetto
    color:"transparent"

    function numNotches() { return axis.major+axis.minor*(axis.major-1)}
    function axesEffectiveWidth(){return axis.width-leftMargin-rightMargin}

    AxisSettings{
        id: axisLabels
        max:axis.max
        min: axis.min
        gridNumLines: axis.major
        subGridNumLines: axis.minor
        axisFormat: axis.format
        numberDecimalMaj: axis.decimalsMaj
        numberDecimalMin: axis.decimalsMin
        dateTimeFormatMaj: axis.dateTimeformatMaj
        dateTimeFormatMin: axis.dateTimeformatMin
        regolarStep: axis.regolarStep
    }
    Rectangle{
        anchors.left: parent.left
        anchors.right:parent.right
        anchors.top:parent.top
        height: fontMajSize*2
        color:"transparent"

        Row{
            id:rowGrid
            visible: textMajVisible
            x: axis.regolarStep ? leftMargin - (axesEffectiveWidth()/(axis.max - axis.min)/2) : leftMargin - ( axesEffectiveWidth()/(major-1)/2)
            Repeater{
                id: gridRepeater
                model: axisLabels.gridLables
                Rectangle
                {
                    id: rGrid
                    color:"transparent"
                    width: axis.regolarStep ? axesEffectiveWidth() / (axis.max - axis.min) : axesEffectiveWidth()/(major-1)
                    height:axis.height
                    Rectangle{
                        id:gridSpacer
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top:parent.top
                        height:textMajDist
                        width: parent.width
                        color:"transparent"
                    }
                    Text{
                        id:gridLabel
                        anchors.horizontalCenter: rGrid.horizontalCenter
                        anchors.top:gridSpacer.bottom
                        font.family: fontMajFamily
                        color: fontMajColor
                        font.pointSize: fontMajSize
                        opacity: opacityMaj
                        text: {
                            if (axis.regolarStep) {
                                var step = 10
                                if (axisLabels.gridLables.length-1 > 60)
                                    step = 15
                                if (axisLabels.gridLables.length-1 > 120)
                                    step = 30
                                if (axisLabels.gridLables.length-1 > 300)
                                    step = 60
                                var v = Math.round(index/step)
                                var mod = index-(v*step)
                                if (mod === 0 || index === axisLabels.gridLables.length-1)
                                    text = modelData
                                else
                                    text= ""
                            }
                            else
                                text = modelData
                        }
                    }
                }
            }
        }

        Row{
            id:rowSubGrid
            visible: textMinVisible
            x: leftMargin - axesEffectiveWidth()/((numNotches()-1)*2)
            Repeater{   //--- SUB GRID ----
                id: subGridRepeater
                model: axisLabels.subGridLables//numNotches()
                Rectangle
                {
                    id: rSubGrid
                    color:"transparent"
                    width: axesEffectiveWidth()/(numNotches()-1)
                    height:axis.height
                    Rectangle{
                        id:subGridSpacer
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top:parent.top
                        height:textMinDist
                        width: parent.width
                        color:"transparent"
                    }
                    Text{
                        id:subGridLabel
                        anchors.horizontalCenter: rSubGrid.horizontalCenter
                        anchors.top:subGridSpacer.bottom
                        font.family: fontMinFamily
                        color: fontMinColor
                        font.pointSize: fontMinSize
                        opacity: opacityMin
                        text: modelData//axisLabels.getSubGridLabel(index)
                    }
                }
            }
        }

        Text{
            id:unitLabel
            y: fontMajSize+textMajDist+textUOMDist
            anchors.horizontalCenter: parent.horizontalCenter

            font.family: fontUOMFamily
            color: fontUOMColor
            font.pointSize: fontUOMSize
            opacity: opacityUOM
            visible: unitOfMeasureVisible
            text: unitOfMeasure
            rotation: unitOfMeasureRotation
        }
    }
}

