import QtQuick 2.0

Item {
    id: scroolBar

    // The properties that define the scrollbar's state.
    // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
    // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
    // of the height of the view.
    // orientation can be either Qt.Vertical or Qt.Horizontal
    property real position: 0.5
    property real pageSize: 1.0
    property real step: 0
    property real barW: bar.width
    property int orientation : Qt.Horizontal

    signal moved


    // A light, semi-transparent background
    Rectangle {
        id: background
        anchors.fill: parent
        //radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "gray"

    }

    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        id:bar
        x: orientation == Qt.Vertical ? 1 : (scroolBar.position * (scroolBar.width-2) + 1 - width/2)
        y: orientation == Qt.Vertical ? (scroolBar.position * (scroolBar.height-2) + 1) : 1
        width: orientation == Qt.Vertical ? (parent.width-2) : (scroolBar.pageSize * (scroolBar.width-2))
        height: orientation == Qt.Vertical ? (scroolBar.pageSize * (scroolBar.height-2)) : (parent.height-2)
        radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "black"
        opacity: 0.6

    }

    Timer{
        id:timRes
        interval:50
        onTriggered: {
            var dist, perc
            if(mousearea.stato==="moving")
            {
                dist=mousearea.mouseX-mousearea.pos
                if (dist !== 0)
                {
                    perc=dist/scroolBar.width
                    var pos = scroolBar.position + perc
                    var newLimitSx = (pos * (scroolBar.width-2) + 1 - bar.width/2)
                    var newLimitDx = newLimitSx + (scroolBar.pageSize * (scroolBar.width-2))
                    if ((newLimitDx <  scroolBar.width-2)&&(newLimitSx > 0))
                    {
                        scroolBar.position = pos
                        scroolBar.step = perc
                    }
                    else
                    {
                        var newPos = (bar.x -1 + bar.width/2)/(scroolBar.width-2)
                        scroolBar.position = newPos
                        scroolBar.step = 0
                    }
                    scroolBar.moved()
                }
            }
        }
    }


    MouseArea {
        id: mousearea
        property string stato: "idle"
        property real pos: 0
        anchors.fill: bar
        onPressed: {
            mousearea.cursorShape = Qt.ClosedHandCursor
            stato="moving"
            pos=mouseX
            timRes.start()
        }
        onReleased:{
            mousearea.cursorShape = Qt.ArrowCursor
            stato="idle"
            timRes.stop()
        }
        onPositionChanged: {if(stato==="moving")timRes.start()}
        hoverEnabled: true

    }

}
