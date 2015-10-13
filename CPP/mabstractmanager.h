#ifndef MABSTRACTMANAGER_H
#define MABSTRACTMANAGER_H

#include <QObject>
#include <datafilemanager.h>
#include "SimpleTCPClient.h"
#include "TcpServers.h"
#include "ancestry.h"
#include "msignal.h"
#include "flowBT_types.h"
#include "TcpServerFlowBt_types.h"
#include "inheritEnum.h"
#include "alarmmanager.h"
#include "p7settingsmanager.h"
#include "QtGlobal"

class MAbstractManager : public QObject
{
    Q_OBJECT


public:
    explicit MAbstractManager(QObject *parent = 0);
    ~MAbstractManager();

    Q_INVOKABLE QVariantList markersInfo(QString __filterType="",
                                         QVariantList __filterValues=QVariantList());

signals:

public slots:
    bool load(void);
    QString plotConfigFileName();


protected:
    QString m_applicationPath;

    DatafileManager *m_mng;

    QMap<QVariant,VarMap> m_markerMap;//mappa di tutti i possibili marker ordinati per key

    QMap<QString,QStringList>   m_chanInPlots;//associa nome plot ad una mappa con cui ripescare il buffer
    QMap<QString,int32_t> m_dataChanNameMap;//associa il nome del canale al suo indice
    QMap<QString,SimpleTCPChannel *> m_tcpChannels;//canali di comunicazione verso l'esterno
    Ancestry m_configLocale,    //è la prima ad essere caricata e contiene la lingua
    m_configMarkers,            //contiene le info per i marker
    m_configUser;               //contiene le info modificate dall'utente

    /// lista delle info di entrata:
    /// [[plotName1,[traccia1,markers,ecc]][plotName2,[traccia2,markers,ecc]]]
    QVariantList m_infoList;

    bool buildConfigurationFile();
    bool buildMarkerInfoMap();
};

#endif // MABSTRACTMANAGER_H
