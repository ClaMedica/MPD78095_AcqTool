import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import UI 1.0
import QtQuick.Controls.Styles 1.2

Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string message:"Hi I'm a Message"
    signal accepted
    signal rejected

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootMex
    width:200+mex.font.pixelSize*message.length/2
    height:100

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:mex
        height: font.pixelSize*2
        text:rootMex.message
        anchors.top: parent.top
        anchors.margins: font.pixelSize
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
        font.pixelSize: 20
    }

    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        width:parent.width/2-20
        y:75-height/2
        x:10+rootMex.width/2
        text:"Yes"        
        showImage: false

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: rootMex.accepted()
    }

    MButton {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        width:parent.width/2-20
        y:75-height/2
        x:10
        text:"No"
        showImage: false

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: rootMex.rejected()        
    }
}
