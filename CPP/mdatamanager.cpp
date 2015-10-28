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
    m_applicationPath=QApplication::applicationDirPath();
    m_configurationFileLoaded=false;//nessun file di configurazione caricato
    m_changesToBeSaved=false;

    m_analized = false;
    m_autoPrint = false;
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

        qDebug()<<"Building infoList ...";
        if(!buildInfoList())
        {qCritical()<<"Error building infolist";return;}

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
            qDebug()<<numSamp[0];
            qDebug()<<numSamp[1];
            qDebug()<<numSamp[2];
            qDebug()<<numSamp[3];
            double val=(double)numSamp[0]/m_mng->GetNAS(0);
            qDebug()<<"Marker"<<key<<val;
            if(m_markerMap.keys().contains(key))
            {//marker conosciuto le info ce le ho già
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
            (*def)["xMin"]=(tStart[0]*1000)/m_mng->GetNAS(0);
            (*def)["xMax"]=(tEnd[0]*1000)/m_mng->GetNAS(0);
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

        //libreria di analisi: creo oggetto.
        m_ana = new Analyze();
        m_ana->SetData(__fileName);

        break;
        m_mng->Close();
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
    //se c'era già un'altra copia la cancelliamo
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

void MDataManager::updateInfoList()
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

        //tracce molto facile dato che ce le ho già
        foreach (QString chanName, m_chanInPlots[graph]) {
            elements<<chanName+":Signal";
        }

        //markers
        VarMapVec *op = m_storage.getAll(CAT_MARKER);
        if (op->size() > 0)
            elements<<"Markers:Operative";

//        if (m_mng->GetNumDefiners()> 0)
//           elements<<"Definers:Operative";

        VarMapVec *def = m_storage.getAll(CAT_DEFINER);
//        if (def->size() > 0)
//            elements<<"Definers:Operative";
        foreach (VarMap *curMap, (*def)) {
            QStringList enabled = curMap->value("enCh").toStringList();
            if (enabled.at(countGraphs) == "1")
                 elements<<"Definers:"+curMap->value("descr").toString();
        }

        //elementi finiti
        gElemPack=elements;
        //a posto impacchetto tutto
        pair<<gName<<gElemPack;
        row=pair;
        infoList<<row;

        countGraphs++;
    }

    m_infoList=infoList;

    return;
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

    //allora dato che questa funzione è chiamata dopo aver costruito i plotter
    //so già quanti e come si chiamano i grafici

    QStringList graphs=m_chanInPlots.keys();

    QVariantList infoList;
    foreach(QString graph,graphs)
    {
        QVariantList pair;
        QStringList elements;
        QVariant row,gName,gElemPack;
        gName=graph;
        //riempo con gli elementi che mi servono

        //tracce molto facile dato che ce le ho già
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
        //tolgo il whoami e lascio solo le proprietà
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

    //per ora salvo io su file pic l'analisi flussimetria ...
    m_mng->SetAnalysis("Flussimetria");

    int anaProg = QDateTime::currentDateTime().toTime_t();
    QVariantList En;

    int numAna = m_mng->GetAnalysiNum();
    for (int i=0; i<numAna; i++)
    {
        QString anaType = m_mng->GetAnalysis(i);
        if (anaType == "Flussimetria")
        {
            //verifica se c'è un definitore per questa analisi.
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

                if (numAna == 1 || posQ > -1)
                {
                    //se non c'è il defintore, ma questa è l'unica analisi,
                    //viene inserito automaticamente sul canale del flusso.
                    for (int i=0;i<m_mng->GetChanNum();i++)
                        En << 0;

                    En[posQ] = 1;
                    if (posV > -1)
                        En[posV] = 1;

                    //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'è

                    //provo a disegnare il definitore

                    VarMap *def=new VarMap;
                    (*def)["key"] = MK_FLOWMETRY;
                    (*def)["xMin"]= 1;
                    (*def)["xMax"]= m_end*1000;
                    (*def)["yMin"]=0;
                    (*def)["yMax"]=100;
                    (*def)["enCh"]= En;
                    (*def)["descr"]="FLW";
                    (*def)["color"]="green";
                    (*def)["category"]=CAT_DEFINER;
                    (*def)["resizeable"]=1;

                    QString family="Definers";
                    QString name="FLW";
                    qDebug()<<*def;
                    VarMapVec *defVec=new VarMapVec;
                    defVec->append(def);
                    saveDataAndUpdate(family,name,defVec,APPEND);
                    mkOpAnIn = *def;
                    found = true;

                    //saveall?? se si la updateinfolist puo leggere da datafile?
                   // saveChanges();//(?)
                    updateInfoList();
                    emit reloadingCompleted();

                }
//                else
//                {
//                    //cotrolliamo se c'è il marker 'ff' free flow
//                    //nel qual caso, il definitore viene inserito da lì fino a:
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

//                                //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'è


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
                if (m_autoPrint)
                        setValVolRes(-1);
                else {
                    setValVolRes(0);//in futuro sarà letto da proprietà xml
                    emit sg_openVolResDlg("Flowmetry");
                }
            }
        }
    }

}

void MDataManager::setValVolRes(int __val)
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

void MDataManager::volRelDlgOk(QString __anaType)
{
    //                                            myGraphPlot.FreeQMax = formSelAna.FreeQMax
    //                                            UpdateTestOther()

    //Lancia analisi e Inizializza nomogrammi
    InitPageGraphs(__anaType);
    //                                            InitPageGraphs(TypeAnalysis, DescAna)

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
    //                                            'su DlgReport il messaggio d'errore anzichè il risultato.
    //                                            'TODO: implementare un sistema per ottenere il numero di
    //                                            'analisi dello stesso tipo effettuate.
    //                                            'MarkerOpAn(mkOpAnIn).Warn = True
    //                                            WaitForDone = 2
    //                                        End If

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
               evEnd = curMap->value("xMax").toInt();
               evMarkOpIn = *curMap;
               evAuto = 1;

               //ogni volta che si passano i definitori alla libreria di analisi dopo aver nascosto/rivisualizzato i canali
               //è necessario sistemare l'array dei canali abilitati considerando tutti i canali dell'analisi.
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
        //                    If Not InitArraysFLW(evStart, evEnd, ChEn, evMarkOpIn, evAuto) Then
        //                        MessageBox.Show(DescAnalysis + RMLoc.GetString("VERIFYANMARKER"), RMLoc.GetString("ERRORE"), MessageBoxButtons.OK, MessageBoxIcon.Error)
        //                        Exit Sub
        //                    End If
        //                    Dim tempPage As New TabPage
        //                    tempPage.Text = RMLoc.GetString("FLOWMETRY")

        //                    'Tabella risultati
        //                    TabRisFlw = New DataGridView()
        //                    TabRisFlw.Location = New Point(6, 6)
        //                    TabRisFlw.BorderStyle = BorderStyle.None
        //                    TabRisFlw.AllowUserToAddRows = False
        //                    TabRisFlw.AllowUserToDeleteRows = False
        //                    TabRisFlw.AllowUserToOrderColumns = False
        //                    TabRisFlw.AllowUserToResizeColumns = False
        //                    TabRisFlw.AllowUserToResizeRows = False
        //                    TabRisFlw.ReadOnly = True
        //                    TabRisFlw.SelectionMode = DataGridViewSelectionMode.RowHeaderSelect
        //                    TabRisFlw.MultiSelect = False
        //                    TabRisFlw.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None
        //                    TabRisFlw.AllowUserToResizeColumns = False
        //                    TabRisFlw.AllowUserToResizeRows = False
        //                    TabRisFlw.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.AutoSizeToAllHeaders
        //                    TabRisFlw.BackgroundColor = Color.Silver
        //                    TabRisFlw.RowHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single
        //                    TabRisFlw.RowHeadersDefaultCellStyle.BackColor = Color.LightSkyBlue
        //                    TabRisFlw.ColumnHeadersVisible = False
        //                    TabRisFlw.Columns.Add("valore", "valore")
        //                    TabRisFlw.Columns.Item(0).Width = 40

        //                    TabRisFlw.Rows.Insert(0, Format(FLWAdvReport(1).waiting_time, "##0.0"))
        //                    TabRisFlw.Rows.Item(0).HeaderCell.Value = RMLoc.GetString("WAITTIME") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(1, Format(FLWAdvReport(1).Q_MAX, "##0.0"))
        //                    TabRisFlw.Rows.Item(1).HeaderCell.Value = RMLoc.GetString("MAXFLOW") & RMLoc.GetString("MISMLSEC")
        //                    TabRisFlw.Rows.Insert(2, Format(FLWAdvReport(1).q_ave, "##0.0"))
        //                    TabRisFlw.Rows.Item(2).HeaderCell.Value = RMLoc.GetString("AVEFLOW") & RMLoc.GetString("MISMLSEC")
        //                    TabRisFlw.Rows.Insert(3, Format(FLWAdvReport(1).time_at_qmax, "##0.0"))
        //                    TabRisFlw.Rows.Item(3).HeaderCell.Value = RMLoc.GetString("TMAXFLOW") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(4, Format(FLWAdvReport(1).time_90, "##0.0"))
        //                    TabRisFlw.Rows.Item(4).HeaderCell.Value = RMLoc.GetString("TIME5_95") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(5, Format(FLWAdvReport(1).flow_time, "##0.0"))
        //                    TabRisFlw.Rows.Item(5).HeaderCell.Value = RMLoc.GetString("FLOWTIME") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(6, Format(FLWAdvReport(1).desc_time, "##0.0"))
        //                    TabRisFlw.Rows.Item(6).HeaderCell.Value = RMLoc.GetString("DESCTIME") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(7, Format(FLWAdvReport(1).voiding_time, "##0.0"))
        //                    TabRisFlw.Rows.Item(7).HeaderCell.Value = RMLoc.GetString("VOIDINGTIME") & RMLoc.GetString("MISSEC")
        //                    TabRisFlw.Rows.Insert(8, Format(FLWAdvReport(1).vol_at_qmax, "0"))
        //                    TabRisFlw.Rows.Item(8).HeaderCell.Value = RMLoc.GetString("MAXVOL") & RMLoc.GetString("MISML")
        //                    TabRisFlw.Rows.Insert(9, Format(FLWAdvReport(1).voided_volume, "0"))
        //                    TabRisFlw.Rows.Item(9).HeaderCell.Value = RMLoc.GetString("VOIDVOL") & RMLoc.GetString("MISML")
        //                    TabRisFlw.Rows.Insert(10, Format(FLWAdvReport(1).cQ, "##0.0"))
        //                    TabRisFlw.Rows.Item(10).HeaderCell.Value = RMLoc.GetString("MAXFLOWCR") & RMLoc.GetString("MISML12SEC")
        //                    TabRisFlw.Rows.Insert(11, Format(FLWAdvReport(1).acceleration, "##0.0"))
        //                    TabRisFlw.Rows.Item(11).HeaderCell.Value = RMLoc.GetString("ACCELFLOW") & RMLoc.GetString("MISMLSEC2")
        //                    TabRisFlw.Rows.Insert(12, Format(FLWAdvReport(1).v_det_max, "##0.0"))
        //                    TabRisFlw.Rows.Item(12).HeaderCell.Value = RMLoc.GetString("MAXCONTRSPEED") & RMLoc.GetString("MISMMSEC")
        //                    If FLWAdvReport(1).residual_volume = -999 Then
        //                        TabRisFlw.Rows.Insert(13, Format(FLWAdvReport(1).residual_volume, "-"))
        //                    Else
        //                        TabRisFlw.Rows.Insert(13, Format(FLWAdvReport(1).residual_volume, "0"))
        //                    End If
        //                    TabRisFlw.Rows.Item(13).HeaderCell.Value = RMLoc.GetString("RESVOL") & RMLoc.GetString("MISML")

        //                    TabRisFlw.Size = New Size(314, 310)

        //                    'creo i bottoni da mettere nella barra dei nomogrammi
        //                    Dim Bt_Liverpool_Max As ToolStripButton = New ToolStripButton()
        //                    Bt_Liverpool_Max.Text() = RMLoc.GetString("LIVERPOOLQMAX")
        //                    Bt_Liverpool_Max.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Liverpool_Max.Click, AddressOf Bt_NomoFLW_Click
        //                    Dim Bt_Liverpool_Ave As ToolStripButton = New ToolStripButton()
        //                    Bt_Liverpool_Ave.Text() = RMLoc.GetString("LIVERPOOLQAVE")
        //                    Bt_Liverpool_Ave.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Liverpool_Ave.Click, AddressOf Bt_NomoFLW_Click
        //                    Dim Bt_Siroky_Ave As ToolStripButton = New ToolStripButton()
        //                    Bt_Siroky_Ave.Text() = RMLoc.GetString("SIROKYAVE")
        //                    Bt_Siroky_Ave.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Siroky_Ave.Click, AddressOf Bt_NomoFLW_Click
        //                    Dim Bt_Siroky_Max As ToolStripButton = New ToolStripButton()
        //                    Bt_Siroky_Max.Text() = RMLoc.GetString("SIROKYMAX")
        //                    Bt_Siroky_Max.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Siroky_Max.Click, AddressOf Bt_NomoFLW_Click
        //                    Dim Bt_Miskloc_Max As ToolStripButton = New ToolStripButton()
        //                    Bt_Miskloc_Max.Text() = RMLoc.GetString("MISKOLCQMAX")
        //                    Bt_Miskloc_Max.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Miskloc_Max.Click, AddressOf Bt_NomoFLW_Click
        //                    Dim Bt_Miskloc_Ave As ToolStripButton = New ToolStripButton()
        //                    Bt_Miskloc_Ave.Text() = RMLoc.GetString("MISKOLCQAVE")
        //                    Bt_Miskloc_Ave.Size = New Size(WBUTTON, HBUTTON)
        //                    AddHandler Bt_Miskloc_Ave.Click, AddressOf Bt_NomoFLW_Click

        //                    'creo la barra
        //                    BrNomogrammi = New ToolStrip()
        //                    BrNomogrammi.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional
        //                    BrNomogrammi.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        //                    BrNomogrammi.Font = New System.Drawing.Font("Arial", 9.0, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        //                    BrNomogrammi.Dock = System.Windows.Forms.DockStyle.None
        //                    BrNomogrammi.BackColor = Color.LightGray
        //                    BrNomogrammi.AutoSize = True

        //                    BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                    BrNomogrammi.Items.Add(Bt_Liverpool_Max)
        //                    BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                    BrNomogrammi.Items.Add(Bt_Liverpool_Ave)
        //                    BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                    If Not TestDescription.Sex Then
        //                        BrNomogrammi.Items.Add(Bt_Siroky_Ave)
        //                        BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                        BrNomogrammi.Items.Add(Bt_Siroky_Max)
        //                        BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                    End If
        //                    If TestDescription.Age >= 3 And TestDescription.Age <= 18 And TestDescription.Peso <> 0 And TestDescription.Altezza <> 0 Then
        //                        BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                        BrNomogrammi.Items.Add(Bt_Miskloc_Max)
        //                        'BrNomogrammi.Items.Add(New ToolStripSeparator)
        //                        'BrNomogrammi.Items.Add(Bt_Miskloc_Ave)
        //                    End If

        //                    'aggiungo gli elementi al tab
        //                    tempPage.BackColor = Color.Silver
        //                    tempPage.Controls.Add(TabRisFlw)
        //                    tempPage.Controls.Add(N_Liverpool_Max(NumAnalysis))
        //                    tempPage.Controls.Add(N_Liverpool_Ave(NumAnalysis))
        //                    If Not TestDescription.Sex Then
        //                        tempPage.Controls.Add(N_Siroky_Ave(NumAnalysis))
        //                        tempPage.Controls.Add(N_Siroky_Max(NumAnalysis))
        //                    End If
        //                    If TestDescription.Age >= 3 And TestDescription.Age <= 18 And TestDescription.Peso <> 0 And TestDescription.Altezza <> 0 Then
        //                        tempPage.Controls.Add(N_Miskolc_Max(NumAnalysis))
        //                        'tempPage.Controls.Add(N_Miskolc_Ave(NumAnalysis))
        //                    End If
        //                    tempPage.Controls.Add(BrNomogrammi)
        //                    RisAnaTab.TabPages.Add(tempPage)


    }

}

bool MDataManager::InitArraysFLW(int __start,
                                 int __end,
                                 QVector<unsigned char> __chEn,
                                 int __curDef,
                                 byte __auto)
{
    //da leggere da file

    //           Dim parList As New PrjReviewConfig.ReviewConfig
    //           parList.FileName = configPath + "\ParAna.xml"
    //           parList.Load()
    //           Dim Sth As Double, Ath As Double, Lth As Double
    //           Dim b As Boolean = parList.GetTh(102, "Q", Sth, Ath, Lth)
    double startTh = 0;
    double heightTh = 2;
    double widthTth = 0;

    int res = m_ana->FLW_Adv_Analysis_Time(1,__chEn,__start,__end,__curDef,startTh,heightTh,widthTth,__auto,getValVolRes(), 0);
    if (res < 0)
        return false;


    //Legge i risultati
    int ris = ReadResult();

}

//    Private Function InitArraysFLW(ByVal start_time As Int32, ByVal end_time As Int32, ByVal ChEn() As Byte, ByVal MkId As Int32, ByVal Auto As Byte) As Boolean


//           'Legge i risultati
//           Dim StructType As Integer
//           StructType = ReadResults(FileGraphName, 1)

//           ReDim N_Liverpool_Max(MAX_ANALYSIS)
//           N_Liverpool_Max(NumAnalysis) = New Nomogramma(FLW_ADV_STUDY, G_LIVERPOOL_MAX, 310)
//           ReDim N_Liverpool_Ave(MAX_ANALYSIS)
//           N_Liverpool_Ave(NumAnalysis) = New Nomogramma(FLW_ADV_STUDY, G_LIVERPOOL_AVE, 310)

//           If Not TestDescription.Sex Then
//               ReDim N_Siroky_Ave(MAX_ANALYSIS)
//               N_Siroky_Ave(NumAnalysis) = New Nomogramma(FLW_ADV_STUDY, G_FLW_Siroky_Ave, 310)
//               ReDim N_Siroky_Max(MAX_ANALYSIS)
//               N_Siroky_Max(NumAnalysis) = New Nomogramma(FLW_ADV_STUDY, G_FLW_Siroky_Max, 310)
//           End If

//           N_Liverpool_Max(NumAnalysis).FontSize = FontSel.LabelAxisFontSize
//           N_Liverpool_Max(NumAnalysis).FontName = FontSel.FontName
//           N_Liverpool_Max(NumAnalysis).PrintFontSize = PRINT_FONT_SIZE
//           N_Liverpool_Max(NumAnalysis).strX = RMLoc.GetString("VOIDVOL2")
//           N_Liverpool_Max(NumAnalysis).strY = RMLoc.GetString("QMAX")
//           N_Liverpool_Max(NumAnalysis).Name = RMLoc.GetString("LIVERPOOLQMAX")
//           N_Liverpool_Max(NumAnalysis).flNotCursor = True

//           N_Liverpool_Ave(NumAnalysis).FontSize = FontSel.LabelAxisFontSize
//           N_Liverpool_Ave(NumAnalysis).PrintFontSize = PRINT_FONT_SIZE
//           N_Liverpool_Ave(NumAnalysis).FontName = FontSel.FontName
//           N_Liverpool_Ave(NumAnalysis).strX = RMLoc.GetString("VOIDVOL2")
//           N_Liverpool_Ave(NumAnalysis).strY = RMLoc.GetString("QMAX")
//           N_Liverpool_Ave(NumAnalysis).Name = RMLoc.GetString("LIVERPOOLQAVE")
//           N_Liverpool_Ave(NumAnalysis).flNotCursor = True


//           Const N_LIVERPOOL = 24
//           Const N_LIVERPOOL_CENT = 5
//           Dim Step_Liverpool As Long
//           Step_Liverpool = 600 / N_LIVERPOOL
//           N_Liverpool_Max(NumAnalysis).nPoints1 = N_LIVERPOOL - 1
//           N_Liverpool_Max(NumAnalysis).nPoints2 = N_LIVERPOOL_CENT
//           N_Liverpool_Max(NumAnalysis).nPoints3 = N_LIVERPOOL_CENT
//           N_Liverpool_Max(NumAnalysis).nPoints4 = N_LIVERPOOL_CENT
//           N_Liverpool_Max(NumAnalysis).nPoints5 = N_LIVERPOOL_CENT
//           N_Liverpool_Max(NumAnalysis).nPoints6 = N_LIVERPOOL_CENT
//           N_Liverpool_Max(NumAnalysis).nPoints7 = N_LIVERPOOL_CENT
//           ReDim N_Liverpool_Max(NumAnalysis).Linex1(N_LIVERPOOL - 1)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney1(N_LIVERPOOL - 1)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex2(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney2(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex3(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney3(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex4(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney4(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex5(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney5(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex6(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney6(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Linex7(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Max(NumAnalysis).Liney7(N_LIVERPOOL_CENT)

//           N_Liverpool_Ave(NumAnalysis).nPoints1 = N_LIVERPOOL - 1
//           N_Liverpool_Ave(NumAnalysis).nPoints2 = N_LIVERPOOL_CENT
//           N_Liverpool_Ave(NumAnalysis).nPoints3 = N_LIVERPOOL_CENT
//           N_Liverpool_Ave(NumAnalysis).nPoints4 = N_LIVERPOOL_CENT
//           N_Liverpool_Ave(NumAnalysis).nPoints5 = N_LIVERPOOL_CENT
//           N_Liverpool_Ave(NumAnalysis).nPoints6 = N_LIVERPOOL_CENT
//           N_Liverpool_Ave(NumAnalysis).nPoints7 = N_LIVERPOOL_CENT
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex1(N_LIVERPOOL - 1)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney1(N_LIVERPOOL - 1)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex2(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney2(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex3(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney3(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex4(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney4(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex5(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney5(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex6(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney6(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Linex7(N_LIVERPOOL_CENT)
//           ReDim N_Liverpool_Ave(NumAnalysis).Liney7(N_LIVERPOOL_CENT)

//           For i = 1 To N_LIVERPOOL
//               If Not TestDescription.Sex Then
//                   If TestDescription.Age < 50 Then
//                       'Liverpool uomini età < 50
//                       N_Liverpool_Max(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                       N_Liverpool_Max(NumAnalysis).Liney1(i - 1) = (2.37 + 0.18 * System.Math.Sqrt(i * Step_Liverpool) - 0.014 * 35) ^ 2
//                       N_Liverpool_Ave(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                       N_Liverpool_Ave(NumAnalysis).Liney1(i - 1) = (1.8 + 0.14 * System.Math.Sqrt(i * Step_Liverpool) - 0.011 * 35) ^ 2
//                   Else
//                       'Liverpool uomini >= 50
//                       N_Liverpool_Max(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                       N_Liverpool_Max(NumAnalysis).Liney1(i - 1) = (2.37 + 0.18 * System.Math.Sqrt(i * Step_Liverpool) - 0.014 * 60) ^ 2
//                       N_Liverpool_Ave(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                       N_Liverpool_Ave(NumAnalysis).Liney1(i - 1) = (1.8 + 0.14 * System.Math.Sqrt(i * Step_Liverpool) - 0.011 * 60) ^ 2
//                   End If
//               Else
//                   'Liverpool donne
//                   N_Liverpool_Max(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                   N_Liverpool_Max(NumAnalysis).Liney1(i - 1) = 2.718282 ^ (0.511 + 0.505 * System.Math.Log(i * Step_Liverpool)) '(2.37 + 0.18 * Sqr(i * Step_Liverpool) - 0.014 * 35) ^ 2)
//                   N_Liverpool_Ave(NumAnalysis).Linex1(i - 1) = i * Step_Liverpool
//                   N_Liverpool_Ave(NumAnalysis).Liney1(i - 1) = (-0.921 + 0.869 * System.Math.Log(i * Step_Liverpool)) ^ 2
//               End If
//           Next i

//           Dim LUtil = New ParamSiroky()

//           If Not TestDescription.Sex Then
//               LUtil.ReadLiverpoolParameter(True, False, TestDescription.Age)
//               For i = 0 To N_LIVERPOOL_CENT
//                   N_Liverpool_Max(NumAnalysis).Linex2(i) = LUtil.ArrLineX1(i)
//                   N_Liverpool_Max(NumAnalysis).Liney2(i) = LUtil.ArrLineY1(i)
//                   N_Liverpool_Max(NumAnalysis).Linex3(i) = LUtil.ArrLineX2(i)
//                   N_Liverpool_Max(NumAnalysis).Liney3(i) = LUtil.ArrLineY2(i)
//                   N_Liverpool_Max(NumAnalysis).Linex4(i) = LUtil.ArrLineX3(i)
//                   N_Liverpool_Max(NumAnalysis).Liney4(i) = LUtil.ArrLineY3(i)
//                   N_Liverpool_Max(NumAnalysis).Linex5(i) = LUtil.ArrLineX4(i)
//                   N_Liverpool_Max(NumAnalysis).Liney5(i) = LUtil.ArrLineY4(i)
//                   N_Liverpool_Max(NumAnalysis).Linex6(i) = LUtil.ArrLineX5(i)
//                   N_Liverpool_Max(NumAnalysis).Liney6(i) = LUtil.ArrLineY5(i)
//                   N_Liverpool_Max(NumAnalysis).Linex7(i) = LUtil.ArrLineX6(i)
//                   N_Liverpool_Max(NumAnalysis).Liney7(i) = LUtil.ArrLineY6(i)
//               Next i
//               LUtil.ReadLiverpoolParameter(False, False, TestDescription.Age)
//               For i = 0 To N_LIVERPOOL_CENT
//                   N_Liverpool_Ave(NumAnalysis).Linex2(i) = LUtil.ArrLineX1(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney2(i) = LUtil.ArrLineY1(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex3(i) = LUtil.ArrLineX2(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney3(i) = LUtil.ArrLineY2(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex4(i) = LUtil.ArrLineX3(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney4(i) = LUtil.ArrLineY3(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex5(i) = LUtil.ArrLineX4(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney5(i) = LUtil.ArrLineY4(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex6(i) = LUtil.ArrLineX5(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney6(i) = LUtil.ArrLineY5(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex7(i) = LUtil.ArrLineX6(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney7(i) = LUtil.ArrLineY6(i)
//               Next i
//           Else
//               LUtil.ReadLiverpoolParameter(True, True, 0)
//               For i = 0 To N_LIVERPOOL_CENT
//                   N_Liverpool_Max(NumAnalysis).Linex2(i) = LUtil.ArrLineX1(i)
//                   N_Liverpool_Max(NumAnalysis).Liney2(i) = LUtil.ArrLineY1(i)
//                   N_Liverpool_Max(NumAnalysis).Linex3(i) = LUtil.ArrLineX2(i)
//                   N_Liverpool_Max(NumAnalysis).Liney3(i) = LUtil.ArrLineY2(i)
//                   N_Liverpool_Max(NumAnalysis).Linex4(i) = LUtil.ArrLineX3(i)
//                   N_Liverpool_Max(NumAnalysis).Liney4(i) = LUtil.ArrLineY3(i)
//                   N_Liverpool_Max(NumAnalysis).Linex5(i) = LUtil.ArrLineX4(i)
//                   N_Liverpool_Max(NumAnalysis).Liney5(i) = LUtil.ArrLineY4(i)
//                   N_Liverpool_Max(NumAnalysis).Linex6(i) = LUtil.ArrLineX5(i)
//                   N_Liverpool_Max(NumAnalysis).Liney6(i) = LUtil.ArrLineY5(i)
//                   N_Liverpool_Max(NumAnalysis).Linex7(i) = LUtil.ArrLineX6(i)
//                   N_Liverpool_Max(NumAnalysis).Liney7(i) = LUtil.ArrLineY6(i)
//               Next i
//               LUtil.ReadLiverpoolParameter(False, True, 0)
//               For i = 0 To N_LIVERPOOL_CENT
//                   N_Liverpool_Ave(NumAnalysis).Linex2(i) = LUtil.ArrLineX1(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney2(i) = LUtil.ArrLineY1(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex3(i) = LUtil.ArrLineX2(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney3(i) = LUtil.ArrLineY2(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex4(i) = LUtil.ArrLineX3(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney4(i) = LUtil.ArrLineY3(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex5(i) = LUtil.ArrLineX4(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney5(i) = LUtil.ArrLineY4(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex6(i) = LUtil.ArrLineX5(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney6(i) = LUtil.ArrLineY5(i)
//                   N_Liverpool_Ave(NumAnalysis).Linex7(i) = LUtil.ArrLineX6(i)
//                   N_Liverpool_Ave(NumAnalysis).Liney7(i) = LUtil.ArrLineY6(i)
//               Next i
//           End If

//           N_Liverpool_Max(NumAnalysis).Unit = RMLoc.GetString("Q") & RMLoc.GetString("MISMLSEC")
//           N_Liverpool_Max(NumAnalysis).TitleAxixX = RMLoc.GetString("VOL") & RMLoc.GetString("MISML")
//           N_Liverpool_Max(NumAnalysis).flGrRight = False
//           N_Liverpool_Max(NumAnalysis).rxMin = 0
//           N_Liverpool_Max(NumAnalysis).ryMin = 0
//           N_Liverpool_Max(NumAnalysis).rxMax = 600
//           If TestDescription.Sex Then
//               N_Liverpool_Max(NumAnalysis).ryMax = 60
//           Else
//               N_Liverpool_Max(NumAnalysis).ryMax = 80 '75 '70 da chiedere a Davide
//           End If

//           N_Liverpool_Ave(NumAnalysis).Unit = RMLoc.GetString("Q") & RMLoc.GetString("MISMLSEC")
//           N_Liverpool_Ave(NumAnalysis).TitleAxixX = RMLoc.GetString("VOL") & RMLoc.GetString("MISML")
//           N_Liverpool_Ave(NumAnalysis).flGrRight = False
//           N_Liverpool_Ave(NumAnalysis).rxMax = 600
//           N_Liverpool_Ave(NumAnalysis).ryMax = 40

//            N_Liverpool_Max(NumAnalysis).DatoAveX = FLWAdvReport(NumAnalysis).voided_volume
//                   N_Liverpool_Max(NumAnalysis).DatoMaxY = FLWAdvReport(NumAnalysis).Q_MAX
//                   N_Liverpool_Ave(NumAnalysis).DatoAveX = FLWAdvReport(NumAnalysis).voided_volume
//                   N_Liverpool_Ave(NumAnalysis).DatoAveY = FLWAdvReport(NumAnalysis).q_ave




typedef struct {
    float waiting_time;			// waiting time
    float q_max;					// maximum flow
    float q_ave;					// average flow
    float time_at_v3;             // time at Vol/3
    float time_at_v2;             // time at Vol/2
    float time_at_qmax;			// time at Q max
    float time_90;				// time between 5% and 95% of the voided volume
    float flow_time;				// flow time
    float desc_time;				// time between Q max and 95% of the voided volume
    float voiding_time;			// voiding time
    float vol_at_qmax;			// volume at Q max
    long voided_volume;          // voided volume
    float acceleration;           // Q max / T qmax
    long residual_volume;		// residual volume inserted by the user
    float v_det_max;              // detrusor contraction maximum speed
    float cQ;						// Flow corrective factor

} FLWAdvRepStruct;


enum {
    WAITING_TIME = 0,
    MAXIMUM_FLOW = 4,
    AVERAGE_FLOW = 8,
    TIME_AT_VOL3 = 12,
    TIME_AT_VOL2 = 16,
    TIME_AT_QMAX = 20,
    TIME_5_95_VOIDED_VOL = 24,
    FLOW_TIME  = 28,
    TIME_QMAX_95_VOIDED_VOL = 32,
    VOIDING_TIME = 36,
    VOLUME_QMAX = 40,
    VOIDED_VOLUME = 44,
    ACCELERATION = 48,
    RESIDUAL_USER = 52,
    DETRUSOR = 56,
    FLOW_CORR_FACTOR = 60
}  ;

int MDataManager::ReadResult(int __numEv)
{
    int StructType = m_mng->GetAnaType();
    if (StructType <= 0)
        //error
        return -1;

    __numEv = m_mng->readNumEv();

    FLWAdvRepStruct FLWAdvReport[__numEv+1];
   // byte* strTemp = new byte(sizeof(FLWAdvReport[0]));
    byte* strTemp = (byte*) malloc (sizeof(FLWAdvReport[0]));

    switch (StructType) {
    case FLW_AVD_STUDY: {
                //dati analisi
        FLWAdvReport[0].waiting_time = 0;
        FLWAdvReport[0].q_max = 0;
        FLWAdvReport[0].q_ave = 0;
        FLWAdvReport[0].time_at_v3 = 0;
        FLWAdvReport[0].time_at_v2 = 0;
        FLWAdvReport[0].time_90 = 0;
        FLWAdvReport[0].flow_time = 0;
        FLWAdvReport[0].desc_time = 0;
        FLWAdvReport[0].voiding_time = 0;
        FLWAdvReport[0].vol_at_qmax = 0;
        FLWAdvReport[0].voided_volume = 0;
        FLWAdvReport[0].acceleration = 0;
        FLWAdvReport[0].residual_volume = 0;
        FLWAdvReport[0].v_det_max = 0;
        FLWAdvReport[0].cQ = 0;

        for (int i=1; i<=__numEv; i++)
        {
            m_mng->readResAna(sizeof(FLWAdvReport[0]),strTemp);
            FLWAdvReport[i].waiting_time = *((float*)(strTemp + WAITING_TIME));
            FLWAdvReport[i].q_max = *((float*)(strTemp + MAXIMUM_FLOW));
            FLWAdvReport[i].q_ave = *((float*)(strTemp + AVERAGE_FLOW));
            FLWAdvReport[i].time_at_v3 = *((float*)(strTemp + TIME_AT_VOL3));
            FLWAdvReport[i].time_at_v2 = *((float*)(strTemp + TIME_AT_VOL2));
            FLWAdvReport[i].time_at_qmax = *((float*)(strTemp + TIME_AT_QMAX));
            FLWAdvReport[i].time_90 = *((float*)(strTemp + TIME_5_95_VOIDED_VOL));
            FLWAdvReport[i].flow_time = *((float*)(strTemp + FLOW_TIME));
            FLWAdvReport[i].desc_time = *((float*)(strTemp + TIME_QMAX_95_VOIDED_VOL));
            FLWAdvReport[i].voiding_time = *((float*)(strTemp + VOIDING_TIME));
            FLWAdvReport[i].vol_at_qmax = *((float*)(strTemp + VOLUME_QMAX));
            FLWAdvReport[i].voided_volume = *((float*)(strTemp + VOIDED_VOLUME));
            FLWAdvReport[i].acceleration = *((float*)(strTemp + ACCELERATION));
            FLWAdvReport[i].residual_volume = *((float*)(strTemp + RESIDUAL_USER));
            FLWAdvReport[i].v_det_max = *((float*)(strTemp + DETRUSOR));
            FLWAdvReport[i].cQ = *((float*)(strTemp +FLOW_CORR_FACTOR));
        }

        //calcolo la media
        for (int i=1; i<=__numEv; i++)
        {
            FLWAdvReport[0].waiting_time += FLWAdvReport[i].waiting_time;
            FLWAdvReport[0].q_max += FLWAdvReport[i].q_max;
            FLWAdvReport[0].q_ave += FLWAdvReport[i].q_ave;
            FLWAdvReport[0].time_at_v3 += FLWAdvReport[i].time_at_v3;
            FLWAdvReport[0].time_at_v2 += FLWAdvReport[i].time_at_v2;
            FLWAdvReport[0].time_at_qmax += FLWAdvReport[i].time_at_qmax;
            FLWAdvReport[0].time_90 += FLWAdvReport[i].time_90;
            FLWAdvReport[0].flow_time += FLWAdvReport[i].flow_time;
            FLWAdvReport[0].desc_time += FLWAdvReport[i].desc_time;
            FLWAdvReport[0].voiding_time += FLWAdvReport[i].voiding_time;
            FLWAdvReport[0].vol_at_qmax += FLWAdvReport[i].vol_at_qmax;
            FLWAdvReport[0].voided_volume += FLWAdvReport[i].voided_volume;
            FLWAdvReport[0].acceleration += FLWAdvReport[i].acceleration;
            FLWAdvReport[0].residual_volume += FLWAdvReport[i].residual_volume;
            FLWAdvReport[0].v_det_max += FLWAdvReport[i].v_det_max;
            FLWAdvReport[0].cQ += FLWAdvReport[i].cQ;
        }

        FLWAdvReport[0].waiting_time /= __numEv;
        FLWAdvReport[0].q_max /= __numEv;
        FLWAdvReport[0].q_ave /= __numEv;
        FLWAdvReport[0].time_at_v3 /= __numEv;
        FLWAdvReport[0].time_at_v2 /= __numEv;
        FLWAdvReport[0].time_at_qmax /= __numEv;
        FLWAdvReport[0].time_90 /= __numEv;
        FLWAdvReport[0].flow_time /= __numEv;
        FLWAdvReport[0].desc_time /= __numEv;
        FLWAdvReport[0].voiding_time /= __numEv;
        FLWAdvReport[0].vol_at_qmax /= __numEv;
        FLWAdvReport[0].voided_volume /= __numEv;
        FLWAdvReport[0].acceleration /= __numEv;
        FLWAdvReport[0].residual_volume /= __numEv;
        FLWAdvReport[0].v_det_max /= __numEv;
        FLWAdvReport[0].cQ /= __numEv;

        break;
    }
    default:
        break;
    }

    free(strTemp);
    return StructType;

}



//Dim StructType As Integer = -1
//Dim strTemp() As Byte

//Dim pEvAve As Integer
//Dim pUraAve As Integer

//Dim fileR As New wrapperReadAnalysis(FileGraphName)
//Dim b As Boolean = fileR.Open()
//If (Not b) Then
//    MessageBox.Show(RMLoc.GetString("ERRORFILEREAD"), RMLoc.GetString("ERROR"), MessageBoxButtons.OK, MessageBoxIcon.Error)
//    Return -1
//End If

//StructType = fileR.getAnaType()
//If StructType <= 0 Then
//    fileR.Close()
//    MessageBox.Show(RMLoc.GetString("ERRORFILEREAD"), RMLoc.GetString("ERROR"), MessageBoxButtons.OK, MessageBoxIcon.Error)
//    Return -1
//End If
//fileR.readNumEv(NumEv)

//Select Case StructType
//    Case FLW_ADV_STUDY
//        'media
//        FLWAdvReport(0).acceleration = 0
//        FLWAdvReport(0).cQ = 0
//        FLWAdvReport(0).desc_time = 0
//        FLWAdvReport(0).flow_time = 0
//        FLWAdvReport(0).q_ave = 0
//        FLWAdvReport(0).Q_MAX = 0
//        FLWAdvReport(0).residual_volume = 0
//        FLWAdvReport(0).time_90 = 0
//        FLWAdvReport(0).time_at_qmax = 0
//        FLWAdvReport(0).time_at_v2 = 0
//        FLWAdvReport(0).time_at_v3 = 0
//        FLWAdvReport(0).v_det_max = 0
//        FLWAdvReport(0).voided_volume = 0
//        FLWAdvReport(0).voiding_time = 0
//        FLWAdvReport(0).vol_at_qmax = 0
//        FLWAdvReport(0).waiting_time = 0
//        'dati analisi
//        For i As Integer = 1 To NumEv
//            strTemp = Nothing
//            fileR.readResAna(Len(FLWAdvReport(0)) - 1, strTemp)
//            FLWAdvReport(i).waiting_time = BitConverter.ToSingle(strTemp, WAITING_TIME)
//            FLWAdvReport(i).Q_MAX = BitConverter.ToSingle(strTemp, MAXIMUM_FLOW)
//            FLWAdvReport(i).q_ave = BitConverter.ToSingle(strTemp, AVERAGE_FLOW)
//            FLWAdvReport(i).time_at_v3 = BitConverter.ToSingle(strTemp, TIME_AT_VOL3)
//            FLWAdvReport(i).time_at_v2 = BitConverter.ToSingle(strTemp, TIME_AT_VOL2)
//            FLWAdvReport(i).time_at_qmax = BitConverter.ToSingle(strTemp, TIME_AT_QMAX)
//            FLWAdvReport(i).time_90 = BitConverter.ToSingle(strTemp, TIME_5_95_VOIDED_VOL)
//            FLWAdvReport(i).flow_time = BitConverter.ToSingle(strTemp, FLOW_TIME)
//            FLWAdvReport(i).desc_time = BitConverter.ToSingle(strTemp, TIME_QMAX_95_VOIDED_VOL)
//            FLWAdvReport(i).voiding_time = BitConverter.ToSingle(strTemp, VOIDING_TIME)
//            FLWAdvReport(i).vol_at_qmax = BitConverter.ToSingle(strTemp, VOLUME_QMAX)
//            FLWAdvReport(i).voided_volume = BitConverter.ToInt32(strTemp, VOIDED_VOLUME)
//            FLWAdvReport(i).acceleration = BitConverter.ToSingle(strTemp, ACCELERATION)
//            FLWAdvReport(i).residual_volume = BitConverter.ToInt32(strTemp, RESIDUAL_USER)
//            FLWAdvReport(i).v_det_max = BitConverter.ToSingle(strTemp, DETRUSOR)
//            FLWAdvReport(i).cQ = BitConverter.ToSingle(strTemp, FLOW_CORR_FACTOR)
//        Next

//        'calcolo la media
//        For i = 1 To NumEv
//            FLWAdvReport(0).waiting_time += FLWAdvReport(i).waiting_time
//            FLWAdvReport(0).Q_MAX += FLWAdvReport(i).Q_MAX
//            FLWAdvReport(0).q_ave += FLWAdvReport(i).q_ave
//            FLWAdvReport(0).time_at_v3 += FLWAdvReport(i).time_at_v3
//            FLWAdvReport(0).time_at_v2 += FLWAdvReport(i).time_at_v2
//            FLWAdvReport(0).time_at_qmax += FLWAdvReport(i).time_at_qmax
//            FLWAdvReport(0).time_90 += FLWAdvReport(i).time_90
//            FLWAdvReport(0).flow_time += FLWAdvReport(i).flow_time
//            FLWAdvReport(0).desc_time += FLWAdvReport(i).desc_time
//            FLWAdvReport(0).voiding_time += FLWAdvReport(i).voiding_time
//            FLWAdvReport(0).vol_at_qmax += FLWAdvReport(i).vol_at_qmax
//            FLWAdvReport(0).voided_volume += FLWAdvReport(i).voided_volume
//            FLWAdvReport(0).acceleration += FLWAdvReport(i).acceleration
//            FLWAdvReport(0).residual_volume += FLWAdvReport(i).residual_volume
//            FLWAdvReport(0).v_det_max += FLWAdvReport(i).v_det_max
//            FLWAdvReport(0).cQ += FLWAdvReport(i).cQ
//        Next

//        FLWAdvReport(0).waiting_time /= NumEv
//        FLWAdvReport(0).Q_MAX /= NumEv
//        FLWAdvReport(0).q_ave /= NumEv
//        FLWAdvReport(0).time_at_v3 /= NumEv
//        FLWAdvReport(0).time_at_v2 /= NumEv
//        FLWAdvReport(0).time_at_qmax /= NumEv
//        FLWAdvReport(0).time_90 /= NumEv
//        FLWAdvReport(0).flow_time /= NumEv
//        FLWAdvReport(0).desc_time /= NumEv
//        FLWAdvReport(0).voiding_time /= NumEv
//        FLWAdvReport(0).vol_at_qmax /= NumEv
//        FLWAdvReport(0).voided_volume /= NumEv
//        FLWAdvReport(0).acceleration /= NumEv
//        FLWAdvReport(0).residual_volume /= NumEv
//        FLWAdvReport(0).v_det_max /= NumEv
//        FLWAdvReport(0).cQ /= NumEv

//        End Select

//        fileR.Close()

//        Return StructType
