#include "mdatamanager.h"


MDataManager::MDataManager(QObject *parent)
{
    m_mng=NULL;
    m_ana = NULL;
    m_copy=NULL;
    m_currentSignalName="custom_signal";
    m_pCurrentSignal=NULL;
    m_updateWhenNews=false;
    m_start=0;
    m_end=3600;//fine esame di default a 1 ora
    //m_applicationPath=applicationDirPath();
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_changesToBeSaved=false;

    m_analized = false;
    m_autoPrint = false;
    m_numAna = 0;
    m_toSave = "";
    setValVolRes(-999);
}

MDataManager::~MDataManager()
{
    if(m_mng!=NULL)
    {
        m_mng->Close();
        delete m_mng;
    }
    if (m_ana != NULL)
    {
        m_ana = NULL;
        delete m_ana;
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
    (*traccia)["descr"]=__pSignal->getName();
    (*traccia)["category"]=CAT_TRACK;
    (*traccia)["pointer"]=(qulonglong)__pSignal;
    vec->append(traccia);
    saveDataAndUpdate(family,name,vec);
    emit availableTracksChanged();
    return true;

}



void MDataManager::loadFile(QString __fileName)
{
    __fileName.remove("file:///");
    qDebug()<<"Apro il file "<<__fileName;
    if(!QFile::exists(__fileName))
    {qCritical()<<__fileName<<MEX_FILE_NOT_EXISTS;return;}
    m_fileName=__fileName;

    int n;

    VarMapVec *mrkOpVec=new VarMapVec;
    VarMapVec *mrkAnVec=new VarMapVec;
    VarMapVec *defVec=new VarMapVec;
    QMap<int,QVariantList > defEn;

    double sigMin=INF,sigMax=-INF;
    byte key;
    int32_t numCh;
    int32_t numSamp[4];//4 byte per avere il numero del campione
    int32_t numDef;
    int32_t tStart[20],tEnd[20];
    QString descr;
    byte chEn[20];

    int i;

    switch(fileType(__fileName))
    {
    case PIC:
    {
        if(m_mng!=NULL)
        {
            delete m_mng;
        }
        m_mng=new DatafileManager;
        m_mng->SetFileName(__fileName);
        m_mng->SetFileType(6);
        qDebug()<<"File Aperto?"<<m_mng->Open();
        qDebug()<<"File Caricato?"<<m_mng->GetParameters();

        qDebug()<<"Building configuration file ...";
        if(!buildConfigurationFile())
        {qCritical()<<MEX_FILE_CORRUPTED;return;}

        QString patientName=m_mng->GetPatient().section(";",0,1);
        patientName.replace(";","_");
        m_sexPatient = true;
        if (m_mng->GetPatient().section(";",12,12) == "F")
            m_sexPatient = true;
        else
            m_sexPatient = false;
        n=m_mng->GetDuration();
        m_end=n/1000;
        qDebug()<<"Durata esame = "<<m_end;
        qDebug()<<"NÂ° di canali = "<<m_mng->GetChanNum();

        //------ Aggiungo i markers operativi, sono comuni a tutti i canali

        qDebug()<<"Marker Operativi = "<<m_mng->GetNumOperativeMarkers();



        for(i=0;i<m_mng->GetNumOperativeMarkers();i++)
        {
            VarMap *mrk=new VarMap;
            m_mng->GetOpMarker(i,&key,numSamp,&descr);
            qDebug()<<numSamp[0];
            qDebug()<<numSamp[1];
            qDebug()<<numSamp[2];
            qDebug()<<numSamp[3];
            double val=(double)numSamp[0]/m_mng->GetNAS(0);
            qDebug()<<"Marker"<<key<<val;
            if(m_markerMap.keys().contains(key))
            {//marker conosciuto le info ce le ho giÃ
                (*mrk)=m_markerMap[key];
            }
            else
            {//me lo costruisco
                (*mrk)["code"]="USR";
                (*mrk)["descr"]=descr;//sovrascrivo
                (*mrk)["lock"]=false;
                (*mrk)["key"]=key;
                (*mrk)["color"]=COLOR_OPERATIVE;
                (*mrk)["visible"]=true;
                (*mrk)["category"]=CAT_MARKER;
            }
            (*mrk)["val"]=val;

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
            sig->resize(m_mng->GetSamplesNumber(h));
            for(int i=0;i<m_mng->GetSamplesNumber(h);i++)
                sig->replace(i,m_mng->GetValue(h,i));

            qDebug()<<sig;
            sig->setName(m_mng->GetChanName(h));
            sig->setSamplingFrequency(m_mng->GetNAS(h));
            double M=sig->maximum();
            double m=sig->minimum();
            if(sigMax<M)
                sigMax=M;
            if(sigMin>m)
                sigMin=m;

            qDebug()<<(*sig);
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

            (*def)["key"]=key;
            (*def)["name"] = descr;
            (*def)["xMin"]=(tStart[0]-1)/m_mng->GetNAS(0);
            (*def)["xMax"]=(tEnd[0]-1)/m_mng->GetNAS(0);
            (*def)["yMin"]=sigMin;
            (*def)["yMax"]=sigMax;
            (*def)["enCh"]=defEn[i];
            (*def)["descr"]=descr;
            (*def)["color"]="cyan";
            (*def)["category"]=CAT_DEFINER;
            (*def)["resizeable"]=1;
            defVec->append(def);

        }
        if(!defVec->isEmpty())
        {
            QString family="Definers";
            QString name="Operative";
            saveDataAndUpdate(family,descr,defVec);
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

            double val=(double)numSamp[i]/m_mng->GetNAS(0);
            qDebug()<<val;
            (*mrk)["val"]=val;
            (*mrk)["descr"]=descr;
            (*mrk)["lock"]=false;
            (*mrk)["channel"]=numCh;
            (*mrk)["defCode"]=(qulonglong)defVec->value(numDef);
            (*mrk)["key"]=key;
            (*mrk)["color"]=COLOR_ANALYTICAL;
            (*mrk)["visible"]=true;
            (*mrk)["category"]=CAT_MARKER;
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


        qDebug()<<"Building infoList ...";
        if(!updateInfoList())
        {qCritical()<<"Error building infolist";return;}

        //libreria di analisi: creo oggetto.
        m_ana = new Analyze();
        m_mng->Close();
        break;

    }
    default:qDebug()<<"Should not be here!!!!!";break;
    }

    emit loadingCompleted();


    qDebug()<<"Load file operation completed succesfully!";
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
    //se c'era giÃ  un'altra copia la cancelliamo
    if(m_copy!=NULL)
        delete m_copy;

    m_copy=new DatafileManager;
    QString copyName=m_fileName;
    copyName.insert(copyName.length()-4,"_copy");

    qDebug()<<"Faccio la copia?"<<QFile::copy(m_fileName,copyName);
    //apro il file copia

    m_copy->SetFileName(copyName);
    m_copy->SetFileType(6);
    qDebug()<<"Copia aperta?"<<m_copy->Open();
    qDebug()<<"Copia caricata?"<<m_copy->GetParameters();
    qDebug()<<"Eliminati marker e definer?"<<m_copy->DeleteAllMarkers();


    //-Salvataggio Marker
    VarMapVec *elements=m_storage.getAll(CAT_MARKER);

    foreach (VarMap *curMap, (*elements)) {
        qDebug()<<"Salvo l'oggetto: "<<curMap;

        qDebug()<<"Inizio salvataggio";
        int32_t *numCamp;
        numCamp=new int32_t[m_copy->GetChanNum()];
        qDebug()<<curMap->value("val").toFloat();
        for(int i=0;i<m_copy->GetChanNum();i++)
            numCamp[i]=curMap->value("val").toFloat()*m_copy->GetNAS(i);
        if(curMap->value("color").toString()==COLOR_OPERATIVE)
        {
            m_copy->AddOpMarker(numCamp,curMap->value("key").toInt(),curMap->value("descr").toString());
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
    elements=m_storage.getAll(CAT_DEFINER);

    foreach (VarMap *curMap, (*elements)) {
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
        m_copy->AddOpMarkerAn(start,end,enCh,curMap->value("key").toInt(),curMap->value("descr").toString());
        qDebug()<<"Fine salvataggio definer";
    }
    //oltre questi due if non ci si dovrebbe arrivare a meno che non sia un segnale e nel caso si prosegue


    qDebug()<<"Commit markers?"<<m_copy->CommitMarkers();
    qDebug()<<"Chiudo il file?"<<m_copy->Close();

}
/**
 * @brief MDataManager::addCustomObj add a custom object like a marker or a definer linked to __families,
 * @param __families is the list of families were to insert the new obj
 * @param __cat marker definer ecc.
 * @param __info to add to the object
 * @return
 */
bool MDataManager::addCustomObj(QStringList __families, QString __name, QString __cat, QVariantList __info)
{
    qDebug()<<"Aggiungi alle famiglie "<<__families<<" un "<<__cat<<" chiamato "<<__name<<" con queste caratteristiche"<<__info;
    if(!m_possibleCategories.contains(__cat))
    {qCritical()<<"unknown category"+__cat;return false;}
    if(__families.isEmpty())
    {qCritical()<<"Families corrupted";return false;}
    foreach (QString family, __families) {
        //devo aggiungere il mio nuovo oggetto ad ogni famiglia che ho scelto
        VarMapVec *objVec=NULL;

        if(__cat==CAT_MARKER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *mrk=new VarMap;
                (*mrk)["val"]=1;
                (*mrk)["descr"]="New Marker";
                (*mrk)["lock"]=false;
                (*mrk)["color"]=COLOR_CUSTOM;
                (*mrk)["visible"]=true;
                (*mrk)["category"]=CAT_MARKER;
                objVec->append(mrk);
            }
        }

        if(__cat==CAT_DEFINER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *def=new VarMap;
                (*def)["key"]="-1";
                (*def)["xMin"]=0;
                (*def)["xMax"]=1;
                (*def)["yMin"]=0;
                (*def)["yMax"]=1;
                (*def)["descr"]="New Definer";
                (*def)["color"]="white";
                (*def)["category"]=CAT_DEFINER;
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
        {qCritical()<<"Category "+__cat+" not recognized";return false;}
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

bool MDataManager::updateInfoList()
{
    // grafici

    QStringList graphs=m_chanInPlots.keys();

    QVariantList infoList;
    int countGraphs = 0;
    foreach(QString graph,graphs)
    {
        QVariantList pair;
        QStringList elements;
        QVariant row,gName,gElemPack;
        gName=graph;
        //riempo con gli elementi che mi servono

        //tracce molto facile dato che ce le ho giÃ
        foreach (QString chanName, m_chanInPlots[graph]) {
            elements<<chanName+":Signal";
        }

        //markers
//        VarMapVec *op = m_storage.getAll(CAT_MARKER);
//        if (op->size() > 0)
//            elements<<"Markers:Operative";

        //        if (m_mng->GetNumDefiners()> 0)
        //           elements<<"Definers:Operative";

        VarMapVec *def = m_storage.getAll(CAT_DEFINER);
        //        if (def->size() > 0)
        //            elements<<"Definers:Operative";
        foreach (VarMap *curMap, (*def)) {
            QStringList enabled = curMap->value("enCh").toStringList();
            if (enabled.at(countGraphs) == "1")
                elements<<"Definers:"+curMap->value("name").toString();
        }

        //elementi finiti
        gElemPack=elements;
        //a posto impacchetto tutto
        pair<<gName<<gElemPack;
        row=pair;
        infoList<<row;

        countGraphs++;
    }
    qDebug()<<"infolist update   "<< infoList;
    m_infoList=infoList;


    return true;
}

bool MDataManager::buildInfoList()
{
    /*abbiamo detto che di default ci sono:
     *
     * le tracce per ogni grafico
     * i marker operativi 1 2 e 6 per ogni grafico
     * i definitori relativi ai canali
     * i marker di commento per ogni grafico
     * i marker di distanza per ogni grafico
     */

    //allora dato che questa funzione Ã¨ chiamata dopo aver costruito i plotter
    //so giÃ  quanti e come si chiamano i grafici

    QStringList graphs=m_chanInPlots.keys();

    QVariantList infoList;
    foreach(QString graph,graphs)
    {
        QVariantList pair;
        QStringList elements;
        QVariant row,gName,gElemPack;
        gName=graph;
        //riempo con gli elementi che mi servono

        //tracce molto facile dato che ce le ho giÃ
        foreach (QString chanName, m_chanInPlots[graph]) {
            elements<<chanName+":Signal";
        }

        //markers
        if(m_mng->GetNumOperativeMarkers()>0)
            elements<<"Markers:Operative";

        if (m_mng->GetNumDefiners()> 0)
            elements<<"Definers:Operative";

        //elementi finiti
        gElemPack=elements;
        //a posto impacchetto tutto
        pair<<gName<<gElemPack;
        row=pair;
        infoList<<row;
    }

    qDebug()<<"infolist   "<< infoList;
    m_infoList=infoList;

    return true;
}

/**
 * @brief MDataManager::registerModel registers a model to the class
 * @param __cat of the model marker, definer, track ...
 * @param __roles is the list of roles of the current model
 */
void MDataManager::registerModel(QString __cat, QStringList __roles)
{
    m_possibleCategories<<__cat;
    m_modelMap[__cat]=__roles;
    m_storage.addCategory(__cat);

}

QVariantList MDataManager::getData(QString __cat)
{
    if(m_possibleCategories.contains(__cat))
    {
        ModelManager mng;
        mng.setType(__cat);
        mng.setRoles(m_modelMap[__cat]);
        mng.setStore(&m_storage);
        mng.setInfoList(m_infoList);
        qDebug()<<mng.drawList();
        return mng.drawList();
    }
    else
    {
        qCritical()<<"Category"<<__cat<<"not registered!";
        return QVariantList();
    }
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
            double xMin=INF,xMax=-INF,yMin=INF,yMax=-INF;
            foreach (QString family, families) {
                VarMapVec *cur=(VarMapVec *)m_storage.pickUp(family,"Signal");

                foreach(VarMap *map,*cur)
                {
                    qulonglong p=(*map)["pointer"].toULongLong();
                    MSignal *sig=(MSignal*)p;
                    //qDebug()<<(*sig);
                    if(sig->getT0()<xMin)xMin=sig->getT0();
                    if(sig->getDuration()>xMax)xMax=sig->getDuration();
                    if(sig->minimum()<yMin)yMin=sig->minimum();
                    if(sig->maximum()>yMax)yMax=sig->maximum();
                    //qDebug()<<yMin<<yMax;
                    //qDebug()<<"Segnale lungo:"<<sig->getSize();
                }
            }
            limits<<xMin<<xMax<<yMin<<(yMax+abs(yMax*0.05));
        }
        limits<<"&Limit";
    }
    //qDebug()<<limits;
    return limits;
}

bool MDataManager::changeObject(QVariantList __curObj)
{
    //qDebug()<<"Cambio un elemento con queste caratteristiche :"<<__curObj;
    if(__curObj.length()!=2)
    {
        qulonglong whoAmI=__curObj.first().toULongLong();
        //tolgo il whoami e lascio solo le proprietÃ
        __curObj.removeFirst();
        qDebug()<<"Richiesta di modifica per "<<whoAmI;
        if(m_storage.modifyElement(whoAmI,__curObj))
            m_changesToBeSaved=true;
        return true;
    }
    qCritical()<<"Length error";
    return false;
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
    {qCritical()<<"No data in __elements";return false;}

    if(m_storage.archive(__family,__name,__elements,__whatIfAlreadyPresent))
    {//se siamo qua dentro vuol dire che tutto Ã¨ andato liscio e possiamo visualizzare le info all'utente
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

void MDataManager::exitFromReview()
{
    if (getToSave() == "")
        emit sg_exitFromReview();
    else
    {
        QString copyName=m_fileName;
        copyName.insert(copyName.length()-4,"_copy");
        if (getToSave() == "yes")
        {        //copio il file copy nell'originale
            if (QFile::exists(copyName))
            {
                qDebug()<<"cancello vecchio file"<<QFile::remove(m_fileName);
                qDebug()<<"copio le modifiche"<<QFile::copy(copyName,m_fileName);
                qDebug()<<"cancellata copia all'exit"<<QFile::remove(copyName);
            }
        }
        else //"no"
                    //cancello il file copy
            qDebug()<<"cancellata copia all'exit"<<QFile::remove(copyName);
        exit(0); //poi dovrà tornare al modulo database
    }

}

bool MDataManager::checkForVolRes()
{
    if (getValVolRes() != -999)
        return false;

    m_mng=new DatafileManager;
    m_mng->SetFileName(m_fileName);
    m_mng->SetFileType(6);

    qDebug()<<"File Aperto?"<<m_mng->Open();
    qDebug()<<"File Caricato?"<<m_mng->GetParameters();

    //ciclo per individuare se Ã¨ necessario aprire la dlg del volume residuo
    bool volRes = false;
    for (int i=0; i<m_numAna; i++)
    {
        QString anaType = m_mng->GetAnalysis(i);
        if (anaType == "Flussimetria")
        {
            if (m_autoPrint)
                setValVolRes(-1);
            else
            {
                volRes = true;
                setValVolRes(0);//in futuro sarÃ  letto da proprietÃ  xml
                emit sg_openVolResDlg("Flowmetry");
                break;
            }
        }

    }

    m_mng->Close();
    return volRes;
}

void MDataManager::analysis()
{
    //                Flussimetria
    //                Cistometria (Riempimento)
    //                PFS (Riemp.+Svuot.))
    //                Flussimetria domiciliare
    //                UPP statico
    //                UPP Dinamico
    //                Whitaker Test
    //                Leak Point Pressure
    //                Detrusor Overactivity
    //                Biofeedback
    //                Compliance


    if (checkForVolRes())
        return;

    m_mng=new DatafileManager;
    m_mng->SetFileName(m_fileName);
    m_mng->SetFileType(6);

    qDebug()<<"File Aperto?"<<m_mng->Open();
    qDebug()<<"File Caricato?"<<m_mng->GetParameters();

    //per ora salvo io su file pic l'analisi flussimetria ...
    m_mng->SetAnalysis("Flussimetria");
    m_ana->SetData(m_mng);


    int anaProg = QDateTime::currentDateTime().toTime_t();
    QVariantList En;

    m_numAna = m_mng->GetAnalysiNum();
    for (int i=0; i<m_numAna; i++)
    {
        QString anaType = m_mng->GetAnalysis(i);
        if (anaType == "Flussimetria")
        {
            //verifica se c'Ã¨ un definitore per questa analisi.
            VarMap mkOpAnIn;
            bool found = false;
            VarMapVec* elements=m_storage.getAll(CAT_DEFINER);
            foreach (VarMap *curMap, (*elements))
            {
                QChar tempChar = curMap->value("key").toChar();
                unsigned char key = tempChar.toLatin1();
                if (key == MK_FLOWMETRY)
                {
                    mkOpAnIn = *curMap;
                    found = true;
                }
            }

            if (!found)
            {
                int posQ = -1, posV = -1;
                int i = 0;
                foreach(MSignal *sig,m_signalVector)
                {
                    QString name = sig->getName();
                    if (name == "Q")
                        posQ = i;

                    if (name == "VLMv")
                        posV = i;
                    i++;
                }

                if (m_numAna == 1 || posQ > -1)
                {
                    //se non c'Ã¨ il defintore, ma questa Ã¨ l'unica analisi,
                    //viene inserito automaticamente sul canale del flusso.
                    for (int i=0;i<m_mng->GetChanNum();i++)
                        En << 0;

                    En[posQ] = 1;
                    if (posV > -1)
                        En[posV] = 1;

                    //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'Ã¨

                    //provo a disegnare il definitore
                    QString family="Definers";
                    QString name="Flowmetry";
                    VarMap *def=new VarMap;
                    (*def)["key"] = MK_FLOWMETRY;
                    (*def)["name"] = name;
                    (*def)["xMin"]= 1;
                    (*def)["xMax"]= m_end-1;
                    (*def)["yMin"]=0;
                    (*def)["yMax"]=100;
                    (*def)["enCh"]= En;
                    (*def)["descr"]=name;
                    (*def)["color"]="green";
                    (*def)["category"]=CAT_DEFINER;
                    (*def)["resizeable"]=1;

                    qDebug()<<*def;
                    VarMapVec *defVec=new VarMapVec;
                    defVec->append(def);
                    saveDataAndUpdate(family,name,defVec,APPEND);
                    mkOpAnIn = *def;
                    found = true;

                    //saveall?? se si la updateinfolist puo leggere da datafile?
                    saveChanges();//(?)
                    //buildInfoList();
                    updateInfoList();
                    emit reloadingCompleted();

                }
                //                else
                //                {
                //                    //cotrolliamo se c'Ã¨ il marker 'ff' free flow
                //                    //nel qual caso, il definitore viene inserito da lÃ¬ fino a:
                //                    //fine esame o inizio FILL o inizio VOID
                //                    VarMapVec* elements=m_storage.getAll(CAT_MARKER);
                //                    foreach (VarMap *curMap, (*elements))
                //                    {
                //                        if(curMap->value("color").toString()==COLOR_OPERATIVE)
                //                        {
                //                            QChar tempChar = curMap->value("key").toChar();
                //                            unsigned char key = tempChar.toLatin1();
                //                            if (key == MK_FREEFLOW)
                //                            {
                //                                int defEnd = m_end;
                //                                for (int i=0;i<m_mng->GetChanNum();i++)
                //                                    En << 0;

                //                                En[posQ] = 1;
                //                                if (posV > -1)
                //                                    En[posV] = 1;

                //                                //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'Ã¨


                //                                VarMapVec* definers=m_storage.getAll(CAT_DEFINER);
                //                                foreach (VarMap *curDef, (*definers))
                //                                {
                //                                    QChar tempChar = curDef->value("key").toChar();
                //                                    unsigned char key = tempChar.toLatin1();
                //                                    if ((key == MK_VOIDING) || (key == MK_FILLING))
                //                                    {
                //                                      //  int startDef = curDef->value("xMin");

                //                                   //     if (defEnd > startQ || startQ > )


                //                                                //
                //                                                //                            For k = 0 To MarkerUtils.getNumOpMarkerAn() - 1
                //                                                //                                If MarkerUtils.getOpMarkerAn(k).myKey = MK_VOID Or MarkerUtils.getOpMarkerAn(k).myKey = MK_FILL Then
                //                                                //                                    If (defEnd > MarkerUtils.getOpMarkerAn(k).myNumStart(myGraphPlot.posQ)) And (MarkerUtils.getOpMarkerAn(k).myNumStart(myGraphPlot.posQ) > MarkerUtils.getOpMarker(j).myNumCamp(myGraphPlot.MaxNASCh)) Then
                //                                                //                                        defEnd = MarkerUtils.getOpMarkerAn(k).myNumStart(myGraphPlot.posQ)
                //                                                //                                    End If
                //                                                //                                End If
                //                                                //                            Next k

                //                                    }
                //                                }

                //                                //                            For k = 0 To MarkerUtils.getNumOpMarker() - 1
                //                                //                                If MarkerUtils.getOpMarker(k).myKey = MK_INVITATION Or MarkerUtils.getOpMarker(k).myKey = MK_FIRSTDESIRE Or MarkerUtils.getOpMarker(k).myKey = MK_STRONG_DESIRE Or MarkerUtils.getOpMarker(k).myKey = MK_MAXCYSCAP Then
                //                                //                                    If (defEnd > MarkerUtils.getOpMarker(k).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh))) And (MarkerUtils.getOpMarker(k).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)) > MarkerUtils.getOpMarker(j).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh))) Then
                //                                //                                        defEnd = MarkerUtils.getOpMarker(k).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh))
                //                                //                                    End If
                //                                //                                End If
                //                                //                            Next k
                //                                //                            If defEnd > 1 And MarkerUtils.getOpMarker(j).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)) Then
                //                                //                                Dim TdefEnd As Double = myGraphPlot.calcPosByNumCampTotal(defEnd, myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh))
                //                                //                                Dim TdefStart As Double = myGraphPlot.calcPosByNumCampTotal(MarkerUtils.getOpMarker(j).myNumCamp(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)), myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh))
                //                                //                                Dim imageDef As Image = Nothing
                //                                //                                Dim tempB As Boolean = findOpMarkerAnIcon(MK_FLW, imageDef, "f1")

                //                                //                                MarkerUtils.drawOpMarkerAn(myGraphPlot, MK_FLW, "f1", "f1", En, TdefStart, TdefEnd, New Point2D(1, 0.0), imageDef)
                //                                //                                mkOpAnIn = MarkerUtils.getNumOpMarkerAn()
                //                                //                                myGraphPlot.saveAllChanges()
                //                                //                                found = True
                //                                //                                Exit For
                //                                //                            End If
                //                            }
                //                        }

                //                    }
                //                }
            }

            if (found)
            {
                //                                        'Salva l'immagine dei tracciati all'interno del definitore
                //                                        myGraphPlot.RedrawGraphForPrint("GR200", MarkerUtils.getOpMarkerAn(mkOpAnIn - 1).myNumStart(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)), MarkerUtils.getOpMarkerAn(mkOpAnIn - 1).myNumEnd(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)))
                m_analized = True;
                //                                            UpdateTestOther()
                //Lancia analisi e Inizializza nomogrammi
                InitPageGraphs("Flowmetry");
                //                                            'ridimensiono il definitore sulla base del marker F2 del flusso
                //                                            'For k = 1 To theGraphs.NMarkerAn
                //                                            '    If MarkerAn(k).NumOpMark = mkOpAnIn And MarkerAn(k).key = 3 Then
                //                                            '        MarkerOpAn(mkOpAnIn).end = MarkerAn(k).index * (theGraphs.MaximumNAS / curve(MarkerAn(k).NumGraph).NAS) + 1 * theGraphs.MaximumNAS
                //                                            '    End If
                //                                            'Next k

                //                                            'Dim objDef As objectOpMarkerAn = MarkerUtils.getOpMarkerAn(mkOpAnIn)
                //                                            'For k = 1 To objDef.getNumAnMarker()
                //                                            '    If objDef.getAnMarker(k).myKey = 3 Then
                //                                            '        objDef.myNumEnd(0) = objDef.getAnMarker(k).myNumCamp(0) * ((1000/myGraphPlot.getPeriod())/
                //                                            '    End If

                //                                            'Next k
                //                                            'theGraphs.Reset_Zoom()
                //                                            'TODO: implementare una gestione degli errori che mostri
                //                                            'su DlgReport il messaggio d'errore anzichÃ¨ il risultato.
                //                                            'TODO: implementare un sistema per ottenere il numero di
                //                                            'analisi dello stesso tipo effettuate.
                //                                            'MarkerOpAn(mkOpAnIn).Warn = True
                //                                            WaitForDone = 2
            }
        }
    }


    m_mng->Close();
    emit sg_loadResult();
}


void MDataManager::setToSave(QString __val)
{
    if (m_toSave == __val)
        return;

    m_toSave = __val;
    emit infoToSave();
}

void MDataManager::setValVolRes(int  __val)
{
    QString valString = QString::number(__val);
    if (m_VolRes == valString)
        return;

    m_VolRes = valString;
    emit infoValVolRes();
}


int MDataManager::getValVolRes()
{
    return m_VolRes.toInt();
}


void MDataManager::InitPageGraphs(QString __anaType)
{
    if (__anaType == "Flowmetry")
    {
        //determina tratti da analizzare.
        //Per ora considera solo il primo.

        int evStart;
        int evEnd;
        VarMap evMarkOpIn;
        byte evAuto;
        QVector<unsigned char> enCh;

        VarMapVec* elements=m_storage.getAll(CAT_DEFINER);
        foreach (VarMap *curMap, (*elements))
        {
            QChar tempChar = curMap->value("key").toChar();
            unsigned char key = tempChar.toLatin1();
            if (key == MK_FLOWMETRY)
            {
                evStart = curMap->value("xMin").toInt();
                evEnd = curMap->value("xMax").toInt()*1000;
                evMarkOpIn = *curMap;
                evAuto = 1;

                //ogni volta che si passano i definitori alla libreria di analisi dopo aver nascosto/rivisualizzato i canali
                //Ã¨ necessario sistemare l'array dei canali abilitati considerando tutti i canali dell'analisi.
                //Se un canale non era visibile al momento dell'analisi,
                //consideriamo quel canale abilitato
                //                            If objOpMAn.myEnabled.Length < myGraphPlot.totAnalisysChannel Then
                //                                For k As Integer = 0 To myGraphPlot.totAnalisysChannel - 1
                //                                    ChEn(k) = 1
                //                                Next
                //                                For k = 0 To myGraphPlot.myPosChannelShow.Length - 1
                //                                    ChEn(myGraphPlot.GetTruePosChannel(k)) = objOpMAn.myEnabled(k)
                //                                Next
                //                            Else
                //                                ChEn = objOpMAn.myEnabled
                //                            End If



                for(int i=0;i<m_mng->GetChanNum();i++)
                    enCh.append(curMap->value("enCh").toList().at(i).toBool());


                // marker analitici
                VarMapVec* eleAnMarkers=m_storage.getAll(CAT_MARKER);
                foreach (VarMap *curMarker, (*eleAnMarkers))
                {
                    if ((curMarker->value("color") == COLOR_ANALYTICAL) && (curMarker->value("defCode") == evMarkOpIn))
                    {
                        evAuto = 0;
                        break;
                    }

                }

                //eventuali marker analitici nascosti
                //                            For i = 1 To MarkerUtils.getNumAnMarkerHide - 1
                //                                Dim objOpM As objectAnMarkerHide = MarkerUtils.getAnMarkerHide(i - 1)
                //                                If objOpM.myMarkOp = j Then
                //                                    evAuto = 0
                //                                    Exit For
                //                                End If
                //                            Next i


            }
        }

        //init arrays and perform analysis
        bool ret = InitArraysFLW(evStart,evEnd,enCh,evMarkOpIn.value("defCode").toInt(),evAuto);


    }

}

bool MDataManager::InitArraysFLW(int __start,
                                 int __end,
                                 QVector<unsigned char> __chEn,
                                 int __curDef,
                                 byte __auto)
{
    double startTh = 0;
    double heightTh = 0;
    double widthTth = 0;

    Ancestry *config_ana = new Ancestry;
    config_ana->loadFromXML(":/Config/ParAna.xml");

    QString value;
    foreach (Ancestry *child,config_ana->getChildren())
    {
        if (child->name() == "Analysis")
        {
            foreach (Ancestry *def,child->getChildren())
            {
                value=def->getAttribute("Type");
                if (value.toInt() == MK_FLOWMETRY)
                {
                    Ancestry *flusso = def->getChild("Q");
                    startTh = flusso->getChild("StartTh")->getTextOfChild("value").toDouble();
                    heightTh = flusso->getChild("AmpTh")->getTextOfChild("value").toDouble();
                    widthTth = flusso->getChild("DurTh")->getTextOfChild("value").toDouble();
                }
            }
        }

    }

    int res = m_ana->FLW_Adv_Analysis_Time(1,__chEn,__start,__end,__curDef,startTh,heightTh,widthTth,__auto,getValVolRes(), 0);
    if (res < 0)
        return false;

    //Legge i risultati
    int numEv = 1;
    res = ReadResult(numEv);
    if (res < 0)
        return false;

    //costruisco i segnali da disegnare nel plot per i nomogrammi
    for (int i=0;i<numEv;i++)
    {
        //nomogramma LiverpoolQMax
        MSignal *sig0=new MSignal;
        MSignal *sig1=new MSignal;
        MSignal *sig2=new MSignal;
        MSignal *sig3=new MSignal;
        MSignal *sig4=new MSignal;
        MSignal *sig5=new MSignal;
        MSignal *sig6=new MSignal;
        int lunx = m_aflwdatas.at(i+1)->getLiverpoolMax()->getXmax();
        sig0->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(0).data(),lunx);
        sig0->setName("linea1");
        sig1->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(1).data(),lunx);
        sig1->setName("linea2");
        sig2->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(2).data(),lunx);
        sig2->setName("linea3");
        sig3->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(3).data(),lunx);
        sig3->setName("linea4");
        sig4->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(4).data(),lunx);
        sig4->setName("linea5");
        sig5->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(5).data(),lunx);
        sig5->setName("linea6");
        sig6->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(6).data(),lunx);
        sig6->setName("linea7");
        QVariantList tracce;
        tracce<< "$Track"<<"family"<<sig0->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig0<<"&Track";
        tracce<< "$Track"<<"family"<<sig1->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig1<<"&Track";
        tracce<< "$Track"<<"family"<<sig2->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig2<<"&Track";
        tracce<< "$Track"<<"family"<<sig3->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig3<<"&Track";
        tracce<< "$Track"<<"family"<<sig4->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig4<<"&Track";
        tracce<< "$Track"<<"family"<<sig5->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig5<<"&Track";
        tracce<< "$Track"<<"family"<<sig6->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig6<<"&Track";
        QVariantList colori;
        colori << "green" << "red" << "white" << "white" << "white" << "white" << "green";
        m_aflwdatas.at(i+1)->getLiverpoolMax()->setColors(colori);
        m_aflwdatas.at(i+1)->getLiverpoolMax()->setTracce(tracce);

        //nomogramma LiverpoolQAve
        MSignal *sig0Ave=new MSignal;
        MSignal *sig1Ave=new MSignal;
        MSignal *sig2Ave=new MSignal;
        MSignal *sig3Ave=new MSignal;
        MSignal *sig4Ave=new MSignal;
        MSignal *sig5Ave=new MSignal;
        MSignal *sig6Ave=new MSignal;
        lunx = m_aflwdatas.at(i+1)->getLiverpoolAve()->getXmax();
        sig0Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(0).data(),lunx);
        sig0Ave->setName("linea1");
        sig1Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(1).data(),lunx);
        sig1Ave->setName("linea2");
        sig2Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(2).data(),lunx);
        sig2Ave->setName("linea3");
        sig3Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(3).data(),lunx);
        sig3Ave->setName("linea4");
        sig4Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(4).data(),lunx);
        sig4Ave->setName("linea5");
        sig5Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(5).data(),lunx);
        sig5Ave->setName("linea6");
        sig6Ave->setData(m_aflwdatas.at(i+1)->getLiverpoolAve()->getLineY(6).data(),lunx);
        sig6Ave->setName("linea7");
        tracce.clear();
        tracce<< "$Track"<<"family"<<sig0Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig0Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig1Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig1Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig2Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig2Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig3Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig3Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig4Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig4Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig5Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig5Ave<<"&Track";
        tracce<< "$Track"<<"family"<<sig6Ave->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig6Ave<<"&Track";
        colori.clear();
        colori << "green" << "red" << "white" << "white" << "white" << "white" << "green";
        m_aflwdatas.at(i+1)->getLiverpoolAve()->setColors(colori);
        m_aflwdatas.at(i+1)->getLiverpoolAve()->setTracce(tracce);

        //nomogramma Siroky Max
        MSignal *sig0SirMax=new MSignal;
        MSignal *sig1SirMax=new MSignal;
        MSignal *sig2SirMax=new MSignal;
        MSignal *sig3SirMax=new MSignal;
        lunx = m_aflwdatas.at(i+1)->getSirokyMax()->getXmax();
        sig0SirMax->setData(m_aflwdatas.at(i+1)->getSirokyMax()->getLineY(0).data(),lunx);
        sig0SirMax->setName("linea1");
        sig1SirMax->setData(m_aflwdatas.at(i+1)->getSirokyMax()->getLineY(1).data(),lunx);
        sig1SirMax->setName("linea2");
        sig2SirMax->setData(m_aflwdatas.at(i+1)->getSirokyMax()->getLineY(2).data(),lunx);
        sig2SirMax->setName("linea3");
        sig3SirMax->setData(m_aflwdatas.at(i+1)->getSirokyMax()->getLineY(3).data(),lunx);
        sig3SirMax->setName("linea4");
        tracce.clear();
        tracce<< "$Track"<<"family"<<sig0SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig0SirMax<<"&Track";
        tracce<< "$Track"<<"family"<<sig1SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig1SirMax<<"&Track";
        tracce<< "$Track"<<"family"<<sig2SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig2SirMax<<"&Track";
        tracce<< "$Track"<<"family"<<sig3SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig3SirMax<<"&Track";
        colori.clear();
        colori << "white" << "red" << "white" << "white";
        m_aflwdatas.at(i+1)->getSirokyMax()->setColors(colori);
        m_aflwdatas.at(i+1)->getSirokyMax()->setTracce(tracce);
        //bande colorate
        QVector<int> linee;
        linee.append(3);
        linee.append(1);
        m_aflwdatas.at(i+1)->getSirokyMax()->setLinea(linee);

        //nomogramma Siroky Ave
        MSignal *sig0SirAve=new MSignal;
        MSignal *sig1SirAve=new MSignal;
        MSignal *sig2SirAve=new MSignal;
        MSignal *sig3SirAve=new MSignal;
        MSignal *sig4SirAve=new MSignal;
        lunx = m_aflwdatas.at(i+1)->getSirokyAve()->getXmax();
        sig0SirAve->setData(m_aflwdatas.at(i+1)->getSirokyAve()->getLineY(0).data(),lunx);
        sig0SirAve->setName("linea1");
        sig1SirAve->setData(m_aflwdatas.at(i+1)->getSirokyAve()->getLineY(1).data(),lunx);
        sig1SirAve->setName("linea2");
        sig2SirAve->setData(m_aflwdatas.at(i+1)->getSirokyAve()->getLineY(2).data(),lunx);
        sig2SirAve->setName("linea3");
        sig3SirAve->setData(m_aflwdatas.at(i+1)->getSirokyAve()->getLineY(3).data(),lunx);
        sig3SirAve->setName("linea4");
        sig4SirAve->setData(m_aflwdatas.at(i+1)->getSirokyAve()->getLineY(4).data(),lunx);
        sig4SirAve->setName("linea4");
        tracce.clear();
        tracce<< "$Track"<<"family"<<sig0SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig0SirAve<<"&Track";
        tracce<< "$Track"<<"family"<<sig1SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig1SirAve<<"&Track";
        tracce<< "$Track"<<"family"<<sig2SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig2SirAve<<"&Track";
        tracce<< "$Track"<<"family"<<sig3SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig3SirAve<<"&Track";
        tracce<< "$Track"<<"family"<<sig4SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line"<<"pointer"<<(qulonglong)sig4SirAve<<"&Track";
        colori.clear();
        colori << "white" << "red" << "white" << "white" << "white";
        m_aflwdatas.at(i+1)->getSirokyAve()->setColors(colori);
        m_aflwdatas.at(i+1)->getSirokyAve()->setTracce(tracce);
        //bande colorate
        linee.clear();
        linee.append(4);
        linee.append(1);
        m_aflwdatas.at(i+1)->getSirokyAve()->setLinea(linee);
    }
    return true;
}



int MDataManager::ReadResult(int __numEv)
{
    int StructType = m_mng->GetAnaType();
    if (StructType <= 0)
        //error
        return -1;

    __numEv = m_mng->readNumEv();

    switch (StructType)
    {
    case FLW_AVD_STUDY: {
        //dati analisi
        m_aflwdatas.append(new mflowdatas());
        byte* strTemp = (byte*) malloc (sizeof(FLWAdvRepStruct));

        m_aflwdatas.at(0)->setWaitingTime(0);
        m_aflwdatas.at(0)->setQMax(0);
        m_aflwdatas.at(0)->setQAve(0);
        m_aflwdatas.at(0)->setTimeAtV3(0);
        m_aflwdatas.at(0)->setTimeAtV2(0);
        m_aflwdatas.at(0)->setTime90(0);
        m_aflwdatas.at(0)->setTimeAtQmax(0);
        m_aflwdatas.at(0)->setFlowTime(0);
        m_aflwdatas.at(0)->setDescTime(0);
        m_aflwdatas.at(0)->setVoidingTime(0);
        m_aflwdatas.at(0)->setVolAtQqmax(0);
        m_aflwdatas.at(0)->setVoidedVolume(0);
        m_aflwdatas.at(0)->setAcceleration(0);
        m_aflwdatas.at(0)->setResidualVolume(0);
        m_aflwdatas.at(0)->setVDetMax(0);
        m_aflwdatas.at(0)->setCQ(0);
        for (int i=1; i<=__numEv; i++)
        {
            m_aflwdatas.append(new mflowdatas());
            m_mng->readResAna(sizeof(FLWAdvRepStruct),strTemp);
            m_aflwdatas.last()->setWaitingTime(*((float*)(strTemp + WAITING_TIME)));
            m_aflwdatas.last()->setQMax(*((float*)(strTemp + MAXIMUM_FLOW)));
            m_aflwdatas.last()->setQAve(*((float*)(strTemp + AVERAGE_FLOW)));
            m_aflwdatas.last()->setTimeAtV3(*((float*)(strTemp + TIME_AT_VOL3)));
            m_aflwdatas.last()->setTimeAtV2(*((float*)(strTemp + TIME_AT_VOL2)));
            m_aflwdatas.last()->setTimeAtQmax(*((float*)(strTemp + TIME_AT_QMAX)));
            m_aflwdatas.last()->setTime90(*((float*)(strTemp + TIME_5_95_VOIDED_VOL)));
            m_aflwdatas.last()->setFlowTime(*((float*)(strTemp + FLOW_TIME)));
            m_aflwdatas.last()->setDescTime(*((float*)(strTemp + TIME_QMAX_95_VOIDED_VOL)));
            m_aflwdatas.last()->setVoidingTime(*((float*)(strTemp + VOIDING_TIME)));
            m_aflwdatas.last()->setVolAtQqmax(*((float*)(strTemp + VOLUME_QMAX)));
            m_aflwdatas.last()->setVoidedVolume(*((int*)(strTemp + VOIDED_VOLUME)));
            m_aflwdatas.last()->setAcceleration(*((float*)(strTemp + ACCELERATION)));
            m_aflwdatas.last()->setResidualVolume(*((int*)(strTemp + RESIDUAL_USER)));
            m_aflwdatas.last()->setVDetMax(*((float*)(strTemp + DETRUSOR)));
            m_aflwdatas.last()->setCQ(*((float*)(strTemp +FLOW_CORR_FACTOR)));
            m_aflwdatas.last()->buildTable();
            m_aflwdatas.last()->buildNomogrammi(m_sexPatient, 45);
        }

        //calcolo la media
        for (int i=1; i<=__numEv; i++)
        {
            m_aflwdatas.at(0)->addWaitingTime(m_aflwdatas.at(i)->getWaitingTime());
            m_aflwdatas.at(0)->addQMax(m_aflwdatas[i]->getQMax());
            m_aflwdatas.at(0)->addQAve(m_aflwdatas[i]->getQAve());
            m_aflwdatas.at(0)->addTimeAtV3(m_aflwdatas.at(i)->getTimeAtV3());
            m_aflwdatas.at(0)->addTimeAtV2(m_aflwdatas.at(i)->getTimeAtV2());
            m_aflwdatas.at(0)->addTimeAtQmax(m_aflwdatas.at(i)->getTimeAtQmax());
            m_aflwdatas.at(0)->addTime90(m_aflwdatas.at(i)->getTime90());
            m_aflwdatas.at(0)->addFlowTime(m_aflwdatas.at(i)->getFlowTime());
            m_aflwdatas.at(0)->addDescTime(m_aflwdatas.at(i)->getDescTime());
            m_aflwdatas.at(0)->addVoidingTime(m_aflwdatas.at(i)->getVoidingTime());
            m_aflwdatas.at(0)->addVolAtQqmax(m_aflwdatas.at(i)->getVolAtQqmax());
            m_aflwdatas.at(0)->addVoidedVolume(m_aflwdatas.at(i)->getVoidedVolume());
            m_aflwdatas.at(0)->addAcceleration(m_aflwdatas.at(i)->getAcceleration());
            m_aflwdatas.at(0)->addResidualVolume(m_aflwdatas.at(i)->getResidualVolume());
            m_aflwdatas.at(0)->addVDetMax(m_aflwdatas.at(i)->getVDetMax());
            m_aflwdatas.at(0)->addCQ(m_aflwdatas.at(i)->getCQ());
        }
        m_aflwdatas.at(0)->buildTable();
        free(strTemp);


        break;
    }
    default:
        break;
    }

    return StructType;

}

mflowdatas *MDataManager::getFlowDatas(int __i)
{
    return m_aflwdatas.at(__i);\
}

