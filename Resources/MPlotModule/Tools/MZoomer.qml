import QtQuick 2.0

Rectangle {
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

    signal clicked
    signal zoom
    signal move

    color:"transparent"

    state: "idle"


    function setZoom(z)
    {
        setZoomAll(z[0],z[1],z[2],z[3])
    }

    function setZoomAll(x,X,y,Y){
        xMin=x
        xMax=X
        yMin=y
        yMax=Y

        checkLimits()

    }

    function setZoomX(x,X){
        xMin=x
        xMax=X

        checkLimits()

    }
    function checkLimits()
    {
        if(xMin<xAbsoluteMin || xMin>xMax)
            xMin=xAbsoluteMin
        if(xMax>xAbsoluteMax || xMin>xMax)
            xMax=xAbsoluteMax
        if(yMin<yAbsoluteMin || yMin>yMax)
            yMin=yAbsoluteMin
        if(yMax>yAbsoluteMax || yMin>yMax)
            yMax=yAbsoluteMax        
    }

    function restore()
    {
        xMin=xAbsoluteMin
        xMax=xAbsoluteMax
        yMin=yAbsoluteMin
        yMax=yAbsoluteMax
        //console.log(xMin,xMax,yMin,yMax)
    }

    Timer
    {
        id:timWheel
        interval: 100
        repeat: false
        onTriggered:{
            //console.log("eseguo")
            zoomArea.doWheel()
            zoomArea.wheelZoom=0
        }
    }

        MouseArea{

            property int wheelZoom:0
            property real angle:0
            id:zoomArea

            anchors.fill:parent
            hoverEnabled: true
            function doWheel()
            {
                var inc=zoomInc*(Math.log(wheelZoom)+1)
                if(inc>0.9)
                    inc=0.9
                var incX=(xMax-xMin)*inc/2
                var incY=(yMax-yMin)*inc/2

                var percX=zoomArea.mouseX/rootZoom.width
                var percY=zoomArea.mouseY/rootZoom.height
                var dir=zoomArea.angle>0?"in":"out";
                //console.log(inc,zoomArea.angle>0?incX:-incX,xMin,xMax,percX)

                switch(rootZoom.state)
                {
                case "idle":
                    rootZoom.state="wheeling xy"
                    if(zoomArea.angle>0)
                    {
                        xMin+=percX*incX
                        xMax-=(1-percX)*incX
                        yMin+=(1-percY)*incY
                        yMax-=percY*incY
                    }
                    else
                    {
                        xMin-=percX*incX
                        xMax+=(1-percX)*incX
                        yMin-=(1-percY)*incY
                        yMax+=percY*incY
                    }
                    rootZoom.state="idle"
                    break;
                case "wheeling h":
                    if(zoomArea.angle>0)
                    {
                        xMin+=percX*incX
                        xMax-=(1-percX)*incX
                    }
                    else
                    {
                        xMin-=percX*incX
                        xMax+=(1-percX)*incX
                    }
                    break;
                case "wheeling v":
                    if(zoomArea.angle>0)
                    {
                        yMin+=(1-percY)*incY
                        yMax-=percY*incY
                    }
                    else
                    {
                        yMin-=(1-percY)*incY
                        yMax+=percY*incY
                    }
                    break;
                }
                //console.log("before check",dir,inc,xMin,xMax,yMin,yMax)
                checkLimits()
                //console.log("after check",xMin,xMax,yMin,yMax)
            }

            onWheel:
            {
                angle=wheel.angleDelta.y
                if(wheelZoom===0)
                {
                    timWheel.start()

                }
                wheelZoom++

                //console.log("accumulo")
            }

            onPressed: {
                if(rootZoom.state==="idle")
                {
                    xClick=zoomArea.mouseX;
                    yClick=zoomArea.mouseY;
                    rootZoom.state="moving"
                }
                if(rootZoom.state==="zooming")
                {
                    xClick=zoomArea.mouseX;
                    yClick=zoomArea.mouseY;
                    zoomRect.height=0
                    zoomRect.width=0
                    zoomRect.visible=true;
                }
                if(rootZoom.state==="moving")
                {
                    timMov.start()
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
            onTriggered:
            {

                if( xClick!=zoomArea.mouseX || yClick!=zoomArea.mouseY  )
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
    }
