//qt imports
import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.1
//medica imports
import MComponents 1.0

TableView{
    id:rootTable
    property bool completed:false
    property var roles:[]
    property var upDowns:[]
    property string type:"default"
    property bool sortEnabled:true
    height:300
    width:300
    backgroundVisible :false
    frameVisible: false
    clip:true
    selectionMode:SelectionMode.ExtendedSelection
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    onWidthChanged: if(completed)resize()

    onSortIndicatorColumnChanged: order(sortIndicatorColumn, sortIndicatorOrder)
    onSortIndicatorOrderChanged: order(sortIndicatorColumn, sortIndicatorOrder)
    function populate(){//al caricamento della tabella popolo con le colonne
        sortIndicatorOrder=true
        if(model===undefined)
            return
        var info = model.modelInfo(type)
        var sizeUsed=0
        for (var i=0; i<info.length; i++) {
            //console.log(rootTable.width,rootTable.height)
            //console.log("nuova colonna",info[i][0],table.columnCount)
            var column = addColumn( Qt.createQmlObject(
                                       "import QtQuick.Controls 1.4; TableViewColumn {}",
                                       this) )
            column.role  = info[i][0]
            roles[roles.length]=column.role
            if(i<info.length-1)
                column.width = Math.round(info[i][1]*rootTable.width/100)
            else
                column.width = rootTable.width-sizeUsed
            sizeUsed+=column.width
            column.title = info[i][2]
            column.movable = false
            column.resizable = false
            upDowns[upDowns.length]=false
        }
        sortIndicatorColumn=0
        sortIndicatorOrder=false
        resize()
    }
    function resize()
    {
        var info = model.modelInfo(type)
        var sizeUsed=0
        for (var i=0; i<info.length; i++) {
            var column = getColumn(i)
            if(i<info.length-1)
                column.width = Math.round(info[i][1]*rootTable.width/100)
            else
                column.width = rootTable.width-sizeUsed
            sizeUsed+=column.width
            //console.log(rootTable.width,sizeUsed,column.width)
        }
    }

    function order(c,order){
        if(!sortEnabled)
            return
        upDowns[c]=order
        //console.log(upDowns)
        model.orderByInfo(type,roles[c],order)
    }

    rowDelegate:TRowDelegate{itemData:styleData}
    itemDelegate:TItemDelegate{itemData:styleData}
    headerDelegate:THeaderDelegate{itemData:styleData}

    Rectangle{
        id:frame
        anchors.fill:rootTable
        width:rootTable.width-2
        height:rootTable.height-2
        border.width: 2
        border.color: layout.value("BackgroundColor")
        color:"transparent"
    }
}




