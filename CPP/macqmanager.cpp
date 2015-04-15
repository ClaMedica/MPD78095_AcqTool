#include "macqmanager.h"
int dataCount=0;



MAcqManager::MAcqManager(QObject *parent) :
    QObject(parent)
{
    m_mng=NULL;
    m_acqFileOpened=false;//nessuna acquisizione in atto
    m_sendingToPlot=false;//nessuno sta spedendo qualcosa per cui ci si può scrivere sopra
    m_serverReady=false;//i server non sono inizializzati quindi falso
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_oldSample=0;
    m_superProcess=NULL;


    m_oldState=ESTATE_IDLE_NOT_CONNECTED;

    //connetto il gestore degli allarmi alla proprietà alarms
    connect(&m_alarmMng,SIGNAL(alarmsUpdated(QVariantList)),this,SLOT(setAlarms(QVariantList)));

}

MAcqManager::~MAcqManager()
{
    if(m_mng!=NULL)
    {
        m_mng->Close();
        delete m_mng;
    }

    if(m_superProcess!=NULL)
    {
        m_superProcess->close();
        if(m_superProcess->waitForFinished())
            delete m_superProcess;
    }

    if(m_tcpClients.values().size()>0)
    {
        foreach (SimpleTCPClient * cur, m_tcpClients.values()) {
            delete cur;
        }
    }

    if(m_tcpChannels.values().size()>0)
    {
        foreach (SimpleTCPChannel * cur, m_tcpChannels.values()) {
            delete cur;
        }
    }
    //    for(int i=0;i<m_signalVector.size();i++)
    //        if(m_signalVector[i]!=NULL)
    //            delete m_signalVector[i];
}

void MAcqManager::setAcqFile(QString __name)
{
    if(__name!=m_acqFileName)
    {
        m_acqFileName=__name;
        emit acqFileChanged();
    }
}

void MAcqManager::setConfigurationFile(QString __name)
{
    if(__name!=m_configurationFileName)
    {
        m_configurationFileLoaded=loadConfiguration(__name);//carico la nuova configurazione
        if(m_configurationFileLoaded)
        {
            m_configurationFileName=__name;
            emit configurationFileChanged();
        }
    }
}

void MAcqManager::setAlarmFile(QString __name)
{
    if(__name!=m_alarmFileName)
    {
        m_alarmFileLoaded=loadAlarms(__name);//carico la nuova configurazione
        if(m_alarmFileLoaded)
        {
            m_alarmFileName=__name;
            emit alarmFileChanged();
        }
    }
}

bool MAcqManager::newAcquisition(QString __newName)
{

    if(m_acqFileOpened)
    {
        //sono già in acquisizione e voglio farne partire un altra...strano ma
    }
    else
    {
        /* non sono in acquisizione e quindi posso lanciarne una nuova aprendo il file e leggendo le info
         * oppure pescandole dal file di configurazione
        */
        if(m_mng!=NULL)
        {//se c'è qualcosa di vecchio lo chiudo
            delete m_mng;
        }

        if(__newName!="")
            m_acqFileName=__newName;
        else
            return error("MAcqManager::newAcquisition","Error file empty");

        //ho letto la configurazione

        qDebug()<<m_acqFileName<<" exists? "<<QFile::exists(m_acqFileName);
        if(QFile::exists(m_acqFileName))
        {
            m_mng=new DatafileManager;

            m_mng->SetFileName(m_acqFileName);
            m_mng->SetFileType(5);

            qDebug()<<"Aperto il file?"<<m_mng->Open();
            m_acqFileOpened=true;//mi segno che ho aperto il file
        }
        else
        {
            return error("MAcqManager::newAcquisition","File does not exists");
        }
        buildConfigurationFile();
        //inizializzo i server di comunicazione con i plotter
        initializeServers();
        //disabilito alcuni allarmi
        m_alarmMng.manageAlarm(S_ALA_NOT_ACQUIRING,DISABLE);
        //mi connetto ai server del supe e del programma di gestione archivi
        QTimer::singleShot(500,this,SLOT(connectToServers()));


    }
    //faccio partire il timer per l'allarme di stato
    m_alarmMng.startTimeoutAlarms();

    return true;
}

void MAcqManager::connectToServers()
{
    //mi connetto ai server
    foreach (SimpleTCPClient *client, m_tcpClients) {
        client->registerDataReadyCallBack(&(this->dataOnTCP));
        client->connectToHost();
        if(client->waitForConnected(10000))
        {
            qDebug()<<"connesso "<<client->hostAddress().toString()<<client->hostPort();
        }
        else
        {
            QStringList superPorts;
            superPorts<<"CMD"<<"VAL"<<"STA";
            if(superPorts.contains(m_tcpClients.key(client)))
            {
                if(m_superProcess!=NULL)
                {
                    m_superProcess->close();
                    if(m_superProcess->waitForFinished())
                        delete m_superProcess;
                }
                startSupe("hide");
                QTimer::singleShot(500,this,SLOT(connectToServers()));
            }
        }

    }
}

void MAcqManager::startSupe(QString __mode)
{
    m_superProcess=new QProcess();

    QString path="M:/Lavoro/Software/Build/StandAlone/";
    m_superProcess->start(path+"FlowBtSupe.exe",QStringList()<<__mode);

}

void MAcqManager::saveAcquisition()
{
    //disabilito gli allarmi
    m_alarmMng.disableAll();
    //interrompo la connessione

    foreach (SimpleTCPClient *client, m_tcpClients) {
        if(client->disconnectToHost())
            qDebug()<<"disconnesso "
                   <<client->hostAddress().toString()
                  <<client->hostPort();
    }
    if(m_acqFileOpened)
    {//se siamo in acq facciamo un commit
        m_mng->CommitValues();
    }
    m_mng->Close();
    delete m_mng;
    m_mng=NULL;

}

void MAcqManager::deleteAcquisition()
{

}

void MAcqManager::addMarker(QVariant __key,QVariant __descr)
{
    if(m_acqFileOpened)
    {
        m_mng->AppendOpMarker((uchar)__key.toUInt(),__descr.toString());

        VarMap mrk;
        mrk["key"]=__key;
        mrk["color"]="red";
        mrk["popUp"]=__descr;
        mrk["lock"]=true;
        mrk["val"]=(float)m_mng->GetSamplesNumber(0)/m_mng->GetNAS(0);
        m_acqMarker.append(mrk);
        updateAcqData();
    }
}

void MAcqManager::addDefiner(bool __startEnd, QVariantList __info)
{
    //BUG
}

bool MAcqManager::sendStartAcq()
{
    return sendCommand(1);
}

bool MAcqManager::sendStopAcq()
{
    m_alarmMng.manageAlarm(S_ALA_NOT_ACQUIRING,DISABLE);
    return sendCommand(3);
}



void MAcqManager::resetAlarms()
{
    m_alarmMng.resetAlarms();

}

void MAcqManager::setAlarms(QVariantList __list)
{
    if(m_alarmList!=__list)
    {
        m_alarmList=__list;
        emit alarmsChanged();
    }

}

void MAcqManager::dataOnTCP(QObject *__pParent, SimpleTCPClient *__pTCP, QByteArray __block)
{//arriviamo qua dentro ogni volta che arriva qualcosa da uno dei server a cui siamo collegati

    if(__pParent!=NULL)
    {//punta a qualcosa andiamo avanti
        if(__pTCP!=NULL)
        {//punta a qualcosa proviamo a gestirlo
            ((MAcqManager *)__pParent)->handleTCP(__pTCP,__block);
        }
    }

}



void MAcqManager::updateAcqData()
{
    QStringList plotNames;
    plotNames<<"Cella";//BUG
    QString type="Marker";
    QString sGroup="$"+type+"Group";
    QString eGroup="&"+type+"Group";
    QString sType="$"+type;
    QString eType="&"+type;

    foreach(QString currentPlot,plotNames) {

        m_acqMarkerList<<sGroup;
        m_acqMarkerList<<currentPlot;

        foreach (VarMap curMap, m_acqMarker) {
            m_acqMarkerList<<sType;
            foreach (QString curRole, curMap.keys())
            {
                m_acqMarkerList<<curRole;
                m_acqMarkerList<<curMap.value(curRole);
            }
            m_acqMarkerList<<eType;
        }
        m_acqMarkerList<<eGroup;
    }
    //qDebug()<<m_acqMarkerList;
    emit acqMarkersChanged();
}

void MAcqManager::handleTCP(SimpleTCPClient *__client, QByteArray __block)
{
    if(m_tcpClients.values().contains(__client))
    {
        QString who=m_tcpClients.key(__client);
        //qDebug()<<who<<__block;
        if(who=="STA")
        {//allora è uno stato
            __block.remove(0,4);
            flowBT_status_t status;
            alarms_t alarms;
            uint i=0;
            for(i = 0; i < sizeof(flowBT_status_t); i++)
                ((qint8*)(&status))[i]=__block[i];
            i++;
            for(uint j = i; j < sizeof(alarms_t)+i; j++)
                ((qint8*)(&alarms))[j-i]=__block[j];

            analyzeStatus(status);
            analyzeAlarms(alarms);
            m_alarmMng.startTimeoutAlarms(T_ALA_TIMEOUT_STATUS,TIMEOUT_TIME_ON_STATUS);
        }
        /*
        if(who=="CMD")
        {//allora è un comando
            uchar id=__block[0];//catturiamone l'id

            switch(id)
            {
            case CMD_NEW_ACQ:
            {//dobbiamo iniziare ad acquisire per cui creiamo un nuovo file
                QVariantList info;
                //spacchetto il pacchetto
                // ---- da completare
                //e lo spedisco
                newAcquisition("",info);
                break;
            }
            case CMD_END_ACQ:
            {
                m_mng->CommitValues();
                m_mng->Close();
                m_acqFileOpened=false;
                break;
            }
            case CMD_ADD_MRK:
            {
                m_mng->AppendOpMarker(KEY_DEG,"sono un marker");
                break;
            }

            default:qDebug()<<"handleTCP"<<"invalid id"<<id;break;
            }

        }
        */
        if(who=="VAL")
        {
            /**
             * TCP PACKET:
             * - numChan, type qint32. Number on channel present into the packet. It has to be between 0 and m_maxNumChan
             * The following data are repeated numChan times
             * - currChan, type qint32. Current channel has to be a value between 0 and m_maxNumChan
             * - numChanData, type qint32. Number of samples (float) for the current channel
             * - samples, type qreal, lenght numChanData. Channel's samples
             */

            dataCount++;
            //qDebug()<<"dal server"<<dataCount<<(float)tim.elapsed()/1000;
            QByteArray block,blockOut;
            QDataStream in(&block, QIODevice::ReadOnly),out(&blockOut, QIODevice::WriteOnly);
            block=__block;

            //e poi lo spacchettiamo
            //qDebug()<<"new pack"<<dataCount;
            qint32 numChan = 0;
            qint32 numBytes = 0;
            qint32 currChan = 0;
            qint32 numChanData = 0;
            uchar maxNumChan=m_mng->GetChanNum();
            qreal sample;

            in >> numBytes;
            out << numBytes;
            //qDebug()<<"N° bytes: "<<numBytes;

            //Chan number
            in >> numChan;
            out << numChan;
            //qDebug()<<"N° chan: "<<numChan;

            if(numChan > maxNumChan)
            {
                qDebug( "handleTCP: ERROR, numChan(%d) > maxNumChan(%d)", numChan,maxNumChan);
                return;
            }

            for(int k = 0; k < numChan; k++)
            {
                in >> currChan;
                out << currChan;
                in >> numChanData;
                out << numChanData;
                //qDebug()<<currChan;
                if(currChan < maxNumChan && currChan >= 0)
                {//se è un canale con del senso
                    for(int i=0;i < numChanData;i++)
                    {
                        in >> sample;

                        float res=m_acqData.resolution[k];
                        float v;
                        float diff=fabs(m_oldSample-sample);

                        if(diff>(res*0.75))
                        {
                            v=round(sample/res)*res;
                            m_oldSample=v;
                        }
                        else
                            v=m_oldSample;


                        out << (qreal)v;
                        //qDebug()<<v;
                        m_mng->AppendValue(&currChan,&v,1);
                    }
                }
                else
                {
                    qDebug( "handleTCP: WARNING, packet corrupt --> currChan (%d) out of range", currChan);
                }
            }
            if(!m_sendingToPlot && m_serverReady)
            {
                m_sendingPack = blockOut;
                //qDebug()<<"al plot"<<dataCount<<(float)tim.elapsed()/1000;
                //mandiamo i dati alla visualizzazione
                m_sendingToPlot=true;
                m_tcpChannels["PLT"]->sendData(&m_sendingPack);
                m_sendingToPlot=false;
            }
            else
            {
                qDebug()<<"Skip";
            }

        }
    }
}

void MAcqManager::initializeServers()
{
    //creiamo un nuovo canale con la qmlplotter
    foreach(SimpleTCPChannel *chan,m_tcpChannels.values())
        chan->listen();
    m_serverReady=true;
}

bool MAcqManager::loadConfiguration(QString __name)
{
    QString curConfigFile="";

    if(__name=="")
        curConfigFile=m_configurationFileName;
    else
        curConfigFile=__name;

    if(!QFile::exists(curConfigFile))
        return error("MAcqManager::loadConfiguration()","File "+curConfigFile+" does not exists");
    if(!m_configuration.loadFromXML(curConfigFile))
        return error("MAcqManager::loadConfiguration()","XML file corrupted");

    qDebug()<<curConfigFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
    //carichiamo le connessioni
    if(m_configuration.getChild(XML_CONNECTIONS)!=NULL)
        return loadConnectivityInfo(m_configuration.getChild(XML_CONNECTIONS));
    else
        return error("MAcqManager::loadConfiguration()","XML file corrupted");
}

bool MAcqManager::loadAlarms(QString __name)
{
    QString curAlaFile="";

    if(__name=="")
        curAlaFile=m_alarmFileName;
    else
        curAlaFile=__name;

    if(!QFile::exists(curAlaFile))
        return error("MAcqManager::loadAlarms()","File "+curAlaFile+" does not exists");
    if(!m_alarmMng.load(curAlaFile))
        return error("MAcqManager::loadAlarms()","XML file corrupted");

    qDebug()<<curAlaFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
    //carichiamo le connessioni
}

void MAcqManager::saveConfiguration()
{
    m_configuration.saveToXML(m_configurationFileName);
}

bool MAcqManager::loadConnectivityInfo(Ancestry *__info)
{
    if(__info==NULL)
        return error("MAcqManager::loadConnectivityInfo","NULL pointer");
    if(__info->getChild(XML_TCP)!=NULL)
    {
        Ancestry *tcp=__info->getChild(XML_TCP);

        foreach (Ancestry *child, tcp->getChildren()) {
            QString name=child->name();
            QString address=child->getAttribute(XML_ADDRESS);
            int port=child->getAttribute(XML_PORT).toInt();
            if(child->getAttribute(XML_TYPE)=="client")
                m_tcpClients[name]=new SimpleTCPClient(QHostAddress(address),port,this);
            else if(child->getAttribute(XML_TYPE)=="server")
                m_tcpChannels[name]=new SimpleTCPChannel(QHostAddress(address),port,this);
        }
    }
    else
        return error("MAcqManager::loadConnectivityInfo()","XML file corrupted");

    return true;
}

void MAcqManager::analyzeStatus(flowBT_status_t __status)
{
    if(__status.currState!=m_oldState)
    {
        qDebug()<<"Stato "<<__status.currState;
    }


    switch(__status.currState)
    {
    case ESTATE_IDLE_NOT_CONNECTED:
        m_alarmMng.addAlarm(S_ALA_NOT_CONNECTED);
        break;
    case ESTATE_IDLE_CONNECTED:
        if(m_oldState==ESTATE_IDLE_NOT_CONNECTED)
            sendStartAcq();
        m_alarmMng.addAlarm(S_ALA_NOT_ACQUIRING);
        break;
    case ESTATE_ACQUIRING:
        if(m_oldState==ESTATE_IDLE_CONNECTED)
            m_alarmMng.manageAlarm(S_ALA_NOT_ACQUIRING,ENABLE);
        break;
    default:break;
    }
    m_oldState=__status.currState;

}

void MAcqManager::analyzeAlarms(alarms_t __alarms)
{

}

void MAcqManager::buildConfigurationFile()
{
    Ancestry config;
    if(!config.loadFromXML(QDir::currentPath()+"/generalConfig.xml"))
        return "";
    //creo il file di configurazione in base all'esame
    config.addChild("Graphs");
    Ancestry *graph=config.getChild("Graphs");
    QList<int> graphNumbers;
    int chanlNum=m_mng->GetChanNum();
    for(int nc=0;nc<chanlNum;nc++)
    {//contiamo i grafici
        if(!graphNumbers.contains(m_mng->GetGraph(nc)))
            graphNumbers<<m_mng->GetGraph(nc);
    }

    foreach (int num, graphNumbers) {

        graph->addChild("Graph_"+QString::number(num));
        graph->getChild("Graph_"+QString::number(num))->addChild("Tracks"); ;
        Ancestry *cur=graph->getChild("Graph_"+QString::number(num))->getChild("Tracks");
        for(int nc=0;nc<chanlNum;nc++)
        {

            //vediamo chi ci va disegnato dentro
            if(graphNumbers.contains(m_mng->GetGraph(nc)))
            {//ci vado disegnato dentro
                QString chanName=m_mng->GetChanName(nc);
                cur->addChild(chanName);
                //dm.SetChanName(nc,Channel->name());
                //dm.SetLoc(nc,Channel->loc());
                //dm.SetGraph(nc,Channel->graph().toInt());
                cur->setAttribute("yAUOM",QString::number(m_mng->GetUdM(nc)),QStringList()<<chanName<<"Axis");
                //dm.SetUdM(nc,50);
                //dm.SetZero(nc,Channel->zero().toInt());
                //dm.SetRange(nc,Channel->range().toFloat());
                cur->setAttribute("yAbsoluteMax",QString::number(m_mng->GetSupLim(nc)),QStringList()<<chanName<<"Axis");
                //dm.SetSupLim(nc,supLim);
                cur->setAttribute("yAbsoluteMin",QString::number(m_mng->GetInfLim(nc)),QStringList()<<chanName<<"Axis");
                //dm.SetInfLim(nc,infLim);
                //dm.SetDist(nc,Channel->dist().toInt());
                //dm.SetAngle(nc,Channel->angle().toInt());
                cur->setAttribute("samplingFrq",QString::number(m_mng->GetNAS(nc)),QStringList()<<chanName<<"Time");
                //dm.SetNAS(nc,10);
                //dm.SetChanOther(nc,"");
                cur->setAttribute("serverPort",QString::number(9000+nc),QStringList()<<chanName<<"Network");
            }
        }
    }
    config.saveToXML(QDir::homePath()+DATAFILE_PATH+"/pv"+testnum+"1A.xml");
}





bool MAcqManager::sendCommand(tcp_flow_bt_cmd_t __command)
{

    if(m_tcpClients.contains("CMD"))
    {
        qDebug()<<"Sending command: "<<__command;
        quint8 c=(quint8)__command;
        m_tcpClients["CMD"]->sendData((char *)&c,sizeof(quint8));
        return true;
    }
    else
        return error("MAcqManager::sendCommand","No CMD channel loaded");
}




