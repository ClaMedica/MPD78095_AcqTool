import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import MComponents 1.0
import "Images"
Rectangle {
    id:rootKeyboard
    property real labelSize:5
    readonly property int btnW:width/10
    property var btnLetters:[]
    property var target:undefined
    property bool destroyWhenOK:false
    property string testo: ""
    property string labelTarget: ""
    property string oldTesto: ""
    property bool password: false
    property int maxChars: 255

    anchors.fill: parent

    function show()
    {
        visible=true
        if (testo !== "") oldTesto = testo
        if (password)
            txtField.text = ""
        else
            txtField.text = testo
    }

    MouseArea{
        anchors.fill: parent
    }

    Rectangle{
        id:recPreview
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right:parent.right
        height:parent.height/6
        color:"white"

        MLabel{
            id:txtTarget
          //  anchors.fill: parent
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            labelSize: rootKeyboard.labelSize
            text: rootKeyboard.labelTarget;
        }
        MTextField{
            id:txtField
//            selectByMouse:false
            anchors.left: txtTarget.right
            anchors.right:parent.right
            labelSize: rootKeyboard.labelSize
            anchors.verticalCenter: parent.verticalCenter
            onTextChanged: if(target!=undefined) target.text=txtField.text
            validator: target!=undefined?target.validator:null
            echoMode: password ? TextInput.Password : TextInput.Normal
            maximumLength: maxChars
        }
    }

    Column{
        id:col
        anchors.top: recPreview.bottom
        anchors.left: parent.left
        anchors.right:parent.right
        anchors.bottom: parent.bottom

        Repeater{
            id:repRow
            model:[
                [90,["1","2","3"]],

                [90,["4","5","6"]],

                [90,["7","8","9"]],

                [100,[["OK",15],"0",["",15,"backspace.png"]]],

                [75,[["BACK",20]]]
            ]
            delegate:
                Row{
                id:row
                anchors.horizontalCenter: parent.horizontalCenter
                width:modelData[0]*parent.width/100
                height: parent.height/5
                Repeater{
                    id:repKey
                    model:modelData[1]

                    delegate:MButton{
                        labelSize: rootKeyboard.labelSize
                        text:modelData[0]
                        height: row.height
                        visible:text=="nn"?false:true
                        width: modelData[1]!==undefined?
                                   modelData[1]*row.width/100:btnW

                        image: modelData[2]!==undefined?modelData[2]:""
                        Component.onCompleted: btnLetters.push(this)

                        onClicked:{
                            switch(text)
                            {
                            case "":
                                if(image=="backspace.png"){
                                    var pos=txtField.cursorPosition
                                    txtField.remove(pos,pos-1)
                                }
                                break;
                            case "OK":
                                if(destroyWhenOK)
                                    rootKeyboard.destroy()
                                else
                                {
                                    rootKeyboard.visible=false
                                    target.focus=false
                                }

                                break;
                            case "BACK":
                                txtField.text = oldTesto
                                if(destroyWhenOK)
                                    rootKeyboard.destroy()
                                else
                                {
                                    rootKeyboard.visible=false
                                    target.focus=false
                                }
                                break;
                            default:
                                txtField.insert(txtField.cursorPosition,text)

                                break;

                            }

                        }
                    }
                }
            }
        }



    }
}

