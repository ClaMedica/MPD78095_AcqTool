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
    m_superProcess=NULL;//nessun supervisore avviato
    m_saving=false;//non sto salvando i dati

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

bool MAcqManager::newAcquisition(QString __dataFile, QString __configFile)
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

        //controllo l'esistenza dei file

        if(!QFile::exists(__dataFile))
        {qCritical()<<"File does not exists";return false;}
        m_acqFileName=__dataFile;

        if(!QFile::exists(__configFile))
        {qCritical()<<"File does not exists";return false;}
        setConfigurationFile(__configFile);

        m_mng=new DatafileManager;

        m_mng->SetFileName(m_acqFileName);
        m_mng->SetFileType(5);

        qDebug()<<"Opening file ... "<<m_mng->Open();
        qDebug()<<"Loading parameters ... "<<m_mng->GetParameters();

        m_acqFileOpened=true;//mi segno che ho aperto il file

        qDebug()<<"Building configuration file for plots";
        if(!buildConfigurationFile())
        {qCritical()<<"Error during building of configuration file";return false;}

        //aggiorno il datafile con i dati relativi alla mia configurazione
        qDebug()<<"Updating datafile...";
        updateDataFile();
        //ripristino il file in acquisizione
        qDebug()<<"Continue ..."<<m_mng->Continue();
        //inizializzo i server di comunicazione con i plotter
        initializeServers();
        //disabilito alcuni allarmi
        m_alarmMng.manageAlarm(S_ALA_NOT_ACQUIRING,DISABLE);
        //mi connetto ai server del supe e del programma di gestione archivi
        QTimer::singleShot(10000,this,SLOT(connectToServers()));


    }
    //faccio partire il timer per l'allarme di stato
    //m_alarmMng.startTimeoutAlarms();

    return true;
}

void MAcqManager::connectToServers()
{
    //mi connetto ai server
    qDebug()<<"Connecting to servers ..."<<m_tcpClients.keys();
    foreach (SimpleTCPClient *client, m_tcpClients) {
        client->registerDataReadyCallBack(&(this->dataOnTCP));
        client->connectToHost();

        if(client->waitForConnected(10000))
        {
            qDebug()<<"connesso "<<client->hostAddress().toString()<<client->hostPort();
        }
        else
        {
            qCritical()<<"Timeout of"<<m_tcpClients.key(client);
            QStringList superPorts;
            superPorts<<"CMD"<<"VAL"<<"STA";
            if(superPorts.contains(m_tcpClients.key(client)))
            {
                if(m_superProcess!=NULL)
                {
                    m_superProcess->kill();
                    if(m_superProcess->waitForFinished())
                        delete m_superProcess;
                }
                startSupe("show");
                QTimer::singleShot(10000,this,SLOT(connectToServers()));
            }
        }
    }
}

void MAcqManager::startSupe(QString __mode)
{
    m_superProcess=new QProcess();
    qDebug()<<"Supervisor starting...";
    QString path="M:/Lavoro/Software/Build/StandAlone/";
    m_superProcess->start(path+"FlowBtSupe.exe",QStringList()<<__mode);
}

void MAcqManager::endAcquisition(QString __exit)
{
    //disabilito gli allarmi
    m_alarmMng.disableAll();
    //interrompo la connessione

    foreach (SimpleTCPClient *client, m_tcpClients) {
        //mi disconnetto dal supe

        if(client->disconnectToHost())
            qDebug()<<"disconnect "
                   <<client->hostAddress().toString()
                  <<client->hostPort();
    }
    qDebug()<<"statoooooo "<<m_mng->GetState();
    if(m_acqFileOpened)
    {//se siamo in acq facciamo un commit
        qDebug()<<"Commit Values?"<<m_mng->CommitValues();
    }
    qDebug()<<"File closed?"<<m_mng->Close();
    delete m_mng;
    m_mng=NULL;
    //mando il comando al gestore archivio paziente
    QByteArray mex=__exit.toLatin1();
    m_tcpChannels["MNG"]->sendData(&mex);
    qDebug()<<__exit;

    foreach (SimpleTCPChannel *channel, m_tcpChannels) {
        //mi disconnetto dal resto

        if(channel->disconnect())
            qDebug()<<"disconnect "
                   <<channel->serverAddress().toString().toLatin1()
                  <<channel->serverPort();
    }
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
    //#BUG
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
    {qCritical()<<"No CMD channel loaded";return false;}
}

void MAcqManager::updateAcqData()
{
    QStringList plotNames;
    plotNames<<"Cella";//#BUG
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

        if(who=="VAL")
        {
            fillBuffers(__block);

            //abbiamo spacchettato i canali e salvati nella mappa dei buffer
            //calcoliamoci i canali software se ci sono
            calculateSoftwareChannels();

            if(!m_saving)
                checkAutomaticStartStop("Start");//finchè non devo salvare riempo il buffer e controllo
            else
            {//ora vado a salvare i canali fisici se ho dei buffer un po grandini tipo ogni 10 campioni
                qDebug()<<"Start acquiring";
                foreach (uint bufNum, m_HWChannelMap.keys()) {
                    int32_t chan=m_HWChannelMap[bufNum];
                    qDebug()<<chan;
                    if(m_HBufferMap[bufNum].size()>10){
                        foreach (qreal val, m_HBufferMap[bufNum]) {
                            float v=(float)val;
                            m_mng->AppendValue(&chan,&v,1);
                        }
                    }
                }

                //e quelli software

                foreach (QString bufName, m_SWChannelMap.keys()) {
                    int32_t chan=m_SWChannelMap[bufName];
                    qDebug()<<chan;
                    if(m_SBufferMap[bufName].size()>10){
                        foreach (qreal val, m_SBufferMap[bufName]) {
                            float v=(float)val;
                            m_mng->AppendValue(&chan,&v,1);
                        }
                    }
                }


                if(m_saving)
                    m_HBufferMap.clear();//pulisco il buffer temporaneo se sto già salvando i dati

                //checkAutomaticStartStop("Stop");
            }
            if(!m_sendingToPlot && m_serverReady && m_saving)
            {
                //qDebug()<<"al plot"<<dataCount<<(float)tim.elapsed()/1000;
                //mandiamo i dati alla visualizzazione
                m_sendingToPlot=true;
                sendToPlots();
                m_sendingToPlot=false;
            }
        }
    }
}

void MAcqManager::initializeServers()
{
    //creiamo un nuovo canale con la qmlplotter
    qDebug()<<"Creating servers for plots ... "<<m_tcpChannels.keys();
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
    {
        qCritical()<<"Files "<<m_configurationFileName<<__name<<"does not exists";return false;
    }
    if(!m_configuration.loadFromXML(curConfigFile))
    {
        qCritical()<<"XML file corrupted";return false;
    }

    qDebug()<<curConfigFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
    //carichiamo le connessioni
    if(m_configuration.getChild(XML_CONNECTIONS)!=NULL)
        return loadConnectivityInfo(m_configuration.getChild(XML_CONNECTIONS));
    else
    {qCritical()<<"XML file corrupted";return false;}
}

bool MAcqManager::loadAlarms(QString __name)
{
    QString curAlaFile="";

    if(__name=="")
        curAlaFile=m_alarmFileName;
    else
        curAlaFile=__name;

    if(!QFile::exists(curAlaFile))
    {qCritical()<<"File "+curAlaFile+" does not exists";return false;}
    if(!m_alarmMng.load(curAlaFile))
    {qCritical()<<"XML file corrupted";return false;}

    qDebug()<<curAlaFile<<"Loaded correctly";
    //ora abbiamo caricato tutto ciò che ci serve dentro a m_configuration
    //carichiamo le connessioni
    return true;
}

void MAcqManager::saveConfiguration()
{
    m_configuration.saveToXML(m_configurationFileName);
}

bool MAcqManager::loadConnectivityInfo(Ancestry *__info)
{
    qDebug()<<"Loading connectivity info";
    if(__info==NULL)
    {qCritical()<<"NULL pointer";return false;}
    if(__info->getChild(XML_TCP)!=NULL)
    {
        Ancestry *tcp=__info->getChild(XML_TCP);

        foreach (Ancestry *child, tcp->getChildren()) {
            QString name=child->name();

            QString address=child->getAttribute(ATT_ADDRESS);
            int port=child->getAttribute(ATT_PORT).toInt();
            if(child->getAttribute(ATT_TYPE)=="client")
                m_tcpClients[name]=new SimpleTCPClient(QHostAddress(address),port,this);
            else if(child->getAttribute(ATT_TYPE)=="server")
                m_tcpChannels[name]=new SimpleTCPChannel(QHostAddress(address),port,this);
            qDebug()<<name<<address<<port;
        }
    }
    else
    {qCritical()<<"XML file corrupted";return false;}
    qDebug()<<"Loaded clients:"<<m_tcpClients.keys();
    qDebug()<<"Loaded channels:"<<m_tcpChannels.keys();
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

bool MAcqManager::buildConfigurationFile()
{
    /*per costruire il file di configurazione corrente da dare in pasto al plotter
    *devo carpire le informazioni che mi servono:
    *
    * DatafileManager, mi dice:
    * _ quanti canali ci sono in totale
    * _ dove andarli a disegnare
    * _ come si chiamano
    * _ unità di misura
    *
    * ConfigFile originale, mi dice:
    * _ particolarià del singolo canale: in base al suo nome vado a vedere cosa ci devo fare
    * _ gain e offset
    * _ se è un canale software o hardware
    * _ nel caso sia un canale software mi dice che operazioni ci devo fare con gli attributi
    * operation e chanList dove il primo mi dice il tipo di operazione e il secondo la lista
    * di canali coinvolti separata da virgola
    *
    */
    Ancestry config;//iniziamo col pescare il file di configurazione
    //ora che la mia classe è popolata la vado a completare iniziando con l'aggiungere il campo graphs

    Ancestry *graph=config.addChild(XML_GRAPHS);
    if(graph==NULL){qCritical()<<"Could not create child";return false;}
    //mi salvo il puntatore al livello channels
    Ancestry *channels=m_configuration.getChild(XML_CHANNELS);

    //ok finita questa fase preliminare iniziamo con calma a scrivere qualcosa
    //peschiamo il numero totale di canali
    int chanlNum=m_mng->GetChanNum();
    //qDebug()<<"N° Canali: "<<chanlNum;
    if(chanlNum==0){qCritical()<<"No channels in file";return false;}
    for(int nc=0;nc<chanlNum;nc++)
    {//contiamo i grafici e popoliamo la mappa di associazione dei canali fisici
        QString chanName=m_mng->GetChanName(nc);
        Ancestry *cur=NULL;
        //scorro tutti i canali alla ricerca di quello col mio nome
        foreach(Ancestry *channel,channels->getChildren())
            if(chanName.contains(channel->getTextOfChild(XML_NAME)))//#BUG non mi piace per nulla
            {cur=channel;break;}

        //è necessario controllare che nel file di configurazione ci sia scritto come gestire questo canale
        if(cur==NULL){qCritical()<<"No channel name in config file";return false;}
        //ok ora mi segno se e a quale canale fisico è associato questo canale
        QString tcpChan=cur->getTextOfChild(XML_TCPCHAN);
        if(tcpChan!="none" || tcpChan!=QString())
            m_HWChannelMap[tcpChan.toUInt()]=(int32_t)nc;
        /*ok magari fa un po' di confusione ma facciamo un esempio:
        *sul file di config c'è scritto <VBT1 HWC="0"> e nel datafile VBT1 è il nome del canale 1
        *allora l'operazione è m_HWChannelMap[0]=1
        * questo vuol dire che il canale che nel pacchetto TCP che mi arriva è al posto 0 va salvato nel posto 1 del datafile
        */
        //pesco il numero del grafico dove far vedere questo canale e me lo annoto in mappa
        m_chanInPlots["Graph_"+QString::number(m_mng->GetGraph(nc))]<<nc;
    }
    //bene ora mi servono le info della parte di connection per poter istruire i plot
    Ancestry *connections=m_configuration.getChild(XML_CONNECTIONS);
    if(connections==NULL){qCritical()<<"No connections field in file";return false;}
    foreach (QString graphName, m_chanInPlots.keys()) {//scorro per ogni grafico


        Ancestry *  graphN=graph->addChild(graphName);
        if(graphN==NULL){qCritical()<<"Could not create child";return false;}
        Ancestry *  prop=graphN->addChild(XML_PROPERTIES);
        Ancestry *  tracks=graphN->addChild(XML_TRACKS);
        if(prop==NULL){qCritical()<<"Could not create child";return false;}
        if(tracks==NULL){qCritical()<<"Could not create child";return false;}
        //ho la certezza che i canali su ogni grafico hanno tutti le stesse proprietà grafiche per cui vado tranquillo
        if(m_chanInPlots[graphName].size()==0)
            qFatal("Graph without channels");//controllo superfluo ma se da errore meglio chiudere baracca e burattini
        int nc=m_chanInPlots[graphName].at(0);
        Ancestry *  axis=prop->addChild(XML_AXIS);
        Ancestry *  time=prop->addChild(XML_TIME);
        Ancestry *  network=prop->addChild(XML_NETWORK);
        if(axis==NULL){qCritical()<<"Could not create child";return false;}
        if(time==NULL){qCritical()<<"Could not create child";return false;}
        if(network==NULL){qCritical()<<"Could not create child";return false;}
        axis->setAttribute("yAUOM",QString::number(m_mng->GetUdM(nc)));
        axis->setAttribute("yAbsoluteMax",QString::number(m_mng->GetSupLim(nc)));
        axis->setAttribute("yAbsoluteMin",QString::number(m_mng->GetInfLim(nc)));
        time->setAttribute("samplingFrq",QString::number(m_mng->GetNAS(nc)));
        time->setAttribute("pageTime",QString::number(m_mng->GetPageTime()));
        network->setAttribute(ATT_PORT,QString::number(9000+nc));
        network->setAttribute(ATT_ADDRESS,"127.0.0.1");

        //aggiungo il canale su cui comunicherà questo plot
        m_tcpChannels[graphName]=new SimpleTCPChannel(QHostAddress("127.0.0.1"),9000+nc,this);

        foreach (int chan, m_chanInPlots[graphName])
        {//qui scrivo le proprietà delle tracce
            QString chanName=m_mng->GetChanName(chan);
            Ancestry * trkN=tracks->addChild(chanName);
            if(trkN==NULL){qCritical()<<"Could not create child";return false;}
            trkN->setAttribute(ATT_THICK,"3");
            trkN->setAttribute(ATT_COLOR,"white");
        }
    }
    //ora salvo il file di configurazione come cur.xml
    config.saveToXML(QDir::currentPath()+"/cur.xml");
    qDebug()<<"Configuration file builded succesfully";
    //e mi sovrascrivo il nome perchè tanto le info ce le ho
    m_plotConfigFileName=QDir::currentPath()+"/cur.xml";
    return true;
}

void MAcqManager::sendToPlots()
{

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    foreach(QString plotName,m_chanInPlots.keys())
    {
        QList<int> chanList=m_chanInPlots[plotName];
        QVector<MSignal> channels;
        block.clear();
        //controllo tra i buffer hardware
        foreach(int32_t ch,m_HWChannelMap)
            if(chanList.contains(ch))
                channels<<m_HBufferMap[m_HWChannelMap.key(ch)];

        //controllo tra i buffer software
        foreach(int32_t ch,m_SWChannelMap)
            if(chanList.contains(ch))
                channels<<m_SBufferMap[m_SWChannelMap.key(ch)];

        out<<(qint32)chanList.size();
        for(int i=0;i<chanList.size();i++){
            out<<i;
            out<<(qint32)channels[i].size();
            foreach (qreal val, channels[i]) {
                out<<val;
            }
        }
        if(m_tcpChannels.keys().contains(plotName))
            m_tcpChannels[plotName]->sendData(block.data(),block.size());
        else
            qCritical()<<"No server of name"<<plotName;
    }

}

void MAcqManager::checkAutomaticStartStop(QString __which)
{//ok controlliamo se c'è qualche condizione automatica
    //mi salvo il puntatore al livello acquisition
    qDebug()<<"Start check";
    Ancestry *channels=m_configuration.getChild(XML_CHANNELS);
    if(channels==NULL){qCritical("Child not alive");return;}
    foreach (Ancestry *channel, channels->getChildren()) {//scorro ogni canale alla ricerca di una condizione
        uint chanNum=channel->getTextOfChild(XML_TCPCHAN).toUInt();
        Ancestry *autoConfig=channel->getChild("Auto"+__which);
        if(autoConfig==NULL)
            continue;//questo canale non viene toccato -> skip
        //trovo un canale con una condizione cercata

        QString enabled=autoConfig->getAttribute(ATT_ENABLED);
        if(enabled=="false")
            continue;//ci sono condizioni ma sono disabilitate
        //ci sono le condizioni e sono abilitate -> avanti savoia!

        foreach(Ancestry *condition,autoConfig->getChildren())
        {
            if(condition->name()==XML_STEP)
            {//condizione a gradino

                //per prima cosa controlliamo quanti campioni è
                uint min=condition->getChild(XML_DURATION)->getAttribute(ATT_MIN).toUInt();
                uint bufferSize=condition->getAttribute(ATT_BUFFERSIZE).toUInt();
                qreal ampMin=condition->getChild(XML_AMPLITUDE)->getAttribute(ATT_MIN).toInt();
                qreal ampMax=condition->getChild(XML_AMPLITUDE)->getAttribute(ATT_MAX).toInt();
                qDebug()<<m_HBufferMap[chanNum];
                qDebug()<<min<<bufferSize<<ampMax<<ampMin;
                if(m_HBufferMap[chanNum].size()<min)
                    continue;//non ho ancora abbastanza campioni per decidere skip alla prossima condizione
                //ora quindi sono sicuro che arrivo qui solo quando ho abbastanza campioni
                if(m_HBufferMap[chanNum].size()>bufferSize)
                    m_HBufferMap[chanNum].removeFirst();//effetto buffer
                //controllo se c'è un gradino
                qreal startVal=m_HBufferMap[chanNum].at(0);
                int count=0;
                foreach(qreal sample,m_HBufferMap[chanNum]){
                    if(sample-startVal>ampMin && sample-startVal<ampMax)
                        count++;
                    else
                        count=0;
                }

                if(count>=min)
                {
                    m_saving=true;//posso iniziare a salvare i dati
                    saveBuffersToFile();
                    qDebug()<<"inizio acq";
                    return;
                }
            }

        }

    }
    qDebug()<<"Stop check";
}

void MAcqManager::saveBuffersToFile()
{
    foreach (MSignal currBuff, m_HBufferMap) {
        int32_t chanNum=m_HBufferMap.key(currBuff);
        foreach(qreal sample,currBuff){
            float v=sample;
            int32_t chan=(int32_t)m_HWChannelMap[chanNum];
            //qDebug()<<chan;
            m_mng->AppendValue(&chan,&v,1);
        }
        //qDebug()<<currBuff;
    }

    foreach (MSignal currBuff, m_SBufferMap) {
        QString chanName=m_SBufferMap.key(currBuff);
        foreach(qreal sample,currBuff){
            float v=sample;
            int32_t chan=(int32_t)m_SWChannelMap[chanName];
            //qDebug()<<chan;
            m_mng->AppendValue(&chan,&v,1);
        }
        //qDebug()<<currBuff;
    }
}

bool MAcqManager::updateDataFile()
{//viene chiamata per aggiornare il datafile a seconda del file di configurazione del relativo esame
    Ancestry *channels=m_configuration.getChild(XML_CHANNELS);
    if(channels==NULL){qCritical("Child not alive");return false;}
    //ciclo per ogni canale del datafile alla ricerca di proprietà da completare
    int maxChan=m_mng->GetChanNum();
    for(int i=0;i<maxChan;i++)
    {
        QString chanName=m_mng->GetChanName(i);

        foreach (Ancestry *channel, channels->getChildren()) {
            if(channel->getChild(XML_NAME)!=NULL)
                if(channel->getChild(XML_NAME)->text()==chanName){
                    if(channel->getChild(XML_GAIN)!=NULL)
                        m_mng->SetGain(i,channel->getChild(XML_GAIN)->text().toFloat());
                    if(channel->getChild(XML_OFFSET)!=NULL)
                        m_mng->SetOffset(i,channel->getChild(XML_OFFSET)->text().toFloat());
                }
        }
        qDebug()<<m_mng->GetGain(i)<<m_mng->GetOffset(i);
    }
    qDebug()<<"stato"<<m_mng->GetState();
    qDebug()<<"Commit Parameters?"<<m_mng->CommitParameters();
    return true;
}

void MAcqManager::calculateSoftwareChannels()
{//dobbiamo leggere il file di configurazione e capire se ci sono canali software
    Ancestry *channels=m_configuration.getChild(XML_CHANNELS);
    if(channels==NULL){qCritical("Child not alive");return;}
    foreach (Ancestry *channel, channels->getChildren()) {
        QString tcpChan=channel->getTextOfChild(XML_TCPCHAN);
        if(tcpChan!="")
        {//canale agganciato a qualcosa
            int32_t dataChan=channel->getTextOfChild(XML_DATACHAN).toUInt();
            m_SWChannelMap[channel->getTextOfChild(XML_NAME)]=dataChan;
            //controlliamo che operazione devo fare
            if(channel->getTextOfChild(XML_OPERATION)=="derive")
            {//derivataaaaa
                uint chToDeriv=tcpChan.toUInt();
                //mi intende il canale fisico ovviamente
                m_SBufferMap[channel->getTextOfChild(XML_NAME)]=m_HBufferMap[chToDeriv].derive();
            }
        }
    }
}

void MAcqManager::fillBuffers(QByteArray __block)
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
        qCritical( "ERROR, numChan(%d) > maxNumChan(%d)", numChan,maxNumChan);
        return;
    }

    for(int k = 0; k < numChan; k++)
    {
        in >> currChan;
        in >> numChanData;
        //qDebug()<<currChan;
        if(currChan < maxNumChan && currChan >= 0)
        {//se è un canale con del senso

            for(int i=0;i < numChanData;i++)
            {
                in >> sample;
                //                        float v;
                //                        float diff=fabs(m_oldSample-sample);

                //                        if(diff>0.75)
                //                        {
                //                            v=round(sample);
                //                            m_oldSample=v;
                //                        }
                //                        else
                //                            v=m_oldSample;
                //ora di questo campione cosa ne faccio?
                m_HBufferMap[currChan]<<sample;//lo metto nel buffer del canale Hardware corrispondente
            }
        }
        else
            qCritical()<< "currChan out of range"<<currChan;
    }
}
