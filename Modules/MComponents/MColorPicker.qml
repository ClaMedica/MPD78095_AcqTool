import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import MComponents 1.0
Rectangle {
    id:rootColorPicker
    property bool builded:false
    property alias choosedColor:recChoosed.color
    property color beginColor
    readonly property int steps:24
    property var boardEle:[]
    property var caller
    signal colorSelected

    function erase()
    {
        this.destroy()
    }

    function build()
    {
        //buildo la barra
        var i
        var model=[]
        for(i=0;i<steps;i++)
            model.push(Qt.hsla(i/steps,1,0.5,1))
        repBar.model=model
        buildMap(1)

        builded=true

    }
    function buildMap(offset)
    {
        repBoard.model=0
        console.log(offset)
        var model=[]
        //buildo la mappa
        for(var i=0;i<steps;i++)
            for(var j=0;j<steps;j++)
                model.push(setColor(offset,i,j))
        repBoard.model=model
    }

    function colorMap(offset)
    {

        //ricoloro la mappa
        for(var i=0;i<steps;i++)
            for(var j=0;j<steps;j++){
                var index=i*steps+j
                boardEle[index].color=setColor(offset,i,j)
            }

    }

    function setColor(offset,i,j){
        return Qt.hsla(offset/steps,(1-i/(steps-1))*(1-j/(steps-1)),(0.5+(j/(steps-1))/2)*(1-i/(steps-1)),1)
    }

    Row{
        anchors.fill: parent
        spacing:width*0.05

        Column{
            spacing:height*0.05
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width:parent.width*0.3

            Rectangle{
                id:recChoosed
                height:parent.height*0.4
                anchors.left: parent.left
                anchors.right: parent.right
                border.color: "black"
                border.width: 3
                color:beginColor
                MouseArea{
                    id:areaClick
                    anchors.fill:parent
                    onClicked: {
                        choosedColor=beginColor
                        colorSelected()
                    }
                }
            }

            MLabel{
                id:label
                height:parent.height*0.25
                anchors.left: parent.left
                anchors.right: parent.right
                text:qsTr("Color")
            }
            MLabel{
                id:parColor
                height:parent.height*0.25
                anchors.left: parent.left
                anchors.right: parent.right
                text:recChoosed.color
            }
        }

        Column{
            id:colorBar
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width:parent.width*0.2
            Repeater{
                id:repBar
                model:["red","green","blue"]
                delegate:
                    Rectangle{
                    id:recSelBar
                    property int indexx:index
                    height:colorBar.height/steps
                    width: colorBar.width
                    color:modelData
                }
            }
        }

        Grid{
            id:colorBoard
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width:parent.width*0.4
            columns: steps
            Repeater{
                id:repBoard
                delegate:
                    Rectangle{
                    id:recSelBoard
                    height: colorBoard.height/steps
                    width: colorBoard.width/steps
                    color:modelData
                    Component.onCompleted: boardEle.push(this)
                }
            }
        }
    }
    MouseArea{
        id:areaSelBoard
        x:colorBoard.x
        y:colorBoard.y
        height:colorBoard.height
        width:colorBoard.width
        hoverEnabled: true
        onPositionChanged: {
            var sub=colorBoard.childAt(mouseX,mouseY)
            recChoosed.color=sub.color
        }

        onClicked: colorSelected()
    }


    MouseArea{
        id:areaSelBar
        x:colorBar.x
        y:colorBar.y
        height:colorBar.height
        width:colorBar.width
        hoverEnabled: true
        onPositionChanged: {
            var sub=colorBar.childAt(mouseX,mouseY)
            colorMap(sub.indexx)
        }
    }
}


