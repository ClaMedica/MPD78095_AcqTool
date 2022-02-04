import QtQuick 2.0

//Item {
//   id: scrollbar

//    property Flickable flk : undefined
//    property int basicWidth: 10
//    property int expandedWidth: 20
//    property alias color : scrl.color
//    property alias radius : scrl.radius

//    width: basicWidth
//    anchors.right: flk.right;
//    anchors.top: flk.top
//    anchors.bottom: flk.bottom

//    clip: true
//    visible: flk.visible
//    z:1

//    Binding {
//        target: scrollbar
//        property: "width"
//        value: expandedWidth
//        when: ma.drag.active || ma.containsMouse
//    }
//    Behavior on width {NumberAnimation {duration: 150}}

//    Rectangle {
//        id: scrl
//        clip: true
//        anchors.left: parent.left
//        anchors.right: parent.right
//        height: flk.visibleArea.heightRatio * flk.height
//        visible: flk.visibleArea.heightRatio < 1.0
//        radius: 10
//        color: "gray"

//        opacity: ma.pressed ? 1 : ma.containsMouse ? 0.65 : 0.4
//        Behavior on opacity {NumberAnimation{duration: 150}}

//        Binding {
//            target: scrl
//            property: "y"
//            value: !isNaN(flk.visibleArea.heightRatio) ? (ma.drag.maximumY * flk.contentY) / (flk.contentHeight * (1 - flk.visibleArea.heightRatio)) : 0
//            when: !ma.drag.active
//        }

//        Binding {
//            target: flk
//            property: "contentY"
//            value: ((flk.contentHeight * (1 - flk.visibleArea.heightRatio)) * scrl.y) / ma.drag.maximumY
//            when: ma.drag.active && flk !== undefined
//        }

//        MouseArea {
//            id: ma
//            anchors.fill: parent
//            hoverEnabled: true
//            drag.target: parent
//            drag.axis: Drag.YAxis
//            drag.minimumY: 0
//            drag.maximumY: flk.height - scrl.height
//            preventStealing: true
//        }
//    }
//}


Item {
    id: scrollBar

    // The properties that define the scrollbar's state.
    // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
    // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
    // of the height of the view.
    // orientation can be either Qt.Vertical or Qt.Horizontal
    property real position: 0.5
    property real pageSize: 1.0
    property int orientation : Qt.Horizontal
    height:10
    function writeSomething() {
        console.log("stodisegnando???", position, pageSize)
    }

    // A light, semi-transparent background
    Rectangle {
        id: background
        anchors.fill: parent
        radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "red"
        opacity: 0.3
    }

    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        x: orientation == Qt.Vertical ? 1 : (scrollBar.position * (scrollBar.width-2) + 1)
        y: orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height-2) + 1) : 1
        width: orientation == Qt.Vertical ? (parent.width-2) : (scrollBar.pageSize * (scrollBar.width-2))
        height: orientation == Qt.Vertical ? (scrollBar.pageSize * (scrollBar.height-2)) : (parent.height-2)
        radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "black"
        opacity: 0.7

    }
}
