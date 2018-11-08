#include "macqmanager.h"

MAcqManager::MAcqManager(QObject *parent)
{
    (void) parent;

    m_mng = NULL;
    m_acqFileOpened = false;    //nessuna acquisizione in atto
    m_sendingToPlot = false;    //nessuno sta spedendo qualcosa per cui ci si puo scrivere sopra
    m_serverReady = false;      //i server non sono inizializzati quindi falso
    m_autoStartStop = false;
    m_saving = false;           //non sto salvando i dati
    m_supeConnected = 0;
    m_oldState = 0;
    m_itsok = "              &";
    OutFile = NULL;
    m_acquired = false;

    //media mobile flusso e volume
    m_lenMMobile = 10;
    m_sommaMMobileF = 0;
    m_sommaMMobileV = 0;

    //filtro digitale
    m_lenDifFilter = 6;
    m_sommaCoef = 0;
    for (int i=0; i<m_lenDifFilter;i++)
       m_sommaCoef += COEFDigFilter[i];

    m_valPrecVolume = -1;


    m_startAcqManuale = false; //non ancora premuto tasto start
#ifdef PICOFLOW

#endif

    if(!m_configAcq.loadFromXML(g_P7SettingsManager.progPath() + "/Config_Acq.xml"))
        qCritical() << "Error on acq configuration file";
    qDebug("qui");
    //carico info di connettivitA
    loadConnectivityInfo(m_configAcq.getSafeChild(XML_CONNECTIONS));
    qDebug("qui");
//    QString lang = m_configLocale.getChild(XML_LOCALE)->getSafeAttribute("value");
//    qDebug("qui");
//    QString configAlarms = m_applicationPath + "/Config_Alarms_" + lang + ".xml";
//    qDebug("qui");
//    if(!QFile::exists(configAlarms))
//        configAlarms = ":/Config/Config_Alarms_"+ lang + ".xml";
//    qDebug("qui");
//    if(!m_alarmMng.load(configAlarms))
//        qCritical() << "Error on alarm configuration file";
    qDebug("tutto");

    QTimer::singleShot(2000, this, SLOT(connectToServers()));

    //connetto il gestore degli allarmi alla proprietA  alarms
    connect(&m_alarmMng, SIGNAL(alarmsUpdated(QVariantList)), this, SLOT(setAlarms(QVariantList)));
}

MAcqManager::~MAcqManager()
{
    if(m_mng != NULL) {
        m_mng->Close();
        delete m_mng;
        m_mng = NULL;
    }

    if(m_tcpClients.values().size() > 0) {
        foreach (SimpleTCPClient * cur, m_tcpClients.values()) {
            delete cur;
        }
    }

    if(m_tcpChannels.size() > 0) {
        foreach (SimpleTCPChannel * cur, m_tcpChannels.values()) {
            delete cur;
        }
    }


    foreach (QList<MSignal *> list, m_channelMap) {
        foreach(MSignal *sig,list) {
            delete sig;
        }
    }


    //    for(int i=0;i<m_signalVector.size();i++)
    //        if(m_signalVector[i]!=NULL)
    //            delete m_signalVector[i];
}

void MAcqManager::dataOnTCP(QObject *__pParent, SimpleTCPClient *__pTCP, QByteArray __block)
{//arriviamo qua dentro ogni volta che arriva qualcosa da uno dei server a cui siamo collegati

    qDebug() << __pTCP->hostAddress() << __pTCP->hostPort() << __block;
    if(__pParent != NULL) {     //punta a qualcosa andiamo avanti
        if(__pTCP != NULL) {    //punta a qualcosa proviamo a gestirlo
            ((MAcqManager *) __pParent)->handleTCP(__pTCP, __block);
        }
    }
}

bool MAcqManager::newAcquisition(QString __dataFile)
{
    if(m_acqFileOpened) {
        qCritical() << "acquisizione in corso";   //sono gia' in acquisizione e voglio farne partire un altra
    }
    else
    {
        qDebug() << "carico la configurazione per l'acquisizione";
        qDebug() << g_P7SettingsManager.progPath();
        if(!m_configAcq.loadFromXML(g_P7SettingsManager.progPath() + "/Config_Acq.xml"))
            qCritical() << "Error on acq configuration file";

        //e infine carico il file degli allarmi con la lingua giusta
        QString lang = m_configLocale.getChild(XML_LOCALE)->getSafeAttribute("value");
        QString configAlarms = m_applicationPath + "/Config_Alarms_" + lang + ".xml";
        if(!QFile::exists(configAlarms))
            configAlarms = ":/Config/Config_Alarms_"+ lang + ".xml";
        if(!m_alarmMng.load(configAlarms))
            qCritical() << "Error on alarm configuration file";

        //carico info di connettivitA
        loadConnectivityInfo(m_configAcq.getSafeChild(XML_CONNECTIONS));

        //resetto le QMap se non sono pulite
        m_bufferMap.clear();
        m_operationMap.clear();
        m_HWChansMap.clear();
        m_acqMarker.clear();
        m_sampleFreqMap.clear();
        m_frameMap.clear();
        m_bufSizeMap.clear();

        //inizializzazione media mobile e filtro digitale per flusso
        m_valPrecVolume = 0;
        m_sommaMMobileF = 0;
        m_sommaMMobileV = 0;
        m_buffer_MMobileV.clear();
        m_buffer_MMobileF.clear();
        for (int i=0; i<m_lenMMobile;i++){
            m_buffer_MMobileV.append(0);
            m_buffer_MMobileF.append(0);
        }
        m_buffer_DigFilter.clear();
        for (int i=0; i<m_lenDifFilter;i++)
            m_buffer_DigFilter.append(0);


        /* non sono in acquisizione e quindi posso lanciarne una nuova aprendo il file e leggendo le info
         * oppure pescandole dal file di configurazione
        */
        if(m_mng != NULL) {     //se c'e' qualcosa di vecchio lo chiudo
            delete m_mng;
            m_mng = NULL;
        }

        //controllo l'esistenza dei file

        if(!QFile::exists(__dataFile))
            qCritical() << "File does not exists";

        //gestione esami interrotti
        //creo il file out_file nel quale va scritto l'ok se tutto va a buon fine
        //altrimenti c'è il nome del file interrotto
        QString outF = "out_file";
        OutFile = new QFile(g_P7SettingsManager.progPath() + "/" + outF);

        //Create the out_file new
        if (QDir(g_P7SettingsManager.progPath() ).exists(outF))
            QDir(g_P7SettingsManager.progPath()).remove(outF);

        OutFile->open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream* out = new QTextStream(OutFile);
        *out << __dataFile;
        OutFile->close();
#ifdef PICOFLOW
        system("sync");
#endif

        m_mng = new DatafileManager;

        m_mng->SetFileName(__dataFile);
        m_mng->SetFileType(7);

        qDebug() << "Opening file ... " << m_mng->Open();
        qDebug() << "Loading parameters ... " << m_mng->GetParameters();

        m_acqFileOpened = true;     //mi segno che ho aperto il file

        m_acqFinished = false;

        //popoliamo la lista dei canali prenotati

        m_channelNames.clear();

        for(int i = 0; i < m_mng->GetChanNum(); i++)
            m_channelNames.append(m_mng->GetChanName(i));

        qDebug() << "Canali presenti nell'esame" << m_channelNames;
        qDebug() << "Reading configuration file";
        if(!readConfigurationFile())
            qCritical() << "Error during reading of configuration file";

        //bene nel file di configurazione c'e scritto tutto il massimo potenziale della scheda
        //tuttavia non e detto che ci serva tutto per cui
        qDebug() << "Building configuration file for plots";
        if(!buildConfigurationFile())
            qCritical() << "Error during building of configuration file";

        //aggiorno il datafile con i dati relativi alla mia configurazione #BUG da togliere non appena il file verrA  scritto correttamente
        qDebug() << "Updating datafile...";
        handleDataFile();

        //ripristino il file in acquisizione
        bool res = m_mng->Continue();
        qDebug() << "Continue ..." << res;

        //dico a medica di salvare il file nel db
        g_mainAppBridge->sendSave();

        //inizializzo i server di comunicazione con i plotter
        initializeServers();

        //disabilito alcuni allarmi
        m_alarmMng.manageAlarm(ALA_NOT_ACQUIRING, DISABLE);

        //mi connetto ai server del supe e del programma di gestione archivi
        connectToServers();
    }
    //faccio partire il timer per l'allarme di stato
    //m_alarmMng.startTimeoutAlarm();

    return true;
}

void MAcqManager::connectToServers()
{
    //mi connetto ai server
    qDebug() << "Connecting to servers ..." << m_tcpClients.keys();
    foreach (SimpleTCPClient *client, m_tcpClients) {
        client->registerDataReadyCallBack(&(this->dataOnTCP));
        client->connectToHost();
    }

    if(m_supeConnected == false) {
        qDebug() << "Retrying to connect in 2 seconds...";
        QTimer::singleShot(2000, this, SLOT(connectToServers()));
    }
}


void MAcqManager::endAcquisitionSave()
{
    endAcquisition();
    g_mainAppBridge->sendOpen();
}

void MAcqManager::endAcquisitionDiscard()
{
    endAcquisition(true);
    g_mainAppBridge->sendDiscard();
}

void MAcqManager::endAcquisition(bool discard)
{
    //disabilito gli allarmi
    m_alarmMng.disableAll();

    //interrompo la connessione
    sendStopAcq();

    foreach (SimpleTCPClient *client, m_tcpClients) {
        //mi disconnetto dal supe
        if(client->disconnectToHost())
            qDebug() << "disconnect "
                     << client->hostAddress().toString()
                     << client->hostPort();
    }

    qDebug() << "stato:" << m_mng->GetState();

    if(m_acqFileOpened)    //se siamo in acq facciamo un commit
    {
        qDebug() << m_mng->GetFileName() << m_mng->GetFileType() << m_mng->GetChanNum() ;
        bool cvres = m_mng->CommitValues();
        qDebug() << "Commit Values?" << cvres;
    }

    bool ret = m_mng->Close();
    qDebug() << "File closed?" << ret;

    //write in out_file the OK string
    OutFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream* out = new QTextStream(OutFile);
    *out << m_itsok;
    OutFile->close();

    if (discard)  //devo cancellare il file
    {
        qDebug() << QFile::remove(m_mng->GetFileName());
        //devo resettare il parametro di flusso automatico a false per non far partire sempre l'analisi in automatico all'apertura in review di un file
        Ancestry *autoflow = m_configUser.getSafeChild("AutomaticFlow");
        Ancestry *child = autoflow->getSafeChild("Auto");
        QString valueAuto = child->getAttribute("value");
        if (valueAuto == "true")
        {
            child->setAttribute("value","false");
            QString configUser = g_P7SettingsManager.userSettings();
            m_configUser.saveToXML(configUser);
        }
    }

    m_acqFileOpened = false;    //nessuna acquisizione in atto
    m_sendingToPlot = false;    //nessuno sta spedendo qualcosa per cui ci si puo scrivere sopra
    m_serverReady = false;      //i server non sono inizializzati quindi falso
    m_autoStartStop = false;
    m_saving = false;           //non sto salvando i dati
    m_supeConnected = 0;
    m_oldState = ESTATE_IDLE_NOT_CONNECTED;
    emit acquisitionEnded();
}

void MAcqManager::addMarker(QVariant __key)
{
    if(m_acqFileOpened) {
        VarMap mrk;
        mrk = m_markerMap[__key];

        m_mng->AppendOpMarker(mrk[ATT_KEY].toUInt(), mrk[ATT_DESCR].toString());

        mrk["val"] = (float) m_mng->GetSamplesNumber(0) / m_mng->GetNAS(0);
        m_acqMarker.append(mrk);
        updateAcqData();

        qDebug() << "Marker key:" << mrk["key"] << "appended at" << mrk["val"].toString() << "sec";
    }
}

void MAcqManager::addDefiner(bool __startEnd, QVariantList __info)
{
    (void) __startEnd;
    (void) __info;
    //  #BUG    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

bool MAcqManager::sendStartAcq()
{
    return sendCommand(ETCP_CMD_START_WITH_ZERO /*ETCP_CMD_START_WITH_ZERO*/);
}

bool MAcqManager::sendStopAcq()
{
    m_alarmMng.manageAlarm(ALA_NOT_ACQUIRING, DISABLE);
    return sendCommand(ETCP_CMD_STOP);
}

void MAcqManager::resetAlarms()
{
    m_alarmMng.resetAlarms();
}

void MAcqManager::setAlarms(QVariantList __list)
{
    if(m_alarmList != __list) {
        m_alarmList = __list;
        emit alarmsChanged();
    }
}

bool MAcqManager::sendCommand(tcp_flow_bt_cmd_t __command)
{
    if(m_tcpClients.contains("CMD")) {
        quint8 c = (quint8) __command;
        m_tcpClients["CMD"]->sendData((char *) &c, sizeof(quint8));
        qDebug() << "Sending command: " << __command;
        return true;
    }
    else {
        qCritical() << "No CMD channel loaded";
        return false;
    }
}

void MAcqManager::updateAcqData()
{
    QStringList plotNames = m_chanInPlots.keys();
    QString type = "Marker";
    QString sGroup = "$" + type + "Group";
    QString eGroup = "&" + type + "Group";
    QString sType = "$" + type;
    QString eType = "&" + type;

    foreach(QString currentPlot, plotNames) {
        m_acqMarkerList << sGroup;
        m_acqMarkerList << currentPlot;

        foreach (VarMap curMap, m_acqMarker) {
            m_acqMarkerList << sType;
            foreach (QString curRole, curMap.keys()) {
                m_acqMarkerList << curRole;
                m_acqMarkerList << curMap.value(curRole);
            }
            m_acqMarkerList << eType;
        }
        m_acqMarkerList << eGroup;
    }
    //qDebug()<<m_acqMarkerList;
    emit acqMarkersChanged();
}

void MAcqManager::handleTCP(SimpleTCPClient *__client, QByteArray __block)
{
    if(m_tcpClients.values().contains(__client)) {
        QString who = m_tcpClients.key(__client);
        //qDebug() << who ;//<< __block;

        if(who == "STA") {      //allora e' uno stato
            m_supeConnected = true;
            __block.remove(0, 4);
            //#BUG non e' in gradi di interpretare lo stato del picoflow(cavo+emg) ma solo quello della cellaBT
            // vedere picoFlow_status_t e flowBT_status_t
            flowBT_status_t stBT;
            picoFlow_status_t stPico;
            alarms_t alarms;
            union {
                flowBT_states_t bt;
                picoFlow_states_t pf;
            } currState;
            uint8_t newState = 0;
            bool isBT;
            uint i = 0;
            qDebug("blk.sz:%d sz(alarms_t):%d sz(flowBT_status_t):%d sz(picoFlow_status_t):%d",__block.size(), sizeof(alarms_t),sizeof(flowBT_status_t),sizeof(picoFlow_status_t));
            if((__block.size() - sizeof(alarms_t)) == sizeof(flowBT_status_t)) {
                qDebug(" --> currState.bt = stBT.currState");
                qint8  * d = (qint8 *) &stBT;
                for(i = 0; i < sizeof(flowBT_status_t); i++)
                    *d++ = __block.at(i);
//                currState.bt = stBT.currState;
                newState = stBT.currState;
                isBT = true;
            }
            else {
                qDebug(" --> currState.pf = stPico.currState");
                qint8  * d = (qint8 *) &stPico;
                for(i = 0; i < sizeof(picoFlow_status_t); i++)
                    *d++ = __block.at(i);
//                currState.pf = stPico.currState;
                newState = stPico.currState;
                isBT = false;
            }
//            for(i = 0; i < sizeof(flowBT_status_t); i++)
//                ((qint8 *) (& status) )[i] = __block[i];
            i++;
            for(uint j = i; j < sizeof(alarms_t) + i; j++)
                ((qint8 *) (& alarms))[j-i] = __block[j];

            analyzeStatus(newState, isBT);
            analyzeAlarms(alarms);
            qDebug() << "Supervisore connesso"<<sizeof(flowBT_status_t)<<sizeof(picoFlow_status_t)<<sizeof(alarms_t)<<__block.size();
        }
        else if(who == "VAL")
        {
            if (m_autoStartStop || m_startAcqManuale) {
                // riempo i buffer
                fillBuffers(__block);

                // finche' i buffer hanno abbastanza campioni
                // faccio le mie operazioni e rimuovo i primi campioni
                while(buffersReady()) {
                    qDebug() << "BUFFER ready";
                    applyOperations();
                    foreach(QString hwc, m_totalHWChan)
                        m_bufferMap[hwc]->remove(0, m_frameMap[hwc]);
                }

                qDebug() << "m_saving:" << m_saving << "m_autoStartStop:" << m_autoStartStop;
                if(m_autoStartStop) {
                    if(!m_saving)
                        checkAutomaticStartStop("Start");   //finche' non devo salvare riempo il buffer e controllo
                    else {
                        checkAutomaticStartStop("Stop");
                        if(!m_acqFinished) {
                            sendBuffersToPlot();
                            saveBuffersToFile();
                        }
                    }
                }
                else {
                    if(m_saving && !m_acqFinished) {
                        sendBuffersToPlot();
                        saveBuffersToFile();
                    }
                }
            }
        }
        else if(who == "CMD")
        {
            if(__block[4] == '5') {
                if(!m_saving) {
                    //parte immediatamnte l'acquisizione
                    //azzero
                    sendStartAcq();
                    m_startAcqManuale = true;
                    int secToSave = 0.0;
                    //in caso di flussimetria manuale non devo tenermi buffer di dati:
                    //i dati salvati partono dal momento dello start acquisizione da parte dell'utente
                    if (m_autoStartStop)  //effetto buffer tengo solo gli ultimi 5 secondi
                        secToSave = 5.0;
                    foreach(QString type, m_channelMap.keys())
                        foreach(MSignal *sig, m_channelMap[type])
                            sig->saveLastSec(secToSave);

                    m_saving = true;    //posso iniziare a salvare i dati
                    emit acquisitionStarted();
                    qDebug() << "Start acquiring";
                    m_acqFinished = false;
                }
                else {
                    //ferma immediatamente l'acquisizione
                    qDebug() << "Stop acquiring";
                    endAcquisitionSave();
                    m_startAcqManuale = false;
                    m_acqFinished = true;
                }
            }
        }
    }
}

void MAcqManager::initializeServers()
{
    //creiamo un nuovo canale con la qmlplotter
    qDebug() << "Creating servers for plots ... " << m_tcpChannels.keys();

    foreach(SimpleTCPChannel *chan, m_tcpChannels.values())
        chan->listen();

    m_serverReady = true;
}

bool MAcqManager::loadConnectivityInfo(Ancestry *__info)
{
    qDebug() << "Loading connectivity info";
    //__info A? sicuro
    Ancestry *tcp = __info->getSafeChild(XML_TCP);

    foreach (Ancestry *child, tcp->getChildren()) {
        QString name = child->name();

        QString address = child->getSafeAttribute(ATT_ADDRESS);
        int port = child->getSafeAttribute(ATT_PORT).toInt();

        if(child->getSafeAttribute(ATT_TYPE) == "client") {
            if( ! m_tcpClients.keys().contains(name))
                m_tcpClients[name] = new SimpleTCPClient(QHostAddress(address), port, this);
        }
        else if(child->getSafeAttribute(ATT_TYPE) == "server") {
            if( ! m_tcpChannels.keys().contains(name))
                m_tcpChannels[name] = new SimpleTCPChannel(QHostAddress(address), port, this);
        }
        else
            qDebug() << "Skip" << name << address << port;
        qDebug() << name << address << port;
    }

    qDebug() << "Loaded clients:" << m_tcpClients.keys();
    qDebug() << "Loaded channels:" << m_tcpChannels.keys();
    return true;
}

void MAcqManager::analyzeStatus(uint8_t __currState, bool __isBT)
{
    static const char * names[] = { "st_IDLE_NOT_CONNECTED", "st_IDLE_CONNECTED", "st_ACQUIRING" };

    // m_alarmMng.stopTimeoutAlarm(ALA_TIMEOUT_STATUS);
    qDebug("olstate:%s new:%s %s", names[m_oldState], names[__currState], __isBT ? "BT" : "Cavo");

    // trasformato in  m_acquired
    // static acquired ok SOLO per la prima volta dall'accensione
//    static bool acquired = false;

//    picoFlow_states_t inputEv = (picoFlow_states_t) __currState;
//    switch(m_oldState)
//    {
//    case ESTATE_IDLE_NOT_CONNECTED:
//        switch(inputEv) {
//                        case ESTATE_IDLE_CONNECTED:
//                                m_alarmMng.stopTimeoutAlarm(ALA_NOT_CONNECTED);
//                                sendStartAcq ();
//                                break;
//                        case ESTATE_ACQUIRING:
//                        case ESTATE_IDLE_NOT_CONNECTED:
//                        default:break;
//        }
//        break;
//    case ESTATE_IDLE_CONNECTED:
//        switch(inputEv) {
//                        case ESTATE_IDLE_NOT_CONNECTED:
//                                m_alarmMng.startTimeoutAlarm(ALA_NOT_CONNECTED, 1000);
//                                break;
//                        case ESTATE_IDLE_CONNECTED:
//                                if(acquired) { acquired = false; sendStartAcq(); } // re-start se il caso
//                                break;
//                        case ESTATE_ACQUIRING:
//                                emit systemInAcqStatus();
//                                acquired = true;
//                                m_alarmMng.manageAlarm(ALA_NOT_ACQUIRING, ENABLE);
//                                break;
//                        default:break;
//        }
//        break;
//    case ESTATE_ACQUIRING:
//        switch(inputEv) {
//                        case ESTATE_IDLE_NOT_CONNECTED:
//                                m_alarmMng.addAlarm(ALA_NOT_CONNECTED);
//                                break;
//                        case ESTATE_IDLE_CONNECTED:
//                                m_alarmMng.addAlarm(ALA_NOT_ACQUIRING);
//                                break;
//                        case ESTATE_ACQUIRING:
//                        default:break;
//        }
//        break;
//    default:break;
//    }

    switch(__currState)
    {
    case ESTATE_IDLE_NOT_CONNECTED:
        if(m_oldState == ESTATE_IDLE_CONNECTED)
            m_alarmMng.startTimeoutAlarm(ALA_NOT_CONNECTED, 1000);
        else if(m_oldState == ESTATE_ACQUIRING)
            m_alarmMng.addAlarm(ALA_NOT_CONNECTED);
        break;

    case ESTATE_IDLE_CONNECTED:
        if(m_oldState == ESTATE_IDLE_NOT_CONNECTED) {
            m_alarmMng.stopTimeoutAlarm(ALA_NOT_CONNECTED);
            sendStartAcq ();
        }
        if(m_oldState == ESTATE_ACQUIRING) {
            m_alarmMng.addAlarm(ALA_NOT_ACQUIRING);
        }
        if(m_oldState == ESTATE_IDLE_CONNECTED && m_acquired) {
            m_acquired = false;
            sendStartAcq();
        }
        break;

    case ESTATE_ACQUIRING:
        if(m_oldState == ESTATE_IDLE_CONNECTED) {
            emit systemInAcqStatus();
            m_acquired = true;
            m_alarmMng.manageAlarm(ALA_NOT_ACQUIRING, ENABLE);
        }
        break;

    default:break;
    }

    m_oldState = __currState;
   // m_alarmMng.startTimeoutAlarm(ALA_TIMEOUT_STATUS, 2000);
}

void MAcqManager::analyzeAlarms(alarms_t __alarms)
{
    qDebug()<<"analyzeAlarm"<<__alarms.ala;
}


bool MAcqManager::checkAutomaticFlow()
{
    //ok controlliamo se c'e' flusso automatico attivo

    Ancestry *autoflow = m_configUser.getSafeChild("AutomaticFlow");
    QString value = autoflow->getSafeChild("Auto")->getSafeAttribute(ATT_VALUE);

    return (value == "true");
}


void MAcqManager::checkAutomaticStartStop(QString __which)
{
    //ok controlliamo se c'e' qualche condizione automatica
    //mi salvo il puntatore al livello acquisition

    //qDebug()<<"Start check"<<__which;

    Ancestry *condition = m_configUser.getSafeChild("Auto" + __which + "s");
   // foreach (Ancestry *condition, contitions->getChildren()) {  //scorro le condizioni di autostart anche se ce ne e' solo una
        //pesco su quale canale e' fatta

        QString chanType = condition->getSafeChild(ATT_CHANTYPE)->getSafeAttribute(ATT_VALUE);
        int num = condition->getSafeChild(ATT_NUM)->getSafeAttribute(ATT_VALUE).toInt() - 1;    //il -1 e' per ovviare al fatto che si parte da 1
        //e in base a come si chiama vedo che farci

        //qDebug()<<"Tipo canale"<<chanType<<num;//condition->name();

        //if(condition->name() == XML_STEP) { //condizione a gradino
        if (__which == "Start")
        {
            Ancestry *childDur = condition->getSafeChild(XML_DURATION);
            Ancestry *childAmp = condition->getSafeChild(XML_AMPLITUDE);

            //per prima cosa controlliamo quanti campioni
            int      minSec = childDur->getSafeChild(ATT_MIN)->getSafeAttribute(ATT_VALUE).toUInt();
            qreal ampMin = childAmp->getSafeChild(ATT_MIN)->getSafeAttribute(ATT_VALUE).toInt();
            qreal ampMax = childAmp->getSafeChild(ATT_MAX)->getSafeAttribute(ATT_VALUE).toInt();

            int min = minSec*m_channelMap[chanType].at(num)->getSamplingFrequency();
            //se non ho ancora abbastanza campioni per decidere non vado avanti
            if (!(m_channelMap[chanType].at(num)->size() < min))
            {
                //ora quindi sono sicuro che arrivo qui solo quando ho abbastanza campioni

                //controllo se c'e' un gradino
                qreal startVal = m_channelMap[chanType].at(num)->first();
                int count = 0;
                foreach(qreal sample, (*m_channelMap[chanType].at(num))) {
                    if((sample-startVal > ampMin) && (sample-startVal < ampMax))
                        count++;
                    else
                        count = 0;
                }

                if(count >= min) {//trigger inizio acq
                    //effetto buffer tengo solo gli ultimi 5 secondi
                    foreach(QString type, m_channelMap.keys())
                        foreach(MSignal *sig, m_channelMap[type])
                            sig->saveLastSec(5.0);

                    m_saving = true;    //posso iniziare a salvare i dati
                    emit acquisitionStarted();
                    qDebug() << "Start acquiring";
                    m_acqFinished = false;
                    return;
                }
                else {
                    //qDebug()<<"Non ho trovato nulla";
                }
            }
        }

        if (__which == "Stop")
        {
            Ancestry *childDur = condition->getSafeChild(XML_DURATION);
            Ancestry *childVal = condition->getSafeChild(XML_VALUE);

            //per prima cosa controlliamo quanti campioni ha
            int min = childDur->getSafeChild(ATT_MIN)->getSafeAttribute(ATT_VALUE).toUInt();

            qreal valMin = childVal->getSafeChild(ATT_MIN)->getSafeAttribute(ATT_VALUE).toDouble();
            qreal valMax = childVal->getSafeChild(ATT_MAX)->getSafeAttribute(ATT_VALUE).toDouble();
            //qDebug()<<"Buffer"<<chanType<<num<<"="<<*(m_channelMap[chanType].at(num))<<m_channelMap[chanType].at(num)->getSamplingPeriod();

            m_stopBuffer.setSamplingPeriod(m_channelMap[chanType].at(num)->getSamplingPeriod());
            m_stopBuffer << *(m_channelMap[chanType].at(num));
            //qDebug()<<"StopBuffer Len"<<m_stopBuffer.getDuration()<<m_stopBuffer.size();
            m_stopBuffer.saveLastSec(min);

            if (!(m_stopBuffer.getDuration() < min))
            {//   continue;
                //ora quindi sono sicuro che arrivo qui solo quando ho abbastanza campioni

                //controllo gli ultimi min campioni
                qreal smin = m_stopBuffer.minimum();
                qreal smax = m_stopBuffer.maximum();
                //qDebug()<<"STOP flowauto"<<smin<<valMin<<smax<<valMax;
                if((smin > valMin) && (smax < valMax)) {
                    qDebug() << "Stop acquiring";
                    endAcquisitionSave();
                    m_acqFinished = true;
                    return;
                }
                else {
                    //qDebug()<<chanType<<num<<(*m_channelMap[chanType].at(num));
                }
            }
        }
   // }

    //effetto buffer tengo solo gli ultimi 5 secondi
    foreach(QString type, m_channelMap.keys())
        foreach(MSignal *sig, m_channelMap[type])
            sig->saveLastSec(5.0);

    //qDebug()<<"Stop check";
}

void MAcqManager::saveBuffersToFile()
{
    //qui so che ho gia'  spedito i campioni al plot per cui salvo sul file ed elimino i campioni dal buffer per sempre

    qDebug() << "saveBuffersToFile() m_dataChanNameMap:" << m_dataChanNameMap;
    foreach (QString chanName, m_dataChanNameMap.keys()) {
        int size = chanName.size();
        QString type,number;

        for(int i = 0; i < size; i++) {
            if(chanName.at(i).isNumber())       //se numero
                number.append(chanName.at(i));
            else
                type.append(chanName.at(i));
        }

        if(!m_channelMap.contains(type) || (number.toInt() < 1) || number.isEmpty())
            qCritical() << type << number << "Not recognized";

        int index = number.toInt() - 1;
        int32_t chanNum = m_dataChanNameMap[chanName];

        //svuota tutto
        //qDebug()<<"Saving"<<m_channelMap[type].at(index)->size()<<"samples in channel"<<chanNum;
        while(m_channelMap[type].at(index)->size() > 0) {
            float v = m_channelMap[type].at(index)->takeFirst();
            m_mng->AppendValue(&chanNum, &v, 1);
            //qDebug("AppendValue(chanNum:%d v:%f),chanNum,v)",chanNum,v);
        }

    }
    //qDebug()<<"Salvooo"<<m_mng->GetDuration();
}

void MAcqManager::sendBuffersToPlot()
{
    //m_namesToDataChanNum mi dice in base al canale

    foreach(QString plotName, m_chanInPlots.keys()) {
        qDebug() << "Gestisco plot" << plotName;
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        QList<MSignal> channels;
        bool toSend = false;
        foreach(QString chanName, m_chanInPlots[plotName]) {
            int size = chanName.size();
            QString type,number;

            for(int i = 0; i < size; i++) {
                if(chanName.at(i).isNumber())   //se numero
                    number.append(chanName.at(i));
                else
                    type.append(chanName.at(i));
            }

            if(!m_channelMap.contains(type) || (number.toInt() < 1) || number.isEmpty())
                qCritical() << m_channelMap.keys() << type<<number << "Not recognized";

            int index = number.toInt() - 1;
            MSignal copy = *(m_channelMap[type].at(index));

            channels << copy;
            qDebug() << "Canale" << chanName << index << number << type;

            if (m_channelMap[type].at(index)->size() > 0)
                toSend = true;
        }

        if (!toSend)
            return;

        qDebug() << "Sending data to" << plotName << m_tcpChannels[plotName]->serverPort() << channels;

        out << (qint32) channels.size();
        for(int i = 0; i < channels.size(); i++) {
            out << i;
            out << (qint32) channels[i].size();
            while(channels[i].size() > 0) {
                out << channels[i].takeFirst();
            }
        }
//        qDebug() << "block[" << block.size() << "]:" << block;
        if(m_tcpChannels.keys().contains(plotName))
            m_tcpChannels[plotName]->sendData(block.data(), block.size());
        else
            qCritical() << "No server of name" << plotName;
    }
}

void MAcqManager::removeLastFrame()
{           //rimuovo un frame temporale
    foreach(QString hwc, m_totalHWChan)
        m_bufferMap[hwc]->remove(0, m_frameMap[hwc]);
}

bool MAcqManager::handleDataFile()
{   // viene chiamata per aggiornare il datafile a seconda del file di configurazione del relativo esame
    Ancestry *channels = m_configAcq.getChild(XML_CHANNELS);
    if(channels == NULL)
        qCritical("Child not alive");

    // ciclo per ogni canale del datafile alla ricerca di proprietA  da completare
    int maxChan = m_mng->GetChanNum();
    for(int i = 0; i < maxChan; i++) {
        QString chanName = m_mng->GetChanName(i);
        qDebug() << "handleDataFile(): chanName:" << chanName;
        foreach (Ancestry *channel, channels->getChildren()) {
            if(channel->getChild(XML_NAME) != NULL)
                if(chanName.contains(channel->getTextOfChild(XML_NAME))) {
                    if(channel->getTextOfChild(XML_GAIN) != "")
                        m_mng->SetGain(i, channel->getTextOfChild(XML_GAIN).toFloat());
                    if(channel->getTextOfChild(XML_OFFSET) != "")
                        m_mng->SetOffset(i, channel->getTextOfChild(XML_OFFSET).toFloat());
                }
        }
#if defined(PICOFLOW) || defined(LINUXDESKTOP)
        m_autoStartStop = checkAutomaticFlow();
#else
        //controllo se questo canale ha i requisiti per fare l'acq automatica
        //mi fido del software archivio pazienti
        if(m_mng->GetLoc(i) == "a")
            m_autoStartStop = true;
#endif
        qDebug() << "handleDataFile(): GetGain,GetOffset:" << m_mng->GetGain(i) << m_mng->GetOffset(i);
    }

    //    //qDebug()<<"stato"<<m_mng->GetState();
    //    qDebug() << "Commit Parameters?" << m_mng->CommitParameters();
    //    qDebug() << "Close?" << m_mng->Close();
    //    qDebug() << "Open?" << m_mng->Open();
    //    qDebug() << "Get Parameters?" << m_mng->GetParameters();
    return true;
}

void MAcqManager::applyOperations()
{
    //abbiamo l'elenco di operazioni da fare per ogni tipo di canale
    //e le eseguiamo
    foreach (QString type, m_operationMap.keys()) { //per ogni tipo di canale
        foreach(QString op, m_operationMap[type]) { //sono tranquillo di avere le operazioni in ordine
            QStringList opData = op.split("@");
            //opData.at(0) e' il nome dell'operazione
            //opData.at(1) e' la buffersize
            //siamo sicuri che ci sono in quanto il controllo sulle dimensioni lo facciamo in readconfiguration
            QString name = opData.at(0);
            int bufferSize = opData.at(1).toInt();

            if(name == "none") {    //non devo fare nulla per cui copio un frame nei channel giusti
                foreach (QString hwc, m_HWChansMap[type]) {
                    int index = m_HWChansMap[type].indexOf(hwc);
                    for(int i = 0; i < m_frameMap[hwc]; i++){
                        double v = m_bufferMap[hwc]->at(i);
                        //se il canale è "VV" devo fare la media mobile
                        if  (type == "VV")
                        {
                            if (v < 0 )
                                v = 0;
                            //media mobile
                            m_sommaMMobileV = m_sommaMMobileV - m_buffer_MMobileV.at(0) + v;
                            m_buffer_MMobileV.remove(0);
                            m_buffer_MMobileV.append(v);
                            double mediato = m_sommaMMobileV/m_lenMMobile;
                            //controllo valori monotoni, i valori di volume non devono decrescere
                            if (m_valPrecVolume < 0) //impostiamo la prima volta il valore precedente
                                m_valPrecVolume = mediato;
                            if (m_valPrecVolume > mediato)
                                mediato = m_valPrecVolume;
                            qDebug()<<"media volume applicata in "<<v<<"ris "<<mediato;

                            m_channelMap[type].at(index)->append(mediato);
                            m_valPrecVolume = mediato;
                        }
                        else
                            m_channelMap[type].at(index)->append(v);
                    }
                    qDebug() << "Canale" << type << "Copiato" << m_frameMap[hwc] << "campioni su" << index;
                }
            }

            if(name == "derive") {      //derivo flusso
                MSignal der;
                foreach (QString hwc, m_HWChansMap[type]) {
                    int index = m_HWChansMap[type].indexOf(hwc);
                    der = m_bufferMap[hwc]->derive(bufferSize);

                    for(int i = 0; i < m_frameMap[hwc]; i++){
                        double deri = der.at(i);
                        if (deri < 0)
                            deri = 0;
                        //media mobile
                        m_sommaMMobileF = m_sommaMMobileF - m_buffer_MMobileF.at(0) + deri;
                        m_buffer_MMobileF.remove(0);
                        m_buffer_MMobileF.append(deri);
                        double mediato = m_sommaMMobileF/m_lenMMobile;

                        //filtro digitale
                        m_buffer_DigFilter.remove(0);
                        m_buffer_DigFilter.append(mediato);
                        double somma = 0;
                        for (int i=0; i<m_lenDifFilter; i++)
                            somma += m_buffer_DigFilter.at(i)*COEFDigFilter[i];
                        double flusso = somma/m_sommaCoef;
                        m_channelMap[type].at(index)->append(flusso);

                    }
                }
            }

            if(name == "threshold") {   //faccio soglia tra due valori quindi mi aspetto almeno altri due parametri
                if(opData.length() != 4)
                    qCritical() << MEX_FILE_CORRUPTED;

                QString smin = opData.at(2);
                QString smax = opData.at(3);
                qreal min, max;

                if(smin == "-INF")
                    min = -INF;
                else
                    min = smin.toDouble();

                if(smax == "INF")
                    max = INF;
                else
                    max = smax.toDouble();

                foreach (QString hwc, m_HWChansMap[type]) {
                    int index = m_HWChansMap[type].indexOf(hwc);

                    m_channelMap[type].at(index)->threshold(min,max);
                }
            }
        }
    }
}
/**
     * TCP PACKET:
     * - numChan, type qint32. Number on channel present into the packet. It has to be between 0 and m_maxNumChan
     * The following data are repeated numChan times
     * - currChan, type qint32. Current channel has to be a value between 0 and m_maxNumChan
     * - numChanData, type qint32. Number of samples (float) for the current channel
     * - samples, type qreal, lenght numChanData. Channel's samples
     */

void MAcqManager::fillBuffers(QByteArray __block)
{
    uchar maxNumChan = m_mng->GetChanNum();
    static QByteArray staticblock;
    staticblock += __block;

    bool samplesToRead = true;
    while(samplesToRead)
    {
        //qDebug() << "inizio loop bytes rimanenti:" << staticblock.size();
        qint32 numChan = 0;
        qint32 numBytes = 0;
        qint32 currChan = 0;
        qint32 numChanData = 0;
        qreal sample;
        QDataStream in(&staticblock, QIODevice::ReadOnly);

        in >> numBytes;
        qDebug()<<"NA? bytes: "<<numBytes;
        if(staticblock.size() < numBytes)
            break;

        //Chan number
        in >> numChan;
        qDebug()<<"NA? chan: "<<numChan;

        if(numChan > maxNumChan) {
            qCritical( "ERROR, numChan(%d) > maxNumChan(%d)", numChan,maxNumChan);
            return;
        }

        for(int k = 0; k < numChan; k++) {
            in >> currChan;
            in >> numChanData;
//            qDebug() << "currChan:" << currChan;

            if((currChan < maxNumChan) && (currChan >= 0)) {    //se e' un canale con del senso
                if(m_bufferMap.keys().contains(QString::number(currChan))) {
                    for(int i = 0; i < numChanData; i++) {
                        in >> sample;
                        m_bufferMap[QString::number(currChan)]->append(sample);
//                        if (m_valPrecVolume > mediato)
//                            mediato = m_valPrecVolume;

//                        if (sample < 0 )
//                            sample = 0;
//                        //media mobile
//                        m_sommaMMobileV = m_sommaMMobileV - m_buffer_MMobileV.at(0) + sample;
//                        m_buffer_MMobileV.remove(0);
//                        m_buffer_MMobileV.append(sample);
//                        double mediato = m_sommaMMobileV/m_lenMMobile;
//                        //controllo valori monotoni, i valori di volume non devono decrescere
//                        if (m_valPrecVolume < 0) //impostiamo la prima volta il valore precedente
//                            m_valPrecVolume = mediato;
//                        if (m_valPrecVolume > mediato)
//                            mediato = m_valPrecVolume;
//                        qDebug()<<"media volume applicata in "<<sample<<"ris "<<mediato;

//                        m_bufferMap[QString::number(currChan)]->append(mediato);
//                        m_valPrecVolume = mediato;
                        //qDebug("samples(ch:%d, nd:%d):%f",currChan,numChanData,sample);
                    }

                    //qDebug() << currChan << m_bufferMap[QString::number(currChan)]->size();
                }
            }
            else
                qCritical() << "currChan out of range" << currChan;
        }
        //rimuoviamo roba gia'  processata
        staticblock.remove(0, numBytes+4);

        if (staticblock.size() < 4)
            samplesToRead = false;
    }
}

bool MAcqManager::buffersReady()
{
    // questa funzione controlla se i miei buffer sono pronti
    // controllo se hanno abbastanza campioni per fare le operazioni e
    // contemporaneamente restituire un frame

    foreach(QString hwchan, m_totalHWChan) {
        if(m_bufferMap[hwchan]->size() < (m_bufSizeMap[hwchan] + m_frameMap[hwchan])) {
            qDebug() << hwchan << "buffer non pronto con" << m_bufferMap[hwchan]->size() << "<" << (m_bufSizeMap[hwchan] + m_frameMap[hwchan]);
            return false;
        }
    }

//    foreach(QString hwchan, m_totalHWChan)
//        qDebug() << "buffers pronti" << m_bufferMap[hwchan]->size() << ">" << (m_bufSizeMap[hwchan] + m_frameMap[hwchan]);

    return true;
}

bool MAcqManager::readConfigurationFile()
{
    //in questa funzione leggo il file di configurazione e mi annoto le info che mi servono
    qDebug() << "Inizio a leggere il file di configurazione";

    Ancestry *channels = m_configAcq.getSafeChild(XML_CHANNELS);
    qDebug() << "Trovati" << channels->getChildren().size() << "canali";

    foreach(Ancestry *channel, channels->getChildren()) {
        qDebug() << "Inizio analisi Canale";
        QString name = channel->getSafeChild(XML_NAME)->text();
        qDebug() << "Name" << name;
        bool skip = true;

        foreach (QString n, m_channelNames) {   //QBT1,VBT1 ecc
            if(n.contains(name)) {
                skip = false;//questo tipo di canale e tra quelli del datafile per cui posso processarlo
                break;
            }
        }

        if(skip) {
            qWarning() << "Skip canali di tipo" << name;
            continue;
        }

        // scorriamo tutti i canali per acquisire le info
        // che supervisore serve?
        QString card = channel->getSafeAttribute("card");
        qDebug() << "Card" << card;

        if(card == "")
            qCritical() << "File corrupted";

        // se non c'e' gia'  questo supe lo aggiungo alla lista di quelli da far partire
        if(!m_superList.contains(card))
            m_superList << card;

        if(name == "")
            qCritical() << "File corrupted";

        // ora leggiamo quanti canali di questo tipo ci sono
        QString num = channel->getSafeChild(XML_NUM)->text();
        qDebug() << num << "canali di questo tipo";

        // e che frequenza hanno
        QString f = channel->getSafeChild(XML_FREQUENCY)->text();
        qDebug() << "con frequenza" << f;
        if(f == "")
            qCritical() << "File corrupted";

        int fmin = 0x7fffffff;
        int sampleFreq = f.toInt();
        if(sampleFreq < fmin)
            fmin = sampleFreq;
        qDebug() << "Aggiornamento frequenza minima" << fmin;

        if(num.toUInt() > 0) {
            for(uint i = 0; i < num.toUInt(); i++) {
                MSignal *p = new MSignal();
                p->setSamplingFrequency(sampleFreq);
                m_channelMap[name].append(p);

                qDebug() << "Aggiunto segnale a channelMap" << name << "con frequenza di campionamento" << sampleFreq;
            }
        }
        else
            qCritical() << "Fake channel" << num;


        //ci sono delle operazioni?
        QString operations = channel->getSafeChild(XML_OPERATIONS)->text();

        int bufMax = 0;     // massima lunghezza di buffer
        // memorizzo la mappa delle operazioni
        m_operationMap[name] = operations.split("#");
        foreach(QString operation, m_operationMap[name]) {
            QStringList opData = operation.split("@");
            if(opData.length() < 2)
                qCritical() << "File corrupted";
            if(opData.at(1) != "") {
                int bufSize = opData.at(1).toInt();
                if(bufSize > bufMax)
                    bufMax = bufSize;
            }
        }
        qDebug() << m_operationMap[name];
        // ora si suppone che canali dello stesso tipo subiscono le stesse operazioni

        // quali canali sono coinvolti?
        QString hwchans = channel->getSafeChild(XML_HWCHAN)->text();

        if(hwchans == "")
            qCritical() << "File corrupted";

        // ora si suppone che ci sia scritto num valori
        if(hwchans.split("#").size() == num.toInt()) {
            foreach(QString hwc, hwchans.split("#")) {
                m_HWChansMap[name] << hwc;
                if(m_sampleFreqMap.keys().contains(hwc))
                    if(m_sampleFreqMap[hwc] != sampleFreq)  // controllo che questo canale abbia una sola frequenza di campionamento
                        qCritical() << "Frequenza di campionamento diverse" << m_sampleFreqMap[hwc] << sampleFreq;
                m_sampleFreqMap[hwc] = sampleFreq;
                if(m_bufSizeMap[hwc] < bufMax)
                    m_bufSizeMap[hwc] = bufMax;

                if(!m_totalHWChan.contains(hwc))
                    m_totalHWChan << hwc;
            }
        }
        else
            qCritical() << "missing hw channel";
    }


    // lo faccio adesso perche ho la mappa delle frequenze completata
    foreach(QString c, m_totalHWChan) {     //inizializzo i buffer hardware;
        m_bufferMap[c] = new MSignal();
        m_bufferMap[c]->setSamplingFrequency(m_sampleFreqMap[c]);
        qDebug() << "mcd" << gcd(m_sampleFreqMap.values());
        m_frameMap[c] = m_sampleFreqMap[c] / gcd(m_sampleFreqMap.values());
        qDebug() << "Aggiungo canale buffer" << c << m_sampleFreqMap[c] << "frame" << m_frameMap[c];
    }

    //sono a posto cosA?
    qDebug() << "Configuration file read succesfully!";

    return true;
}
