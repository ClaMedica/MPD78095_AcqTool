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
            if(old<cbDay.model.count)
                cbDay.currentIndex=old
            break;
        case 3:
        case 5:
        case 8:
        case 10:
            old=cbDay.currentIndex
            cbDay.model=cbDay.mod30;
            if(old<cbDay.model.count)
                cbDay.currentIndex=old
            break;
        case 1:
            old=cbDay.currentIndex
            if(edYear.text%4===0)
                cbDay.model=cbDay.mod29
            else
                cbDay.model=cbDay.mod28

            if(old<cbDay.model.count)
                cbDay.currentIndex=old
            break;
        default:console.error("Wrong month",month)
        }
        date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)
    }

    Rectangle{
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:btnDialog.left
        anchors.bottom:parent.bottom
        anchors.margins: parent.height*0.01*marginPerc
        color:"transparent"
        id:recDate
        ComboBox{
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
            anchors.top:parent.top
            anchors.left:parent.left
            anchors.margins: parent.height*0.01*marginPerc
            width:Math.round(parent.width*0.3)
            style: ComboBoxStyle{
                label:Text{
                    text:cbDay.currentText
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    font.pixelSize: Screen.height*0.01*labelSize
                }
            }

            anchors.bottom:parent.bottom
            onCurrentIndexChanged: date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex)

        }
        ComboBox{
            id:cbMonth
            anchors.top:parent.top
            anchors.left:cbDay.right
            anchors.margins: parent.height*0.01*marginPerc
            width:Math.round(parent.width*0.3)
            anchors.bottom:parent.bottom
            model:["01","02","03","04","05","06","07","08","09","10","11","12"]
            onCurrentIndexChanged: uppa(currentIndex)
            style: ComboBoxStyle{
                label:Text{
                    text:cbMonth.currentText
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    font.pixelSize: Screen.height*0.01*labelSize
                }
            }

        }
        TextField{
            id:edYear
            anchors.top:parent.top
            anchors.left:cbMonth.right
            anchors.right:parent.right
            anchors.bottom:parent.bottom
            anchors.margins: parent.height*0.01*marginPerc
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
