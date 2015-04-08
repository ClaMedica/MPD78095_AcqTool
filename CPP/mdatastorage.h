#ifndef MDATASTORAGE_H
#define MDATASTORAGE_H

#include <QObject>
#include <global.h>
#include <QMap>

class MDataStorage : public QObject
{
    Q_OBJECT
public:
    explicit MDataStorage(QObject *parent = 0);

    qulonglong pickUp(QString __family,QString __name);
    qulonglong getAll(void){return (qulonglong)m_all;}
    QStringList getFamilies();
    QStringList getNames(QStringList __families,QStringList __filterType=QStringList());

    bool archive(QString __family, QString __name, qulonglong __pointer,bool __whatIfAlreadyPresent=OVERWRITE);
    bool contains(QString __family,QString __name);
    bool modifyElement(QString __family, QString __name, qulonglong __code, QVariantList __news);
signals:

public slots:


private:
    VarMapVec *m_all;
    QMap<QString,QMap<QString,qulonglong> > m_storage;
};

#endif // MDATASTORAGE_H
