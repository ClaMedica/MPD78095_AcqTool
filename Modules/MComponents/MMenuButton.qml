import QtQuick 2.5
import QtQuick.Window 2.0
Rectangle  {
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string theme
    property string colCur: colorMe("Idl")
    property var text
    //property real radius: 8
    property bool pressed: mouseArea.pressed
    property int tipo: 0
    property bool visibile: false
    property bool selected: false
    property string mechAction:"latch"
    property int labelSize:2
    signal clicked
    signal entered
    signal exited

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: container
    height: Screen.height*0.1
    width:Screen.width*0.1
    clip:true
    //border.color: "black"

    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#000000"
        }
        GradientStop {
            position: 0.50;
            color: colCur;
        }
        GradientStop {
            position: 1.00;
            color: "#000000";
        }
    }
    border.width:1
    states:[
        State{name:"idle";      PropertyChanges{target:container;colCur:colorMe("Idl")}},
        State{name:"hovered";   PropertyChanges{target:container;colCur:colorMe("Hov")}},
        State{name:"pressed";   PropertyChanges{target:container;colCur:colorMe("Pre")}},
        State{name:"selected";  PropertyChanges{target:container;colCur:colorMe("Sel")}}
    ]

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function display(){animation.start()}

    function colorMe(st)
    {
        switch(theme)
        {
        case "red":
            switch(st)
            {
            case "Idl":return Qt.rgba(1,0,0,1)
            case "Hov":return Qt.rgba(1,0.5,0.5,1)
            case "Sel":return Qt.rgba(0.75,0,0,1)
            case "Pre":return Qt.rgba(0.333,0,0,1)
            default:return;
            }
        case "green":
            switch(st)
            {
            case "Idl":return Qt.rgba(0,1,0,1)
            case "Hov":return Qt.rgba(0.5,1,0.5,1)
            case "Sel":return Qt.rgba(0,0.75,0,1)
            case "Pre":return Qt.rgba(0,0.333,0,1)
            default:return;
            }
        default:
            switch(st)
            {
            case "Idl":return Qt.rgba(0,0,1,1)
            case "Hov":return Qt.rgba(0.5,0.5,1,1)
            case "Sel":return Qt.rgba(0,0,0.75,1)
            case "Pre":return Qt.rgba(0,0,0.333,1)
            default:return;
            }
        }
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    onVisibleChanged: animation.start()

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    NumberAnimation {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:animation
        target: container
        property: "width"
        easing.overshoot: 3
        from:0
        to:container.width
        duration: 500
        easing.type: Easing.OutBack
    }

    MouseArea  {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: mouseArea
        scale: 1
        hoverEnabled: true
        anchors.fill: parent

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: {container.clicked();if(mechAction==="latch")selected=!selected}
        onPressed: container.state="pressed"
        onReleased: container.state=(container.selected || mechAction==="switch")?"idle":"selected"
        onEntered: {container.entered();container.state="hovered"}
        onExited: {container.exited();container.state=container.selected?"selected":"idle"}
    }

    Text  {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: buttonLabel
        anchors.left: container.left
        anchors.verticalCenter: container.verticalCenter
        anchors.right: container.right
        color: "white"
        text: container.text.toString()
        font.family:
        {
            if (PicoFlow)
                if (layout !== undefined)
                    buttonLabel.font.family = layout.value("FFamily")
                else
                    buttonLabel.font.family ="utopia"
            else if (layout !== undefined)
                buttonLabel.font.family = layout.value("FFamilyW")
            else
                buttonLabel.font.family = "Calibri"
        }
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Screen.height*0.01*labelSize
        elide: Text.ElideRight
    }
}
