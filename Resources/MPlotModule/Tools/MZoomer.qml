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
    property real xCen:xMax+xMin
    property real yCen:0
    property real xSpa:1000000
    property real ySpa:1000000

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

    signal clicked
    signal zoom
    signal move

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
        if(preview.xMin<xAbsoluteMin || preview.xMin>preview.xMax)
            preview.xMin=xAbsoluteMin
        if(preview.xMax>xAbsoluteMax || preview.xMin>preview.xMax)
            preview.xMax=xAbsoluteMax
        if(preview.yMin<yAbsoluteMin || preview.yMin>preview.yMax)
            preview.yMin=yAbsoluteMin
        if(preview.yMax>yAbsoluteMax || preview.yMin>preview.yMax)
            preview.yMax=yAbsoluteMax
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
        //console.log(xMin,xMax,yMin,yMax)
    }

    function doZoom(x,y,magn){
        //x posizione orizzontale centro dello zoom in pixel
        //y posizione verticale centro dello zoom in pixel
        //magn ingrandimento ]-1,1[ compreso con estremi esclusi


        var incX=(xMax-xMin)*Math.abs(magn) //incremento unilaterale in a.u.
        var incY=(yMax-yMin)*Math.abs(magn) //incremento unilaterale in a.u.


        var percX=x/rootZoom.width //posizione del centro in %
        var percY=y/rootZoom.height//posizione del centro in %
        var d=magn>0?1:-1;

        console.log("doZoom",magn,incX,incY,percX,percY,d)
        switch(rootZoom.state)
        {
        case "idle":            //zoom bidirezionale
            preview.xMin=xMin + d*incX*percX
            preview.xMax=xMax - d*incX*(1-percX)
            preview.yMin=yMin + d*incY*(1-percY)
            preview.yMax=yMax - d*incY*percY
            break;
        case "wheeling h":      //zoom orizzontale
            preview.xMin=xMin+d*incX*percX
            preview.xMax=xMax-d*incX*(1-percX)
            break;
        case "wheeling v":      //zoom verticale
            preview.yMin=yMin+d*incY*(1-percY)
            preview.yMax=yMax-d*incY*percY
            break;
        }
        console.log("before check",preview.xMin,preview.xMax,preview.yMin,preview.yMax)
        checkLimits()
        console.log("after check",xMin,xMax,yMin,yMax)
    }

    function polarZoom(x,y,spanX,spanY){

    }

    Timer{
        id:timWheel
        interval: 100
        repeat: false
        onTriggered:{
            console.log("trig",zoomArea.angle)
            var inc=(zoomArea.angle)*0.1
            if(inc>0.9)//limito ad uno zoom in del 90%
                inc=0.9
            if(inc<-0.9)//limito ad uno zoom out del -90%
                inc=-0.9
            doZoom(zoomArea.mouseX,
                   zoomArea.mouseY,
                   inc)
            zoomArea.angle=0
        }
    }

    MouseArea{
        property int angle:0
        id:zoomArea
        //enabled: platform==="android"?false:true
        anchors.fill:parent
        hoverEnabled: true

        onWheel:{
            if(angle===0)
                timWheel.start()
            if(wheel.angleDelta.y>0)
                angle++
            else
                angle--
        }

        onPressed: {

            if(rootZoom.state==="idle")
            {
                xClick=zoomArea.mouseX;
                yClick=zoomArea.mouseY;
                rootZoom.state="moving"
                timMov.start()
            }
            if(rootZoom.state==="zooming")
            {
                xClick=zoomArea.mouseX;
                yClick=zoomArea.mouseY;
                zoomRect.height=0
                zoomRect.width=0
                zoomRect.visible=true;
            }
        }
        onClicked: {
            rootZoom.clicked();
            rootZoom.state="idle";
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
        }


        onReleased: {
            if(rootZoom.state==="zooming"){
                rootZoom.zoom();
                zoomRect.visible=false
                var spanX=(xMax-xMin)
                var spanY=(yMax-yMin)
                xMin=(zoomRect.x)*spanX/rootZoom.width
                xMax=(zoomRect.x+zoomRect.width)*spanX/rootZoom.width
                yMin=yMin+(rootZoom.height-(zoomRect.y+zoomRect.height))*spanY/rootZoom.height
                yMax=yMax-(zoomRect.y)*spanY/rootZoom.height

                checkLimits()

            }
            if(rootZoom.state==="moving")
                timMov.stop()
            rootZoom.state="idle"
        }
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
        border.color: "white"
        x:0
        y:0
        width:0
        height:0
        visible:false
    }

    MultiPointTouchArea{
        property TouchPoint prev

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

        }
        onTouchUpdated: {//quando cambiano i punti
        console.log("touch changed")
        }

        onReleased:{//quando lascio
        }
    }
}
