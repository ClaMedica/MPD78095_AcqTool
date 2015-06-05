import QtQuick 2.0
import UI 1.0

Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string title:"Lista"
    property string type:"none" //indica il tipo degli elementi visualizzati
    property var elements:[]
    property string curText:""
    property string preText:""
    property int curIndex:-1
    property bool completed:false
    property bool dialogBtnVisible:true
    property bool enableNew:true
    property string owner:"none" //indica chi sta usando l'oggetto
    property int animDuration:1000
    property bool multipleChoice:false
    property var choices:[]
    property var selectedElements:[]
    property var preSelectedElements:[]
    property int itemHeight:30
    signal selected
    signal nuovo
    signal cancel

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootList
    color:"yellow"
    anchors.margins: 5
    radius:10
    visible:owner==""?false:true
    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function setCurIndex(index)
    {
        if(index<elements.length)
            curIndex=index
    }

    function add(it)
    {
        if(elements.indexOf(it)==-1)
        {
            var newEl=elements
            newEl[newEl.length]=it
            elements=newEl
            curIndex=elements.length-1
        }
        else
            curIndex=elements.indexOf(it)
    }

    function remove(it)
    {
        var newEl,index=0
        newEl=[]
        for(var i=0;i<elements.length;i++)
            if(elements[i]!==it)
            {
                newEl[index]=elements[i]
                index++
            }
        if(index==0)
        {
            elements=[]
            curIndex=-1
        }
        else
        {
            elements=newEl
            curIndex--
        }
    }
    //    function goNext()
    //    {
    //        if(list.currentIndex<mod.count-1)
    //        list.currentIndex++
    //    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    Keys.onPressed:
    {
        switch(event.key)
        {
        case Qt.Key_Up:if(curIndex>0)curIndex--;break;
        case Qt.Key_Down:if(curIndex<elements.length-1)curIndex++;break;
        default:console.log("ListSelector: Key not recognized")
        }
    }
    Component.onCompleted: completed=true
    onElementsChanged: if(completed){
                           curIndex=-1;
                           preText=curText;
                           curText="";
                           preSelectedElements=selectedElements;
                           selectedElements=[]
                           choices=[]
                           //console.log(selectedElements,preSelectedElements)
                       }
    onCurIndexChanged: if(curIndex!=-1){preText=curText;curText=elements[curIndex]}

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
        text:rootList.title
    }

    ListModel{id:mod}

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
        Rectangle{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:high
            width:listRect.width
            height:itemHeight
            x:0
            y:(curIndex-list.scorr)*itemHeight
            color: "lightsteelblue"
            radius: 5;
            visible:!multipleChoice
            Behavior on y {NumberAnimation { duration: animDuration; easing.type: Easing.OutElastic  }}
        }

        Repeater{
            //@@@@@@@@@@    Definitions     @@@@@@@@@@
            property int scorr:0

            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:list
            anchors.fill:parent
            anchors.margins: 5
            model: elements
            delegate:item
            focus:true
        }

        Component{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:item

            //@@@@@@@@@@    Objects         @@@@@@@@@@
            Item{
                //@@@@@@@@@@    Objects         @@@@@@@@@@
                Rectangle {
                    //@@@@@@@@@@    Properties      @@@@@@@@@@
                    id:itemRect
                    width:listRect.width
                    height:itemHeight
                    x:0
                    y:index>=0?(index-list.scorr)*itemHeight:0
                    color: "lightskyblue"
                    border.color: "deepskyblue"
                    border.width: 2
                    radius: 5;
                    visible:multipleChoice
                    opacity:0
                    Behavior on opacity {NumberAnimation { duration: animDuration/2; easing.type: Easing.Linear  }}
                    Behavior on y {NumberAnimation { duration: animDuration/2; easing.type: Easing.Linear  }}
                }
                Text{
                    //@@@@@@@@@@    Properties      @@@@@@@@@@
                    id:itemText
                    anchors.left:itemRect.left
                    anchors.right:itemRect.right
                    anchors.verticalCenter: itemRect.verticalCenter
                    anchors.margins: 5
                    text:index>=0?modelData:""
                    height: itemHeight-10
                    font.bold: true
                    font.pixelSize: 14

                    //@@@@@@@@@@    Objects         @@@@@@@@@@
                    MouseArea{
                        //@@@@@@@@@@    Properties      @@@@@@@@@@
                        id:area
                        anchors.fill:parent;

                        //@@@@@@@@@@    Events          @@@@@@@@@@
                        onClicked:{
                            rootList.focus=true;
                            curIndex=index
                            //console.log("clicco",curIndex,curText)
                            itemRect.opacity=!itemRect.opacity
                            if(multipleChoice)
                                choices[index]=!choices[index]

                        }
                        onWheel:{
                            //console.log(wheel.angleDelta.y,list.scorr)
                            //se è negativo devo scendere
                            if(elements.length*itemHeight>listRect.height)
                            {
                                if(wheel.angleDelta.y<0)
                                {
                                    if(list.scorr<elements.length-1)
                                        list.scorr++
                                }
                                else
                                {
                                    if(list.scorr>0)
                                        list.scorr--
                                }
                            }

                        }
                    }
                }
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

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:select
            anchors.top:parent.top
            anchors.right:parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5
            radius:10
            text:"Select"
            width:parent.width/3-10
            visible:curIndex!==-1?true:false
            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {
                if(multipleChoice)
                {
                    preSelectedElements=selectedElements
                    var scelte=[]
                    for(var i=0;i<choices.length;i++)
                        if(choices[i])
                            scelte[scelte.length]=elements[i]
                    selectedElements=scelte
                    //console.log(elements,selectedElements,preSelectedElements)
                }


                rootList.selected()}
        }

        MMenuButton{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:nuovo
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.margins: 5
            radius:10
            text:"New"
            width:parent.width/3-10
            visible: enableNew

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {rootList.nuovo()}            
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
            width:parent.width/3-10

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onClicked: {curIndex=-1;choices=[];owner="";rootList.cancel();rootList.visible=false}
        }
    }


}

