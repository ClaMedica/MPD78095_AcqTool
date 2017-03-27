import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
//import Resources 1.0
import "qrc:/Forms"
import "qrc:/Dialog"

import MComponents 1.0
import Managers 1.0
import QtQuick.Window 2.2

//import QtQuick.VirtualKeyboard 1.0

ApplicationWindow {
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property var keyboard:appKey
    property string examFolder:settings.datafilePath
    property string configFolder:settings.appPath()

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    flags: Qt.FramelessWindowHint
    visible: false

    width:platform==="linux"?640:Screen.width//*0.94
    height:platform==="linux"?480:Screen.height//*0.94

    color:"steelblue"

    function launchDEBUG(mode,dataFile)
     {
         console.log("launchDEBUG(mode,dataFile)", mode, dataFile)

         if(mode === "acq")
         {
             console.log("Start new acq")
             mngAcq.load()
             mngAcq.newAcquisition(dataFile)
             forReal.setMarkersInfo(mngAcq.markersInfo("type", [1, 6]))
             forReal.configurationFile = mngAcq.plotConfigFileName()
             forReal.displayMessage("Wait for inizialization...", -1)
             forHome.whoIsVisible = forReal.name
             forReal.setAcqInfo(mngData.acqInfo())
         }
         else if(mode === "vis")
         {
             console.log("Start new vis")
             forAna.initialize()
             mngData.load()
             if(platform !== "android")
                 mngData.loadFile(dataFile);
             else
                 mngData.loadFile("/mnt/sdcard/Medica/pv000461A.pic")
             forAna.setMarkersInfo(mngData.markersInfo("type", [1, 6]))
             forAna.setDefinersInfo(mngData.definersInfo())
             forAna.setCommandsInfo(mngData.commandsInfo())
         }
         console.log("Application Ready!")
     }

    //@@@@@@@@@@    Events          @@@@@@@@@@
     Component.onCompleted: {
         console.log("debug??? ", Qt.application.arguments[3])
         if (Qt.application.arguments[3] === "debug") {
             root.visible = true
             launchDEBUG(Qt.application.arguments[1],Qt.application.arguments[2])
         }
     }

    function launch(mode,dataFile)
    {
        console.log("launch(mode,dataFile)", mode, dataFile)

        if(mode === "acq")
        {
            console.log("Start new acq")
            mngAcq.load()
            mngAcq.newAcquisition(dataFile)
            forReal.setMarkersInfo(mngAcq.markersInfo("type", [1, 6]))
            forReal.configurationFile = mngAcq.plotConfigFileName()
            forReal.displayMessage("Wait for initialization...", -1)
            forHome.whoIsVisible = forReal.name
            forReal.setAcqInfo(mngData.acqInfo())
        }
        else if(mode === "vis")
        {
            console.log("Start new vis")
            forAna.initialize()
            mngData.load()
            if(platform !== "android")
                mngData.loadFile(dataFile);
            else
                mngData.loadFile("/mnt/sdcard/Medica/pv000461A.pic")
            forAna.setMarkersInfo(mngData.markersInfo("type", [1, 6]))
            forAna.setDefinersInfo(mngData.definersInfo())
            forAna.setCommandsInfo(mngData.commandsInfo())
        }
        console.log("Application Ready!")
        bridgeMain.sendSwitch()
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Connections {
        id: connMainApp
        target: bridgeMain
        ignoreUnknownSignals: true
        onNewAcquisition:
        {
            console.log(datafile);
            launch("acq", datafile)
        }
        onNewVisualization:
        {
            console.log(datafile);
            launch("vis", datafile)
        }
    }


    MAcqManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngAcq

        //@@@@@@@@@@    Events          @@@@@@@@@@
        Component.onCompleted: console.log("MAcqManager Ready!")
        onSystemInAcqStatus:
        {
            console.log("Go Go Go");
            forReal.displayMessage("start",2000)
        }
    }

    MDataManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngData

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onLoadingCompleted: {
            forAna.loadConfigurationFile(mngData.plotConfigFileName())
            forAna.populate()
            forHome.whoIsVisible = forAna.name
        }

        onReloadingCompleted: forAna.populate()

        onSg_openVolResDlg:{
            //itemVolRes.visible = true
            //itemVolRes.focus = true
            volRes.focus = true
            volRes.volResVal = "0"
            volRes.titleDlg = __tipoAn
            volRes.visible = true
        }

        onSg_loadResult:{
            forAna.visible = false
            forRes.loadPageAnalysis()
            forRes.visible = true
        }

        onSg_exitFromReview:{
            exit.visible = true;
        }

        Component.onCompleted: console.log("MDataManager Ready!")
    }

    MForm {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forHome
        anchors.fill: parent
        name:"HomeForm"
        whoIsVisible: "HomeForm"
    }


    AnalysisForm {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forAna
        anchors.fill: parent
        name:"AnalysisForm"
        whoIsVisible:forHome.whoIsVisible

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onBack: forHome.whoIsVisible = forHome.name
    }


    RealTimeForm{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forReal
        anchors.fill: parent
        name: "RealTimeForm"
        whoIsVisible: forHome.whoIsVisible

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onBack: forHome.whoIsVisible = forHome.name
    }


    ResultForm{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forRes
    }

    VolResDlg{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: volRes
        visible: false
    }

    ExitDlg{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: exit
        visible: false
    }
    /*
    //per la tastiera virtuale
    InputPanelNumeric {
        id: inputPanelNumeric
        z: 99
        y: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
       // visible: false
        states: State {
            name: "visible"
            when: Qt.inputMethod.visible
            PropertyChanges {
                target: inputPanelNumeric
                y: parent.height - inputPanelNumeric.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 150
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }*/
    MKeyboard{
        id:appKey
        visible:false
        z:300
    }
}
