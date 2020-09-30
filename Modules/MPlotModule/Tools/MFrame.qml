import QtQuick 2.3
import MPlotModule 1.0

Rectangle{
    property Rectangle rec
    property var modello
    property real vMinX
    property real vMaxX
    property real vMinY
    property real vMaxY
    property real xMin:modF.xMin
    property real xMax:modF.xMax
    property real yMin:modF.yMin
    property real yMax:modF.yMax
    property real whoAmI:modF.whoAmI
    property var zoomMe
    signal modifyMe
    signal deleteMe

    id:rootFrame
    color:"transparent"
    height:rec.height-20
    width:rec.width*(modF.xMax-modF.xMin)/(vMaxX-vMinX)
    x:rec.width*(modF.xMin-vMinX)/(vMaxX-vMinX)
    y:10
    radius:10
    border.width: 2
    border.color: modF.color
    clip:true


    Rectangle{
        id:sizeRecH
        rotation:90
        anchors.centerIn: parent
        height:parent.width
        width:parent.height
        radius: 10
        opacity:0
        Behavior on opacity {NumberAnimation { duration: 200 }}
        gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1-(20/rootFrame.width); color: "transparent" }
                GradientStop { position: 1.0; color: "white" }
            }
    }

    Rectangle{
        id:sizeRecV
        rotation:90
        anchors.centerIn: parent
        height:parent.height
        width:parent.width
        radius: 10
        opacity:0
        Behavior on opacity {NumberAnimation { duration: 200 }}
        gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1-(20/rootFrame.height); color: "transparent" }
                GradientStop { position: 1.0; color: "white" }
            }
    }

    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        if(modello.length%2===0 && modello.length>=2)
        {
            //console.log("modello frame",modello)
            for(var i=0;i<modello.length;i+=2)
                modF.setProperty(modello[i],modello[i+1])
        }

        //console.log(x,y,width,height,rec.height,rec.width,modF.xMax,modF.xMin,vMaxX,vMinX)
    }

    FrameModel{id:modF}

    Rectangle{
        id:sfondo
        color:Qt.rgba(0.2,0.2,0.2,0.2)
        anchors.top:parent.top
        width:parent.width
        height:parent.height
        opacity: 0
        radius:10
        Behavior on opacity {NumberAnimation { duration: 300 }}

        Text{
            id:testoDef
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            text:modF.descr
            color:"white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: screenH * 0.015

            MouseArea{
                anchors.fill:testoDef
                acceptedButtons: Qt.RightButton
                onPressed: {
                     if (!isTouch && mouse.button === Qt.RightButton)
                     {
                         clicright.y = testoDef.y
                         clicright.x = testoDef.x
                         clicright.visible = true
                         delTim.start()
                     }
                }
                preventStealing: true
            }
        }
        clip:true
    }

    MouseArea{
        id:allMove
        property string stato:"idle"
        property real posX:0
        property real posY:0

        anchors.fill:parent
        propagateComposedEvents: true
        hoverEnabled: true
        enabled: PicoFlow?false:true
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MidButton
        onPressed: {
             if (!isTouch && mouse.button === Qt.RightButton)
                 mouse.accepted = false

             if (!isTouch && mouse.button === Qt.LeftButton)
                 mouse.accepted = false

             if (!isTouch && mouse.button === Qt.MidButton)
                 mouse.accepted = true
        }

        onEntered: {
            rootFrame.focus=true
            if (!PicoFlow) sfondo.opacity=1
        }
        onExited: {
            rootFrame.focus=false
            sfondo.opacity=0
        }

        onDoubleClicked: {
            if (!isTouch && mouse.button === Qt.MidButton){
                zoomMe=[modF.xMin,modF.xMax,modF.yMin,modF.yMax]}
        }
    }

//    Keys.onPressed: {
//        switch(event.key)
//        {
//        case Qt.Key_Delete:rootFrame.deleteMe();break;
//        }
//    }

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
            text: qsTr("delete")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea{
          anchors.fill:parent
          //delete
          onClicked: {
             clicright.visible = false
             rootFrame.deleteMe()
          }
        }
    }

    Timer{
        id:delTim
        interval: 2000
        onTriggered:
            clicright.visible = false
    }

    MouseArea{
        id:leftMove
        property string stato:"idle"
        property real pos:0
        width: 10
        anchors.left:parent.left
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeHorCursor
        onPressed: {stato="changing";pos=mouseX;timRes.start()}
        onReleased: {stato="idle";timRes.stop();rootFrame.modifyMe()}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        onEntered:{sizeRecH.rotation=90;sizeRecH.opacity=1}
        onExited:{sizeRecH.opacity=0}
        enabled: PicoFlow? false:modF.resizeable
    }
    MouseArea{
        id:rightMove
        property string stato:"idle"
        property real pos:0
        width: 10
        anchors.right:parent.right
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeHorCursor
        onPressed: {stato="changing";pos=mouseX;timRes.start()}
        onReleased: {stato="idle";timRes.stop();rootFrame.modifyMe()}
        onEntered:{sizeRecH.rotation=270;sizeRecH.opacity=1}
        onExited:{sizeRecH.opacity=0}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        enabled: PicoFlow? false:modF.resizeable
    }

    Timer{
        id:timRes
        interval:50
        onTriggered: {
            var dist
            var perc
            var step
            if(leftMove.stato==="changing")
            {
                dist=leftMove.mouseX-leftMove.pos
                perc=dist/rec.width
                step=(vMaxX-vMinX)*perc
                modF.xMin+=step
            }
            if(rightMove.stato==="changing")
            {
                dist=rightMove.mouseX-rightMove.pos
                perc=dist/rec.width
                step=(vMaxX-vMinX)*perc
                modF.xMax+=step
            }
        }
    }


}

 
