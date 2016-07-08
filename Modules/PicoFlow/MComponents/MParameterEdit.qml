import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import MComponents 1.0
Rectangle {
    property string role:"none"
    property string type:typTextField
    property string style:"default"
    property var component:undefined
    property var info:""
    property var model //contiene un modello per il componente
    property real viewPerc:0.6 //indica quanto spazio occupa la label
    property bool labelFirst:true
    property int labelSize:2//punti percentuali dello schermo
    property string unit:""
    property bool created:false
    property bool editable:true
    property color labelColor:"black"//colore della role
    readonly property int currentIndex:type===typComboBox && component!=undefined?component.currentIndex:-1
    readonly property string typTextField:"TextField"
    readonly property string typComboBox:"ComboBox"
    readonly property string typCheckBox:"CheckBox"
    readonly property string typPathEdit:"PathEdit"
    readonly property string typDateEdit:"DateEdit"
    readonly property string typColorBox:"MColorBox"    
    id:rootParEdit
    color:"transparent"
    anchors.margins: Screen.height*0.001
    onTypeChanged: reload()
    height:100
    width:300
    onComponentChanged: if(component!==undefined)connection.target=component

    function reload()
    {
        if(component!==undefined){
        component.destroy()
        container.loadComponent()
        }
    }

    Connections{
        id:connection
        //target:rootParEdit.component
        ignoreUnknownSignals: true
        onInfoChanged:{
            if(component===undefined)
                return
            rootParEdit.info=connection.target.info
        }
    }

    onModelChanged: if(created)loadModel(model)

    function setInfo(__info){
        if(__info!==undefined)
            if(created){
                switch(type){
                case typTextField:component.text         =__info.toString();break;
                case typComboBox :component.currentIndex =component.find(__info.toString());break;
                case typCheckBox :component.checked     =__info;break;
                case typPathEdit :component.fileURL      =__info;break;
                case typDateEdit :component.setDate(__info);break;
                case typColorBox :component.color=__info;break;
                }
            }
    }

    function loadModel(__model)
    {
        if(__model===undefined)
            return

        //console.log(role,"carico il modello",__model)
        switch(rootParEdit.type){
        case typTextField:
            component.placeholderText = model[0]
            setInfo("")
            break;
        case typComboBox:
            component.model=model
            component.currentIndex=0
            component.labelSize=labelSize
            rootParEdit.info=component.currentText
            setInfo(model[0])
            break;
        case typCheckBox:
            component.text=""
            setInfo("0")
            break;
        case typPathEdit:
            component.folder=model[0]
            component.nameFilters=model[1]
            component.selectFolder=model[2]
            setInfo("")
            break;
        case typDateEdit:
            component.date=""
            setInfo(Date())
            break;
        }

        //console.log(rootParEdit.type,rootParEdit.role,component.model)
    }

    Label{
        id:label
        text:role
        anchors.top:parent.top
        anchors.left:labelFirst?parent.left:undefined
        anchors.right:labelFirst?undefined:parent.right
        anchors.bottom:parent.bottom
        anchors.margins: Screen.height*0.001
        width:Math.round(parent.width*viewPerc)
        font.pixelSize: labelSize*Screen.height*0.01
        font.bold:true
        verticalAlignment: Text.AlignVCenter
        color:labelColor
    }


    Rectangle{
        id: container
        anchors.left:labelFirst?label.right:parent.left
        anchors.right:labelSide.left
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        color:"transparent"
        border.color: "transparent"
        border.width: 2
        enabled:editable

        Component.onCompleted: waitDim.start()

        Timer{
            id:waitDim
            interval:100
            repeat: false
            onTriggered:{
                if(rootParEdit.height === 0 || rootParEdit.width === 0){
                    waitDim.start()
                    return
                }
                container.loadComponent()
            }
        }

        function loadComponent() {
            var c
            switch(rootParEdit.type){
            case typTextField:c=Qt.createQmlObject('import QtQuick.Controls 1.4;
                                                    TextField {property var info:text}',container);
                c.font.pixelSize=labelSize;
                break;
            case typComboBox:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MComboBox {property var info:currentText}',container);break;
            case typCheckBox:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MCheckBox {property var info:checked}',container);
                c.text=""
                viewPerc=0.8;
                break;
            case typPathEdit:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MPathEdit{property var info:fileURL}',container);break;
            case typDateEdit:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MDateEdit{property var info:date}',container);break;
            case typColorBox:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MColorBox{property var info:color}',container);break;
            default:console.error("erroreeeee",rootParEdit.type)
            }
            //c.anchors.centerIn=container
            //c.height=container.height
            //c.width=container.width
            if(type!==typComboBox)
                c.anchors.fill=container
            else
            {
                c.anchors.verticalCenter=container.verticalCenter
                c.height=container.height
                c.anchors.left=container.left
                c.anchors.right=container.right
            }
            component=c
            created=true
            loadModel(rootParEdit.model)
        }
    }


    Label{
        id:labelSide
        text:rootParEdit.unit
        anchors.top:parent.top
        anchors.right:labelFirst?parent.right:label.left
        anchors.bottom:parent.bottom
        anchors.margins: Screen.height*0.001
        width:text!=""?Math.round(parent.width*0.2):0
        font.pixelSize: labelSize
        font.bold:true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color:"white"
    }

}

