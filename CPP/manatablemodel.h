#ifndef MTABLEMODEL_H
#define MTABLEMODEL_H
#include <QAbstractTableModel>
#include <QDebug>
class MAnaTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MAnaTableModel(const QHash<int, QByteArray> &__roles=QHash<int, QByteArray>(),QObject *__pParent = 0);

    //reimplemented functions
    Q_INVOKABLE int rowCount(const QModelIndex &__parent = QModelIndex()) const;
    Q_INVOKABLE int columnCount(const QModelIndex &__parent = QModelIndex()) const;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE void clearModel();
    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;


    Q_INVOKABLE QVariantList modelInfo(const QString &__tabType);
    Q_INVOKABLE bool setData(const int __index,QString __field, const QVariant &__value);
    Qt::ItemFlags flags(const QModelIndex &__index) const;

    Q_INVOKABLE void updateRowData(int __row){
        QModelIndex topLeft;
        QModelIndex bottomRight;
        topLeft = index(__row, 0);
        bottomRight = index(__row, m_roles.count()-1);
        emit dataChanged(topLeft, bottomRight);}
    Q_INVOKABLE void updateData(){
        QModelIndex topLeft = index(0,0);
        QModelIndex bottomRight = index(m_data.count()-1,m_roles.count()-1);
        emit dataChanged(topLeft, bottomRight);}


    Q_INVOKABLE QVariantList rowValues(const int &__row);

protected:
    QMap<QString,QMap<int,QVariantList> > m_modelInfo;   //contiene i nomi dei campi da visualizzare nelle tabelle dove viene usato
    QHash<int, QByteArray> m_roles;//hash delle role
    QMap<int , QMap<QByteArray,QVariant> > m_data;

};



#endif // MTABLEMODEL_H
