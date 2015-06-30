import QtQuick 2.0
import Resources 1.0
Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property var alarms:[]
    property int alarmCount:0
    property int alarmHeight:80
    signal resetAlarms
    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootAlarmBox
    color:"transparent"
    Behavior on height {NumberAnimation { duration: 200 }}
    //clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function popola()
    {
        var aCount=0
        //console.log("popolaAlarm",items)

        var ll=alarms.length
        for(var i=0;i<ll;i++)
            if(alarms[i]==="$Alarm")
                aCount++
        alarmCount=aCount
        //console.log("popolato con ",alarmCount," allarmi")
        if(alarmCount>0)
        {
            height=alarmCount*alarmHeight
            modify(alarms)
        }
        else
            height=0;
    }

    function modify(it)
    {
        //console.log("modify",it)
        if(it.length>0)
        {
            var l=[]
            var index=0
            var alarmIndex=0

            for(var i=0;i<it.length;i++)
            {
                if(it[i]==="$Alarm")
                {
                    l=[];
                    index=0;
                    continue;
                }
                if(it[i]==="&Alarm")
                {
                    //console.log("marker index",markerIndex,l)
                    lista.itemAt(alarmIndex).modello=l
                    alarmIndex++
                    continue
                }
                //inizia la lettura
                l[index]=it[i]
                index++
            }
        }
    }

    function moveAlarms(ind,direction)
    {
        height+=direction*alarmHeight*2
        for(var i=ind+1;i<alarmCount;i++)
        {
            var oldY=lista.itemAt(i).y
            lista.itemAt(i).y=oldY+direction*2*alarmHeight
        }
    }

    //@@@@@@@@@@    Events          @@@@@@@@@@
    onAlarmsChanged: popola()

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnReset
        background: "qrc:/Images/resetOff"
        pressedBackground: "qrc:/Images/resetOn"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 2
        width:alarmHeight
        hoverEnable: false
        imageFill: Image.PreserveAspectFit

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked: resetAlarms()
    }

    Repeater{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:lista
        anchors.top:parent.top
        anchors.left:btnReset.right
        anchors.right:parent.right
        anchors.bottom:parent.bottom
        anchors.margins: 2
        model:alarmCount

        //@@@@@@@@@@    Objects         @@@@@@@@@@
        delegate:
            MAlarm{
            //@@@@@@@@@@    Properties      @@@@@@@@@@
            id:curAla

            anchors.left:lista.left
            anchors.right:lista.right
            y:index*(alarmHeight+curAla.anchors.margins)
            alarmMexHeight: alarmHeight

            //@@@@@@@@@@    Events          @@@@@@@@@@
            onOpening: moveAlarms(index,1)
            onClosing: moveAlarms(index,-1)
        }
    }
}
