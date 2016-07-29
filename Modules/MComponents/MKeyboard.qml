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
    property bool shift:false
    property bool special:false
    property bool caps:false
    readonly property int btnW:width/10
    property var btnLetters:[]
    property var target:undefined
    property bool destroyWhenOK:false


    //onBtnLettersChanged: console.log(btnLetters)

    //Component.onCompleted: show()

    anchors.fill: parent

    onShiftChanged:
    {
        for(var i=0;i<btnLetters.length;i++)
            if(btnLetters[i].text.length==1)
            {
                var t=btnLetters[i].text
                if(shift)
                    btnLetters[i].text=t.toUpperCase()
                else
                    btnLetters[i].text=t.toLowerCase()

            }
    }

    function show()
    {

        visible=true
    }


    Rectangle{
        id:recPreview
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right:parent.right
        height:parent.height/6
        color:"white"
        MTextField{
            id:txtField
            selectByMouse:false
            anchors.fill: parent            
            labelSize: rootKeyboard.labelSize
            onTextChanged: if(target!=undefined)target.text=txtField.text
            validator: target!=undefined?target.validator:null
        }
    }

    Column{
        id:col
        anchors.top: recPreview.bottom
        anchors.left: parent.left
        anchors.right:parent.right
        anchors.bottom: parent.bottom
        //spacing:height*0.01
        Repeater{
            id:repRow
            model:[
                [100,["1","2","3","4","5","6","7","8","9","0"],["!",'"',"£","$","%","&","/","(",")","="]],

                [100,["q","w","e","r","t","y","u","i","o","p"],["'","?","^","@","#","°","*","+","[","]"]],

                [90,["a","s","d","f","g","h","j","k","l"],["<",">",",",";",".",":","-","_","§"]],

                [100,[["",15,"shift.png"],"z","x","c","v","b","n","m",["",15,"backspace.png"]],
                 [["",15,"shift.png"],"|","à","è","é","ì","ò","ù",["",15,"backspace.png"]]],

                [100,[["1@#",15],[" ",70,"space.png"],["OK",15]],[["1@#",15],[" ",70,"space.png"],["OK",15]]]
            ]
            delegate:
                Row{
                id:row
                anchors.horizontalCenter: parent.horizontalCenter
                width:modelData[0]*parent.width/100
                height: parent.height/5
                //spacing:width*0.01
                Repeater{
                    id:repKey
                    model:modelData[special?2:1]

                    delegate:MButton{
                        labelSize: rootKeyboard.labelSize
                        text:modelData[0]
                        height: row.height
                        visible:text=="nn"?false:true
                        width: modelData[1]!==undefined?
                                   modelData[1]*row.width/100:btnW

                        image: modelData[2]!==undefined?modelData[2]:""
                        Component.onCompleted: btnLetters.push(this)
                        switchEnabled: image==="shift.png"|text==="1@#"
                        onPressAndHold:
                        {
                            if(image=="shift.png"){
                                shift=true
                                caps=!caps
                            }
                        }
                        onClicked:{
                            //console.log(text)
                            switch(text)
                            {
                            case "":
                                if(image=="shift.png"){
                                    shift=!shift
                                }
                                if(image=="backspace.png"){
                                    var pos=txtField.cursorPosition
                                    txtField.remove(pos,pos-1)
//                                    var first=txtField.text.substring(0,txtField.cursorPosition-1)
//                                    var sec=txtField.text.substring(txtField.cursorPosition)
//                                    var res=first.concat(sec)
//                                    txtField.text=res
//                                    txtField.cursorPosition=pos-1
                                }
                                break;
                            case "1@#":
                                special=!special

                                break;
                            case "OK":
                                if(destroyWhenOK)
                                    rootKeyboard.destroy()
                                else
                                {
                                    shift=false
                                    special=false
                                    caps=false
                                    rootKeyboard.visible=false
                                    target.focus=false
                                }

                                break;
                            default:
                                txtField.insert(txtField.cursorPosition,text)
                                special=false
                                if(!caps)
                                    shift=false
                                break;

                            }

                        }
                    }
                }
            }
        }



    }
}

