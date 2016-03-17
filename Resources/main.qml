import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
//import Resources 1.0
import "qrc:/Forms"
import "qrc:/Dialog"
import MPlotModule 1.0
import Managers 1.0
import QtQuick.Window 2.2

import QtQuick.VirtualKeyboard 1.0



ApplicationWindow {
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string examFolder:settings.datafilePath
    property string configFolder:settings.appPath()
    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    flags: Qt.FramelessWindowHint
    visible: true

    width:platform==="android"?640:Screen.width//*0.94
     height:platform==="android"?480:Screen.height//*0.94


      // width:640
      // height:480
    color:"steelblue"

    function launch(arguments)
    {

        if(arguments.length>1)
        {
            console.log("arguments founded",arguments,arguments.length)
            //gli argomenti sono acq/ana dataFile lingua


            if(arguments[1]==="acq")
            {
                mngAcq.load()
                mngAcq.startSupe(arguments[3])
                mngAcq.newAcquisition(arguments[2]);
                console.log("Start configuring screen")
                forReal.setMarkersInfo(mngAcq.markersInfo("type",[1,6]))
                forReal.configurationFile=mngAcq.plotConfigFileName()
                forReal.displayMessage("Wait for inizialization...",-1)
                forHome.whoIsVisilbe=forReal.name

            }
            else if(arguments[1]==="vis")
            {
                forAna.initialize()
                mngData.load()
                if(platform!=="android")
                    mngData.loadFile(arguments[2]);
                else
                    mngData.loadFile("/mnt/sdcard/Medica/pv000461A.pic")
                forAna.setMarkersInfo(mngData.markersInfo("type",[1,6]))
                forAna.setDefinersInfo(mngData.definersInfo())
                forAna.setCommandsInfo(mngData.commandsInfo())
            }
        }
        else
        {
            console.log("No arguments founded")
            forAna.initialize()
            mngData.load()
            if(platform==="android")
                mngData.loadFile("/mnt/sdcard/Medica/pv000461A.pic")
        }
        console.log("Application Ready!")
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:launch(Qt.application.arguments)


    //@@@@@@@@@@    Objects         @@@@@@@@@@


    MAcqManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngAcq

        //@@@@@@@@@@    Events          @@@@@@@@@@
        Component.onCompleted: console.log("MAcqManager Ready!")
        onSystemInAcqStatus:forReal.displayMessage("Go go go!",2000)
    }

    MDataManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngData

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onLoadingCompleted:{
            forAna.configurationFile=mngData.plotConfigFileName()
            forAna.populate()
            forHome.whoIsVisilbe=forAna.name
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

    HomeForm {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forHome
        anchors.fill: parent
        name:"HomeForm"
        whoIsVisilbe: "HomeForm"
    }


    AnalysisForm{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forAna
        anchors.fill: parent
        name:"AnalysisForm"
        whoIsVisilbe:forHome.whoIsVisilbe

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onBack: forHome.whoIsVisilbe=forHome.name
    }


    RealTimeForm{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: forReal
        anchors.fill: parent
        name:"RealTimeForm"
        whoIsVisilbe: forHome.whoIsVisilbe

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onBack: forHome.whoIsVisilbe=forHome.name
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
    }
}
