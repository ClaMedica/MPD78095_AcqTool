import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import MComponents 1.0
import "Images"

Rectangle {
    id: rootComboBox
    property string currentText: ""
    property int currentIndex: -1
    property var model: []
    property int oriH: height
    property int labelSize: 2
    property var lastSelected: []
    property bool opened: false
    property int numElementiMax: 5
    readonly property bool modelOk: (model.length != 0) && (model[0] !== undefined)
    clip: true
    Behavior on height { NumberAnimation { duration:300; easing.type: Easing.OutExpo}}
    border.width: screenH*0.005
    onModelChanged:        if(modelOk) currentIndex = 0
    onCurrentIndexChanged: if(modelOk && model[currentIndex] !== undefined) curEle.text = model[currentIndex]

    signal clicked

    function find(text)
    {
        if(model === undefined || model === null)
            return 0
        if(model.indexOf(text) < 0)
            return 0
        return model.indexOf(text)
    }

    function openBox()
    {
        if(model.length == 0 || opened)
            return
        //console.log("open box")
        oriH = height
        //se è ancorato si espanderà dove può
        if(model.length > numElementiMax)
            rootComboBox.height *= numElementiMax
        else
            rootComboBox.height *= model.length
        arrow.visible = false
        view.model = rootComboBox.model
        view.visible = true
        curEle.visible = false
        DataEngine.putItemOnTop(rootComboBox)
        opened = true
        rootComboBox.clicked()
    }

    function closeBox(index,text)
    {
        if(!opened)
            return
        currentIndex = index
        currentText = text
        curEle.text = text
        view.visible = false
        view.model = []
        rootComboBox.height = oriH
        arrow.visible = true
        curEle.visible = true
        opened =false
    }

    Timer {
        id: autoClose
        interval: 2000
        onTriggered: closeBox(lastSelected[0], lastSelected[1])
    }

    MLabel {
        id:curEle
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: rootComboBox.border.width*2
        height: oriH
        labelSize: rootComboBox.labelSize
        text:""
        //horizontalAlignment: text.length>5?Text.AlignLeft:Text.AlignHCenter
    }

    ListView{
        id:view
        anchors.fill: parent
        delegate:MLabel {
            y:index*height
            height:oriH
            width:parent.width
            labelSize: rootComboBox.labelSize
            text:modelData
            horizontalAlignment: text.length>10?Text.AlignLeft:Text.AlignHCenter
            BuzzMouseArea {
                hoverEnabled: true
                anchors.fill: parent
                onEntered: {
                    autoClose.stop()
                    highlight.opacity=0.5
                    lastSelected=[index,modelData]
                }
                onExited:
                {
                    autoClose.start()
                    highlight.opacity=0
                }
                onClicked:closeBox(index,modelData)
            }
            Rectangle {
                id:highlight
                Behavior on opacity{NumberAnimation{duration:100; easing.type: Easing.OutExpo}}
                anchors.fill: parent
                color:"orange"
                opacity:0
                radius:5
            }
        }
    }
    Rectangle {
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

    BuzzMouseArea{
        id:area
        enabled: arrow.visible
        anchors.fill: parent
        onClicked: openBox()
    }
}


