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

    VarMapVec* pickUp(QString __family,QString __name);
    VarMapVec* getAll(void){return m_all;}
    QStringList getFamilies();
    QStringList getNames(QStringList __families,QStringList __filterType=QStringList());

    bool archive(QString __family, QString __name, VarMapVec *__elements, bool __whatIfAlreadyPresent=OVERWRITE);
    bool contains(QString __family,QString __name);
    bool modifyElement(QString __family, QString __name,qulonglong __code, QVariantList __news);
signals:

public slots:


private:
    VarMapVec *m_all;
    QMap<QString,QMap<QString,VarMapVec *> > m_storage;
};

#endif // MSTORAGE_H
