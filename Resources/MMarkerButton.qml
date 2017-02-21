import QtQuick 2.0
import MPlotModule 1.0
import QtQuick.Controls 1.4

import "qrc:/Components"
import MComponents 1.0
import "qrc:/Models"
Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property int oriX:0
    property int oriY:0
    property int oriW:0
    property int oriH:0
    property real zoom:0
    property var modello:[]
    signal click(var value)
    signal tooltipActive(var testo, var posY)

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootMarkerButton
    x:oriX-oriW/2*zoom
    y:oriY-oriH/2*zoom
    height:oriH+zoom*oriH
    width:oriW+zoom*oriW

    clip:true
    state:"nul"
    states:[
        State {name: "nul"; },
        State {name: "hov"; PropertyChanges{target: img; imageName: modM.img}
            PropertyChanges{target: rootMarkerButton; zoom: 0.2 }
            PropertyChanges{target: toolTip; font.pixelSize: 12}},
        State {name: "idl"; PropertyChanges{target: img; imageName: modM.img}
            PropertyChanges{target: rootMarkerButton; zoom: 0}},
        State {name: "pre"; PropertyChanges{target: img; imageName: modM.img+"_click" }
            PropertyChanges{target: rootMarkerButton; zoom: 0.2}},
        State {name: "dis"; PropertyChanges{target: img; imageName: modM.img+"_off" } },
        State {name: "mrk"; PropertyChanges{target: img; imageName: modM.img+"_16" } }

    ]
    Behavior on opacity { NumberAnimation { easing.overshoot: 5; easing.type: Easing.OutBack; duration: rootGrid.dur }}
    Behavior on zoom    { NumberAnimation {
            id:zoomAnimation
            easing.overshoot: 5
            easing.type: Easing.OutBack
            duration:400
        }}
    //@@@@@@@@@@    Events          @@@@@@@@@@
    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modM.setProperty(modello[i],modello[i+1])
            state="idl"

        }
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MarkerModel{id:modM}

    Image{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        property string imageName:""
        property int extCount:0
        property var extModel:[".bmp",".png",".jpg"]
        id:img
        anchors.fill:parent
        //modM.countVisible==0?parent.height:parent.height*0.6
        fillMode: Image.Stretch
        onImageNameChanged: source=imageName+extModel[extCount]
        onStatusChanged:
        {
            if (status === Image.Error)
            {
                extCount++
                if(extCount>=extModel.length)
                    console.log("finite le estensioni")
                source=imageName+extModel[extCount]
            }
        }
    }

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:frame
        color:"transparent"
        anchors.centerIn: parent
        width:parent.width+radius
        height:parent.height+radius
        border.color: "lightgray"//Qt.rgba(1,1,0.5,1)
        border.width:radius/2
        radius:parent.width/2
    }

    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:toolTip
        color:"black"
        font.family: (layout !== undefined) ? layout.value("FFamily") : "ubuntu"
        font.bold: false
        font.pixelSize: 12
        anchors.top:img.top

        //        anchors.bottom: parent.bottom
        //        anchors.right: parent.right
        //        anchors.left: parent.right
        visible:false
        text:modM.descr
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }


    MouseArea{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:area
        anchors.fill: parent
        hoverEnabled: true

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onEntered:  {
            if(!isTouch){
                rootMarkerButton.state="hov"
                tooltipActive(modM.descr,rootMarkerButton.y)
            }
        }
        onReleased: {
            //if(!isTouch)
                rootMarkerButton.state="hov"
        }
        onExited:    {
            if(!isTouch){
                rootMarkerButton.state="idl"
                tooltipActive("",rootMarkerButton.y)
            }
        }
        onPressed:  {
            //if(!isTouch)
                rootMarkerButton.state="pre"
        }
        onClicked:  {
            rootMarkerButton.state="idl";
            click(modM.key)
        }
    }

}

