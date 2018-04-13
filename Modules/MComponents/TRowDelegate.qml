import QtQuick 2.5
import QtQuick.Controls 1.1
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import MComponents 1.0
Rectangle{
    property var itemData:Item{
        property bool selected:false
        property int column:0
        property int row:0
        property int elideMode:0
        property string value:""
    }
    height:itemData.selected?Math.round(layout.dimensionOf("TIF")*1.2):layout.dimensionOf("TIF")
    Behavior on height {NumberAnimation {duration: 200}}
}

