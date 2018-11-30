import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import MComponents 1.0
import "qrc:/Components"
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0

MForm{
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string fileName:""
    property alias configurationFile:mngCon.fileName
    property string lastAcqFileName:""
    property int saveMe:plot.savedData
    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootRealTime    

    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function displayMessage(mex,time,rows)
    {
        pop.rows =rows
        pop.message = mex
        pop.display(1, 0)
        if(time !== -1)
            pop.display(0, time)
    }

    function setMarkersInfo(info)
    {
        console.log(info)
        grid.items = info
    }

    function setAcqInfo(info)
    {
        console.log(info)
        gridAcq.items = info
    }

    function endAcq()
    {
        plot.stopAll()
    }

    MPlot2DRealStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: grid.left
        height: root.height - alarmBox.height
        plotProp:mngCon.plotSetting
        Behavior on width {NumberAnimation { duration: 1000 }}

//        //@@@@@@@@@@    Events          @@@@@@@@@@
        onPlotPropChanged:
        {
            if(mngCon.fileName!=="")
            {
                console.log("start!");
                plot.startAll()
            }

        }
    }

    //Managers

    ParameterManager{id:mngPar}

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onFileNameChanged: if(fileName!==""){plot.completed=true;read()}
    }



    ParameterBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:box
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width:0
        height: parent.height - 30
        Behavior on width {NumberAnimation { duration: 1000 }}
        items:["$Parameter",
            "label","Page Time",
            "name","Page Time",
            "values",[10,30,60,120,300],
            "&Parameter"]
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onValueNewsChanged:
        {
            if(valueNews.length===3)
            {
                if(valueNews[0]==="none" && valueNews[1]==="Page Time")
                {
                    plot.setPageTime(valueNews[2])
                }
            }
        }

    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:grid
        anchors.right: gridAcq.left
        anchors.top: parent.bottom
        anchors.bottom: parent.bottom
        width:0
        owner:"Marker"
        itemsInRow:2
        delegate: MMarkerButton{
            onClick:{
                mngAcq.addMarker(value);
                //console.log(mngAcq.acqMarkers)
                plot.markers=mngAcq.acqMarkers;
            }
        }
        visible:false

    }


    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridAcq
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:60
        owner:"Acq"
        itemsInRow:1
        delegate: MMarkerButton{
            onClick:{
                if (value === "116") {
                    //necessaria richiesta di conferma
                    dlgDiscard.visible = true
                }
            }
        }
        visible:true
    }


    MAlarmBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:alarmBox
        alarmHeight: 50
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:grid.left
        alarms: mngAcq.alarms

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onResetAlarms: mngAcq.resetAlarms()
    }

    MPopUp {
        id: pop
        anchors.centerIn: parent
        width: 200
        height: 100
    }

    Rectangle {
        id: dlgDiscard
        height:screenH*grafic.valueOf("Dialog","height")
        width:screenW*grafic.valueOf("Dialog","width")
        anchors.centerIn: parent
        visible: false
        color : layout.value("BackgroundColor")

        Keys.onReturnPressed: {
            dlgDiscard.visible = false
        }

        MLabel
         {
             anchors.top:parent.top
             anchors.left: parent.left
             anchors.right:parent.right
             height:parent.height*0.8
             labelSize: grafic.valueOf("Dialog","labelSize")
             color: "white"
             horizontalAlignment: Text.AlignHCenter
             verticalAlignment: Text.AlignVCenter
             text: qsTr("Are you sure to discard the exam?")
             wrapMode: Text.WordWrap
         }

        MButton {
            id: btnYes
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: parent.height/5
            width: parent.width/3
            anchors.margins: 10
            labelSize: layout.value("F4")
            text: qsTr("Yes")
            onClicked: {
                dlgDiscard.visible = false
                plot.stopAll()
                mngAcq.endAcquisitionDiscard()
            }
        }

        MButton {
            id: btnNo
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            height: parent.height/5
            width: parent.width/3
            anchors.margins: 10
            labelSize: layout.value("F4")
            text: qsTr("No")
            onClicked: dlgDiscard.visible = false
        }
    }
}
