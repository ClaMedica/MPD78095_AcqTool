import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
//import Resources 1.0
import "qrc:/Forms"
import "qrc:/Dialog"

import MComponents 1.0
import Managers 1.0
import QtQuick.Window 2.2
import FileIO 1.0

//import QtQuick.VirtualKeyboard 1.0

ApplicationWindow {
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    //property var keyboard:appKey
    property string examFolder:settings.datafilePath
    property string configFolder:settings.appPath()
    property bool vis: false
//    property int btStatusUdp: 0    // 0:don't care 1:stopping 2:stopped 3:restarting 4:restarted
    property bool btStopped: false
    property bool timBtAvail: true
    property bool btOkPrint: false

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    flags:Qt.Window | Qt.FramelessWindowHint
    visible: false

    width:platform==="linux"?640:Screen.width//*0.94
    height:platform==="linux"?480:Screen.height//*0.94

    color:"steelblue"

    function launchDEBUG(mode,dataFile,codSoft)
     {
         console.log("launchDEBUG(mode,dataFile)", mode, dataFile,codSoft)

         if(mode === "acq")
         {
             console.log("Start new acq")
             mngAcq.load()
             mngAcq.newAcquisition(dataFile)
             forReal.setMarkersInfo(mngAcq.markersInfo("type", [1, 6]))
             forReal.configurationFile = mngAcq.plotConfigFileName()
             forReal.displayMessage(qsTr("Wait for ..."), -1,2)
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
             forAna.setActionsInfo(mngData.actionsInfo())
             forAna.setCommandsInfo(mngData.commandsInfo())
             forAna.setCommandsInfoBottom(mngData.commandsInfoBottom())
         }
         console.log("Application Ready!")
     }

    //@@@@@@@@@@    Events          @@@@@@@@@@
     Component.onCompleted: {
         acqLoaded.createFileLoaded()
         console.log("debug??? ", Qt.application.arguments[4])
         if (Qt.application.arguments[4] === "debug") {
             root.visible = true
             launchDEBUG(Qt.application.arguments[1],Qt.application.arguments[2], Qt.application.arguments[3])
         }
     }

    function launch(mode,dataFile,codSoft)
    {
        console.log("launch(mode,dataFile)", mode, dataFile)
        if(mode === "acq")
        {
            console.log("Start new acq", codSoft)
            mngAcq.load()
            mngAcq.newAcquisition(dataFile)
            forReal.setMarkersInfo(mngAcq.markersInfo("type", [1, 6]))
            forReal.configurationFile = ""
            forReal.configurationFile = mngAcq.plotConfigFileName()
            forReal.displayMessage(qsTr("Wait for ..."), -1,1)
            forHome.whoIsVisible = forReal.name
            forReal.setAcqInfo(mngAcq.acqInfo())
        }
        else if(mode === "vis")
        {
            if (PicoFlow) suspendBt()
            console.log("Start new vis", codSoft)
            forAna.initialize()
            mngData.load()
            if(platform !== "android")
                mngData.loadFile(dataFile);
            else
                mngData.loadFile("/mnt/sdcard/Medica/pv000461A.pic")
            forAna.setMarkersInfo(mngData.markersInfo("type", [1, 6]))
            forAna.setDefinersInfo(mngData.definersInfo())
            forAna.setActionsInfo(mngData.actionsInfo())
            forAna.setCommandsInfo(mngData.commandsInfo())
            forAna.setCommandsInfoBottom(mngData.commandsInfoBottom())
        }
//        else if (mode === "sta")
//        {
//            if (PicoFlow) suspendBt()
//            console.log("Start stampa prova")
//            mngData.sendPrintTest()
//        }

        console.log("Application Ready!")
        if (mode !== "sta")
            bridgeMain.sendSwitch()
    }



    //@@@@@@@@@@    Objects         @@@@@@@@@@
    FileIO {
        id: acqLoaded
    }

    function suspendBt()
    {
//        console.log("================ btStatusUdp:",btStatusUdp,"btOkPrint",btOkPrint)
        mngAcq.send_Command(4)   // STOPBT
//        if(btStatusUdp == 0)
//            timStopBT.start()
    }

//    function restartBt()
//    {
////        console.log("================ btStatusUdp:",btStatusUdp,"btOkPrint",btOkPrint)
//        mngAcq.send_Command(5)   // STARTBT
////        if(btStatusUdp == 2)
////            timStopBT.start()
//    }

//    Timer{
//        id: timStopBT
//        interval: 500
//        repeat: true
//        onTriggered: {
//            console.log("================ btStatusUdp:",btStatusUdp,"btOkPrint",btOkPrint)
//            switch(btStatusUdp) {  // 0:don't care 1:stopping 2:stopped 3:restarting 4:restarted
//            case 0:
//                mngAcq.send_Command(4)   // STOPBT
//                btStatusUdp = 1
//                break
//            case 1: // da 1 a 2 alla ricezione di udpBtStopped
//                break
//            case 2:
//                if(!btOkPrint) {
//                    btOkPrint = true    //  mngData.sendToPrint()
//                    stop()
//                }
//                else {
////                    mngAcq.send_Command(5)   // STARTBT
//                    btStatusUdp = 3
//                }
//                break
//            case 3: // da 3 a 4 alla ricezione di udpBtRestarted
//                break
//            case 4:
//                btStatusUdp = 0
//                btOkPrint = false
//                stop()
//                break
//            }

//        }
//    }


    Connections {
        id: connMainApp
        target: bridgeMain
        ignoreUnknownSignals: true
        onNewAcquisition:
        {
            console.log(datafile);
            launch("acq", datafile, codSoft)
            vis = false
        }
        onNewVisualization:
        {
            console.log(datafile);
            launch("vis", datafile, codSoft)
            vis = true
        }
        onStampaProva:
        {
            console.log("Stampa di prova");
            launch("sta","")
        }
        onClosePico:
        {
            mngData.exitFromReview()
        }
        onAnaAutomatica:
        {
            if (vis && mngData.getAutoFlow() === 0){
                console.log("analisi automatica");
                mngData.analysis()
            }
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
            forReal.displayMessage(qsTr("start"),3000,6)
        }
        onAcquisitionEnded:
        {
            console.log("ENDED")
            forReal.endAcq()
        }
        onUdpBtStopped:
        {
            console.log("udp BtStopped")
//            btStatusUdp = 2
        }
        onUdpBtRestarted:
        {
            console.log("udp BtRestarted")
//            btStatusUdp = 4
        }
        onNoBeaker:
        {
            forReal.noBeaker()
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
            volRes.focus = true
            volRes.titleDlg = __tipoAn
            volRes.open()
        }

        onSg_loadResult:{
            forAna.visible = false
            forRes.loadPageAnalysis()
            forRes.visible = true

        }

        onSg_openReport: {
            forRes.disablebuttons(__disable)
        }

        onSg_exitFromReview:{
             exit.open()
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
        visible: false
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
