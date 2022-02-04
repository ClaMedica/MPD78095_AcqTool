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
    height:{
        var dim = layout.dimensionOf("TIF")
        if (isTouch)
            itemData.selected?Math.round(dim*1.2):dim
        else
            itemData.selected?Math.round(dim/2*1.2):dim/2
    }
        Behavior on height {NumberAnimation {duration: 200}}
}

