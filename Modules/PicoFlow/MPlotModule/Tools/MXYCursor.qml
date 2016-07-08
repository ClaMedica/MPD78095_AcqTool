import QtQuick 2.2
import MPlotModule 1.0
//da decommentare per debug con Test
//import QuickRealTimeGraph 1.0

Rectangle {
    id:curXY

    property int dragXmin:  0
    property int dragXmax:  100
    property int dragYmin:  0
    property int dragYmax:  100
    property int lineLengthX: 100
    property int lineLengthY: 100
    property int numero: 1
    property double vMinX:  0
    property double vMaxX:  1
    property double vMinY:  0
    property double vMaxY:  1

    property bool lock:   false
    property string colore: "red"
    property alias vPx: curX.valPer
    property alias vPy: curY.valPer
    property alias valueX:curX.value
    property alias valueY:curY.value


    color:"transparent"


    height: curX.height
    width:  curY.width

    MCursorX{
        id:curX
        lineLength: curXY.lineLengthX
        number: curXY.numero
        lock:true
        colore:curXY.colore
        anchors.bottom:curXY.bottom
        min: 0
        max: curXY.lineLengthY
        vMin: vMinX
        vMax: vMaxX

        anchors.horizontalCenter: cross.horizontalCenter
    }

    MCursorY{
        id:curY
        lineLength: curXY.lineLengthY
        number: curXY.numero
        lock:true
        colore:curXY.colore
        anchors.left:curXY.left
        min: curXY.height-curXY.lineLengthX
        max: curXY.height
        vMin: vMinY
        vMax: vMaxY
        anchors.verticalCenter: cross.verticalCenter
    }

    onHeightChanged:cross.y=curX.height-(1-curY.valPer)*lineLengthX-cross.height/2
    onWidthChanged:cross.x=curX.valPer*lineLengthY-cross.width/2

    Rectangle{
        id:cross

        height:10
        width:10

        Component.onCompleted:
        {
        x=lineLengthY*curX.valPer-cross.height/2
        y=curXY.height-lineLengthX*curY.valPer-cross.height/2
        }
        color:curXY.colore
        opacity: 0.3
        onFocusChanged: {
            curX.selected=focus;
            curY.selected=focus;}
        MouseArea{
            id:mouse
            anchors.fill:cross
            enabled: !curXY.lock
            drag.target: cross
            drag.axis: "XandYAxis"
            drag.filterChildren: true
            drag.minimumX: 0-cross.width/2
            drag.maximumX: curXY.lineLengthY-cross.width/2
            drag.minimumY: curXY.height-curXY.lineLengthX-cross.height/2
            drag.maximumY: curXY.height-cross.height/2
            onEntered: curX.showPopUp()
            onExited: curX.hidePopUp()
            onPressed: {
                cross.focus=true;
                curX.moving=true;
                curY.moving=true;
                curX.selected=true;
                curY.selected=true;
            }
            onReleased: {curX.moving=false;curY.moving=false;}
            hoverEnabled: true
        }


        Keys.onPressed: {
            curX.moving=true;curY.moving=true
            switch(event.key)
            {
            case Qt.Key_Left:curX.x=curX.x-1;break;
            case Qt.Key_Right:curX.x=curX.x+1;break;
            case Qt.Key_Up:curY.y=curY.y-1;break;
            case Qt.Key_Down:curY.y=curY.y+1;break;
            case Qt.Key_Escape:cross.focus=false;curX.selected=false;curY.selected=false;break;
            }
            curX.moving=false;curY.moving=false;
        }
    }

}
/*
    Rectangle
    {
     id: dragProxy;
     width: 10; height:10; color:"transparent"; // make me invisible

     property variant dragObjects:[curX,curY];

     property int lastX;
     property int lastY;

     onXChanged:
     {
      var deltaX = x - lastX;
      for(var i = 0; i < dragObjects.length; ++i)
       dragObjects[i].x += deltaX;
      lastX = x;
     }

     onYChanged:
     {
      var deltaY = y - lastY;
      for(var i = 0; i < dragObjects.length; ++i)
       dragObjects[i].y += deltaY;
      lastY = y;
     }
    }
*/
