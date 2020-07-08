import QtQuick 2.2
import MPlotModule 1.0

Rectangle{

    id:root_cursor

    property int min:  0
    property int max:  100
    property int lineLength: 100
    property int number: 1
    property double vMin: 0
    property double vMax: 1
    property bool moving: false
    property bool lock:   false
    property string colore: "red"
    property bool focused
    property double valPer: 0.50
    property bool selected:false
    property real value:0.3
    property bool created:false
    property bool remoteControl:false

    color:"transparent"

    height:arrow.height
    width:lineLength+arrow.width+label.width

    MarkerModel{
        id:modM

    }
    function updateY(v)
    {
        valPer=(v-vMin)/(vMax-vMin)
        var newy=(max-min)*Math.abs(1-valPer)-arrow.height/2+min
        if (v >= vMax)
            newy = min
        y = newy
        //console.log(v,valPer,height,y,vMax,vMin,max,min)
        if(modM.visible)
            visible = true

    }

    onYChanged:if(!remoteControl)updateY(value)
    onVMaxChanged: updateY(value)
    onVMinChanged: updateY(value)
    onMaxChanged:updateY(value)
    onMinChanged:updateY(value)
    onValueChanged:
    {
        if(!created || remoteControl)
        {
            updateY(value)
            created=true
            if(remoteControl)
                label.opacity=1
        }
    }

    Component.onCompleted:
    {
        valPer=(value-vMin)/(vMax-vMin)
        y=(max-min)*valPer-arrow.height/2+min
        focus=false
        animate()
    }




    onSelectedChanged: animate()
    onFocusChanged: selected=focus

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


    Rectangle{
        id:linea
        height:1
        width:lineLength
        anchors.left:root_cursor.left
        anchors.verticalCenter:root_cursor.verticalCenter
        color:arrow.colore
        opacity: 1
    }

    DCursor{
        id:arrow
        orientation: false
        rotation: 0
        locked:lock
        colore:root_cursor.colore

        anchors.left:linea.right
        anchors.verticalCenter:root_cursor.verticalCenter
        height:20
        width:20
    }

    MouseArea{
        id:mouseCursor
        anchors.fill:arrow
        enabled: !arrow.locked
        drag.target: root_cursor
        drag.axis: "YAxis"
        drag.filterChildren: true
        drag.minimumY: min-arrow.width/2
        drag.maximumY: max-arrow.width/2
        onPressed: {root_cursor.focus=true;moving=true;selected=true}
        onReleased: {moving=false;}
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
        font.pixelSize: 20
        anchors.left:arrow.right
        anchors.verticalCenter:root_cursor.verticalCenter
        text:root_cursor.value.toFixed(0)
    }

    Keys.onPressed: {
        moving=true;
        switch(event.key)
        {
        case Qt.Key_Left:x=x-1;break;
        case Qt.Key_Right:x=x+1;break;
        case Qt.Key_Up:y=y-1;break;
        case Qt.Key_Down:y=y+1;break;
        case Qt.Key_Escape:root_cursor.focus=false;selected=false;break;
        }
        moving=false;
    }



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


