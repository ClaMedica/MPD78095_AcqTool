#include "mabstractmanager.h"
#include "QApplication"

MAbstractManager::MAbstractManager(QObject *parent) : QObject(parent)
{
    m_applicationPath = QApplication::applicationDirPath();
    // Stringhe necessarie per la traduzione delle descrizioni dei definitori e markers caricati
    //dai file xml
    trMap["Flowmetry"] = tr("Flowmetry");
    trMap["Cough"] = tr("Cough");
    trMap["Speak"] = tr("Speak");
    trMap["definer"] = tr("definer");
    trMap["marker"] = tr("marker");


}

MAbstractManager::~MAbstractManager()
{
}


QString MAbstractManager::translate(QString __string)
{
    if(trMap.contains(__string))
        return trMap[__string];
    else
        return __string;
}


QVariantList MAbstractManager::markersInfo(QString __filterType,
                                           QVariantList __filterValues)
{
    //leggiamo il file di configurazione e riempiamo le info
    QVariantList list;
    foreach(VarMap marker, m_markerMap.values()) {
        if(__filterType != "")
            if(!__filterValues.contains(marker.value(__filterType)))
                continue;

        int mrk = marker.value(ATT_KEY).toInt();
        switch (mrk)
        {
        case MK_INVITATION:
            if (m_analysisMap.contains(PFS_STD_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_FIRSTDESIRE:
            if (m_analysisMap.contains(CYS_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_STRONGDESIRE:
            if (m_analysisMap.contains(CYS_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_MAXCYSCAPACITY:
            if (m_analysisMap.contains(CYS_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_COMP1:
            if (m_analysisMap.contains(CYS_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_VALSALVALEAK:
            if (m_analysisMap.contains(LPP_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_DETRUSORLEAK:
            if (m_analysisMap.contains(LPP_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list<<"&GridElement";
            }
            break;

        case MK_MIN_ABD_P:
            if (m_analysisMap.contains(LPP_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_COUGHT:
            if (m_analysisMap.contains(LPP_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_LPD:
            if (m_analysisMap.contains(LPP_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_INSTABILITY:
            if (m_analysisMap.contains(DO_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

            //TO DO: a quale esame associamo il marker operativo ff??
            //            case MK_FREEFLOW:
            //                    if (m_analysisMap.contains(PFS_STD_STUDY))
            //                    {
            //                        list<<"$GridElement";
            //                        foreach(QString key,marker.keys())
            //                        {
            //                            list<<key;
            //                            list<<marker.value(key);
            //                        }
            //                        list<<"&GridElement";
            //                    }
            //                break;

        case MK_STARTSP:
            if (m_analysisMap.contains(UPP_STA_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        case MK_STARTDP:
            if (m_analysisMap.contains(UPP_DYN_STUDY)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    list << marker.value(key);
                }
                list << "&GridElement";
            }
            break;

        default:
            break;

        }

        if (marker.value(__filterType) == "6") {
            list << "$GridElement";
            foreach(QString key, marker.keys()) {
                list << key;
                if (key == "descr")
                    list << translate(marker.value(key).toString());
                else
                    list << marker.value(key);
            }
            list << "&GridElement";
        }
    }

    return list;
}

QVariantList MAbstractManager::definersInfo()
{
    //leggiamo il file di configurazione e riempiamo le info
    QVariantList list;

    foreach(VarMap marker, m_markerMap.values()) {
        if(marker.value(ATT_TYPE)!="5") //no definer
            continue;

        foreach (int key, m_analysisMap.keys())
            if (marker.value(ATT_KEY) == m_analysisMap.value(key)) {
                list << "$GridElement";
                foreach(QString key, marker.keys()) {
                    list << key;
                    if (key == "descr")
                        list << translate(marker.value(key).toString());
                    else
                        list << marker.value(key);
                }
                list << "&GridElement";
            }
    }

    return list;
}

QVariantList MAbstractManager::actionsInfo()
{
    //leggiamo il file di configurazione e riempiamo le info
    QVariantList list;

//    //aggiungo pulsante cancella markers analitici

   list << "$GridElement";
   list << "descr" << tr("delete analytical markers");
   list << "img" << "qrc:/danger";
   list << "key" << 116;
   list << "visible" << true;
   list << "&GridElement";

    return list;
}


QVariantList MAbstractManager::commandsInfo()
{
    QVariantList list;

    //analisi
    list << "$GridElement";
    list << "descr" << tr("analysis");
    list << "img" << "qrc:/analisi";
    list << "key" << 111;
    list << "visible" << true;
    list << "&GridElement";

    //spazio vuoto
    list << "$GridElement";
    list << "descr" << "";
    list << "img" << "";
    list << "key" << 0;
    list << "visible" << false;
    list << "&GridElement";

    //zoom in
    list << "$GridElement";
    list << "descr" << tr("zoom in");
    list << "img" << "qrc:/zoom_in";
    list << "key" << 113;
    list << "visible" << true;
    list << "&GridElement";

    //zoom out
    list << "$GridElement";
    list << "descr" << tr("zoom out");
    list << "img" << "qrc:/zoom_out";
    list << "key" << 114;
    list << "visible" << true;
    list << "&GridElement";

    //zoom none
    list << "$GridElement";
    list << "descr" << tr("zoom none");
    list << "img" << "qrc:/zoom_none";
    list << "key" << 115;
    list << "visible" << true;
    list << "&GridElement";

#ifdef PICOFLOW
    for (int i=0; i<2; i++)
    {
        //spazio vuoto
        list << "$GridElement";
        list << "descr" << "";
        list << "img" << "";
        list << "key" << 0;
        list << "visible" << false;
        list << "&GridElement";
    }

    //salva e chiudi
    list << "$GridElement";
    list << "descr" << tr("save&exit");
    list << "img" << "qrc:/salvaChiudi";
    list << "key" << 112;
    list << "visible" << true;
    list << "&GridElement";
#endif
    return list;
}

QVariantList MAbstractManager::commandsInfoBottom()
{
    QVariantList list;
#ifndef PICOFLOW
    //salva e chiudi
    list << "$GridElement";
    list << "descr" << tr("save&exit");
    list << "img" << "qrc:/salvaChiudi";
    list << "key" << 112;
    list << "visible" << true;
    list << "&GridElement";
#endif
    return list;
}

QVariantList MAbstractManager::acqInfo()
{
    QVariantList list;

    //esci senza salvare
    list << "$GridElement";
    list << "descr" << tr("discard");
    list << "img" << "qrc:/discard";
    list << "key" << 116;
    list << "visible" << true;
    list << "&GridElement";

    return list;
}

bool MAbstractManager::load()
{
    //in questa funzione inizializzo tutto caricando i file di configurazione fissi
    m_configLocale.erase();
    if(!m_configLocale.loadFromXML(":/Config/Config_Locale.xml"))
        qCritical() << "Error on locale configuration file";

    QString configUser;
#ifdef PICOFLOW
    configUser = g_P7SettingsManager.userSettings();
    if(!QFile::exists(configUser))
        configUser = ":/Config/Config_User_pico.xml";
    m_configUser.erase();
    if(!m_configUser.loadFromXML(configUser))
        qCritical() << "Error on user configuration file";
    if(!m_configUserProp.loadFromXML(configUser))
        qCritical() << "Error on user configuration file";
#else
    QString configUserProp;
    configUserProp = g_P7SettingsManager.userPropSettings();
    if(!QFile::exists(configUserProp))
        configUserProp = ":/Config/Config_User_Prop.xml";
    if(!m_configUserProp.loadFromXML(configUserProp))
        qCritical() << "Error on user configuration file";

    configUser = g_P7SettingsManager.userSettings();
    if(!QFile::exists(configUser))
        configUser = ":/Config/Config_User.xml";
    if(!m_configUser.loadFromXML(configUser))
        qCritical() << "Error on user configuration file";
#endif    

    m_configMarkers.erase();
    if(!m_configMarkers.loadFromXML(":/Config/markers.xml"))
        qCritical() << "Error on user configuration file";

    QString configPrinter = g_P7SettingsManager.printerSettings();
    if(!QFile::exists(configPrinter))
        configPrinter = ":/Config/Config_Printer.xml";
    m_configPrinter.erase();
    if(!m_configPrinter.loadFromXML(configPrinter))
        qCritical() << "Error on user configuration file";

    QString configLanguage = g_P7SettingsManager.languageSettings();
    if(!QFile::exists(configLanguage))
        configLanguage = ":/Config/Config_Language.xml";
    m_configLang.erase();
    if(!m_configLang.loadFromXML(configLanguage))
        qCritical() << "Error on user configuration file";


    bool res = buildMarkerInfoMap();
    return res;
}

QString MAbstractManager::plotConfigFileName()
{
#ifdef ANDROID
    return "/mnt/sdcard/Medica/cur.xml";
#else
    return m_applicationPath + "/cur.xml";
#endif
}

bool MAbstractManager::buildConfigurationFile()
{
    //carico i dati necessari
    int32_t chanlNum = m_mng->GetChanNum();
    qDebug()<<"N? Canali: "<<chanlNum;

    m_chanInPlots.clear();
    m_dataChanNameMap.clear();

    if (m_tcpChannels.size() > 0) {
        foreach (SimpleTCPChannel * cur, m_tcpChannels.values()) {
            delete cur;
        }
        m_tcpChannels.clear();
    }


    if(chanlNum == 0)
        qCritical() << "No channels in file";

    for(int32_t nc = 0; nc < chanlNum; nc++) {  //contiamo i grafici e popoliamo le mappe di associazione
        QString chanName = m_mng->GetChanName(nc);

        //per i grafici devo appendere l'informazione perche posso avere piu canali
        QString graphName = "Graph_" + QString::number(m_mng->GetGraph(nc));
        m_chanInPlots[graphName] << chanName;

        //aggiungo il canale su cui comunichera  questo plot
        m_tcpChannels[graphName] = new SimpleTCPChannel(QHostAddress("127.0.0.1"), 9000 + m_mng->GetGraph(nc) - 1, this);
        qDebug() << nc << chanName << "inviato su" << graphName << 9000 + m_mng->GetGraph(nc) - 1;

        //per il datafile invece no
        m_dataChanNameMap[chanName] = nc;
    }

    //scriviamo un file di configurazione che poi l'utente potra modificare a suo gusto
    Ancestry configPlot;

    //aggiungo il campo graphs
    Ancestry *graph = configPlot.addChild(XML_GRAPHS);
    if(graph == NULL)
        qCritical() << "Could not create child";

    foreach (QString graphName, m_chanInPlots.keys()) { //scorro per ogni grafico
        Ancestry *  graphN = graph->addChild(graphName);
        Ancestry *  prop = graphN->addChild(XML_PROPERTIES);
        Ancestry *  tracks = graphN->addChild(XML_TRACKS);

        //ho la certezza che i canali su ogni grafico hanno tutti le stesse proprietA  grafiche per cui vado tranquillo
        QString chanName = m_chanInPlots[graphName].first();
        int32_t nc = m_dataChanNameMap[chanName];
        Ancestry *  axis    = prop->addChild(XML_AXIS);
        Ancestry *  time    = prop->addChild(XML_TIME);
        Ancestry *  network = prop->addChild(XML_NETWORK);

        Ancestry m_configAcq;
        if(!m_configAcq.loadFromXML(g_P7SettingsManager.progPath() + "/Config_Acq.xml"))
            qCritical() << "Error on acq configuration file";

        Ancestry *channels = m_configAcq.getChild(XML_CHANNELS);
        if(channels == NULL)
            qCritical("Child not alive");

        QString udm = "";
        foreach (Ancestry *channel, channels->getChildren()) {
            if(channel->getChild(XML_NAME) != NULL)
                if(chanName.contains(channel->getTextOfChild(XML_NAME).left(1))) {
                    QString udmID = (channel->getChild(XML_UDM))->getAttribute("ID");
                    UDMConst c = (UDMConst) udmID.toInt();
                    udm = UDMUtil.toString(c);
                    break;
                }
        }

        axis->setAttribute("yAUOM", udm);
        axis->setAttribute("yAbsoluteMax", QString::number(m_mng->GetSupLim(nc)));
        axis->setAttribute("yAbsoluteMin", QString::number(m_mng->GetInfLim(nc)));

        time->setAttribute("samplingFrq", QString::number(m_mng->GetNAS(nc)));
        time->setAttribute("pageTime", QString::number(m_mng->GetPageTime()));
        network->setAttribute(ATT_PORT, QString::number(9000+m_mng->GetGraph(nc)-1));
        network->setAttribute(ATT_ADDRESS, "127.0.0.1");

        //devo leggere le informazioni sulle dimensioni delle tracce e il loro colore
        //all'interno del file config_user
        Ancestry * chProps = m_configUserProp.getSafeChild(XML_CHANNELSPROP);
        QStringList chNames = chProps->childrenNames();

        foreach (QString chanName, m_chanInPlots[graphName]) {  //qui scrivo le proprietA  delle tracce
            Ancestry * trkN = tracks->addChild(chanName);
            Ancestry * ch = NULL;
            foreach (QString chNamePart, chNames) {
                if(chanName.contains(chNamePart))
                    ch = m_configUserProp.getSafeChild(chNamePart);
            }

            if(ch != NULL) {
                trkN->setAttribute(ATT_THICK,      ch->getSafeChild(ATT_THICK)->getSafeAttribute(ATT_VALUE));
                trkN->setAttribute(ATT_COLOR,      ch->getSafeChild(ATT_COLOR)->getSafeAttribute(ATT_VALUE));
                axis->setAttribute(ATT_YAUTOSCALE, ch->getSafeChild(ATT_YAUTOSCALE)->getSafeAttribute(ATT_VALUE));
                axis->setAttribute(ATT_RANGE, ch->getSafeChild(ATT_RANGE)->getSafeAttribute(ATT_MODEL));
                axis->setAttribute(ATT_RANGEVALUE, ch->getSafeChild(ATT_RANGE)->getSafeAttribute(ATT_VALUE));
            }
            else
                qWarning() << "Canale" << chanName << "senza proprieta grafiche. Uso le default";
      }
    }

    //ora salvo il file di configurazione come cur.xml
#ifdef ANDROID
    configPlot.saveToXML("/mnt/sdcard/Medica/cur.xml");
#else
    configPlot.saveToXML(m_applicationPath+"/cur.xml");
#endif

    qDebug() << "Configuration file builded succesfully";
    return true;
}

bool MAbstractManager::buildMarkerInfoMap()
{
    m_markerMap.clear();

    foreach(Ancestry *marker, m_configMarkers.getChildren()) {
        VarMap mark;
        QString img = marker->getAttribute(ATT_IMG);
        QString rootURL = "qrc:/";
        mark[ATT_IMG] = rootURL+img;
        mark[ATT_KEY] = marker->getAttribute(ATT_KEY);
        mark[ATT_CODE] = marker->getAttribute(ATT_CODE);
        mark[ATT_DESCR] = marker->getAttribute(ATT_DESCR);
        mark[ATT_TYPE] = marker->getAttribute(ATT_TYPE);
        mark["lock"]     = false;
        mark["color"]    = COLOR_OPERATIVE;
        mark["visible"]  = true;
        mark["category"] = CAT_MARKER;
        m_markerMap[mark[ATT_KEY]] = mark;
    }
    qDebug() << m_markerMap.size() << "markers loaded!";

    return true;
}
