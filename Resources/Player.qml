import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
 
import QtQuick.Controls.Styles 1.2
import "qrc:/Components"
import MComponents 1.0

Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property alias sliderPos:slider.value
    property bool playing:false
    property real startPos:0
    property real endPos:0
    property real duration:0
    signal play
    signal pause

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootPlayer
    color:"yellow"
    y:30
    opacity: 1
    clip:true
    enabled:false
    Behavior on height {NumberAnimation { duration: 500 }}

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function playClick()
    {
        play.playClick()
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:fb
        text:"<<"
        showImage: false
        height: parent.height
        width:parent.width/20
        anchors.left:parent.left
        anchors.top:parent.top

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:
            if( sliderPos-500<0)
                sliderPos=0
            else
                sliderPos-=500
    }

    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:play
        text:"ᐅ"
        showImage: false
        height: parent.height
        width:parent.width/20
        anchors.left:fb.right
        anchors.top:parent.top

        //@@@@@@@@@@    Functions       @@@@@@@@@@
        function playClick()
        {
            if(text=="ᐅ")
            {
                playing=true
                if(slider.value==endPos)
                    sliderPos=startPos
                text="||"
                rootPlayer.play()
                pauseAnim.stop()
                opacity=1
            }
            else
            {
                text="ᐅ"
                rootPlayer.pause()
                pauseAnim.start()
                playing=false
            }
        }

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:playClick();

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        SequentialAnimation{
            id:pauseAnim
            loops: Animation.Infinite
            NumberAnimation {
                target: play
                property: "opacity"
                to:0
                duration: 500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                target: play
                property: "opacity"
                to:1
                duration: 500
                easing.type: Easing.InOutSine
            }
        }


    }

    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:ff
        text:">>"
        showImage: false
        height: parent.height
        width:parent.width/20
        anchors.left:play.right
        anchors.top:parent.top

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:
            if( sliderPos+500>duration)
            {
                sliderPos=duration
            }
            else
            {
                sliderPos+=500
            }
    }

    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:curTime
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:ff.right
        width:parent.width*3/20
        height: parent.height
        text:Func.fromDoubleTimeToString(slider.value)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 12
    }

    Slider{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:slider
        style: SliderStyle {
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            groove: Rectangle {
                //@@@@@@@@@@    Properties      @@@@@@@@@@
                implicitWidth: 200
                implicitHeight: 8
                color: "blue"
                radius: 8
            }
            handle: Rectangle {
                //@@@@@@@@@@    Properties      @@@@@@@@@@
                anchors.centerIn: parent
                color: control.pressed ? "white" : "lightblue"
                border.color: "blue"
                border.width: 2
                width: 34
                height: 34
                radius: 12
            }
        }
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:curTime.right
        width:parent.width*11/20
        height: parent.height
        minimumValue: startPos
        maximumValue: endPos
        value:sliderPos
        updateValueWhileDragging: true

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onValueChanged: {
            if(value==duration)
                play.text="ᐅ"
        }
    }

    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:slider.right
        width:parent.width*3/20
        height: parent.height
        text:Func.fromDoubleTimeToString(endPos)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 12
    }
}
