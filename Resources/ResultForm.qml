import QtQuick 2.3
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import MComponents 1.0
import QtQuick.Layouts 1.1
import QtQml 2.0

import "qrc:/Forms"
import "qrc:/Components"

MForm {

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    property var pagesLocal:[]
    property var pagesSameAna:[]

    property int buttonTable:0
    id : resultForm
    width : forAna.width
    height : forAna.height
    color : "steelblue"
    visible: false

    //@@@@@@@@@@    Functions      @@@@@@@@@@
    function clickButtonSameAna(btnId) {
        for (var i = 0; i < pagesSameAna.length; i++) {
            if (pagesSameAna[i].visible)
                pagesSameAna[i].visible = false;
        }
        pagesSameAna[btnId].visible = true;
    }

    function clickButton(btnId){
        for (var i = 0; i < pagesLocal.length; i++) {
            if (pagesLocal[i].visible)
                pagesLocal[i].visible = false;
        }
        pagesLocal[btnId].visible = true;
    }

    function loadPageAnalysis()
    {
        if (mngData.getNumAnaFlwAdv() > 0)
        {
            //pages for flowmetry
            var rectf = Qt.createQmlObject('import QtQuick 2.5; Rectangle {}',this)
            rectf.width = forAna.width
            rectf.height = forAna.height
            rectf.color = "transparent"
            rectf.border.color = "blue"
            rectf.border.width = 2
            rectf.anchors.right = resultForm.right
            rectf.anchors.top = resultForm.top
           // rectf.anchors.topMargin = 5
            rectf.anchors.rightMargin = resultForm.width/2 - rectf.width/2

            var namePage = ""
            var nameNomo = ""
            var btnTopMargin = 20
            //var btnLeftMargin = 20
            var firstPage = pagesLocal.length
            var firstPageSameAna = pagesSameAna.length

            //salto la 0 perchè è la media
            for (var i = 1; i < mngData.getNumAnaFlwAdv(); i++)
            {
                if (mngData.getNumAnaFlwAdv() === 2)
                    namePage = "Flow"
                else
                    namePage = "Flow " + i.toString()

                var btnLeftMargin = 20
                if (mngData.getNumAnaFlwAdv() !== 2)
                {
                    var buttonfn = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectf)
                    buttonfn.buttonId = pagesSameAna.length
                    buttonfn.anchors.right = rectf.right
                    buttonfn.anchors.top = rectf.top
                    buttonfn.anchors.topMargin = btnTopMargin
                    buttonfn.anchors.rightMargin = 20
                    buttonfn.myText = namePage
                    buttonfn.clicked.connect(clickButtonSameAna)

                    btnTopMargin += 50
                }

                var rectdata = Qt.createQmlObject('import QtQuick 2.5; Rectangle {}',rectf)
                rectdata.width = forAna.width - 200
                rectdata.height = forAna.height
                rectdata.color = "transparent"
                rectdata.border.color = "transparent"
                rectdata.border.width = 2
                rectdata.anchors.left = rectf.left
                rectdata.anchors.top = rectf.top
                rectdata.visible = false
                pagesSameAna[pagesSameAna.length] = rectdata

                var tablefn =  Qt.createQmlObject('import "qrc:/Components"; MAnaTableView {}', rectdata)
                tablefn.width = 350
                tablefn.anchors.left = rectdata.left
                tablefn.anchors.top = rectdata.top
                tablefn.anchors.topMargin = 20
                tablefn.anchors.leftMargin = 50
                tablefn.model = mngData.getFlowDatas(i).datasInfo();
                tablefn.populate()
                tablefn.visible = true

                var buttonT = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonT.buttonId = pagesLocal.length
                buttonT.anchors.bottom = rectdata.bottom
                buttonT.anchors.left = rectdata.left
                buttonT.anchors.bottomMargin = 100
                buttonT.anchors.leftMargin = btnLeftMargin
                buttonT.myText = qsTr("Table")
                buttonT.clicked.connect(clickButton)
                resultForm.buttonTable = buttonT.buttonId
                // btnLeftMargin += 110
                pagesLocal.push(tablefn)

                //nomogrammi flussimetria
                //liverpool Qmax
                nameNomo = mngData.getFlowDatas(i).getLiverpoolMax().getTitle()
                var LQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQM.clip = true
                LQM.height = 300
                LQM.width = 450
                LQM.anchors.left = rectdata.left
                LQM.anchors.top = rectdata.top
                LQM.anchors.topMargin = 20
                LQM.anchors.leftMargin = 20
                LQM.xMin = mngData.getFlowDatas(i).getLiverpoolMax().getXmin()
                LQM.xMax = mngData.getFlowDatas(i).getLiverpoolMax().getXmax()
                LQM.yMin = mngData.getFlowDatas(i).getLiverpoolMax().getYmin()
                LQM.yMax = mngData.getFlowDatas(i).getLiverpoolMax().getYmax()
                LQM.nome = nameNomo
                LQM.xPoint = mngData.getFlowDatas(i).getLiverpoolMax().getDatoX()
                LQM.yPoint = mngData.getFlowDatas(i).getLiverpoolMax().getDatoY()
                LQM.udmX =  mngData.getFlowDatas(i).getLiverpoolMax().getUnitx()
                LQM.udmY =  mngData.getFlowDatas(i).getLiverpoolMax().getUnity()
                LQM.tracksWidth = [2,2,1,1,1,1,2]
                LQM.traksToDraw =  mngData.getFlowDatas(i).getLiverpoolMax().getTracce()
                LQM.tracksColors =  mngData.getFlowDatas(i).getLiverpoolMax().getColors()
                LQM.drawTracks()
                LQM.visible = false

                //button
                var buttonLQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQM.buttonId = pagesLocal.length
                buttonLQM.anchors.bottom = rectdata.bottom
                buttonLQM.anchors.left = rectdata.left
                buttonLQM.anchors.bottomMargin = 60
                buttonLQM.anchors.leftMargin = btnLeftMargin
                buttonLQM.myText = nameNomo
                buttonLQM.clicked.connect(clickButton)
                btnLeftMargin += buttonLQM.width + 5
                pagesLocal.push(LQM)

                //liverpool QAve
                nameNomo = mngData.getFlowDatas(1).getLiverpoolAve().getTitle()
                var LQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQA.clip = true
                LQA.height = 300
                LQA.width = 450
                LQA.anchors.left = rectdata.left
                LQA.anchors.top = rectdata.top
                LQA.anchors.topMargin = 20
                LQA.anchors.leftMargin = 20
                LQA.xMin = mngData.getFlowDatas(i).getLiverpoolAve().getXmin()
                LQA.xMax = mngData.getFlowDatas(i).getLiverpoolAve().getXmax()
                LQA.yMin = mngData.getFlowDatas(i).getLiverpoolAve().getYmin()
                LQA.yMax = mngData.getFlowDatas(i).getLiverpoolAve().getYmax()
                LQA.nome = nameNomo
                LQA.xPoint = mngData.getFlowDatas(i).getLiverpoolAve().getDatoX()
                LQA.yPoint = mngData.getFlowDatas(i).getLiverpoolAve().getDatoY()
                LQA.udmX =  mngData.getFlowDatas(i).getLiverpoolAve().getUnitx()
                LQA.udmY =  mngData.getFlowDatas(i).getLiverpoolAve().getUnity()
                LQA.tracksWidth = [2,2,1,1,1,1,2]
                LQA.traksToDraw =  mngData.getFlowDatas(i).getLiverpoolAve().getTracce()
                LQA.tracksColors =  mngData.getFlowDatas(i).getLiverpoolAve().getColors()
                LQA.drawTracks()
                LQA.visible = false

                //button
                var buttonLQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQA.buttonId = pagesLocal.length
                buttonLQA.anchors.bottom = rectdata.bottom
                buttonLQA.anchors.left = rectdata.left
                buttonLQA.anchors.bottomMargin = 60
                buttonLQA.anchors.leftMargin = btnLeftMargin
                buttonLQA.myText = nameNomo
                buttonLQA.clicked.connect(clickButton)
                btnLeftMargin += buttonLQA.width + 5
                pagesLocal.push(LQA)

                //siroky QMax
                nameNomo = mngData.getFlowDatas(1).getSirokyMax().getTitle()
                if (nameNomo !== "" )
                {
                    var SQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    SQM.clip = true
                    SQM.height = 300
                    SQM.width = 450
                    SQM.anchors.left = rectdata.left
                    SQM.anchors.top = rectdata.top
                    SQM.anchors.topMargin = 20
                    SQM.anchors.leftMargin = 20
                    SQM.xMin = mngData.getFlowDatas(i).getSirokyMax().getXmin()
                    SQM.xMax = mngData.getFlowDatas(i).getSirokyMax().getXmax()
                    SQM.yMin = mngData.getFlowDatas(i).getSirokyMax().getYmin()
                    SQM.yMax = mngData.getFlowDatas(i).getSirokyMax().getYmax()
                    SQM.nome = nameNomo
                    SQM.xPoint = mngData.getFlowDatas(i).getSirokyMax().getDatoX()
                    SQM.yPoint = mngData.getFlowDatas(i).getSirokyMax().getDatoY()
                    SQM.udmX =  mngData.getFlowDatas(i).getSirokyMax().getUnitx()
                    SQM.udmY =  mngData.getFlowDatas(i).getSirokyMax().getUnity()
                    SQM.tracksWidth = [1,2,1,1]
                    SQM.traksToDraw =  mngData.getFlowDatas(i).getSirokyMax().getTracce()
                    SQM.tracksColors =  mngData.getFlowDatas(i).getSirokyMax().getColors()
                    SQM.drawTracks()
                    //bande colorate
                    SQM.numBande = 2
                    SQM.numPunti = mngData.getFlowDatas(i).getSirokyMax().getNumPunti()+1
                    SQM.polygonXpoints = mngData.getFlowDatas(i).getSirokyMax().getXpoints()
                    SQM.polygonYpoints = mngData.getFlowDatas(i).getSirokyMax().getYpoints()
                    SQM.visible = false

                    //button
                    var buttonSQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonSQM.buttonId = pagesLocal.length
                    buttonSQM.anchors.bottom = rectdata.bottom
                    buttonSQM.anchors.left = rectdata.left
                    buttonSQM.anchors.bottomMargin = 60
                    buttonSQM.anchors.leftMargin = btnLeftMargin
                    buttonSQM.myText = nameNomo
                    buttonSQM.clicked.connect(clickButton)
                    btnLeftMargin += buttonSQM.width + 5
                    pagesLocal.push(SQM)

                    //siroky QAve
                    nameNomo = mngData.getFlowDatas(i).getSirokyAve().getTitle()

                    var SQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    SQA.clip = true
                    SQA.height = 300
                    SQA.width = 450
                    SQA.anchors.left = rectdata.left
                    SQA.anchors.top = rectdata.top
                    SQA.anchors.topMargin = 20
                    SQA.anchors.leftMargin = 20
                    SQA.xMin = mngData.getFlowDatas(i).getSirokyAve().getXmin()
                    SQA.xMax = mngData.getFlowDatas(i).getSirokyAve().getXmax()
                    SQA.yMin = mngData.getFlowDatas(i).getSirokyAve().getYmin()
                    SQA.yMax = mngData.getFlowDatas(i).getSirokyAve().getYmax()
                    SQA.nome = nameNomo
                    SQA.xPoint = mngData.getFlowDatas(i).getSirokyAve().getDatoX()
                    SQA.yPoint = mngData.getFlowDatas(i).getSirokyAve().getDatoY()
                    SQA.udmX =  mngData.getFlowDatas(i).getSirokyAve().getUnitx()
                    SQA.udmY =  mngData.getFlowDatas(i).getSirokyAve().getUnity()
                    SQA.tracksWidth = [1,2,1,1,1]
                    SQA.traksToDraw =  mngData.getFlowDatas(i).getSirokyAve().getTracce()
                    SQA.tracksColors =  mngData.getFlowDatas(i).getSirokyAve().getColors()
                    SQA.drawTracks()
                    SQA.numBande = 2
                    SQA.numPunti = mngData.getFlowDatas(i).getSirokyAve().getNumPunti()+1
                    SQA.polygonXpoints = mngData.getFlowDatas(i).getSirokyAve().getXpoints()
                    SQA.polygonYpoints = mngData.getFlowDatas(i).getSirokyAve().getYpoints()
                    SQA.visible = false

                    //button
                    var buttonSQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonSQA.buttonId = pagesLocal.length
                    buttonSQA.anchors.bottom = rectdata.bottom
                    buttonSQA.anchors.left = rectdata.left
                    buttonSQA.anchors.bottomMargin = 60
                    buttonSQA.anchors.leftMargin = btnLeftMargin
                    buttonSQA.myText = nameNomo
                    buttonSQA.clicked.connect(clickButton)
                    //btnLeftMargin += 110
                    pagesLocal.push(SQA)
                }
            }

            pagesSameAna[firstPageSameAna].visible = true
            timPrint.start()
        }
    }

    Timer{
        id:timPrint
        interval:1000
        onTriggered: {
            mngData.startPrint()
        }
    }

    //@@@@@@@@@@    Graphics      @@@@@@@@@@

    MButton{
        id: btnPrint
        text: qsTr("print")
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width:parent.width*0.2
        height:parent.height*0.08
        labelSize: layout.value("F4")
        onClicked: {
            mngData.sendToPrint()
        }
    }

    MButton {
        id:btnBack
        text: qsTr("back to graphs")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        width:parent.width*0.2
        height:parent.height*0.08
        labelSize: layout.value("F4")
        onClicked: {            
            for (var i = 0; i < pagesLocal.length; i++) {
                if (pagesLocal[i].visible)
                    pagesLocal[i].visible = false;
            }
            forAna.visible = true
            resultForm.visible = false
        }
    }
}

