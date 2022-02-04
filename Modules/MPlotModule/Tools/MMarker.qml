import QtQuick 2.2
import MPlotModule 1.0
import "../Models"

Rectangle{

    id:rootMarker

    property var modello:[]
    property int min:  0
    property int max:  100
    property int lineLength: 100
    property real vMin: 0 //valore vero convertito in udm min
    property real vMax: 1 //valore vero convertito in udm max
    property real yMin: 0 //valore vero convertito in pixel min
    property real yMax: 1 //valore vero convertito in pixel max
    property bool moving: false
    property real valPer: (x+icon.width/2-min)/(max-min)
    property bool selected:false
    property bool enablePopUp: true
    property real value:0 //valore in x a cui sta il marker in udm
    property real realX:x+icon.width/2
    property real limiteDefMin: modM.limDefMin
    property real limiteDefMax: modM.limDefMax
    property real defCode: modM.defCode
    property bool deselectOnRelease:false
    property real whoAmI:modM.whoAmI


    signal modifyMe
    signal deleteMe


    color:"transparent"
    visible:modM.visible
    height:lineLength+icon.height+label.height
    width:icon.height
    onXChanged: value=(vMax-vMin)*((rootMarker.x+icon.width/2)-min)/(max-min)+vMin

    onVMaxChanged: {updateX(value);updateY(value)}
    onVMinChanged: {updateX(value);updateY(value)}

    MarkerModel{id:modM
    //onImgChanged: if(img!=="")icon.source=img+".bmp"
    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        //console.log("modello marker"),rootMarker.code,"=",modello)
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modM.setProperty(modello[i],modello[i+1])
        }
        updateX(modM.val);
        updateY(modM.val);
        //e' necessario settare la propietà qui
        rootMarker.visible = modM.visible;

    }

    Component.onCompleted:
    {
        selected=false
    }

    function updateYZoom()
    {
        updateY(value)
        rootMarker.modifyMe()
    }

    function updateY(xval)
    {//aggiorna l'altezza del marker in funzione della traccia a cui è associato
        if (modM.type === "Analytical")
        {
            var pos = xval*modM.nas
            pos = pos.toFixed(0)
            var valPer=(modM.valuesY[pos])/(yMax - yMin)
            linea.height = (lineLength*valPer)
        }
    }


    function updateX(v)
    {
        valPer=(v-vMin)/(vMax-vMin)
        x=(max-min)*valPer-icon.width/2+min
        if(modM.visible)
            visible=(valPer<0 || valPer>1)?false:true
        //console.log("mi aggiorno",visible,valPer,x,value)
    }

    onLimiteDefMaxChanged: {
        if (value >= limiteDefMax){
            valPer=(rootMarker.x+icon.width/2-min)/(max-min)
            updateX(limiteDefMax)
            rootMarker.modifyMe()
        }
    }
    onLimiteDefMinChanged: {
        if (value <= limiteDefMin){
            valPer=(rootMarker.x+icon.width/2-min)/(max-min)
            updateX(limiteDefMin)
            rootMarker.modifyMe()
        }
    }

    onMaxChanged: x=valPer*(max-min)+min-icon.width/2

    onMinChanged: x=valPer*(max-min)+min-icon.width/2

    function markerPopUp(){return modM.code+"\n"+timeToText()}
    function showPopUp(){popTim.interval=1000;popTim.start();}
    function hidePopUp(){popTim.interval=200;popTim.start();}
    function timeToText()
    {
        var tempo=rootMarker.value.toFixed(2)
        var ore=Math.floor(tempo/3600)
        tempo-=ore*3600
        var min=Math.floor(tempo/60)
        tempo-=min*60
        var sec=Math.floor(tempo)
        return ore+":"+min+":"+sec
    }

    Rectangle{
        id:linea
        height:lineLength
        width:1
        anchors.bottom:rootMarker.bottom
        anchors.horizontalCenter: icon.horizontalCenter
        color:modM.color
        opacity: 1
    }



    Image{
        id:icon
        anchors.bottom:linea.top
        anchors.left: rootMarker.left
        height:16
        width:16
        clip:true
        visible:false
    }

    Text{
        id:label
        color:"black"
        font.family:
        {
            if (PicoFlow)
                if (layout !== undefined)
                    label.font.family = layout.value("FFamily")
                else
                    label.font.family ="Luxi Serif"
            else if (layout !== undefined)
                label.font.family = layout.value("FFamilyW")
            else
                label.font.family = "Calibri"
        }
        font.bold: true
        font.pixelSize:PicoFlow? screenH * 0.03 : screenH * 0.02
        anchors.bottom: linea.top
        anchors.horizontalCenter: icon.horizontalCenter
        text:modM.code
        opacity:1
    }

    MouseArea{
        id:popMouse
        anchors.fill:icon
        onEntered: showPopUp()
        onExited: hidePopUp()
        hoverEnabled: true
    }

    MouseArea{
        id:mouseCursor
        anchors.fill:icon
        enabled: PicoFlow?false:!modM.lock
        drag.target: rootMarker
        drag.axis: "XAxis"
        drag.filterChildren: true
        drag.minimumX: modM.type === "Analytical" ? (max-min)*((limiteDefMin - vMin)/(vMax-vMin))-icon.width/2+min : min-icon.width/2
        drag.maximumX: modM.type === "Analytical" ? (max-min)*((limiteDefMax - vMin)/(vMax-vMin))-icon.width/2+min : max-icon.width/2
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        onEntered: {
            rootMarker.focus=true
            showPopUp()
        }
        onExited: {
            rootMarker.focus=false
            hidePopUp()
        }
        onPressed: {
            hidePopUp()
            if (mouse.button === Qt.RightButton) {
                clicright.visible = true
                delTim.start()
            }
            else
            {
                moving=true
                selected=true
            }
        }
        onReleased: {
            moving=false;
            rootMarker.modifyMe()
            if(deselectOnRelease)
                selected=false
        }
        onPositionChanged:{
           if (moving){
               valPer=(rootMarker.x+icon.width/2-min)/(max-min)
               updateY(value)
            }
        }
        preventStealing: true
    }


    Rectangle{
        id:popRect
        anchors.top:linea.top
        anchors.left:linea.right
        border.width: 2
        border.color: modM.lock?"red":"black"
        height:popText.height
        width:popText.width+2*popRect.border.width
        opacity:0
        color:"transparent"//Qt.rgba(255,255,255,0.8)
        Behavior on width {NumberAnimation { duration: 300 }}
        Behavior on height {NumberAnimation { duration: 300 }}
        Behavior on opacity {NumberAnimation { duration: 300 }}
        clip:true


        Text{
            id:popText
            color:"black"
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        popText.font.family = layout.value("FFamily")
                    else
                        popText.font.family ="Luxi Serif"
                else if (layout !== undefined)
                    popText.font.family = layout.value("FFamilyW")
                else
                    popText.font.family = "Calibri"
            }
            font.bold: true
            font.pixelSize: 15
            anchors.centerIn: parent
            height:font.pixelSize*3
            width:font.pixelSize*6
            text:markerPopUp()
            wrapMode: Text.WrapAnywhere
            horizontalAlignment : Text.AlignHCenter
        }
    }

    Timer{
        id:popTim
        interval: 1000
        onTriggered:
        {
            if(enablePopUp)
            {
                if(interval==200)
                {
                    popText.text=""
                    popRect.opacity=0;
                }
                if (interval !== 200 && moving===false)
                {
                    popText.text=markerPopUp()
                    popRect.opacity=1;
                }
            }
        }
    }

    Keys.onPressed: {
        switch(event.key)
        {
        case Qt.Key_Delete:rootMarker.deleteMe();break;
        }
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
        anchors.top:linea.top
        anchors.left:linea.right
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
            text: qsTr("delete")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea{
          anchors.fill:parent
          //delete
          onClicked: {
             clicright.visible = false
             rootMarker.deleteMe()
          }
        }
    }

    Timer{
        id:delTim
        interval: 2000
        onTriggered:
            clicright.visible = false
    }

}

