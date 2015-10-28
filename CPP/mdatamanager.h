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

    Q_PROPERTY(int valVolRes READ getValVolRes WRITE setValVolRes NOTIFY infoValVolRes)

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    QVariantList infoList(){return m_infoList;}
    void setInfoList(QVariantList __list);

    int getValVolRes();
    void setValVolRes(int __val);


    //analysis manager

    bool addSignal(MSignal *__pSignal);
    void storeNews(QString __family, QString __name, qulonglong __element);


signals:
    void dataNewsChanged();
    void availableDataChanged();
    void alarmsChanged();
    void availableTracksChanged();
    void infoListChanged();
    void loadingCompleted();
    void reloadingCompleted();

    void sg_openVolResDlg(QString __tipoAn);
    void infoValVolRes();

public slots:
    void analysis(void);
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

     void volRelDlgOk(QString __anaType);

private:
    QVector<MSignal *> m_signalVector;

    QMap<QString,QStringList> m_data;

    QStringList m_availableData,
    m_possibleCategories,
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

    QString m_VolRes;
    bool m_analized;
    bool m_autoPrint;

//----
    bool saveDataAndUpdate(QString __family, QString __name, VarMapVec*__elements,bool __whatIfAlreadyPresent=OVERWRITE);
    void updateAvailableData();
    bool buildInfoList();
    void updateInfoList();


    void InitPageGraphs(QString __anaType);
    bool InitArraysFLW(int __start, int __end, QVector<unsigned char> __chEn, int __curDef, byte __auto);
    int ReadResult(int __numEv = 1);
};




#endif // MDATAMANAGER_H
