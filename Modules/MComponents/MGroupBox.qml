import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

Rectangle{
    id:rootGroupBox
    property int labelSize:1
    property string title:"Gruppo 1"
    property Rectangle drawableRect:recDrawable
    signal titleClicked
    color:"orange"
    clip:true
    Rectangle{
        id:recDrawable
        anchors.top:recLabel.bottom
        anchors.left:recBorder.left
        anchors.right: recBorder.right
        anchors.bottom: recBorder.bottom
        anchors.margins: screenH*0.005
        color:"red"
        clip:true
    }
    Rectangle{
        id:recBorder
        anchors.fill: parent
        anchors.margins: screenH*0.005
        border.color: "yellow"
        border.width: screenH*0.001
        color:"white"
        clip:true
    }

    Rectangle{
        id:recLabel
        anchors.top: parent.top
        anchors.left:parent.left
        anchors.leftMargin: screenH*0.01

        color:rootGroupBox.color
        width: text.contentWidth*1.1
        height: text.contentHeight
        clip:true
        Text{
            id:text
            font.pixelSize: screenH*0.02*labelSize
            font.family:
            {
                if (PicoFlow)
                    if (layout !== undefined)
                        rootLabel.font.family = layout.value("FFamily")
                    else
                        rootLabel.font.family ="utopia"
                else if (layout !== undefined)
                    rootLabel.font.family = layout.value("FFamilyW")
                else
                    rootLabel.font.family = "Calibri"
            }
            anchors.centerIn: parent
        }
        MouseArea{
            id:area
            anchors.fill: parent
            onClicked: titleClicked()
        }
    }


}
