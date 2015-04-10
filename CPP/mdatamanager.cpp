#include "mdatamanager.h"



MDataManager::MDataManager(QObject *parent) :
    QObject(parent)
{
    m_mng=NULL;
    m_copy=NULL;
    m_currentSignalName="custom_signal";
    m_pCurrentSignal=NULL;
    m_updateWhenNews=false;
    m_start=0;
    m_end=3600;//fine esame di default a 1 ora
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_changesToBeSaved=false;
}

MDataManager::~MDataManager()
{
    if(m_mng!=NULL)
    {
        m_mng->Close();
        delete m_mng;
    }
}

void MDataManager::setInfoList(QVariantList __list)
{
    if(__list!=m_infoList)
    {
        m_infoList=__list;
        emit infoListChanged();
    }
}

QString MDataManager::currentSignal()
{
    return m_currentSignalName;
}

bool MDataManager::addSignal(MSignal *__pSignal)
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
    saveDataAndUpdate(family,name,vec);
    emit availableTracksChanged();
    return true;

}

bool MDataManager::setCurrentSignal(QString __name)
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

void MDataManager::setConfigurationFile(QString __name)
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



void MDataManager::loadFile(QString __fileName)
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
            saveDataAndUpdate(family,name,mrkOpVec);
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
            saveDataAndUpdate(family,name,defVec);
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
                saveDataAndUpdate(m_mng->GetChanName(nc),"Definers",subVec);
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
            saveDataAndUpdate(family,name,mrkAnVec);
        }

        for(int32_t nc=0;nc<m_mng->GetChanNum();nc++)
        {
            VarMapVec *subVec=new VarMapVec;
            for(i=0;i<numChVec.size();i++)
                if(nc==numChVec[i])
                    subVec->append(mrkAnVec->at(i));

            if(!subVec->isEmpty())
                saveDataAndUpdate(m_mng->GetChanName(nc),"Analytical Markers",subVec);
        }
        break;
        m_mng->Close();
    }
    default:qDebug()<<"Should not be here!!!!!";break;
    }


}


void MDataManager::resetAll()
{    
    //    m_data.clear();
    //    m_availableData.clear();
    //    for(int i=0;i<m_signalVector.size();i++)
    //        delete m_signalVector[i];
    //    m_signalVector.clear();
    //    emit availableTracksChanged();
    //    emit availableDataChanged();
}


void MDataManager::saveChanges()
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



    VarMapVec *elements=m_storage.getAll();

    foreach (VarMap *curMap, (*elements)) {
        qDebug()<<"Salvo l'oggetto: "<<curMap;
        //-Salvataggio Marker
        if(curMap->value("type").toString()==TYP_MARKER)
        {
            qDebug()<<"Inizio salvataggio marker";
            int32_t *numCamp;
            numCamp=new int32_t[m_copy->GetChanNum()];
            qDebug()<<curMap->value("val").toFloat();
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
            qDebug()<<"Fine salvataggio marker";
        }
        //-Salvataggio Definer
        if(curMap->value("type").toString()==TYP_DEFINER)
        {
            qDebug()<<"Inizio salvataggio definer";
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
                qDebug()<<start[i]<<end[i]<<enCh[i];
            }
            m_copy->AddOpMarkerAn(start,end,enCh,curMap->value("key").toInt(),curMap->value("popUp").toString());
            qDebug()<<"Fine salvataggio definer";
        }
        //oltre questi due if non ci si dovrebbe arrivare a meno che non sia un segnale e nel caso si prosegue
    }

    qDebug()<<"Commit markers?"<<m_copy->CommitMarkers();
    qDebug()<<"Chiudo il file?"<<m_copy->Close();

}
/**
 * @brief MDataManager::addCustomObj add a custom object like a marker or a definer linked to __families,
 * @param __families is the list of families were to insert the new obj
 * @param __type marker definer ecc.
 * @param __info to add to the object
 * @return
 */
bool MDataManager::addCustomObj(QStringList __families, QString __name, QString __type, QVariantList __info)
{
    qDebug()<<"Aggiungi alle famiglie "<<__families<<" un "<<__type<<" chiamato "<<__name<<" con queste caratteristiche"<<__info;
    if(!m_possibleTypes.contains(__type))
        return error("MDataManager::addCustomObj","unknown type"+__type);
    if(__families.isEmpty())
        return error("MDataManager::addCustomObj","Families corrupted");
    foreach (QString family, __families) {
        //devo aggiungere il mio nuovo oggetto ad ogni famiglia che ho scelto
        VarMapVec *objVec=NULL;

        if(__type==TYP_MARKER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *mrk=new VarMap;
                (*mrk)["val"]=1;
                (*mrk)["popUp"]="New Marker";
                (*mrk)["lock"]=false;
                (*mrk)["color"]=COLOR_CUSTOM;
                (*mrk)["visible"]=true;
                (*mrk)["type"]=TYP_MARKER;
                objVec->append(mrk);
            }
        }

        if(__type==TYP_DEFINER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *def=new VarMap;
                (*def)["xMin"]=0;
                (*def)["xMax"]=1;
                (*def)["yMin"]=0;
                (*def)["yMax"]=1;
                (*def)["popUp"]="New Definer";
                (*def)["color"]="white";
                (*def)["type"]=TYP_DEFINER;
                (*def)["resizeable"]=1;
                //(*def)["moveable"]=1;
                objVec->append(def);
            }
        }
        if(objVec!=NULL)
        {
            if(saveDataAndUpdate(family,__name,objVec,APPEND))
                continue;
            else
                return false;
        }
        else
            return error("MDataManager::addCustomObj","Type "+__type+" not recognized");
    }
    return true;

}


void MDataManager::updateAvailableData()
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

bool MDataManager::loadConfiguration(QString __name)
{
    QString curConfigFile="";

    if(__name=="")
        curConfigFile=m_configurationFileName;
    else
        curConfigFile=__name;

    if(!QFile::exists(curConfigFile))
        return error("MDataManager::loadConfiguration()","File "+curConfigFile+" does not exists");
    if(!m_configuration.loadFromXML(curConfigFile))
        return error("MDataManager::loadConfiguration()","XML file corrupted");

    qDebug()<<curConfigFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
}

void MDataManager::saveConfiguration()
{
    m_configuration.saveToXML(m_configurationFileName);
}

/**
 * @brief MDataManager::registerModel registers a model to the class
 * @param __type of the model marker, definer, track ...
 * @param __roles is the list of roles of the current model
 */
void MDataManager::registerModel(QString __type, QStringList __roles)
{
    m_possibleTypes<<__type;
    m_modelMap[__type]=__roles;
}

QVariantList MDataManager::getData(QString __type)
{
    ModelManager mng;
    mng.setType(__type);
    mng.setRoles(m_modelMap[__type]);
    mng.setStore(&m_storage);
    mng.setInfoList(m_infoList);
    return mng.drawList();
}

QVariantList MDataManager::getPlotLimits()
{
    QMap<QString, QStringList> infoMap,plotMap;

    for(int plotIndex=0;plotIndex<m_infoList.size();plotIndex++)
    {//dalle info che ci arrivano da dialog creiamo una mappa dove ad ogni plot assegniamo
        //quello che ci va disegnato
        QVariantList plotInfo=m_infoList.at(plotIndex).toList();
        QString plotName=plotInfo.at(0).toString();
        QStringList info=plotInfo.at(1).toStringList();
        infoMap[plotName]=info;
    }

    foreach (QString plotName, infoMap.keys()) {
        foreach (QString data, infoMap[plotName]) {
            if(data.contains("Signal"))//cerco segnali originali
                plotMap[plotName]<<data.split(":").at(0);
        }
    }

    //ora abbiamo dentro plotMap l'elenco dei nomi dei segnali segnali che ci servono

    QVariantList limits;

    foreach (QString plotName, plotMap.keys()) {
        QStringList families=plotMap[plotName];
        limits<<"$Limit";
        limits<<plotName;
        if(!families.isEmpty())
        {
            float xMin=INF,xMax=-INF,yMin=INF,yMax=-INF;
            foreach (QString family, families) {
                VarMapVec *cur=(VarMapVec *)m_storage.pickUp(family,"Signal");

                foreach(VarMap *map,*cur)
                {
                    qulonglong p=(*map)["pointer"].toULongLong();
                    MSignal *sig=(MSignal*)p;
                    if(sig->getT0()<xMin)xMin=sig->getT0();
                    if(sig->getDuration()>xMax)xMax=sig->getDuration();
                    if(sig->minimum()<yMin)yMin=sig->minimum();
                    if(sig->maximum()>yMax)yMax=sig->maximum();
                }
            }
            limits<<xMin<<xMax<<yMin<<(yMax+abs(yMax*0.05));
        }
        limits<<"&Limit";
    }
    qDebug()<<limits;
    return limits;
}

bool MDataManager::changeObject(QVariantList __curObj)
{
    //qDebug()<<"Cambio un elemento con queste caratteristiche :"<<__curObj;
    if(__curObj.contains("code"))
    {
        int codeIndex=__curObj.indexOf("code")+1;

        QVariantList fullCode=__curObj.at(codeIndex).toList();
        QString family=fullCode.at(0).toString();
        QString name=fullCode.at(1).toString();
        qulonglong code=fullCode.at(2).toULongLong();
        //tolgo il fullcode e lascio solo le proprietà
        __curObj.removeFirst();
        __curObj.removeFirst();
        //qDebug()<<"Richiesta di modifica per "<<family<<name<<code;
        if(m_storage.modifyElement(family,name,code,__curObj))
            m_changesToBeSaved=true;
    }
    return error("MDataManager::changeObject","No code info founded");
}

QVariant MDataManager::getSignal(QString __name)
{
    VarMapVec *v=(VarMapVec*) m_storage.pickUp(__name,"Signal");
    return v->at(0)->value("pointer");
}

/**
 * @brief MDataManager::getLinks
 * @param __what
 * @param __filter
 * @return
 */
QStringList MDataManager::getLinks(QString __what, QStringList __filterFamily, QStringList __filterType)
{
    //i filters servono quando devo ottenere i nomi date le famiglie
    QStringList list;
    if(__what=="Families")
        list<<m_storage.getFamilies();
    if(__what=="Names")
        list<<m_storage.getNames(__filterFamily,__filterType);

    return list;
}

bool MDataManager::saveDataAndUpdate(QString __family, QString __name, VarMapVec* __elements, bool __whatIfAlreadyPresent)
{
        if(__elements->isEmpty())
               return error("MDataManager::saveDataAndUpdate","No data in __elements");

        if(m_storage.archive(__family,__name,__elements,__whatIfAlreadyPresent))
        {//se siamo qua dentro vuol dire che tutto è andato liscio e possiamo visualizzare le info all'utente
            if(!m_data.keys().contains(__family))
            {
                m_data[__family].append(__name);
                emit availableTracksChanged();
            }
            else
                if(!m_data[__family].contains(__name))
                    m_data[__family].append(__name);
            updateAvailableData();
            return true;
        }
        else
            return false;

}
