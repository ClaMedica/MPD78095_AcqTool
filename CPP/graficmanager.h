#ifndef GRAFICMANAGER_H
#define GRAFICMANAGER_H

#include <QObject>
#include "global.h"
#include "ancestry.h"
#include "p7settingsmanager.h"

class GraficManager : public QObject
{
    Q_OBJECT
public:
    explicit GraficManager(QObject *parent = 0);
    Q_INVOKABLE QVariant valueOf(const QString __name, const QString __type);
signals:

public slots:
private:
    Ancestry m_graphics;//contiene il file xml caricato

};

#endif // GRAFICMANAGER_H
