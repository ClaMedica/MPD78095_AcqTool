import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import MComponents 1.0
import "Images"
Rectangle{
    id:rootComboBox
    property string currentText:"Courier"
    property int currentIndex:0
    readonly property var model:layout.fontFamilies()
    property int oriH: height
    property int labelSize:2
    readonly property bool modelOk: model.length !=0 && model[0]!=undefined
    clip:true
    height:100
    width:300
    Behavior on height{NumberAnimation{duration:300; easing.type: Easing.OutExpo}}
    border.width: screenH*0.005
    onModelChanged: if(modelOk)curEle.text=model[0]
    onCurrentIndexChanged: if(modelOk)curEle.text=model[currentIndex]

    function find(text){
        if(model === undefined || model === null)
            return 0
        if(model.indexOf(text)<0)
            return 0
        return model.indexOf(text)
    }

    function openBox()
    {
        //console.log("open box")
        oriH=height
        //se è ancorato si espanderà dove può
        if(model.length>5)
            rootComboBox.height*=5//model.length
        else
            rootComboBox.height*=model.length
        arrow.visible=false
        view.model=rootComboBox.model
        view.visible=true
        curEle.visible=false
        DataEngine.putItemOnTop(rootComboBox)
    }

    function closeBox(index,text)
    {
        //console.log("close box")
        currentIndex=index
        currentText=text
        curEle.text=text
        view.visible=false
        view.model=[]
        if(model.length>5)
            rootComboBox.height/=5//model.length
        else
            rootComboBox.height/=model.length
        arrow.visible=true
        curEle.visible=true
    }

    MLabel {
        id: curEle
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: rootComboBox.border.width
        height: oriH
        labelSize: rootComboBox.labelSize
        text: font.family
        horizontalAlignment: (text.length > 5) ? Text.AlignLeft : Text.AlignHCenter
    }

    ListView{
        id:view
        anchors.fill: parent
        delegate:MLabel{
            y:index*height
            height:oriH
            width:parent.width
            labelSize: rootComboBox.labelSize
            text:modelData
            font.family: modelData
            horizontalAlignment: text.length>5?Text.AlignLeft:Text.AlignHCenter
            MouseArea{
                hoverEnabled: true
                anchors.fill: parent
                onEntered: highlight.opacity=0.5
                onExited: highlight.opacity=0
                onClicked:closeBox(index,modelData)
            }
            Rectangle{
                id:highlight
                Behavior on opacity{NumberAnimation{duration:100; easing.type: Easing.OutExpo}}
                anchors.fill: parent
                color:"orange"
                opacity:0
                radius:5
            }
        }
    }
    Rectangle{
        id:arrow
        anchors.right:parent.right
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        width:height
        color: "transparent"
        anchors.margins: rootComboBox.border.width-1

        Image{
            Behavior on opacity{NumberAnimation{duration:200; easing.type: Easing.OutExpo}}
            opacity: arrow.visible
            source:"arrow.png"
            anchors.fill: parent

        }
    }

    MouseArea{
        id:area
        enabled: arrow.visible
        anchors.fill: parent
        onClicked: openBox()
    }
}

