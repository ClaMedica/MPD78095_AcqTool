import QtQuick 2.0
import Resources 1.0
Rectangle  {    
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string text: "Button"
    property string background: ""
    property string whoIsVisilbe:""
    property string name:"Form"

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: container
    width: img_Background.width; height: img_Background.height

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted: console.log(name+" Ready!")

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Image{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: img_Background
        fillMode:  Image.PreserveAspectCrop
        source:container.background
        anchors.centerIn: container
        anchors.fill: container
    }
}
