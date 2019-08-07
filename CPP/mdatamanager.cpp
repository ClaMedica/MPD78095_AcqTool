#include "mdatamanager.h"
#include "systemmanager.h"

extern bool DebugAcqTool;

MDataManager::MDataManager(QObject *parent)
{
    (void) parent;

    m_mng = NULL;
    m_ana = NULL;
    m_copy = NULL;
    m_currentSignalName = "custom_signal";
    m_pCurrentSignal = NULL;
    m_numChannels = 0;

    m_end = 3600;   //fine esame di default a 1 ora

    m_analized = false;
    m_autoPrint = false;
    m_autoFlow = 2;
    m_Siroky = false;
    m_Liverpool = false;
    m_landscape = false;

    m_numAna = 0;
    m_toSave = "ret";

    m_firstHead = "Medica S.p.A - Menfis Divisione";
    m_secondHead = "Pico Flow 2";
    m_etaPatient = -1;

    m_datiCalib = "";

    setValVolRes(-999);
}

MDataManager::~MDataManager()
{
    if(m_mng != NULL) {
        m_mng->Close();
        delete m_mng;
        m_mng = NULL;
    }
    if (m_ana != NULL) {
        delete m_ana;
        m_ana = NULL;
    }
}

void MDataManager::setInfoList(QVariantList __list)
{
    if(__list != m_infoList) {
        m_infoList = __list;
        emit infoListChanged();
    }
}

bool MDataManager::addSignal(MSignal *__pSignal)
{
    if(__pSignal == NULL) {
        qDebug() << "The signal is empty in FunctionHandler::addSignal";
        return false;
    }

    foreach(MSignal *sig, m_signalVector)
        if(sig->getName() == __pSignal->getName()) {
            qDebug() << "File with the same name already exist in the vector";
            return false;
        }

    m_signalVector.append(__pSignal);
    VarMapVec *vec = new VarMapVec;
    VarMap *traccia = new VarMap;
    QString family = __pSignal->getName();
    QString name = "Signal";
    (*traccia)["descr"] = __pSignal->getName();
    (*traccia)["category"] = CAT_TRACK;
    (*traccia)["pointer"] = (qulonglong) __pSignal;
    vec->append(traccia);
    saveDataAndUpdate(family,name,vec);
    emit availableTracksChanged();

    return true;
}

void MDataManager::loadFile(QString __fileName)
{
    __fileName.remove("file:///");
    qDebug() << "Apro il file " << __fileName;

    if( ! QFile::exists(__fileName)) {
        qDebug() << __fileName << MEX_FILE_NOT_EXISTS;
        return;
    }

    m_fileName = __fileName;
    m_pathData = m_fileName.left(m_fileName.lastIndexOf("/")+1);


    //la prima volta che salvo mi faccio la copia del file originale
    m_copyFileName = m_fileName;
    m_copyFileName.insert(m_copyFileName.length() - 4, "_copy");
    //se è rimasta per sbaglio una copia di aperture precedenti del file, la cancelliamo prima di ricrearla
    if (QFile(m_copyFileName).exists()) {
        QFile::remove(m_copyFileName);
    }
    QFile::copy(m_fileName, m_copyFileName);
    m_toSave = "ret";//quando lo apro non devo salvare niente

    //devo pulire tutti i vettori utilizzati
    resetAll();
    //reset del valore di volume residuo diverso per ogni esame
    setValVolRes(-999);

    VarMapVec *mrkOpVec = new VarMapVec;
    VarMapVec *mrkAnVec = new VarMapVec;
    VarMapVec *  defVec = new VarMapVec;
    QMap<int, QVariantList> defEn;

    double sigMin = INF, sigMax = -INF;
    unsigned char key;
    int32_t numCh;
    int32_t numSamp[4]; //4 byte per avere il numero del campione
    int32_t numDef;
    int32_t tStart[20], tEnd[20];
    QString descr;
    unsigned char chEn[20];

    switch(fileType(m_copyFileName)) {
    case PIC:
    {
       if(m_mng != NULL)
            delete m_mng;
        m_mng = new DatafileManager;
        m_mng->SetFileName(m_copyFileName);
        m_mng->SetFileType(7);
        bool res = m_mng->Open();
        qDebug() << "File Aperto?" << res;
        res = m_mng->GetParameters();
        qDebug() << "File Caricato?" << res;

        qDebug() << "Building configuration file ...";
        if(!buildConfigurationFile()) {
            qCritical() << MEX_FILE_CORRUPTED;
            return;
        }

        m_testNumber = m_mng->GetTestNum();
        m_patientInfo = m_mng->GetPatient().section(";",0,1);
        m_patientInfo.replace(";", " ");

        QString dataNascita = m_mng->GetPatient().section(";",2,2);
        QDate datD = QDate::fromString(dataNascita,"dd/MM/yyyy");
        QDate dateExam = QDate(1899, 12, 30).addDays(m_mng->GetDataEsame());
        m_etaPatient = dateExam.year() - datD.year();
        qDebug()<<"Eta' paziente"<< m_etaPatient;

        if (!m_patientInfo.contains("Anonymous"))
        {
            QString dateofexam = dateExam.toString("dd/MM/yyyy");
            m_patientInfo = m_patientInfo + " - " + dateofexam;
        }

        m_sexPatient = false;
        if (m_mng->GetPatient().section(";", 12, 12) == "F")
            m_sexPatient = true;

        //dati calibrazione
        m_datiCalib = "none;";
        QString otherString = m_mng->GetOther();
        QStringList stringSplit = otherString.split(";");
        if (otherString == "")
        {
            qDebug() << "otherstring vuota" << m_datiCalib;
            //c'è qualche problema e manca la calibrazione
            //la inseriamo
            otherString = m_datiCalib;
            m_mng->SetOther(otherString);
            m_mng->CommitParameters();
            qDebug() << "otherstring vuota" << m_mng->GetOther();
        }
        else if (stringSplit.length() >= 2)//qui è tutto ok
        {
            QString first = stringSplit.at(0);
            qDebug() << "first:" << first;
            if (first.contains("none") || first.length() >= 5)
            {
                m_datiCalib = first;
            }
            else //nel caso avessimo valori sul campo other ma non dati sulla calibrazione
            {
                QString newOther = "none;" + otherString;
                m_mng->SetOther(newOther);
                m_mng->CommitParameters();
            }
        }

        qDebug()<<"Check OTHER Load"<<otherString << "calib:" << m_datiCalib;

        m_end = m_mng->GetDuration() / 1000;
        qDebug() << "Durata esame = " << m_end;
        m_numChannels = m_mng->GetChanNum();
        qDebug() << "NA? di canali = "<< m_numChannels;


        //------ Aggiungo i markers operativi, sono comuni a tutti i canali

        qDebug() << "Marker Operativi = " << m_mng->GetNumOperativeMarkers();

        for(int i = 0; i < m_mng->GetNumOperativeMarkers(); i++) {
            VarMap *mrk = new VarMap;
            m_mng->GetOpMarker(i, &key, numSamp, &descr);
//            qDebug() << numSamp[0];
//            qDebug() << numSamp[1];
//            qDebug() << numSamp[2];
//            qDebug() << numSamp[3];
            double val = (double) numSamp[0] / m_mng->GetNAS(0);
            qDebug() << "Marker" << key << val << descr;

            if(m_markerMap.keys().contains(key)) {   //marker conosciuto le info ce le ho giA
                (*mrk) = m_markerMap[key];
                if ((*mrk)[ATT_TYPE] == 2)//di sistema
                    (*mrk)["lock"] = true;
            }
            else {  //me lo costruisco
                (*mrk)["code"] = "USR";
                (*mrk)["descr"] = descr;//sovrascrivo
                (*mrk)["lock"] = false;
                (*mrk)["key"] = key;
                (*mrk)["color"] = COLOR_OPERATIVE;
                (*mrk)["visible"] = true;
                (*mrk)["category"] = CAT_MARKER;
            }
            (*mrk)["val"] = val;
            (*mrk)["name"] = "Operative";
            (*mrk)["family"] = "Markers";
            mrkOpVec->append(mrk);
        }

        if(!mrkOpVec->isEmpty()) {
            QString family = "Markers";
            QString name = "Operative";
            saveDataAndUpdate(family, name, mrkOpVec);
        }

        //------Aggiungo i canali
        qDebug()<<"aggiungo i canali";

        for(int h = 0; h < m_numChannels; h++) {
            MSignal *sig  = new MSignal;
            sig->resize(m_mng->GetSamplesNumber(h));
            for(int i = 0; i < m_mng->GetSamplesNumber(h); i++)
                sig->replace(i, m_mng->GetValue(h, i));

            qDebug() << "sig:" << sig;
            sig->setName(m_mng->GetChanName(h));
            sig->setSamplingFrequency(m_mng->GetNAS(h));
            sig->setSupLim(m_mng->GetSupLim(h));

            double M = sig->maximum();
            double m = sig->minimum();
            if(sigMax < M)
                sigMax = M;
            if(sigMin > m)
                sigMin = m;

            double supLim = m_mng->GetSupLim(h);
            double infLim = m_mng->GetInfLim(h);
#ifdef PICOFLOW
            Ancestry *chProp = m_configUser.getSafeChild(XML_CHANNELSPROP);
#else
            Ancestry *chProp = m_configUserProp.getSafeChild(XML_CHANNELSPROP);
#endif
            Ancestry *chName = chProp->getSafeChild(m_mng->GetChanName(h).remove("1"));
            //max#min#step#decimals
            QStringList rangesDef = chName->getSafeChild(ATT_RANGE)->getSafeAttribute(ATT_MODEL).split("#");

            while (M > supLim)
            {
                double newSupLim = supLim + rangesDef.at(2).toInt();//aggiungo lo step
                if (newSupLim <= rangesDef.at(0).toInt())
                    supLim = newSupLim;
                else
                    break;
            }

            sig->setSupLim(supLim);
            while (m < infLim)
            {
                double newInfLim = infLim - rangesDef.at(2).toInt();//aggiungo lo step
                if (newInfLim >= -rangesDef.at(0).toInt())
                    infLim = newInfLim;
                else
                    break;
            }
            sig->setInfLim(infLim);
            this->addSignal(sig);
        }

        //------Aggiungo i definer
        int numDefinitori = m_mng->GetNumDefiners();
        qDebug() << "Definer = " << numDefinitori;

        for(int i = 0; i < numDefinitori; i++) {
            VarMap *def = new VarMap;
            m_mng->GetOpMarkerAn(i, &key, tStart, tEnd, chEn, &descr);
            for(int nc = 0; nc < m_numChannels; nc++)
                defEn[i] << chEn[nc];

            (*def)["key"] = key;
            (*def)["name"] = descr;
            (*def)["family"] = "Definers";
            (*def)["xMin"] = (tStart[0]-1)/m_mng->GetNAS(0);
            (*def)["xMax"] = (tEnd[0]-1)/m_mng->GetNAS(0);
            (*def)["yMin"] = sigMin;
            (*def)["yMax"] = sigMax;
            (*def)["num"] = i;
            (*def)["enCh"] = defEn[i];
            (*def)["descr"] = translate(descr);
            (*def)["color"] = "cyan";
            (*def)["category"] = CAT_DEFINER;
            (*def)["resizeable"] = 1;
            qDebug()<<"Definitori:"<< defEn[i];
            defVec->append(def);
        }

        if(!defVec->isEmpty()) {
            QString family = "Definers";
            // QString name="Operative";
            saveDataAndUpdate(family, descr, defVec);
        }

        //mi genera un crash dell'acqtool alla ripaertura dell'esame analizzato
        //il ciclo associa i definitori ai canali abilitati
        //non so se mi servirà in seguito
/*        for(int nc = 0; nc < m_numChannels; nc++) {
            VarMapVec *subVec = new VarMapVec;
            for(int i = 0; i < m_mng->GetNumDefiners(); i++)
                if(defEn[i].at(nc).toBool())
                    subVec->append(defVec->at(i));
            if(!subVec->isEmpty())
                saveDataAndUpdate(m_mng->GetChanName(nc), "Definers", subVec);
        }*/

        //------ Aggiungo i markers analitici, sono associati ad un definitore
        //con questo controllo evitiamo i problemi dovuti ad un salvataggio errato del file,
        //dove risulta nessun definitore ma markers analitici salvati
#ifndef PICOFLOW
        if (numDefinitori > 0)
        {
            qDebug() << "Marker Analitici = " << m_mng->GetNumAnalyticalMarkers();

            //mrkAnVec->clear();
            QVector<int32_t> numChVec;
            for(int i = 0; i < m_mng->GetNumAnalyticalMarkers(); i++) {
                VarMap *mrk = new VarMap;
                m_mng->GetAnMarker(i, &key, &numCh, &numSamp[0], &numDef);
                numChVec << numCh;

                QVariantList valuesY;
                foreach(MSignal *sig, m_signalVector)
                    if(sig->getName() == m_mng->GetChanName(numCh)) {
                        qDebug("%s: %d", sig->getName().toLatin1().constData(), sig->size());
                        for (int i = 0; i < sig->size(); i++)
                            valuesY.append(sig->at(i));
                    }

                double val = (double) numSamp[0] / m_mng->GetNAS(numCh);
                (*mrk)["val"] = val;
                (*mrk)["type"] = TYPE_ANALYTICAL;
                (*mrk)["name"] = "Analitical";
                (*mrk)["family"] = "Markers";
                (*mrk)["nas"] =  m_mng->GetNAS(numCh);
                (*mrk)["valuesY"] = valuesY;
                (*mrk)["graph"] = m_mng->GetGraph(numCh)-1;
                (*mrk)["code"] =  "f" + QString::number(key);
                (*mrk)["descr"] = descr;
                (*mrk)["lock"] = false;
                (*mrk)["channel"] = numCh;
                (*mrk)["defCode"] = (qulonglong)defVec->value(numDef);
                (*mrk)["key"] = key;
                (*mrk)["color"] = COLOR_ANALYTICAL;
                (*mrk)["visible"] = true;
                (*mrk)["category"] = CAT_MARKER;
                mrkAnVec->append(mrk);
                //lo associo al suo definitore
                QList<QVariant> anM  =  defVec->value(numDef)->value("anMarkers").toList();
                anM.append((qulonglong)mrk);
                (*defVec->value(numDef))["anMarkers"] = anM;
            }

            if(!mrkAnVec->isEmpty()) {
                QString family = "Markers";
                QString name = "Analitical";
                saveDataAndUpdate(family, name, mrkAnVec);
            }
        }
#endif

        //        for(int32_t nc=0;nc<m_numChannels;nc++)
        //        {
        //            VarMapVec *subVec=new VarMapVec;
        //            for(i=0;i<numChVec.size();i++)
        //                if(nc==numChVec[i])
        //                    subVec->append(mrkAnVec->at(i));

        //            if(!subVec->isEmpty())
        //                saveDataAndUpdate(m_mng->GetChanName(nc),"Analytical Markers",subVec);
        //        }

        qDebug() << "mi memorizzo le analisi associate a questo esame e l'associazione con i definitori";
        for (int i = 0; i < m_mng->GetAnalysiNum(); i++) {
            int anal = m_mng->GetAnalysis(i).toInt();

            switch (anal) {
            case FLW_AVD_STUDY:
                m_analysisMap[FLW_AVD_STUDY] = MK_FLOWMETRY;
                break;

            case CYS_STUDY:
                m_analysisMap[CYS_STUDY] = MK_FILLING;
                break;

            case PFS_STD_STUDY:
                m_analysisMap[PFS_STD_STUDY] = MK_VOIDING;
                break;

            case UPP_STA_STUDY:
                m_analysisMap[UPP_STA_STUDY] =  MK_STARTPROFILE;
                break;

            case UPP_DYN_STUDY:
                m_analysisMap[UPP_DYN_STUDY] = MK_STARTDYNPROFILE;
                break;

            case WTK_STUDY:
                m_analysisMap[WTK_STUDY] = MK_WTK;
                break;

            case LPP_STUDY:
                m_analysisMap[LPP_STUDY] = MK_LPP;
                break;

            case DO_STUDY:
                m_analysisMap[DO_STUDY] = MK_DO;
                break;

            case BIO_STUDY:
                m_analysisMap[BIO_STUDY] = MK_BIO;
                break;

            default:
                break;
            }
            //m_analysisMap
        }

        qDebug() << "Building infoList ...";
        if(!updateInfoList()) {
            qCritical() << "Error building infolist";
            return;
        }

        //carico le info necessarie dal file di config
        Ancestry *autoflow = m_configUser.getSafeChild("AutomaticFlow");
        m_autoFlow = (autoflow->getSafeChild("Auto")->getSafeAttribute(ATT_VALUE) == "true" ? 0 : 2);

        Ancestry *autoloop = m_configUser.getSafeChild("AutomaticFlow");
        m_autoLoop = (autoloop->getSafeChild("Loop")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

        Ancestry *autoprint = m_configPrinter.getSafeChild("Settings");
        m_autoPrint = (autoprint->getSafeChild("AutoPrint")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

        Ancestry *siroky = m_configPrinter.getSafeChild("Settings");
        m_Siroky = (siroky->getSafeChild("Siroky")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

        Ancestry *liverpool = m_configPrinter.getSafeChild("Settings");
        m_Liverpool = (liverpool->getSafeChild("Liverpool")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

        Ancestry *printmode = m_configPrinter.getSafeChild("Settings");
        m_landscape = (printmode->getSafeChild("PrinterMode")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

        Ancestry *head1 = m_configPrinter.getSafeChild("Headers");
        m_firstHead = head1->getSafeChild("First")->getSafeAttribute(ATT_VALUE);

        Ancestry *head2 = m_configPrinter.getSafeChild("Headers");
        m_secondHead = head2->getSafeChild("Second")->getSafeAttribute(ATT_VALUE);

//        if (m_Liverpool) //da sentire sergio
//            m_Siroky = false;

        //libreria di analisi: creo oggetto.
        m_ana = new Analyze();

        m_mng->Close();
        break;
    }
    default: qDebug() << "Should not be here!!!!!"; break;
    }

    emit loadingCompleted();

    qDebug()<<"Load file operation completed succesfully!";
}

void MDataManager::resetAll()
{
    m_data.clear();
    m_availableData.clear();
    for(int i = 0; i < m_signalVector.size(); i++)
        delete m_signalVector[i];
    m_signalVector.clear();
    m_storage.clearAll();
}

void MDataManager::saveChanges()
{
    if(m_copy != NULL) {
        delete m_copy;
        m_copy = NULL;
    }
    m_copy = new DatafileManager;
    m_copy->SetFileName(m_copyFileName);
    m_copy->SetFileType(7);

    qDebug() << "Copia aperta?" << m_copy->Open();
    qDebug() << "Copia caricata?" << m_copy->GetParameters();
    qDebug() << "Eliminati marker e definer?" << m_copy->DeleteAllMarkers();

    //-Salvataggio Marker
    VarMapVec *elements = m_storage.getAll(CAT_MARKER);

    foreach (VarMap *curMap, (*elements)) {
        qDebug() << "Salvo l'oggetto: " << curMap;

        qDebug() << "Inizio salvataggio";

        QVector<int32_t> numCamp;
        numCamp.resize(m_numChannels);
        for(int i = 0; i < m_numChannels; i++)
            numCamp[i] = curMap->value("val").toFloat() * m_copy->GetNAS(i) + 0.5;

        if(curMap->value("color").toString() == COLOR_OPERATIVE) {
            m_copy->AddOpMarker(numCamp.data(), curMap->value("key").toInt(), curMap->value("descr").toString());
        }
        if((curMap->value("color").toString() == COLOR_ANALYTICAL) && (curMap->value("visible").toBool() == true)) {
            m_copy->AddAnMarker(curMap->value("channel").toInt(),
                                numCamp[curMap->value("channel").toInt()],
                                curMap->value("key").toInt(),
                                curMap->value("num").toInt());
        }
        qDebug() << "Fine salvataggio marker";
    }

    //-Salvataggio Definer
    elements = m_storage.getAll(CAT_DEFINER);

    foreach (VarMap *curMap, (*elements)) {
        qDebug() << "Inizio salvataggio definer";
        int32_t * start = new int32_t[m_numChannels];
        int32_t * end   = new int32_t[m_numChannels];
        unsigned char *enCh = new unsigned char[m_numChannels];

        for(int i = 0; i < m_numChannels; i++) {
            start[i] = curMap->value("xMin").toFloat() * m_copy->GetNAS(i);
            end[i]   = curMap->value("xMax").toFloat() * m_copy->GetNAS(i);
            enCh[i]  = curMap->value("enCh").toList().at(i).toBool();
            qDebug() << start[i] << end[i] << enCh[i];
        }

        m_copy->AddOpMarkerAn(start, end, enCh, curMap->value("key").toInt(), curMap->value("descr").toString());
        qDebug() << "Fine salvataggio definer";
    }

    qDebug() << "Commit markers?" << m_copy->CommitMarkers();
    qDebug() << "Chiudo il file?" << m_copy->Close();

    delete m_copy;
    m_copy = NULL;
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
    qDebug() << "Aggiungi alle famiglie " << __families << " un " << __cat << " chiamato " << __name << " con queste caratteristiche" << __info;

    if(!m_possibleCategories.contains(__cat)) {
        qCritical() << "unknown category" + __cat;
        return false;
    }

    if(__families.isEmpty()) {
        qCritical() << "Families corrupted";
        return false;
    }

    foreach (QString family, __families) {
        //devo aggiungere il mio nuovo oggetto ad ogni famiglia che ho scelto
        VarMapVec *objVec = NULL;

        if(__cat == CAT_MARKER) {
            objVec = new VarMapVec;
            if(__info.isEmpty()) {      // link==none vuol dire che non prende info da nessuno
                VarMap *mrk = new VarMap;
                (*mrk)["val"] = 1;
                (*mrk)["descr"] = "New Marker";
                (*mrk)["lock"] = false;
                (*mrk)["color"] = COLOR_CUSTOM;
                (*mrk)["visible"] = true;
                (*mrk)["category"] = CAT_MARKER;
                objVec->append(mrk);
            }
        }

        if(__cat == CAT_DEFINER) {
            objVec = new VarMapVec;
            if(__info.isEmpty()) {      //link==none vuol dire che non prende info da nessuno
                VarMap *def = new VarMap;
                (*def)["key"] = "-1";
                (*def)["xMin"] = 0;
                (*def)["xMax"] = 1;
                (*def)["yMin"] = 0;
                (*def)["yMax"] = 1;
                (*def)["descr"] = "New Definer";
                (*def)["color"] = "white";
                (*def)["category"] = CAT_DEFINER;
                (*def)["resizeable"] = 1;
                //(*def)["moveable"] = 1;
                objVec->append(def);
            }
        }

        if(objVec != NULL) {
            if(saveDataAndUpdate(family, __name, objVec, APPEND))
                continue;
            else
                return false;
        }
        else {
            qCritical() << "Category " + __cat + " not recognized";
            return false;
        }
    }

    return true;
}

void MDataManager::updateAvailableData()
{
    m_availableData.clear();
    QStringList signalNames = m_data.keys();

    for(int i = 0; i < signalNames.size(); i++) {
        m_availableData << "$Group";
        //m_availableData << signalNames[i];
        m_availableData << m_data[signalNames[i]];
        m_availableData << "&Group";
    }

    //qDebug()<<"m_availableData = "<<m_availableData;
    emit availableDataChanged();
}

bool MDataManager::updateInfoList()
{
    // grafici

    QStringList graphs = m_chanInPlots.keys();

    QVariantList infoList;
    int countGraphs = 0;

    foreach(QString graph, graphs) {
        QVariantList pair;
        QStringList elements;
        QVariant row, gName, gElemPack;
        gName = graph;

        //riempo con gli elementi che mi servono

        //tracce molto facile dato che ce le ho giA
        foreach (QString chanName, m_chanInPlots[graph]) {
            elements << chanName + ":Signal";
        }

        //markers
        VarMapVec *op = m_storage.getAll(CAT_MARKER);
        foreach (VarMap *curMap, (*op)) {
            if (curMap->value("color") == COLOR_OPERATIVE) {
                elements << "Markers:Operative";
                break;
            }
        }

        VarMapVec *opAn = m_storage.getAll(CAT_MARKER);
        foreach (VarMap *curMap, (*opAn)) {
            if (curMap->value("color") == COLOR_ANALYTICAL) {
                int ch = curMap->value("graph").toInt();
                if (ch == countGraphs) {
                    elements << "Markers:Analitical";
                    break;
                }
            }
        }

        //devo trovare i canali dei segnali associati al grafo (countGraphs)
       QVector<int> chInGraph;
        int i= 0;
        foreach(MSignal *sig, m_signalVector) {
            QString name = sig->getName();
            foreach (QString chanName, m_chanInPlots[graph]) {
                if (chanName == name){
                    chInGraph.append(i);
                }
            }
            i++;
        }

        VarMapVec *def = m_storage.getAll(CAT_DEFINER);
        foreach (VarMap *curMap, (*def)) {
            QStringList enabled = curMap->value("enCh").toStringList();
            for (int j=0; j<chInGraph.length(); j++)
            {//se un canale associato al grafo è abilitato disegno il definitore
                if (enabled.at(chInGraph.at(j)) == "1")
                    elements << "Definers:" + curMap->value("name").toString();
            }
        }

        //elementi finiti
        gElemPack = elements;
        //a posto impacchetto tutto
        pair << gName << gElemPack;
        row = pair;
        infoList << row;

        countGraphs++;
    }

    qDebug() << "infolist update   " << infoList;
    m_infoList = infoList;

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

    //allora dato che questa funzione e chiamata dopo aver costruito i plotter
    //so gia  quanti e come si chiamano i grafici

    QStringList graphs = m_chanInPlots.keys();
    QVariantList infoList;

    foreach(QString graph,graphs) {
        QVariantList pair;
        QStringList elements;
        QVariant row, gName, gElemPack;
        gName = graph;
        //riempo con gli elementi che mi servono

        //tracce molto facile dato che ce le ho giA
        foreach (QString chanName, m_chanInPlots[graph]) {
            elements << chanName + ":Signal";
        }

        //markers
        if(m_mng->GetNumOperativeMarkers() > 0)
            elements << "Markers:Operative";

        if (m_mng->GetNumDefiners() > 0)
            elements << "Definers:Operative";

        //elementi finiti
        gElemPack = elements;

        //a posto impacchetto tutto
        pair << gName << gElemPack;
        row = pair;
        infoList << row;
    }

    qDebug() << "infolist   " << infoList;
    m_infoList = infoList;

    return true;
}

/**
 * @brief MDataManager::registerModel registers a model to the class
 * @param __cat of the model marker, definer, track ...
 * @param __roles is the list of roles of the current model
 */
void MDataManager::registerModel(QString __cat, QStringList __roles)
{
    m_possibleCategories << __cat;
    m_modelMap[__cat] = __roles;
    m_storage.addCategory(__cat);
}

QVariantList MDataManager::getData(QString __cat)
{
    if(m_possibleCategories.contains(__cat)) {
        ModelManager mng;
        mng.setType(__cat);
        mng.setRoles(m_modelMap[__cat]);
        mng.setStore(&m_storage);
        mng.setInfoList(m_infoList);
        return mng.drawList();
    }
    else {
        qCritical() << "Category" << __cat << "not registered!";
        return QVariantList();
    }
}

QVariantList MDataManager::getPlotLimits()
{
    QMap<QString, QStringList> infoMap, plotMap;

    for(int plotIndex = 0; plotIndex < m_infoList.size(); plotIndex++) {
        //dalle info che ci arrivano da dialog creiamo una mappa dove ad ogni plot assegniamo
        //quello che ci va disegnato
        QVariantList plotInfo = m_infoList.at(plotIndex).toList();
        QString plotName = plotInfo.at(0).toString();
        QStringList info = plotInfo.at(1).toStringList();
        infoMap[plotName] = info;
    }

    foreach (QString plotName, infoMap.keys()) {
        foreach (QString data, infoMap[plotName]) {
            if(data.contains("Signal"))     //cerco segnali originali
                plotMap[plotName] << data.split(":").at(0);
        }
    }

    //ora abbiamo dentro plotMap l'elenco dei nomi dei segnali segnali che ci servono
    QVariantList limits;

    foreach (QString plotName, plotMap.keys()) {
        QStringList families = plotMap[plotName];
        limits << "$Limit";
        limits << plotName;
        if(!families.isEmpty()) {
            double xMin = INF, xMax = -INF, yMin = INF, yMax = -INF;
            foreach (QString family, families) {
                VarMapVec *cur = (VarMapVec *)m_storage.pickUp(family,"Signal");

                foreach(VarMap *map, *cur) {
                    qulonglong p = (*map)["pointer"].toULongLong();
                    MSignal *sig = (MSignal*)p;
                    //qDebug()<<(*sig);
                    if(sig->getT0()       < xMin) xMin = sig->getT0();
                    if(sig->getDuration() > xMax) xMax = sig->getDuration();
                    //if(sig->minimum()     < yMin) yMin = sig->minimum();
                    //if(sig->maximum()     > yMax) yMax = sig->maximum();
                    yMin = sig->getInfLim();
                    yMax = sig->getSupLim();
                    //qDebug()<<yMin<<yMax;
                    //qDebug()<<"Segnale lungo:"<<sig->getSize();
                }
            }
            limits << xMin << xMax << yMin << yMax;//(yMax + abs(yMax*0.05));
        }
        limits << "&Limit";
    }
    return limits;
}

bool MDataManager::changeObject(QVariantList __curObj)
{
    //qDebug()<<"Cambio un elemento con queste caratteristiche :"<<__curObj;
    if(__curObj.length() != 2) {
        setToSave("");  //necessario chiedere se salvare

        qulonglong whoAmI = __curObj.first().toULongLong();
        //tolgo il whoami e lascio solo le proprietA
        __curObj.removeFirst();

       // qDebug() << "Richiesta di modifica per " << whoAmI;
        if(m_storage.modifyElement(whoAmI, __curObj)) {
            //una volta applicate le modifiche le salvo e aggiorno la grafica
            saveChanges();
            updateInfoList();
            emit reloadingCompleted();
        }
        return true;
    }
    qCritical() << "Length error";
    return false;
}

QVariant MDataManager::getSignal(QString __name)
{
    VarMapVec *v = (VarMapVec*) m_storage.pickUp(__name, "Signal");
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
    if(__what == "Families")
        list << m_storage.getFamilies();
    if(__what == "Names")
        list << m_storage.getNames(__filterFamily, __filterType);

    return list;
}

bool MDataManager::saveDataAndUpdate(QString __family, QString __name, VarMapVec* __elements, bool __whatIfAlreadyPresent)
{
    if(__elements->isEmpty()) {
        qCritical() << "No data in __elements";
        return false;
    }

    if(m_storage.archive(__family, __name, __elements, __whatIfAlreadyPresent)) {
        //se siamo qua dentro vuol dire che tutto A? andato liscio e possiamo visualizzare le info all'utente
        if(!m_data.keys().contains(__family)) {
            m_data[__family].append(__name);
            emit availableTracksChanged();
        }
        else
            if(!m_data[__family].contains(__name))
                m_data[__family].append(__name);
        updateAvailableData();
        setToSave("");  //necessario chiedere se salvare
        return true;
    }
    else
        return false;
}

bool MDataManager::checkForVolRes()
{
    if (getValVolRes() != -999)
        return false;

    m_mng = new DatafileManager;
    qDebug() << m_copyFileName;
    m_mng->SetFileName(m_copyFileName);
    m_mng->SetFileType(7);

    qDebug() << "File Aperto?" << m_mng->Open();
    qDebug() << "File Caricato?" << m_mng->GetParameters();

    m_numAna = m_mng->GetAnalysiNum();

    //gestione campo Other del file .pic
    QString otherString = m_mng->GetOther();
    qDebug() << "m_mng->GetOther() ==" << otherString;
    if(otherString == "none")
        otherString  += ";";
    QStringList stringSplit = otherString.split(";");
    qDebug() << "m_mng->GetOther() ==" << otherString << stringSplit;
    if (stringSplit.length() == 2)
    {
        otherString  += "0;" + QString::number(m_autoFlow) + ";";
        setValVolRes(0);
        m_mng->SetOther(otherString);
        m_mng->CommitParameters();
        //se è la prima volta che apro un esame di flussimetria automatica la stampa è automatica
        if (m_autoFlow == 0)
            m_autoPrint =  true;
    }
    else
    {
        //nel caso non fosse la prima volta che apro l'esame (appena dopo l'acquisizione)
        //non devo considerare la stampa automatica
        m_autoPrint =  false;
        setValVolRes(stringSplit.at(1).toInt());
        m_autoFlow = stringSplit.at(2).toInt();
    }

    //ciclo per individuare se e necessario aprire la dlg del volume residuo
    bool volRes = false;
    for (int i = 0; i < m_numAna; i++)
    {
        int anaType = m_mng->GetAnalysis(i).toInt();
        if (anaType == FLW_AVD_STUDY) {
            if (m_autoPrint)
                setValVolRes(0);//-1?;
            else {
                volRes = true;
                QString tipoAna = tr("Flowmetry");
                emit sg_openVolResDlg(tipoAna);
                break;
            }
        }
    }

    m_mng->Close();
    qDebug() << "FINE" << volRes;
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
qDebug() << "INIZIO";

    if (checkForVolRes())
        return;

    saveChanges();

    m_mng = new DatafileManager;
    m_mng->SetFileName(m_copyFileName);
    m_mng->SetFileType(7);

    qDebug() << "File Aperto?" << m_mng->Open();
    qDebug() << "File Caricato?" << m_mng->GetParameters();

    //mi salvo nel campo other il valore del volume residuo nel caso l'utente lo avesse nonecambiato
    QString other = m_mng->GetOther();
    QStringList otherList = other.split(";");
    m_datiCalib = otherList.at(0);//stringa per dati calibrazione da stampare
    qDebug() << "m_mng->GetOther() ==" << other;
    other.clear();
    int valResOld = otherList.at(1).toInt();
    int valResNew = getValVolRes();
    if (valResNew != valResOld) {
        otherList[1] = QString::number(valResNew);
        for (int j=0; j<otherList.length()-1;j++)
            other += otherList.at(j) + ";";
        qDebug()<<"OTHER ANALISI"<<other;
        m_mng->SetOther(other);
        m_mng->CommitParameters();
        setToSave("");  //necessario chiedere se salvare
    }

    m_ana->SetData(m_mng);

//    int anaProg = QDateTime::currentDateTime().toTime_t();
    QVariantList En;

    for (int i = 0; i < m_numAna; i++) {
        int anaType = m_mng->GetAnalysis(i).toInt();
        if (anaType == FLW_AVD_STUDY) {
            //verifica se c'e' un definitore per questa analisi.
            VarMap mkOpAnIn;
            bool found = false;
            VarMapVec* elements = m_storage.getAll(CAT_DEFINER);
            foreach (VarMap *curMap, (*elements)) {
                if (curMap->value("key") == MK_FLOWMETRY) {
                    mkOpAnIn = *curMap;
                    found = true;
                }
            }

            if (!found) {

                //in caso di PICOFLOW2REV3, da controllare in caso desktop
                setToSave("");  //necessario chiedere se salvare

                int posQ = -1, posV = -1;
                int i = 0;
                foreach(MSignal *sig, m_signalVector) {
                    QString name = sig->getName();
                    if (name.startsWith("Q"))
                        posQ = i;
                    if (name.startsWith("VV") || name ==  "VLMv")
                        posV = i;
                    i++;
                }

                if ((m_numAna == 1) || (posQ > -1)) {
                    //se non c'e' il defintore, ma questa e' l'unica analisi,
                    //viene inserito automaticamente sul canale del flusso.
                    for (int i = 0; i < m_numChannels; i++)
                        En << 0;

                    En[posQ] = 1;
                    if (posV > -1)
                        En[posV] = 1;

                    //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'e
                    //provo a disegnare il definitore
                    QString family = "Definers";
                    QString name = "Flowmetry";
                    VarMap *def = new VarMap;
                    (*def)["key"] = MK_FLOWMETRY;
                    (*def)["family"] = family;
                    (*def)["name"] = name;
                    (*def)["xMin"] = 1;
                    (*def)["xMax"] = m_end - 1;
                    (*def)["yMin"] = 0;
                    (*def)["yMax"] = 100;
                    (*def)["num"] = elements->length();
                    (*def)["enCh"] = En;
                    (*def)["descr"] = translate(name);
                    (*def)["color"] = COLOR_DEFINER;
                    (*def)["category"] = CAT_DEFINER;
                    (*def)["resizeable"] = 1;

                    VarMapVec *defVec = new VarMapVec;
                    defVec->append(def);
                    saveDataAndUpdate(family, name, defVec);
                    mkOpAnIn = *def;
                    found = true;

                    //saveall?? se si la updateinfolist puo leggere da datafile?
                    //saveChanges();//(?)
                    //buildInfoList();
                    updateInfoList();
                    emit reloadingCompleted();
                }
                //                else
                //                {
                //                    //cotrolliamo se c'A? il marker 'ff' free flow
                //                    //nel qual caso, il definitore viene inserito da lA? fino a:
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
                //                                for (int i=0;i<m_numChannels;i++)
                //                                    En << 0;

                //                                En[posQ] = 1;
                //                                if (posV > -1)
                //                                    En[posV] = 1;

                //                                //ho tralasciato la parte che gestisce l'iconizzazione che forse non c'A?


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

            if (found) {
                //                                        'Salva l'immagine dei tracciati all'interno del definitore
                //                                        myGraphPlot.RedrawGraphForPrint("GR200", MarkerUtils.getOpMarkerAn(mkOpAnIn - 1).myNumStart(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)), MarkerUtils.getOpMarkerAn(mkOpAnIn - 1).myNumEnd(myGraphPlot.GetTruePosChannel(myGraphPlot.MaxNASCh)))
                m_analized = true;
                //                                            UpdateTestOther()
                //Lancia analisi e Inizializza nomogrammi
                InitPageGraphs(FLW_AVD_STUDY);
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
                //                                            'su DlgReport il messaggio d'errore anzichA? il risultato.
                //                                            'TODO: implementare un sistema per ottenere il numero di
                //                                            'analisi dello stesso tipo effettuate.
                //                                            'MarkerOpAn(mkOpAnIn).Warn = True
                //                                            WaitForDone = 2
            }
        }
    }

    qDebug() << "File chiuso" << m_mng->Close();

    //qml
    qDebug() << "FINE";
    emit sg_loadResult();
}

void MDataManager::setToSave(QString __val)
{
    if (m_toSave == __val)
        return;

    m_toSave = __val;
    emit infoToSave();
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

void MDataManager::InitPageGraphs(int __anaType)
{
    if (__anaType == FLW_AVD_STUDY) {
        //determina tratti da analizzare.
        //Per ora considera solo il primo.

        int evStart = 0;
        int evEnd = 0;
        VarMap *evMarkOpIn = NULL;
        unsigned char evAuto = 0;
        QVector<unsigned char> enCh;

        VarMapVec* elements = m_storage.getAll(CAT_DEFINER);
        foreach (VarMap *curMap, (*elements)) {
            if (curMap->value("key") == MK_FLOWMETRY) {
                evStart = curMap->value("xMin").toInt()*1000;
                evEnd = curMap->value("xMax").toInt()*1000;
                evMarkOpIn = curMap;
                evAuto = 1;

                //ogni volta che si passano i definitori alla libreria di analisi dopo aver nascosto/rivisualizzato i canali
                //e' necessario sistemare l'array dei canali abilitati considerando tutti i canali dell'analisi.
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



                for(int i = 0; i < m_numChannels; i++)
                    enCh.append(curMap->value("enCh").toList().at(i).toBool());

                // marker analitici
                VarMapVec* eleAnMarkers = m_storage.getAll(CAT_MARKER);
                foreach (VarMap *curMarker, (*eleAnMarkers)) {
//                    int ff = evMarkOpIn->value("name").toInt();
                    if ((curMarker->value("type") == TYPE_ANALYTICAL) &&
                        (curMarker->value("defCode").toInt() == evMarkOpIn->value("whoAmI").toInt()))
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
        int def = evMarkOpIn->value("num").toInt();
        (void) def;
        bool ret = InitArraysFLW(evStart, evEnd, enCh, evMarkOpIn->value("num").toInt(), evAuto);
        (void) ret;
#ifndef PICOFLOW
        //se gli anMarker li trovo per la prima volta li inserisco in grafica
        if (evAuto != 0) {
            VarMapVec *mrkAnVec = new VarMapVec;
            unsigned char key;
            int32_t numCh;
            int32_t numSamp;
            int32_t numDef;
            QString descr = "flowmetry";
            for (int i = 0; i < m_mng->GetNumAnalyticalMarkers(); i++) {
                VarMap *mrk = new VarMap;
                m_mng->GetAnMarker(i ,&key, &numCh, &numSamp, &numDef);

                QVariantList valuesY;
                foreach(MSignal *sig, m_signalVector)
                    if(sig->getName() == m_mng->GetChanName(numCh))
                        for (int i = 0; i < sig->size(); i++)
                            valuesY.append(sig->at(i));

                double val = (double) numSamp / m_mng->GetNAS(numCh);
                (*mrk)["val"] = val;
                (*mrk)["type"] = TYPE_ANALYTICAL;
                (*mrk)["name"] = "Analitical";
                (*mrk)["family"] = "Markers";
                (*mrk)["nas"] = m_mng->GetNAS(numCh);
                (*mrk)["valuesY"] = valuesY;
                (*mrk)["graph"] = m_mng->GetGraph(numCh)-1;
                (*mrk)["code"] = "f" + QString::number(key);
                (*mrk)["descr"] = descr;
                (*mrk)["lock"] = false;
                (*mrk)["channel"] = numCh;
                (*mrk)["defCode"] = (qulonglong)evMarkOpIn->value("whoAmI").toULongLong();
                (*mrk)["key"] = key;
                (*mrk)["color"] = COLOR_ANALYTICAL;
                (*mrk)["visible"] = true;
                (*mrk)["category"] = CAT_MARKER;
                mrkAnVec->append(mrk);
                //lo associo al suo definitore
                QList<QVariant> anM = evMarkOpIn->value("anMarkers").toList();
                anM.append((qulonglong)mrk);
                (*evMarkOpIn)["anMarkers"] = anM;
            }

            if(!mrkAnVec->isEmpty()) {
                QString family = "Markers";
                QString name = "Analitical";
                saveDataAndUpdate(family, name, mrkAnVec);
            }

            // saveChanges();//(?)
            updateInfoList();
            emit reloadingCompleted();
        }
#endif
    }
}

bool MDataManager::InitArraysFLW(int __start,
                                 int __end,
                                 QVector<unsigned char> __chEn,
                                 int __curDef,
                                 unsigned char __auto)
{
    double startTh = 0;
    double heightTh = 0;
    double widthTth = 0;

    Ancestry *config_ana = new Ancestry;
    qDebug()<<"File Par Ana caricato correttamente?"<<config_ana->loadFromXML(":/Config/ParAna.xml");

    QString value;
    foreach (Ancestry *child, config_ana->getChildren()) {
        if (child->name() == "Analysis") {
            foreach (Ancestry *def, child->getChildren()) {
                value = def->getSafeAttribute("Type");
                if (value.toInt() == MK_FLOWMETRY) {
                    qDebug() << "Trovato MK_FLOWMETRY";
                    Ancestry *flusso = def->getSafeChild("Q");
                    startTh = flusso->getSafeChild("StartTh")->getTextOfChild("value").toDouble();
                    heightTh = flusso->getSafeChild("AmpTh")->getTextOfChild("value").toDouble();
                    widthTth = flusso->getSafeChild("DurTh")->getTextOfChild("value").toDouble();
                    break;
                }
            }
        }
    }

    delete config_ana;

    qDebug() << "analisi";

    int res = m_ana->FLW_Adv_Analysis(1, __chEn, __start, __end, __curDef, startTh, heightTh, widthTth, __auto, getValVolRes(), 0, true);
    if (res < 0)
        return false;

    qDebug() <<  "Legge i risultati";
    int numEv = 1;
    res = ReadResult(numEv);
    if (res < 0)
        return false;

    qDebug() << "costruisco i segnali da disegnare nel plot per i nomogrammi";
    for (int i = 0; i < numEv; i++) {
        qDebug() << "nomogramma LiverpoolQMax";
        MSignal *sig0 = new MSignal;
        MSignal *sig1 = new MSignal;
        MSignal *sig2 = new MSignal;
        MSignal *sig3 = new MSignal;
        MSignal *sig4 = new MSignal;
        MSignal *sig5 = new MSignal;
        MSignal *sig6 = new MSignal;

        int lunx = m_aflwdatas.at(i+1)->getLiverpoolMax()->getXmax();
        sig0->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(0).data(), lunx);
        sig0->setName("linea1");
        sig1->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(1).data(), lunx);
        sig1->setName("linea2");
        sig2->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(2).data(), lunx);
        sig2->setName("linea3");
        sig3->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(3).data(), lunx);
        sig3->setName("linea4");
        sig4->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(4).data(), lunx);
        sig4->setName("linea5");
        sig5->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(5).data(), lunx);
        sig5->setName("linea6");
        sig6->setData(m_aflwdatas.at(i+1)->getLiverpoolMax()->getLineY(6).data(), lunx);
        sig6->setName("linea7");

        QVariantList tracce;
        tracce << "$Track" << "family" << sig0->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line1" << "pointer" << (qulonglong)sig0 << "&Track";
        tracce << "$Track" << "family" << sig1->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line2" << "pointer" << (qulonglong)sig1 << "&Track";
        tracce << "$Track" << "family" << sig2->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line3" << "pointer" << (qulonglong)sig2 << "&Track";
        tracce << "$Track" << "family" << sig3->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line4" << "pointer" << (qulonglong)sig3 << "&Track";
        tracce << "$Track" << "family" << sig4->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line5" << "pointer" << (qulonglong)sig4 << "&Track";
        tracce << "$Track" << "family" << sig5->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line6" << "pointer" << (qulonglong)sig5 << "&Track";
        tracce << "$Track" << "family" << sig6->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line7" << "pointer" << (qulonglong)sig6 << "&Track";

        QVariantList colori;
        colori << "green" << "red" << "black" << "black" << "black" << "black" << "green";

        m_aflwdatas.at(i+1)->getLiverpoolMax()->setColors(colori);
        m_aflwdatas.at(i+1)->getLiverpoolMax()->setTracce(tracce);


        qDebug() << "nomogramma LiverpoolQAve";
        MSignal *sig0Ave = new MSignal;
        MSignal *sig1Ave = new MSignal;
        MSignal *sig2Ave = new MSignal;
        MSignal *sig3Ave = new MSignal;
        MSignal *sig4Ave = new MSignal;
        MSignal *sig5Ave = new MSignal;
        MSignal *sig6Ave = new MSignal;

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
        tracce << "$Track" << "family" << sig0Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line1" << "pointer" << (qulonglong)sig0Ave << "&Track";
        tracce << "$Track" << "family" << sig1Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line2" << "pointer" << (qulonglong)sig1Ave << "&Track";
        tracce << "$Track" << "family" << sig2Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line3" << "pointer" << (qulonglong)sig2Ave << "&Track";
        tracce << "$Track" << "family" << sig3Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line4" << "pointer" << (qulonglong)sig3Ave << "&Track";
        tracce << "$Track" << "family" << sig4Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line5" << "pointer" << (qulonglong)sig4Ave << "&Track";
        tracce << "$Track" << "family" << sig5Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line6" << "pointer" << (qulonglong)sig5Ave << "&Track";
        tracce << "$Track" << "family" << sig6Ave->getName() << "name" << "Signal" << "Category" << CAT_TRACK << "descr" << "line7" << "pointer" << (qulonglong)sig6Ave << "&Track";

        colori.clear();
        colori << "green" << "red" << "black" << "black" << "black" << "black" << "green";
        m_aflwdatas.at(i+1)->getLiverpoolAve()->setColors(colori);
        m_aflwdatas.at(i+1)->getLiverpoolAve()->setTracce(tracce);

        qDebug() << "nomogramma Siroky Max & Ave";
        if (!m_sexPatient) {
            MSignal *sig0SirMax = new MSignal;
            MSignal *sig1SirMax = new MSignal;
            MSignal *sig2SirMax = new MSignal;
            MSignal *sig3SirMax = new MSignal;
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
            tracce << "$Track"<<"family"<<sig0SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line1"<<"pointer"<<(qulonglong)sig0SirMax<<"&Track";
            tracce << "$Track"<<"family"<<sig1SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line2"<<"pointer"<<(qulonglong)sig1SirMax<<"&Track";
            tracce << "$Track"<<"family"<<sig2SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line3"<<"pointer"<<(qulonglong)sig2SirMax<<"&Track";
            tracce << "$Track"<<"family"<<sig3SirMax->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line4"<<"pointer"<<(qulonglong)sig3SirMax<<"&Track";

            colori.clear();
            colori << "black" << "red" << "black" << "black";
            m_aflwdatas.at(i+1)->getSirokyMax()->setColors(colori);
            m_aflwdatas.at(i+1)->getSirokyMax()->setTracce(tracce);

            //bande colorate
            QVector<int> linee;
            linee.append(3);
            linee.append(1);
            m_aflwdatas.at(i+1)->getSirokyMax()->setLinea(linee);

            //nomogramma Siroky Ave
            MSignal *sig0SirAve = new MSignal;
            MSignal *sig1SirAve = new MSignal;
            MSignal *sig2SirAve = new MSignal;
            MSignal *sig3SirAve = new MSignal;
            MSignal *sig4SirAve = new MSignal;

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
            tracce<< "$Track"<<"family"<<sig0SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line1"<<"pointer"<<(qulonglong)sig0SirAve<<"&Track";
            tracce<< "$Track"<<"family"<<sig1SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line2"<<"pointer"<<(qulonglong)sig1SirAve<<"&Track";
            tracce<< "$Track"<<"family"<<sig2SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line3"<<"pointer"<<(qulonglong)sig2SirAve<<"&Track";
            tracce<< "$Track"<<"family"<<sig3SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line4"<<"pointer"<<(qulonglong)sig3SirAve<<"&Track";
            tracce<< "$Track"<<"family"<<sig4SirAve->getName()<<"name"<<"Signal"<<"Category"<<CAT_TRACK<<"descr"<<"line5"<<"pointer"<<(qulonglong)sig4SirAve<<"&Track";

            colori.clear();
            colori << "black" << "red" << "black" << "black" << "black";
            m_aflwdatas.at(i+1)->getSirokyAve()->setColors(colori);
            m_aflwdatas.at(i+1)->getSirokyAve()->setTracce(tracce);

            //bande colorate
            linee.clear();
            linee.append(4);
            linee.append(1);
            m_aflwdatas.at(i+1)->getSirokyAve()->setLinea(linee);
        }
    }
    qDebug() << "Fine";
    return true;
}

int MDataManager::ReadResult(int & __numEv)
{
    int StructType = m_mng->GetAnaType();
    if (StructType <= 0)
        //error
        return -1;

   __numEv = m_mng->readNumEv();

    m_aflwdatas.clear();

    switch (StructType)
    {
    case FLW_AVD_STUDY: {
        //dati analisi
        m_aflwdatas.append(new mflowdatas());
        unsigned char * strTemp = (unsigned char *) malloc (sizeof(FLWAdvRepStruct));
        m_aflwdatas.at(0)->setParent(this);
        m_aflwdatas.at(0)->setAutoFlow(m_autoFlow == 0);
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

        for (int i = 1; i <= __numEv; i++) {
            m_aflwdatas.append(new mflowdatas());
            m_aflwdatas.last()->setParent(this);
            m_mng->readResAna(sizeof(FLWAdvRepStruct),strTemp);
            FLWAdvRepStruct* structureFlow = (FLWAdvRepStruct*)strTemp;
            m_aflwdatas.last()->setParent(this);
            m_aflwdatas.last()->setWaitingTime(structureFlow->waiting_time);
            m_aflwdatas.last()->setQMax(structureFlow->q_max);
            m_aflwdatas.last()->setQAve(structureFlow->q_ave);
            m_aflwdatas.last()->setTimeAtV3(structureFlow->time_at_v3);
            m_aflwdatas.last()->setTimeAtV2(structureFlow->time_at_v2 );
            m_aflwdatas.last()->setTimeAtQmax(structureFlow->time_at_qmax);
            m_aflwdatas.last()->setTime90(structureFlow->time_90);
            m_aflwdatas.last()->setFlowTime(structureFlow->flow_time);
            m_aflwdatas.last()->setDescTime(structureFlow->desc_time);
            m_aflwdatas.last()->setVoidingTime(structureFlow->voiding_time);
            m_aflwdatas.last()->setVolAtQqmax(structureFlow->vol_at_qmax);
            m_aflwdatas.last()->setVoidedVolume(structureFlow->voided_volume);
            m_aflwdatas.last()->setAcceleration(structureFlow->acceleration);
            m_aflwdatas.last()->setResidualVolume(structureFlow->residual_volume);
            m_aflwdatas.last()->setVDetMax(structureFlow->v_det_max);
            m_aflwdatas.last()->setCQ(structureFlow->cQ);
            m_aflwdatas.last()->setAutoFlow(m_autoFlow == 0);
            m_aflwdatas.last()->buildTable();
            m_aflwdatas.last()->buildNomogrammi(m_sexPatient, m_etaPatient);
        }

        //calcolo la media
        for (int i = 1; i <= __numEv; i++) {
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
        m_aflwdatas.at(0)->setWaitingTime(m_aflwdatas.at(0)->getWaitingTime()/__numEv);
        m_aflwdatas.at(0)->setQMax(m_aflwdatas.at(0)->getQMax()/__numEv);
        m_aflwdatas.at(0)->setQAve(m_aflwdatas.at(0)->getQAve()/__numEv);
        m_aflwdatas.at(0)->setTimeAtV3(m_aflwdatas.at(0)->getTimeAtV3()/__numEv);
        m_aflwdatas.at(0)->setTimeAtV2(m_aflwdatas.at(0)->getTimeAtV2()/__numEv);
        m_aflwdatas.at(0)->setTime90(m_aflwdatas.at(0)->getTime90()/__numEv);
        m_aflwdatas.at(0)->setTimeAtQmax(m_aflwdatas.at(0)->getTimeAtQmax()/__numEv);
        m_aflwdatas.at(0)->setFlowTime(m_aflwdatas.at(0)->getFlowTime()/__numEv);
        m_aflwdatas.at(0)->setDescTime(m_aflwdatas.at(0)->getDescTime()/__numEv);
        m_aflwdatas.at(0)->setVoidingTime(m_aflwdatas.at(0)->getVoidingTime()/__numEv);
        m_aflwdatas.at(0)->setVolAtQqmax(m_aflwdatas.at(0)->getVolAtQqmax()/__numEv);
        m_aflwdatas.at(0)->setVoidedVolume(m_aflwdatas.at(0)->getVoidedVolume()/__numEv);
        m_aflwdatas.at(0)->setAcceleration(m_aflwdatas.at(0)->getAcceleration()/__numEv);
        m_aflwdatas.at(0)->setResidualVolume(m_aflwdatas.at(0)->getResidualVolume()/__numEv);
        m_aflwdatas.at(0)->setVDetMax(m_aflwdatas.at(0)->getVDetMax()/__numEv);
        m_aflwdatas.at(0)->setCQ(m_aflwdatas.at(0)->getCQ()/__numEv);

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
    return m_aflwdatas.at(__i);
}


