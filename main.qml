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
    property string workFolder:"../../AcqTool/"
    property string examFolder:workFolder+"Exams/"
    property string configFolder:workFolder+"Config/"

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    visible: true
    width:Screen.width*0.9
    height:Screen.height*0.9

    Rectangle{
        color:"red"
        anchors.fill:parent
    }


    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:
    {

        var arguments=Qt.application.arguments
        if(arguments.length===4)
        {
            console.log("arguments founded",arguments,arguments.length)
            //gli argomenti sono acq/ana dataFile configFile
            mngAcq.startSupe("show")
            mngAcq.alarmFile=configFolder+"alarms.xml"
            if(Qt.application.arguments[1]==="acq")
            {
                mngAcq.newAcquisition(arguments[2],arguments[3]);
                forReal.configurationFile=mngAcq.plotConfigFileName()
                forHome.whoIsVisilbe=forReal.name
            }
            else
            {
                mngData.loadFile(Qt.application.arguments[2]);
                forAna.configurationFile=mngData.plotConfigFileName()
                forHome.whoIsVisilbe=forAna.name
            }
        }
        else
        {
            console.log("No arguments founded")
        }
        console.log("Application Ready!")
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MAcqManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngAcq

        //@@@@@@@@@@    Events          @@@@@@@@@@
        Component.onCompleted: console.log("MAcqManager Ready!")
    }

    MDataManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngData

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onInfoListChanged:forAna.populate()
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
