import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import MComponents 1.0
import MPlotModule 1.0
import MForms 1.0
import Managers 1.0

ApplicationWindow {
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string workFolder:"../../AcqTool/"
    property string examFolder:workFolder+"Exams/"
    property string configFolder:workFolder+"Config/"

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: root
    visible: true
    width: 800
    height: 600

    Rectangle{
        color:"red"
        anchors.fill:parent
    }


    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:
    {
        console.log("Application Ready!")
        if(Qt.application.arguments[1]!==undefined)
        {
            forReal.fileName=Qt.application.arguments[1]
            mngData.acqFile=Qt.application.arguments[1]
            forHome.whoIsVisilbe=forReal.name
        }
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MDataFileManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngData

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onDataNewsChanged:forAna.storeInfo(dataNews)        
        Component.onCompleted: console.log("MDataFileManager Ready!")
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
        onSaveExam: mngData.saveAcquisition()
    }


}
