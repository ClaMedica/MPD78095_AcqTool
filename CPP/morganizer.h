#ifndef MORGANIZER_H
#define MORGANIZER_H

#include <mdatastorage.h>
#include <modelmanager.h>
#include <global.h>
#include <QObject>

class MOrganizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList infoList READ infoList WRITE setInfoList NOTIFY infoListChanged)
    Q_PROPERTY(QStringList availableTracks READ availableTracks NOTIFY availableTracksChanged)
    Q_PROPERTY(QStringList availableData READ availableData NOTIFY availableDataChanged)

public:
    explicit MOrganizer(QObject *parent = 0);

    QVariantList infoList(void){return m_infoList;}
    void setInfoList(QVariantList __list);



    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

signals:
    void infoListChanged();
    void availableDataChanged();
    void availableTracksChanged();
    void saveStoreChanges();

public slots:
    bool storeNews(QVariantList __news);
    //funzione per aggiungere un nuovo oggetto grafico come marker o definer
    bool addCustomObj(QStringList __families, QString __name, QString __type, QVariantList __info=QVariantList());
    void addModel(QString __type,QStringList __roles);
    void save(void);
    QVariantList getData(QString __type);
    QVariantList getPlotLimits(void);
    bool changeObject(QVariantList __curObj);
    QVariant getSignal(QString __name);
    QStringList getLinks(QString __what, QStringList __filterFamily=QStringList(),QStringList __filterType=QStringList());
    void resetAll();
    QVariant storage(void){return (qulonglong)&m_storage;}
private:
    /// lista delle info di entrata:
    /// [[plotName1,[traccia1,markers,ecc]][plotName2,[traccia2,markers,ecc]]]
    QVariantList m_infoList;

    QStringList m_possibleTypes;
    MDataStorage m_storage;

    QMap<QString,QStringList> m_modelMap;

    QMap<QString,QStringList> m_data;
    QStringList m_availableData;
    void updateAvailableData();
};

#endif // MORGANIZER_H
