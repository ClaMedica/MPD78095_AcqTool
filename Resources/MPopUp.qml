import QtQuick 2.0
 
Rectangle{
    //@@@@@@@@@@    Definitions     @@@@@@@@@@
    property string message:"Ciao questo messaggio vuole verificare come funziona questo cavolo di messaggino piop up"
    property int rows:3 //identifica il numero di righe su cui voglio esporre il messaggio
    property int pixelXrow:16
    property real anim:0 //numerino che va da 0 a 1 e indica la percentuale di completamento di un'animazione
    property int xPos:0
    property int yPos:0
    property int dur:500

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id:rootPopUp
    border.width: 3
    opacity:anim
    height:Math.ceil(anim*popText.height)+border.width*2+100
    width:Math.ceil(anim*popText.width)+border.width*2+10
    color:Qt.rgba(255,255,255,0.8)
    Behavior on anim {NumberAnimation { easing.type: Easing.OutExpo; duration: dur }}
    Behavior on x {NumberAnimation { duration: dur/5 }}
    Behavior on y {NumberAnimation { duration: dur/5 }}
    clip:true

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function display(onOff,delay)
    {
        if(delay===0)
        {
            anim=onOff
            rootPopUp.visible=1
        }
        else
        {
            timDisplay.vis=onOff
            timDisplay.start(delay)
        }
    }

    //@@@@@@@@@@    Objects         @@@@@@@@@@
    Timer{
        //@@@@@@@@@@    Definitions     @@@@@@@@@@
        property bool vis:false
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:timDisplay
        interval:700


        //@@@@@@@@@@    Events          @@@@@@@@@@
        onTriggered: {
            x=xPos
            y=yPos
            anim=vis
            rootPopUp.visible=vis
        }
    }
    Text{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:popText
        color:"black"
        font.family:
        {
            if (PicoFlow)
                return (layout !== undefined) ? layout.value("FFamily") : "Luxi Serif"

            return (layout !== undefined) ? layout.value("FFamilyW") : "Calibri"
        }
        font.bold: true
        font.pixelSize: 16
        anchors.centerIn: rootPopUp
        height: (pixelXrow+2)*rows
        width: pixelXrow*message.length/(1.6*rows)
        opacity:anim
        text:message
        wrapMode: Text.Wrap
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
