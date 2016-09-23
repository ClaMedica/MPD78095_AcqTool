#include "manatablemodel.h"

MAnaTableModel::MAnaTableModel(const QHash<int, QByteArray> &__roles, QObject *__pParent):
    QAbstractTableModel(__pParent)
{
    m_roles=__roles;
}

int MAnaTableModel::rowCount(const QModelIndex &__parent) const
{
    (void) __parent;

    return m_data.count();
}

int MAnaTableModel::columnCount(const QModelIndex &__parent) const
{
    (void) __parent;

    return m_roles.count();
}

QVariant MAnaTableModel::data(const QModelIndex &__index, int __role) const
{
    if (!__index.isValid() || (__role < 0) || (__role > (m_roles.count() - 1)))
        return QVariant();

    return m_data[__index.row()][m_roles[__role]];
}

void MAnaTableModel::clearModel()
{
    removeRows(0, m_data.count());
    m_data.clear();
    updateData();
}

Qt::ItemFlags MAnaTableModel::flags(const QModelIndex &__index) const
{
    (void) __index;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> MAnaTableModel::roleNames() const
{
    return m_roles;
}

QVariantList MAnaTableModel::modelInfo(const QString &__tabType)
{
    if(!m_modelInfo.keys().contains(__tabType)) {
        qCritical() << "Nome tabella non presente" << __tabType;
    }

    QVariantList list;
    QVariant v;

    for(int i = 0; i < m_modelInfo[__tabType].size(); i++) {
        v = m_modelInfo[__tabType][i];
        list << v;
    }

    return list;
}

bool MAnaTableModel::setData(const int __index, QString __field, const QVariant &__value)
{
    if(!m_roles.values().contains(__field.toLatin1()))
        qCritical() << "Campo non riconosciuto" << __field << m_roles;

    m_data[__index][__field.toLatin1()] = __value;
    updateData();

    return true;
}

QVariantList MAnaTableModel::rowValues(const int &__row)
{
    QVariantList values;

    for(int i = 0; i < m_roles.count(); i++)
        values << m_data[__row][m_roles[i]];

    return values;
}


