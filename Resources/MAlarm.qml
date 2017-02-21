import QtQuick 2.0
//import Resources 1.0
import "qrc:/Models"

Rectangle {
    //@@@@@@@@@@    Definitions      @@@@@@@@@@
    property var modello:[]
    property int alarmMexHeight:30    
    signal opening
    signal closing
    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootAlarm
    color:"orange"
    radius:5
    border.width: 2
    border.color: "black"
    opacity:1
    height:alarmMexHeight
/*
    SoundEffect{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:sound
        source:modA.sound
        loops:1
        volume: 1
        onLoadedChanged:play()
    }
*/
    Behavior on height {NumberAnimation { duration: 200 }}
    Behavior on y {NumberAnimation { duration: 200 }}

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function closeHelp()
    {
        //console.log("chiudo l'help")
        rootAlarm.height=alarmMexHeight
        closing()
        rootAlarm.state="closed"
    }

    function openHelp()
    {
        //console.log("apro l'help")
        rootAlarm.height=alarmMexHeight*3
        opening()
        rootAlarm.state="opened"
    }
    //@@@@@@@@@@    Events          @@@@@@@@@@
    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modA.setProperty(modello[i],modello[i+1])
        }
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    AlarmModel{id:modA}

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mexRec
        color:modA.color
        radius:5
        anchors.margins: 5
        anchors.top:rootAlarm.top
        anchors.left:rootAlarm.left
        anchors.right: rootAlarm.right
        height:alarmMexHeight-anchors.margins*2

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:mex
            color:"white"
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: true
            font.pixelSize: 16
            anchors.fill:parent
            opacity:1
            text:modA.code+" "+modA.message
            wrapMode: Text.Wrap
            horizontalAlignment : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:mouseHelp
            anchors.fill:parent

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: rootAlarm.state==="opened"?closeHelp():openHelp()
        }
    }

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:helpRec
        color:"white"
        radius:5
        anchors.margins: 5
        anchors.top:mexRec.bottom
        anchors.left:rootAlarm.left
        anchors.right:rootAlarm.right
        anchors.bottom:rootAlarm.bottom
        clip:true

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        Text{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:mexHelp
            visible:helpRec.height>0?true:false
            color:"black"
            font.family:  (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
            font.bold: true
            font.pixelSize: 12
            anchors.fill:parent
            opacity:1
            text:modA.help
            wrapMode: Text.Wrap
            horizontalAlignment : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

    }

}
