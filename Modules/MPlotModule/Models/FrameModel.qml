import QtQuick 2.0

ListModel {

    property string descr: "Frame x"
    property string family: "none"
    property string name:"none"
    property string category: "none"
    property string key:"key nessuna"
    property int num:0
    property var enCh:[]
    property real xMin:0
    property real xMax:1
    property real yMin:0
    property real yMax:1
    property int resizeable:0
    property var anMarkers:[]
    property real whoAmI:0 //numero del frame
    property string color:"red"
    property var strList:["descr","family","name","category","key","num","enCh","xMin","xMax","yMin","yMax","whoAmI","color","resizeable","anMarkers"]

    function setProperty(role,value)
    {
        //console.log("propChange",role,value)
        switch(role)
        {
        case "descr": descr = value; break;
        case "family": family = value; break;
        case "name": name = value; break;
        case "category": category = value; break;
        case "key": key = value; break;
        case "num": num = value; break;
        case "enCh": enCh = value; break;
        case "xMin": xMin = value; break;
        case "xMax": xMax = value; break;
        case "yMin": yMin = value; break;
        case "yMax": yMax = value; break;
        case "whoAmI": whoAmI = value; break;
        case "color": color = value; break;
        case "resizeable": resizeable = value; break;
        case "anMarkers": anMarkers = value; break;
        default: console.log("WindowModel: Should not be Here!!!", role, value); break;
        }

    }

}
