import QtQuick 2.5

Rectangle {
    /*
      Questo oggetto è quello che gestisce lo zoom del singolo plot
      Il plotter che gli sta a fianco è agganciato alle variabili xMax xMin yMax yMin che dunque sono
      l'ultimo anello della catena
      di conseguenza c'è un item preview che è quello che raccoglie tutte le eventuali modifiche prima
      di trasmetterle all'esterno dopo aver chiamato la check limits
      */
    id:rootZoom

    property var zoomR
    property real xMax
    property real xMin
    property real yMax
    property real yMin
    property real xAbsoluteMax: 10
    property real xAbsoluteMin: 0
    property real yAbsoluteMax: 1
    property real yAbsoluteMin: -1
    property int xClick:0
    property int yClick:0
    property bool moving:false
    property real zoomInc:0.3
    property real dur:300
    property bool enableXZoom:true
    property bool enableYZoom:true
    property bool newOpMarker: false
    property bool newDefiner: false
    property bool newAnMarker: false

    signal clicked
    signal move
    signal newOpMarkerPos(real xpos)
    signal newDefinerPos(var pos)
    signal newAnMarkerPos(real xpos)

    color:"transparent"
    state: "idle"

    Item{
        id:preview
        property real xMax:xAbsoluteMax
        property real xMin:xAbsoluteMin
        property real yMax:yAbsoluteMax
        property real yMin:yAbsoluteMin

    }

    function setZoom(z){
        setZoomAll(z[0],z[1],z[2],z[3])
    }
    function setZoomAll(x,X,y,Y){
        preview.xMin=x
        preview.xMax=X
        preview.yMin=y
        preview.yMax=Y
        checkLimits()
    }
    function setZoomX(x,X){
        preview.xMin=x
        preview.xMax=X
        checkLimits()
    }
    function checkLimits(){
        if(preview.xMin<xAbsoluteMin || preview.xMin>preview.xMax || isNaN(preview.xMin))
            preview.xMin=xAbsoluteMin
        if(preview.xMax>xAbsoluteMax || preview.xMin>preview.xMax || isNaN(preview.xMax))
            preview.xMax=xAbsoluteMax
        if(preview.yMin<yAbsoluteMin || preview.yMin>preview.yMax || isNaN(preview.yMin))
            preview.yMin=yAbsoluteMin
        if(preview.yMax>yAbsoluteMax || preview.yMin>preview.yMax || isNaN(preview.yMax))
            preview.yMax=yAbsoluteMax
        if(preview.xMin >= (preview.xMax -1))
            return
        //applico le modifiche
        xMin=preview.xMin
        xMax=preview.xMax
        yMin=preview.yMin
        yMax=preview.yMax

    }
    function restore(){//ripristina lo zoom iniziale senza passare dal via
        xMin=xAbsoluteMin
        xMax=xAbsoluteMax
        yMin=yAbsoluteMin
        yMax=yAbsoluteMax
        //console.log("restore",xMin,xMax,yMin,yMax)
    }

    function fromXtoAU(x){
        return (xMax-xMin)*x/rootZoom.width
    }

    function fromYtoAU(y){
        return (yMax-yMin)*y/rootZoom.height
    }

    function setLimFromSectionX(min,max,x1,x2){
        var m=(max-min)/(x2-x1)
        preview.xMin=min-m*x1
        preview.xMax=m*rootZoom.width+preview.xMin
    }

    function setLimFromSectionY(min,max,y1,y2){
        var m=(max-min)/(y2-y1)
        preview.yMax=min-m*y1
        preview.yMin=preview.yMax+m*rootZoom.height
    }

    function doZoom(x,y,magnX,magnY){
        //x posizione orizzontale centro dello zoom in pixel
        //y posizione verticale centro dello zoom in pixel
        //magn ingrandimento ]-1,1[ compreso con estremi esclusi

        var incX=(xMax-xMin)*Math.abs(magnX) //incremento unilaterale in a.u.
        var incY=(yMax-yMin)*Math.abs(magnY) //incremento unilaterale in a.u.

        var percX=x/rootZoom.width //posizione del centro in %
        var percY=y/rootZoom.height//posizione del centro in %
        var dx=magnX>0?1:-1;
        var dy=magnY>0?1:-1;

       // console.log("doZoom",magnX,magnY,incX,incY,percX,percY,dx,dy)


        switch(rootZoom.state)
        {
        case "idle":            //zoom bidirezionale
            if(enableXZoom){
            preview.xMin=xMin + dx*incX*percX
            preview.xMax=xMax - dx*incX*(1-percX)
            }
            if(enableYZoom){
            preview.yMin=yMin + dy*incY*(1-percY)
            preview.yMax=yMax - dy*incY*percY
            }
            break;
        case "wheeling h":      //zoom orizzontale
            if(enableXZoom){
            preview.xMin=xMin+dx*incX*percX
            preview.xMax=xMax-dx*incX*(1-percX)
            }
            break;
        case "wheeling v":      //zoom verticale
            if(enableYZoom){
            preview.yMin=yMin+dy*incY*(1-percY)
            preview.yMax=yMax-dy*incY*percY
            }
            break;
        }
       // console.log("before check",preview.xMin,preview.xMax,preview.yMin,preview.yMax)
        checkLimits()
      //  console.log("after check",xMin,xMax,yMin,yMax)
    }

    function setView(x,y,spanX,spanY){
        //questa funzione setta la view in termini di a.u.
        preview.xMax=x+spanX/2
        preview.xMin=x-spanX/2
        preview.yMax=y+spanY/2
        preview.yMin=y-spanY/2
        checkLimits()
    }

    function zoomButton(inc)
    {
        preview.xMin=xMin + inc
        preview.xMax=xMax - inc
        checkLimits()
    }

    Timer{
        id:timWheel
        interval: 100
        repeat: false
        onTriggered:{
           // console.log("trig",zoomArea.angle)
            var inc=(zoomArea.angle)*0.1
            if(inc>0.9)//limito ad uno zoom in del 90%
                inc=0.9
            if(inc<-0.9)//limito ad uno zoom out del -90%
                inc=-0.9
            doZoom(zoomArea.mouseX,
                   zoomArea.mouseY,
                   inc,inc)
            zoomArea.angle=0
        }
    }

    MouseArea{
        property int angle:0
        property bool drawDefiner: false
        id:zoomArea
        enabled: platform==="android"?false:true
        anchors.fill:parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onWheel:{
            if(angle===0)
                timWheel.start()
            if(wheel.angleDelta.y>0)
                angle++
            else
                angle--
        }

        onPressed: {
            if (!isTouch && mouse.button === Qt.RightButton)
                rootZoom.state="zooming"
            xClick=zoomArea.mouseX;
            yClick=zoomArea.mouseY;
            if (newDefiner){
                drawDefiner = true
                //console.log("inserisco nuovo definitore",zoomArea.mouseX, zoomArea.mouseY)
                rectDef.x = zoomArea.mouseX
            }
            else
            {
                if(rootZoom.state==="idle")
                {
                    rootZoom.state="moving"
                    timMov.start()
                }
                if(rootZoom.state==="zooming")
                {
                    zoomRect.height=0
                    zoomRect.width=0
                    zoomRect.visible=true;
                }
            }
        }
        onClicked: {
            if (newOpMarker) {
                console.log("inserisco nuovo marker operativo")
                var pos = (zoomArea.mouseX*(xMax-xMin)/zoomArea.width) + xMin //(+xMin è necessario nel caso di zoom)
                newOpMarkerPos(pos)
                newOpMarker = false
            }
            else if (newAnMarker) {
                var posan = (zoomArea.mouseX*(xMax-xMin)/zoomArea.width) + xMin //(+xMin è necessario nel caso di zoom)
                console.log("inserisco nuovo marker analitico",posan)
                newAnMarkerPos(posan)
                newAnMarker = false
            }
            else if (!newDefiner) {
                rootZoom.clicked();
                rootZoom.state="idle";
            }
        }

        onPositionChanged: {
            if(rootZoom.state==="zooming")
            {
                var w=zoomArea.mouseX-xClick
                var h=zoomArea.mouseY-yClick
                zoomRect.width=Math.abs(w)
                zoomRect.height=Math.abs(h)
                zoomRect.x=w>0?xClick:zoomArea.mouseX
                zoomRect.y=h>0?yClick:zoomArea.mouseY
            }
            if (drawDefiner) {
                rectDef.movingX = zoomArea.mouseX
                rectDef.visible = true
            }
        }
        onReleased: {
            if (drawDefiner)
            {
                drawDefiner = false
                newDefiner = false
                var x = rectDef.x*(xMax-xMin)/zoomArea.width + xMin //(+xMin è necessario nel caso di zoom)
                var w = x + (rectDef.width*(xMax-xMin))/zoomArea.width
                var y = rectDef.y*(yMax-yMin)/zoomArea.height + yMin //(+xMin è necessario nel caso di zoom)
                var h = y +  (rectDef.height*(yMax-yMin))/zoomArea.height
                var pos = [x,y,w,h]
                newDefinerPos(pos)
                //resetto il rettangolo provvisorio
                rectDef.visible = false
                rectDef.x = 0
                rectDef.y = 15
                rectDef.movingX = 0
            }
            if(rootZoom.state==="zooming"){
                zoomRect.visible=false
                var spanX=(xMax-xMin)
               // var spanY=(yMax-yMin)
                preview.xMin=(zoomRect.x)*spanX/rootZoom.width
                preview.xMax=(zoomRect.x+zoomRect.width)*spanX/rootZoom.width
               // preview.yMin=yMin+(rootZoom.height-(zoomRect.y+zoomRect.height))*spanY/rootZoom.height
               // preview.yMax=yMax-(zoomRect.y)*spanY/rootZoom.height

                checkLimits()
            }
            if(rootZoom.state==="moving")
                timMov.stop()
            rootZoom.state="idle"
        }
    }

    Rectangle {
        id:rectDef
        property real movingX: 0
        x: 0
        y: 15
        border.width: 2
        border.color: "blue"
        color: "transparent"
        height: rootZoom.height - y*2
        width: movingX - x
        radius: 7
        visible: false
    }

    states: [
        State {
            name: "zooming"
            PropertyChanges{target:zoomArea;cursorShape: Qt.CrossCursor}
            PropertyChanges{target:rootZoom;dur: 300}
        },
        State {
            name: "moving"
            PropertyChanges{target:zoomArea;cursorShape: Qt.ClosedHandCursor}
            PropertyChanges{target:rootZoom;dur: 10}
        },
        State {
            name: "wheeling h"
            PropertyChanges{target:zoomArea;cursorShape: Qt.SizeHorCursor}
            PropertyChanges{target:rootZoom;dur: 300}
        },
        State {
            name: "wheeling v"
            PropertyChanges{target:zoomArea;cursorShape: Qt.SizeVerCursor}
            PropertyChanges{target:rootZoom;dur: 300}
        },
        State {
            name: "wheeling xy"
            PropertyChanges{target:zoomArea;cursorShape: Qt.OpenHandCursor}
            PropertyChanges{target:rootZoom;dur: 300}
        },
        State {
            name: "idle"
            PropertyChanges{target:zoomArea;cursorShape: Qt.OpenHandCursor}
            PropertyChanges{target:rootZoom;dur: 300}
        }

    ]


    Timer{
        id:timMov

        interval: 50
        onTriggered:{

            if(xClick!==zoomArea.mouseX || yClick!==zoomArea.mouseY)
            {

                var xx=-(zoomArea.mouseX-xClick)
                var yy=-(zoomArea.mouseY-yClick)
                var spanX=(xMax-xMin)
                var spanY=(yMax-yMin)

                var x=(xx)*spanX/rootZoom.width
                var y=(yy)*spanY/rootZoom.height

                if(xMin+x>xAbsoluteMin && xMax+x<xAbsoluteMax)
                {//tutto ok
                    xMin+=x
                    xMax+=x
                }
                else if(xMin+x<xAbsoluteMin && xMax+x<xAbsoluteMax)
                {//tutto ok
                    xMin=xAbsoluteMin
                    xMax=xMin+spanX
                }
                else if(xMin+x>xAbsoluteMin && xMax+x>xAbsoluteMax)
                {//tutto ok
                    xMax=xAbsoluteMax
                    xMin=xMax-spanX
                }

                if(yMin-y>yAbsoluteMin && yMax-y<yAbsoluteMax)
                {//tutto ok
                    yMin-=y
                    yMax-=y
                }
                else if(yMin-y<yAbsoluteMin && yMax-y<yAbsoluteMax)
                {//tutto ok
                    yMin=yAbsoluteMin
                    yMax=yMin+spanY
                }
                else if(yMin-y>yAbsoluteMin && yMax-y>yAbsoluteMax)
                {//tutto ok
                    yMax=yAbsoluteMax
                    yMin=yMax-spanY
                }

                xClick=zoomArea.mouseX
                yClick=zoomArea.mouseY
            }

            timMov.start()
        }
    }

    Rectangle{
        id:zoomRect
        color:"transparent"
        border.width: 2
        border.color: "gray"
        x:0
        y:0
        width:0
        height:0
        visible:false
    }

    MultiPointTouchArea{
        property TouchPoint prev
        Item{
            id:old
            property real x1:0
            property real x2:0
            property real y1:0
            property real y2:0
        }

        id:zoomTouchArea
        minimumTouchPoints: 1
        maximumTouchPoints: 2
        enabled: platform==="android"?true:false
        mouseEnabled: true
        anchors.fill: parent
        touchPoints: [
            TouchPoint { id: p1 },
            TouchPoint { id: p2 }
        ]
        onPressed:{//quando premo
            old.x1=fromXtoAU(p1.startX)
            old.y1=fromYtoAU(p1.startY)
            old.x2=fromXtoAU(p2.startX)
            old.y2=fromYtoAU(p2.startY)
        }
        onTouchUpdated: {//quando cambiano i punti
            if(touchPoints.length===2)
            {
                setLimFromSectionX(old.x1,old.x2,p1.x,p2.x)
                setLimFromSectionY(old.y1,old.y2,p1.y,p2.y)
                checkLimits()
            }

        }

        onReleased:{//quando lascio

        }
    }
}
