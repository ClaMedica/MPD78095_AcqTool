#ifndef MDATAMNGDESKTOP_H
#define MDATAMNGDESKTOP_H

#include <QObject>
#include "mdatamanager.h"
#include <QQuickItemGrabResult>

class MDataMngDesktop : public MDataManager
{
    Q_OBJECT
public:
    explicit MDataMngDesktop(QObject *parent = 0);

     Q_INVOKABLE void saveImg(QQuickItem* __item, QString __nome);

signals:
    void sg_exitFromReview();

public slots:
    void openReport();
    void addOpMarker(QVariant __key, QVariant __posX);
    void addDefiner(QVariant __key, QVector<double> __pos);
    QString getNameOfObj(QVariantList __whoAmI);
    void startPrint();
    void exitFromReview();


private:
    //referto
    QTimer* m_reportOpenedTimer;
    QTimer* m_reportTimer;
    bool m_winword;
    QString m_nomeReferto;
    bool checkReportFileOpen();

private slots:
    //per referto
    void slot_checkReportOpened();
    void slot_startReport();

};

#endif // MDATAMNGDESKTOP_H
