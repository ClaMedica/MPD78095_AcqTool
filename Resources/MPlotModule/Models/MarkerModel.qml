import QtQuick 2.0

ListModel {
    property string descr: "Marker x"
    property string img:""
    property string family: "none"
    property string name: "none"
    property string type: "none"
    property string category: "none"
    property real val:0
    property real valY:0
    property real whoAmI:0 //codice identificativo del singolo marker
    property string code:"code" //codice del tipo di marker
    property int channel:0
    property int lock:0
    property real defCode:0 //codice del definitore a cui è associato
    property string color:"red"
    property string key:"none"
    property int visible:1
    property int count:0
    property int countVisible:0
    property var strList:["descr","img","family","name","color","type","category","val","valY","code",
                            "whoAmI","channel","lock","defCode","key","visible","count","countVisible"]

    function setProperty(role,value)
    {
        //console.log("propChange",role,value)
        switch(role)
        {
        case "descr":descr=value;break;
        case "img":img=value;break;
        case "family":family=value;break;
        case "name":name=value;break;
        case "color":color=value;break;
        case "type":type=value;break;
        case "category":category=value;break;
        case "val":val=value;break;
        case "valY":valY=value;break;
        case "code":code=value;break;
        case "whoAmI":whoAmI=value;break;
        case "channel":channel=value;break;
        case "lock":lock=value;break;
        case "defCode":defCode=value;break;
        case "key":key=value;break;
        case "visible":visible=value;break;
        case "count":count=value;break;
        case "countVisible":countVisible=value;break;
        default:console.log("MarkerModel: Should not be Here!!!",role,value);break;
        }

    }

}
