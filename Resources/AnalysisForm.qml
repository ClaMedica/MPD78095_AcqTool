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

    function printImagePlot()
    {
        plot.grabToImage(
                    function(result) { mngData.getGrabbedImage(result, "grafo"); },
                    Qt.size(600,400)
                    )
    }

    //@@@@@@@@@@    Objects     @@@@@@@@@@

    MPlot2DStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: gridComand.left
        anchors.bottom: parent.bottom
        height: PicoFlow?(rootAna.height - 15):(rootAna.height - 25)
        plotProp:mngCon.plotSetting

        //@@@@@@@@@@    Events          @@@@@@@@@@

        onCurObjChanged: {
            if(completed)
                mngData.changeObject(curObj)
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

                if (value === "113")
                    //zoom in
                    plot.zoomFromButton("zoomIn")

                if (value === "114")
                    //zoom out
                    plot.zoomFromButton("zoomOut")

                if (value === "115")
                    //zoom none
                    plot.zoomFromButton("zoomReset")
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
        width: toolTipText.width *1.1
        height: toolTipText.height *1.1
        anchors.right: gridDefiners.left
        y: 0
        color: "whitesmoke"
        border.color: "blue"
        border.width: 2
        radius: 5
        visible:false
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:toolTipText
            color:"blue"
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: false
            font.pixelSize:screenH * 0.02
            text:toolTip.text
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }


}
