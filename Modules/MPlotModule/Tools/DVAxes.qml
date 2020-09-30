import QtQuick 2.2
import MPlotModule 1.0
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle {
    id:axis
    property int    major:              3
    property int    minor:              3

    property int    format:             AxisSettings.EFORMAT_FLOAT
    property int    decimalsMaj:        1
    property int    decimalsMin:        2
    property string dateTimeformatMaj:  "hh:mm:ss.zzz"
    property string dateTimeformatMin:  "mm:ss.zzz"

    property real   max:            10
    property real   min:            0
    //Proprietà del testo Major
    property string fontMajFamily:     "Luxi Serif"
    property color  fontMajColor:      "white"
    property int    fontMajSize:       12
    property int    textMajDist:       10
    property bool   textMajVisible:    true
    property real   opacityMaj:        1.0
    //Proprietà del testo Minor
    property string fontMinFamily:     "Luxi Serif"
    property color  fontMinColor:      "white"
    property int    fontMinSize:       12
    property int    textMinDist:       10
    property bool   textMinVisible:    true
    property real   opacityMin:        1.0

    //Unit of measure
    property string unitOfMeasure:     "[unit]"
    property bool   unitOfMeasureVisible:    true
    property string fontUOMFamily:     "utopia"
    property color  fontUOMColor:      "white"
    property int    fontUOMSize:       12
    property int    textUOMDist:       5
    property real   opacityUOM:        1.0
    property int    unitOfMeasureRotation: -90

    property real topMargin:50
    property real bottomMargin:50
    //Proprietà dell'oggetto
    color:"transparent"

    function numNotches() { return axis.major+axis.minor*(axis.major-1)}
    function axisEffectiveHeight(){return axis.height-topMargin-bottomMargin}

    AxisSettings{
        id: axisLabels
        max:axis.max
        min:axis.min
        gridNumLines: axis.major
        subGridNumLines: 0
//        subGridNumLines: axis.minor
        axisFormat: axis.format
        numberDecimalMaj: axis.decimalsMaj
        numberDecimalMin: axis.decimalsMin
        dateTimeFormatMaj: axis.dateTimeformatMaj
        dateTimeFormatMin: axis.dateTimeformatMin
        invert: true
    }

    Rectangle{
        anchors.fill: parent
        color:"transparent"
        Column{
            id:colGrid
            y: topMargin - ( axisEffectiveHeight()/((major-1)*2))
            Repeater{
                id: gridRepeater
                model: axisLabels.gridLables
                Rectangle
                {
                    id: rGrid
                    color:"transparent"
                    height: axisEffectiveHeight()/(major-1)
                    width:axis.width
                    Rectangle{
                        id:gridSpacer
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right:parent.right
                        width:textMajDist
                        height: parent.height
                        color:"transparent"
                    }
                    Text{
                        id:gridLabel
                        anchors.verticalCenter: rGrid.verticalCenter
                        anchors.right:gridSpacer.left
                        font.family: fontMajFamily
                        color: fontMajColor
                        font.pointSize: fontMajSize
                        opacity: opacityMaj
                        visible: textMajVisible
                        text: modelData
                    }
                }
            }
        }

        Column{
            id:colSubGrid
            y: topMargin - axisEffectiveHeight()/((numNotches()-1)*2)
            Repeater{   //--- SUB GRID ----
                id: subGridRepeater
                model: axisLabels.subGridLables
                Rectangle
                {
                    id: rSubGrid
                    color:"transparent"
                    height: axisEffectiveHeight()/(numNotches()-1)
                    width:axis.width
                    Rectangle{
                        id:subGridSpacer
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right:parent.right
                        width:textMinDist
                        height: parent.height
                        color:"transparent"
                    }
                    Text{
                        id:subGridLabel
                        anchors.verticalCenter: rSubGrid.verticalCenter
                        anchors.right:subGridSpacer.left
                        font.family: fontMinFamily
                        color: fontMinColor
                        font.pointSize: fontMinSize
                        opacity: opacityMin
                        visible: textMinVisible
                        text: modelData
                    }
                }
            }
        }

        Text{
            id:unitLabel
            y: textUOMDist
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -(parent.width/3)
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
/*Column{
        id:col
        y: topMargin -( axisEffectiveHeight()/(numNotches()*2))
        Repeater{
            model:numNotches()
            Rectangle{
                id:sample
                color:"transparent"
                height: axisEffectiveHeight()/(numNotches()-1) - 0.5 //0.5 for rounding
                width:axis.width
                Rectangle{
                    id:spacer
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right:parent.right
                    width:textMajDist
                    height: parent.height
                    color:"transparent"
                }
                Text{
                    id:testo
                    anchors.verticalCenter: sample.verticalCenter
                    anchors.right:spacer.left
                    font.family: fontMajFamily
                    color: axis.fontMajColor
                    font.pointSize: fontMajSize
                    opacity: opacityMaj
                    visible: textMajVisible
                    text:
                        if(0!=(Math.round(min+(numNotches()-1-index)*(max-min)/(numNotches()-1))-(min+(numNotches()-1-index)*(max-min)/(numNotches()-1))))
                            {(min+(numNotches()-1-index)*(max-min)/(numNotches()-1)).toFixed(decimals)}
                        else
                            {(min+(numNotches()-1-index)*(max-min)/(numNotches()-1)).toFixed(0)}

                }

            }
        }*/

