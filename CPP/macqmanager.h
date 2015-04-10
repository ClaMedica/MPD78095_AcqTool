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

class AlarmTimer : public QObject
{
    Q_OBJECT
public:
    AlarmTimer(TimeoutAlarmRecord __code);
    ~AlarmTimer();
    void reset();
    void start(int __msec);

signals:
    void timeout(int);
public slots:
    void send();
private:
    QTimer *m_tim;
    int m_code;

};

class MAcqManager : public QObject
{
    Q_OBJECT
public:
    explicit MAcqManager(QObject *parent = 0);
    ~MAcqManager();

    Q_PROPERTY(QVariantList alarms READ alarms NOTIFY alarmsChanged)
    Q_PROPERTY(QVariantList acqMarkers READ acqMarkers NOTIFY acqMarkersChanged)
    Q_PROPERTY(QString acqFile READ acqFile WRITE setAcqFile NOTIFY acqFileChanged)
    Q_PROPERTY(QString configurationFile READ configurationFile WRITE setConfigurationFile NOTIFY configurationFileChanged)

    QVariantList alarms(){return m_alarmList;}
    QVariantList acqMarkers(){return m_acqMarkerList;}

    QString acqFile(){return m_acqFileName;}
    void setAcqFile(QString __name);

    QString configurationFile(){return m_configurationFileName;}
    void setConfigurationFile(QString __name);

    static void dataOnTCP(QObject *__pParent=NULL, SimpleTCPClient *__pTCP=NULL, QByteArray __block=QByteArray());

signals:
    void alarmsChanged();
    void acqMarkersChanged();
    void acqFileChanged();
    void configurationFileChanged();

public slots:

    bool newAcquisition(QString __newName="", QVariantList __info=QVariantList());
    void connectToServers();
    void startSupe();
    void saveAcquisition();
    void deleteAcquisition();
    void addMarker(QVariant __key, QVariant __descr);
    void addDefiner(bool __startEnd,QVariantList __info);    
    bool sendStartAcq(void){return sendCommand(1);}
    bool sendStopAcq (void){return sendCommand(3);}
    void addAlarm(int __code);
    void resetAlarms();

private slots:
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command){return sendCommand((tcp_flow_bt_cmd_t)__command);}
private:
    QStringList m_serversNames;

    QString m_acqFileName,
    m_configurationFileName;

    bool    m_acqInProgress,
    m_serverReady,
    m_sendingToPlot,
    m_configurationFileLoaded;

    QByteArray m_sendingPack;

    float m_oldSample;//BUG deve essere aggiunta la classe di filtraggio

    DatafileManager *m_mng;

    QVariantList m_alarmList,m_acqMarkerList;//BUG aggiungere la definer list se servirà

    QMap<QString,SimpleTCPClient *> m_tcp;//elenco dei client attivi

    SimpleTCPChannel *m_visualChannel,*m_commandChannel;//canali di comunicazione verso l'esterno

    QVector<VarMap> m_acqMarker,m_alarms;

    Ancestry m_configuration;

    AcqData m_acqData;

    QProcess *m_superProcess;

    QMap<int,AlarmTimer *> m_ATMap;

    void updateAlarms();
    void updateAcqData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConfiguration(QString __name);
    void saveConfiguration();
    bool loadConnectivityInfo(Ancestry *__info);
    void analyzeStatus(flowBT_status_t __status);
    void analyzeAlarms(alarms_t __alarms);

};



#endif // MACQMANAGER_H
