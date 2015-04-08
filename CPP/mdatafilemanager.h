#ifndef MDATAFILEMANAGER_H
#define MDATAFILEMANAGER_H

#include <QObject>
#include <mplayer.h>
#include "datafilemanager.h"
#include "morganizer.h"
#include "SimpleTCPClient.h"
#include "TcpServers.h"
#include "ancestry.h"
#include "msignal.h"
#include "tabble.h"
#include "flowBT_types.h"
#include "TcpServerFlowBt_types.h"

class MDataFileManager : public QObject
{
    Q_OBJECT
public:
    explicit MDataFileManager(QObject *parent = 0);
    ~MDataFileManager();

    Q_PROPERTY(MOrganizer* organizer READ organizer NOTIFY organizerChanged)
    Q_PROPERTY(QVariantList infoList READ infoList WRITE setInfoList NOTIFY infoListChanged)
    Q_PROPERTY(QVariantList dataNews READ dataNews NOTIFY dataNewsChanged)
    Q_PROPERTY(QStringList availableTracks READ availableTracks NOTIFY availableTracksChanged())
    Q_PROPERTY(QStringList availableData READ availableData NOTIFY availableDataChanged)
    Q_PROPERTY(QString currentSignal READ currentSignal WRITE setCurrentSignal NOTIFY currentSignalChanged)
    Q_PROPERTY(QString acqFile READ acqFile WRITE setAcqFile NOTIFY acqFileChanged)
    Q_PROPERTY(QString configurationFile READ configurationFile WRITE setConfigurationFile NOTIFY configurationFileChanged)

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    MOrganizer *organizer();
    QVariantList infoList(){return m_org->infoList();}
    void setInfoList(QVariantList __list);

    QVariantList dataNews();

    QString currentSignal();
    bool setCurrentSignal(QString __name);

    QString acqFile(){return m_acqFileName;}
    void setAcqFile(QString __name);

    QString configurationFile(){return m_configurationFileName;}
    void setConfigurationFile(QString __name);

    //analysis manager

    bool addSignal(MSignal *__pSignal);
    void sendNews(QString __family, QString __name, qulonglong __element);
    static void dataOnTCP(QObject *__pParent=NULL, SimpleTCPClient *__pTCP=NULL, QByteArray __block=QByteArray());

signals:
    void dataNewsChanged();
    void availableDataChanged();
    void availableTracksChanged();
    void currentSignalChanged();
    void organizerChanged();
    void infoListChanged();
    void connectivityInfoChanged();
    void acqFileChanged();
    void configurationFileChanged();

public slots:
    void loadFile(QString __fileName);
    float getStartTime(){return m_start;}
    float getEndTime(){return m_end;}
    QVariantList getAcqMarkers(void);
    void resetAll(void);
    void createOrganizer();
    void saveChanges(QVariant __storage);
    bool newAcquisition(QString __newName="", QVariantList __info=QVariantList());
    void startSupe();
    void saveAcquisition();
    void deleteAcquisition();
    void addMarker(QVariant __key, QVariant __descr);
    void addDefiner(bool __startEnd,QVariantList __info);
    bool sendCommand(tcp_flow_bt_cmd_t __command);
    bool sendCommand(int __command){return sendCommand((tcp_flow_bt_cmd_t)__command);}

private:
    QVector<MSignal *> m_signalVector;

    QMap<QString,QStringList> m_data;

    QStringList m_availableData,
    m_serversNames,
    m_filesLoaded;//contiene l'elenco di tutti i file che sono stati aperti e di cui vi sono i dati disponibili per l'utente

    QString m_currentSignalName,
    m_fileName,
    m_acqFileName,
    m_configurationFileName;

    MSignal *m_pCurrentSignal;

    AnalysisType m_anaType;

    bool    m_updateWhenNews,
    m_acqInProgress,
    m_serverReady,
    m_sendingToPlot,
    m_configurationFileLoaded;

    QByteArray m_sendingPack;

    float m_start,m_end,m_oldSample;

    DatafileManager *m_mng,*m_copy;

    QVariantList m_morphPointer,m_infoList;

    MOrganizer *m_org;

    QMap<QString,SimpleTCPClient *> m_tcp;

    SimpleTCPChannel *m_visualChannel,*m_commandChannel;

    QVector<VarMap> m_acqMarker;

    Ancestry m_configuration;

    AcqData m_acqData;

    QProcess *m_superProcess;

    void updateAvailableData();
    void handleTCP(SimpleTCPClient *__client, QByteArray __block);
    void initializeServers();
    bool loadConfiguration(QString __name);
    void saveConfiguration();
    bool loadConnectivityInfo(Ancestry *__info);

};



#endif // MDATAFILEMANAGER_H
