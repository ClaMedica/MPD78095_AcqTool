import QtQuick 2.0
//import Resources 1.0
ListModel {
    //@@@@@@@@@@    Definitions      @@@@@@@@@@
    property string message: "Alarm"
    property string help: "call 911"
    property int code: 0
    property string image:Images.medica
    property string color: "red"
    property string sound:""
    property var strList:["message","code","image","color","help","sound"]

    //@@@@@@@@@@    Functions       @@@@@@@@@@
    function setProperty(role,value)
    {
        //console.log("propChange",role,value)
        switch(role)
        {
        case "message":message=value;break;
        case "image":image=value;break;
        case "help":help=value;break;
        case "code":code=value;break;
        case "color":color=value;break;
        case "sound":sound=value;break;
        default:console.log("AlarmModel: Should not be Here!!!",role,value);break;
        }
    }
}

