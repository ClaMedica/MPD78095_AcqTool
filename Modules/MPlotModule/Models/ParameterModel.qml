import QtQuick 2.0
ListModel {

    property string label: "Par 0"
    property string name: "none"
    property real min:0
    property real max:100    
    property real val:0
    property real stepSize:0.1
    property var values:[0]
    property var strList:["label","name","min","max","val","stepSize","values"]

    function setProperty(role,value)
    {
        //console.log("propChange",role,value)
        switch(role)
        {
        case "label":label=value;break;
        case "name":name=value;break;
        case "max":max=value;break;
        case "min":min=value;break;
        case "val":val=value;break;
        case "values":values=value;break;
        case "stepSize":stepSize=value;break;
        default:console.log("ParameterModel: Should not be Here!!!",role,value);break;
        }

    }

}
