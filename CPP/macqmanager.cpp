#include "macqmanager.h"
int dataCount=0;

AlarmTimer::AlarmTimer(TimeoutAlarmRecord __code){
    m_code=(int)__code;
    m_tim=new QTimer;
    m_tim->setSingleShot(true);
    connect(m_tim,SIGNAL(timeout()),this,SLOT(send()));
}

AlarmTimer::~AlarmTimer(){delete m_tim;}

void AlarmTimer::reset(){start(TIMEOUT_AFTER_RESET);}

void AlarmTimer::start(int __msec){m_tim->start(__msec);}

void AlarmTimer::send(){qDebug()<<"Timeout Alarm code = "<<m_code;emit timeout(m_code);}

MAcqManager::MAcqManager(QObject *parent) :
    QObject(parent)
{
    m_mng=NULL;
    m_acqInProgress=false;//nessuna acquisizione in atto
    m_sendingToPlot=false;//nessuno sta spedendo qualcosa per cui ci si può scrivere sopra
    m_serverReady=false;//i server non sono inizializzati quindi falso
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_oldSample=0;
    m_superProcess=NULL;
    m_visualChannel=NULL;
    m_commandChannel=NULL;

    //definisco gli allarmi a tempo

    for(int i=0;i<T_ALA_NUM;i++)
    {
        TimeoutAlarmRecord curAla=(TimeoutAlarmRecord)i;
        m_ATMap[curAla]=new AlarmTimer(curAla);
        connect(m_ATMap[i],SIGNAL(timeout(int)),this,SLOT(addAlarm(int)));
    }

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

    if(m_visualChannel!=NULL)
    {
        delete m_visualChannel;
    }

    if(m_commandChannel!=NULL)
    {
        delete m_commandChannel;
    }

    if(m_tcp.values().size()>0)
    {
        foreach (SimpleTCPClient * cur, m_tcp.values()) {
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

bool MAcqManager::newAcquisition(QString __newName,QVariantList __info)
{

    if(m_acqInProgress)
    {
        //sono già in acquisizione e voglio farne partire un altra...strano ma
    }
    else
    {
        /* non sono in acquisizione e quindi posso lanciarne una nuova
        * con le info che ho:
        * _ il primo elemento è il tipo di file che voglio creare di default è 5
        * _ dal secondo alla fine sono tutte liste strutturate come solito ovvero da
        *  coppie formate da nome proprietà e valore
        */
        if(m_mng!=NULL)
        {//se c'è qualcosa di vecchio lo chiudo
            delete m_mng;
        }

        if(__newName!="")
            m_acqFileName=__newName;
        else
            return error("MAcqManager::newAcquisition","Filename empty");


        initializeServers();
        QVector<VarMap> info;

        VarMap def;
        if(!m_configuration.hasLineage(QStringList()<<XML_ACQUISITION<<XML_TRACKS))
        {
            return error("MAcqManager::newAcquisition","No info in configuration file");
        }
        Ancestry *tracks=m_configuration.getChild(XML_TRACKS,QStringList()<<XML_ACQUISITION);
        m_acqData.create(tracks->getChildren().size());
        int index=0;
        foreach (Ancestry *track, tracks->getChildren()) {
            QMap<QString,QString> attributes=track->getAttributes();
            def.clear();
            def[XML_NAME]=track->name();
            foreach(QString field,attributes.keys())
            {
                def[field]=attributes[field];
            }
            m_acqData.frequency[index]=def[XML_FREQUENCY].toInt();
            m_acqData.resolution[index]=def[XML_RESOLUTION].toFloat();
            m_acqData.gain[index]=def[XML_GAIN].toFloat();
            m_acqData.offset[index]=def[XML_OFFSET].toFloat();
            info<<def;
            index++;
        }
        //ho letto la configurazione

        int32_t fileType=5;

        if(__info.size()>0)
        {
            //ho delle info diverse per cui sovrascrivo tutto
            info.clear();
            fileType=__info.takeFirst().toInt();

            foreach(QVariant chanInfo,__info)
            {
                QVariantList list=chanInfo.toList();
                VarMap infoMap;
                for(int i=0;i<list.size();i+=2)
                    infoMap[list[i].toString()]=__info[i+1];
                info<<infoMap;
            }
        }


        qDebug()<<m_acqFileName;
        QFile f;
        f.setFileName(m_acqFileName);
        f.open(QIODevice::WriteOnly);
        f.close();

        qDebug()<<QFile::exists(m_acqFileName);
        if(QFile::exists(m_acqFileName))
        {
            m_mng=new DatafileManager;

            m_mng->SetFileName(m_acqFileName);
            m_mng->SetFileType(fileType);
            m_mng->SetChanNum((int32_t)info.size());

            qDebug()<<"Creato il file?"<<m_mng->Create()<<" con "<<info.size()<<" canali";

            //m_mng->Open();
            for(int i=0;i<info.size();i++)
            {//settiamo le info per ogni canale
                VarMap ch=info[i];
                m_mng->SetChanName(i,ch[XML_NAME].toString());
                m_mng->SetNAS(i,ch[XML_FREQUENCY].toInt());
                m_mng->SetGain(i,ch[XML_GAIN].toFloat());
                m_mng->SetOffset(i,ch[XML_OFFSET].toFloat());
            }


            m_mng->CommitParameters();//salvo i parametri
            m_acqInProgress=true;//mi segno che sono in acquisizione

            //mi connetto ai server
            QTimer::singleShot(500,this,SLOT(connectToServers()));

            //partiamo
            QTimer::singleShot(5000,this,SLOT(sendStartAcq()));
            //sendCommand(ETCP_CMD_START);//spedisco il comando su tcp al modulo di acq
        }
        else
        {
            return error("MAcqManager::newAcquisition","Error during file creation");
        }


    }
    //faccio partire il timer per l'allarme di stato
    foreach (AlarmTimer *tim, m_ATMap.values()) {
        tim->start(TIMEOUT_TIME_ON_STATUS);
    }

    return true;
}

void MAcqManager::connectToServers()
{
    //mi connetto ai server
    foreach (SimpleTCPClient *client, m_tcp) {
        client->registerDataReadyCallBack(&(this->dataOnTCP));
        if(client->connectToHost())
            qDebug()<<"connesso "
                   <<client->hostAddress().toString()
                  <<client->hostPort();
    }
}

void MAcqManager::startSupe()
{
    m_superProcess=new QProcess();

    QString path="M:/Lavoro/Software/Build/StandAlone/";
    m_superProcess->start(path+"FlowBtSupe.exe",QStringList()<<"hde");

}

void MAcqManager::saveAcquisition()
{
    //interrompo la connessione

    foreach (SimpleTCPClient *client, m_tcp) {
        if(client->disconnectToHost())
            qDebug()<<"disconnesso "
                   <<client->hostAddress().toString()
                  <<client->hostPort();
    }
    if(m_acqInProgress)
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
    if(m_acqInProgress)
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

void MAcqManager::addAlarm(int __code)
{
    VarMap ala;

    foreach (VarMap raisedAlarms, m_alarms) {
        if(raisedAlarms["code"]==__code)
            return;
    }
    ala["code"]=__code;
    ala["message"]="Help me!! code "+QString::number(__code);
    ala["help"]="I don't care";
    ala["color"]="red";

    m_alarms.append(ala);
    updateAlarms();
}

void MAcqManager::resetAlarms()
{
    m_alarms.clear();
    foreach (AlarmTimer *tim, m_ATMap.values()) {
        tim->reset();
    }
    updateAlarms();
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

void MAcqManager::updateAlarms()
{
    m_alarmList.clear();

    foreach (VarMap alarm, m_alarms) {
        m_alarmList<<"$Alarm";
        foreach(QString key,alarm.keys())
        {
            m_alarmList<<key;
            m_alarmList<<alarm[key];
        }
        m_alarmList<<"&Alarm";
    }

    //qDebug()<<"m_alarmList = "<<m_alarmList;
    emit alarmsChanged();
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
    if(m_tcp.values().contains(__client))
    {
        QString who=m_tcp.key(__client);
        //qDebug()<<who<<__block;
        if(who=="STA")
        {//allora è uno stato

            flowBT_status_t status;
            alarms_t alarms;
            uint i=0;
            for(i = 0; i < sizeof(flowBT_status_t); i++)
                ((qint8*)(&status))[sizeof(flowBT_status_t)-1-i]=__block[i];
            for(uint j = i+1; j < sizeof(alarms_t)+i+1; j++)
                ((qint8*)(&alarms))[sizeof(alarms_t)-j+i]=__block[j];

            analyzeStatus(status);
            analyzeAlarms(alarms);
            m_ATMap[T_ALA_TIMEOUT_STATUS]->start(TIMEOUT_TIME_ON_STATUS);
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
                m_acqInProgress=false;
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
                m_visualChannel->sendData(&m_sendingPack);
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
    m_visualChannel=new SimpleTCPChannel;
    m_visualChannel->setServerAddress("127.0.0.1");
    m_visualChannel->setServerPort(9000);
    m_visualChannel->listen();
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

            m_tcp[name]=new SimpleTCPClient(QHostAddress(address),port,this);

        }
    }
    else
        return error("MAcqManager::loadConnectivityInfo()","XML file corrupted");

    return true;
}

void MAcqManager::analyzeStatus(flowBT_status_t __status)
{
    qDebug()<<"Stato "<<__status.currState;
    switch(__status.currState)
    {
        case ESTATE_IDLE_NOT_CONNECTED:
        addAlarm(0);
        break;
        default:break;
    }
}

void MAcqManager::analyzeAlarms(alarms_t __alarms)
{

}

bool MAcqManager::sendCommand(tcp_flow_bt_cmd_t __command)
{

    if(m_tcp.contains("CMD"))
    {
        qDebug()<<"Sending command: "<<__command;
        quint8 c=(quint8)__command;
        m_tcp["CMD"]->sendData((char *)&c,sizeof(quint8));
        return true;
    }
    else
        return error("MAcqManager::sendCommand","No CMD channel loaded");
}


