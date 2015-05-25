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
        enabled:configSelector.visible?false:true

        //@@@@@@@@@@    Events          @@@@@@@@@@
        onClicked:{
            configSelector.visible=true
            configSelector.owner="RealTimeForm"
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
        enabled:configSelector.visible?false:true
        onClicked:{            
            configSelector.visible=true
            configSelector.owner="AnalysisForm"
        }

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
        onClicked: mngAcq.startSupe("show")
        enabled:configSelector.visible?false:true
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
        enabled:configSelector.visible?false:true
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
                configFile=configFolder+"Config_BT.xml"


            switch(owner)
            {
            case "RealTimeForm":
                forReal.configurationFile=configFile
                mngAcq.configurationFile=configFile
                mngAcq.alarmFile=configFolder+"alarms.xml"
                mngAcq.startSupe("hide")
                break;
            case "AnalysisForm":
                console.log(root.applicationDirPath);
                forAna.loadConfiguration(configFile)
                mngData.configurationFile=configFile
                break;
            }



            whoIsVisilbe=owner

            configSelector.visible=false

        }
    }
}
