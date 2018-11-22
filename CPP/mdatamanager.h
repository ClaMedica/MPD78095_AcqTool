#ifndef MDATAMANAGER_H
#define MDATAMANAGER_H

#include "mabstractmanager.h"
#include "mstorage.h"
#include "modelmanager.h"
#include "mflowdatas.h"
#include "printermanager.h"
#include <QQuickItemGrabResult>


typedef struct {
    float waiting_time;			// waiting time
    float q_max;					// maximum flow
    float q_ave;					// average flow
    float time_at_v3;             // time at Vol/3
    float time_at_v2;             // time at Vol/2
    float time_at_qmax;			// time at Q max
    float time_90;				// time between 5% and 95% of the voided volume
    float flow_time;				// flow time
    float desc_time;				// time between Q max and 95% of the voided volume
    float voiding_time;			// voiding time
    float vol_at_qmax;			// volume at Q max
    int32_t voided_volume;          // voided volume
    float acceleration;           // Q max / T qmax
    int32_t residual_volume;		// residual volume inserted by the user
    float v_det_max;              // detrusor contraction maximum speed
    float cQ;						// Flow corrective factor

} FLWAdvRepStruct;


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
    Q_PROPERTY(QString toSave READ getToSave WRITE setToSave NOTIFY infoToSave)

    Q_PROPERTY(int numAnaFlwAdv READ getNumAnaFlwAdv)
    Q_INVOKABLE int getNumAnaFlwAdv(){return m_aflwdatas.length();}

    Q_INVOKABLE void getGrabbedImage(QObject *gi, QString nome);
    Q_INVOKABLE bool getAutoPrint(){return m_autoPrint;}
    Q_INVOKABLE int getAutoFlow(){return m_autoFlow;}

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    QVariantList infoList(){return m_infoList;}
    void setInfoList(QVariantList __list);

    int getValVolRes();
    void setValVolRes(int __val);

    QString getToSave(){return m_toSave;}
    void setToSave(QString __val);

    //analysis manager
    bool addSignal(MSignal *__pSignal);
    void storeNews(QString __family, QString __name, qulonglong __element);


    Q_INVOKABLE mflowdatas *getFlowDatas(int __i);

signals:
    void dataNewsChanged();
    void availableDataChanged();
    void alarmsChanged();
    void availableTracksChanged();
    void infoListChanged();
    void loadingCompleted();
    void reloadingCompleted();

    void sg_openVolResDlg(QString __tipoAn);
    void sg_loadResult();
    void infoValVolRes();
    void infoToSave();

    void sg_exitFromReview();

public slots:
    void analysis(void);

    void exitFromReview();
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
    void startPrint();
    void sendToPrint();

    void sendPrintTest();


private:
    QVector<MSignal *> m_signalVector;

    QMap<QString,QStringList> m_data;

    QStringList m_availableData,
    m_possibleCategories,
    m_filesLoaded;//contiene l'elenco di tutti i file che sono stati aperti e di cui vi sono i dati disponibili per l'utente

    QString m_currentSignalName,
    m_fileName,
    m_copyFileName,
    m_pathData;

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
    QString m_toSave;

    bool    m_analized;
    bool    m_autoPrint;
    int     m_autoFlow;     //Modalita dell'esame 0=auto; 2=manual
    bool    m_Siroky;       //se stampare Siroky
    bool    m_Liverpool;    //se stampare Liverpool
    bool    m_landscape;    //modalità di stampa: portrait o landascape (ture se landscape)   
    bool    m_sexPatient;   //true se donna, false se uomo
    QString m_firstHead;    //primo header della stmpa personalizzabile
    QString m_secondHead;   //secondo header della stampa personalizzabile

    int m_numAna; //numero di analisi --> non sappiamo se serve
    QVector<mflowdatas*> m_aflwdatas; //array di analisi di tipo flussimetria

    printermanager *m_mngPrint;


//----
    bool saveDataAndUpdate(QString __family, QString __name, VarMapVec*__elements,bool __whatIfAlreadyPresent=OVERWRITE);
    void updateAvailableData();
    bool buildInfoList();
    bool updateInfoList();


    void InitPageGraphs(int __anaType);
    bool InitArraysFLW(int __start, int __end, QVector<unsigned char> __chEn, int __curDef, unsigned char __auto);
    int ReadResult(int & __numEv);
    bool checkForVolRes();
};




#endif // MDATAMANAGER_H
