import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import MComponents 1.0
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
    signal saveExam
    signal deleteExam

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootRealTime
    background: Images.realTime
    visible:whoIsVisilbe===name?true:false
    clip:true

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

    MAlarmBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:alarmBox
        alarmHeight: 50
        anchors.topMargin: 30
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:parent.right
        alarms: mngAcq.alarms

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onResetAlarms: mngAcq.resetAlarms()
    }

    MPlot2DRealStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right//box.left
        height: root.height - 30 - alarmBox.height
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
        anchors.centerIn: rootRealTime
        model:[1,2,3,4,5,65,7,8,9]
        type:"text"
        visible:false
        width:220

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:
        {
            switch(owner)
            {
            case "Marker":
                mngAcq.addMarker(currentItem,"Sono un marker");
                //console.log(mngAcq.acqMarkers)
                plot.markers=mngAcq.acqMarkers;
                break;
            }
        }
    }

    MMessageYesNo{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: messageDialog
        anchors.centerIn: parent
        message: "Do you want to save the current exam?"
        visible:false

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onAccepted: {
            if(message=="Do you want to save the current exam?")
            {
                saveExam()
                plot.stopAll()
                timClose.start(1000);
                //message="Do you want to analyze it?"
            }
//            else
//            {
//                forAna.openFile(mngAcq.acqFile);
//                forAna.loadConfiguration(mngAcq.configurationFile)
//                forHome.whoIsVisilbe=forAna.name
//            }
        }
        onRejected:
        {
            if(message=="Do you want to save the current exam?")
            {
                deleteExam()
                timClose.start(1000);
            }
//            else
//            {
//                timClose.start(1000);
//            }
        }
    }



    Timer{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:timClose

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onTriggered: Qt.quit()
    }


    MMenuBar {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forReaMenu
        anchors.top: parent.top
        color: "transparent"
        height: root.height
        width: root.width
        opacity: rootRealTime.opacity
        theme:"green"
        items: [
            ["Acquisition","Start","Stop"],
            ["Add","Marker","Start Definer","Stop Definer"],
            ["Plot","Properties"]
        ]

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onSelected: {

            if (itemClicked == "Start") {
                editName.owner="NewAcq"
                editName.visible=true
                box.popola()
            }
            if (itemClicked == "Stop") {
                messageDialog.visible=true
                mngAcq.sendStopAcq()
            }
            if (itemClicked == "Marker")
            {
                grid.owner="Marker"
                grid.visible=true
                grid.display()
            }

            if (itemClicked == "Properties")
            {
                //box.ready=false
                box.width=parent.width/2

                //box.popola()
                //box.ready=true
            }

        }
    }


}
