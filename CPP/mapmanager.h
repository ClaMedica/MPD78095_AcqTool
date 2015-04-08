#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <global.h>
#include <tabble.h>
#include <QObject>
#include <QVariantList>

class MapManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList mapNews READ mapNews WRITE setMapNews NOTIFY mapNewsChanged)
    Q_PROPERTY(QStringList nameList READ nameList NOTIFY nameListChanged)

public:
    explicit MapManager(QObject *parent = 0);
    ~MapManager();
    QVariantList mapNews();
    void setMapNews(QVariantList __news);
    QStringList nameList();


signals:
    void mapNewsChanged();
    void nameListChanged();

public slots:
    void updateMaps();
    void addMap(QVariantList __news);

private:
    QMap<QString,QVariantList> m_drawVector;
    QVariantList m_list;
    //per ogni plot conserva la rispettiva analisi
    QMap<QString,Tabble*> m_map;

};

#endif // MAPMANAGER_H
