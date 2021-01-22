import QtQuick 2.0
import MPlotModule 1.0

ListModel {
    property string name:"Plot 1"

    property color backgroundColor:"whitesmoke"
    property color borderColor: "black"

    property real xAbsoluteMax: 10
    property real xAbsoluteMin: 0
    property real yAbsoluteMax: 1
    property real yAbsoluteMin: -1

    property color gridLineColor: "grey"
    property real gridLineWidth: 1
    property color subGridLineColor: "grey"
    property real subGridLineWidth: 1
    property real xGridLines: 7
    property real yGridLines: 3
    property real xSubGridLines: 0
    property real ySubGridLines: 2

    property real xNotchLenght: 5
    property real yNotchLenght: 5

    // X Axis General properties
    property real xALFormat: AxisSettings.EFORMAT_TIME
    property real xALDecimalsMaj: 0
    property real xALDecimalsMin: 0
    property string xALDateTimeformatMaj: "mm:ss"
    property string xALDateTimeformatMin: "mm:ss"
//    property string xALDateTimeformatMaj: "hh:mm:ss.zzz"
//    property string xALDateTimeformatMin: "mm:ss.zzz"
    //X Proprietà del testo Major
    property string xALMajFontFamily: "Luxi Serif"
    property string xALMajFontColor: "black"
    property real xALMajFontSize: 8
    property real xALMajTextDist: 5
    property bool xALMajTextVisible: true
    property real xALMajOpacity: 1.0
    //X Proprietà del testo Minor
    property string xALMinFontFamily: "Luxi Serif"
    property color xALMinFontColor: Qt.rgba(0.5, 0.5, 0.5)
    property real xALMinFontSize: 6
    property real xALMinTextDist: 2
    property bool xALMinTextVisible: false
    property real xALMinOpacity: 1.0
    // X Axis unit of measure
    property string xAUOM: "[hh:mm:ss.zzz]"
    property bool xAUOMVisible: false
    property string xAFontUOMFamily: "Luxi Serif"
    property color xAFontUOMColor: Qt.rgba(0.8, 0.8, 0.8)
    property real xAFontUOMSize: 14
    property real xATextUOMDist: 10
    property real xAOpacityUOM: 1.0
    property real xAUOMRotation: 0
    // Y Axis General properties
    property real yALFormat: AxisSettings.EFORMAT_INT
    property real yALDecimalsMaj: 1
    property real yALDecimalsMin: 2
    property string yALDateTimeformatMaj: ""
    property string yALDateTimeformatMin: ""
    property bool yAutoScale: false
    //Y Proprietà del testo Major
    property string yALMajFontFamily: "Luxi Serif"
    property string yALMajFontColor: "grey"
    property real yALMajFontSize: 12
    property real yALMajTextDist: 5
    property bool yALMajTextVisible: true
    property real yALMajOpacity: 1.0
    //Y Proprietà del testo Minor
    property string yALMinFontFamily: "Luxi Serif"
    property color yALMinFontColor: Qt.rgba(0.5, 0.5, 0.5)
    property real yALMinFontSize: 10
    property real yALMinTextDist: 2
    property bool yALMinTextVisible: true
    property real yALMinOpacity: 1.0
    // y Axis unit of measure
    property string yAUOM: "[unit]"
    property bool yAUOMVisible: true
    property string yAFontUOMFamily: "Luxi Serif"
    property color yAFontUOMColor: "gray"//Qt.rgba(0.8, 0.8, 0.8)
    property real yAFontUOMSize: 10
    property real yATextUOMDist: 5
    property real yAOpacityUOM: 1.0
    property real yAUOMRotation: -90

    property string range: ""
    property real rangeValue: 0

    property bool drawed:false

    //Tracks
    property var tName: ["a","b","c","d"]
    property var tColor: ["red", "green", "blue", "yellow"]
    property var tThick: [3]
    property int maxTracks: 16

    //Margins
    property real topMargin: 20
    property real bottomMargin: 30
    property real leftMargin: 80
    property real rightMargin: PicoFlow ? 65 : 76

    //zoomer
    property real zoomSpeed:300

    //time cursor
    property int enableTimeCursor:0

    //step regolare
    property bool regolarStep: true

    //Real Time

    property int serverPort // server port
    property string serverAddress // server address
    property bool running: false
    property int updateInterval: 100 //MSEC
    property int samplingFrq: 10
    property int pageTime: 30
    property int enableTracking: 1

    property var strList:[
        "name",        
        "backgroundColor",
        "borderColor",
        "xAbsoluteMax",
        "xAbsoluteMin",
        "yAbsoluteMax",
        "yAbsoluteMin",
        "tName",
        "tColor",
        "tThick",
        "gridLineColor",
        "gridLineWidth",
        "subGridLineColor",
        "subGridLineWidth",
        "xGridLines",
        "yGridLines",
        "xSubGridLines",
        "ySubGridLines",
        "xNotchLenght",
        "xALFormat",
        "xALDecimalsMaj",
        "xALDecimalsMin",
        "xALDateTimeformatMaj",
        "xALDateTimeformatMin",
        "xALMajFontFamily",
        "xALMajFontColor",
        "xALMajFontSize",
        "xALMajTextDist",
        "xALMajTextVisible",
        "xALMajOpacity",
        "xALMinFontFamily",
        "xALMinFontColor",
        "xALMinFontSize",
        "xALMinTextDist",
        "xALMinTextVisible",
        "xALMinOpacity",
        "xAUOM",
        "xAUOMVisible",
        "xAFontUOMFamily",
        "xAFontUOMColor",
        "xAFontUOMSize",
        "xATextUOMDist",
        "xAOpacityUOM",
        "xAUOMRotation",
        "yALFormat",
        "yALDecimalsMaj",
        "yALDecimalsMin",
        "yALDateTimeformatMaj",
        "yALDateTimeformatMin",
        "yAutoScale",
        "yALMajFontFamily",
        "yALMajFontColor",
        "yALMajFontSize",
        "yALMajTextDist",
        "yALMajTextVisible",
        "yALMajOpacity",
        "yALMinFontFamily",
        "yALMinFontColor",
        "yALMinFontSize",
        "yALMinTextDist",
        "yALMinTextVisible",
        "yALMinOpacity",
        "yAUOM",
        "yAUOMVisible",
        "yAFontUOMFamily",
        "yAFontUOMColor",
        "yAFontUOMSize",
        "yATextUOMDist",
        "yAOpacityUOM",
        "yAUOMRotation",
        "Range",
        "rangeValue",
        "tColors",
        "topMargin",
        "bottomMargin",
        "leftMargin",
        "rightMargin",
        "zoomSpeed",
        "enableTimeCursor",
        "regolarStep",
        "serverPort",
        "serverAddress",
        "running",
        "updateInterval",
        "samplingFrq",
        "pageTime",
        "enableTracking",
        "maxTracks"
    ]

    function setProperty(role, value)
    {
        switch(role)
        {
        case "name":name=value;break;
        case "backgroundColor": backgroundColor = value; break;
        case "borderColor": borderColor = value; break;
        case "xAbsoluteMax":xAbsoluteMax=value;break;
        case "xAbsoluteMin":xAbsoluteMin=value;break;
        case "yAbsoluteMax":yAbsoluteMax=value;break;
        case "yAbsoluteMin":yAbsoluteMin=value;break;
        case "tName":tName=value;break;
        case "tColor":tColor=value;break;
        case "tThick":tThick=value;break;
        case "gridLineColor":gridLineColor=value;break;
        case "gridLineWidth":gridLineWidth=value;break;
        case "subGridLineColor":subGridLineColor=value;break;
        case "subGridLineWidth":subGridLineWidth=value;break;
        case "xGridLines":xGridLines=value;break;
        case "yGridLines":yGridLines=value;break;
        case "xSubGridLines":xSubGridLines=value;break;
        case "ySubGridLines":ySubGridLines=value;break;
        case "xNotchLenght":xNotchLenght=value;break;
        case "yNotchLenght":yNotchLenght=value;break;

        case "xALFormat":xALFormat=value;break;
        case "xALDecimalsMaj":xALDecimalsMaj=value;break;
        case "xALDecimalsMin":xALDecimalsMin=value;break;
        case "xALDateTimeformatMaj":xALDateTimeformatMaj=value;break;
        case "xALDateTimeformatMin":xALDateTimeformatMin=value;break;
        case "xALMajFontFamily":xALMajFontFamily=value;break;
        case "xALMajFontColor":xALMajFontColor=value;break;
        case "xALMajFontSize":xALMajFontSize=value;break;
        case "xALMajTextDist":xALMajTextDist=value;break;
        case "xALMajTextVisible":xALMajTextVisible=value;break;
        case "xALMajOpacity":xALMajOpacity=value;break;
        case "xALMinFontFamily":xALMinFontFamily=value;break;
        case "xALMinFontColor":xALMinFontColor=value;break;
        case "xALMinFontSize":xALMinFontSize=value;break;
        case "xALMinTextDist":xALMinTextDist=value;break;
        case "xALMinTextVisible":xALMinTextVisible=value;break;
        case "xALMinOpacity":xALMinOpacity=value;break;
        case "xAUOM":xAUOM=value;break;
        case "xAUOMVisible":xAUOMVisible=value;break;
        case "xAFontUOMFamily":xAFontUOMFamily=value;break;
        case "xAFontUOMColor":xAFontUOMColor=value;break;
        case "xAFontUOMSize":xAFontUOMSize=value;break;
        case "xATextUOMDist":xATextUOMDist=value;break;
        case "xAOpacityUOM":xAOpacityUOM=value;break;
        case "xAUOMRotation":xAUOMRotation=value;break;

        case "yALFormat":yALFormat=value;break;
        case "yALDecimalsMaj":yALDecimalsMaj=value;break;
        case "yALDecimalsMin":yALDecimalsMin=value;break;
        case "yALDateTimeformatMaj":yALDateTimeformatMaj=value;break;
        case "yALDateTimeformatMin":yALDateTimeformatMin=value;break;
        case "yAutoScale":
            if (value === "false")
                yAutoScale = false;
            if (value === "true")
                yAutoScale = true;
            break;
        case "yALMajFontFamily":yALMajFontFamily=value;break;
        case "yALMajFontColor":yALMajFontColor=value;break;
        case "yALMajFontSize":yALMajFontSize=value;break;
        case "yALMajTextDist":yALMajTextDist=value;break;
        case "yALMajTextVisible":yALMajTextVisible=value;break;
        case "yALMajOpacity":yALMajOpacity=value;break;
        case "yALMinFontFamily":yALMinFontFamily=value;break;
        case "yALMinFontColor":yALMinFontColor=value;break;
        case "yALMinFontSize":yALMinFontSize=value;break;
        case "yALMinTextDist":yALMinTextDist=value;break;
        case "yALMinTextVisible":yALMinTextVisible=value;break;
        case "yALMinOpacity":yALMinOpacity=value;break;
        case "yAUOM":yAUOM=value;break;
        case "yAUOMVisible":yAUOMVisible=value;break;
        case "yAFontUOMFamily":yAFontUOMFamily=value;break;
        case "yAFontUOMColor":yAFontUOMColor=value;break;
        case "yAFontUOMSize":yAFontUOMSize=value;break;
        case "yATextUOMDist":yATextUOMDist=value;break;
        case "yAOpacityUOM":yAOpacityUOM=value;break;
        case "yAUOMRotation":yAUOMRotation=value;break;

        case "Range": range = value; break;
        case "rangeValue": rangeValue = value; break;

        case "tColors":tColors=value;break;
        case "topMargin":topMargin=value;break;
        case "bottomMargin":bottomMargin=value;break;
        case "leftMargin":leftMargin=value;break;
        case "rightMargin":rightMargin=value;break;
        case "zoomSpeed":zoomSpeed=value;break;
        case "enableTimeCursor":enableTimeCursor=value;break;
        case "regolarStep":regolarStep=value;break;
        case "serverPort":serverPort=value;break;
        case "serverAddress":serverAddress=value;break;
        case "running":running=value;break;
        case "updateInterval":updateInterval=value;break;
        case "samplingFrq":samplingFrq=value;break;
        case "pageTime":pageTime=value;break;
        case "enableTracking":enableTracking=value;break;
        case "maxTracks":maxTracks=value;break;
        default:console.log("PlotModel: Should not be Here!!!",role,value);break;
        }
    }
}
