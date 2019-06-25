import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3
import MPlotModule 1.0
 

Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property bool compressed:false
    property bool sliding: false
    property bool spinning: false
    property bool ready:false
    property real factor:4
    property alias value:modP.val
    property alias name:modP.name
    property alias label:modP.label
    property var modello:[]

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootPar
    height:50
    border.width: 2
    radius:10
    color:"lightblue"
    clip:true
    Behavior on factor {NumberAnimation { duration: 1000 }}


    onModelloChanged:
    {
        //questo modello carica una stringa composta dalla role e poi dal valore
        //console.log("modello parameter",modello)
        if(modello.length%2===0 && modello.length>=2)
        {
            for(var i=0;i<modello.length;i+=2)
                modP.setProperty(modello[i],modello[i+1])
        }
        if(modP.values.length>1)
        {
            spin.visible=false
            slider.visible=false
            combo.visible=true

        }
        else
        {
            spin.visible=true
            slider.visible=true
            combo.visible=false
            spin.value=modP.val
            slider.value=modP.val
        }
        //console.log(x,y,width,height,rec.height)
        //console.log("I'm cursor n°",number,modM.number)
    }



    onCompressedChanged: factor=compressed?2:4
    Component.onCompleted:
    {
        spin.value=modP.val
        ready=true
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    ParameterModel{id:modP}
    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:label
        anchors.margins: 3
        anchors.left:rootPar.left
        anchors.top:rootPar.top
        height: rootPar.height/2
        width:rootPar.width-spin.width-10
        text:modP.label
        font.bold: true
        elide: Text.ElideRight
        font.pixelSize: compressed?12:16
        Behavior on font.pixelSize {NumberAnimation { duration: 1000 }}
        color:"red"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    ComboBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:combo
        anchors.right:rootPar.right
        anchors.top: rootPar.top
        anchors.bottom: slider.top
        anchors.margins: 5
        Behavior on height {NumberAnimation { duration: 1000 }}
        width:rootPar.width/factor
        model:modP.values
        style:
            ComboBoxStyle{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            label:
                Label {
                //@@@@@@@@@@    Properties      @@@@@@@@@@
                verticalAlignment: Qt.AlignVCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                text: control.currentText
                color: !control.enabled ? "#DADAD9" : "#6F6E6E"
                anchors.fill: parent
                font.pixelSize: 12
                font.family:
                {
                    if (PicoFlow)
                        if (layout !== undefined)
                            rootLabel.font.family = layout.value("FFamily")
                        else
                            rootLabel.font.family ="utopia"
                    else if (layout !== undefined)
                        rootLabel.font.family = layout.value("FFamilyW")
                    else
                        rootLabel.font.family = "Calibri"
                }
            }
            selectedTextColor: "steelblue"
        }


        onCurrentIndexChanged: rootPar.value=modP.values[combo.currentIndex]
    }

    SpinBox{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:spin
        clip: true
        anchors.right:rootPar.right
        anchors.top: rootPar.top
        anchors.bottom: slider.top
        anchors.margins: 5
        Behavior on height {NumberAnimation { duration: 1000 }}
        width:rootPar.width/factor
        stepSize: modP.stepSize
        maximumValue: modP.max
        minimumValue: modP.min
        font.bold: true
        font.pixelSize: 16
        decimals: Math.log(1/modP.stepSize)/Math.log(10)
        horizontalAlignment: Text.AlignHCenter

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onFocusChanged: {
            spinning=focus;
            sliding=false;
            if(focus===false)
                modP.val=spin.value
            //console.log(cmn++,"spin",spin.focus,spinning,sliding,rootPar.stepSize,rootPar.max,rootPar.min);
        }
        onValueChanged: if(spinning)slider.value=spin.value
    }

    Slider{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:slider
        anchors.margins: 5
        anchors.left:rootPar.left
        anchors.right: rootPar.right
        anchors.bottom: rootPar.bottom
        anchors.top:label.bottom
        stepSize: modP.stepSize
        maximumValue: modP.max
        minimumValue: modP.min
        activeFocusOnPress: true

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onPressedChanged: {
            sliding=pressed;
            spinning=false;
            if(pressed===false)
                modP.val=slider.value

        }
        onValueChanged: if(sliding)spin.value=slider.value
    }

}
