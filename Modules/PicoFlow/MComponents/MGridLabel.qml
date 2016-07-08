import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import MComponents 1.0

Grid{
    id:rootGridLabel
    property var model:["A","B","C","D"]
    property real labelSize: 3
    columns: 2
    function rows()
    {
        return Math.ceil(model.length/columns)
    }

    Repeater{
        id:rep
        model:rootGridLabel.model
        delegate:
            MLabel{
            id:lblWeight
            text: modelData
            height: rootGridLabel.height/rows()
            width: rootGridLabel.width/columns
            labelSize: rootGridLabel.labelSize
        }
    }
}
