import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import "Images"

Rectangle {
    id:rootCheck
    color:"transparent"
    anchors.top: parent.top
    anchors.right: parent.right
    anchors.bottomMargin: 20
    property var    checkedM:   ["false","false"]
    property var    model:      ["primo","secondo"]
    property bool   setCheck:   false

    onModelChanged: tmmodel.start()
    onCheckedMChanged: if (!setCheck) tmchecked.start()

    signal clicked

    Timer {
        id: tmmodel
        onTriggered: {
            if (chekboxes.count > 0)
                for (var i=0; i<chekboxes.count; i++)
                    chekboxes.itemAt(i).text = rootCheck.model[i]
            else
                tmmodel.start()
        }
    }

    Timer {
        id: tmchecked
        onTriggered: {
            if (chekboxes.count > 0) {
                setCheck = true
                for (var i=0; i<chekboxes.count; i++)
                    if (checkedM[i] === "false")
                        chekboxes.itemAt(i).checked = false
                    else
                        chekboxes.itemAt(i).checked = true
            }
            else
                tmchecked.start()
        }
    }

    Column {
        spacing: rootCheck.height/chekboxes.count/4

        Repeater {
            id: chekboxes
            model: rootCheck.model.length

            delegate: MCheckBox {
                height: rootCheck.height/chekboxes.count
                width:  rootCheck.width/chekboxes.count
                pixelSize:  Screen.height*0.02
                onClicked: {
                    rootCheck.clicked()
                    if (checked) {
                        checkedM[index] = "true"
                        for (var i=0;i<chekboxes.count;i++) {
                            if (i !== index) {
                                chekboxes.itemAt(i).checked = false
                                checkedM[i] = "false"
                            }
                        }
                    }
                    else
                    {
                        checkedM[index] = "false"
                        if (index !== 0) {
                            chekboxes.itemAt(0).checked = true
                            checkedM[0] = "true"
                        }
                        else {
                            chekboxes.itemAt(1).checked = true
                            checkedM[1] = "true"
                        }
                    }
                }
            }
        }
    }
}
