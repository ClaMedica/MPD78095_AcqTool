import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
//import Resources 1.0
import "qrc:/Components"
Rectangle {
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property var plotList:[]
    property var trackList:[]
    property var dataList:[]
    property var news:[]
    signal ready

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootPlotDialog
    color:"red"
    visible:false    

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function lookAndPick(str){
        var notFound
        for(var i=0;i<str.length;i++){
            notFound=true
            for(var j=0;j<dataList.length;j++)
                if(dataList[j]===str[i])
                    notFound=false;
            if(notFound)
                dataList[dataList.length]=str[i]
        }
    }

    function popola(){
        var i
        if(dataList.length>0){
            var l=[]
            var index=0
            var where
            var a=dataList

            for(i=0;i<dataList.length;i++){
                if(dataList[i]==="$Group"){
                    l=[];
                    continue;
                }
                if(dataList[i]==="&Group"){
                    repData.itemAt(index).elements=l
                    index++
                    continue
                }
                //inizia la lettura
                l[l.length]=dataList[i]

            }
        }

    }

    function updateModel(){
        var a=[]
        for(var i=0;i<repDraw.count;i++)
            a[a.length]=[plotList[i],repDraw.itemAt(i).elements]
        news=a;
        console.log("manda questo ",a)
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    onVisibleChanged: popola()

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MListSelector{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:tracks
        anchors.left:boxBtn.right
        anchors.right:parent.right
        anchors.top: parent.top
        height: 150
        title:"All Available Families"
        elements:trackList
        dialogBtnVisible:false

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onCurTextChanged: if(curText!="")repData.itemAt(curIndex).setCurIndex(0)
    }
    Repeater{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:repData
        anchors.left:boxBtn.right
        anchors.right:parent.right
        anchors.top: tracks.bottom
        anchors.bottom: btnOk.top
        model:trackList
        delegate:
            MListSelector{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            title:"Data of: "+modelData
            anchors.fill:repData
            visible:(tracks.curText==modelData)?true:false
            dialogBtnVisible: false
        }

    }

    Rectangle{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:boxBtn
        width:40
        anchors.top:parent.top
        anchors.bottom: btnOk.top
        anchors.horizontalCenter: parent.horizontalCenter
        color:"transparent"

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:addAll
            radius:10
            height:parent.height/4-10
            anchors.top: parent.top
            anchors.left:parent.left
            anchors.right:parent.right
            anchors.margins: 5
            theme: "red"
            text:"<<"
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked:{
                for(var i=0;i<plotList.length;i++)
                    repDraw.itemAt(i).add(tracks.curText+':'+repData.itemAt(tracks.curIndex).curText);
            }
        }

        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:add
            radius:10
            height:parent.height/4-10
            anchors.top: addAll.bottom
            anchors.left:parent.left
            anchors.right:parent.right
            anchors.margins: 5
            text:"<"
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked:{repDraw.itemAt(plots.curIndex).add(tracks.curText+':'+repData.itemAt(tracks.curIndex).curText);}            
        }

        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:remove
            radius:10
            height:parent.height/4-10
            anchors.top: add.bottom
            anchors.left:parent.left
            anchors.right:parent.right
            anchors.margins: 5
            text:">"
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: repDraw.itemAt(plots.curIndex).remove(repDraw.itemAt(plots.curIndex).curText)            
        }
        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:remAll
            radius:10
            height:parent.height/4-10
            anchors.bottom: parent.bottom
            anchors.left:parent.left
            anchors.right:parent.right
            anchors.margins: 5
            theme: "red"
            text:">>"
            mechAction: "switch"

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked:{
                for(var i=0;i<plotList.length;i++)
                {
                    var ele=repDraw.itemAt(i).elements
                    var cur=repDraw.itemAt(plots.curIndex).curText
                    if(repDraw.itemAt(i).elements.indexOf(cur)!==-1)
                        repDraw.itemAt(i).remove(cur);
                }
            }            
        }
    }

    MListSelector{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:plots
        anchors.left:parent.left
        anchors.right:boxBtn.left
        anchors.top: parent.top
        height: 150
        title:"All Available Plots"
        elements:plotList
        dialogBtnVisible:false
    }

    Repeater{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:repDraw
        anchors.left:parent.left
        anchors.right:boxBtn.left
        anchors.top: plots.bottom
        anchors.bottom: btnOk.top
        model:plotList
        delegate:
            MListSelector{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:trackList
            title:modelData+" Elements"
            anchors.fill:repDraw
            visible:(plots.curText==modelData)?true:false
            dialogBtnVisible: false
            elements:[]
        }
    }

    MMenuButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnOk
        anchors.bottom: parent.bottom
        height:40
        anchors.left:parent.left
        anchors.right:parent.right
        text:"OK"
        mechAction: "switch"

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: {updateModel();parent.visible=false;rootPlotDialog.ready();}        
    }

}
