import QtQuick 2.5
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
Rectangle {
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string theme
    property int selectedMenu: -1
    property int menuWidth: 100
    property int buttonHeight:Screen.height*0.1
    property string itemClicked:"none"
    property var items:[]
    property var titles:[]
    property bool completed:false
    signal selected

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id: rootBar;
    color: "transparent"

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function popola()
    {
        for(var i=0;i<items.length;i++){

            var g=items[i][1]
            mod.append({"lista":g,
                           "titolo":items[i][0].toString(),
                           "stato":"chiuso",
                           "numero":i});

        }
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:complete.start(10)
    onItemsChanged: {popola();}
    onSelectedMenuChanged:
    {
        for(var k=0;k<mod.count;k++)
        {
            var pX=mposXY.get(k).posX
            var pY=mposXY.get(k).posY
            var state=mod.get(k).stato
            if(k!=selectedMenu && state==="aperto")
            {
                listBar.itemAt(pX,pY).chiudi()
            }
        }
        if(selectedMenu==-1)
            listBar.height=30
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Timer{id:complete;onTriggered:completed=true;}

    ListModel{id:mod}

    Repeater{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id: listBar
        anchors.top: rootBar.top
        anchors.left:rootBar.left
        anchors.right:rootBar.right
        height:30
        model:items
        delegate: MMenu{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:figlio
            y:0
            x:width*index
            width:rootBar.width/listBar.count
            color:"red"
            type:"main"
            items:modelData
            height:listBar.height
            state:"chiuso"
            number:index
            buttonHeight: rootBar.buttonHeight
            theme:rootBar.theme

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onSelected:
            {
                rootBar.itemClicked=figlio.itemClicked
                rootBar.selected()
                listBar.height=30
            }
        }

    }

    Timer{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:tim_close_all
        interval: 100

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onTriggered: selectedMenu=-1
    }
}







