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
    property bool btOkPrint: false

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    flags:Qt.Window | Qt.FramelessWindowHint
    visible: false

    width:platform==="linux"?640:Screen.width
    height:platform==="linux"?480:Screen.height+1

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
            forAna.disablebuttons(__disable)
        }

        onSg_exitFromReview:{
             exit.open()
        }

        onSg_warning: {
            warning.testoWarning = __msg
            warning.open()
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

    WarningDlg {
        id: warning
        visible: false
    }

    MKeyboard{
        id:appKey
        visible:false
        z:300
    }
}
