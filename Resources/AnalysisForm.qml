import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0
import QtQml 2.0

import MComponents 1.0
import "qrc:/Components"
MForm{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    //cambiamenti effettuati nel plot ma che non modificano i suoi limiti (definitori, marker)
    //in caso di plot zoomato il cambiamento dei limiti genera un reset dello zoom
    property bool change: false

    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootAna
    anchors.fill: parent.fill

    color:"lightgray"

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function populate()
    {
        plot.tracks=mngData.getData("Track")
        plot.markers=mngData.getData("Marker")
        plot.frames=mngData.getData("Definer")
        if (!change)
            plot.limits=mngData.getPlotLimits();
        else
            change = false
       lbNamePat.testo = mngData.protocollo + " - " +mngData.patientInfo;
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

    function setActionsInfo(info)
    {
        console.log(info)
        gridActions.items=info
    }

    function setCommandsInfo(info)
    {
        console.log(info)
        gridComand.items=info
    }

    function setCommandsInfoBottom(info)
    {
        console.log(info)
        gridComandBottom.items=info
        gridComandBottom.count = 0
        for(var i=0;i<info.length;i++)
            if(info[i]==="$GridElement")
                gridComandBottom.count++
    }

    function loadConfigurationFile(configurationFile)
    {
        mngCon.fileName = configurationFile
        plot.completed=true;
        mngCon.read()
    }

    function activeAnalisysButton()
    {
        if (!PicoFlow)
        {
            mngData.saveImg(sourceImg,"GR000")
            //rendo abilitato il pulsante per passare ai risultati senza dover rifare l'analisi
            //abilito anche il pulsante export
            for (var i=0; i<gridActions.items.length; i++)
            {
                if (gridActions.items[i]===MDataManager.RISULTATI)
                {
                    gridActions.items[i+4] = "true"
                    break;
                }
            }
            gridActions.populate()
            for (i=0; i<gridComand.items.length; i++)
            {
                if (gridComand.items[i]===MDataManager.EXPORT)
                {
                    gridComand.items[i+4] = "true"
                    break;
                }
            }
            gridComand.populate()
        }
    }

    function disablebuttons(__disable){
        for (var i=0; i<gridComand.items.length; i++)
        {
            if (gridComand.items[i]===MDataManager.ANALISI)
            {
                gridComand.items[i+4] = !__disable
                break;
            }
        }
        for (i=0; i<gridComandBottom.items.length; i++)
        {
            if (gridComandBottom.items[i]===MDataManager.CHIUDIREV)
                gridComandBottom.items[i+4] = !__disable
        }
        gridComand.populate()
        gridComandBottom.populate()

        notClose.visible = __disable
    }

    //@@@@@@@@@@    Objects     @@@@@@@@@@
    MPlot2DStack{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: gridComand.left
        anchors.bottom: PicoFlow ? parent.bottom : lbNamePat.top
        plotProp:mngCon.plotSetting

        property real opMarkerKey: -1
        property real definerKey: -1
        property string anMarkerKey: ""
        //@@@@@@@@@@    Events          @@@@@@@@@@

        onCurObjChanged: {
            if(completed){
                change = true;
                if (!PicoFlow && curObj.length === 1) {//sto cancellando un markers
                    dialogDelete.obj = curObj
                    dialogDelete.owner=this
                }
                else //sto modificando
                    mngData.changeObject(curObj)
            }
        }

        onOpMarkerPosChanged:{
            change = true; //cambiamenti nel plot
            mngData.addOpMarker(opMarkerKey,opMarkerPos)
        }

        onDefinerPosChanged:{
            change = true; //cambiamenti nel plot
            mngData.addDefiner(definerKey,definerPos)
        }

        onAnMarkerPosChanged: {
            change = true; //cambiamenti nel plot
            mngData.addAnMarker(anMarkerKey,anMarkerPos,anMarkChNames)
        }

        onToCopyPlotChanged: {
            if (plot.toCopyPlot[0]){
                activeMouse = true
                startDelTim.start()
            }
        }
        property bool activeMouse: false
        //necessario per capire la posizione del mouse per far apparire clicright
        //per il copy del plot
        MouseArea {
            id: plotMouse
            anchors.fill: parent
            enabled: !PicoFlow
            acceptedButtons: Qt.NoButton
            propagateComposedEvents: true
            hoverEnabled: plot.activeMouse
        }

        Rectangle
        {
            id:clicright
            border.width: 1
            border.color: "blue"
            height:testoDel.height
            width:testoDel.width
            radius: 7
            visible: false
            Text{
                id: testoDel
                font.family:
                {
                    if (PicoFlow)
                        if (layout !== undefined)
                            testoDel.font.family = layout.value("FFamily")
                        else
                            testoDel.font.family ="Luxi Serif"
                    else if (layout !== undefined)
                        testoDel.font.family = layout.value("FFamilyW")
                    else
                        testoDel.font.family = "Calibri"
                }
                font.pixelSize: 15
                anchors.centerIn: parent
                height:font.pixelSize*2
                width:font.pixelSize*6
                color: "blue"
                text: qsTr("copy")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea{
              anchors.fill:parent
              onClicked: {
                  delTim.stop()
                  plot.activeMouse = false
                  clicright.visible = false
                  plot.setToCopy = true
                  mngData.copyImg(sourceImg)
                  endCopy.start()
              }
            }
        }

        Timer{
            id:startDelTim
            interval: 100
            onTriggered:{
                clicright.y = plotMouse.mouseY
                clicright.x = plotMouse.mouseX-plot.toCopyPlot[0]
                DataEngine.putItemOnTop(clicright)
                clicright.visible = true
                delTim.start()
            }
        }
        Timer{
            id:delTim
            interval: 1500
            onTriggered:{
                plot.activeMouse = false
                clicright.visible = false
            }
        }
        Timer{
            id:endCopy
            interval: 5
            onTriggered:
                plot.setToCopy = false
        }
    }



    Rectangle
    {
        id: lbNamePat
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: gridComandBottom.left
        border.width: 1
        border.color: "black"
        visible: PicoFlow? false : true
        height: parent.height/25
        color: "lightgray"
        property string testo: "datiPaziente"
        MLabel{
            height:  parent.height
            labelSize: 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: parent.testo
        }

//        MouseArea {
//            id: plotMouse
//            anchors.fill: parent
//            enabled: !PicoFlow
//            acceptedButtons: Qt.NoButton
//            propagateComposedEvents: true
//            hoverEnabled: plot.activeMouse
//        }
    }

    //Models

    MarkerModel{id:marMod}
    FrameModel{id:fraMod}
    TrackModel{id:traMod}

    //Managers

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
    }


    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridMarker
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width:PicoFlow ? 0 : 60
        owner:"Marker"
        itemsInRow:1
        delegate: MMarkerButton{
            onClick: {
                plot.opMarkerKey = value
                plot.newOpMarker = true
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY
                    toolTip.text = testo
                    toolTip.grid = gridMarker
                    toolTip.visible = true
                }
            }
        }
        visible: PicoFlow ? false : true
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridDefiners
        anchors.right: gridMarker.left
        anchors.top: parent.top
        width:PicoFlow ? 0 : 60
        owner:"Definers"
        itemsInRow:1
        delegate: MMarkerButton {
            onClick: {
                plot.newDefiner = true
                plot.definerKey = value
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY
                    toolTip.text = testo
                    toolTip.grid = gridDefiners
                    toolTip.visible = true
                }
            }
        }
        visible: PicoFlow ? false : true
    }

    Rectangle
    {
        id: rectBreak
        height: 2
        width: 60
        anchors.right: gridMarker.left
        y: PicoFlow ? parent.height : parent.height/2
        color: layout.value("textTable")//"black"
        visible: PicoFlow ? false : true
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridActions
        anchors.right: gridMarker.left
        anchors.top: rectBreak.bottom
        anchors.bottom: parent.bottom
        width:PicoFlow ? 0 : 60

        owner:"Actions"
        itemsInRow:1
        delegate: MMarkerButton {

            onClick: {
                if (value === MDataManager.DELETEALL.toString())
                    mngData.deleteAnMArkers()
                if (value === "f1" || value === "f2" || value === "f3")
                {
                    plot.anMarkerKey = value
                    plot.newAnMarker = true
                }
                if (value === MDataManager.RISULTATI.toString()) {
                    forAna.visible = false
                    forRes.loadPageAnalysis()
                    forRes.visible = true
                }
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY + rectBreak.y
                    toolTip.text = testo
                    toolTip.grid = gridActions
                    toolTip.visible = true
                }
            }
        }
        visible: PicoFlow ? false : true

    }

    ShaderEffectSource {
        id: sourceImg
        width: plot.width
        height: plot.height
        sourceItem: plot
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridComand
        anchors.right: gridDefiners.left
        anchors.top: parent.top
        width:60
        owner:"Commands"
        itemsInRow:1

        delegate: MMarkerButton{
            onClick: {
                if (value === MDataManager.ANALISI.toString()) {                    
                    mngData.analysis();                    
                }

                if (value === MDataManager.CHIUDIREV.toString()) {
                    mngData.exitFromReview()
                }

                if (value === MDataManager.ZOOMIN.toString())
                    //zoom in
                    plot.zoomFromButton("zoomIn")

                if (value === MDataManager.ZOOMOUT.toString())
                    //zoom out
                    plot.zoomFromButton("zoomOut")

                if (value === MDataManager.ZOOMNONE.toString())
                    //zoom none
                    plot.zoomFromButton("zoomReset")

                if (value === MDataManager.EXPORT.toString())
                    //export
                    mngData.openExportTool()
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY
                    toolTip.text = testo
                    toolTip.grid = gridComand
                    toolTip.visible = true
                }
            }
        }
        visible:true
    }

    Rectangle
    {
        id: rectBreakCommands
        property real buttonHeight: 0
        height: 2
        width: 60
        anchors.right: gridDefiners.left
        y: PicoFlow ? parent.height : parent.height - buttonHeight*gridComandBottom.count
        color: layout.value("textTable")//"black"
        visible: false
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridComandBottom
        property int count: 0
        anchors.right: gridDefiners.left
        anchors.top: rectBreakCommands.top
        anchors.bottom: parent.bottom
        width:60
        owner:"CommandsBottom"
        itemsInRow:1

        delegate: MMarkerButton{
            id:button
            onHeightChanged: {
                   if (rectBreakCommands.buttonHeight === 0)
                       rectBreakCommands.buttonHeight = button.height*1.4
            }
            onClick: {
                if (value === MDataManager.CHIUDIREV.toString()) {
                    mngData.exitFromReview()
                }
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY + rectBreakCommands.y
                    toolTip.text = testo
                    toolTip.grid = gridComandBottom
                    toolTip.visible = true
                }
            }
        }
        visible:true

    }

    Rectangle{
        property string text: ""
        property var grid:gridComand
        id: toolTip
        width: toolTipText.width *1.1
        height: toolTipText.height *1.1
        anchors.horizontalCenter: grid.horizontalCenter
        y: 0
        color: "whitesmoke"
        border.color: "blue"
        border.width: 1
        radius: 5
        visible:false
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:toolTipText
            color:"blue"
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        toolTipText.font.family = layout.value("FFamily")
                    else
                        toolTipText.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    toolTipText.font.family = layout.value("FFamilyW")
                else
                    toolTipText.font.family = "Calibri"
            }
            font.bold: false
            font.pixelSize:screenH * 0.015
            text:toolTip.text
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: notClose
        border.width: 1
        border.color:layout.value("textTable")// "black"
        height:gridComandBottom.height/gridComandBottom.count*0.75
        width:lbNotClose.width
        color: "transparent"
        radius: 5
        visible: false
        anchors.right: parent.right
        anchors.left: plot.right
        anchors.bottom: gridComandBottom.bottom
        anchors.bottomMargin: 3

        MLabel {
            id: lbNotClose
            labelSize: layout.value("F4")
            anchors.fill: parent
            anchors.margins: 5
            color: layout.value("textTable")//"black"
            text: qsTr("<p>File report open.</p>Close the file to continue.")
        }
    }

    MDialogYesNo {
        id: dialogDelete
        property var owner:dialogDelete
        property var obj
        onOwnerChanged: {
            switch(owner){
            case dialogDelete:break;
            case plot:
                var name = mngData.getNameOfObj(obj)
                var testo = qsTr("Do you really want to delete the ")
                message=qsTr(testo + name + "?")
                break

            default:console.error("Owner sconosciuto",owner)
            }
            if(owner!=dialogDelete)
                dialogDelete.open()
        }
        onAccepted:  {
            switch(owner){
            case dialogDelete:break;
            case plot:
                mngData.changeObject(obj)
                break
            default:console.error("Owner sconosciuto",owner)
            }
        }
        onRejected: {
            switch(owner){
            case dialogDelete:break;
            case plot:break
            default:console.error("Owner sconosciuto",owner)
            }
        }

    }

}
