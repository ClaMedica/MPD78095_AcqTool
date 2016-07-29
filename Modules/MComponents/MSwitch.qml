import QtQuick 2.4
import QtQuick.Controls 1.4
import MComponents 1.0 //import "qrc:/Components"
import "qrc:/GeneralData.js" as DataEngine
Rectangle {
    id:rootSwitch
    property string opt1:"ON"
    property string opt2:"OFF"
    property bool choose:false
    property color col1:"transparent"
    property color col2:"transparent"
    radius:10
    width: 200
    height: 100
    color: layout.value("BackgroundColor")
    state:"opt1"
    border.color: "white"
    border.width: 3
    clip:true




    Rectangle{
        id:rec1
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.left:parent.left
        anchors.margins: parent.border.width
        width:Math.round(parent.width*0.5)
        color:col1
        Text{
            id:txt1
            anchors.fill:parent
            font.bold:true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text:opt1
            color:!choose?"black":"white"
            Behavior on color {ColorAnimation{duration: 200}}
        }
    }

    Rectangle{
        id:rec2
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.right:parent.right
        anchors.margins: parent.border.width
        width:Math.round(parent.width*0.5)
        color:col2
        Text{
            id:txt2
            anchors.fill:parent
            font.bold:true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text:opt2
            color:choose?"black":"white"
            Behavior on color {ColorAnimation{duration: 200}}
        }

    }


    MouseArea{
        id:area
        property string stato:"idle"
        property int offset:0

        anchors.top:parent.top
        anchors.bottom:parent.bottom
        x:0

        width:Math.round(parent.width*0.5)


        onPressed: {
            stato="pressed"
            offset=mouse.x
            //console.log(area.x)
        }
        onMouseXChanged:
            if(stato === "moving" || stato === "pressed"){
                area.x+=Math.round(mouse.x-offset)
                if(area.x>Math.round(rootSwitch.width*0.50)){
                    area.x=Math.round(rootSwitch.width*0.50)
                    stato="idle"
                }
                if(area.x<0){
                    area.x=0
                    stato="idle"
                }
                if(mouse.x-offset!==0)
                    stato="moving"
            }
        onReleased: {
            //console.log(stato)
            if(stato==="moving")
                choose=area.x>Math.round(rootSwitch.width*0.25)?true:false

            if(stato==="pressed")
                choose=!choose

            if(choose)
                anim.to=Math.round(rootSwitch.width*0.50)
            else
                anim.to=0
            anim.start()

            stato="idle"
        }
        onExited:   stato="idle";
        onEntered:  stato="idle";

        NumberAnimation{
            id:anim
            target:area
            properties:"x"
            easing.type: Easing.InCubic;
            duration:200
        }

        Rectangle{
            id:recHover
            anchors.fill:parent
            anchors.margins: rootSwitch.border.width
            color: DataEngine.c_btn_enabled
            opacity: 0.7
            radius:10
        }
    }


}

