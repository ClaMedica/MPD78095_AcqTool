#ifndef MACQMANAGER_H
#define MACQMANAGER_H

#include <QObject>
#include <mplayer.h>
#include "datafilemanager.h"
#include "SimpleTCPClient.h"
#include "TcpServers.h"
#include "ancestry.h"
#include "msignal.h"
#include "tabble.h"
#include "flowBT_types.h"
#include "TcpServerFlowBt_types.h"
#include "inheritEnum.h"
#include "alarmmanager.h"
#include "QtGlobal"


class MAcqManager : public QObject
{
    Q_OBJECT    
public:
    explicit MAcqManager(QObject *parent = 0);
    ~MAcqManager();


    Q_PROPERTY(QVariantList alarms READ alarms WRITE setAlarms NOTIFY alarmsChanged)
    Q_PROPERTY(QVariantList acqMarkers READ acqMarkers NOTIFY acqMarkersChanged)
    Q_PROPERTY(QString acqFile READ acqFile WRITE setAcqFile NOTIFY acqFileChanged)

    QVariantList alarms(){return m_alarmList;}

    QVariantList acqMarkers(){return m_acqMarkerList;}

    QString acqFile(){return m_acqFileName;}
    void setAcqFile(QString __name);

    static void dataOnTCP(QObject *__pParent=NULL, SimpleTCPClient *__pTCP=NULL, QByteArray __block=QByteArray());

signals:
    void alarmsChanged();
    void acqMarkersChanged();
    void acqFileChanged();

public slots:

    bool load(void);
    bool newAcquisition(QString __dataFile="");
    void connectToServers();
    void startSupe(QString __mode);
    void endAcquisition(QString __exit);
    void addMarker(QVariant __key, QVariant __descr);
    void addDefiner(bool __startEnd,QVariantList __info);    
    bool sendStartAcq(void);
    bool sendStopAcq (void);
    void resetAlarms();
    void setAlarms(QVariantList __list);
    QString plotConfigFileName(){return m_plotConfigFileName;}


private slots:
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command){return sendCommand((tcp_flow_bt_cmd_t)__command);}

private:
    QStringList m_serversNames,
                m_superList;    //lista dei supervisori che dovrò avviare

    QString m_acqFileName,

    m_plotConfigFileName,
    m_applicationPath;

    bool    m_acqFileOpened,
    m_serverReady,
    m_sendingToPlot,
    m_saving;

    QByteArray m_sendingPack;

    DatafileManager *m_mng;

    QVariantList m_alarmList,m_acqMarkerList;//#BUG aggiungere la definer list se servirà

    QMap<QString,SimpleTCPClient *> m_tcpClients;//elenco dei client attivi

    QMap<QString,SimpleTCPChannel *> m_tcpChannels;//canali di comunicazione verso l'esterno

    QMap<QString,QList<int> > m_chanInPlots;//associa nome plot ad una lista di canali del datafile che ci vanno disegnati dentro


    QMap<QString,int32_t> m_namesToDataChanNum; //in base al nome del canale memorizzo il numero del canale del datafile
    QMap<QString,MSignal> m_channelsMap;// in questa mappa ho tutti i canali del datafile elencati per nome
    QMap<QString,QStringList>   m_operationMap,// ho l'elenco delle operazioni da fare per ogni tipo di canale
                                m_involvedChansMap; //ho l'elenco dei canali hw coinvolti per ogni tipo di canale
    QMap<uint,int32_t> m_HWChannelMap;//in base all'indice del canale fisico ottengo in che canale del datafile memorizzarlo
    QMap<uint,MSignal > m_HBufferMap;//mappa dei buffer hardware in base al canale fisico

    QMap<QString,int32_t> m_SWChannelMap;//in base al nome del canale software ottengo in che canale del datafile memorizzarlo
    QMap<QString,MSignal > m_SBufferMap;//mappa dei buffer software in base al nome

    QVector<VarMap> m_acqMarker;

    Ancestry m_configLocale,    //è la prima ad essere caricata e contiene la lingua
    m_configAcq,                //contiene le info fisse di acquisizione
    m_configUser;               //contiene le info modificate dall'utente

    AcqData m_acqData;

    QProcess *m_superProcess;

    AlarmManager m_alarmMng;

    flowBT_states_t m_oldState;

    void updateAcqData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConnectivityInfo(Ancestry *__info);
    void analyzeStatus(flowBT_status_t __status);
    void analyzeAlarms(alarms_t __alarms);
    bool newAcqFromConfigFile();
    bool newAcqFromPIC();
    bool readConfigurationFile();
    bool buildConfigurationFile();
    void bufferManager();
    void checkAutomaticStartStop(QString __which);
    void saveBuffersToFile();
    void sendBuffersToPlot();
    bool updateDataFile();
    void calculateSoftwareChannels();
    void fillBuffers(QByteArray __block);

};



#endif // MACQMANAGER_H
