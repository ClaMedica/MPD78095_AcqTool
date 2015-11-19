import QtQuick 2.3
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import MPlotModule 1.0
import QtQuick.Layouts 1.1
import QtQml 2.0

import "qrc:/Forms"
import "qrc:/Components"

Rectangle {

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    property var pagesLocal:[]
    property var pagesSameAna:[]

    id : resultForm
    width : forAna.width
    height : forAna.height
    color : "steelblue"
    visible: false


    //@@@@@@@@@@    Functions      @@@@@@@@@@
    function clickButtonSameAna(btnId){
        for (var i=0; i<pagesSameAna.length; i++){
            if (pagesSameAna[i].visible)
                pagesSameAna[i].visible = false;
        }
        pagesSameAna[btnId].visible = true;

    }

    function clickButton(btnId){
        for (var i=0; i<pagesLocal.length; i++){
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
            rectf.width = 640//forAna.width - 200
            rectf.height = 480 //forAna.height
            rectf.color = "steelblue"
            rectf.border.color = "blue"
            rectf.border.width = 2
            rectf.anchors.right = resultForm.right
            rectf.anchors.top = resultForm.top
            rectf.anchors.topMargin = 20
            rectf.anchors.rightMargin = resultForm.width/2 - rectf.width/2

            var namePage = ""
            var nameNomo = ""
            var btnTopMargin = 20
            //var btnLeftMargin = 20
            var firstPage = pagesLocal.length
            var firstPageSameAna = pagesSameAna.length

            for (var i=0; i<mngData.getNumAnaFlwAdv(); i++)
            {
                if (mngData.getNumAnaFlwAdv() === 1)
                    namePage = "Flow"
                else
                    namePage = "Flow " + i.toString()

                var btnLeftMargin = 20
                if (mngData.getNumAnaFlwAdv() !== 1)
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
                rectdata.width = 480
                rectdata.height = 480
                rectdata.color = "steelblue"
                rectdata.border.color = "red"
                rectdata.border.width = 2
                rectdata.anchors.left = rectf.left
                rectdata.anchors.top = rectf.top
                rectdata.visible = false
                pagesSameAna[pagesSameAna.length] = rectdata

                var tablefn =  Qt.createQmlObject('import "qrc:/Components"; MAnaTableView {}', rectdata)
                tablefn.width = 300
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
                buttonT.anchors.bottomMargin = 20
                buttonT.anchors.leftMargin = btnLeftMargin
                buttonT.myText = qsTr("Table")
                buttonT.clicked.connect(clickButton)
                btnLeftMargin += 110
                pagesLocal[pagesLocal.length] = tablefn

                //nomogrammi flussimetria
                //liverpool Qmax
                nameNomo = mngData.getFlowDatas(1).getLiverpoolMax().getTitle()
                var LQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQM.clip = true
                LQM.height = 250
                LQM.width = 380
                LQM.anchors.left = rectdata.left
                LQM.anchors.top = rectdata.top
                LQM.anchors.topMargin = 20
                LQM.anchors.leftMargin = 50
                LQM.xMin = mngData.getFlowDatas(1).getLiverpoolMax().getXmin()
                LQM.xMax = mngData.getFlowDatas(1).getLiverpoolMax().getXmax()
                LQM.yMin = mngData.getFlowDatas(1).getLiverpoolMax().getYmin()
                LQM.yMax = mngData.getFlowDatas(1).getLiverpoolMax().getYmax()
                LQM.nome = nameNomo
                LQM.xPoint = mngData.getFlowDatas(1).getLiverpoolMax().getDatoX()
                LQM.yPoint = mngData.getFlowDatas(1).getLiverpoolMax().getDatoY()
                LQM.udmX =  mngData.getFlowDatas(1).getLiverpoolMax().getUnitx()
                LQM.udmY =  mngData.getFlowDatas(1).getLiverpoolMax().getUnity()
                LQM.traksToDraw =  mngData.getFlowDatas(1).getLiverpoolMax().getTracce()
                LQM.tracksColors =  mngData.getFlowDatas(1).getLiverpoolMax().getColors()
                LQM.drawTracks()
                LQM.visible = false

                //button
                var buttonLQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQM.buttonId = pagesLocal.length
                buttonLQM.anchors.bottom = rectdata.bottom
                buttonLQM.anchors.left = rectdata.left
                buttonLQM.anchors.bottomMargin = 20
                buttonLQM.anchors.leftMargin = btnLeftMargin
                buttonLQM.myText = nameNomo
                buttonLQM.clicked.connect(clickButton)
                btnLeftMargin += 110
                pagesLocal[pagesLocal.length] = LQM

                //liverpool QAve
                nameNomo = mngData.getFlowDatas(1).getLiverpoolAve().getTitle()
                var LQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQA.clip = true
                LQA.height = 250
                LQA.width = 380
                LQA.anchors.left = rectdata.left
                LQA.anchors.top = rectdata.top
                LQA.anchors.topMargin = 20
                LQA.anchors.leftMargin = 50
                LQA.xMin = mngData.getFlowDatas(1).getLiverpoolAve().getXmin()
                LQA.xMax = mngData.getFlowDatas(1).getLiverpoolAve().getXmax()
                LQA.yMin = mngData.getFlowDatas(1).getLiverpoolAve().getYmin()
                LQA.yMax = mngData.getFlowDatas(1).getLiverpoolAve().getYmax()
                LQA.nome = nameNomo
                LQA.xPoint = mngData.getFlowDatas(1).getLiverpoolAve().getDatoX()
                LQA.yPoint = mngData.getFlowDatas(1).getLiverpoolAve().getDatoY()
                LQA.udmX =  mngData.getFlowDatas(1).getLiverpoolAve().getUnitx()
                LQA.udmY =  mngData.getFlowDatas(1).getLiverpoolAve().getUnity()
                LQA.traksToDraw =  mngData.getFlowDatas(1).getLiverpoolAve().getTracce()
                LQA.tracksColors =  mngData.getFlowDatas(1).getLiverpoolAve().getColors()
                LQA.drawTracks()
                LQA.visible = false

                //button
                var buttonLQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQA.buttonId = pagesLocal.length
                buttonLQA.anchors.bottom = rectdata.bottom
                buttonLQA.anchors.left = rectdata.left
                buttonLQA.anchors.bottomMargin = 20
                buttonLQA.anchors.leftMargin = btnLeftMargin
                buttonLQA.myText = nameNomo
                buttonLQA.clicked.connect(clickButton)
                btnLeftMargin += 110
                pagesLocal[pagesLocal.length] = LQA


            }

            pagesSameAna[firstPageSameAna].visible = true

        }
    }

    //@@@@@@@@@@    Graphics      @@@@@@@@@@
    Button {
        id:btnBack
        text: qsTr("back to graphs")
        width: 100

        anchors.right: resultForm.right
        anchors.bottom: resultForm.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        onClicked: {
            forAna.visible = true
            resultForm.visible = false
        }
    }
}

