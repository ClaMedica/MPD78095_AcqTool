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
     Q_INVOKABLE void copyImg(QQuickItem* __item);

signals:
    void sg_exitFromReview();

public slots:
    void deleteAnMArkers();
    void openReport(QString __nomeReport);
    void addOpMarker(QVariant __key, QVariant __posX);
    void addDefiner(QVariant __key, QVector<double> __pos);
    void addAnMarker(QVariant __key, QVariant __posX, QVariantList __chName);
    QString getNameOfObj(QVariantList __whoAmI);
    void startPrint();
    void exitFromReview();
    //gestione referti
    QList<QString> getListReports();


private:
    //referto
    QTimer* m_reportOpenedTimer;
    QTimer* m_reportTimer;
    QString m_nomeReferto;
    Ancestry * m_reportEdit;

    void createPdf();

private slots:
    //per referto
    void slot_startReport();

};

#endif // MDATAMNGDESKTOP_H
