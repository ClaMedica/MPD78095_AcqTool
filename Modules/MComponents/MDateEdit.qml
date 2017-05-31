import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    property var date:Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex+1)
    property int labelSize:2
    readonly property int marginPerc:1
    height:50
    width:200
    id:rootDateEdit
    color:"transparent"

    signal clicked

    Component.onCompleted: {
        var d=new Date()
        cbMonth.currentIndex=d.getMonth()
        uppa(d.getMonth())
        cbDay.currentIndex=d.getDate()-1
        edYear.text=d.getFullYear()
    }

    function setDate(newDate){//accetta una stringa
        var d=new Date(String(newDate))
        cbMonth.currentIndex=d.getMonth()
        uppa(d.getMonth())
        cbDay.currentIndex=d.getDate()-1
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

        date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex+1)
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
            onCurrentIndexChanged: date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex+1)
            onClicked: if(isTouch) rootDateEdit.clicked()
        }
        MComboBox{
            id:cbMonth
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.3
            model:["01","02","03","04","05","06","07","08","09","10","11","12"]
            onCurrentIndexChanged: uppa(currentIndex)
            labelSize: rootDateEdit.labelSize
            onClicked: if(isTouch) rootDateEdit.clicked()
        }
        MTextField{
            id:edYear
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.38
            maximumLength:4
            text:""
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            labelSize:rootDateEdit.labelSize
            onTextChanged: date=new Date(edYear.text,cbMonth.currentIndex,cbDay.currentIndex+1)
            onFocusChanged:
            {
                if(focus && isTouch)
                {
                    if (keyboardNum !== undefined)
                    {
                        keyboardNum.testo = edYear.text
                        keyboardNum.target = edYear
                        DataEngine.putItemOnTop(keyboardNum)
                        keyboardNum.show()
                    }
                    rootDateEdit.clicked()
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
        onClicked: {
            dialog.open()
        }
    }

    Rectangle {
        id: dialog
        visible: false
        height:calendar.height+50
        width:calendar.width+10
        y: {
            var globalCoordinares = DataEngine.getAbsolutePosition(this)
            y = -globalCoordinares.y
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset: 50
        anchors.horizontalCenterOffset: -50

        signal accepted
        signal rejected

        function open()
        {
            calendar.selectedDate = new Date(edYear.text*1,cbMonth.currentIndex,cbDay.currentIndex+1)
            DataEngine.putItemOnTop(this)
            btnSaveDialog.enabled = false
            visible=true
        }

        onAccepted: visible=false
        onRejected: visible=false

        MouseArea {
            width: parent.width
            height: parent.height
        }

        MButton
        {
            id:btnCancDialog
            width: parent.width/2.1
            anchors.right:calendar.right
            anchors.top:calendar.bottom
            anchors.bottom:parent.bottom
            anchors.margins: 3
            labelSize: layout.value("F4")
            text:qsTr("Cancel")
            onClicked:dialog.rejected()
        }
        MButton
        {
            id:btnSaveDialog
            width: parent.width/2.1
            anchors.left:calendar.left
            anchors.top:calendar.bottom
            anchors.bottom:parent.bottom
            anchors.right: btnCancDialog.left
            anchors.margins: 3
            enabled: false
            labelSize: layout.value("F4")
            text:qsTr("Save")
            onClicked: {
                var date=new Date(calendar.selectedDate)
                cbMonth.currentIndex=date.getMonth()
                cbDay.currentIndex=date.getDate()-1//cbDay parte da 0
                edYear.text=date.getFullYear()
                dialog.accepted()
            }
        }

        Calendar {
            id: calendar
            width: 440
            onSelectedDateChanged: btnSaveDialog.enabled = true
        }
    }
}
