import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import MComponents 1.0

//import QtQuick.VirtualKeyboard 1.0

Rectangle{
    id:rootForm
    property string title:"title"
    property string name:"name"
    property string whoIsVisible:"none"
    property var keyboard:appKey
    property var keyboardNum:appKeyNum
    property var colorPicker: appColorPicker
    property var brothersVis:[]
    signal closing
    signal shown
    color:layout.value("BackgroundColor")
    height: parent.height
    width:  parent.width
    Component.onCompleted: console.log("Form "+name+" creata con successo")
    onWhoIsVisibleChanged:
    {
        if(whoIsVisible===name)
            show()
        else
            close()
    }

    function close()
    {
        closing()
        visible=false
        if(parent!==undefined && parent!==null && brothersVis!==undefined)
            for(var i=0;i<parent.children.length;i++)
                if(parent.children[i]!==this)
                    if(brothersVis[i])
                        parent.children[i].visible=true
    }

    function show()
    {
        shown()
        if(parent!==undefined)
            for(var i=0;i<parent.children.length;i++)
                if(parent.children[i]!==this)
                {
                    brothersVis.push(parent.children[i].visible)
                    parent.children[i].visible=false
                }
        visible=true
    }

    MKeyboard{
        id:appKey
        y: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false
    }

    MKeyboardNum {
        id: appKeyNum
        visible: false
    }

    MColorPicker {
        id: appColorPicker
        visible: false
        anchors.fill: parent
        Component.onCompleted: build()
    }

    //per la tastiera virtuale
 //   InputPanel {
//        id: inputPanel
//        z: 99
//        y: parent.height
//        anchors.left: parent.left
//        anchors.right: parent.right
//       // visible: false
//        states: State {
//            name: "visible"
//            when: Qt.inputMethod.visible
//            PropertyChanges {
//                target: inputPanel
//                y: parent.height - inputPanel.height
//            }
//        }
//        transitions: Transition {
//            from: ""
//            to: "visible"
//            reversible: true
//            ParallelAnimation {
//                NumberAnimation {
//                    properties: "y"
//                    duration: 150
//                    easing.type: Easing.InOutQuad
//                }
//            }
//        }
//    }
}
