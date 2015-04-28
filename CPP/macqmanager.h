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
    Q_PROPERTY(QString configurationFile READ configurationFile WRITE setConfigurationFile NOTIFY configurationFileChanged)
    Q_PROPERTY(QString alarmFile READ alarmFile WRITE setAlarmFile NOTIFY alarmFileChanged)

    QVariantList alarms(){return m_alarmList;}

    QVariantList acqMarkers(){return m_acqMarkerList;}

    QString acqFile(){return m_acqFileName;}
    void setAcqFile(QString __name);

    QString configurationFile(){return m_configurationFileName;}
    void setConfigurationFile(QString __name);

    QString alarmFile(){return m_alarmFileName;}
    void setAlarmFile(QString __name);

    static void dataOnTCP(QObject *__pParent=NULL, SimpleTCPClient *__pTCP=NULL, QByteArray __block=QByteArray());

signals:
    void alarmsChanged();
    void acqMarkersChanged();
    void acqFileChanged();
    void configurationFileChanged();
    void alarmFileChanged();

public slots:

    bool newAcquisition(QString __newName="");
    void connectToServers();
    void startSupe(QString __mode);
    void endAcquisition(QString __exit);
    void addMarker(QVariant __key, QVariant __descr);
    void addDefiner(bool __startEnd,QVariantList __info);    
    bool sendStartAcq(void);
    bool sendStopAcq (void);
    void resetAlarms();
    void setAlarms(QVariantList __list);
private slots:
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command){return sendCommand((tcp_flow_bt_cmd_t)__command);}

private:
    QStringList m_serversNames;

    QString m_acqFileName,
    m_configurationFileName,
    m_alarmFileName;

    bool    m_acqFileOpened,
    m_serverReady,
    m_sendingToPlot,
    m_configurationFileLoaded,
    m_alarmFileLoaded,
    m_saving;

    QByteArray m_sendingPack;

    float m_oldSample;//BUG deve essere aggiunta la classe di filtraggio

    DatafileManager *m_mng;

    QVariantList m_alarmList,m_acqMarkerList;//BUG aggiungere la definer list se servirà

    QMap<QString,SimpleTCPClient *> m_tcpClients;//elenco dei client attivi

    QMap<QString,SimpleTCPChannel *> m_tcpChannels;//canali di comunicazione verso l'esterno

    QMap<QString,QList<int> > m_chanInPlots;

    QMap<int,QList<qreal> > m_chanMapValues;

    QMap<int,bool> m_automaticChannelsMap;

    QMap<int,QString> m_plotOfChannelMap;

    QMap<QString,QList<qreal> > m_bufferChanMap;

    QVector<VarMap> m_acqMarker;

    Ancestry m_configuration,m_alarmStrings;

    AcqData m_acqData;

    QProcess *m_superProcess;

    AlarmManager m_alarmMng;

    flowBT_states_t m_oldState;



    void updateAcqData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConfiguration(QString __name);
    bool loadAlarms(QString __name);
    void saveConfiguration();
    bool loadConnectivityInfo(Ancestry *__info);
    void analyzeStatus(flowBT_status_t __status);
    void analyzeAlarms(alarms_t __alarms);
    bool newAcqFromConfigFile();
    bool newAcqFromPIC();
    bool buildConfigurationFile();
    void sendToPlots();
    void checkAutomaticStartStop(QString __which);

};



#endif // MACQMANAGER_H
