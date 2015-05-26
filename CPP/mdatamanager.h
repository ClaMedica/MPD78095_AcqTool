#ifndef MDATAMANAGER_H
#define MDATAMANAGER_H

#include "mabstractmanager.h"
#include "mstorage.h"
#include "modelmanager.h"

class MDataManager : public MAbstractManager
{
    Q_OBJECT
public:
    explicit MDataManager(QObject *parent = 0);
    ~MDataManager();
    Q_PROPERTY(QVariantList infoList READ infoList WRITE setInfoList NOTIFY infoListChanged)
    Q_PROPERTY(QStringList availableTracks READ availableTracks NOTIFY availableTracksChanged())
    Q_PROPERTY(QStringList availableData READ availableData NOTIFY availableDataChanged)

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    QVariantList infoList(){return m_infoList;}
    void setInfoList(QVariantList __list);

    //analysis manager

    bool addSignal(MSignal *__pSignal);
    void storeNews(QString __family, QString __name, qulonglong __element);


signals:
    void dataNewsChanged();
    void availableDataChanged();
    void alarmsChanged();
    void availableTracksChanged();
    void infoListChanged();

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
    m_plotConfigFileName,
    m_fileName,
    m_configurationFileName,
    m_applicationPath;

    Ancestry m_configLocale,    //è la prima ad essere caricata e contiene la lingua
    m_configAcq,                //contiene le info fisse di acquisizione
    m_configUser;               //contiene le info modificate dall'utente

    MSignal *m_pCurrentSignal;

    AnalysisType m_anaType;

    bool    m_updateWhenNews,
    m_configurationFileLoaded,
    m_changesToBeSaved;

    float m_start,m_end;

    DatafileManager *m_copy;



    MStorage m_storage;

    QMap<QString,QStringList> m_modelMap;


//----
    bool saveDataAndUpdate(QString __family, QString __name, VarMapVec*__elements,bool __whatIfAlreadyPresent=OVERWRITE);
    void updateAvailableData();   
};



#endif // MDATAMANAGER_H
