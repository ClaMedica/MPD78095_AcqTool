import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.3
import MComponents 1.0

Rectangle{
    id:rootPage
    property string prevPage: ""
    property string nextPage: ""
    property string name: ""
    property string backgroundImage: ""
    property var image: undefined
    property var keyboard: appKey
    property var keyboardNum: appKeyNum
    signal changePage(var page)
    signal backClicked
    signal nextClicked
    signal questionAnswered(bool accepted,var owner,var arg)
    color: "transparent"

    function previous() {
        if(prevPage !== "") {
            changePage(prevPage)
            backClicked()
        }
    }

    function next() {
        if(nextPage !== "") {
            changePage(nextPage)
            nextClicked()
        }
    }

    function askQuestion(owner, arg, question, answ1, answ2)
    {
        var mexBox = Qt.createQmlObject("import MComponents 1.0; MDialogYesNo{}", rootPage);
        mexBox.anchors.centerIn = rootPage
        mexBox.message = question
        mexBox.yesText = answ1
        mexBox.noText = answ2
        mexBox.width = rootPage.width*0.7
        mexBox.height = rootPage.height*0.7
        conQuestion.owner = owner
        conQuestion.arg = arg
        conQuestion.target = mexBox
        mexBox.open()
    }

    Connections {
        id: conQuestion
        property var owner
        property var arg
        ignoreUnknownSignals: true
        onAccepted:
        {
            questionAnswered(true, owner, arg)
            target.destroy()
        }
        onRejected:
        {
            questionAnswered(false, owner, arg)
            target.destroy()
        }
    }

    onBackgroundImageChanged: {
        if(backgroundImage !== "")
        {
            if(image !== undefined)
                image.destroy()
            var s = 'import QtQuick 2.5;'
            if(backgroundImage.search(".gif"))
                image = Qt.createQmlObject(s + "AnimatedImage{}", rootPage);
            else
                image = Qt.createQmlObject(s + "Image{}", rootPage);

            image.anchors.fill = rootPage
            image.source = backgroundImage
            image.z = -1
        }
        else
            if(image !== undefined)
                image.source = ""
    }

    MKeyboard {
        id:appKey
        y: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false
    }

    MKeyboardNum {
        id: appKeyNum
        visible: false
    }
}
