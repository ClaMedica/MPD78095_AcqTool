#ifndef MABSTRACTMANAGER_H
#define MABSTRACTMANAGER_H

#include <QObject>
#include <datafilemanager.h>
#include <analysis.h>
#include "SimpleTCPClientAcq.h"
#include "TcpServers.h"
#include "ancestry.h"
#include "msignal.h"
#include "flowBT_types.h"
#include "PicoFlow_types.h"
#include "TcpServerPicoFlow_types.h"//TcpServerFlowBt_types.h"
#include "inheritEnum.h"
#include "alarmmanager.h"
#include "p7settingsmanager.h"
#include "QtGlobal"
//classe per unita di misura
#include "UdmImpl.h"

#ifdef PICOFLOW
#include "appbridge.h"
extern AcqBridge *g_mainAppBridge;
#endif

enum AvvisiUtente {
    AVV_ANMNOINSERT = 0,
    AVV_ANMOUTCH
};

class MAbstractManager : public QObject
{
    Q_OBJECT

public:
    explicit MAbstractManager(QObject *parent = 0);
    ~MAbstractManager();

    //per traduzioni
    QString translate(QString __string);
    QMap <QString, QString> trMap;

    Q_INVOKABLE QVariantList markersInfo(QString __filterType = "",
                                         QVariantList __filterValues = QVariantList());
    Q_INVOKABLE QVariantList commandsInfo();
    Q_INVOKABLE QVariantList commandsInfoBottom();
    Q_INVOKABLE QVariantList commandsBottomAcq();
    Q_INVOKABLE QVariantList definersInfo();
    Q_INVOKABLE QVariantList actionsInfo();
    Q_INVOKABLE QVariantList acqInfo();

    Q_PROPERTY(QString patientInfo READ patientInfo NOTIFY patientInfoChanged)

    enum CODICIP {
        ANALISI = 111,
        CHIUDIREV,
        ZOOMIN,
        ZOOMOUT,
        ZOOMNONE,
        DISCARD,
        CHIUDIACQ,
        DELETEALL,
        RISULTATI,
        EXPORT,
        STARTACQ,
        SAVEREW,
        PAUSA
    };
    Q_ENUM(CODICIP)


signals:
    void patientInfoChanged();

public slots:
    bool load(void);
    QString plotConfigFileName();
    QString patientInfo() { return m_patientInfo; }


protected:
    QString m_applicationPath,
    m_patientInfo;                      //nome e altre info paziente
    const char* m_language; //codice lingua da usare

    DatafileManager *m_mng;
    Analyze *m_ana;

    QMap<QVariant,VarMap> m_markerMap;  //mappa di tutti i possibili marker ordinati per key
    QMap<int,int> m_analysisMap;        //mappa le analisi associate all'esame con i definitori

    QMap<QString, QStringList>   m_chanInPlots;  //associa nome plot ad una mappa con cui ripescare il buffer
    QMap<QString, int32_t> m_dataChanNameMap;    //associa il nome del canale al suo indice
    QMap<QString, SimpleTCPChannel *> m_tcpChannels;    //canali di comunicazione verso l'esterno

    Ancestry m_configMarkers,            //contiene le info per i marker
    m_configUser,               //contiene le info modificate dall'utente per acquisizione
    m_configUserProp,               //contiene le info modificate dall'utente per grafica canali
    m_configPrinter,            //contiene le info relative alla stampa
    m_configLang;            //contiene le info relative alla lingua

    QMap<int,QStringList> m_messaggiUtente;

    /// lista delle info di entrata:
    /// [[plotName1,[traccia1,markers,ecc]][plotName2,[traccia2,markers,ecc]]]
    QVariantList m_infoList;

    bool buildConfigurationFile();
    bool buildMarkerInfoMap();
};

#endif // MABSTRACTMANAGER_H
