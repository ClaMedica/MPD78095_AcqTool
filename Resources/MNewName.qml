import QtQuick 2.0
 
import "qrc:/Components"
Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string title:"Inserisci"
    property string type:"none" //tipo a cui assegnare un nome
    property string curText:edit.text
    property bool completed:false
    property bool dialogBtnVisible:true
    property string owner:"none" //chi sta usando l'oggetto
    signal selected    

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootNewName
    color:"yellow"
    anchors.margins: 5
    radius:10
    visible:owner==""?false:true
    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function setDefText(text)
    {
        edit.text=text
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted: completed=true


    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:title
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.top: parent.top
        anchors.margins: 5
        height: 20
        font.bold: true
        font.pixelSize: 18
        text:rootNewName.title
    }


    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:listRect
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.top: title .bottom
        anchors.bottom: btnRect.top
        anchors.margins: 5
        radius:10
        clip:true

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        TextInput{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:edit
            anchors.fill:parent
            anchors.margins: 5
            focus:true


            MouseArea{
                //@@@@@@@@@@    Definitions     @@@@@@@@@@
                property var curType

                //@@@@@@@@@@    Properties      @@@@@@@@@@
                anchors.fill: parent
                hoverEnabled: true
                propagateComposedEvents: true

                //@@@@@@@@@@    Events          @@@@@@@@@@
                onEntered:{curType=cursorShape;cursorShape=Qt.IBeamCursor}
                onExited:{cursorShape=curType}
                onClicked: edit.focus=true                
            }
        }
    }

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnRect
        height:dialogBtnVisible?40:0
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.bottom: parent.bottom
        color:parent.color
        visible: dialogBtnVisible
        radius:10

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:select
            anchors.top:parent.top
            anchors.right:parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5
            radius:10
            text:"OK"
            width:parent.width/2-10
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {rootNewName.selected()}
        }
        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:cancel
            anchors.top:parent.top
            anchors.left:parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 5
            radius:10
            text:"Cancel"
            width:parent.width/2-10
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: owner=""
        }
    }

}

