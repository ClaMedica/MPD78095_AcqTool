#ifndef MACQMANAGER_H
#define MACQMANAGER_H

#include "mabstractmanager.h"

class MAcqManager : public MAbstractManager
{
    Q_OBJECT
public:
    explicit MAcqManager(QObject *parent = 0);
    ~MAcqManager();

    Q_PROPERTY(QVariantList alarms READ alarms WRITE setAlarms NOTIFY alarmsChanged)
    Q_PROPERTY(QVariantList acqMarkers READ acqMarkers NOTIFY acqMarkersChanged)

    QVariantList alarms(){return m_alarmList;}
    QVariantList acqMarkers(){return m_acqMarkerList;}

    static void dataOnTCP(QObject *__pParent=NULL, SimpleTCPClient *__pTCP=NULL, QByteArray __block=QByteArray());

signals:
    void alarmsChanged();
    void acqMarkersChanged();
    void acquisitionStarted();
    void systemInAcqStatus();

public slots:

    bool newAcquisition(QString __dataFile="");
    void connectToServers();
    void startSupe(QString __mode);
    void endAcquisition(QString __exit);
    void sendIAmReady();
    void addMarker(QVariant __key);
    void addDefiner(bool __startEnd,QVariantList __info);
    bool sendStartAcq(void);
    bool sendStopAcq (void);
    void resetAlarms();
    void setAlarms(QVariantList __list);

private slots:
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command){return sendCommand((tcp_flow_bt_cmd_t)__command);}

private:
    QStringList m_serversNames,
                m_totalHWChan,//lista dei canali hw che ci sono
                m_superList;    //lista dei supervisori che dovrò avviare

    int m_tcpAttempts;//contiene il numero di tentativi che ci metto per connettermi ai server
    bool    m_acqFileOpened,
    m_supeConnected,    //mi indica quando il supervisore è connesso
    m_serverReady,
    m_sendingToPlot,
    m_saving,
    m_autoStartStop;//mi dice se il controllo è abilitato o meno

    QByteArray m_sendingPack;

    QVariantList m_alarmList,m_acqMarkerList;//#BUG aggiungere la definer list se servirà

    QMap<QString,SimpleTCPClient *> m_tcpClients;//elenco dei client attivi
    QMap<QString,int>   m_sampleFreqMap, //mi dice per ogni canale fisico la frequenza di campionamento
                        m_frameMap,//mi dice quanti campioni cavare via da ogni buffer ad ogni ciclo di controllo
                        m_bufSizeMap;//mi dice per ogni buffer la dimensione da tenere per essere pronti


    QMap<QString,MSignal *> m_bufferMap;//mappa dei buffer fisici
    QMap<QString,QList<MSignal *> > m_channelMap;// in questa mappa ho tutti i canali del datafile elencati per tipo: dentro ho poi la lista
    QMap<QString,QStringList>   m_operationMap,// ho l'elenco delle operazioni da fare per ogni tipo di canale
                                m_HWChansMap; //ho l'elenco dei  canali hw coinvolti per ogni tipo di canale

    QVector<VarMap> m_acqMarker;

    Ancestry m_configAcq;                //contiene le info per l'acquisizione

    QProcess *m_superProcess;

    AlarmManager    m_alarmMng;         //gestore allarmi

    flowBT_states_t m_oldState;

    MSignal m_stopBuffer;

    void updateAcqData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConnectivityInfo(Ancestry *__info);
    void analyzeStatus(flowBT_status_t __status);
    void analyzeAlarms(alarms_t __alarms);
    bool newAcqFromConfigFile();
    bool newAcqFromPIC();
    void checkAutomaticStartStop(QString __which);
    void saveBuffersToFile();
    void sendBuffersToPlot();
    void removeLastFrame();
    bool handleDataFile();
    void applyOperations();
    void fillBuffers(QByteArray __block);
    bool buffersReady();
    bool readConfigurationFile();
};



#endif // MACQMANAGER_H
