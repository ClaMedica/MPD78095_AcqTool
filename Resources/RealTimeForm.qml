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
    function displayMessage(mex,time)
    {
        pop.message=mex
        pop.display(1,0)
        if(time!==-1)
            pop.display(0,time)
    }

    function setMarkersInfo(info)
    {
        console.log(info)
        grid.items=info
    }

    function setAcqInfo(info)
    {
        console.log(info)
        gridAcq.items=info
    }

    //@@@@@@@@@@    Events      @@@@@@@@@@

    //@@@@@@@@@@    Objects         @@@@@@@@@@


    FileDialog{
        //@@@@@@@@@@    Definitions     @@@@@@@@@@
        property string owner:"none"

        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: fileDial
        folder: "../../"

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onAccepted:
        {
            switch(owner)
            {
            case "mngCon":
                console.log(fileUrl)
                mngCon.fileName=fileUrl
                mngCon.read()
                console.log(mngCon.plotSetting)
                break;
            default:console.log("Should Not Be Here!");break;

            }
        }
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

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onPlotPropChanged: if(mngCon.fileName!==""){console.log("start!");plot.startAll()}
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

    MNewName{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:editName
        height: 100
        width:300
        anchors.centerIn: parent
        owner:""

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onSelected: {
            //a seconda di chi lo sta usando decido cosa fare
            switch(owner)
            {
            case "NewAcq":
                lastAcqFileName=examFolder+curText+".pic"
                mngAcq.newAcquisition(lastAcqFileName);
                mngCon.fileName=configurationFile
                mngCon.read()
                plot.startAll();
                break;
            default:break;
            }
            visible=false

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
        delegate: MMarkerButton{}
        visible:false
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:
        {
            switch(owner)
            {
            case "Marker":
                mngAcq.addMarker(value);
                //console.log(mngAcq.acqMarkers)
                plot.markers=mngAcq.acqMarkers;
                break;
            }
        }
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
        delegate: MMarkerButton{}
        visible:true


        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:
        {
            switch(owner)
            {
            case "Acq":
                if (value === "112") {
                    mngAcq.sendStopAcq()
                    plot.stopAll()
                    mngAcq.endAcquisitionSave()
                    timClose.start(1000);
                }

                break;
            }
        }
    }

    Timer{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:timClose

        //@@@@@@@@@@    Events          @@@@@@@@@@
        //onTriggered: Qt.quit()
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


//    MMenuBar {
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id: forReaMenu
//        anchors.top: parent.top
//        color: "transparent"
//        height: root.height
//        width: root.width
//        opacity: rootRealTime.opacity
//        theme:"green"
//        items: [
//            ["Acquisition","Start",qsTr("Exit without saving"),qsTr("Exit and save"),qsTr("Exit, save and review")],
//            ["Add","Marker","Start Definer","Stop Definer"],
//            ["Plot","Properties"]
//        ]

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onSelected: {
//            if (itemClicked == "Start") {
//                editName.owner="NewAcq"
//                editName.visible=true
//                box.popola()
//            }
//            if (itemClicked === qsTr("Exit without saving")) {
//                mngAcq.sendStopAcq()
//                plot.stopAll()
//                mngAcq.endAcquisitionDiscard()
//                timClose.start(1000);
//            }
//            if (itemClicked === qsTr("Exit and save")) {
//                mngAcq.sendStopAcq()
//                plot.stopAll()
//                mngAcq.endAcquisitionSave()
//                timClose.start(1000);
//            }
//            if (itemClicked === qsTr("Exit, save and review")) {
//                mngAcq.sendStopAcq()
//                plot.stopAll()
//                mngAcq.endAcquisitionSave()
//                var arg=[]
//                arg[0]=Qt.application.arguments[0]
//                arg[1]="vis"
//                arg[2]=Qt.application.arguments[2]
//                arg[3]=Qt.application.arguments[3]
//                console.log("Lancioooo",arg[1])
//                launch(arg)
//            }

//            if (itemClicked == "Properties")
//            {
//                //box.ready=false
//                box.width=parent.width/2

//                //box.popola()
//                //box.ready=true
//            }

//        }
//    }

    MPopUp{
        id:pop
        anchors.centerIn: parent
        width:200
        height:100
    }

}
