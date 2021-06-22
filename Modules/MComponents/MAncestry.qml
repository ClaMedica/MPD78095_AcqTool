import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import MComponents 1.0
import Settings 1.0
import AcqModuleManager 1.0

Column {
    id:rootAncestry
    property var ancestry//si suppone che sia una classe ancestry altrimenti non funziona
    property string childName:"No name"
    property int itemHeight:40
    property real labelSize: PicoFlow ? 4 : 3
    property var father

    spacing: 21

    //onHeightChanged: console.log(height)
    onAncestryChanged: {
        if(ancestry !== undefined) {
            if(ancestry.getAttribute("label") === "")
                childName = ancestry.name()
            else
                childName = ancestry.getAttribute("label")
            btnOpen.opened = true
            destroyAncestry()
            recChildren.load()
            updateDimensions()
        }
    }

    function updateDimensions() {
        var newh = header.height
        //sistemiamo il recChildren
        var ele = recChildren.elements.length
        if(ele !== 0) {
            var recChH = 0
            for(var i = 0; i < ele; i++) {
                if (recChildren.elements[i].visible)
                    recChH += recChildren.elements[i].height + rootAncestry.spacing
            }
            recChildren.height = recChH
            newh += recChildren.height
        }
        else {
            recChildren.height = 0
        }
        var updateFather = ((height !== newh) && (father !== undefined))
        height = newh

        if(updateFather)
            father.updateDimensions()
    }

    function save() {
        //salvo i miei attributi
        for(var i = 0; i < recChildren.parameters.length; i++)
            recChildren.parChildren[i].setAttribute("value", recChildren.parameters[i].info)

        //e dico a tutti i figli di fare lo stesso
        for(i = 0; i < recChildren.children.length; i++)
            recChildren.children[i].save()
    }

    function destroyAncestry() {
        //console.log("ariamo tuttoooooo")
        for(var i=0;i<recChildren.parameters.length;i++)
            recChildren.parameters[i].destroy()
        for(var j=0;j<recChildren.children.length;j++){
            recChildren.children[j].destroyAncestry()
            recChildren.children[j].destroy()
        }
        recChildren.parameters=[]
        recChildren.children=[]
        recChildren.elements=[]
        recChildren.parChildren=[]
        recChildren.loaded = false
    }


    Row {
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        height: itemHeight
        spacing: width * 0.05
        Image {//freccetta
            id: image
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            width: height
            source: "qrc:/Images/arrow.png"
            rotation: btnOpen.opened ? 90 : 0
            Behavior on rotation {NumberAnimation{duration:200}}
        }

        MButton{//bottone dell'header
            id: btnOpen
            property bool opened: false
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            width: parent.width-image.width-header.spacing
            labelSize: rootAncestry.labelSize
            switchEnabled: false
            text: qsTranslate("SettingsContext",childName)
            onClicked: {
                btnOpen.opened = !btnOpen.opened
                if(opened) {
                    if (recChildren.loaded)
                        recChildren.visible = true
                    else
                        recChildren.load()
                }
                else
                    recChildren.visible = false

                updateDimensions()
            }
        }

    }


    Column {
        id:recChildren
        property var parameters: []//solo MParameters in qml
        property var parChildren: []//contiene i puntatori alle ancestry dei singoli parametri
        property var children: []//solo altre ancestry qml
        property var elements: []//contiene tutti i qml
        property bool loaded: false
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 4*layout.value("Margin")
        spacing: parent.spacing

        onVisibleChanged: {
            var childrenCount = ancestry.childrenCount()
            var pchild
            for(var i = 0; i < childrenCount; i++)
            {
                pchild = ancestry.getChildAt(i)
                pchild.visible = visible
            }
        }

        function load() {
            recChildren.loaded = true
            var childrenNames = ancestry.childrenNames()
            var childrenCount = ancestry.childrenCount()
            //console.log(ancestry.childrenNames())
            var curChi = []
            var curEle = []
            var last = undefined
            var c, pchild
            var i
            for(i = 0; i < childrenCount; i++)
            {
                pchild = ancestry.getChildAt(i)
                //console.log(pchild,i,childrenCount,ancestry)

                var labelAttr = pchild.getAttribute("label")
                if (labelAttr === "width track" )
                    continue

                console.log("LABELATT",labelAttr)
                if (CODSOFT === AcqModuleManager.FLOWSOFT && (labelAttr === "Range" || labelAttr === "Auto Range" || labelAttr === "Settings"))
                    continue

                if (CODSOFT === AcqModuleManager.FLWHS && (labelAttr === "Channels Abilitation" || labelAttr === "EMG"))
                    continue

                if(pchild.isYoungest()) {//creo un parametro modificabile
                    //controllo se devo visualizzarlo o no
                    var vis = pchild.getAttribute("visible")
                    if (vis === "false")
                        continue

                    //console.log("creo un parametro modificabile",pchild.getAttribute("label"))
                    c = Qt.createQmlObject('import MComponents 1.0 ;MParameterEdit{}', recChildren)
                    var attributesNames = ancestry.getAttributesNames()
                    var attributesValues = ancestry.getAttributesValues()
                    var attributesCount = ancestry.attributesCount()

                    c.anchors.left = recChildren.left
                    c.anchors.right = recChildren.right
                    c.anchors.leftMargin = itemHeight
                    c.height = itemHeight
                    c.type = pchild.getAttribute("type")
                    c.unit = pchild.getAttribute("unit")
                    var model=pchild.getAttribute("model").split("#")

                    //console.log(model)
                    if(model !== "" && model !== undefined)
                        c.model = model
                    var info = pchild.getAttribute("value")
                    //console.log(info,c.component)
                    c.setInfo(info)
                    c.role = qsTranslate("SettingsContext",pchild.getAttribute("label"))
                    c.labelSize = rootAncestry.labelSize

                    if (pchild.getAttribute("alfanum") === "false")
                        c.keyboardAlfaNum = false

                    parChildren.push(pchild)
                    parameters[parameters.length] = c

                }
                else
                {
                    //console.log("creo una sub ancestry")
                    //console.log(child.name(),"ancestry")
                    var visi = pchild.getAttribute("visible")
                    if (visi === "false")
                        continue
                    c = Qt.createQmlObject('import MComponents 1.0 ;MAncestry{}', recChildren)
                    c.anchors.left = recChildren.left
                    c.anchors.right = recChildren.right
                    c.anchors.leftMargin = itemHeight
                    //c.anchors.topMargin=layout.value("Margin")
                    c.father = rootAncestry
                    c.ancestry = pchild
                    c.labelSize = rootAncestry.labelSize
                    c.itemHeight = rootAncestry.itemHeight
                    children[children.length] = c
                }

                curEle[curEle.length] = c
                last = c
            }
            elements = curEle
        }
    }
}
