import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import MComponents 1.0
import MPlotModule 1.0
import MForms 1.0
import Managers 1.0
import QtQuick.Window 2.2

ApplicationWindow {
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string examFolder:settings.datafilePath
    property string configFolder:settings.appPath()

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    visible: true
    width:Screen.width*0.9
    height:Screen.height*0.9

    Rectangle{
        color:"transparent"
        anchors.fill:parent
    }

    MSettings{
        id:settings
        Component.onCompleted: loadSettings()
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:
    {
        var arguments=Qt.application.arguments
        if(arguments.length>1)
        {
            console.log("arguments founded",arguments,arguments.length)
            //gli argomenti sono acq/ana dataFile lingua


            if(Qt.application.arguments[1]==="acq")
            {
                mngAcq.load()
                mngAcq.startSupe("hide")
                mngAcq.newAcquisition(arguments[2]);
                console.log("Start configuring screen")
                forReal.setMarkersInfo(mngAcq.markersInfo("type",[1,6]))
                forReal.configurationFile=mngAcq.plotConfigFileName()
                forReal.displayMessage("Wait for inizialization...",-1)
                forHome.whoIsVisilbe=forReal.name

            }
            else if(Qt.application.arguments[1]==="vis")
            {
                forAna.initialize()
                mngData.load()
                mngData.loadFile(Qt.application.arguments[2]);
            }
        }
        else
        {
            console.log("No arguments founded")
            mngData.loadFile(examFolder+"/pv000641A.pic")
        }
        console.log("Application Ready!")
    }

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
        onSaveMeChanged: forAna.save(saveMe)
        onSaveExam: mngAcq.endAcquisition("TEST_SAVE")
        onDeleteExam: mngAcq.endAcquisition("TEST_DISCARD")
    }


}
