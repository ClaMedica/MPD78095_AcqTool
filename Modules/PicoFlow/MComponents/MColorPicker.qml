import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import "qrc:/GeneralData.js" as DataEngine
import MComponents 1.0 //import "qrc:/Components"
Window {
    id:rootColorPicker
    flags: Qt.Window|Qt.WindowTitleHint|Qt.WindowMinimizeButtonHint|Qt.WindowSystemMenuHint
    modality:Qt.WindowModal
    readonly property int recDim:2
    property bool builded:false
    property alias choosedColor:recChoosed.color
    property color beginColor
    property var board:[]
    property var bar:[]
    property int oriH:100
    property int oriW:200
    readonly property int steps:64
    signal colorSelected()
    minimumWidth: width
    minimumHeight:height
    maximumWidth: width
    maximumHeight:height
    title:qsTr("Choose a color...")
    x:Screen.width/2-width/2
    y:Screen.height/2-height/2


    function erase()
    {
        if(builded){
            for(var i=0;i<steps;i++){
                bar[i].destroy()
                for(var j=0;j<steps;j++)
                    board[i][j].destroy()
            }
        }
        bar=[]
        board=[]
        builded=false
        rootColorPicker.height=oriH+2*layout.value("Margin")
        rootColorPicker.width=oriW+2*layout.value("Margin")
        rootColorPicker.opacity=0
        close()
    }

    function build()
    {
        //buildo la barra
        var rec,i,j,perc,step,p,c,col
        for(i=0;i<steps;i++)
        {
            rec=Qt.createQmlObject('import QtQuick 2.4; Rectangle{}',colorBar)
            rec.height=recDim
            rec.width=colorBar.width

            rec.x=0
            rec.y=i*recDim

            rec.color=Qt.hsla(i/steps,1,0.5,1)
            bar[bar.length]=rec
        }


        //buildo la mappa
        for(i=0;i<steps;i++){
            col=[]
            for(j=0;j<steps;j++)
            {
                rec=Qt.createQmlObject('import QtQuick 2.4; Rectangle{}',colorBoard)

                rec.height=recDim
                rec.width=recDim

                rec.x=j*rec.width
                rec.y=i*rec.height

                setColor(1,rec,i,j)
                col[col.length]=rec
            }
            board[board.length]=col
        }
        builded=true
        rootColorPicker.height=steps*recDim+2*layout.value("Margin")
        rootColorPicker.width=oriW+colorBar.width+colorBoard.width+4*layout.value("Margin")
        rootColorPicker.opacity=1
    }

    function setColor(offset,rec,i,j){
        rec.color=Qt.hsla(offset,(1-i/(steps-1))*(1-j/(steps-1)),(0.5+(j/(steps-1))/2)*(1-i/(steps-1)),1)
    }
    Rectangle{
        id:recColorPicker
        height:parent.height
        width:parent.width
        clip:true
        color:layout.value("BackgroundColor")
        border.width: 2
        border.color: "black"

        Rectangle{
            id:recChoosed
            height:oriH
            width:oriW

            anchors.left:parent.left
            anchors.top:parent.top
            anchors.margins: layout.value("Margin")
            border.color: "black"
            border.width: 3
            MouseArea{
                id:areaClick
                anchors.fill:parent
                onClicked: {
                    choosedColor=beginColor
                    colorSelected()
                }
            }
        }

        Text{
            id:label
            anchors.left:parent.left
            anchors.top:recChoosed.bottom
            anchors.margins: layout.value("Margin")
            height:parent.oriH/3
            font.bold: true
            font.pixelSize: 20
            color:"white"
            text:qsTr("Color")
        }
        Text{
            id:parColor
            anchors.left:parent.left
            anchors.top:label.bottom
            anchors.bottom:parent.bottom
            anchors.margins: layout.value("Margin")
            font.bold: true
            font.pixelSize: 20
            color:"white"
            text:recChoosed.color
        }

        Rectangle{
            id:colorBoard
            anchors.left: colorBar.right
            anchors.top: parent.top
            anchors.margins: layout.value("Margin")
            width: steps*recDim
            height: steps*recDim
            visible: builded
            MouseArea{
                id:areaSelBoard
                anchors.fill:colorBoard
                hoverEnabled: true
                onPositionChanged: {
                    if(!builded)
                        return
                    var sub=colorBoard.childAt(mouseX,mouseY)
                    recSelBoard.x=sub.x
                    recSelBoard.y=sub.y
                    recChoosed.color=sub.color
                }
                onEntered: recSelBoard.visible=true
                onExited: recSelBoard.visible=false
                onClicked: colorSelected()
                z:steps*steps+1
            }
            Rectangle{
                id:recSelBoard
                radius: recDim/2
                height: recDim
                width: height
                visible:false
                z:steps*steps+2
                color:"transparent"
                border.width: 2
                border.color: "black"
                Rectangle{
                    radius: recDim/2
                    anchors.fill: parent
                    color:"transparent"
                    border.width: 1
                    border.color: "white"
                }
            }
        }

        Rectangle{
            id:colorBar
            anchors.left:recChoosed.right
            anchors.top:parent.top
            anchors.margins: layout.value("Margin")
            width:30
            height: steps*recDim
            visible: builded
            MouseArea{
                id:areaSelBar
                anchors.fill:colorBar
                hoverEnabled: true
                onPositionChanged: {

                    if(!builded)
                        return
                    var sub=colorBar.childAt(mouseX,mouseY)
                    recSelBar.y=sub.y
                    for(var i=0;i<steps;i++)
                        for(var j=0;j<steps;j++)
                            setColor(sub.y/(steps*recDim),board[i][j],i,j)
                }
                onEntered: recSelBar.visible=true
                onExited: recSelBar.visible=false
                z:steps+1
            }
            Rectangle{
                id:recSelBar
                height: recDim
                width: colorBar.width
                visible:false
                z:steps+2
                color:"transparent"
                border.width: 2
                border.color: "black"
                Rectangle{
                    anchors.fill: parent
                    color:"transparent"
                    border.width: 1
                    border.color: "white"
                }
            }
        }




    }
}

