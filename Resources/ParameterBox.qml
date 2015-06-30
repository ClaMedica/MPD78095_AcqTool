import QtQuick 2.0
import Managers 1.0
import QtQuick.Controls 1.0
import Resources 1.0
import MPlotModule 1.0

Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property var items:[]
    property var valueNews:[]
    property bool completed:false
    property bool compressed: false
    property int itemHeight: 60
    property bool ready: false
    property int boxNumber:0
    property string currentAna:"none"
    signal close
    signal update

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootBox
    color:Qt.rgba(1,1,1,0.5)
    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function getItem(index,field)
    {
        var str=items
        var v=items[field+index*nFields]
        return v
    }

    function popola()
    {
        boxNumber=0
        var num=0
        for(var i=0;i<items.length;i++)
            if(items[i]==="$Parameter")
                num++
        boxNumber=num
        if(boxNumber>0)
            modify(items);
    }

    function modify(it)
    {
        if(it.length>0)
        {
            var l=[]
            var index=0
            var windowIndex=0

            for(var i=0;i<it.length;i++)
            {
                if(it[i]==="$Parameter")
                {
                    l=[];
                    index=0;
                    continue;
                }
                if(it[i]==="&Parameter")
                {
                    listPar.itemAt(windowIndex).modello=l
                    windowIndex++
                    continue
                }
                //inizia la lettura
                l[index]=it[i]
                index++
            }
        }
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    onItemsChanged: if(completed)popola()
    Component.onCompleted:goTim.start(10)

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Timer{id:goTim;onTriggered: completed=true}

    ParameterModel{id:mod}

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:box
        anchors.centerIn: parent
        width: parent.width-40
        height:itemHeight*boxNumber+btnMinimize.height
        color:"lightgrey"
        radius:20

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        Repeater{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:listPar
            model:boxNumber
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: btnMinimize.top

            //@@@@@@@@@@    Objects         @@@@@@@@@@
            Parameter{
                //@@@@@@@@@@    Properties      @@@@@@@@@@
                id:pa
                x:0
                y:height*index
                clip:true
                width:listPar.width
                height: itemHeight
                compressed: rootBox.compressed

                //@@@@@@@@@@    Events          @@@@@@@@@@
                onValueChanged: {
                    if(pa.ready)
                        valueNews=[currentAna,pa.name,pa.value]
                }
            }
        }

        MButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:btnMinimize
            anchors.bottom: parent.bottom
            height:40
            anchors.left:parent.left
            anchors.right:parent.right
            text:">>>"
            showImage: false

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {
                if(text===">>>")
                {
                    text="Close"
                    rootBox.update()
                    rootBox.width=rootBox.width/2
                    rootBox.compressed=true
                }
                else
                {
                    text=">>>"
                    rootBox.close()
                    rootBox.width=0
                }
            }
        }
    }

}

