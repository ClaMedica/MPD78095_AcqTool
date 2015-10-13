import QtQuick 2.0
import QtQuick.Controls 1.0
import Managers 1.0
 
import "qrc:/Components"


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
        background:"qrc:/Images/Real"
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
        background:"qrc:/Images/Analysis"
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
        background:"qrc:/Images/Supervisor"
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
        background:"qrc:/Images/Options"
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
                configFile=configFolder+"/cur.xml"


            whoIsVisilbe=owner

            configSelector.visible=false

        }
    }

}
