#ifndef MODELMANAGER_H
#define MODELMANAGER_H
#include <global.h>
#include <QAbstractListModel>
#include <mdatastorage.h>
#include <QObject>

class ModelManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVariantList infoList READ infoList WRITE setInfoList NOTIFY infoListChanged)
    Q_PROPERTY(QStringList roles READ roles WRITE setRoles NOTIFY rolesChanged)    
    ///contiene le informazioni per il plot
    Q_PROPERTY(QVariantList drawList READ drawList NOTIFY drawListChanged)
public:
    explicit ModelManager(QObject *parent = 0);

    QString type(void){return m_type;}
    void setType(QString __type);

    QVariantList infoList(void){return m_infoList;}
    void setInfoList(QVariantList __list);

    QStringList roles(){return m_roles;}
    void setRoles(QStringList __roles);

    QVariantList drawList(){return m_drawList;}

    void setStore(MDataStorage *__pStore){m_pStore=__pStore;}

signals:
    void infoListChanged();
    void rolesChanged();
    void drawListChanged();
    void typeChanged();


public slots:

    //void updateNews();

    //bool addElement(QVariantList __element);
    //bool removeElement(QString __plotName,int __number);

private:
    ///tipo: identifica, con un label riconosciuta dal plot, cosa vuole graficare
    QString m_type;

    /// lista delle info di entrata:
    /// [[plotName1,[traccia1,markers,ecc]][plotName2,[traccia2,markers,ecc]]]
    QVariantList m_infoList;

    ///lista delle info in uscita
    QVariantList m_drawList;

    ///mappa delle info che devo andare a disegnare
    QMap<QString, QStringList> m_infoMap;
    ///mappa locale di tutti gli elementi presenti: nome più puntatore al vettore dell'elemento
    QMap<QString, QMap<QString, VarMapVec*> > m_dataMap;

    ///contiene le role dei miei elementi
    QStringList m_roles;

    ///puntatore al datastorage
    MDataStorage *m_pStore;

    int findElement(int __number);
    void updateModelList();
    void updateInfo();
};

#endif // MODELMANAGER_H
