import QtQuick 2.0
import QtQuick.Controls 1.0
import Managers 1.0
import MComponents 1.0


MForm{
    //@@@@@@@@@@ Definitions @@@@@@@@@@
    property string folder

    //@@@@@@@@@@    Properties      @@@@@@@@@@
    id : rootHom
    visible:whoIsVisilbe===name?true:false

    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnRea
        text:"Real Time Section"
        anchors.left:parent.left
        anchors.top:parent.top
        width:rootHom.width/2
        height: rootHom.height/2
        background:Images.realTime

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:{
            btnRea.enabled=false
            btnAna.enabled=false
            btnSup.enabled=false
            btnOpt.enabled=false
            configSelector.visible=true
        }
    }
    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnAna
        text:"Analysis Section"
        anchors.right:parent.right
        anchors.top:parent.top
        width:rootHom.width/2
        height: rootHom.height/2
        background:Images.analysis
        //onClicked:

    }
    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnSup
        text:"Supervisor Section"
        anchors.left:parent.left
        anchors.bottom:parent.bottom
        width:rootHom.width/2
        height: rootHom.height/2
        background:Images.supervisor

    }
    MButton{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:btnOpt
        text:"Options"
        anchors.right:parent.right
        anchors.bottom:parent.bottom
        width:rootHom.width/2
        height: rootHom.height/2
        background:Images.options

    }

    MListSelector{
        //@@@@@@@@@@    Properties      @@@@@@@@@@
        id:configSelector
        height: 300
        width:300
        anchors.centerIn: parent
        visible:false
        enableNew: false
        title:"Choose your configuration"
        elements:["Cella Bluetooth","Doppler"]

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onSelected:
        {
            var configFile
            if(curText=="Cella Bluetooth")
                configFile="../Doppler/Config/cella.xml"
            if(curText=="Doppler")
                configFile="../Doppler/Config/realtime.xml"

            mngData.startSupe()
            forReal.configurationFile=configFile
            mngData.configurationFile=configFile

            whoIsVisilbe="RealTimeForm"

            configSelector.visible=false
            btnRea.enabled=true
            btnAna.enabled=true
            btnSup.enabled=true
            btnOpt.enabled=true
        }
    }
}
