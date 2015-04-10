#ifndef MDATAMANAGER_H
#define MDATAMANAGER_H

#include <QObject>
#include <mplayer.h>
#include "datafilemanager.h"
#include "mstorage.h"
#include "modelmanager.h"
#include "SimpleTCPClient.h"
#include "TcpServers.h"
#include "ancestry.h"
#include "msignal.h"
#include "tabble.h"
#include "flowBT_types.h"
#include "TcpServerFlowBt_types.h"

class MDataManager : public QObject
{
    Q_OBJECT
public:
    explicit MDataManager(QObject *parent = 0);
    ~MDataManager();
    Q_PROPERTY(QVariantList infoList READ infoList WRITE setInfoList NOTIFY infoListChanged)
    Q_PROPERTY(QStringList availableTracks READ availableTracks NOTIFY availableTracksChanged())
    Q_PROPERTY(QStringList availableData READ availableData NOTIFY availableDataChanged)
    Q_PROPERTY(QString currentSignal READ currentSignal WRITE setCurrentSignal NOTIFY currentSignalChanged)
    Q_PROPERTY(QString configurationFile READ configurationFile WRITE setConfigurationFile NOTIFY configurationFileChanged)

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    QVariantList infoList(){return m_infoList;}
    void setInfoList(QVariantList __list);



    QString currentSignal();
    bool setCurrentSignal(QString __name);

    QString configurationFile(){return m_configurationFileName;}
    void setConfigurationFile(QString __name);

    //analysis manager

    bool addSignal(MSignal *__pSignal);
    void storeNews(QString __family, QString __name, qulonglong __element);


signals:
    void dataNewsChanged();
    void availableDataChanged();
    void alarmsChanged();
    void availableTracksChanged();
    void currentSignalChanged();    
    void infoListChanged();
    void configurationFileChanged();

public slots:
    void loadFile(QString __fileName);
    float getStartTime(){return m_start;}
    float getEndTime(){return m_end;}
    void resetAll(void);
    void saveChanges();
    bool addCustomObj(QStringList __families, QString __name, QString __type, QVariantList __info=QVariantList());
    void registerModel(QString __type,QStringList __roles);
    QVariantList getData(QString __type);//ottengo la lista per disegnare marker definer e tracce
    QVariantList getPlotLimits();
    bool changeObject(QVariantList __curObj);
    QVariant getSignal(QString __name);
    QStringList getLinks(QString __what, QStringList __filterFamily=QStringList(), QStringList __filterType=QStringList());

private:
    QVector<MSignal *> m_signalVector;

    QMap<QString,QStringList> m_data;

    QStringList m_availableData,
    m_possibleTypes,
    m_filesLoaded;//contiene l'elenco di tutti i file che sono stati aperti e di cui vi sono i dati disponibili per l'utente

    QString m_currentSignalName,
    m_fileName,
    m_configurationFileName;

    MSignal *m_pCurrentSignal;

    AnalysisType m_anaType;

    bool    m_updateWhenNews,
    m_configurationFileLoaded,
    m_changesToBeSaved;

    float m_start,m_end;

    DatafileManager *m_mng,*m_copy;

    /// lista delle info di entrata:
    /// [[plotName1,[traccia1,markers,ecc]][plotName2,[traccia2,markers,ecc]]]
    QVariantList m_infoList;

    MStorage m_storage;

    Ancestry m_configuration;

    QMap<QString,QStringList> m_modelMap;


//----
    bool saveDataAndUpdate(QString __family, QString __name, VarMapVec*__elements,bool __whatIfAlreadyPresent=OVERWRITE);
    void updateAvailableData();   
    bool loadConfiguration(QString __name);
    void saveConfiguration();
};



#endif // MDATAMANAGER_H
