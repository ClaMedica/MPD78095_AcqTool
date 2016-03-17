import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0
import QtQml 2.0
//import Resources 1.0
import "qrc:/Components"
MForm{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property alias configurationFile:mngCon.fileName
    property string displayInformations:""
    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootAna
    anchors.fill: parent.fill
    //background:"qrc:/Images/Analysis"
    visible:whoIsVisilbe===name?true:false

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function populate()
    {
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

    function setMarkersInfo(info)
    {
        console.log(info)
        gridMarker.items=info
    }
    function setDefinersInfo(info)
    {
        console.log(info)
        gridDefiners.items=info
    }

    function setCommandsInfo(info)
    {
        console.log(info)
        gridComand.items=info
    }


    //@@@@@@@@@@    Objects     @@@@@@@@@@


    MPlot2DStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: gridComand.left
       // anchors.right: box.left
      //  Behavior on width {NumberAnimation { duration: 1000 }}
        height: rootAna.height - 15
        plotProp:mngCon.plotSetting

        //@@@@@@@@@@    Events          @@@@@@@@@@

        //onSaveMeChanged: mngTra.saveThis(saveMe)
        //onTracksChanged:console.log("draw these",tracks)
        onCurObjChanged: {
            if(completed)
            {
                mngData.changeObject(curObj)
//                plot.tracks=mngData.getData("Track")
//                plot.markers=mngData.getData("Marker")
//                plot.frames=mngData.getData("Definer")
            }
        }
        onToZoomChanged:
        {
            var startTime = mngData.getStartTime()
            var endTime = mngData.getEndTime()
            var numSec = (endTime - startTime)/2

            var pageSize
            if (toZoom > oldToZoom){
                pageSize = scroolBar.pageSize - (1/numSec)
                if (pageSize < 0)
                    pageSize = 0
                scroolBar.pageSize = pageSize
            }
            if (toZoom < oldToZoom){
                    pageSize = scroolBar.pageSize + (1/numSec)
                    var LimitSx,LimitDx
                    LimitSx = (scroolBar.position * (scroolBar.width-2) + 1 - scroolBar.barW/2)
                    LimitDx = LimitSx + (scroolBar.pageSize * (scroolBar.width-2))
                    if (LimitSx <= 0){
                        scroolBar.position += (0-LimitSx)/(scroolBar.width-2)//0.0075
                        pageSize = scroolBar.pageSize + (1/numSec)/2
                    }

                    if (LimitDx >= scroolBar.width-2){
                        scroolBar.position -= (LimitDx - scroolBar.width-2)/(scroolBar.width-2)
                        pageSize = scroolBar.pageSize + (1/numSec)/2//0.015
                    }
                    if (pageSize > 1)
                        pageSize = 1
                    scroolBar.pageSize = pageSize
            }
            if (toZoom === 0 && oldToZoom === 0){
                scroolBar.pageSize = 1.0
                scroolBar.position = 0.5
            }
        }
    }

    MScroolBar{
        id: scroolBar
        clip:true
        anchors.top:plot.bottom
        anchors.bottom: parent.bottom
        anchors.right: gridComand.left
        anchors.left: parent.left
        onMoved: {
            var moved = scroolBar.step
            plot.movingZoom = moved
            //console.log("onmoved ",moved)
        }
    }

    //@@@@@@@@@@--- Dialogs and Selectors@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@------

//    MDialog{
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id:plotDial
//        anchors.centerIn: plot
//        width: plot.width*0.8
//        height: plot.height*0.8
//        dataList: mngData.availableData
//        plotList: mngCon.plotList
//        trackList: mngData.availableTracks

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onReady:{mngData.infoList=news;populate()}
//    }

//    FileDialog{
//        //@@@@@@@@@@    Definitions     @@@@@@@@@@
//        property string owner:"none"

//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id: fileDial

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onAccepted:
//        {
//            switch(owner)
//            {
//            case "mngData":
//                mngData.loadFile(fileUrl)
//                plot.setAbsXmin(mngData.getStartTime())
//                plot.setAbsXmax(mngData.getEndTime())
//                break;

//            case "mngCon":
//                mngCon.fileName=fileUrl
//                mngCon.read()
//                break;
//            default:console.log("Should Not Be Here!",owner);break;

//            }
//        }
//    }

//    MNewName{
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id:editName
//        height: 100
//        width:300
//        anchors.centerIn: parent
//        owner:""

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onSelected: {
//            //a seconda di chi lo sta usando decido cosa fare
//            switch(owner)
//            {
//            case "NewObj": mngData.addCustomObj(selector.preSelectedElements,curText,type);plotDial.visible=true;break;
//            default:break;
//            }
//            visible=false

//        }
//    }

//    MListSelector{
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id:selector
//        height: 300
//        width:300
//        anchors.centerIn: parent
//        owner:""

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onOwnerChanged:
//        {
//            //a seconda di chi lo controlla decido come riempirlo
//            switch(owner)
//            {
//            case "audioPlayer": elements=audioPlayer.fileList();    break;
//            case "dopAna":      elements=mngData.availableTracks;   break;
//            case "family":      elements=mngData.getLinks("Families");
//                enableNew=false;
//                break;
//            case "name":        enableNew=true;break;
//            default:break;
//            }
//        }
//        onSelected: {
//            //a seconda di chi lo sta usando decido cosa fare
//            switch(owner)
//            {
//            //case "dopAna":      dopAna.currentSignal=mngData.getSignal(curText);      owner="";break;
//            case "family":
//                title="Choose or create an element"
//                elements=mngData.getLinks("Names",selectedElements,type);
//                //mngData.addCustomObj(choices,type);
//                //plotDial.visible=true;
//                owner="name";break;
//            default:break;
//            }

//        }
//        onNuovo: {
//            switch(owner)
//            {
//            case "name":
//                editName.owner="NewObj"
//                editName.type=type
//                editName.setDefText("New Name")
//                editName.visible=true
//                owner="";
//                break;
//            }
//        }

//    }

    //Models

    MarkerModel{id:marMod}
    FrameModel{id:fraMod}
    TrackModel{id:traMod}
 //   ParameterModel{id:parMod}

    //Managers

//
    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onFileNameChanged: if(fileName!==""){plot.completed=true;read()}
    }

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------

//    ParameterBox{
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id:box
//        anchors.bottom: parent.bottom
//        anchors.right: parent.right
//        width:0
//        height: root.height - 30
//        Behavior on width {NumberAnimation { duration: 1000 }}

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onUpdate: {
//            plotDial.visible=true
//        }

//    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridMarker
        anchors.right: rootAna.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:0
        owner:"Marker"
        itemsInRow:1
        delegate: MMarkerButton{}
        visible:false


        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onClicked:
//        {
//            switch(owner)
//            {
//            case "Marker":
//                mngAcq.addMarker(value);
//                //console.log(mngAcq.acqMarkers)
//                plot.markers=mngAcq.acqMarkers;
//                break;
//            }
//        }
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridDefiners
        anchors.right: gridMarker.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:0
        owner:"Definers"
        itemsInRow:1
        delegate: MMarkerButton{}
        visible:true


        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onClicked:
//        {
//            switch(owner)
//            {
//            case "Marker":
//                mngAcq.addMarker(value);
//                //console.log(mngAcq.acqMarkers)
//                plot.markers=mngAcq.acqMarkers;
//                break;
//            }
//        }
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridComand
        anchors.right: gridDefiners.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:60
        owner:"Commands"
        itemsInRow:1
        delegate: MMarkerButton{}
        visible:true

        onTooltipActived:
        {
            if (testo === "")
                toolTip.visible = false
            else
            {
                toolTip.y = posY
                toolTip.text = testo
                toolTip.visible = true
            }
        }

        //@@@@@@@@@@    Events          @@@@@@@@@@

        onClicked:
        {
            switch(owner)
            {
            case "Commands":
                if (value === "111")
                    mngData.analysis();

                if (value === "112") {
                    mngData.saveChanges()
                    mngData.exitFromReview()
                }
                if (value === "113"){
                    //zoom in
                    plot.oldToZoom = plot.toZoom
                    plot.toZoom = plot.toZoom + 1
                }
                if (value === "114"){
                    //zoom out
                    plot.oldToZoom = plot.toZoom
                    plot.toZoom = plot.toZoom - 1
                }
                if (value === "115"){
                    //zoom none
                    plot.oldToZoom = 0
                    plot.toZoom = 0
                }
                break;
            }
        }
    }



    Rectangle{
        property string text: ""
        id: toolTip
        width: toolTipText.width + 4
        height: toolTipText.height + 4
        anchors.horizontalCenter: gridComand.horizontalCenter
        y: 0
        color: "whitesmoke"
        border.color: "blue"
        visible:false
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:toolTipText
            color:"blue"
            font.family: "Courier 8 Pitch"
            font.bold: false
            font.pixelSize:12
            text:toolTip.text
            horizontalAlignment : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }


    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-
//    MMenuBar {
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id: mainMenu
//        anchors.top: parent.top
//        color: "transparent"
//        height: root.height
//        width: root.width
//        opacity: rootAna.opacity
//        theme: "red"
//        items: [
//            //["File","Open Audio","Open Exam","Save","Back"],
//            //["Add","Data to Plot","New Marker","New Definer"],
//            //["Select Signal","To Analyze","To Play"],
//            //--["Analisys", "Analizza",qsTr("Exit")],
//            //["Actions","Send Toast","Start Service","Connect Client"],
//            //["Plot","Load Settings"]
//        ]

//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onSelected: {

////            if (itemClicked === "Send Toast"){
////                androidmanager.showToast("ciao dal qml");
////            }
////            if(itemClicked === "Start Service"){
////                androidmanager.launchService("medica.blue.START_SERVICE");
////            }
////            if(itemClicked === "Connect Client"){
////                androidmanager.connectClient();
////            }

////            if (itemClicked == "Open Audio") {
////                fileDial.owner="mngData"
////                fileDial.folder="../../ProgettoDoppler/Exams/Audio di Prova"
////                fileDial.setNameFilters("*.wav")
////                fileDial.open()
////            }
////            if (itemClicked == "Open Exam") {
////                fileDial.owner="mngData"
////                fileDial.folder="../../ProgettoDoppler/Exams"
////                fileDial.setNameFilters("*.pic")
////                fileDial.open()
////            }
////            if (itemClicked == "Back") {
////                rootAna.back()
////            }
////            if (itemClicked == "Save") {
////                mngData.saveChanges()
////            }
////            //Add
////            if (itemClicked == "Data to Plot")
////                plotDial.visible=true
////            if (itemClicked == "New Marker")
////            {
////                selector.type="Marker"
////                selector.owner="family"
////                selector.title="Choose a family"
////                selector.multipleChoice=true
////            }
////            if (itemClicked == "New Definer")
////            {
////                selector.type="Definer"
////                selector.owner="family"
////                selector.title="Choose a family"
////                selector.multipleChoice=true
////            }
////            //Select
////            if (itemClicked == "To Play")
////            {
////                selector.owner="audioPlayer"
////            }
////            if (itemClicked == "To Analyze")
////            {
////                selector.owner="dopAna"
////            }
//            //Analysis

//            if (itemClicked == "Analizza")
//            {
//                console.log("Sono dentro il click analysis")
//                mngData.analysis();
//            }

//            if (itemClicked == "Exit")
//            {
//                console.log("Sono dentro il click Exit")
//                mngData.exitFromReview();
//            }

////            var str=items[3];
////            if (str.indexOf(itemClicked)!==-1)
////            {
////                mngPar.curAnalysis=itemClicked

////                box.ready=false
////                box.currentAna=itemClicked
////                box.items=mngPar.items
////                box.width=parent.width/2
////                box.popola()
////                box.ready=true
////            }

//            //Settings
////            if (itemClicked == "Load Settings")
////            {
////                fileDial.owner="mngCon"
////                fileDial.folder="../../ProgettoDoppler/Doppler/Config"
////                fileDial.setNameFilters("*.xml")
////                fileDial.open()
////            }



//        }
//    }


}
