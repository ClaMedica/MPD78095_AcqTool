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
    property var info: ""
    property var model //contiene un modello per il componente
    property real viewPerc:0.6 //indica quanto spazio occupa la label
    property bool labelFirst:true
    property int labelSize:5//punti percentuali dello schermo
    property string unit:""
    property bool created:false
    property bool editable:true
    property color labelColor: PicoFlow ? "white" : layout.value("textTable")//colore della role
    property var beginInfo:undefined//è il valore che ha la info la prima volta
    property bool keyboardAlfaNum: true
    property int comboNumMaxEle:5
    property string containerBorderColor: "transparent"
    property int maxText: 255
    property bool password: false
    property bool emptyVal: false
    readonly property int currentIndex:type===typComboBox && component!=undefined?component.currentIndex:-1
    readonly property string typTextField:"TextField"
    readonly property string typComboBox: "ComboBox"
    readonly property string typCheckBox: "CheckBox"
    readonly property string typCheckBoxMultiple: "CheckBoxMultiple"
    readonly property string typPathEdit: "PathEdit"
    readonly property string typDateEdit: "DateEdit"
    readonly property string typColorBox: "ColorBox"
    readonly property string typSpinBox:  "SpinBox"
    readonly property string typComboFont:"ComboFont"
    id:rootParEdit
    color:"transparent"
    anchors.margins: screenH*0.001
    onTypeChanged: reload()
    height:100
    width:300

    signal clicked
    signal lostFocusTarget

    onComponentChanged: if(component !== undefined) connection.target = component


    onEnabledChanged: {
        if(type === typTextField) {
            if (!enabled)
                component.textColor = "gray"
            else
                component.textColor = "black"
       }
   }

    function reload()
    {
        if(component !== undefined) {
            component.destroy()
            container.loadComponent()
        }
    }

    Connections {
        id:connection
        //target: viene assegnato dal rootParEdit in onComponentChanged
        ignoreUnknownSignals: true
        onInfoChanged:{
            if(target === undefined)
                return
            rootParEdit.info = connection.target.info
        }

        onFocusChanged:
        {
            if(target.focus)
            {
                //console.log("KEYBOARD")
                if (isTouch)
                {
                    mngSys.startSound()
                    var c
                    if (keyboardAlfaNum)
                        c = keyboard
                    else
                        c = keyboardNum

                    if (c !== undefined)
                    {
                        c.target = connection.target
                        c.labelTarget = label.text
                        DataEngine.putItemOnTop(c)
                        if (rootParEdit.type === typTextField) {
                            if (emptyVal)
                                c.testo = ""
                            else
                                c.testo = connection.target.text
                        }
                        if (password) c.password = true
                        c.show()
                        if (!keyboardAlfaNum) {
                            if (maxText < 255) {
                                c.maxChars = maxText
                                c.labelTarget += " " + maxText.toString() + qsTr(" DIGITS")
                            }
                        }
                    }
                    rootParEdit.clicked()
                }
                else
                {
                    if (keyboardAlfaNum)
                        component.onlyNumber = false
                    else
                        component.onlyNumber = true
                }
            }
            else
                lostFocusTarget()
        }
    }


    onModelChanged: if(created) loadModel(model)

    function setInfo(__info) {
        if(__info !== undefined)
        {
            if(created) {
                info = __info
                switch(type) {
                case typTextField       :component.text        =__info.toString();break;
                case typComboBox        :component.currentIndex=component.find(__info.toString());break;
                case typCheckBoxMultiple:component.checkedM = __info;break;
                case typCheckBox        :component.checked     = ((__info === "false") ||
                                                                   (__info === "0") ||
                                                                   (__info === "")) ? false : true;
                    break;
                case typPathEdit :component.fileURL     =__info;break;
                case typDateEdit :component.setDate(__info);break;
                case typColorBox :component.color       =__info;break;
                case typSpinBox  :component.value       =__info;break;
                case typComboFont:component.currentIndex = component.find(__info.toString());break;
                default:console.log("tipo errato",type);break;
                }
            }
            else
                reSetInfo(__info)
        }
        else
            console.log("info non definita")
    }

    Timer {
        id:timInfoReSet
        property var tinfo:undefined
        repeat: false
        interval: 10
        onTriggered: setInfo(tinfo)
    }
    function reSetInfo(r_info){
        timInfoReSet.tinfo=r_info
        timInfoReSet.start()
    }

    function loadModel(__model)
    {
        if(__model===undefined)
            return

        //console.log(role,"carico il modello",__model)

        switch(rootParEdit.type){
        case typTextField:
            component.placeholderText = model[0]
            if(model[1]!==undefined){
                component.validator = model[1]
            }
            setInfo("")
            break;
        case typComboBox:
            component.model=model
            component.labelSize=labelSize
            component.numElementiMax=comboNumMaxEle
            if (beginInfo === undefined) setInfo(model[0])
            break;
        case typCheckBox:
            break;
        case typCheckBoxMultiple:
            component.model = model
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
        case typSpinBox:
            //[30,0,1,0,""]
            component.maximumValue  =model[0]
            component.minimumValue  =model[1]
            component.step          =model[2]
            component.decimals      =model[3]
            component.suffix        =model[4]
            break;
        default:break;
        }

    }

    MLabel{
        id:label
        text:role
        anchors.top:parent.top
        anchors.left:labelFirst?parent.left:undefined
        anchors.right:labelFirst?undefined:parent.right
        anchors.bottom:parent.bottom
        anchors.margins: Screen.height*0.001
        width:Math.round(parent.width*viewPerc)
        labelSize: rootParEdit.labelSize
        color:labelColor
    }


    Rectangle{
        id: container
        anchors.left:labelFirst?label.right:parent.left
        anchors.right:labelSide.left
        anchors.top: parent.top
        anchors.bottom:parent.bottom
        color:"transparent"
        border.color: containerBorderColor
        border.width: 2
        enabled:editable

        Component.onCompleted: waitDim.start()

        Timer{
            id:waitDim
            interval:10
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
            case typTextField:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MTextField {property var info:text}',container);
                break;
            case typComboBox: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MComboBox {property var info:currentText}',container);break;
            case typCheckBox: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MCheckBox {property var info:checked}',container);
                c.text=""
                viewPerc=0.8;
                break;
            case typCheckBoxMultiple: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MCheckBoxMultiple {property var info:checkedM}',container);
                viewPerc=0.8;
                break;
            case typPathEdit: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MPathEdit{property var info:fileURL}',container);break;
            case typDateEdit: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MDateEdit{property var info:date}',container);break;
            case typColorBox: c=Qt.createQmlObject('import MComponents 1.0;
                                                    MColorBox{property var info:color}',container);break;
            case typSpinBox:  c=Qt.createQmlObject('import MComponents 1.0;
                                                    MSpinBox{property var info:value}',container);break;
            case typComboFont:c=Qt.createQmlObject('import MComponents 1.0;
                                                    MComboFont {property var info:currentText}',container);break;
            default:console.error("erroreeeee",rootParEdit.type)
            }

            c.anchors.margins = 2

            if(rootParEdit.type!==typComboBox && rootParEdit.type!==typComboFont)
                c.anchors.fill=container
            else
            {
                c.anchors.verticalCenter=container.verticalCenter
                c.height=container.height
                c.anchors.left=container.left
                c.anchors.right=container.right
            }

            if(c.labelSize !== undefined)
                c.labelSize=rootParEdit.labelSize

            c.clicked.connect(cliccato)

            if(type === typTextField) {
                c.maximumLength = maxText
                if (password)
                    c.echoMode = TextInput.Password
            }

            component=c
            created=true
            loadModel(rootParEdit.model)
            if(beginInfo!==undefined)
                setInfo(beginInfo)
        }
    }
    function cliccato()
    {
        rootParEdit.clicked()
    }

    MLabel{
        id:labelSide
        text:rootParEdit.unit
        anchors.top:parent.top
        anchors.right:labelFirst?parent.right:label.left
        anchors.bottom:parent.bottom
        anchors.margins: Screen.height*0.001
        width:text!=""?Math.round(parent.width*0.15):0
        labelSize: rootParEdit.labelSize
        horizontalAlignment: Text.AlignHCenter
        color:labelColor
    }

}

