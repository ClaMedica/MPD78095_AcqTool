import QtQuick 2.2
import MPlotModule 1.0
import "../Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle{

    id:rootCursor

    property var modello:[]
    property int min:  0
    property int max:  100
    property int lineLength: 100
    property real vMin: 0
    property real vMax: 1
    property bool moving: false
    property real valPer: (x+arrow.width/2-min)/(max-min)
    property bool selected:false
    property bool enablePopUp: true
    property real value:0
    property real realX:x+arrow.width/2

    property bool deselectOnRelease:false
    property var fullCode:[modM.family,modM.name,modM.code]


    signal modifyMe
    signal deleteMe

    color:"transparent"


    MarkerModel{
        id:modM

    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        //console.log("modello marker",rootCursor.code,"=",modello)
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modM.setProperty(modello[i],modello[i+1])
        }
        updateX(modM.val);
        arrow.update();
        //console.log("I'm cursor n°",code," at val:",modM.val," valPer: ",valPer," vis ",visible)
    }

    Component.onCompleted:
    {
        focus=false
        selected=false
        animate()
    }

    height:lineLength+arrow.height+label.height
    width:arrow.height    
    onXChanged:value=(vMax-vMin)*((rootCursor.x+arrow.width/2)-min)/(max-min)+vMin
    onVMaxChanged: updateX(value)
    onVMinChanged: updateX(value)


    function updateX(v)
    {
        valPer=(v-vMin)/(vMax-vMin)
        x=(max-min)*valPer-arrow.width/2+min
        if(modM.visible)
            visible=(valPer<0 || valPer>1)?false:true
        //console.log("mi aggiorno",visible,valPer,x,value)
    }


    onMaxChanged:x=valPer*(max-min)+min-arrow.width/2
    onMinChanged:x=valPer*(max-min)+min-arrow.width/2
    function animate()
    {
        if(selected)
        {
            animOpa.start()
            label.opacity=1
        }
        else
        {
            animOpa.stop()
            arrow.opacity=1
            label.opacity=0
        }
    }

    function showPopUp(){popTim.interval=1000;popTim.start();}
    function hidePopUp(){popTim.interval=200;popTim.start();}
    function timeToText()
    {
        var tempo=rootCursor.value.toFixed(2)
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
        anchors.bottom:rootCursor.bottom
        anchors.horizontalCenter: arrow.horizontalCenter
        color:modM.color
        opacity: 1
    }



    DCursor{
        id:arrow
        rotation: 270
        colore:modM.color
        anchors.bottom:linea.top
        anchors.left: rootCursor.left
        height:20
        width:20
        clip:true
    }

    MouseArea{
        id:popMouse
        anchors.fill:arrow
        onEntered: showPopUp()
        onExited: hidePopUp()
        hoverEnabled: true
    }

    MouseArea{
        id:mouseCursor
        anchors.fill:arrow
        enabled: !modM.lock
        drag.target: rootCursor
        drag.axis: "XAxis"
        drag.filterChildren: true
        drag.minimumX: min-arrow.width/2
        drag.maximumX: max-arrow.width/2
        onPressed: {rootCursor.focus=true;moving=true;selected=true}
        onReleased: {
            moving=false;
            valPer=(rootCursor.x+arrow.width/2-min)/(max-min)
            rootCursor.modifyMe()
            if(deselectOnRelease)
                selected=false
        }
        preventStealing: true
    }

    function timeW(str)
    {
        return str.length*15
    }

    Rectangle{
        id:popRect
        anchors.top:linea.top
        anchors.left:linea.right
        border.width: 3
        border.color: modM.lock?"red":"black"
        height:2*popRect.border.width+popText.height
        width:popText.width+2*popRect.border.width
        opacity:0
        color:Qt.rgba(255,255,255,0.8)
        Behavior on width {NumberAnimation { duration: 300 }}
        Behavior on height {NumberAnimation { duration: 300 }}
        Behavior on opacity {NumberAnimation { duration: 300 }}
        clip:true


        Text{
            id:popText
            color:"black"
            font.family: "Courier 10 Pitch"
            font.bold: true
            font.pixelSize: 20
            anchors.centerIn: parent
            height:contentHeight*(Math.round(text.length/10))
            width:timeW(timeToText())
            text:timeToText()+"\n"+modM.popUp
            wrapMode: Text.WrapAnywhere
            horizontalAlignment : Text.AlignHCenter
        }
    }

    Timer{
        id:popTim
        interval: 1000
        onTriggered:
        {
            if(enablePopUp && moving==false)
            {
                if(interval==200)
                {
                    popText.text=""
                    popRect.opacity=0;
                }
                else
                {
                    popText.text=timeToText()+"\n"+modM.popUp
                    popRect.opacity=1;
                }
            }
        }
    }

    Text{
        id:label
        color:"black"
        font.family: "Courier 10 Pitch"
        font.bold: true
        anchors.bottom:arrow.top
        anchors.horizontalCenter: arrow.horizontalCenter
        text:rootCursor.value.toFixed(2)
    }

    Keys.onPressed: {
        moving=true;
        switch(event.key)
        {
        case Qt.Key_Left:x=x-1;break;
        case Qt.Key_Right:x=x+1;break;
        case Qt.Key_Up:y=y-1;break;
        case Qt.Key_Down:y=y+1;break;
        case Qt.Key_Escape:rootCursor.focus=false;selected=false;break;
        case Qt.Key_Delete:rootCursor.visible=false;rootCursor.deleteMe();break;
        }
        moving=false;
    }

    onSelectedChanged: animate()
    onFocusChanged: selected=focus

    SequentialAnimation{
        id:animOpa
        loops: Animation.Infinite
        NumberAnimation {
            target: arrow
            property: "opacity"
            to:0
            duration: 500
            easing.type: Easing.InOutSine
        }
        NumberAnimation {
            target: arrow
            property: "opacity"
            to:1
            duration: 500
            easing.type: Easing.InOutSine
        }
    }




}

