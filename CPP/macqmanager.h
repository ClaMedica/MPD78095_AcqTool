#ifndef MACQMANAGER_H
#define MACQMANAGER_H

#include <QQueue>

#include "mabstractmanager.h"
#include "udpmsgs.h"


const double COEFDigFilter[6] = {
    0.06667948314423,   0.1945929082815,   0.2888590053723,   0.2888590053723,
     0.1945929082815,  0.06667948314423};//somma = 1,1002627935295

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
    Q_INVOKABLE void send_Command(int __command);    
    Q_INVOKABLE int manAutoQml();

signals:
    void alarmsChanged();
    void acqMarkersChanged();
//    void acquisitionStarted();
    void systemInAcqStatus();
    void acquisitionEnded();

    void udpBtStopped();
    void udpBtRestarted();
    void udpBtUsable(bool);
    void udpPrnStatus(char);

public slots:

    bool newAcquisition(QString __dataFile = "");
    void connectToServers();
    void endAcquisition(bool discard = false);
    void endAcquisitionSave();
    void endAcquisitionDiscard();
    void addMarker(QVariant __key);
    void addDefiner(bool __startEnd, QVariantList __info);
    bool sendStartAcq(void);
    bool sendStopAcq (void);
    void resetAlarms();
    void setAlarms(QVariantList __list);
    void udpBtDecode(enum WHO __from, QByteArray __msg);
    void sendBeakerOkToSupe();

//private slots:
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command) { return sendCommand((tcp_flow_bt_cmd_t) __command); }

private:
    QStringList m_serversNames,
                m_channelNames,//lista dei nomi dei canali
                m_totalHWChan,//lista dei canali hw che ci sono
                m_superList;    //lista dei supervisori che dovrA? avviare

    bool    m_acqFileOpened,
    m_supeConnected,    //mi indica quando il supervisore A? connesso
    m_serverReady,
    m_sendingToPlot,
    m_saving,
    m_acqFinished,  //mi dice se ho finito di acquisire
    m_autoStartStop;//mi dice se il controllo e' abilitato o meno
    bool m_startAcqManuale; //tasto start

    QByteArray m_sendingPack;

    QVariantList m_alarmList,m_acqMarkerList;//#BUG aggiungere la definer list se servirA 

    QMap<QString,SimpleTCPClient *> m_tcpClients;//elenco dei client attivi
    QMap<QString,int>   m_sampleFreqMap, //mi dice per ogni canale fisico la frequenza di campionamento
                        m_frameMap,//mi dice quanti campioni cavare via da ogni buffer ad ogni ciclo di controllo
                        m_bufSizeMap;//mi dice per ogni buffer la dimensione da tenere per essere pronti


    QMap<QString,MSignal *> m_bufferMap;//mappa dei buffer fisici
    QMap<QString,QList<MSignal *> > m_channelMap;// in questa mappa ho tutti i canali del datafile elencati per tipo: dentro ho poi la lista
    QMap<QString,QStringList>   m_operationMap,// ho l'elenco delle operazioni da fare per ogni tipo di canale
                                m_HWChansMap; //ho l'elenco dei  canali hw coinvolti per ogni tipo di canale

    QVector<VarMap> m_acqMarker;

    int m_lenMMobile;                       //lunghezza vettore per media mobile
    QVector<double> m_buffer_MMobileF;      //buffer per il calcolo della media mobile sulla derivata del flusso
    QVector<double> m_buffer_MMobileV;      //buffer per il calcolo della media mobile sul volume
    double m_sommaMMobileF;                 //somma valori media mobile flusso
    double m_sommaMMobileV;                 //somma valori media mobile volume
    int m_lenDifFilter;                     //lunghezza vettore filtro digitale
    QVector<double> m_buffer_DigFilter;     //buffer per il calcolo del filtro digitale sulla derivata del flusso
    double m_sommaCoef;                     //somma valori dei coefficenti
    double m_valPrecVolume;                 //valore precedente di volume

    Ancestry m_configAcq;                //contiene le info per l'acquisizione

    AlarmManager    m_alarmMng;         //gestore allarmi

    uint8_t m_oldState;
    QQueue<int>     m_newStateQ;

    MSignal m_stopBuffer;

    void updateAcqData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConnectivityInfo(Ancestry *__info);
    void analyzeStatus(uint8_t __currState, bool __isBT);
    bool newAcqFromConfigFile();
    bool newAcqFromPIC();
    void checkAutomaticStartStop(QString __which);
    bool checkAutomaticFlow();
    void saveBuffersToFile();
    void sendBuffersToPlot();
    void removeLastFrame();
    bool handleDataFile();
    void applyOperations();
    void fillBuffers(QByteArray __block);
    bool buffersReady();
    bool readConfigurationFile();

    //to manage interrupted file
    QString m_itsok;
    QFile *OutFile;

    //codice software
    int m_codSoft;
    //stringa calibrazione
    QString m_calibCella;
    //bool per sapere se è stato mandato uno startwithzero
    bool m_startWithZero;
    QString m_fileVerifica;
    bool m_disableWeightFilt;

    bool m_noBeaker;
    bool m_fullBeaker;
    bool m_startReset;
    int m_wrongSamples;

};



#endif // MACQMANAGER_H
