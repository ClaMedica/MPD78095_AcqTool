#include "macqmanager.h"
int dataCount=0;



MAcqManager::MAcqManager(QObject *parent) :
    QObject(parent)
{

    m_mng=NULL;
    m_acqFileOpened=false;//nessuna acquisizione in atto
    m_sendingToPlot=false;//nessuno sta spedendo qualcosa per cui ci si può scrivere sopra
    m_serverReady=false;//i server non sono inizializzati quindi falso

    m_superProcess=NULL;//nessun supervisore avviato
    m_saving=false;//non sto salvando i dati
    m_applicationPath=QApplication::applicationDirPath();
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

    foreach (QList<MSignal *> list, m_channelMap) {
        foreach(MSignal *sig,list){
            delete sig;
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

bool MAcqManager::load()
{//in questa funzione inizializzo tutto caricando i file di configurazione fissi
    if(!m_configLocale.loadFromXML(m_applicationPath+"/Config_Locale.xml"))
    {qCritical()<<"Error on locale configuration file";return false;}

    if(!m_configAcq.loadFromXML(m_applicationPath+"/Config_Acq.xml"))
    {qCritical()<<"Error on acq configuration file";return false;}

    if(!m_configUser.loadFromXML(m_applicationPath+"/Config_User.xml"))
    {qCritical()<<"Error on user configuration file";return false;}

    //e infine carico il file degli allarmi con la lingua giusta
    QString lang=m_configLocale.getChild(XML_LOCALE)->getAttribute("Value");
    if(!m_alarmMng.load(m_applicationPath+"/Config_Alarms_"+lang+".xml"))
    {qCritical()<<"Error on alarm configuration file";return false;}

    //carico info di connettività
    if(m_configAcq.getChild(XML_CONNECTIONS)!=NULL)
        return loadConnectivityInfo(m_configAcq.getChild(XML_CONNECTIONS));
    else
    {qCritical()<<"XML file corrupted";return false;}
}

bool MAcqManager::newAcquisition(QString __dataFile)
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

        m_mng=new DatafileManager;

        m_mng->SetFileName(m_acqFileName);
        m_mng->SetFileType(5);

        qDebug()<<"Opening file ... "<<m_mng->Open();
        qDebug()<<"Loading parameters ... "<<m_mng->GetParameters();

        m_acqFileOpened=true;//mi segno che ho aperto il file

        qDebug()<<"Reading configuration file";
        if(!readConfigurationFile())
        {qCritical()<<"Error during reading of configuration file";return false;}

        qDebug()<<"Building configuration file for plots";
        if(!buildConfigurationFile())
        {qCritical()<<"Error during building of configuration file";return false;}

        //aggiorno il datafile con i dati relativi alla mia configurazione #BUG da togliere non appena il file verrà scritto correttamente
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
    QString path="../StandAlone/";
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
        quint8 c=(quint8)__command;
        m_tcpClients["CMD"]->sendData((char *)&c,sizeof(quint8));
        qDebug()<<"Sending command: "<<__command;
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
            //riempo i buffer
            fillBuffers(__block);

            //finchè i buffer hanno abbastanza campioni
            //faccio le mie operazioni e rimuovo i primi campioni
            while(buffersReady())
            {
                applyOperations();
                foreach(QString hwc,m_totalHWChan)
                    m_bufferMap[hwc]->remove(0,m_frameMap[hwc]);
            }

            if(!m_saving){
                checkAutomaticStartStop("Start");//finchè non devo salvare riempo il buffer e controllo
            }
            else
            {
                sendBuffersToPlot();
                saveBuffersToFile();
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

bool MAcqManager::loadConnectivityInfo(Ancestry *__info)
{
    qDebug()<<"Loading connectivity info";
    if(__info==NULL)
    {qCritical()<<MEX_CHILD_NOT_ALIVE;return false;}
    Ancestry *tcp=__info->getChild(XML_TCP);
    if(tcp==NULL)
    {qCritical()<<MEX_CHILD_NOT_ALIVE;return false;}


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

    qDebug()<<"Loaded clients:"<<m_tcpClients.keys();
    qDebug()<<"Loaded channels:"<<m_tcpChannels.keys();
    return true;
}

void MAcqManager::analyzeStatus(flowBT_status_t __status)
{
    if(__status.currState!=m_oldState)
        qDebug()<<"Stato "<<__status.currState;

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

bool MAcqManager::readConfigurationFile()
{
    int fmin=INF;
    //in questa funzione leggo il file di configurazione e mi annoto le info che mi servono
    Ancestry *channels=m_configAcq.getChild(XML_CHANNELS);
    if(channels==NULL){qCritical()<<MEX_CHILD_NOT_ALIVE;return false;}

    foreach(Ancestry *channel,channels->getChildren()){
        //scorriamo tutti i canali per acquisire le info
        //che supervisore serve?
        QString card=channel->getAttribute("card");
        if(card==""){qCritical()<<"File corrupted";return false;}
        if(!m_superList.contains(card))
            m_superList<<card;
        QString name=channel->getTextOfChild(XML_NAME);
        if(name==""){qCritical()<<"File corrupted";return false;}
        //ora leggiamo quanti canali di questo tipo ci sono
        QString num=channel->getTextOfChild(XML_NUM);
        //
        QString f=channel->getTextOfChild(XML_FREQUENCY);
        if(f==""){qCritical()<<"File corrupted";return false;}
        int sampleFreq=f.toInt();
        if(sampleFreq<fmin)
            fmin=sampleFreq;

        if(num.toUInt()>0){
            for(uint i=0;i<num.toUInt();i++)
            {
                MSignal *p=new MSignal();
                p->setSamplingFrequency(sampleFreq);
                m_channelMap[name].append(p);
            }
        }
        else
            qCritical()<<"Fake channel"<<num;

        //ci sono delle operazioni?
        Ancestry *operations=channel->getChild(XML_OPERATIONS);
        if(operations==NULL){qCritical()<<MEX_CHILD_NOT_ALIVE;return false;}
        int bufMax=0;//massima lunghezza di buffer
        foreach(Ancestry *operation, operations->getChildren())
            if(operation->getAttribute(ATT_BUFFERSIZE)!="")
            {
                int bufSize=operation->getAttribute(ATT_BUFFERSIZE).toInt();
                m_operationMap[name][operation->name()]=bufSize;
                if(bufSize>bufMax)
                    bufMax=bufSize;
            }
        //qDebug()<<m_operationMap[name];
        //ora si suppone che canali dello stesso tipo subiscono le stesse operazioni


        //quali canali sono coinvolti?
        QString hwchans=channel->getTextOfChild(XML_HWCHAN);
        if(hwchans==""){qCritical()<<"File corrupted";return false;}
        //ora si suppone che ci sia scritto num valori
        if(hwchans.split("#").size()==num.toInt())
        {
            foreach(QString hwc,hwchans.split("#")){
                m_HWChansMap[name]<<hwc;
                if(m_sampleFreqMap.keys().contains(hwc))
                    if(m_sampleFreqMap[hwc]!=sampleFreq)//controllo che questo canale abbia una sola frequenza di campionamento
                    {qCritical()<<"File corrupted";return false;}
                m_sampleFreqMap[hwc]=sampleFreq;
                if(m_bufSizeMap[hwc]<bufMax)
                    m_bufSizeMap[hwc]=bufMax;
            }
        }
        else
            qCritical()<<"missing hw channel";

        foreach(QString c,hwchans.split("#"))
            if(!m_totalHWChan.contains(c))
                m_totalHWChan<<c;




    }
    //inizializzo i buffer hardware;
    foreach (QString c,m_totalHWChan) {
        m_bufferMap[c]=new MSignal();
        m_frameMap[c]=m_sampleFreqMap[c]/gcd(m_sampleFreqMap.values());
    }





    //sono a posto così
    qDebug()<<"Configuration file read succesfully!";
    return true;

}

bool MAcqManager::buildConfigurationFile()
{

    Ancestry configPlot;//iniziamo col creare una classe vergine
    //aggiungo il campo graphs
    Ancestry *graph=configPlot.addChild(XML_GRAPHS);
    if(graph==NULL){qCritical()<<"Could not create child";return false;}

    //ok iniziamo con calma a scrivere qualcosa, peschiamo il numero totale di canali
    int32_t chanlNum=m_mng->GetChanNum();
    //qDebug()<<"N° Canali: "<<chanlNum;
    if(chanlNum==0){qCritical()<<"No channels in file";return false;}
    for(int32_t nc=0;nc<chanlNum;nc++)
    {//contiamo i grafici e popoliamo le mappe di associazione
        QString chanName=m_mng->GetChanName(nc);

        //per i grafici devo appendere l'informazione perchè posso avere più canali
        m_chanInPlots["Graph_"+QString::number(m_mng->GetGraph(nc))]<<chanName;
        //per il datafile invece no
        m_dataChanNameMap[chanName]=nc;

    }
    //bene ora ho una mappa dei grafici che dovrò visualizzare vado a riempirla con le info configurabili dall'utente SE CI SONO

    foreach (QString graphName, m_chanInPlots.keys()) {//scorro per ogni grafico
        Ancestry *  graphN=graph->addChild(graphName);
        if(graphN==NULL){qCritical()<<"Could not create child";return false;}
        Ancestry *  prop=graphN->addChild(XML_PROPERTIES);
        Ancestry *  tracks=graphN->addChild(XML_TRACKS);
        if(prop==NULL){qCritical()<<"Could not create child";return false;}
        if(tracks==NULL){qCritical()<<"Could not create child";return false;}
        //ho la certezza che i canali su ogni grafico hanno tutti le stesse proprietà grafiche per cui vado tranquillo
        QString chanName=m_chanInPlots[graphName].first();
        int32_t nc=m_dataChanNameMap[chanName];
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

        foreach (QString chanName, m_chanInPlots[graphName])
        {//qui scrivo le proprietà delle tracce
            Ancestry * trkN=tracks->addChild(chanName);
            if(trkN==NULL){qCritical()<<"Could not create child";return false;}
            trkN->setAttribute(ATT_THICK,"3");
            trkN->setAttribute(ATT_COLOR,"white");
        }
    }
    //ora salvo il file di configurazione come cur.xml
    configPlot.saveToXML(QDir::currentPath()+"/cur.xml");
    qDebug()<<"Configuration file builded succesfully";
    //e mi sovrascrivo il nome perchè tanto le info ce le ho
    m_plotConfigFileName=QDir::currentPath()+"/cur.xml";
    return true;
}

void MAcqManager::checkAutomaticStartStop(QString __which)
{//ok controlliamo se c'è qualche condizione automatica
    //mi salvo il puntatore al livello acquisition
    //qDebug()<<"Start check";
    Ancestry *contitions=m_configUser.getChild("Auto"+__which+"s");
    if(contitions==NULL){qCritical(MEX_CHILD_NOT_ALIVE);return;}
    foreach (Ancestry *condition, contitions->getChildren()) {//scorro le condizioni di autostart anche se ce ne è solo una
        //pesco su quale canale è fatta
        QString chanType=condition->getAttribute(ATT_CHANTYPE);
        int num=condition->getAttribute(ATT_NUM).toInt()-1;//il meno 1 è per ovviare al fatto che si parte da 1
        //e in base a come si chiama vedo che farci
        //qDebug()<<condition->name();
        if(condition->name()==XML_STEP)
        {//condizione a gradino
            Ancestry *childDur=condition->getChild(XML_DURATION);
            if(childDur==NULL){qCritical(MEX_CHILD_NOT_ALIVE);return;}
            Ancestry *childAmp=condition->getChild(XML_AMPLITUDE);
            if(childAmp==NULL){qCritical(MEX_CHILD_NOT_ALIVE);return;}
            //per prima cosa controlliamo quanti campioni è
            int min=childDur->getAttribute(ATT_MIN).toUInt();

            qreal ampMin=childAmp->getAttribute(ATT_MIN).toInt();
            qreal ampMax=childAmp->getAttribute(ATT_MAX).toInt();
            //qDebug()<<"Buffer"<<chanType<<num<<"="<<*(m_channelMap[chanType].at(num));
            //qDebug()<<min<<bufferSize<<ampMax<<ampMin;
            if(m_channelMap[chanType].at(num)->size()<min)
                continue;//non ho ancora abbastanza campioni per decidere skip alla prossima condizione
            //ora quindi sono sicuro che arrivo qui solo quando ho abbastanza campioni

            //controllo se c'è un gradino
            qreal startVal=m_channelMap[chanType].at(num)->first();
            int count=0;
            foreach(qreal sample,(*m_channelMap[chanType].at(num))){
                if(sample-startVal>ampMin && sample-startVal<ampMax)
                    count++;
                else
                    count=0;
            }

            if(count>=min)
            {
                //effetto buffer tengo solo gli ultimi 5 secondi
                foreach(QString type,m_channelMap.keys())
                    foreach(MSignal *sig,m_channelMap[type])
                        sig->saveLastSec(5.0);
                m_saving=true;//posso iniziare a salvare i dati
                qDebug()<<"Start acquiring";
                return;
            }
            else
            {
                qDebug()<<chanType<<num<<(*m_channelMap[chanType].at(num));
            }
        }



    }
    //effetto buffer tengo solo gli ultimi 5 secondi
    foreach(QString type,m_channelMap.keys())
        foreach(MSignal *sig,m_channelMap[type])
            sig->saveLastSec(5.0);

    //qDebug()<<"Stop check";
}

void MAcqManager::saveBuffersToFile()
{//qui so che ho già spedito i campioni al plot per cui salvo sul file ed elimino i campioni dal buffer per sempre

    foreach (QString chanName, m_dataChanNameMap.keys()) {
        int size=chanName.size();
        QString type,number;
        for(int i=0;i<size;i++){
            if(chanName.at(i).isNumber())//se numero
                number.append(chanName.at(i));
            else
                type.append(chanName.at(i));
        }
        if(!m_channelMap.contains(type) || number.toInt()<1 || number.isEmpty())
        {qCritical()<<type<<number<<"Not recognized";return;}
        int index=number.toInt()-1;
        int32_t chanNum=m_dataChanNameMap[chanName];
        //svuta tutto
        qDebug()<<"Saving"<<m_channelMap[type].at(index)->size()<<"samples in channel"<<chanNum;
        while(m_channelMap[type].at(index)->size()>0){
            float v=m_channelMap[type].at(index)->takeFirst();
            m_mng->AppendValue(&chanNum,&v,1);
        }

    }
qDebug()<<"Salvooo"<<m_mng->GetDuration();
}

void MAcqManager::sendBuffersToPlot()
{
    //m_namesToDataChanNum mi dice in base al canale

    foreach(QString plotName,m_chanInPlots.keys())
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        QList<MSignal> channels;

        foreach(QString chanName,m_chanInPlots[plotName]){
            int size=chanName.size();
            QString type,number;
            for(int i=0;i<size;i++){
                if(chanName.at(i).isNumber())//se numero
                    number.append(chanName.at(i));
                else
                    type.append(chanName.at(i));
            }

            if(!m_channelMap.contains(type) || number.toInt()<1 || number.isEmpty())
            {qCritical()<<type<<number<<"Not recognized";return;}
            int index=number.toInt()-1;
            MSignal copy=*(m_channelMap[type].at(index));
            channels<<copy;
        }




        //qDebug()<<"Sending data to"<<plotName<<m_tcpChannels[plotName]->serverPort()<<channels;
        out<<(qint32)channels.size();
        for(int i=0;i<channels.size();i++){
            out<<i;
            out<<(qint32)channels[i].size();
            while(channels[i].size()>0){
                out<<channels[i].takeFirst();
            }
        }
        if(m_tcpChannels.keys().contains(plotName))
            m_tcpChannels[plotName]->sendData(block.data(),block.size());
        else
            qCritical()<<"No server of name"<<plotName;
    }
}

void MAcqManager::removeLastFrame()
{//rimuovo un frame temporale
    foreach(QString hwc,m_totalHWChan)
        m_bufferMap[hwc]->remove(0,m_frameMap[hwc]);
}

bool MAcqManager::updateDataFile()
{//viene chiamata per aggiornare il datafile a seconda del file di configurazione del relativo esame
    Ancestry *channels=m_configAcq.getChild(XML_CHANNELS);
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

void MAcqManager::applyOperations()
{
    //abbiamo l'elenco di operazioni da fare per ogni tipo di canale
    //e le eseguiamo
    foreach (QString type, m_operationMap.keys()) {//per ogni tipo di canale
        foreach(QString op,m_operationMap[type].keys()){
            if(op == "None")
            {//non devo fare nulla per cui copio un frame nei channel giusti
                foreach (QString hwc, m_HWChansMap[type]) {
                    int index=m_HWChansMap[type].indexOf(hwc);
                    for(int i=0;i<m_frameMap[hwc];i++)
                        m_channelMap[type].at(index)->append(m_bufferMap[hwc]->at(i));
                }
            }

            if(op == "Derive")
            {//derivo
                MSignal der;
                foreach (QString hwc, m_HWChansMap[type]) {
                    int index=m_HWChansMap[type].indexOf(hwc);
                    der=m_bufferMap[hwc]->derive(m_operationMap[type][op]);

                    for(int i=0;i<m_frameMap[hwc];i++)
                        m_channelMap[type].at(index)->append(der.at(i));
                }
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
                if(m_bufferMap.keys().contains(QString::number(currChan)))
                    m_bufferMap[QString::number(currChan)]->append(sample);
                else
                    qCritical()<<"Channel non recognized";
            }

        }
        else
            qCritical()<< "currChan out of range"<<currChan;
    }
    //rimuoviamo roba già processata
}

bool MAcqManager::buffersReady()
{//questa funzione controlla se i miei buffer sono pronti
    //controllo se hanno abbastanza campioni per fare le operazioni e
    //contrmporaneamente restituire un frame
    foreach(QString hwchan,m_totalHWChan)
        if(m_bufferMap[hwchan]->size()<m_bufSizeMap[hwchan]+m_frameMap[hwchan])
            return false;
    return true;
}
