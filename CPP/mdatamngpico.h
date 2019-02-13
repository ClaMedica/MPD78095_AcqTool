#ifndef MDATAMNGPICO_H
#define MDATAMNGPICO_H

#include <QObject>
#include "mdatamanager.h"

class MDataMngPico : public MDataManager
{
    Q_OBJECT
public:
    explicit MDataMngPico(QObject *parent = 0);

    Q_INVOKABLE int getSpoolerQueueLen() { return spoolerQueueLen; }

signals:

public slots:

private:
    printermanager *m_mngPrint;
    int spoolerQueueLen;

};

#endif // MDATAMNGPICO_H
