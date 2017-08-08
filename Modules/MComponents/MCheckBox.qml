import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import "Images"

Rectangle {
    id:rootCheck
    property var   checked:    undefined
    property bool realChecked:  false
    property string text:       textEdit.text
    property bool   editable:   false
    property int    pixelSize:  Screen.height*0.01
    color:"transparent"
    height: Screen.height*0.02
    width:  Screen.width*0.05
    onTextChanged: if(!editable)initText(text)
    onCheckedChanged: {
        if (checked !== undefined)
            realChecked = checked
    }

    function initText(t)
    {
        rootCheck.text = t
        textEdit.text = t
    }

    Rectangle {
        id:indicator

        width: rootCheck.height
        height: rootCheck.height
        radius: 3
        border.color: rootCheck.activeFocus ? "darkblue" : "gray"
        border.width: 1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Image {
            visible: realChecked
            source:"checked.png"
            anchors.margins: 4
            anchors.fill: parent
        }
        BuzzMouseArea{
            anchors.fill: parent
            onClicked: {
                realChecked=!realChecked;
                checked = realChecked
            }
        }

    }


    TextEdit {
        id:textEdit
        enabled: editable
        anchors.top:rootCheck.top
        anchors.bottom:rootCheck.bottom
        anchors.left:indicator.right
        anchors.right:rootCheck.right
        anchors.margins:rootCheck.height/5
        onFocusChanged:text=focus ? "" : text
        font.bold:true
        font.pixelSize:rootCheck.pixelSize
        verticalAlignment:Text.AlignVCenter
    }

}

