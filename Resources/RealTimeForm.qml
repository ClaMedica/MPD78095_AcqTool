import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import MComponents 1.0
import "qrc:/Components"
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import Managers 1.0

MForm{
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string fileName:""
    property alias configurationFile:mngCon.fileName
    property string lastAcqFileName:""
    property int saveMe:plot.savedData
    property int manAuto: 0 // 1:man_waiting_start 2:auto_waiting_start 3:recording
    property bool manAutoBlink: false
    signal back

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootRealTime    

    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function displayMessage(mex,time,rows)
    {
        pop.rows =rows
        pop.message = mex
        pop.display(1, 0)
        if(time !== -1)
            pop.display(0, time)
    }

    function setMarkersInfo(info)
    {
        console.log(info)
        grid.items = info
    }

    function setAcqInfo(info)
    {
        console.log(info)
        gridAcq.items = info
    }

    function setCommandsBottom(info)
    {
        console.log(info)
        gridComandBottom.items=info
        gridComandBottom.count = 0
        for(var i=0;i<info.length;i++)
            if(info[i]==="$GridElement")
                gridComandBottom.count++
    }

    function connected(){
        for (var i=0; i<gridAcq.items.length; i++)
        {
            if (gridAcq.items[i].toString() === MDataManager.STARTACQ.toString()) {
                timeStart.enable = true
                timeStart.start()
            }
        }
    }

    function endAcq()
    {
        plot.stopAll()
    }

    MPlot2DRealStack {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: plot
        clip:true
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: panManAuto.left
        height: root.height - alarmBox.height
        plotProp:mngCon.plotSetting
    }

    //Managers

    ParameterManager{id:mngPar}

    ConfigManager{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mngCon
        fileName: ""
        //@@@@@@@@@@    Events          @@@@@@@@@@
        onFileNameChanged: if(fileName!==""){plot.completed=true;read()}
    }

//    ParameterBox{
//        //@@@@@@@@@@    Properties      @@@@@@@@@@
//        id:box
//        anchors.bottom: parent.bottom
//        anchors.right: parent.right
//        width:0
//        height: parent.height - 30
//        Behavior on width {NumberAnimation { duration: 1000 }}
//        items:["$Parameter",
//            "label","Page Time",
//            "name","Page Time",
//            "values",[10,30,60,120,300],
//            "&Parameter"]
//        //@@@@@@@@@@    Events          @@@@@@@@@@
//        onValueNewsChanged:
//        {
//            if(valueNews.length===3)
//            {
//                if(valueNews[0]==="none" && valueNews[1]==="Page Time")
//                {
//                    plot.setPageTime(valueNews[2])
//                }
//            }
//        }

//    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:grid
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: PicoFlow ? 0 : 60
        owner:"Marker"
        itemsInRow:1
        delegate: MMarkerButton{
            onClick:{
                mngAcq.addMarker(value);
                plot.markers=mngAcq.acqMarkers;
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY
                    toolTip.text = testo
                    toolTip.gridOwner = grid
                    toolTip.visible = true
                }
            }
        }
        visible: PicoFlow ? false : true
    }


    Timer {
        id:timManAuto
        interval:500
        running: true
        repeat: true
        onTriggered: {
            manAuto = mngAcq.manAutoQml()
            switch (manAuto) {
            case 0: idMan.text = "---"; break;
            case 1: idMan.text = "Man"; break;
            case 2: idMan.text = "Auto"; break;
            case 3: break;
            }

            if(manAuto > 0)
                manAutoBlink ^= true
            else
                manAutoBlink = false
        }
    }

    Rectangle
    {
        id: panManAuto
        anchors.right: PicoFlow ? parent.right : grid.left
        anchors.top: parent.top
        anchors.bottom: PicoFlow ? parent.bottom : rectBreakCommands.top
        width: 60
        color: layout.value("BackgroundColor")

        MGridView{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:gridAcq
            property bool pauseToDisable: false
            property bool startToDisable: false
            anchors.fill: panManAuto
            owner:"Acq"
            itemsInRow:1
            delegate: MMarkerButton {
                onClick:{
                    // Pulsanti PicoFlow2rev3
                    if (value === MDataManager.DISCARD.toString()) {
                        //necessaria richiesta di conferma
                        dlgDiscard.testo = qsTr("Are you sure to discard the study?")
                        dlgDiscard.visible = true
                    }
                    if (value === MDataManager.CHIUDIACQ.toString()) {
                        //necessaria richiesta di conferma
                        dlgDiscard.testo = qsTr("Are you sure to exit from Stand-By?")
                        dlgDiscard.visible = true
                    }
                    // Pulsanti FLWHS
                    var vero = true
                    //start con zero
                    if (value === MDataManager.STARTACQ.toString()) {
                        //necessario partita l'acquisizione abilitare i pulsanti pausa e salvaRivedi                        
                        gridAcq.startToDisable = true
                        gridAcq.enable(vero,MDataManager.PAUSA)
                        gridComandBottom.enable(vero,MDataManager.SAVEREW)
                        //start aquisizione
                        mngAcq.startAcq()
                    }
                    //pausa
                    if (value === MDataManager.PAUSA.toString()) {
                        //necessario disabilitare il pulsante pausa
                        //necessario dare tempo al click di tornare alla forma normale,
                        //dopodichè riesce a prendere l'immagine nuova not enabled.
                        gridAcq.enable(vero,MDataManager.STARTACQ)
                        gridAcq.pauseToDisable = true
                        mngAcq.pauseAcq()
                    }
                }
                onExit: {
                    if (gridAcq.pauseToDisable){
                        //è qui che lanciamo il timer per disabilitare il pulsante pause
                        //dopo averlo cliccato
                        gridAcq.pauseToDisable = false
                        timeDisablePause.start()
                    }
                    if (gridAcq.startToDisable){
                        //è qui che lanciamo il timer per disabilitare il pulsante pause
                        //dopo averlo cliccato
                        gridAcq.startToDisable = false
                        timeStart.enable = false
                        timeStart.start()
                    }
                }

                onTooltipActive: {
                    if (testo === "")
                        toolTip.visible = false
                    else
                    {
                        toolTip.y = posY
                        toolTip.text = testo
                        toolTip.gridOwner = panManAuto
                        toolTip.visible = true
                    }
                }
            }
            visible:true
        }

        Timer {
            id: timeDisablePause
            interval: 500
            onTriggered: {
                var falso = false
                gridAcq.enable(falso,MDataManager.PAUSA)
            }
        }

        Timer {
            id: timeStart
            property bool enable: false
            interval: 500
            onTriggered: {
                gridAcq.enable(enable,MDataManager.STARTACQ)
            }
        }

        MLabel {
            id: idMan
            opacity: (((manAuto == 1) || (manAuto == 2)) & manAutoBlink) ? 1 : 0.5
            labelSize: PicoFlow ? layout.value("F4") : layout.value("F4")-2
            anchors.bottom: parent.verticalCenter
            anchors.left: parent.left
        }
        MLabel {
            id: idRun
            text: "Rec"
            color: "red"
            opacity: ((manAuto == 3) & manAutoBlink) ? 1 : 0.5
            labelSize: PicoFlow ? layout.value("F4") : layout.value("F4")-2
            anchors.top: idMan.bottom
            anchors.left: parent.left
        }
    }

    Rectangle
    {
        id: rectBreakCommands
        property real buttonHeight: 0
        height: 2
        width: 60
        anchors.right: grid.left
        y: PicoFlow ? parent.height : rootRealTime.height-buttonHeight*gridComandBottom.count
        color: layout.value("textTable")//"black"
        visible: false
    }

    MGridView{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:gridComandBottom
        property int count: 0
        anchors.right: grid.left
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
                // Pulsanti FLWHS
                //salva e rivedi
                if (value === MDataManager.SAVEREW.toString()) {
                    mngAcq.stopAcq()
                }
                // abbandona
                if (value === MDataManager.DISCARD.toString()) {
                    //necessaria richiesta di conferma
                    dlgDiscard.testo = qsTr("Are you sure to discard the study?")
                    dlgDiscard.visible = true
                }
            }
            onTooltipActive: {
                if (testo === "")
                    toolTip.visible = false
                else
                {
                    toolTip.y = posY + rectBreakCommands.y
                    toolTip.text = testo
                    toolTip.gridOwner = gridComandBottom
                    toolTip.visible = true
                }
            }
        }
        visible: PicoFlow ? false : true

    }

    MAlarmBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:alarmBox
        alarmHeight: 50
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right: PicoFlow ? grid.left : panManAuto.left
        alarms: mngAcq.alarms

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onResetAlarms: mngAcq.resetAlarms()
    }

    MPopUp {
        id: pop
        anchors.centerIn: parent
        width: 200
        height: 100
    }

    Rectangle{
        property string text: ""
        property var gridOwner
        id: toolTip
        width: toolTipText.width *1.1
        height: toolTipText.height *1.1
        anchors.horizontalCenter: (gridOwner !== undefined) ? gridOwner.horizontalCenter : parent.horizontalCenter
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
        id: dlgDiscard
        property  string testo: ""
        height:screenH*grafic.valueOf("Dialog","height")
        width:screenW*grafic.valueOf("Dialog","width")
        anchors.centerIn: parent
        visible: false
        color : layout.value("BackgroundColor")

        Keys.onReturnPressed: {
            dlgDiscard.visible = false
        }

        MLabel
         {
             anchors.top:parent.top
             anchors.left: parent.left
             anchors.right:parent.right
             height:parent.height*0.8
             labelSize: layout.value("F4")
             color: PicoFlow ? "white" : layout.value("textTable")
             horizontalAlignment: Text.AlignHCenter
             verticalAlignment: Text.AlignVCenter
             text: dlgDiscard.testo
             wrapMode: Text.WordWrap
         }

        MButton {
            id: btnYes
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: parent.height/5
            width: parent.width/3
            anchors.margins: 10
            labelSize: layout.value("F4")
            text: qsTr("Yes")
            onClicked: {
                dlgDiscard.visible = false
                mngAcq.endAcquisitionDiscard()
            }
        }

        MButton {
            id: btnNo
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            height: parent.height/5
            width: parent.width/3
            anchors.margins: 10
            labelSize: layout.value("F4")
            text: qsTr("No")
            onClicked: dlgDiscard.visible = false
        }
    }
}
