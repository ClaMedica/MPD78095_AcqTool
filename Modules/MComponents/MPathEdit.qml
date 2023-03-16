import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Rectangle {
    property string folder:"file://"
    property string nameFilters:"*.*"
    property bool selectFolder:false
    property alias fileURL:path.text
    id:rootPathEdit
    color:"transparent"

    signal clicked

    MTextField{
        id:path
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:btnDialog.left
        anchors.bottom:parent.bottom
        labelSize: layout.value("F4")
        enabled: false
        text:""
    }

    MButton{
        id:btnDialog
        width:height
        anchors.right:parent.right
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        text:"..."
        onClicked: dialog.open()
    }

    FileDialog{
        id:dialog
        folder:rootPathEdit.folder
        nameFilters: rootPathEdit.nameFilters
        onSelectionAccepted: {
            rootPathEdit.clicked()
            if(selectFolder)
                path.text=folder
            else
                path.text=fileUrl
        }
        selectMultiple: false
        selectFolder: rootPathEdit.selectFolder
        onFolderChanged: console.log(folder)
    }
}

