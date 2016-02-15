import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
 


Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property int itemsInRow:4
    property bool displayOnCreation:true
    property int itemZoom: 10
    property Component delegate
    property string owner:"none"
    property var currentItem
    property var items:[]
    signal clicked(var value)

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootGrid
    color:"lightgray"//Qt.rgba(1,1,0.5,1)

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function display()
    {
        tim.start()
    }

    function modify()
    {//modifico elemento per elemento
        //console.log("modify",it)
        if(items.length>0)
        {
            var curMod=[]
            var index=0
            var elementIndex=0

            for(var i=0;i<items.length;i++)
            {
                if(items[i]==="$GridElement")
                {
                    curMod=[];
                    index=0;
                    continue;
                }
                if(items[i]==="&GridElement")
                {
                    //console.log("element index",markerIndex,l)
                    rep.itemAt(elementIndex).modello=curMod
                    elementIndex++
                    continue
                }
                //inizia la lettura
                curMod[index]=items[i]
                index++
            }
        }
    }

    function populate()
    {
        var eCount=0
        var ll=items.length
        for(var i=0;i<ll;i++)
            if(items[i]==="$GridElement")
                eCount++

        if(eCount>0)
        {
            rep.model=eCount
            modify()
            moveAndResize()
            console.log("popolato con ",eCount," elements")
        }
    }

    function moveAndResize()
    {//smista gli elementi
        var row=0
        var col=0
        var w=rootGrid.width
        var n=itemsInRow
        var itemSize=(w/n)/(1+(n+1)*0.2/n)
        var d=itemSize*0.2
        for(var i=0;i<rep.count;i++)
        {

            if(i>(row+1)*itemsInRow-1)
            {
                row++
                col=0
            }

            rep.itemAt(i).oriW=itemSize
            rep.itemAt(i).oriH=itemSize
            rep.itemAt(i).oriX=d+(d+itemSize)*col
            rep.itemAt(i).oriY=d+(d+itemSize)*row
            rep.itemAt(i).click.connect(clicked)
           // console.log("Elemento ",i,rep.itemAt(i).width,rep.itemAt(i).height,rep.itemAt(i).x,rep.itemAt(i).y)
            col++
        }

        //property int itemSize:(rootGrid.width-(itemsInRow+1)*distanceBetweenItems)/itemsInRow
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Component.onCompleted:{if(items.length!==0)populate()}
    onItemsChanged: populate()



    //@@@@@@@@@@    Objects         @@@@@@@@@@


    Repeater{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:rep
        anchors.fill:parent
        delegate:rootGrid.delegate
    }
}
