import QtQuick 2.2
import QtQuick.Controls 1.0

Rectangle {
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string title: "Menu"
    property string type: "main"
    property int number: 0 //identificativo
    property var items:[]
    property string itemClicked:"none"
    property string theme
    property bool completed:false
    property real openClose:1
    signal selected
    signal clicked
    signal entered
    signal exited
    signal bodyBtnEntered
    signal body_btn_exited

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: rootMenu;
    states:[
        State{name:"chiuso";PropertyChanges{target:rootMenu;openClose: 0}},
        State{name:"aperto";PropertyChanges{target:rootMenu;openClose: 1}}
    ]
    Behavior on openClose {NumberAnimation{duration:250;}}

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function popola(){
        listMnu.model=items.length

    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:timCom.start(10)

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Timer{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:timCom;

        onTriggered:{completed=true;popola();}
    }


    Repeater {
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: listMnu
        anchors.fill:parent
        delegate:
            MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            anchors.horizontalCenter: rootMenu.horizontalCenter
            y:index===0?0:height+height*(index-1)*openClose
            width:index===0?rootMenu.width:rootMenu.width*openClose
            height:30
            text:items[index]
            opacity:index===0?1:openClose
            enabled:(index===0||y>=height)?1:0
            theme: index===0?rootMenu.theme:"blue"
            mechAction: index===0?"latch":"switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {
                rootMenu.itemClicked=text
                if(index===0)
                {
                    if(rootMenu.state==="aperto")
                        rootMenu.state="chiuso"
                    else
                        rootMenu.state="aperto"
                }
                else
                    rootMenu.selected()
            }
            onEntered:
            {
                if(rootMenu.state==="aperto")
                    closeMe.stop()
            }
            onExited:
            {
                if(rootMenu.state==="aperto")
                    closeMe.start()
            }
        }
    }

    Timer{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:closeMe;
        interval:100

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onTriggered:{
            rootMenu.state="chiuso";
            listMnu.itemAt(0).state="idle";
            listMnu.itemAt(0).selected=false
        }

    }
}
