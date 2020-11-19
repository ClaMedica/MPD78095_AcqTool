#ifndef ANAFLWADV_H
#define ANAFLWADV_H

#include <QAbstractTableModel>
#include <QObject>
#include "manatablemodel.h"
#include "nomogramma.h"

#define DUMMYVALUE -999


class mflowdatasModel: public MAnaTableModel
{
    Q_OBJECT
public:
    mflowdatasModel() {
        //                            role     width     title
        m_modelInfo["default"][0] << "descr" << 70 << tr("DESCRIPTION");
        m_modelInfo["default"][1] << "value" << 30 << tr("VALUE");
        m_roles[0] = "descr";
        m_roles[1] = "value";
    }
};

class mflowdatas : public QObject
{
    Q_OBJECT
public:
    explicit mflowdatas(QObject *parent = 0);
    ~mflowdatas();


    Q_INVOKABLE mflowdatasModel * datasInfo()   {return m_datasInfo;}
    Q_INVOKABLE Nomogramma * getLiverpoolMax()  {return m_liverpoolMax;}
    Q_INVOKABLE Nomogramma * getLiverpoolAve()  {return m_liverpoolAve;}
    Q_INVOKABLE Nomogramma * getSirokyMax()     {return m_sirokyMax;}
    Q_INVOKABLE Nomogramma * getSirokyAve()     {return m_sirokyAve;}
    Q_INVOKABLE Nomogramma * getMiskolcMax()     {return m_miskolcMax;}
    Q_INVOKABLE Nomogramma * getMiskolcAve()     {return m_miskolcAve;}

    float getWaitingTime()              {return m_waitingTime;}
    void setWaitingTime(float __val)    {m_waitingTime = __val;}
    void addWaitingTime(float __val)    {m_waitingTime += __val;}

    float getQMax()                     {return m_qMax;}
    void setQMax(float __val)           {m_qMax = __val;}
    void addQMax(float __val)           {m_qMax += __val;}

    float getQAve()                     {return m_qAve;}
    void setQAve(float __val)           {m_qAve = __val;}
    void addQAve(float __val)           {m_qAve += __val;}

    float getTimeAtV3()                 {return m_timeAtV3;}
    void setTimeAtV3(float __val)       {m_timeAtV3 = __val;}
    void addTimeAtV3(float __val)       {m_timeAtV3 += __val;}

    float getTimeAtV2()                 {return m_timeAtV2;}
    void setTimeAtV2(float __val)       {m_timeAtV2 = __val;}
    void addTimeAtV2(float __val)       {m_timeAtV2 += __val;}

    float getTimeAtQmax()               {return m_timeAtQmax;}
    void setTimeAtQmax(float __val)     {m_timeAtQmax = __val;}
    void addTimeAtQmax(float __val)     {m_timeAtQmax += __val;}

    float getTime90()                   {return m_time90;}
    void setTime90(float __val)         {m_time90 = __val;}
    void addTime90(float __val)         {m_time90 += __val;}

    float getFlowTime()                 {return m_flowTime;}
    void setFlowTime(float __val)       {m_flowTime = __val;}
    void addFlowTime(float __val)       {m_flowTime += __val;}

    float getDescTime()                 {return m_descTime;}
    void setDescTime(float __val)       {m_descTime = __val;}
    void addDescTime(float __val)       {m_descTime += __val;}

    float getVoidingTime()              {return m_voidingTime;}
    void setVoidingTime(float __val)    {m_voidingTime = __val;}
    void addVoidingTime(float __val)    {m_voidingTime += __val;}

    float getVolAtQqmax()               {return m_volAtQqmax;}
    void setVolAtQqmax(float __val)     {m_volAtQqmax = __val;}
    void addVolAtQqmax(float __val)     {m_volAtQqmax += __val;}

    int getVoidedVolume()               {return m_voidedVolume;}
    void setVoidedVolume(int __val)   {m_voidedVolume = __val;}
    void addVoidedVolume(int __val)   {m_voidedVolume += __val;}

    float getAcceleration()             {return m_acceleration;}
    void setAcceleration(float __val)   {m_acceleration = __val;}
    void addAcceleration(float __val)   {m_acceleration += __val;}

    int getResidualVolume()             {return m_residualVolume;}
    void setResidualVolume(int __val) {m_residualVolume = __val;}
    void addResidualVolume(int __val) {m_residualVolume += __val;}

    float getVDetMax()                  {return m_vDetMax;}
    void setVDetMax(float __val)        {m_vDetMax = __val;}
    void addVDetMax(float __val)        {m_vDetMax += __val;}

    float getCQ()                       {return m_cQ;}
    void setCQ(float __val)             {m_cQ = __val;}
    void addCQ(float __val)             {m_cQ += __val;}

    void buildTable();
    void buildNomogrammi(bool __sex, int __age, int __peso, int __altezza);

    void setAutoFlow(bool __auto)       {m_autoflow = __auto;}

    int getLineMiskolcMax() { return m_miskolcLines;}
    int getLineMiskolcAve() { return 2;}

signals:

public slots:


private:
    float m_waitingTime;			// waiting time
    float m_qMax;					// maximum flow
    float m_qAve;					// average flow
    float m_timeAtV3;             // time at Vol/3
    float m_timeAtV2;             // time at Vol/2
    float m_timeAtQmax;			// time at Q max
    float m_time90;				// time between 5% and 95% of the voided volume
    float m_flowTime;				// flow time
    float m_descTime;				// time between Q max and 95% of the voided volume
    float m_voidingTime;			// voiding time
    float m_volAtQqmax;			// volume at Q max
    int m_voidedVolume;          // voided volume
    float m_acceleration;           // Q max / T qmax
    int m_residualVolume;		// residual volume inserted by the user
    float m_vDetMax;              // detrusor contraction maximum speed
    float m_cQ;						// Flow corrective factor

    mflowdatasModel *m_datasInfo;


    //nomogrammi
    Nomogramma *m_liverpoolMax;
    Nomogramma *m_liverpoolAve;
    Nomogramma *m_sirokyMax;
    Nomogramma *m_sirokyAve;
    //pediatrico
    Nomogramma *m_miskolcMax;
    Nomogramma *m_miskolcAve;

    void SetLineaInterpolata(QVector <double> __yVal, int __max, int __step, int __nLinea);
    void ReadLiverpoolParameter(bool __flowMax, bool __sex, int __age);
    void ReadSirokyParameter(bool __flowAve);
    QVector<QVector<double>> MiskolcLinesCostruct(QString __tipoQ, double __bodyS, bool __sex, int __len);
    void MiskolcMaxInit();
    void MiskolcAveInit();

    QVector<double> m_arrLineX1;
    QVector<double> m_arrLineY1;
    QVector<double> m_arrLineX2;
    QVector<double> m_arrLineY2;
    QVector<double> m_arrLineX3;
    QVector<double> m_arrLineY3;
    QVector<double> m_arrLineX4;
    QVector<double> m_arrLineY4;
    QVector<double> m_arrLineX5;
    QVector<double> m_arrLineY5;
    QVector<double> m_arrLineX6;
    QVector<double> m_arrLineY6;

    QVector<double> m_arrAve;
    QVector<double> m_arrMax;

    double m_miskolcLines;

    //struttura necessaria per disegnare il nomogramma di Miskolc
    //BS1 = Body Surface minore 0.92
    //BS2 = Body surface tra 0.92 e 1.42
    //BS3 = Body surface maggiore di 1.42
    typedef struct {
        double BS1_QmA; //Q massima valore A
        double BS1_QaA; //Q media valore A
        double BS1_QmB; //Q massima valore B
        double BS1_QaB; //Q media valore B
        double BS2_QmA;
        double BS2_QaA;
        double BS2_QmB;
        double BS2_QaB;
        double BS3_QmA;
        double BS3_QaA;
        double BS3_QmB;
        double BS3_QaB;
    } MiskolcStruct;

    QVector<MiskolcStruct> m_sMiskolcBoy;
    QVector<MiskolcStruct> m_sMiskolcGirl;

    bool m_autoflow;



protected:
    QHash<int, QByteArray> m_roles;
    QMap<QString, QMap<int, QVariantList> > m_modelInfo;
};

#endif // ANAFLWADV_H


