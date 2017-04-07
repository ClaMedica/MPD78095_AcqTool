import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0
import QtQml 2.0

import MComponents 1.0
import "qrc:/Components"
MForm{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string displayInformations:""
    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootAna
    anchors.fill: parent.fill

    color:"lightgray"

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function populate()
    {
        plot.tracks=mngData.getData("Track")
        plot.markers=mngData.getData("Marker")
        plot.frames=mngData.getData("Definer")
        plot.limits=mngData.getPlotLimits();
        lbNamePat.text = mngData.patientInfo
    }


    function initialize()
    {
        console.log("inizialize")
        mngData.registerModel("Track",traMod.strList)
        mngData.registerModel("Marker",marMod.strList)
        mngData.registerModel("Definer",fraMod.strList)
    }

    function save(pointer)
    {
       // dopAna.addSignals(pointer)
    }

    function setMarkersInfo(info)
    {
        console.log(info)
        gridMarker.items=info
    }
    function setDefinersInfo(info)
    {
        console.log(info)
        gridDefiners.items=info
    }

    function setCommandsInfo(info)
    {
        console.log(info)
        gridComand.items=info
    }


    function loadConfigurationFile(configurationFile)
    {
        mngCon.fileName = configurationFile
        plot.completed=true;
        mngCon.read()
    }

    //@@@@@@@@@@    Objects     @@@@@@@@@@


    MPlot2DStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: gridComand.left
        height: PicoFlow?(rootAna.height - 15):(rootAna.height - 25)
        plotProp:mngCon.plotSetting

        //@@@@@@@@@@    Events          @@@@@@@@@@

        onCurObjChanged: {
            if(completed)
                mngData.changeObject(curObj)
        }

        onToZoomChanged:
        {
            if (!PicoFlow)
            {
                var startTime = mngData.getStartTime()
                var endTime = mngData.getEndTime()
                var numSec = (endTime - startTime)/2

                var pageSize
                if (toZoom > oldToZoom){
                    pageSize = scroolBar.pageSize - (1/numSec)
                    if (pageSize < 0)
                        pageSize = 0
                    scroolBar.pageSize = pageSize
                }
                if (toZoom < oldToZoom){
                    pageSize = scroolBar.pageSize + (1/numSec)
                    var LimitSx,LimitDx
                    LimitSx = (scroolBar.position * (scroolBar.width-2) + 1 - scroolBar.barW/2)
                    LimitDx = LimitSx + (scroolBar.pageSize * (scroolBar.width-2))
                    if (LimitSx <= 0){
                        scroolBar.position += (0-LimitSx)/(scroolBar.width-2)//0.0075
                        pageSize = scroolBar.pageSize + (1/numSec)/2
                    }

                    if (LimitDx >= scroolBar.width-2){
                        scroolBar.position -= (LimitDx - scroolBar.width-2)/(scroolBar.width-2)
                        pageSize = scroolBar.pageSize + (1/numSec)/2//0.015
                    }
                    if (pageSize > 1)
                        pageSize = 1
                    scroolBar.pageSize = pageSize
                }
                if (toZoom === 0 && oldToZoom === 0){
                    scroolBar.pageSize = 1.0
                    scroolBar.position = 0.5
                }
            }
        }
    }

    MScroolBar{
        id: scroolBar
        clip:true
        visible: PicoFlow? false:true
        anchors.top:plot.bottom
        anchors.bottom: parent.bottom
        anchors.right: gridComand.left
        anchors.left: parent.left
        onMoved: {
            var moved = scroolBar.step
            plot.movingZoom = moved
        }
    }

    MLabel{
        id: lbNamePat
        clip:true
        visible: PicoFlow? true:false
        anchors.top:plot.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "patientName"
    }

    //Models

    MarkerModel{id:marMod}
    FrameModel{id:fraMod}
    TrackModel{id:traMod}

    //Managers

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
    }


    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridMarker
        anchors.right: rootAna.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:0
        owner:"Marker"
        itemsInRow:1
        delegate: MMarkerButton{
            onClick: {
                mngAcq.addMarker(value);
                //console.log(mngAcq.acqMarkers)
                plot.markers=mngAcq.acqMarkers;
            }
        }
        visible:false
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridDefiners
        anchors.right: gridMarker.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:0
        owner:"Definers"
        itemsInRow:1
        delegate: MMarkerButton {

        }
        visible:true

    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridComand
        anchors.right: gridDefiners.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:60
        owner:"Commands"
        itemsInRow:1
        delegate: MMarkerButton{
            onClick: {
                if (value === "111")
                    mngData.analysis();

                if (value === "112")
                    mngData.exitFromReview()

                if (value === "113"){
                    //zoom in
                    plot.oldToZoom = plot.toZoom
                    plot.toZoom = plot.toZoom + 1
                }
                if (value === "114"){
                    //zoom out
                    plot.oldToZoom = plot.toZoom
                    plot.toZoom = plot.toZoom - 1
                }
                if (value === "115"){
                    //zoom none
                    plot.oldToZoom = 0
                    plot.toZoom = 0
                }
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY
                    toolTip.text = testo
                    toolTip.visible = true
                }
            }
        }
        visible:true
    }



    Rectangle{
        property string text: ""
        id: toolTip
        width: toolTipText.width + 4
        height: toolTipText.height + 4
        anchors.horizontalCenter: gridComand.horizontalCenter
        y: 0
        color: "whitesmoke"
        border.color: "blue"
        visible:false
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:toolTipText
            color:"blue"
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: false
            font.pixelSize:12
            text:toolTip.text
            horizontalAlignment : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

}
