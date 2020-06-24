import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1

import MComponents 1.0
Rectangle  {
    id: mainRect
    visible: true
    width: 280; height: 280
    color: "transparent"

    property string time: h.text + m.text

    RowLayout {
        id: digital
        anchors.centerIn: parent

        Text {
            id: h
            font.pixelSize: 30
            font.bold: true
            color: "blue"
            text: dentroAM.choiceActive?dentroAM.currentItem.text:fuoriPM.currentItem.text

            SequentialAnimation {
                id: animaHour
                running: false
                OpacityAnimator {
                    target: h
                    to: 1
                    duration: 800
                    alwaysRunToEnd: true
                    easing.type: Easing.InOutQuad
                }
                OpacityAnimator {
                    target: h
                    to: 0
                    duration: 800
                    alwaysRunToEnd: true
                    easing.type: Easing.InOutQuad
                }
                onStopped: h.opacity = 1
            }

            BuzzMouseArea {
                anchors.fill: parent
                onClicked: {
                    minuti.visible = false
                    dentroAM.visible = true
                    animaHour.start()
                    animaMinute.stop()
                }
            }
        }
        Text {
            id: div
            font.pixelSize: 30
            font.bold: true
            color: "blue"
            text: qsTr(":")
        }
        Text {
            id: m
            font.pixelSize: 30
            font.bold: true
            color: "blue"
            text: minuti.currentIndex<10?"0"+minuti.currentIndex:minuti.currentIndex

            SequentialAnimation {
                id: animaMinute
                running: false
                OpacityAnimator {
                    target: m
                    to: 1
                    duration: 800
                    alwaysRunToEnd: true
                    easing.type: Easing.InOutQuad
                }
                OpacityAnimator {
                    target: m
                    to: 0
                    duration: 800
                    alwaysRunToEnd: true
                    easing.type: Easing.InOutQuad
                }
                onStopped: m.opacity = 1
                onStarted: minuti.visible = true
            }

            BuzzMouseArea {
                anchors.fill: parent
                onClicked: {
                    animaHour.stop()
                    animaMinute.start()
                    minuti.visible = true
                    dentroAM.visible = false
                }
            }
        }
    }

    PathView {          // hours path
        id: dentroAM
        model: 12

        interactive: false
        highlightRangeMode:  PathView.NoHighlightRange
        property bool choiceActive: true
        visible: true

        highlight: Rectangle {
            width: 30 * 1.5
            height: width
            radius: width / 2
            border.color: "darkgray"
            color: "steelblue"
            visible: dentroAM.choiceActive
        }

        delegate: Item {
            width: 30
            height: 30
            property bool currentItem: PathView.view.currentIndex == index
            property alias text : textHouAM.text
            Text {
                id: textHouAM
                anchors.centerIn: parent
                font.pixelSize: 24
                font.bold: currentItem
                text: index + 1
                color: currentItem ? "black" : "white"
            }

            BuzzMouseArea {
                anchors.fill: parent
                enabled: dentroAM.visible
                onClicked: {
                    dentroAM.currentIndex = index
                    dentroAM.choiceActive = true
                    fuoriPM.choiceActive = false
                    dentroAM.visible = false
                    animaMinute.start()
                    animaHour.stop()
                }
            }
        }

        path: Path {
            startX: 180; startY: 70
            PathArc {
                x: 100; y: 210
                radiusX: 40; radiusY: 40
                useLargeArc: false
            }
            PathArc {
                x: 180; y: 70
                radiusX: 40; radiusY: 40
                useLargeArc: false
            }
        }
    }

    PathView {          // hours path
        id: fuoriPM
        model: 12

        interactive: false
        highlightRangeMode:  PathView.NoHighlightRange
        property bool choiceActive: false
        visible: dentroAM.visible

        highlight: Rectangle {
            width: 30 * 1.5
            height: width
            radius: width / 2
            border.color: "darkgray"
            color: "steelblue"
            visible: fuoriPM.choiceActive
        }

        delegate: Item {
            width: 30
            height: 30
            property bool currentItem: PathView.view.currentIndex == index
            property alias text : textHouPM.text
            Text {
                id: textHouPM
                anchors.centerIn: parent
                font.pixelSize: 18
                font.bold: currentItem
                text: index===11?"00":index + 13
                color: currentItem ? "black" : "white"
            }

            BuzzMouseArea {
                anchors.fill: parent
                enabled: fuoriPM.visible
                onClicked: {
                    fuoriPM.currentIndex = index
                    dentroAM.choiceActive = false
                    fuoriPM.choiceActive = true
                    dentroAM.visible = false
                    animaMinute.start()
                    animaHour.stop()
                }
            }
        }

        path: Path {
            startX: 200; startY: 40
            PathArc {
                x: 80; y: 240
                radiusX: 110; radiusY: 110
                useLargeArc: false
            }
            PathArc {
                x: 200; y: 40
                radiusX: 110; radiusY: 110
                useLargeArc: false
            }
        }

    }

    PathView {          // minutes path
        id: minuti
        model: 60
        interactive: true
        highlightRangeMode:  PathView.NoHighlightRange

        property string valore: currentItem.text
        property bool choiceActive: true
        visible: false

        highlight: Rectangle {
            width: 30 * 1.5
            height: width
            radius: width / 2
            border.color: "darkgray"
            color: "lightgreen"
            visible: minuti.choiceActive
        }

        delegate: Item {
            width: 30
            height: 30
            property bool currentItem: PathView.view.currentIndex == index
            property alias text : textMin.text
            Text {
                id: textMin
                anchors.centerIn: parent
                font.pixelSize: 18
                text: {
                    if (index%2===0)
                        if (index < 10)
                            text = "0"+index
                        else
                            text = index
                    else
                       text = "-"

                }
                color: currentItem ? "black" : "white"
            }

            BuzzMouseArea {
                anchors.fill: parent
                enabled: minuti.visible
                onClicked:  {
                    minuti.currentIndex = index
                    animaMinute.stop()
                }
            }
        }

        path: Path {
            startX: 140; startY: 30
            PathArc {
                x: 140; y: 290
                radiusX: 65; radiusY: 40
                useLargeArc: true
            }
            PathArc {
                x: 140; y: 30
                radiusX: 65; radiusY: 40
                useLargeArc: true
            }
        }
    }

    // to set current time!
    Component.onCompleted:
    {
        var d = new Date();
        var ora = d.getUTCHours()
        if (ora > 12 || ora === 0) {
            fuoriPM.choiceActive = true
            dentroAM.choiceActive = false
            fuoriPM.currentIndex = ora - 1
        }
        else
            dentroAM.currentIndex = ora - 1
        minuti.currentIndex = d.getMinutes()

        animaMinute.loops = Animation.Infinite
        animaHour.loops = Animation.Infinite
        animaHour.start()
    }
}
