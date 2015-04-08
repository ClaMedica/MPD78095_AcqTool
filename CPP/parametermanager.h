#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <QObject>
#include <QVariantList>
#include <../MGlobal/global.h>

class ParameterManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
    Q_PROPERTY(QVariantList lastParMod READ lastParMod WRITE setLastParMod NOTIFY lastParModChanged)
    Q_PROPERTY(QString curAnalysis READ curAnalysis WRITE setCurAnalysis NOTIFY curAnalysisChanged)
    Q_PROPERTY(QVariant curPar READ curPar NOTIFY curParChanged)
    Q_PROPERTY(QStringList roles READ roles WRITE setRoles NOTIFY rolesChanged)
public:
    explicit ParameterManager(QObject *parent = 0);

   QVariantList items();
   QVariant curPar();
   QVariantList lastParMod();
   void setLastParMod(QVariantList __l);

    QString curAnalysis();
    void setCurAnalysis(QString __cur);

    QStringList roles(){return m_roles;}
    void setRoles(QStringList __roles);
signals:
    void curAnalysisChanged();
    void curParChanged();
    void itemsChanged();
    void lastParModChanged();
    void rolesChanged();
public slots:

private:
    void updateItems();
    QVariantList m_items;
    QVariantList m_lastParMod;
    QString m_curAnalysis;
    int m_nItems,m_nFields;
    QStringList m_roles;
    QMap<QString,QMap<QString,VarMap> > m_parameterMap;
    QMap<QString,VarMap> m_current;
};

#endif // PARAMETERMANAGER_H
