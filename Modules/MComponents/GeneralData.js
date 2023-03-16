.pragma library
var isPatientRowSelected = false
var selectedPatientRow = ""
var selectedTestRow= ""
var BTN_STATE = true
var BACKGROUND_COLOR = "#355667"
var TASKBAR_COLOR = "#203d46"
var hr0 = "#6666FF"
var hr1 = "#3333FF"
var FONT_HEADER_POINTSIZE = 10
var FONT_SUBHEADER_POINTSIZE = 9
var SELECTED_ROW ="#70B8FF"
var ir0 = "#FFFFFF"
var ir1 = "#E0E0E0"
var ir2 = "#C0C0C0"
var ir3 = "#A0A0A0"
var sr = "#66CCFF"

var c_btn_idle = "#C0C0C0"
var c_btn_hovered ="#CCE5FF"
var c_btn_enabled ="#99CCFF"
var c_btn_checked ="#3399FF"
var c_btn_pressed ="#0066CC"

var c_btn_orange_hovered ="#F4BC69"
var c_btn_orange_enabled ="#FFA41C"
var c_btn_orange_checked ="#F46400"
var c_btn_orange_pressed ="#F45000"

var MARGIN = 5
var CategoryWinWidth = 600
var CategoryWinHeight = 600
var rowHeight = 40

function isValidDate(d){
    // An invalid date object returns NaN for parse() and NaN is the only
    // object not strictly equal to itself.
    var data=new Date(d)
    console.log(data)
    return data===data
}

function calculateAge(birthDateStr){
    var birthDate = new Date(String(birthDateStr))
    var d2 = new Date();
    var diff = d2.getTime() - birthDate.getTime();
    return Math.floor(diff / (1000 * 60 * 60 * 24 * 365.25));
}

function getAbsolutePosition(node) {//permette di trovare la posizione assoluta all'interno dello schermo di determinato item
    var returnPos = {};
    returnPos.x = 0;
    returnPos.y = 0;
    if(node !== undefined && node !== null) {
        var parentValue = getAbsolutePosition(node.parent);
        returnPos.x = parentValue.x + node.x;
        returnPos.y = parentValue.y + node.y;
    }
    return returnPos;
}
function putItemOnTop(item)
{
    if(item.parent !== undefined && item.parent !== null)
    {//ha un papa'
        var maxZ=0
        var padre=item.parent
        for(var i=0;i<padre.children.length;i++)//per ogni cinno
            if(padre.children[i].z>maxZ)
                maxZ=padre.children[i].z
        if(item.z<=maxZ)
            item.z=maxZ+1
        //console.log(item,item.z)
        putItemOnTop(padre)
    }
}
function riseItem(item)
{
    item.z=1
    //console.log("nuovo",item)//diciamo chi siamo
    if(item.children !== undefined && item.children !== null){//e poi iteriamo ricorsivamente
        for(var i=0;i<item.children.length;i++)//per ogni cinno
            riseItem(item.children[i])
    }
    //e anche all'indietro a settare tutto a 1
    findContentItem(item,1)

}

function findContentItem(item,newZ)
{
    //console.log(item.parent)
    if(item.parent !== undefined && item.parent !== null)
        return findContentItem(item.parent,newZ)
    else
        return item
}

function restoreZorder(item,newZ)
{
    //console.log(item,item.z)//diciamo chi siamo
    item.z=newZ//mettiamolo al livello di default
    if(item.children !== undefined){//e poi iteriamo ricorsivamente
        for(var i=0;i<item.children.length;i++)//per ogni cinno
            restoreZorder(item.children[i],newZ)
    }
}

