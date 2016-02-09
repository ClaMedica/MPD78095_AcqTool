import QtQuick 2.2
import MPlotModule 1.0
import "../Models"
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle{

    id:rootMarker

    property var modello:[]
    property int min:  0
    property int max:  100
    property int lineLength: 100
    property real vMin: 0
    property real vMax: 1
    property real yMin: 0
    property real yMax: 1
    property bool moving: false
    property real valPer: (x+icon.width/2-min)/(max-min)
    property bool selected:false
    property bool enablePopUp: true
    property real value:0
    property real realX:x+icon.width/2

    property var valuesY:[]

    property bool deselectOnRelease:false
    property real whoAmI:modM.whoAmI

    signal modifyMe
    signal deleteMe


    color:"transparent"


    MarkerModel{id:modM
    //onImgChanged: if(img!=="")icon.source=img+".bmp"
    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        //console.log("modello marker",rootMarker.code,"=",modello)
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modM.setProperty(modello[i],modello[i+1])
        }
        updateX(modM.val);

       // if (modM.type === "Analytical")
            updateY(modM.val);
    }

    Component.onCompleted:
    {
        focus=false
        selected=false
        animate()
    }


    height:lineLength+icon.height+label.height
    width:icon.height
    onXChanged:value=(vMax-vMin)*((rootMarker.x+icon.width/2)-min)/(max-min)+vMin
    onVMaxChanged: updateX(value)
    onVMinChanged: updateX(value)

    //onYMaxChanged: updateYZoom()
    //onYMinChanged: updateYZoom()


    function updateYZoom()
    {
        //valPer=(rootMarker.x+icon.width/2-min)/(max-min)
        updateY(value)
        rootMarker.modifyMe()

    }

    function updateY(yval)
    {
        if (modM.type === "Analytical")
        {
            var pos = yval*modM.nas
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


    onMaxChanged:x=valPer*(max-min)+min-icon.width/2
    onMinChanged:x=valPer*(max-min)+min-icon.width/2
    function animate()
    {
//        if(selected)
//        {
//            animOpa.start()
//            label.opacity=1
//        }
//        else
//        {
//            animOpa.stop()
//            icon.opacity=1
//            label.opacity=0
//        }

    }

    function markerPopUp(){return timeToText()+"\n["+modM.key+"] "+modM.descr+" "+modM.code}
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
        width:2
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
        font.family: "Courier 10 Pitch"
        font.bold: true
        anchors.bottom:linea.top
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
        enabled: !modM.lock
        drag.target: rootMarker
        drag.axis: "XAxis"
        drag.filterChildren: true
        drag.minimumX: min-icon.width/2
        drag.maximumX: max-icon.width/2
        hoverEnabled: true
        //onEntered: showPopUp()
        //onExited: hidePopUp()
        onPressed: {rootMarker.focus=true;moving=true;selected=true}
        onReleased: {
            moving=false;
            //if(deselectOnRelease)
                selected=false
        }
        onPositionChanged:{
            valPer=(rootMarker.x+icon.width/2-min)/(max-min)
            rootMarker.modifyMe()
            updateY(value)
        }
        preventStealing: true
    }




//    function timeW(str)
//    {
//        return str.length*15
//    }

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
            height:font.pixelSize*5
            width:font.pixelSize*7
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
            if(enablePopUp && moving==false)
            {
                if(interval==200)
                {
                    popText.text=""
                    popRect.opacity=0;
                }
                else
                {
                    popText.text=markerPopUp()
                    popRect.opacity=1;
                }
            }
        }
    }

    Keys.onPressed: {
        moving=true;
        switch(event.key)
        {
        case Qt.Key_Left:x=x-1;break;
        case Qt.Key_Right:x=x+1;break;
        case Qt.Key_Up:y=y-1;break;
        case Qt.Key_Down:y=y+1;break;
        case Qt.Key_Escape:rootMarker.focus=false;selected=false;break;
        case Qt.Key_Delete:rootMarker.visible=false;rootMarker.deleteMe();break;
        }
        moving=false;
    }

    onSelectedChanged: animate()
    onFocusChanged: selected=focus

    SequentialAnimation{
        id:animOpa
        loops: Animation.Infinite
        NumberAnimation {
            target: icon
            property: "opacity"
            to:0
            duration: 500
            easing.type: Easing.InOutSine
        }
        NumberAnimation {
            target: icon
            property: "opacity"
            to:1
            duration: 500
            easing.type: Easing.InOutSine
        }
    }




}

