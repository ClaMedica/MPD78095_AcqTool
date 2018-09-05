//qt imports
import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.1
//medica imports
import MComponents 1.0
import QtQuick.Controls.Styles 1.4

BuzzTableView{
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

    onSortIndicatorColumnChanged: order(sortIndicatorColumn, sortIndicatorOrder,true)
    onSortIndicatorOrderChanged:  order(sortIndicatorColumn, sortIndicatorOrder,true)

    signal selected()

    function selectRowFromPK(pk)
    {
        rootTable.selection.clear()
        if (pk !== -1)
        {
            var row = model.indexOfPK(pk)
            rootTable.positionViewAtRow(row,ListView.Beginning)
            rootTable.selection.select(row)
            rootTable.currentRow = row
        }
        rootTable.selected()
    }

    function populate(){//al caricamento della tabella popolo con le colonne
      // sortIndicatorOrder=1
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
            upDowns[upDowns.length]=0
        }
        sortIndicatorColumn=0
        sortIndicatorOrder=0
        //resize()
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

    function order(c,order,sound){
        if(!sortEnabled)
            return
        upDowns[c]=order
        model.orderByInfo(type,roles[c],order)
        if (sound && rootTable.visible && rootTable.completed && isTouch) //altrimenti il click su header non suona
            mngSys.startSound()
    }

    function getColumnValues()
    {
        var col = []
        var info = model.modelInfo(type)

        for (var i=0; i<info.length; i++)
            col[i] = info[i][0]

        return col
    }

    rowDelegate:TRowDelegate{itemData:styleData}
    itemDelegate:TItemDelegate{
        colonna:getColumnValues()
        itemData:styleData
    }
    headerDelegate:THeaderDelegate{itemData:styleData}

    style: TableViewStyle {
        scrollToClickedPosition : true

        handle: Rectangle {
            z: 100
            implicitWidth: 40
            implicitHeight: 30
            Rectangle {
                color: layout.value("THC1")
                anchors.fill: parent
                anchors.topMargin: 6
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                anchors.bottomMargin: 6
            }
        }
        incrementControl: Rectangle {
            visible: false
        }
        decrementControl: Rectangle {
            visible: false
        }

        scrollBarBackground: Item {
            implicitWidth: 40
            implicitHeight: 30
        }
    }

     Rectangle {
        id:frame
        anchors.fill:rootTable
        width:rootTable.width-2
        height:rootTable.height-2
        border.width: 2
        border.color: layout.value("BackgroundColor")
        color:"transparent"
    }
}




