import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import "qrc:/GeneralData.js" as DataEngine
import Settings 1.0
import "qrc:/"
import "qrc:/Forms"
import MComponents 1.0

//Uguale a MEditTree ma con la scroolbar
Rectangle {
    id: rootTree
    property real labelSize: 4
    property int divH: 10
    color: "transparent"
    clip: true

    function loadFile(f) {
        console.log("editSettings", f)
        if(anctree.ancestry !== undefined){
            anctree.destroyAncestry()
            anctree.ancestry.destroy()
        }
        var a = Qt.createQmlObject('import Settings 1.0; Ancestry{}', this)
        if(a.loadFromXML(f)) {
            anctree.ancestry = a
            //flick.contentItem = anctree
        }
    }

    function saveFile(f) {
        anctree.save()
        anctree.ancestry.saveToXML(f)
    }

    function destroyTree(){
        anctree.destroyAncestry()
    }


    Flickable{
        id:flick
        anchors.fill:parent
        contentHeight: anctree.height
        contentWidth: anctree.width

        MAncestry {
            id:anctree
            width:rootTree.width
            labelSize: rootTree.labelSize
            itemHeight: rootTree.height/divH
        }

        ScrollBar.vertical: ScrollBar {
            parent: flick.parent
            anchors.top: flick.top
            anchors.left: flick.right
            anchors.bottom: flick.bottom
            width: flick.width*0.02
        }
    }


}
