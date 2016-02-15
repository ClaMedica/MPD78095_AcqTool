#include "mabstractmanager.h"
#include "QApplication"
MAbstractManager::MAbstractManager(QObject *parent) : QObject(parent)
{
    m_applicationPath=QApplication::applicationDirPath();
}

MAbstractManager::~MAbstractManager()
{

}

QVariantList MAbstractManager::markersInfo(QString __filterType,
                                           QVariantList __filterValues)
{
    //leggiamo il file di configurazione e riempiamo le info
    QVariantList list;
    foreach(VarMap marker,m_markerMap.values())
    {
        if(__filterType!="")
            if(!__filterValues.contains(marker.value(__filterType)))
                continue;
        list<<"$GridElement";
        foreach(QString key,marker.keys())
        {
            list<<key;
            list<<marker.value(key);
        }
        list<<"&GridElement";
    }

    return list;
}

QVariantList MAbstractManager::commandsInfo()
{
    QVariantList list;

    //analisi
    list<<"$GridElement";
    list<<"descr"<<tr("analysis");
    QString rootURL="file:///"+m_applicationPath+"/Icone/";
    list<<"img"<<rootURL+"analisi";
    list<<"key"<<111;
    list<<"visible"<<true;
    list<<"&GridElement";
    //save
    list<<"$GridElement";
    list<<"descr"<<tr("save");
    rootURL="file:///"+m_applicationPath+"/Icone/";
    list<<"img"<<rootURL+"Spoken";
    list<<"key"<<112;
    list<<"visible"<<true;
    list<<"&GridElement";

//    foreach(VarMap marker,m_markerMap.values())
//    {
//        if(__filterType!="")
//            if(!__filterValues.contains(marker.value(__filterType)))
//                continue;
//        list<<"$GridElement";
//        foreach(QString key,marker.keys())
//        {
//            list<<key;
//            list<<marker.value(key);
//        }
//        list<<"&GridElement";
//    }

    return list;
}

bool MAbstractManager::load()
{//in questa funzione inizializzo tutto caricando i file di configurazione fissi
    if(!m_configLocale.loadFromXML(":/Config/Config_Locale.xml"))
    qCritical()<<"Error on locale configuration file";

    if(!m_configUser.loadFromXML(":/Config/Config_User.xml"))
    qCritical()<<"Error on user configuration file";

    if(!m_configMarkers.loadFromXML(":/Config/markers.xml"))
    qCritical()<<"Error on user configuration file";

    buildMarkerInfoMap();
}

QString MAbstractManager::plotConfigFileName()
{
#ifdef ANDROID
    return "/mnt/sdcard/Medica/cur.xml";
#else
    return m_applicationPath+"/cur.xml";
#endif
}



bool MAbstractManager::buildConfigurationFile()
{
    //carico i dati necessari
    int32_t chanlNum=m_mng->GetChanNum();
    qDebug()<<"NÂ° Canali: "<<chanlNum;
    if(chanlNum==0)
        qCritical()<<"No channels in file";
    for(int32_t nc=0;nc<chanlNum;nc++)
    {//contiamo i grafici e popoliamo le mappe di associazione
        QString chanName=m_mng->GetChanName(nc);

        //per i grafici devo appendere l'informazione perchÃ¨ posso avere piÃ¹ canali
        QString graphName="Graph_"+QString::number(m_mng->GetGraph(nc));


        m_chanInPlots[graphName]<<chanName;
        //aggiungo il canale su cui comunicherÃ  questo plot
        m_tcpChannels[graphName]=new SimpleTCPChannel(QHostAddress("127.0.0.1"),9000+m_mng->GetGraph(nc)-1,this);
        //per il datafile invece no
        m_dataChanNameMap[chanName]=nc;

    }

    QString curfile = plotConfigFileName();
    if (QFile(curfile).exists())
    {   //il file già c'è e non va creato uno nuovo ma usiamo quello esistente
        return true;
    }

    //scriviamo un file di configurazione che poi l'utente potrà modificare a suo gusto
    Ancestry configPlot;
    //aggiungo il campo graphs
    Ancestry *graph=configPlot.addChild(XML_GRAPHS);
    if(graph==NULL)
        qCritical()<<"Could not create child";

    foreach (QString graphName, m_chanInPlots.keys()) {//scorro per ogni grafico
        Ancestry *  graphN=graph->addChild(graphName);
        Ancestry *  prop=graphN->addChild(XML_PROPERTIES);
        Ancestry *  tracks=graphN->addChild(XML_TRACKS);

        //ho la certezza che i canali su ogni grafico hanno tutti le stesse proprietÃ  grafiche per cui vado tranquillo
        QString chanName=m_chanInPlots[graphName].first();
        int32_t nc=m_dataChanNameMap[chanName];
        Ancestry *  axis=prop->addChild(XML_AXIS);
        Ancestry *  time=prop->addChild(XML_TIME);
        Ancestry *  network=prop->addChild(XML_NETWORK);

        Ancestry m_configAcq;
        if(!m_configAcq.loadFromXML(g_P7SettingsManager.progPath()+"/Config_Acq.xml"))
            qCritical()<<"Error on acq configuration file";


        Ancestry *channels=m_configAcq.getChild(XML_CHANNELS);
        if(channels==NULL)
            qCritical("Child not alive");

        QString udm = "";
        foreach (Ancestry *channel, channels->getChildren())
        {
            if(channel->getChild(XML_NAME)!=NULL)
                if(chanName.contains(channel->getTextOfChild(XML_NAME))){
           //         if ((channel->getChild(XML_UDM))->getAttribute("ID")== QString::number(m_mng->GetUdM(nc)))
           //         {
                        udm = channel->getTextOfChild(XML_UDM);
           //             break;
            //        }
                }
        }


        axis->setAttribute("yAUOM",udm);
        axis->setAttribute("yAbsoluteMax",QString::number(m_mng->GetSupLim(nc)));
        axis->setAttribute("yAbsoluteMin",QString::number(m_mng->GetInfLim(nc)));
        time->setAttribute("samplingFrq",QString::number(m_mng->GetNAS(nc)));
        time->setAttribute("pageTime",QString::number(m_mng->GetPageTime()));
        network->setAttribute(ATT_PORT,QString::number(9000+nc));
        network->setAttribute(ATT_ADDRESS,"127.0.0.1");

        foreach (QString chanName, m_chanInPlots[graphName])
        {//qui scrivo le proprietÃ  delle tracce
            Ancestry * trkN=tracks->addChild(chanName);
            trkN->setAttribute(ATT_THICK,"3");
            trkN->setAttribute(ATT_COLOR,"black");
        }
    }
    //ora salvo il file di configurazione come cur.xml
#ifdef ANDROID
    configPlot.saveToXML("/mnt/sdcard/Medica/cur.xml");
#else
    configPlot.saveToXML(m_applicationPath+"/cur.xml");
#endif

    qDebug()<<"Configuration file builded succesfully";
    return true;
}

bool MAbstractManager::buildMarkerInfoMap()
{
    foreach(Ancestry *marker,m_configMarkers.getChildren())
    {
        VarMap mark;
        QString img=marker->getAttribute(ATT_IMG);
        QString rootURL="file:///"+m_applicationPath+"/Icone/";
        mark[ATT_IMG]=rootURL+img;
        mark[ATT_KEY]=marker->getAttribute(ATT_KEY);
        mark[ATT_CODE]=marker->getAttribute(ATT_CODE);
        mark[ATT_DESCR]=marker->getAttribute(ATT_DESCR);
        mark[ATT_TYPE]=marker->getAttribute(ATT_TYPE);
        mark["lock"]=true;
        mark["color"]=COLOR_OPERATIVE;
        mark["visible"]=true;
        mark["category"]=CAT_MARKER;
        m_markerMap[mark[ATT_KEY]]=mark;
    }
    qDebug()<<m_markerMap.size()<<"markers loaded!";
    return true;
}
