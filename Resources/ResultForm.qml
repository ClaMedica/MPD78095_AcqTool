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
    property var buttons:[]
    property var rectf
    property int buttonTable:0
    id : resultForm
    anchors.fill: parent
    color : "steelblue"
    visible: false

    onVisibleChanged: {
        if (visible && !PicoFlow) {
            comboReport.listReports = []
            var referti = mngData.getListReports()
            for (var i=0; i<referti.length;i++)
                comboReport.listReports.push(referti[i])
        }

    }

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
            if (pagesLocal[i].visible) {
                pagesLocal[i].visible = false;
                pagesLocal[i].focus = false;
            }
        }
        pagesLocal[btnId].visible = true;
        pagesLocal[btnId].focus = true;
    }

    function disablebuttons(__disable){
        if (__disable){
            btnReport.enabled = false
            btnExit.enabled = false
        }
        else {
            btnReport.enabled = true
            btnExit.enabled = true
        }
    }

    function loadPageAnalysis()
    {
        if (mngData.getNumAnaFlwAdv() > 0)
        {
            //pages for flowmetry
            rectf = Qt.createQmlObject('import QtQuick 2.5; Rectangle {}',this)
            rectf.width = forAna.width
            rectf.height = forAna.height
            rectf.anchors.fill = parent
            rectf.color = "transparent"
            rectf.border.color = "transparent"//layout.value("THC2")//"blue"
            rectf.border.width = 2

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

                var btnLeftMargin = 10
                if (mngData.getNumAnaFlwAdv() !== 2)
                {
                    var buttonfn = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectf)
                    buttonfn.buttonId = pagesSameAna.length
                    buttonfn.anchors.right = rectf.right
                    buttonfn.anchors.top = rectf.top
                    buttonfn.anchors.topMargin = btnTopMargin
                    buttonfn.anchors.rightMargin = 20
                    buttonfn.height=rectf.height*0.06
                    buttonfn.myText = namePage
                    buttonfn.clicked.connect(clickButtonSameAna)

                    btnTopMargin += 50
                }

                var rectdata = Qt.createQmlObject('import QtQuick 2.5; Rectangle {}',rectf)
                rectdata.width = rectf.width - rectf.width/10
                rectdata.height = rectf.height
                rectdata.color = "transparent"
                rectdata.border.color = "transparent"
                rectdata.border.width = 2
                rectdata.anchors.left = rectf.left
                rectdata.anchors.top = rectf.top
                rectdata.visible = false
                pagesSameAna[pagesSameAna.length] = rectdata

                var tablefn =  Qt.createQmlObject('import "qrc:/Components"; MAnaTableView {}', rectdata)
                tablefn.height = rectf.height*grafic.valueOf("TableRis","height")
                tablefn.width = rectf.width*grafic.valueOf("TableRis","width")
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
                buttonT.anchors.bottomMargin = rectf.height/5
                buttonT.anchors.leftMargin = btnLeftMargin
                buttonT.width = screenW*grafic.valueOf("Button","width")
                buttonT.height = screenH*grafic.valueOf("Button","height")
                buttonT.labelSize= layout.value("F4")
                buttonT.myText = qsTr("Table")
                buttonT.clicked.connect(clickButton)
                resultForm.buttonTable = buttonT.buttonId
                // btnLeftMargin += 110
                pagesLocal.push(tablefn)
                buttons.push(buttonT)

                //nomogrammi flussimetria
                //liverpool Qmax
                nameNomo = mngData.getFlowDatas(i).getLiverpoolMax().getTitle()
                var LQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQM.clip = true
                LQM.height = screenH*grafic.valueOf("Nomogrammi","height")
                LQM.width = screenW*grafic.valueOf("Nomogrammi","width")
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

                if (LQM.xPoint > LQM.xMin && LQM.xPoint < LQM.xMax && LQM.yPoint > LQM.yMin && LQM.yPoint < LQM.yMax)
                    LQM.isVis = true
                else
                    LQM.isVis = false

                LQM.udmX =  mngData.getFlowDatas(i).getLiverpoolMax().getUnitx()
                LQM.udmY =  mngData.getFlowDatas(i).getLiverpoolMax().getUnity()
                LQM.tracksWidth = [2,2,1,1,1,1,2]
                LQM.traksToDraw =  mngData.getFlowDatas(i).getLiverpoolMax().getTracce()
                LQM.tracksColors =  mngData.getFlowDatas(i).getLiverpoolMax().getColors()
                LQM.drawTracks()
                LQM.visible = false
                LQM.saveImgNomogramma()

                //button
                var buttonLQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQM.buttonId = pagesLocal.length
                buttonLQM.anchors.bottom = rectdata.bottom
                buttonLQM.anchors.left = rectdata.left
                buttonLQM.anchors.bottomMargin = rectf.height/9
                buttonLQM.anchors.leftMargin = btnLeftMargin
                buttonLQM.width = screenW*grafic.valueOf("Button","width")
                buttonLQM.height = screenH*grafic.valueOf("Button","height")
                buttonLQM.labelSize= layout.value("F4")
                buttonLQM.myText = nameNomo
                buttonLQM.clicked.connect(clickButton)
                btnLeftMargin += buttonLQM.width + 5
                pagesLocal.push(LQM)
                buttons.push(buttonLQM)

                //liverpool QAve
                nameNomo = mngData.getFlowDatas(1).getLiverpoolAve().getTitle()
                var LQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                LQA.clip = true
                LQA.height = screenH*grafic.valueOf("Nomogrammi","height")
                LQA.width = screenW*grafic.valueOf("Nomogrammi","width")
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

                if (LQA.xPoint > LQA.xMin && LQA.xPoint < LQA.xMax && LQA.yPoint > LQA.yMin && LQA.yPoint < LQA.yMax)
                    LQA.isVis = true
                else
                    LQA.isVis = false
                LQA.udmX =  mngData.getFlowDatas(i).getLiverpoolAve().getUnitx()
                LQA.udmY =  mngData.getFlowDatas(i).getLiverpoolAve().getUnity()
                LQA.tracksWidth = [2,2,1,1,1,1,2]
                LQA.traksToDraw =  mngData.getFlowDatas(i).getLiverpoolAve().getTracce()
                LQA.tracksColors =  mngData.getFlowDatas(i).getLiverpoolAve().getColors()
                LQA.drawTracks()
                LQA.visible = false
                LQA.saveImgNomogramma()

                //button
                var buttonLQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                buttonLQA.buttonId = pagesLocal.length
                buttonLQA.anchors.bottom = rectdata.bottom
                buttonLQA.anchors.left = rectdata.left
                buttonLQA.anchors.bottomMargin = rectf.height/9
                buttonLQA.anchors.leftMargin = btnLeftMargin
                buttonLQA.height = screenH*grafic.valueOf("Button","height")
                buttonLQA.width = screenW*grafic.valueOf("Button","width")
                buttonLQA.labelSize= layout.value("F4")
                buttonLQA.myText = nameNomo
                buttonLQA.clicked.connect(clickButton)
                btnLeftMargin += buttonLQA.width + 5
                pagesLocal.push(LQA)
                buttons.push(buttonLQA)

                //siroky QMax
                nameNomo = mngData.getFlowDatas(1).getSirokyMax().getTitle()
                if (nameNomo !== "" )
                {
                    var SQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    SQM.clip = true
                    SQM.height = screenH*grafic.valueOf("Nomogrammi","height")
                    SQM.width = screenW*grafic.valueOf("Nomogrammi","width")
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

                    if (SQM.xPoint > SQM.xMin && SQM.xPoint < SQM.xMax && SQM.yPoint > SQM.yMin && SQM.yPoint < SQM.yMax)
                        SQM.isVis = true
                    else
                        SQM.isVis = false

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
                    SQM.saveImgNomogramma()

                    //button
                    var buttonSQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonSQM.buttonId = pagesLocal.length
                    buttonSQM.anchors.bottom = rectdata.bottom
                    buttonSQM.anchors.left = rectdata.left
                    buttonSQM.anchors.bottomMargin = rectf.height/9
                    buttonSQM.anchors.leftMargin = btnLeftMargin
                    buttonSQM.width = screenW*grafic.valueOf("Button","width")
                    buttonSQM.height = screenH*grafic.valueOf("Button","height")
                    buttonSQM.labelSize= layout.value("F4")
                    buttonSQM.myText = nameNomo
                    buttonSQM.clicked.connect(clickButton)
                    btnLeftMargin += buttonSQM.width + 5
                    pagesLocal.push(SQM)
                    buttons.push(buttonSQM)

                    //siroky QAve
                    nameNomo = mngData.getFlowDatas(i).getSirokyAve().getTitle()

                    var SQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    SQA.clip = true
                    SQA.height = screenH*grafic.valueOf("Nomogrammi","height")
                    SQA.width = screenW*grafic.valueOf("Nomogrammi","width")
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

                    if (SQA.xPoint > SQA.xMin && SQA.xPoint < SQA.xMax && SQA.yPoint > SQA.yMin && SQA.yPoint < SQA.yMax)
                        SQA.isVis = true
                    else
                        SQA.isVis = false

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
                    SQA.saveImgNomogramma()

                    //button
                    var buttonSQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonSQA.buttonId = pagesLocal.length
                    buttonSQA.anchors.bottom = rectdata.bottom
                    buttonSQA.anchors.left = rectdata.left
                    buttonSQA.anchors.bottomMargin = rectdata.height/9
                    buttonSQA.anchors.leftMargin = btnLeftMargin
                    buttonSQA.width = screenW*grafic.valueOf("Button","width")
                    buttonSQA.height = screenH*grafic.valueOf("Button","height")
                    buttonSQA.labelSize= layout.value("F4")
                    buttonSQA.myText = nameNomo
                    buttonSQA.clicked.connect(clickButton)
                    btnLeftMargin += buttonSQA.width + 5
                    pagesLocal.push(SQA)
                    buttons.push(buttonSQA)
                }

                //Miskolc QMax
                nameNomo = mngData.getFlowDatas(1).getMiskolcMax().getTitle()
                if (nameNomo !== "" )
                {
                    var MQM =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    MQM.clip = true
                    MQM.height = screenH*grafic.valueOf("Nomogrammi","height")
                    MQM.width = screenW*grafic.valueOf("Nomogrammi","width")
                    MQM.anchors.left = rectdata.left
                    MQM.anchors.top = rectdata.top
                    MQM.anchors.topMargin = 20
                    MQM.anchors.leftMargin = 20
                    MQM.xMin = mngData.getFlowDatas(i).getMiskolcMax().getXmin()
                    MQM.xMax = mngData.getFlowDatas(i).getMiskolcMax().getXmax()
                    MQM.yMin = mngData.getFlowDatas(i).getMiskolcMax().getYmin()
                    MQM.yMax = mngData.getFlowDatas(i).getMiskolcMax().getYmax()
                    MQM.nome = nameNomo
                    MQM.xPoint = mngData.getFlowDatas(i).getMiskolcMax().getDatoX()
                    MQM.yPoint = mngData.getFlowDatas(i).getMiskolcMax().getDatoY()

                    if (MQM.xPoint > MQM.xMin && MQM.xPoint < MQM.xMax && MQM.yPoint > MQM.yMin && MQM.yPoint < MQM.yMax)
                        MQM.isVis = true
                    else
                        MQM.isVis = false

                    MQM.udmX =  mngData.getFlowDatas(i).getMiskolcMax().getUnitx()
                    MQM.udmY =  mngData.getFlowDatas(i).getMiskolcMax().getUnity()
                    MQM.tracksWidth = [1,1,1,1,1]
                    MQM.traksToDraw =  mngData.getFlowDatas(i).getMiskolcMax().getTracce()
                    MQM.tracksColors =  mngData.getFlowDatas(i).getMiskolcMax().getColors()
                    MQM.drawTracks()

                    MQM.visible = false
                    MQM.saveImgNomogramma()

                    //button
                    var buttonMQM = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonMQM.buttonId = pagesLocal.length
                    buttonMQM.anchors.bottom = rectdata.bottom
                    buttonMQM.anchors.left = rectdata.left
                    buttonMQM.anchors.bottomMargin = rectf.height/9
                    buttonMQM.anchors.leftMargin = btnLeftMargin
                    buttonMQM.width = screenW*grafic.valueOf("Button","width")
                    buttonMQM.height = screenH*grafic.valueOf("Button","height")
                    buttonMQM.labelSize= layout.value("F4")
                    buttonMQM.myText = nameNomo
                    buttonMQM.clicked.connect(clickButton)
                    btnLeftMargin += buttonMQM.width + 5
                    pagesLocal.push(MQM)
                    buttons.push(buttonMQM)
                }

                //Miskolc QAve Sergio:non deve essere visibile
                nameNomo = "";//mngData.getFlowDatas(1).getMiskolcAve().getTitle()
                if (nameNomo !== "" )
                {
                    var MQA =  Qt.createQmlObject('import "qrc:/Components"; MNomogramma {}', rectdata)
                    MQA.clip = true
                    MQA.height = screenH*grafic.valueOf("Nomogrammi","height")
                    MQA.width = screenW*grafic.valueOf("Nomogrammi","width")
                    MQA.anchors.left = rectdata.left
                    MQA.anchors.top = rectdata.top
                    MQA.anchors.topMargin = 20
                    MQA.anchors.leftMargin = 20
                    MQA.xMin = mngData.getFlowDatas(i).getMiskolcAve().getXmin()
                    MQA.xMax = mngData.getFlowDatas(i).getMiskolcAve().getXmax()
                    MQA.yMin = mngData.getFlowDatas(i).getMiskolcAve().getYmin()
                    MQA.yMax = mngData.getFlowDatas(i).getMiskolcAve().getYmax()
                    MQA.nome = nameNomo
                    MQA.xPoint = mngData.getFlowDatas(i).getMiskolcAve().getDatoX()
                    MQA.yPoint = mngData.getFlowDatas(i).getMiskolcAve().getDatoY()

                    if (MQA.xPoint > MQA.xMin && MQA.xPoint < MQA.xMax && MQA.yPoint > MQA.yMin && MQA.yPoint < MQA.yMax)
                        MQA.isVis = true
                    else
                        MQA.isVis = false

                    MQA.udmX =  mngData.getFlowDatas(i).getMiskolcAve().getUnitx()
                    MQA.udmY =  mngData.getFlowDatas(i).getMiskolcAve().getUnity()
                    MQA.tracksWidth = [2,2,2,2,2]
                    MQA.traksToDraw =  mngData.getFlowDatas(i).getMiskolcAve().getTracce()
                    MQA.tracksColors =  mngData.getFlowDatas(i).getMiskolcAve().getColors()
                    MQA.drawTracks()

                    MQA.visible = false
                    MQA.saveImgNomogramma()

                    //button
                    var buttonMQA = Qt.createQmlObject('import "qrc:/Components"; MAnaButton {}', rectdata)
                    buttonMQA.buttonId = pagesLocal.length
                    buttonMQA.anchors.bottom = rectdata.bottom
                    buttonMQA.anchors.left = rectdata.left
                    buttonMQA.anchors.bottomMargin = rectf.height/9
                    buttonMQA.anchors.leftMargin = btnLeftMargin
                    buttonMQA.width = screenW*grafic.valueOf("Button","width")
                    buttonMQA.height = screenH*grafic.valueOf("Button","height")
                    buttonMQA.labelSize= layout.value("F4")
                    buttonMQA.myText = nameNomo
                    buttonMQA.clicked.connect(clickButton)
                    btnLeftMargin += buttonMQA.width + 5
                    pagesLocal.push(MQA)
                    buttons.push(buttonMQA)
                }
            }

            pagesSameAna[firstPageSameAna].visible = true
            btnPrint.dataSent = false
            timPrint.start()
        }
    }

    Timer{
        id:timPrint
        interval:500
        onTriggered: {
            mngData.startPrint()
            btnPrint.dataSent = true
        }
    }

    //@@@@@@@@@@    Graphics      @@@@@@@@@@
    MDialogYesNo
    {
        id: dialogMain
        property var owner: dialogMain
        height:screenH*0.3
        width:screenW*0.3

        onOwnerChanged: {
            switch(owner) {
            case dialogMain: break;
            case btnReport:
                message = qsTr("Report already exists. Do you want overwrite it?")
                break
            default:console.error("Owner sconosciuto", owner)
            }
            if(owner != dialogMain)
                dialogMain.open()
        }
        onAccepted: {
            switch(owner) {
            case dialogMain: break
            case btnReport:
                mngData.openReport(comboReport.currentText)
                break
            default:
                console.error("Owner sconosciuto",owner)
            }
        }
        onRejected: {
            switch(owner) {
            case dialogMain: break;
            case btnReport:
                break;
            default: console.error("Owner sconosciuto",owner)
            }
        }
    }


    MButton{
        id: btnPrint
        property bool dataSent: true
        text: qsTr("print")
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")
        height: screenH*grafic.valueOf("Button","height")
        labelSize: layout.value("F4")
        visible: PicoFlow ? true : false
        enabled: PicoFlow ? mngData.getSpoolerQueueLen() === 0 && dataSent : false
        onClicked: {
            console.log("do print")
            mngData.sendToPrint()
        }
    }

    MButton{
        id: btnReport
        text: qsTr("report")
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")
        height: screenH*grafic.valueOf("Button","height")
        labelSize: layout.value("F4")
        visible: PicoFlow ? false : true
        onClicked: {
            if( mngData.checkReport())
                dialogMain.owner = btnReport
            else
                mngData.openReport(comboReport.currentText)
        }
    }

    MLabel{
        id: lblPrinting
        anchors.left: btnReport.right
        anchors.bottom: coverCombo.top
        anchors.bottomMargin: 5
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")/2
        height: screenH*grafic.valueOf("Button","height")/2
        labelSize: layout.value("F4")-2
        visible: PicoFlow ? false : true
        text: qsTr("Template")
    }

    Rectangle {
        id:coverCombo
        anchors.left: btnReport.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")/2
        height: screenH*grafic.valueOf("Button","height")/2
        color: "transparent"
        visible: PicoFlow ? false : true
    }

    MComboBox {
        id: comboReport
        property var listReports:[]
        anchors.left: btnReport.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")/2
        height: screenH*grafic.valueOf("Button","height")/2
        labelSize: layout.value("F4")-2
        visible: PicoFlow ? false : true
        model: listReports
        currentIndex:1
        numElementiMax:5
    }

    MButton {
        id:btnBack
        property real wbt: grafic.valueOf("Button","width")
        text: qsTr("back to graphs")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width:  screenW*(0.01+btnBack.wbt)
        height: screenH*grafic.valueOf("Button","height")
        labelSize: layout.value("F4")
        onClicked:exitFromResult()
    }

    MButton{
        id: btnExit
        text: qsTr("exit")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width: screenW*grafic.valueOf("Button","width")
        height: screenH*grafic.valueOf("Button","height")
        labelSize: layout.value("F4")
        onClicked: {
            exitFromResult()
            mngData.exitFromReview()
        }
    }

    function exitFromResult()
    {
        for (var i = 0; i < pagesLocal.length; i++)
            pagesLocal[i].destroy();
        pagesLocal = []
        for (i = 0; i < buttons.length; i++)
            buttons[i].destroy();
        buttons = []
        rectf.destroy()

        forAna.visible = true

        resultForm.visible = false
    }

}

