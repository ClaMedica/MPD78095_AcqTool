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
    property var fullCode:[modF.family,modF.name,modF.code]
    property var zoomMe
    signal modifyMe

    id:rootFrame
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
        //console.log("modello marker",rootCursor.number,"=",modello)
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modF.setProperty(modello[i],modello[i+1])
        }

        //console.log(x,y,width,height,rec.height)
        //console.log("I'm cursor n°",number,modM.number)
    }

    FrameModel{id:modF}

    //Behavior on opacity {NumberAnimation { duration: 1000 }}
    focus:true

    color:"transparent"

    height:rec.height*(modF.yMax-modF.yMin)/(vMaxY-vMinY)
    width:rec.width*(modF.xMax-modF.xMin)/(vMaxX-vMinX)
    x:rec.width*(modF.xMin-vMinX)/(vMaxX-vMinX)
    y:rec.height*(vMaxY-modF.yMax)/(vMaxY-vMinY)
    //---------------------------------------------
    Rectangle{
        id:sfondo
        color:Qt.rgba(0.2,0.2,0.2,0.5)
        anchors.top:parent.top
        width:parent.width
        height:parent.height
        opacity: 0
        radius:10
        Behavior on opacity {NumberAnimation { duration: 300 }}

        Text{
            anchors.centerIn: parent
            height:parent.height-10
            width:parent.width-10
            text:modF.popUp
            color:"white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        clip:true
    }

    MouseArea{
        id:allMove
        property string stato:"idle"
        property real posX:0
        property real posY:0

        anchors.fill:parent
        cursorShape: Qt.SizeAllCursor
        onPressed: {stato="changing";posX=mouseX;posY=mouseY;timRes.start()}
        onReleased:{stato="idle";timRes.stop();rootFrame.modifyMe()}
        onEntered: sfondo.opacity=true
        onExited: sfondo.opacity=false
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        onDoubleClicked: zoomMe=[modF.xMin,modF.xMax,modF.yMin,modF.yMax]
    }
    MouseArea{
        id:leftMove
        property string stato:"idle"
        property real pos:0
        width: 5
        anchors.left:parent.left
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeHorCursor
        onPressed: {stato="changing";pos=mouseX;timRes.start()}
        onReleased:{stato="idle";timRes.stop();rootFrame.modifyMe()}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        onEntered:{sizeRecH.rotation=90;sizeRecH.opacity=1}
        onExited:{sizeRecH.opacity=0}
        enabled: modF.resizeable
    }
    MouseArea{
        id:rightMove
        property string stato:"idle"
        property real pos:0
        width: 5
        anchors.right:parent.right
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeHorCursor
        onPressed: {stato="changing";pos=mouseX;timRes.start()}
        onReleased:{stato="idle";timRes.stop();rootFrame.modifyMe()}
        onEntered:{sizeRecH.rotation=270;sizeRecH.opacity=1}
        onExited:{sizeRecH.opacity=0}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        enabled: modF.resizeable
    }

    MouseArea{
        id:topMove
        property string stato:"idle"
        property real pos:0
        height:5
        anchors.left:parent.left
        anchors.top:parent.top
        anchors.right: parent.right
        cursorShape: Qt.SizeVerCursor
        onPressed: {stato="changing";pos=mouseY;timRes.start()}
        onReleased:{stato="idle";timRes.stop();rootFrame.modifyMe()}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        onEntered:{sizeRecV.rotation=180;sizeRecV.opacity=1}
        onExited:{sizeRecV.opacity=0}
        enabled: modF.resizeable
    }
    MouseArea{
        id:bottomMove
        property string stato:"idle"
        property real pos:0
        height: 5
        anchors.right:parent.right
        anchors.left:parent.left
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeVerCursor
        onPressed: {stato="changing";pos=mouseY;timRes.start()}
        onReleased:{stato="idle";timRes.stop();rootFrame.modifyMe()}
        onEntered:{sizeRecV.rotation=0;sizeRecV.opacity=1}
        onExited:{sizeRecV.opacity=0}
        onPositionChanged: {if(stato==="changing")timRes.start()}
        hoverEnabled: true
        enabled: modF.resizeable
    }

    Timer{id:timRes
        interval:50
        onTriggered: {

            var dist
            var perc
            var step
            var dist2
            var perc2
            var step2

            if(allMove.stato==="changing")
            {
                dist=allMove.mouseX-allMove.posX
                dist2=allMove.mouseY-allMove.posY
                perc=dist/rec.width
                perc2=dist2/rec.height
                step=(vMaxX-vMinX)*perc
                step2=(vMaxY-vMinY)*perc2
                modF.xMin+=step
                modF.xMax+=step
                modF.yMin-=step2
                modF.yMax-=step2
            }

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

            if(topMove.stato==="changing")
            {
                dist=topMove.mouseY-topMove.pos
                perc=dist/rec.height
                step=(vMaxY-vMinY)*perc
                modF.yMax-=step
            }

            if(bottomMove.stato==="changing")
            {
                dist=bottomMove.mouseY-bottomMove.pos
                perc=dist/rec.height
                step=(vMaxY-vMinY)*perc
                modF.yMin-=step
            }

        }

    }

    radius:10
    border.width: 2
    border.color: modF.color
    clip:true
}
