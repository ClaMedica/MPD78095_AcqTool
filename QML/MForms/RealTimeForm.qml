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
    property string configurationFile:""
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
        anchors.right: parent.right//box.left        
        height: root.height - 30 //- player.height
        plotProp:mngCon.plotSetting
        Behavior on width {NumberAnimation { duration: 1000 }}
    }

    //Managers

    ParameterManager{id:mngPar}

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName:fileDial.fileUrl
    }



    ParameterBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:box
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width:0
        height: parent.height - 30
        Behavior on width {NumberAnimation { duration: 1000 }}

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
                mngData.newAcquisition(lastAcqFileName);
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
                mngData.addMarker(currentItem,"Sono un marker");
                console.log(mngData.getAcqMarkers())
                plot.markers=mngData.getAcqMarkers();
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
                message="Do you want to analyze it?"
            }
            else
            {
                forAna.openFile(lastAcqFileName);
                forAna.loadConfiguration(configurationFile)
                forHome.whoIsVisilbe=forAna.name
            }
        }
        onRejected:
        {
            if(message=="Do you want to save the current exam?")
            {
                deleteExam()
                //timClose.start(1000);
            }
            else
            {
                timClose.start(1000);
            }
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
        id: forAnaMenu
        anchors.top: parent.top
        color: "transparent"
        height: root.height
        width: root.width
        opacity: rootRealTime.opacity
        theme:"green"
        items: [
            ["Acquisition","Start","Stop","Send","Back"],
            ["Save","Tracks"],
            ["Add","Marker","Start Definer","Stop Definer"],
            ["Analisys","Spectral","Wavelet","Energy","Time Spectrum"],
            ["Player","Show Player Audio"],
            ["Plot","Load Settings","Properties"]
        ]

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onSelected: {

            if (itemClicked == "Start") {               
                editName.owner="NewAcq"
                editName.visible=true
            }
            if (itemClicked == "Send") {

                plot.startAll();
                mngData.sendCommand(1)
            }
            if (itemClicked == "Stop") {
                messageDialog.visible=true
            }            
            if (itemClicked == "Back") {
                rootRealTime.back()
            }
            if (itemClicked == "Exit") {

            }
            if (itemClicked == "Marker")
            {
                grid.owner="Marker"
                grid.visible=true
                grid.display()                
            }

            if (itemClicked == "Tracks") {
                plot.saveScreen(0)
            }

            if (itemClicked == "Load Settings")
            {
                fileDial.owner="mngCon"
                fileDial.folder="../../Config"
                fileDial.setNameFilters("*.xml")
                fileDial.open()
            }

            if (itemClicked == "Properties")
            {
                box.ready=false
                box.items=["$Parameter",
                           "label","Page Time",
                           "name","Page Time",
                           "values",[0.1,0.2,0.5,1,2,5,10,20],
                           "&Parameter"]
                box.width=parent.width/2

                box.popola()
                box.ready=true
            }

        }
    }


}
