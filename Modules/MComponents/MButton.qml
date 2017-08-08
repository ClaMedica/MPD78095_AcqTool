import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import MComponents 1.0

Rectangle {
    id:rootBtn
    property string image:""
    property real labelSize:2
    property string text
    property bool switchEnabled:false
    property bool checked:control.checked
    property bool orange: false

    signal clicked(var id)
    signal pressed(var id)
    signal released(var id)
    signal pressAndHold(var id)



    onImageChanged:img.source=image


    border.width: control.activeFocus ? 3 : 1
    border.color: DataEngine.TASKBAR_COLOR
    radius: height*0.1
    color:{
        if (!control.enabled)
            return DataEngine.c_btn_idle
        if (rootBtn.orange)
        {
            if (control.pressed)
                return DataEngine.c_btn_orange_pressed
            if (control.checked)
                return DataEngine.c_btn_orange_checked
            if (control.hovered)
                return DataEngine.c_btn_orange_hovered
            if (control.enabled)
                return DataEngine.c_btn_orange_enabled
        }
        else
        {
            if (control.pressed)
                return DataEngine.c_btn_pressed
            if (control.checked)
                return DataEngine.c_btn_checked
            if (control.hovered)
                return DataEngine.c_btn_hovered
            if (control.enabled)
                return DataEngine.c_btn_enabled
        }
    }

    Behavior on color {
        ColorAnimation {
            duration: 200
        }
    }

    MLabel{
        text:rootBtn.text
        anchors.fill: rootBtn
        labelSize: rootBtn.labelSize
        color: control.pressed?"white":"black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
    }


    function blink(onOff)
    {
        if(onOff)
            anim.start()
        else
            anim.stop()
    }

    Image{
        id:img
        anchors.fill:rootBtn
        anchors.margins: parent.height*0.01
    }

    Rectangle{
        id:recBlink
        radius: 4
        border.width: 3
        color:"transparent"
        border.color: "transparent"
        anchors.fill: rootBtn

        SequentialAnimation on border.color {
            id:anim
            running: false
            ColorAnimation {to: "yellow";       duration: 500; easing.type: Easing.InOutQuad }
            ColorAnimation {to: "transparent";  duration: 500; easing.type: Easing.InOutQuad}
            loops: Animation.Infinite
            alwaysRunToEnd: true
        }
    }
    BuzzMouseArea{
        id:control
        property bool hovered:false
        property bool pressed:false
        property bool checked:false
        hoverEnabled: true
        anchors.fill: rootBtn
        onEntered:
        {
            if(!isTouch)
                hovered=true
        }
        onExited:
        {
            if(!isTouch)
                hovered=false
        }
        onPressed:
        {
            rootBtn.pressed(this)
            pressed=true
        }
        onReleased:
        {
            rootBtn.released(this)
            pressed=false
        }
        onClicked:
        {
            rootBtn.clicked(this);
            if(switchEnabled)
            {
                pressed=false
                checked=!checked
            }
        }
        enabled:rootBtn.enabled
        onPressAndHold: rootBtn.pressAndHold(this)
    }
}

