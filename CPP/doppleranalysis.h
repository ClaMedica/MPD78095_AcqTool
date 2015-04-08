#ifndef DOPPLERANALYSIS_H
#define DOPPLERANALYSIS_H

#include <QObject>
#include <functionhandler.h>

class DopplerAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit DopplerAnalysis();
    ~DopplerAnalysis();
    Q_PROPERTY(int anaType READ anaType WRITE setAnaType NOTIFY anaTypeChanged)    
    Q_PROPERTY(QVariantList analysisNews READ analysisNews NOTIFY analysisNewsChanged)
    Q_PROPERTY(QStringList availableTracks READ availableTracks NOTIFY availableTracksChanged())
    Q_PROPERTY(QStringList availableData READ availableData NOTIFY availableDataChanged)
    Q_PROPERTY(QVariant currentSignal READ currentSignal WRITE setCurrentSignal NOTIFY currentSignalChanged)

    int anaType();
    void setAnaType(int __a);

    QStringList availableData(){return m_availableData;}
    QStringList availableTracks(){return m_data.keys();}

    QVariantList analysisNews();



    QVariant currentSignal();
    void setCurrentSignal(QVariant __sig);

    //analysis manager

    bool addSignal(MSignal *__pSignal);


    void sendNews(QString __name, QString __tag, qulonglong __element);


signals:
    void parModelChanged();
    void anaTypeChanged();
    void analysisNewsChanged();
    void availableDataChanged();
    void availableTracksChanged();
    void currentSignalChanged();

public slots:
    void analyze(QString __anaType,QVariant __parameters);
    void handleMenu(QString __s);
    bool addSignals(qulonglong __pSignal);
    QVariantList getPlotSettings();
    float getStartTime(){return m_start;}
    float getEndTime(){return m_end;}
    void resetAll(void);

private:
    QMap<QString,float> m_parForAna;
    QVector<MSignal *> m_signalVector;
    QMap<QString,QStringList> m_data;
    QStringList m_availableData;
    QString m_currentSignalName;
    MSignal *m_pCurrentSignal;
    AnalysisType m_anaType;
    float m_start,m_end;

    QVariantList m_morphPointer;
    void updateAvailableData();
    void calcEnergy(QVariant __paramenters);
    void calcTwi(QVariant __paramenters);
};

#endif // DOPPLERANALYSIS_H
