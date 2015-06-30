import QtQuick 2.0
//import Resources 1.0
Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string text: "Button"
    property string background: ""
    property string pressedBackground: ""
    property int borderWidth
    property int defWidth: button.text==""?button.width:buttonLabel.width + 20
    property int defHeight: button.text==""?button.height:buttonLabel.height + 5
    property int defX:100
    property int defY:100
    property string type:"testo"
    property bool showImage: true
    property real radius: 8
    property bool hoverEnable: true
    property bool pressed: mouseArea.pressed
    property int imageFill:Image.Stretch
    signal clicked
    signal entered
    signal exited    

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: button
    width:defWidth
    height:defHeight
    x:defX
    y:defY
    clip:true
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#00ffff"
        }
        GradientStop {
            position: 0.593
            color: if(pressed)
                       "#0099ff";
                   else
                       "#0000ff";
        }
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Image {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: ima
        source: pressed?button.pressedBackground:button.background
        opacity: showImage?1:0
        anchors.fill:button
        fillMode:imageFill
        Behavior on opacity {
                NumberAnimation { easing.type: Easing.InExpo; duration: 200 }
            }
    }

    MouseArea  {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: mouseArea
        scale: 1
        anchors.fill: button
        hoverEnabled: button.hoverEnable

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: button.clicked();
        onExited:
        {
            if(type=="testoImg")
                ima.opacity=showImage?1:0
        }
        onEntered:
        {
            if(type=="testoImg")
                ima.opacity=0
            button.entered();
        }

    }

    Text  {
        //@@@@@@@@@@    Definitions     @@@@@@@@@@
        id: buttonLabel
        anchors.centerIn: button
        text: button.text
        color: "white"
        font.family: "Ubuntu"
        font.bold: true
        styleColor: "#f9f9f9"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 18
        opacity: !ima.opacity
        Behavior on opacity {
                NumberAnimation { easing.type: Easing.InExpo; duration: 200 }
            }
    }
}
