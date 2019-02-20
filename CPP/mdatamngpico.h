#ifndef MDATAMNGPICO_H
#define MDATAMNGPICO_H

#include "mdatamanager.h"
#include "printermanager.h"
#include <QQuickItemGrabResult>
#include "udpmsgs.h"

class MDataMngPico : public MDataManager
{
    Q_OBJECT
public:
    explicit MDataMngPico(QObject *parent = 0);
    ~MDataMngPico();

    Q_INVOKABLE void getGrabbedImage(QObject *gi, QString __nome);
    Q_INVOKABLE int getSpoolerQueueLen() { return spoolerQueueLen; }
    void send_Command(int __command);   // replicato da macqmanager perche' non si puo' invocare l'originale

signals:
    void udpMdmBtStatus(enum WHO, int);
    void udpMdmPrnStatus(enum WHO, int);

    void sg_exitFromReview();

public slots:
    void sendToPrint();
    void sendPrintTest();
    void udpMdmBtDecode(enum WHO __from, QByteArray __msg);
    void startPrint();
    void exitFromReview();

private:
    void initPrinter();
    void loadFile(QString __fileName);
    printermanager *m_mngPrint;
    int spoolerQueueLen;


};

#endif // MDATAMNGPICO_H
