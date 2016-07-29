import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    property var date:Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)
    property int labelSize:2

    readonly property int marginPerc:1
    height:50
    width:200
    id:rootDateEdit
    color:"transparent"
    Component.onCompleted: {
        var d=new Date()
        cbMonth.currentIndex=d.getMonth()
        uppa(d.getMonth())
        cbDay.currentIndex=d.getDate()
        edYear.text=d.getFullYear()
    }

    function setDate(newDate){//accetta una stringa
        var d=new Date(String(newDate))
        console.log(newDate,d,date)
        cbMonth.currentIndex=d.getMonth()
        uppa(d.getMonth())
        cbDay.currentIndex=d.getDate()
        edYear.text=d.getFullYear()

    }

    function uppa(month)
    {
        var old
        //console.log(month)
        if(month===undefined)
            return
        switch(month){
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            old=cbDay.currentIndex
            cbDay.model=cbDay.mod31;          
            break;
        case 3:
        case 5:
        case 8:
        case 10:
            old=cbDay.currentIndex
            cbDay.model=cbDay.mod30;
            break;
        case 1:
            old=cbDay.currentIndex
            if(edYear.text%4===0)
                cbDay.model=cbDay.mod29
            else
                cbDay.model=cbDay.mod28
            break;
        default:console.error("Wrong month",month)
        }
        if(old<cbDay.model.length)
            cbDay.currentIndex=old
        console.log(old,cbDay.model.length,cbDay.currentIndex)
        date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)
    }

    Row{
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:btnDialog.left
        anchors.bottom:parent.bottom
        spacing:width*0.01
        id:recDate
        MComboBox{
            property var mod28:["01","02","03","04","05","06","07","08","09","10",
                "11","12","13","14","15","16","17","18","19","20",
                "21","22","23","24","25","26","27","28"]
            property var mod29:["01","02","03","04","05","06","07","08","09","10",
                "11","12","13","14","15","16","17","18","19","20",
                "21","22","23","24","25","26","27","28","29"]
            property var mod30:["01","02","03","04","05","06","07","08","09","10",
                "11","12","13","14","15","16","17","18","19","20",
                "21","22","23","24","25","26","27","28","29","30"]
            property var mod31:["01","02","03","04","05","06","07","08","09","10",
                "11","12","13","14","15","16","17","18","19","20",
                "21","22","23","24","25","26","27","28","29","30","31"]
            id:cbDay
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.3
            labelSize: rootDateEdit.labelSize
            onCurrentIndexChanged: date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)

        }
        MComboBox{
            id:cbMonth
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.3
            model:["01","02","03","04","05","06","07","08","09","10","11","12"]
            onCurrentIndexChanged: uppa(currentIndex)
            labelSize: rootDateEdit.labelSize
        }
        TextField{
            id:edYear
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.38
            maximumLength:4
            validator: RegExpValidator {
                regExp: /[0-9]+/
            }
            text:cbMonth.currentText
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: Screen.height*0.01*labelSize
            onTextChanged: date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)
            onFocusChanged:
            {
                if(focus && isTouch)
                {
                    var c
                    if(keyboard===undefined){
                        console.log("creiamo questa tasteira")
                        c=Qt.createQmlObject('import MComponents 1.0;
                                          MKeyboard {}',rootApp);
                        c.destroyWhenOK=true
                    }
                    else
                        c=keyboard

                    c.target=edYear
                    DataEngine.putItemOnTop(c)
                    c.show()
                }
            }
        }
    }



    MButton{
        id:btnDialog
        width:30
        anchors.right:parent.right
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.margins: parent.height*0.01*marginPerc
        labelSize: rootDateEdit.labelSize
        text:"..."
        onClicked: dialog.open()
    }

    Dialog {
        id: dialog
        visible: false
        title: "Choose a date"
        standardButtons: StandardButton.Save | StandardButton.Cancel
        height:calendar.height+20
        width:calendar.width+20
        onAccepted:{
            var date=new Date(calendar.selectedDate)
            console.log(date.getDate(),date.getMonth(),date.getFullYear())
            cbMonth.currentIndex=date.getMonth()
            cbDay.currentIndex=date.getDate()
            edYear.text=date.getFullYear()
        }

        Calendar {
            id: calendar
            onDoubleClicked: dialog.click(StandardButton.Save)
        }
    }
}
