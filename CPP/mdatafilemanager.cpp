#include "mdatafilemanager.h"
int dataCount=0;
QTime tim;

MDataFileManager::MDataFileManager(QObject *parent) :
    QObject(parent)
{
    m_mng=NULL;
    m_copy=NULL;
    m_org=NULL;
    m_currentSignalName="custom_signal";
    m_pCurrentSignal=NULL;
    m_updateWhenNews=false;
    m_start=0;
    m_end=3600;//fine esame di default a 1 ora
    m_acqInProgress=false;//nessuna acquisizione in atto
    m_sendingToPlot=false;//nessuno sta spedendo qualcosa per cui ci si può scrivere sopra
    m_serverReady=false;//i server non sono inizializzati quindi falso
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_oldSample=0;
    m_superProcess=NULL;
    m_visualChannel=NULL;
    m_commandChannel=NULL;
}

MDataFileManager::~MDataFileManager()
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

MOrganizer *MDataFileManager::organizer()
{
    return m_org;
}

void MDataFileManager::setInfoList(QVariantList __list)
{
    if(m_org!=NULL)
    {
        m_org->setInfoList(__list);
        if(__list!=m_infoList)
        {
            m_infoList=__list;
            emit infoListChanged();
        }
    }
}

FileType fileType(QString __fileName);

FileType fileType(QString __fileName)
{
    if(__fileName.contains(".wav"))
        return WAV;
    if(__fileName.contains(".pic"))
        return PIC;

    return NOF;
}

QVariantList MDataFileManager::dataNews()
{
    return m_morphPointer;
}

QString MDataFileManager::currentSignal()
{
    return m_currentSignalName;
}

bool MDataFileManager::addSignal(MSignal *__pSignal)
{

    if(__pSignal==NULL)
    {
        qDebug() << "The signal is empty in FunctionHandler::addSignal";
        return false;
    }

    foreach(MSignal *sig,m_signalVector)
        if(sig->getName()==__pSignal->getName())
        {
            qDebug() << "File with the same name already exist in the vector";
            return false;
        }
    m_signalVector.append(__pSignal);
    VarMapVec *vec=new VarMapVec;
    VarMap *traccia=new VarMap;
    QString family=__pSignal->getName();
    QString name="Signal";
    (*traccia)["popUp"]=__pSignal->getName();
    (*traccia)["type"]=TYP_SIGNAL;
    (*traccia)["pointer"]=(qulonglong)__pSignal;
    vec->append(traccia);
    sendNews(family,name,(qulonglong)vec);
    emit availableTracksChanged();
    return true;

}

void MDataFileManager::sendNews(QString __family, QString __name, qulonglong __element)
{

    if(!m_data.keys().contains(__family))
    {
        m_data[__family].append(__name);
        emit availableTracksChanged();
    }
    else
        if(!m_data[__family].contains(__name))
            m_data[__family].append(__name);

    m_morphPointer.clear();
    m_morphPointer.append(__family);
    m_morphPointer.append(__name);
    m_morphPointer.append(__element);

    updateAvailableData();
    if(m_org!=NULL)
        m_org->storeNews(m_morphPointer);
    emit dataNewsChanged();

}

bool MDataFileManager::setCurrentSignal(QString __name)
{
    for(int i=0;i<m_signalVector.size();i++)
    {
        if(m_signalVector[i]->getName()==__name)
        {
            m_pCurrentSignal=m_signalVector[i];
            m_currentSignalName=__name;
            emit currentSignalChanged();
            return true;
        }
    }
    return false;
}

void MDataFileManager::setAcqFile(QString __name)
{
    if(__name!=m_acqFileName)
    {
        m_acqFileName=__name;
        emit acqFileChanged();
    }
}

void MDataFileManager::setConfigurationFile(QString __name)
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



void MDataFileManager::loadFile(QString __fileName)
{
    __fileName.remove("file:///");
    qDebug()<<"Apro il file "<<__fileName;
    m_fileName=__fileName;

    MAudio audio;
    QStringList list;
    int nChannels,n;
    QString name;
    VarMapVec *mrkOpVec=new VarMapVec;
    VarMapVec *mrkAnVec=new VarMapVec;
    VarMapVec *defVec=new VarMapVec;
    QMap<int,QVariantList > defEn;

    float sigMin=INF,sigMax=-INF;
    byte key;
    int32_t numCh;
    int32_t numSamp[20];
    int32_t numDef;
    int32_t tStart[20],tEnd[20];
    QString descr;
    byte chEn[20];

    int i;

    switch(fileType(__fileName))
    {
    case WAV:
    {
        m_pCurrentSignal=NULL;
        nChannels=audio.fromFileToSignal(&m_pCurrentSignal,__fileName);
        list=__fileName.split(QRegularExpression("\\b"));
        name=list[list.size()-4];
        for(int i=0;i<nChannels;i++)
        {
            if(nChannels>1)
            {
                name.append("_ch_");
                QString s;
                s.setNum(i);
                name.append(s);
            }
            m_pCurrentSignal[i].setName(name);
            m_end=m_pCurrentSignal[i].getDuration();
            this->addSignal(m_pCurrentSignal+i);
        }

        break;
    }
    case PIC:
    {
        if(m_mng!=NULL)
        {
            qDebug()<<"m_mng già creato errore!!!";
            return;
        }
        m_mng=new DatafileManager;
        m_mng->SetFileName(__fileName);
        m_mng->SetFileType(5);
        qDebug()<<"File Aperto?"<<m_mng->Open();
        qDebug()<<"File Caricato?"<<m_mng->GetParameters();
        QString patientName=m_mng->GetPatient().section(";",0,1);
        patientName.replace(";","_");
        n=m_mng->GetDuration();
        m_end=n/1000;
        qDebug()<<"Durata esame = "<<m_end;
        qDebug()<<"N° di canali = "<<m_mng->GetChanNum();

        //------ Aggiungo i markers operativi, sono comuni a tutti i canali

        qDebug()<<"Marker Operativi = "<<m_mng->GetNumOperativeMarkers();

        for(i=0;i<m_mng->GetNumOperativeMarkers();i++)
        {
            VarMap *mrk=new VarMap;
            m_mng->GetOpMarker(i,&key,numSamp,&descr);
            float val=(float)numSamp[i]/m_mng->GetNAS(0);
            qDebug()<<val;
            (*mrk)["val"]=val;
            (*mrk)["popUp"]=descr;
            (*mrk)["lock"]=false;
            (*mrk)["key"]=key;
            (*mrk)["color"]=COLOR_OPERATIVE;
            (*mrk)["visible"]=true;
            (*mrk)["type"]=TYP_MARKER;
            mrkOpVec->append(mrk);
        }
        if(!mrkOpVec->isEmpty())
        {
            QString family="Markers";
            QString name="Operative";
            sendNews(family,name,(qulonglong)mrkOpVec);
        }

        //------Aggiungo i canali

        for(int h=0;h<m_mng->GetChanNum();h++)
        {
            MSignal *sig=new MSignal;
            sig->setSize(m_mng->GetSamplesNumber(h));
            for(int i=0;i<m_mng->GetSamplesNumber(h);i++)
            {
                sig->setValue(i,m_mng->GetValue(h,i));
            }
            sig->setName(m_mng->GetChanName(h));
            sig->setSamplingFrequency(m_mng->GetNAS(h));
            float M=sig->maximum();
            float m=sig->minimum();
            if(sigMax<M)
                sigMax=M;
            if(sigMin>m)
                sigMin=m;

            this->addSignal(sig);
        }

        //------Aggiungo i definer
        qDebug()<<"Definer = "<<m_mng->GetNumDefiners();

        for(i=0;i<m_mng->GetNumDefiners();i++)
        {
            VarMap *def=new VarMap;
            m_mng->GetOpMarkerAn(i,&key,tStart,tEnd,chEn,&descr);
            for(int nc=0;nc<m_mng->GetChanNum();nc++)
                defEn[i]<<chEn[nc];

            (*def)["xMin"]=tStart[0]/m_mng->GetNAS(0);
            (*def)["xMax"]=tEnd[0]/m_mng->GetNAS(0);
            (*def)["yMin"]=sigMin;
            (*def)["yMax"]=sigMax;
            (*def)["enCh"]=defEn[i];
            (*def)["popUp"]=descr;
            (*def)["color"]="cyan";
            (*def)["type"]=TYP_DEFINER;
            (*def)["resizeable"]=1;
            defVec->append(def);

        }
        if(!defVec->isEmpty())
        {
            QString family="Definers";
            QString name="Operative";
            sendNews(family,name,(qulonglong)defVec);
        }

        for(int nc=0;nc<m_mng->GetChanNum();nc++)
        {
            VarMapVec *subVec=new VarMapVec;
            for(i=0;i<m_mng->GetNumDefiners();i++)
                if(defEn[i].at(nc).toBool())
                {
                    subVec->append(defVec->at(i));

                }
            if(!subVec->isEmpty())
                sendNews(m_mng->GetChanName(nc),"Definers",(qulonglong)subVec);
        }

        //------ Aggiungo i markers analitici, sono associati ad un definitore

        qDebug()<<"Marker Analitici = "<<m_mng->GetNumAnalyticalMarkers();

        mrkAnVec->clear();
        QVector<int32_t> numChVec;
        for(i=0;i<m_mng->GetNumAnalyticalMarkers();i++)
        {
            VarMap *mrk=new VarMap;
            m_mng->GetAnMarker(i,&key,&numCh,numSamp,&numDef);
            numChVec<<numCh;

            float val=(float)numSamp[i]/m_mng->GetNAS(0);
            qDebug()<<val;
            (*mrk)["val"]=val;
            (*mrk)["popUp"]=descr;
            (*mrk)["lock"]=false;
            (*mrk)["channel"]=numCh;
            (*mrk)["defCode"]=(qulonglong)defVec->value(numDef);
            (*mrk)["key"]=key;
            (*mrk)["color"]=COLOR_ANALYTICAL;
            (*mrk)["visible"]=true;
            (*mrk)["type"]=TYP_MARKER;
            mrkAnVec->append(mrk);
        }
        if(!mrkAnVec->isEmpty())
        {
            QString family="Markers";
            QString name="Analitical";
            sendNews(family,name,(qulonglong)mrkAnVec);
        }

        for(int32_t nc=0;nc<m_mng->GetChanNum();nc++)
        {
            VarMapVec *subVec=new VarMapVec;
            for(i=0;i<numChVec.size();i++)
                if(nc==numChVec[i])
                    subVec->append(mrkAnVec->at(i));

            if(!subVec->isEmpty())
                sendNews(m_mng->GetChanName(nc),"Analytical Markers",(qulonglong)subVec);
        }
        break;
        m_mng->Close();
    }
    default:qDebug()<<"Should not be here!!!!!";break;
    }


}

QVariantList MDataFileManager::getAcqMarkers()
{
    QStringList plotNames;
    plotNames<<"Cella";
    QString type="Marker";
    QString sGroup="$"+type+"Group";
    QString eGroup="&"+type+"Group";
    QString sType="$"+type;
    QString eType="&"+type;

    QVariantList list;

    foreach(QString currentPlot,plotNames) {


        list<<sGroup;
        list<<currentPlot;

        foreach (VarMap curMap, m_acqMarker) {
            list<<sType;
            foreach (QString curRole, curMap.keys())
            {
                list<<curRole;
                list<<curMap.value(curRole);
            }
            list<<eType;
        }
        list<<eGroup;
    }
    //qDebug()<<list;
    return list;
}

void MDataFileManager::resetAll()
{    
    //    m_data.clear();
    //    m_availableData.clear();
    //    for(int i=0;i<m_signalVector.size();i++)
    //        delete m_signalVector[i];
    //    m_signalVector.clear();
    //    emit availableTracksChanged();
    //    emit availableDataChanged();
}

void MDataFileManager::createOrganizer()
{
    m_org=new MOrganizer;
}

void MDataFileManager::saveChanges(QVariant __storage)
{
    //se c'era già un'altra copia la cancelliamo
    if(m_copy!=NULL)
        delete m_copy;

    m_copy=new DatafileManager;
    QString copyName=m_fileName;
    copyName.insert(copyName.length()-4,"_copy");

    qDebug()<<"Faccio la copia?"<<QFile::copy(m_fileName,copyName);
    //apro il file copia

    m_copy->SetFileName(copyName);
    m_copy->SetFileType(5);
    qDebug()<<"Copia aperta?"<<m_copy->Open();
    qDebug()<<"Copia caricata?"<<m_copy->GetParameters();
    qDebug()<<"Eliminati marker e definer?"<<m_copy->DeleteAllMarkers();

    MDataStorage *store=(MDataStorage *)__storage.toULongLong();

    VarMapVec *elements=(VarMapVec *)store->getAll();

    foreach (VarMap *curMap, (*elements)) {
        //qDebug()<<"Salvo l'oggetto: "<<curMap;
        //-Salvataggio Marker
        if(curMap->value("type").toString()==TYP_MARKER)
        {
            //qDebug()<<"Inizio salvataggio marker";
            int32_t *numCamp;
            numCamp=new int32_t[m_copy->GetChanNum()];
            //qDebug()<<curMap->value("val").toFloat();
            for(int i=0;i<m_copy->GetChanNum();i++)
                numCamp[i]=curMap->value("val").toFloat()*m_copy->GetNAS(i);
            if(curMap->value("color").toString()==COLOR_OPERATIVE)
            {
                m_copy->AddOpMarker(numCamp,curMap->value("key").toInt(),curMap->value("popUp").toString());
            }
            if(curMap->value("color").toString()==COLOR_ANALYTICAL)
            {
                m_copy->AddAnMarker(curMap->value("channel").toInt(),
                                    numCamp[curMap->value("channel").toInt()],
                        curMap->value("key").toInt(),
                        curMap->value("defCode").toInt());
            }
            //qDebug()<<"Fine salvataggio marker";
        }
        //-Salvataggio Definer
        if(curMap->value("type").toString()==TYP_DEFINER)
        {
            //qDebug()<<"Inizio salvataggio definer";
            int32_t *start,*end;
            unsigned char *enCh;
            start=new int32_t[m_copy->GetChanNum()];
            end=new int32_t[m_copy->GetChanNum()];
            enCh=new unsigned char[m_copy->GetChanNum()];
            for(int i=0;i<m_copy->GetChanNum();i++)
            {
                start[i]=curMap->value("xMin").toFloat()*m_copy->GetNAS(i);
                end[i]=curMap->value("xMax").toFloat()*m_copy->GetNAS(i);
                enCh[i]=curMap->value("enCh").toList().at(i).toBool();
                //qDebug()<<start[i]<<end[i]<<enCh[i];
            }
            m_copy->AddOpMarkerAn(start,end,enCh,curMap->value("key").toInt(),curMap->value("popUp").toString());
            //qDebug()<<"Fine salvataggio definer";
        }
        //oltre questi due if non ci si dovrebbe arrivare a meno che non sia un segnale e nel caso si prosegue
    }

    qDebug()<<"Commit markers?"<<m_copy->CommitMarkers();
    qDebug()<<"Chiudo il file?"<<m_copy->Close();

}

bool MDataFileManager::newAcquisition(QString __newName,QVariantList __info)
{
    tim.start();
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
            return error("MDataFileManager::newAcquisition","Filename empty");


        initializeServers();
        QVector<VarMap> info;

        VarMap def;
        if(!m_configuration.hasLineage(QStringList()<<XML_ACQUISITION<<XML_TRACKS))
        {
            return error("MDataFileManager::newAcquisition","No info in configuration file");
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
            return error("MDataFileManager::newAcquisition","Error during file creation");
        }


    }
    return true;
}

void MDataFileManager::connectToServers()
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

void MDataFileManager::startSupe()
{
    m_superProcess=new QProcess();

    QString path="M:/Lavoro/Software/Build/StandAlone/";
    m_superProcess->start(path+"FlowBtSupe.exe",QStringList()<<"hde");

}

void MDataFileManager::saveAcquisition()
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

void MDataFileManager::deleteAcquisition()
{

}

void MDataFileManager::addMarker(QVariant __key,QVariant __descr)
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
    }
}

void MDataFileManager::addDefiner(bool __startEnd, QVariantList __info)
{

}

void MDataFileManager::addAlarm(int __code)
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

void MDataFileManager::resetAlarms()
{
    m_alarms.clear();
    updateAlarms();
}

void MDataFileManager::dataOnTCP(QObject *__pParent, SimpleTCPClient *__pTCP, QByteArray __block)
{//arriviamo qua dentro ogni volta che arriva qualcosa da uno dei server a cui siamo collegati

    if(__pParent!=NULL)
    {//punta a qualcosa andiamo avanti
        if(__pTCP!=NULL)
        {//punta a qualcosa proviamo a gestirlo
            ((MDataFileManager *)__pParent)->handleTCP(__pTCP,__block);
        }
    }

}

void MDataFileManager::updateAvailableData()
{
    m_availableData.clear();
    QStringList signalNames=m_data.keys();
    for(int i=0;i<signalNames.size();i++)
    {
        m_availableData<<"$Group";
        //m_availableData<<signalNames[i];
        m_availableData<<m_data[signalNames[i]];
        m_availableData<<"&Group";
    }

    //qDebug()<<"m_availableData = "<<m_availableData;
    emit availableDataChanged();
}

void MDataFileManager::updateAlarms()
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

void MDataFileManager::handleTCP(SimpleTCPClient *__client, QByteArray __block)
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
                ((qint8*)(&status))[i]=__block[i];
            for(uint j = i; j < sizeof(alarms_t)+i; j++)
                ((qint8*)(&alarms))[i]=__block[j-i];

            analyzeStatus(status);
            analyzeAlarms(alarms);
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
                        qDebug()<<v;
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

void MDataFileManager::initializeServers()
{
    //creiamo un nuovo canale con la qmlplotter
    m_visualChannel=new SimpleTCPChannel;
    m_visualChannel->setServerAddress("127.0.0.1");
    m_visualChannel->setServerPort(9000);
    m_visualChannel->listen();
    m_serverReady=true;
}

bool MDataFileManager::loadConfiguration(QString __name)
{
    QString curConfigFile="";

    if(__name=="")
        curConfigFile=m_configurationFileName;
    else
        curConfigFile=__name;

    if(!QFile::exists(curConfigFile))
        return error("MDataFileManager::loadConfiguration()","File "+curConfigFile+" does not exists");
    if(!m_configuration.loadFromXML(curConfigFile))
        return error("MDataFileManager::loadConfiguration()","XML file corrupted");

    qDebug()<<curConfigFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
    //carichiamo le connessioni
    if(m_configuration.getChild(XML_CONNECTIONS)!=NULL)
        return loadConnectivityInfo(m_configuration.getChild(XML_CONNECTIONS));
    else
        return error("MDataFileManager::loadConfiguration()","XML file corrupted");
}

void MDataFileManager::saveConfiguration()
{
    m_configuration.saveToXML(m_configurationFileName);
}

bool MDataFileManager::loadConnectivityInfo(Ancestry *__info)
{
    if(__info==NULL)
        return error("MDataFileManager::loadConnectivityInfo","NULL pointer");
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
        return error("MDataFileManager::loadConnectivityInfo()","XML file corrupted");

    emit connectivityInfoChanged();
    return true;
}

void MDataFileManager::analyzeStatus(flowBT_status_t __status)
{
    qDebug()<<__status.currState;
    switch(__status.currState)
    {
        case ESTATE_IDLE_NOT_CONNECTED:
        addAlarm(0);
        break;
        default:break;
    }
}

void MDataFileManager::analyzeAlarms(alarms_t __alarms)
{

}

bool MDataFileManager::sendCommand(tcp_flow_bt_cmd_t __command)
{

    if(m_tcp.contains("CMD"))
    {
        qDebug()<<"Sending command: "<<__command;
        quint8 c=(quint8)__command;
        m_tcp["CMD"]->sendData((char *)&c,sizeof(quint8));
        return true;
    }
    else
        return error("MDataFileManager::sendCommand","No CMD channel loaded");
}


