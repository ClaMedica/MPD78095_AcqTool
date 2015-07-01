import QtQuick 2.0

ListModel {

    property string descr: "Track x"
    property string family: "none"
    property string name:"none"
    property string category: "none"
    property int pointer:0 //numero del frame
    property string color:"none"
    property real whoAmI:0//codice della traccia
    property var strList:["descr","family","name","category","pointer","color","whoAmI"]

    function setProperty(role,value)
    {
        //console.log("propChange",role,value)
        switch(role)
        {
        case "descr":descr=value;break;
        case "family":family=value;break;
        case "name":name=value;break;
        case "category":category=value;break;
        case "pointer":pointer=value;break;
        case "color":color=value;break;
        case "whoAmI":whoAmI=value;break;
        default:console.log("TrackModel: Should not be Here!!!",role,value);break;
        }

    }

}
