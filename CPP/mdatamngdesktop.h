#ifndef MDATAMNGDESKTOP_H
#define MDATAMNGDESKTOP_H

#include <QObject>
#include <QQuickItemGrabResult>
#include "mdatamanager.h"

#define REFHTM  0
#define REFRTF  1

class MDataMngDesktop : public MDataManager
{
    Q_OBJECT
public:
    explicit MDataMngDesktop(QObject *parent = 0);

     Q_INVOKABLE void saveImg(QQuickItem* __item, QString __nome);
     Q_INVOKABLE void copyImg(QQuickItem* __item);

signals:
    void sg_exitFromReview();
    void sg_warnReport(QString __msg);

public slots:
    void deleteAnMArkers();
    void openReport(QString __nomeReport, QString __codSoft);
    void addOpMarker(QVariant __key, QVariant __posX);
    void addDefiner(QVariant __key, QVector<double> __pos);
    void addAnMarker(QVariant __key, QVariant __posX, QVariantList __chName);
    QString getNameOfObj(QVariantList __whoAmI);
    void startPrint(QString __codSoft);
    void exitFromReview();
    void openExportTool(QString __codSoft);
    //gestione referti
    QList<QString> getListReports();
    void slot_checkReportOpened();
    void slot_startReport();

private:
    //referto
    QString m_nomeReferto;
    QString m_nomeRefertoPdf;
    Ancestry * m_reportEdit;
    int m_tipoReferto;
    QTimer *m_reportOpenedTimer;
    QTimer *m_reportTimer;
    bool m_winword;

    void loadFile(QString __fileName);
    void createPdf();
    QString createNamePDf();
    bool checkReportFileOpen();

};

#endif // MDATAMNGDESKTOP_H
