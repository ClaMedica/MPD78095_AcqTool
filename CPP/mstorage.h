#ifndef MSTORAGE_H
#define MSTORAGE_H

#include <QObject>
#include <global.h>
#include <QMap>

class MStorage : public QObject
{
    Q_OBJECT
public:
    explicit MStorage(QObject *parent = 0);
    void clearAll();

    VarMapVec* pickUp(QString __family,QString __name);
    VarMapVec* getAll(QString __category);
    QStringList getFamilies();
    QStringList getNames(QStringList __families,QStringList __filterCategory=QStringList());

    bool archive(QString __family, QString __name, VarMapVec *__elements, bool __whatIfAlreadyPresent=OVERWRITE);
    bool contains(QString __family,QString __name);
    bool modifyElement(qulonglong __whoAmI, QVariantList __news);
    void addCategory(QString __category);
signals:

public slots:


private:
    QMap<QString,VarMapVec *> m_allMap;
    QMap<QString,QMap<QString,VarMapVec *> > m_storage;
};

#endif // MSTORAGE_H
