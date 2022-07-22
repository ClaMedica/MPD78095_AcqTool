import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MComponents 1.0

Rectangle {
    property var date:Date(edYear.text,cbMonth.text,cbDay.text)
    property int labelSize:2
    readonly property int marginPerc:1
    property int offset:0
    property string year: "1900"
    property bool erDay: false
    property bool erMonth: false
    property bool doUppa: false
    height:50
    width:200
    id:rootDateEdit
    color:"transparent"

    signal clicked

    Component.onCompleted: {
        var d=new Date()
        cbMonth.text=d.getMonth()+1
        cbDay.text=d.getDate()
        edYear.text=d.getFullYear()
        doUppa = true
        uppa(cbMonth.text)

    }

    function setDate(newDate){//accetta una stringa dal formato MM/dd/yyyy
        var d=new Date(String(newDate))
        edYear.text=d.getFullYear()
        cbMonth.text=d.getMonth()+1
        cbDay.text=d.getDate()
        uppa(cbMonth.text)
    }

    function uppa(month)
    {
   //     console.log("DAY",cbDay.text,month)
        if (!doUppa) return

        erDay = false
        erMonth = false
        if(month===undefined)
            return
        switch(month*1){
        case 0:
            erMonth = true
            break
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (cbDay.text > 31)
                erDay = true
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (cbDay.text > 30)
                erDay = true
            break;
        case 2:
            if(edYear.text%4===0){
                if (cbDay.text > 29)
                    erDay = true
            }
            else{
                if (cbDay.text > 28)
                    erDay = true
            }
            break;
        default:
            erMonth = true
           //console.log("Wrong month",month)

        }
        if (cbDay.text === "")
            erDay = true

        date=new Date(edYear.text,cbMonth.text-1,cbDay.text)
    }

    Row{
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.right:btnDialog.left
        anchors.bottom:parent.bottom
        spacing:width*0.01
        id:recDate
        MTextField{
            id:cbDay
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.3
            labelSize: rootDateEdit.labelSize
            maximumLength:2
            onlyNumber: true
            onTextChanged: uppa(cbMonth.text)
            onFocusChanged:
            {
                if(focus && isTouch)
                {
                    mngSys.startSound()
                    if (keyboardNum !== undefined)
                    {
                        keyboardNum.testo = cbDay.text
                        keyboardNum.target = cbDay
                        keyboardNum.labelTarget = qsTr("Day")
                        DataEngine.putItemOnTop(keyboardNum)
                        keyboardNum.show()
                    }
                    rootDateEdit.clicked()
                }
            }
        }
        MTextField{
            id:cbMonth
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.3
            maximumLength:2
            onlyNumber: true
            labelSize: rootDateEdit.labelSize
            onTextChanged: uppa(cbMonth.text)
            onFocusChanged:
            {
                if(focus && isTouch)
                {
                    mngSys.startSound()
                    if (keyboardNum !== undefined)
                    {
                        keyboardNum.testo = cbMonth.text
                        keyboardNum.target = cbMonth
                        keyboardNum.labelTarget = qsTr("Month")
                        DataEngine.putItemOnTop(keyboardNum)
                        keyboardNum.show()
                    }
                    rootDateEdit.clicked()
                }
            }
        }
        MTextField{
            id:edYear
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width:parent.width*0.38
            maximumLength:4
            onlyNumber: true
            text:""
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            labelSize:rootDateEdit.labelSize
            property string oldYear
            onTextChanged:{
                year = text
                uppa(cbMonth.text)
            }
            onFocusChanged:
            {
                if(focus && isTouch)
                {
                    mngSys.startSound()
                    if (keyboardNum !== undefined)
                    {
                        edYear.oldYear = edYear.text
                        keyboardNum.testo = edYear.text
                        keyboardNum.target = edYear
                        keyboardNum.labelTarget = qsTr("Year")
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
            rootDateEdit.clicked()
        }
    }

    Rectangle {
        id: dialog
        visible: false
        height:calendar.height+50
        width:calendar.width+10
        y: {
            var globalCoordinares = DataEngine.getAbsolutePosition(this)
            y = -globalCoordinares.y - offset
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset: 50
        anchors.horizontalCenterOffset: -50

        signal accepted
        signal rejected

        function open()
        {
            calendar.selectedDate = new Date(edYear.text*1,cbMonth.text-1,cbDay.text)
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
                cbMonth.text=date.getMonth() +1
                cbDay.text=date.getDate()
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
