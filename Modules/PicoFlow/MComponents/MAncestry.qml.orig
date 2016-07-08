import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "qrc:/GeneralData.js" as DataEngine
import "qrc:/"
import "qrc:/Forms"
import MComponents 1.0 //import "qrc:/Components"
import Settings 1.0
Rectangle {
    id:rootAncestry
    property var ancestry//si suppone che sia una classe ancestry altrimenti non funziona
    property string childName:"No name"
    property int itemHeight:50
    property var father

    color:"transparent"
    //onHeightChanged: console.log(height)
    onAncestryChanged: {
        //console.log(ancestry)
        if(ancestry!==undefined){
            if(ancestry.getAttribute("label")==="")
                childName=ancestry.name()
            else
                childName=ancestry.getAttribute("label")
            btnOpen.opened=false
            destroyAncestry()
            updateDimensions()
        }
    }

    function updateDimensions(){
        var newh=header.height
        for(var i=0;i<recChildren.elements.length;i++)
            newh+=recChildren.elements[i].height+layout.value("Margin")
        height=newh

        if(father!==undefined)
            father.updateDimensions()
    }
    function save(){
        //salvo i miei attributi
        for(var i=0;i<recChildren.parameters.length;i++)
            recChildren.parameters[i].setAttribute("value",recChildren.parRef[i].info)


        for(i=0;i<recChildren.children.length;i++)
            recChildren.children[i].save()


    }

    function destroyAncestry()
    {
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
    }

    Rectangle{
        id:header
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.top:parent.top
        height:itemHeight
        color:"transparent"

        Image{//freccetta
            id:image
            anchors.left:parent.left
            anchors.bottom:parent.bottom
            anchors.top:parent.top
            anchors.margins: layout.value("Margin")
            width:height
            source:"qrc:/Images/arrow.png"
            rotation: btnOpen.opened?90:0
            Behavior on rotation {NumberAnimation{duration:200}}
        }

        MButton{//bottone dell'header
            id:btnOpen
            property bool opened:false
            anchors.right:parent.right
            anchors.left:image.right
            anchors.bottom:parent.bottom
            anchors.top:parent.top
            anchors.margins: layout.value("Margin")
            text:childName
            onClicked: {
                btnOpen.opened=!btnOpen.opened
                if(opened){
                    recChildren.load()
                    updateDimensions()
                }
                else
                {
                    destroyAncestry()
                    updateDimensions()
                }


            }
        }

    }


    Rectangle{
        id:recChildren
        property var parameters:[]//solo parametri
        property var children:[]//solo altre ancestry qml
        property var elements:[]//contiene tutti i qml
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.top:header.bottom
        anchors.leftMargin: 4*layout.value("Margin")
        color:"transparent"
       // onElementsChanged: recChildren.height=elements.length*(header.height+layout.value("Margin"))
        function load(){
            var childrenNames=ancestry.childrenNames()
            var childrenCount=ancestry.childrenCount()
            console.log(ancestry.childrenNames())
            var curChi=[]
            var curEle=[]
            var last=undefined
            var c,pchild
            var i
            for(i=0;i<childrenCount;i++)
            {
                pchild=ancestry.getChildAt(i)
                console.log(pchild,i,childrenCount,ancestry)


                if(pchild.isYoungest()){//creo un parametro modificabile
                    //console.log("creo un parametro modificabile")
                    c=Qt.createQmlObject('import MComponents 1.0 ;MParameterEdit{}',recChildren)
                    var attributesNames=ancestry.getAttributesNames()
                    var attributesValues=ancestry.getAttributesValues()
                    var attributesCount=ancestry.attributesCount()

                    c.anchors.left=recChildren.left
                    c.anchors.right=recChildren.right
                    c.height=header.height
                    if(last===undefined)
                        c.anchors.top=recChildren.top
                    else
                        c.anchors.top=last.bottom
                    c.anchors.topMargin=layout.value("Margin")
                    c.type=pchild.getAttribute("type")
                    c.unit=pchild.getAttribute("unit")
                    var model=pchild.getAttribute("model").split("#")

                    //console.log(model)
                    if(model!=="" && model !==undefined)
                        c.model=model
                    var info=pchild.getAttribute("value")
                    c.setInfo(info)
                    c.role=pchild.getAttribute("label")

                    parameters[parameters.length]=c
                }
                else
                {
                    //console.log("creo una sub ancestry")
                    //console.log(child.name(),"ancestry")
                    c=Qt.createQmlObject('import MComponents 1.0 ;MAncestry{}',recChildren)
                    c.anchors.left=recChildren.left
                    c.anchors.right=recChildren.right
                    //c.height=header.height-layout.value("Margin")
                    if(last===undefined)
                        c.anchors.top=recChildren.top
                    else
                        c.anchors.top=last.bottom
                    c.anchors.topMargin=layout.value("Margin")
                    c.father=rootAncestry
                    c.ancestry=pchild
                    children[children.length]=c
                }

                curEle[curEle.length]=c
                last=c
            }
            elements=curEle
        }


    }
}
