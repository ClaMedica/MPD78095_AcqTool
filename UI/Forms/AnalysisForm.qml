import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0
import UI 1.0

MForm{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property alias configurationFile:mngCon.fileName
    property string displayInformations:""
    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootAna
    background:"qrc:/Images/Analysis"
    visible:whoIsVisilbe===name?true:false

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function openFile(file)
    {
        mngData.loadFile(file)
        plot.setAbsXmin(mngData.getStartTime())
        plot.setAbsXmax(mngData.getEndTime())
    }

    function populate()
    {
        console.log("populate")
        plot.tracks=mngData.getData("Track")
        plot.markers=mngData.getData("Marker")
        plot.frames=mngData.getData("Definer")
        plot.limits=mngData.getPlotLimits();
    }

    function initialize()
    {
        console.log("inizialize")
        mngData.registerModel("Track",traMod.strList)
        mngData.registerModel("Marker",marMod.strList)
        mngData.registerModel("Definer",fraMod.strList)
    }

    function save(pointer)
    {
       // dopAna.addSignals(pointer)
    }

    //@@@@@@@@@@    Objects     @@@@@@@@@@

    /*@@@@@@@@@@@@@@@@@@@@
      MAudioFileManager{



      }
    */

    MPlot2DStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: box.left
        Behavior on width {NumberAnimation { duration: 1000 }}
        height: rootAna.height - 30
        plotProp:mngCon.plotSetting

        //@@@@@@@@@@    Events          @@@@@@@@@@

        //onSaveMeChanged: mngTra.saveThis(saveMe)
        //onTracksChanged:console.log("draw these",tracks)
        onCurObjChanged: {
            if(completed)
            {
                mngData.changeObject(curObj)
                plot.tracks=mngData.getData("Track")
                plot.markers=mngData.getData("Marker")
                plot.frames=mngData.getData("Definer")
            }
        }
    }

    //@@@@@@@@@@--- Dialogs and Selectors@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@------

    MDialog{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:plotDial
        anchors.centerIn: plot
        width: plot.width*0.8
        height: plot.height*0.8
        dataList: mngData.availableData
        plotList: mngCon.plotList
        trackList: mngData.availableTracks

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onReady:{mngData.infoList=news;populate()}
    }

    FileDialog{
        //@@@@@@@@@@    Definitions     @@@@@@@@@@
        property string owner:"none"

        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: fileDial

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onAccepted:
        {
            switch(owner)
            {
            case "mngData":
                mngData.loadFile(fileUrl)
                plot.setAbsXmin(mngData.getStartTime())
                plot.setAbsXmax(mngData.getEndTime())

                break;

            case "mngCon":
                mngCon.fileName=fileUrl
                mngCon.read()
                break;
            default:console.log("Should Not Be Here!",owner);break;

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
            case "NewObj": mngData.addCustomObj(selector.preSelectedElements,curText,type);plotDial.visible=true;break;
            default:break;
            }
            visible=false

        }
    }

    MListSelector{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:selector
        height: 300
        width:300
        anchors.centerIn: parent
        owner:""

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onOwnerChanged:
        {
            //a seconda di chi lo controlla decido come riempirlo
            switch(owner)
            {
            case "audioPlayer": elements=audioPlayer.fileList();    break;
            case "dopAna":      elements=mngData.availableTracks;   break;
            case "family":      elements=mngData.getLinks("Families");
                enableNew=false;
                break;
            case "name":        enableNew=true;break;
            default:break;
            }
        }
        onSelected: {
            //a seconda di chi lo sta usando decido cosa fare
            switch(owner)
            {
            //case "dopAna":      dopAna.currentSignal=mngData.getSignal(curText);      owner="";break;
            case "family":
                title="Choose or create an element"
                elements=mngData.getLinks("Names",selectedElements,type);
                //mngData.addCustomObj(choices,type);
                //plotDial.visible=true;
                owner="name";break;
            default:break;
            }

        }
        onNuovo: {
            switch(owner)
            {
            case "name":
                editName.owner="NewObj"
                editName.type=type
                editName.setDefText("New Name")
                editName.visible=true
                owner="";
                break;
            }
        }

    }

    //Models

    MarkerModel{id:marMod}
    FrameModel{id:fraMod}
    TrackModel{id:traMod}
    ParameterModel{id:parMod}

    //Managers

    ParameterManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngPar
        roles:parMod.strList
        lastParMod:box.valueNews

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onLastParModChanged: {
            //dopAna.analyze(mngPar.curAnalysis,mngPar.curPar)
        }
    }

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onFileNameChanged: if(fileName!==""){plot.completed=true;read()}
    }

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------

    ParameterBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:box
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width:0
        height: root.height - 30
        Behavior on width {NumberAnimation { duration: 1000 }}

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onUpdate: {
           // dopAna.analyze(mngPar.curAnalysis,mngPar.curPar)
            plotDial.visible=true
        }


    }
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-
    MMenuBar {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: mainMenu
        anchors.top: parent.top
        color: "transparent"
        height: root.height
        width: root.width
        opacity: rootAna.opacity
        theme: "red"
        items: [
            ["File","Open Audio","Open Exam","Save","Back"],
            ["Add","Data to Plot","New Marker","New Definer"],
            ["Select Signal","To Analyze","To Play"],
            ["Analisys","Spectrum","Time Warping","Energy","Time Spectrum"],
            ["Plot","Load Settings"]
        ]

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onSelected: {
           // dopAna.handleMenu(itemClicked)
            if (itemClicked == "Open Audio") {
                fileDial.owner="mngData"
                fileDial.folder="../../ProgettoDoppler/Exams/Audio di Prova"
                fileDial.setNameFilters("*.wav")
                fileDial.open()
            }
            if (itemClicked == "Open Exam") {
                fileDial.owner="mngData"
                fileDial.folder="../../ProgettoDoppler/Exams"
                fileDial.setNameFilters("*.pic")
                fileDial.open()
            }
            if (itemClicked == "Back") {
                rootAna.back()
            }
            if (itemClicked == "Save") {
                mngData.saveChanges()
            }
            //Add
            if (itemClicked == "Data to Plot")
                plotDial.visible=true
            if (itemClicked == "New Marker")
            {
                selector.type="Marker"
                selector.owner="family"
                selector.title="Choose a family"
                selector.multipleChoice=true
            }
            if (itemClicked == "New Definer")
            {
                selector.type="Definer"
                selector.owner="family"
                selector.title="Choose a family"
                selector.multipleChoice=true
            }

            //if (itemClicked == "Cursors")
            //mngMar.addMarker(["$Marker","doppler","number",Func.newId(),"&Marker"])

            if (itemClicked == "Analysis")
            {
                plotDial.visible=true
            }


            //Select
            if (itemClicked == "To Play")
            {
                selector.owner="audioPlayer"
            }
            if (itemClicked == "To Analyze")
            {
                selector.owner="dopAna"
            }
            //Analysis

            var str=items[3];
            if (str.indexOf(itemClicked)!==-1)
            {
                mngPar.curAnalysis=itemClicked

                box.ready=false
                box.currentAna=itemClicked
                box.items=mngPar.items
                box.width=parent.width/2
                box.popola()
                box.ready=true
            }

            //Settings
            if (itemClicked == "Load Settings")
            {
                fileDial.owner="mngCon"
                fileDial.folder="../../ProgettoDoppler/Doppler/Config"
                fileDial.setNameFilters("*.xml")
                fileDial.open()
            }



        }
    }


}
