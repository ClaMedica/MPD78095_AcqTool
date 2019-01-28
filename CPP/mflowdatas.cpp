#include "mflowdatas.h"
#include <QtMath>

mflowdatas::mflowdatas(QObject *parent) : QObject(parent)
{
    m_waitingTime = DUMMYVALUE;
    m_qMax = DUMMYVALUE;
    m_qAve = DUMMYVALUE;
    m_timeAtV3 = DUMMYVALUE;
    m_timeAtV2 = DUMMYVALUE;
    m_timeAtQmax = DUMMYVALUE;
    m_time90 = DUMMYVALUE;
    m_flowTime = DUMMYVALUE;
    m_descTime = DUMMYVALUE;
    m_voidingTime = DUMMYVALUE;
    m_volAtQqmax = DUMMYVALUE;
    m_voidedVolume = DUMMYVALUE;
    m_acceleration = DUMMYVALUE;
    m_residualVolume = DUMMYVALUE;
    m_vDetMax = DUMMYVALUE;
    m_cQ = DUMMYVALUE;

    m_datasInfo = new mflowdatasModel();

    m_liverpoolMax = new Nomogramma("flowmetry",G_LIVERPOOL_MAX);
    m_liverpoolMax->setParent(this);
    m_liverpoolAve = new Nomogramma("flowmetry",G_LIVERPOOL_AVE);
    m_liverpoolAve->setParent(this);

    m_sirokyMax = new Nomogramma("flowmetry",G_SIROKY_MAX);
    m_sirokyMax->setParent(this);
    m_sirokyAve = new Nomogramma("flowmetry",G_SIROKY_AVE);
    m_sirokyAve->setParent(this);

    m_autoflow = false;
}

mflowdatas::~mflowdatas()
{
    //delete m_datasInfo;
    //delete m_liverpoolMax;
    //delete m_liverpoolAve;
}

void mflowdatas::buildTable()
{
    int count = -1;
    if (!m_autoflow) {
        count++;
        m_datasInfo->setData(count,"descr",tr(" Waiting time (sec)"));
        m_datasInfo->setData(count,"value",QString::number((float)(qRound(getWaitingTime()*10))/10, 'f', 1));
    }

    m_datasInfo->setData(count+1,"descr",tr(" Maximum flow rate (ml/sec)"));
    m_datasInfo->setData(count+1,"value",QString::number((float)(qRound(getQMax()*10))/10, 'f', 1));

    m_datasInfo->setData(count+2,"descr",tr(" Average flow rate (ml/sec)"));
    m_datasInfo->setData(count+2,"value",QString::number((float)(qRound(getQAve()*10))/10, 'f', 1));

    m_datasInfo->setData(count+3,"descr",tr(" Time to maximum flow (sec)"));
    m_datasInfo->setData(count+3,"value",QString::number((float)(qRound(getTimeAtQmax()*10))/10, 'f', 1));

    m_datasInfo->setData(count+4,"descr",tr(" Time between 5% and 95% (sec)"));
    m_datasInfo->setData(count+4,"value",QString::number((float)(qRound(getTime90()*10))/10, 'f', 1));

    m_datasInfo->setData(count+5,"descr",tr(" Flow time (sec)"));
    m_datasInfo->setData(count+5,"value",QString::number((float)(qRound(getFlowTime()*10))/10, 'f', 1));

    m_datasInfo->setData(count+6,"descr",tr(" Descent time (sec)"));
    m_datasInfo->setData(count+6,"value",QString::number((float)(qRound(getDescTime()*10))/10, 'f', 1));

    m_datasInfo->setData(count+7,"descr",tr(" Voiding time (sec)"));
    m_datasInfo->setData(count+7,"value",QString::number((float)(qRound(getVoidingTime()*10))/10, 'f', 1));

    m_datasInfo->setData(count+8,"descr",tr(" Volume to maximum flow (ml)"));
    m_datasInfo->setData(count+8,"value",QString::number((float)(qRound(getVolAtQqmax()*10))/10, 'f', 1));

    m_datasInfo->setData(count+9,"descr",tr(" Voided volume (ml)"));
    m_datasInfo->setData(count+9,"value",QString::number(getVoidedVolume(), 'f', 1));

    m_datasInfo->setData(count+10,"descr",tr(" Corrected maximum flow (ml^(1/2)/s)"));
    m_datasInfo->setData(count+10,"value",QString::number((float)(qRound(getCQ()*10))/10, 'f', 1));

    m_datasInfo->setData(count+11,"descr",tr(" Flow acceleration (ml/sec^2))"));
    m_datasInfo->setData(count+11,"value",QString::number((float)(qRound(getAcceleration()*10))/10, 'f', 1));

    m_datasInfo->setData(count+12,"descr",tr(" Maximum contraction speed (mm/sec)"));
    m_datasInfo->setData(count+12,"value",QString::number((float)(qRound(getVDetMax()*10))/10, 'f', 1));

    m_datasInfo->setData(count+13,"descr",tr(" Residual volume (ml)"));
    m_datasInfo->setData(count+13,"value",QString::number(getResidualVolume(), 'f', 1));

}

void mflowdatas::buildNomogrammi(bool __sex, int __age)
{
    if (__age == -1){
        qDebug()<<"Error: età paziente non valida";
        __age = 40;
    }
    //Liverpool MAX
    m_liverpoolMax->setTitle(tr("Liverpool (Q Max)"));
    m_liverpoolMax->setUnitx(tr("Vol. (ml)"));
    m_liverpoolMax->setUnity(tr("Q (ml/sec)"));
    m_liverpoolMax->setXmin(0);
    m_liverpoolMax->setYmin(0);
    m_liverpoolMax->setXmax(600);
    if (__sex)
        m_liverpoolMax->setYmax(60);
    else
        m_liverpoolMax->setYmax(50);

    m_liverpoolMax->setDatoX(getVoidedVolume());
    m_liverpoolMax->setDatoY(getQMax());

    //da sistemare
    int N_LIVERPOOL = 24;
    int N_LIVERPOOL_CENT = 5;
    double Step_Liverpool = m_liverpoolMax->getXmax() / N_LIVERPOOL;//25

    QVector<double> yValue;
    yValue.append(0);
    for (int i = 0; i < N_LIVERPOOL; i++) {
        if (!__sex) {
            if (__age < 50)
                //Liverpool uomini etA  < 50
                yValue.append(qPow((2.37 + 0.18 * qSqrt((i+1) * Step_Liverpool) - 0.014 * 35), 2));
            else
                //Liverpool uomini >= 50
                yValue.append(qPow((2.37 + 0.18 * qSqrt((i+1) * Step_Liverpool) - 0.014 * 60), 2));
        }
        else
            //Liverpool donne
            yValue.append(qPow(2.718282,(0.511 + 0.505 * qLn((i+1) * Step_Liverpool))));
    }

    int pos = 0;
    double midValue = 0, valueToAdd = 0;
    for (int i = 0; i < m_liverpoolMax->getXmax(); i++) {
        if (i == (Step_Liverpool * pos)) {
            m_liverpoolMax->addToLiney(0, yValue.at(pos));
            midValue = (yValue.at(pos + 1) - yValue.at(pos)) / Step_Liverpool;
            valueToAdd = midValue;
            pos++;
        }
        else {
            m_liverpoolMax->addToLiney(0, yValue.at(pos-1) + valueToAdd);
            valueToAdd=valueToAdd + midValue;
        }

    }

    QMap<int,QVector<double> > yVal,xVal;
    xVal[0].append(0);
    yVal[0].append(0);
    xVal[1].append(0);
    yVal[1].append(0);
    xVal[2].append(0);
    yVal[2].append(0);
    xVal[3].append(0);
    yVal[3].append(0);
    xVal[4].append(0);
    yVal[4].append(0);
    xVal[5].append(0);
    yVal[5].append(0);
    ReadLiverpoolParameter(true, __sex, __age);
    for (int i = 0; i <= N_LIVERPOOL_CENT; i++) {
        xVal[0].append(m_arrLineX1.at(i));
        yVal[0].append(m_arrLineY1.at(i));

        xVal[1].append(m_arrLineX2.at(i));
        yVal[1].append(m_arrLineY2.at(i));

        xVal[2].append(m_arrLineX3.at(i));
        yVal[2].append(m_arrLineY3.at(i));

        xVal[3].append(m_arrLineX4.at(i));
        yVal[3].append(m_arrLineY4.at(i));

        xVal[4].append(m_arrLineX5.at(i));
        yVal[4].append(m_arrLineY5.at(i));

        xVal[5].append(m_arrLineX6.at(i));
        yVal[5].append(m_arrLineY6.at(i));
    }

    for (int j = 0; j < xVal.size(); j++) {
        pos = 0;
        midValue = 0;
        valueToAdd = 0;
        for (int i = 0; i < m_liverpoolMax->getXmax(); i++) {
            if (i == xVal[j].at(pos)) {
                m_liverpoolMax->addToLiney(j+1,yVal[j].at(pos));
                midValue = (yVal[j].at(pos+1) - yVal[j].at(pos)) / (xVal[j].at(pos+1) - xVal[j].at(pos));
                valueToAdd = midValue;
                pos++;
            }
            else {
                m_liverpoolMax->addToLiney(j+1,yVal[j].at(pos-1)+valueToAdd);
                valueToAdd=valueToAdd+midValue;
            }
        }
    }

    //Liverpool AVE
    m_liverpoolAve->setTitle(tr("Liverpool (Q Ave)"));
    m_liverpoolAve->setUnitx(tr("Vol. (ml)"));
    m_liverpoolAve->setUnity(tr("Q (ml/sec)"));
    m_liverpoolAve->setXmin(0);
    m_liverpoolAve->setYmin(0);
    m_liverpoolAve->setXmax(600);
    m_liverpoolAve->setYmax(30);
    m_liverpoolAve->setDatoX(getVoidedVolume());
    m_liverpoolAve->setDatoY(getQAve());

    yValue.clear();
    yValue.append(0);
    for (int i = 0; i < N_LIVERPOOL; i++) {
        if (!__sex) {
            if (__age < 50)
                //Liverpool uomini etA  < 50
                yValue.append(qPow((1.8 + 0.14 * qSqrt((i+1) * Step_Liverpool) - 0.011 * 35), 2));
            else
                //Liverpool uomini >= 50
                yValue.append(qPow((1.8 + 0.14 * qSqrt((i+1) * Step_Liverpool) - 0.011 * 60), 2));
        }
        else {
            //Liverpool donne
            yValue.append( qPow((-0.921 + 0.869 * qLn((i+1) * Step_Liverpool)),2));
        }
    }

    pos = 0;
    midValue = 0;
    valueToAdd = 0;
    for (int i = 0; i < m_liverpoolAve->getXmax(); i++) {
        if (i == (Step_Liverpool * pos)) {
            m_liverpoolAve->addToLiney(0, yValue.at(pos));
            midValue = (yValue.at(pos + 1) - yValue.at(pos)) / Step_Liverpool;
            valueToAdd = midValue;
            pos++;
        }
        else {
            m_liverpoolAve->addToLiney(0, yValue.at(pos-1) + valueToAdd);
            valueToAdd = valueToAdd + midValue;
        }
    }

    xVal.clear();
    yVal.clear();
    xVal[0].append(0);
    yVal[0].append(0);
    xVal[1].append(0);
    yVal[1].append(0);
    xVal[2].append(0);
    yVal[2].append(0);
    xVal[3].append(0);
    yVal[3].append(0);
    xVal[4].append(0);
    yVal[4].append(0);
    xVal[5].append(0);
    yVal[5].append(0);

    ReadLiverpoolParameter(false, __sex, __age);
    for (int i = 0; i <= N_LIVERPOOL_CENT; i++) {
        xVal[0].append(m_arrLineX1.at(i));
        yVal[0].append(m_arrLineY1.at(i));

        xVal[1].append(m_arrLineX2.at(i));
        yVal[1].append(m_arrLineY2.at(i));

        xVal[2].append(m_arrLineX3.at(i));
        yVal[2].append(m_arrLineY3.at(i));

        xVal[3].append(m_arrLineX4.at(i));
        yVal[3].append(m_arrLineY4.at(i));

        xVal[4].append(m_arrLineX5.at(i));
        yVal[4].append(m_arrLineY5.at(i));

        xVal[5].append(m_arrLineX6.at(i));
        yVal[5].append(m_arrLineY6.at(i));
    }

    for (int j = 0; j < xVal.size(); j++) {
        pos = 0;
        midValue = 0;
        valueToAdd = 0;
        for (int i = 0; i < m_liverpoolAve->getXmax(); i++) {
            if (i == xVal[j].at(pos)) {
                m_liverpoolAve->addToLiney(j + 1, yVal[j].at(pos));
                midValue = (yVal[j].at(pos + 1) - yVal[j].at(pos)) / (xVal[j].at(pos+1) - xVal[j].at(pos));
                valueToAdd = midValue;
                pos++;
            }
            else {
                m_liverpoolAve->addToLiney(j + 1, yVal[j].at(pos-1) + valueToAdd);
                valueToAdd = valueToAdd+midValue;
            }
        }
    }

    //siroky Max - Ave
    if (!__sex) {
        //max
        m_sirokyMax->setTitle(tr("Siroky (Q Max)"));
        m_sirokyMax->setUnitx(tr("Vol. (ml)"));
        m_sirokyMax->setUnity(tr("Q (ml/sec)"));
        m_sirokyMax->setXmin(0);
        m_sirokyMax->setYmin(0);
        m_sirokyMax->setXmax(500);
        m_sirokyMax->setYmax(30);

        m_sirokyMax->setDatoX(getVoidedVolume());
        m_sirokyMax->setDatoY(getQMax());

        xVal.clear();
        yVal.clear();
        xVal[0].append(0);
        yVal[0].append(0);
        xVal[1].append(0);
        yVal[1].append(0);
        xVal[2].append(0);
        yVal[2].append(0);
        xVal[3].append(0);
        yVal[3].append(0);

        ReadSirokyParameter(false);
        for (int i = 0; i <m_arrLineX1.length(); i++) {
            xVal[0].append(m_arrLineX1.at(i));
            yVal[0].append(m_arrLineY1.at(i));
        }
        for (int i = 0; i < m_arrLineX2.length(); i++) {
            xVal[1].append(m_arrLineX2.at(i));
            yVal[1].append(m_arrLineY2.at(i));
        }
        for (int i = 0; i < m_arrLineX3.length(); i++) {
            xVal[2].append(m_arrLineX3.at(i));
            yVal[2].append(m_arrLineY3.at(i));
        }
        for (int i = 0; i < m_arrLineX4.length(); i++) {
            xVal[3].append(m_arrLineX4.at(i));
            yVal[3].append(m_arrLineY4.at(i));
        }

        for (int j = 0; j < xVal.size(); j++) {
            pos = 0;
            midValue = 0;
            valueToAdd = 0;
            for (int i = 0; i < m_sirokyMax->getXmax(); i++) {
                if ((i == xVal[j].at(pos)) && (pos != (xVal[j].length() - 1))) {
                    m_sirokyMax->addToLiney(j, yVal[j].at(pos));
                    midValue = (yVal[j].at(pos+1) - yVal[j].at(pos)) / (xVal[j].at(pos+1) - xVal[j].at(pos));
                    valueToAdd = midValue;
                    pos++;
                }
                else {
                    m_sirokyMax->addToLiney(j, yVal[j].at(pos-1)+valueToAdd);
                    valueToAdd=valueToAdd + midValue;
                }
            }
        }

        //ave
        m_sirokyAve->setTitle(tr("Siroky (Q Ave)"));
        m_sirokyAve->setUnitx(tr("Vol. (ml)"));
        m_sirokyAve->setUnity(tr("Q (ml/sec)"));
        m_sirokyAve->setXmin(0);
        m_sirokyAve->setYmin(0);
        m_sirokyAve->setXmax(500);
        m_sirokyAve->setYmax(30);

        m_sirokyAve->setDatoX(getVoidedVolume());
        m_sirokyAve->setDatoY(getQAve());

        xVal.clear();
        yVal.clear();
        xVal[0].append(0);
        yVal[0].append(0);
        xVal[1].append(0);
        yVal[1].append(0);
        xVal[2].append(0);
        yVal[2].append(0);
        xVal[3].append(0);
        yVal[3].append(0);
        xVal[4].append(0);
        yVal[4].append(0);

        ReadSirokyParameter(true);
        for (int i = 0; i < m_arrLineX1.length(); i++) {
            xVal[0].append(m_arrLineX1.at(i));
            yVal[0].append(m_arrLineY1.at(i));
        }
        for (int i = 0; i < m_arrLineX2.length(); i++) {
            xVal[1].append(m_arrLineX2.at(i));
            yVal[1].append(m_arrLineY2.at(i));
        }
        for (int i = 0; i < m_arrLineX3.length(); i++) {
            xVal[2].append(m_arrLineX3.at(i));
            yVal[2].append(m_arrLineY3.at(i));
        }
        for (int i = 0; i < m_arrLineX4.length(); i++) {
            xVal[3].append(m_arrLineX4.at(i));
            yVal[3].append(m_arrLineY4.at(i));
        }
        for (int i = 0; i < m_arrLineX5.length(); i++) {
            xVal[4].append(m_arrLineX5.at(i));
            yVal[4].append(m_arrLineY5.at(i));
        }
        for (int j = 0; j < xVal.size(); j++) {
            pos = 0;
            midValue = 0;
            valueToAdd = 0;
            for (int i = 0; i < m_sirokyAve->getXmax(); i++) {
                if ((i == xVal[j].at(pos)) && (pos != (xVal[j].length() - 1))) {
                    m_sirokyAve->addToLiney(j, yVal[j].at(pos));
                    midValue = (yVal[j].at(pos + 1) - yVal[j].at(pos)) / (xVal[j].at(pos + 1) - xVal[j].at(pos));
                    valueToAdd = midValue;
                    pos++;
                }
                else {
                    m_sirokyAve->addToLiney(j, yVal[j].at(pos-1) + valueToAdd);
                    valueToAdd = valueToAdd + midValue;
                }
            }
        }
    }
}

void mflowdatas::ReadLiverpoolParameter(bool __flowMax, bool __sex, int __age)
{
    m_arrLineX1.clear();
    m_arrLineX2.clear();
    m_arrLineX3.clear();
    m_arrLineX4.clear();
    m_arrLineX5.clear();
    m_arrLineX6.clear();

    m_arrLineY1.clear();
    m_arrLineY2.clear();
    m_arrLineY3.clear();
    m_arrLineY4.clear();
    m_arrLineY5.clear();
    m_arrLineY6.clear();

    if (__flowMax && !__sex && (__age < 50)) {
        //Qmax, pazienti maschi etA  < 50
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(60);
        m_arrLineX1.append(127);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(2);
        m_arrLineY1.append(4.2);
        m_arrLineY1.append(7.5);
        m_arrLineY1.append(11);
        m_arrLineY1.append(15.5);
        m_arrLineY1.append(27.5);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(60);
        m_arrLineX2.append(127);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(3);
        m_arrLineY2.append(5.5);
        m_arrLineY2.append(9);
        m_arrLineY2.append(13);
        m_arrLineY2.append(17.5);
        m_arrLineY2.append(29.5);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(60);
        m_arrLineX3.append(127);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(4.5);
        m_arrLineY3.append(8);
        m_arrLineY3.append(12);
        m_arrLineY3.append(16);
        m_arrLineY3.append(21);
        m_arrLineY3.append(35);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(60);
        m_arrLineX4.append(127);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(10);
        m_arrLineY4.append(14.5);
        m_arrLineY4.append(20);
        m_arrLineY4.append(25);
        m_arrLineY4.append(31);
        m_arrLineY4.append(48);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(60);
        m_arrLineX5.append(127);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(13.5);
        m_arrLineY5.append(18);
        m_arrLineY5.append(24);
        m_arrLineY5.append(29.5);
        m_arrLineY5.append(36.5);
        m_arrLineY5.append(54);

        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(60);
        m_arrLineX6.append(127);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(15.5);
        m_arrLineY6.append(20.5);
        m_arrLineY6.append(26.5);
        m_arrLineY6.append(32.5);
        m_arrLineY6.append(39);
        m_arrLineY6.append(58);

    }
    if (!__flowMax && !__sex && (__age < 50)) {
        //Qave, pazienti maschi etA  < 50
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(50);
        m_arrLineX1.append(100);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(1.5);
        m_arrLineY1.append(2.3);
        m_arrLineY1.append(3.9);
        m_arrLineY1.append(6.5);
        m_arrLineY1.append(9);
        m_arrLineY1.append(16.1);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(50);
        m_arrLineX2.append(100);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(1.9);
        m_arrLineY2.append(3);
        m_arrLineY2.append(4.7);
        m_arrLineY2.append(7.5);
        m_arrLineY2.append(10);
        m_arrLineY2.append(17.8);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(50);
        m_arrLineX3.append(100);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(2.9);
        m_arrLineY3.append(4.2);
        m_arrLineY3.append(6.2);
        m_arrLineY3.append(9.5);
        m_arrLineY3.append(12.3);
        m_arrLineY3.append(20.5);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(50);
        m_arrLineX4.append(100);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(5.8);
        m_arrLineY4.append(7.9);
        m_arrLineY4.append(10.1);
        m_arrLineY4.append(14.4);
        m_arrLineY4.append(18);
        m_arrLineY4.append(27.7);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(50);
        m_arrLineX5.append(100);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(7.7);
        m_arrLineY5.append(9.9);
        m_arrLineY5.append(12.5);
        m_arrLineY5.append(17);
        m_arrLineY5.append(21);
        m_arrLineY5.append(31.2);

        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(50);
        m_arrLineX6.append(100);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(8.8);
        m_arrLineY6.append(11);
        m_arrLineY6.append(13.9);
        m_arrLineY6.append(18.5);
        m_arrLineY6.append(22.6);
        m_arrLineY6.append(33.2);
    }

    if (__flowMax && !__sex && (__age >= 50)) {
        //Qmax, pazienti maschi etA  >= 50
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(50);
        m_arrLineX1.append(100);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(1.5);
        m_arrLineY1.append(2.5);
        m_arrLineY1.append(5);
        m_arrLineY1.append(9);
        m_arrLineY1.append(12.5);
        m_arrLineY1.append(23.5);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(50);
        m_arrLineX2.append(100);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(2);
        m_arrLineY2.append(3.8);
        m_arrLineY2.append(6);
        m_arrLineY2.append(10.5);
        m_arrLineY2.append(14.8);
        m_arrLineY2.append(26);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(50);
        m_arrLineX3.append(100);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(3.7);
        m_arrLineY3.append(5.5);
        m_arrLineY3.append(8.5);
        m_arrLineY3.append(13.5);
        m_arrLineY3.append(18);
        m_arrLineY3.append(30.5);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(50);
        m_arrLineX4.append(100);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(8);
        m_arrLineY4.append(11.5);
        m_arrLineY4.append(15);
        m_arrLineY4.append(21.8);
        m_arrLineY4.append(27.5);
        m_arrLineY4.append(42.8);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(50);
        m_arrLineX5.append(100);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(11);
        m_arrLineY5.append(14.2);
        m_arrLineY5.append(18.8);
        m_arrLineY5.append(26);
        m_arrLineY5.append(32.2);
        m_arrLineY5.append(49);


        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(50);
        m_arrLineX6.append(100);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(13);
        m_arrLineY6.append(16.5);
        m_arrLineY6.append(21.2);
        m_arrLineY6.append(28.8);
        m_arrLineY6.append(35.5);
        m_arrLineY6.append(52.5);
    }

    if ( !__flowMax && !__sex && (__age >= 50)) {
        //Qave, pazienti maschi etA  >= 50
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(50);
        m_arrLineX1.append(100);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(0.7);
        m_arrLineY1.append(1.5);
        m_arrLineY1.append(2.8);
        m_arrLineY1.append(5.2);
        m_arrLineY1.append(7.4);
        m_arrLineY1.append(13.9);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(50);
        m_arrLineX2.append(100);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(1.1);
        m_arrLineY2.append(2.1);
        m_arrLineY2.append(3.5);
        m_arrLineY2.append(6);
        m_arrLineY2.append(8.4);
        m_arrLineY2.append(15);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(50);
        m_arrLineX3.append(100);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(2);
        m_arrLineY3.append(3);
        m_arrLineY3.append(5);
        m_arrLineY3.append(7.9);
        m_arrLineY3.append(10.5);
        m_arrLineY3.append(18);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(50);
        m_arrLineX4.append(100);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(4.6);
        m_arrLineY4.append(6.2);
        m_arrLineY4.append(8.7);
        m_arrLineY4.append(12.2);
        m_arrLineY4.append(15.8);
        m_arrLineY4.append(25);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(50);
        m_arrLineX5.append(100);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(6);
        m_arrLineY5.append(8);
        m_arrLineY5.append(10.5);
        m_arrLineY5.append(14.7);
        m_arrLineY5.append(18.5);
        m_arrLineY5.append(28);

        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(50);
        m_arrLineX6.append(100);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(6.9);
        m_arrLineY6.append(9.1);
        m_arrLineY6.append(11.9);
        m_arrLineY6.append(16.2);
        m_arrLineY6.append(20);
        m_arrLineY6.append(30);
    }

    if ( __flowMax && __sex) {
        //Qmax, pazienti femmine
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(50);
        m_arrLineX1.append(100);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(4);
        m_arrLineY1.append(7);
        m_arrLineY1.append(10);
        m_arrLineY1.append(14);
        m_arrLineY1.append(16.8);
        m_arrLineY1.append(24);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(50);
        m_arrLineX2.append(100);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(4.8);
        m_arrLineY2.append(8);
        m_arrLineY2.append(11);
        m_arrLineY2.append(15.5);
        m_arrLineY2.append(19.2);
        m_arrLineY2.append(27.2);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(50);
        m_arrLineX3.append(100);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(6);
        m_arrLineY3.append(9.5);
        m_arrLineY3.append(13.8);
        m_arrLineY3.append(19.2);
        m_arrLineY3.append(23.5);
        m_arrLineY3.append(33.5);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(50);
        m_arrLineX4.append(100);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(9.8);
        m_arrLineY4.append(15);
        m_arrLineY4.append(21.5);
        m_arrLineY4.append(30.5);
        m_arrLineY4.append(37.2);
        m_arrLineY4.append(52.5);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(50);
        m_arrLineX5.append(100);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(11.8);
        m_arrLineY5.append(18.5);
        m_arrLineY5.append(26.5);
        m_arrLineY5.append(37.5);
        m_arrLineY5.append(46);
        m_arrLineY5.append(64.8);

        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(50);
        m_arrLineX6.append(100);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(13.8);
        m_arrLineY6.append(21);
        m_arrLineY6.append(30);
        m_arrLineY6.append(42.5);
        m_arrLineY6.append(52);
        m_arrLineY6.append(75);
    }

    if ( !__flowMax && __sex) {
        //Qave, pazienti femmine
        //5th Centile
        m_arrLineX1.append(20);
        m_arrLineX1.append(50);
        m_arrLineX1.append(100);
        m_arrLineX1.append(200);
        m_arrLineX1.append(300);
        m_arrLineX1.append(600);

        m_arrLineY1.append(0.4);
        m_arrLineY1.append(2);
        m_arrLineY1.append(4.1);
        m_arrLineY1.append(7);
        m_arrLineY1.append(9);
        m_arrLineY1.append(12.7);

        //10th Centile
        m_arrLineX2.append(20);
        m_arrLineX2.append(50);
        m_arrLineX2.append(100);
        m_arrLineX2.append(200);
        m_arrLineX2.append(300);
        m_arrLineX2.append(600);

        m_arrLineY2.append(0.8);
        m_arrLineY2.append(2.9);
        m_arrLineY2.append(5.1);
        m_arrLineY2.append(8.3);
        m_arrLineY2.append(10.4);
        m_arrLineY2.append(14.5);

        //25th centile
        m_arrLineX3.append(20);
        m_arrLineX3.append(50);
        m_arrLineX3.append(100);
        m_arrLineX3.append(200);
        m_arrLineX3.append(300);
        m_arrLineX3.append(600);

        m_arrLineY3.append(1.7);
        m_arrLineY3.append(4.3);
        m_arrLineY3.append(7);
        m_arrLineY3.append(10.5);
        m_arrLineY3.append(13);
        m_arrLineY3.append(17.5);

        //75th centile
        m_arrLineX4.append(20);
        m_arrLineX4.append(50);
        m_arrLineX4.append(100);
        m_arrLineX4.append(200);
        m_arrLineX4.append(300);
        m_arrLineX4.append(600);

        m_arrLineY4.append(5);
        m_arrLineY4.append(8.7);
        m_arrLineY4.append(12.2);
        m_arrLineY4.append(16.9);
        m_arrLineY4.append(20);
        m_arrLineY4.append(25.7);

        //90th centile
        m_arrLineX5.append(20);
        m_arrLineX5.append(50);
        m_arrLineX5.append(100);
        m_arrLineX5.append(200);
        m_arrLineX5.append(300);
        m_arrLineX5.append(600);

        m_arrLineY5.append(6.5);
        m_arrLineY5.append(11.1);
        m_arrLineY5.append(15.1);
        m_arrLineY5.append(20.3);
        m_arrLineY5.append(23.5);
        m_arrLineY5.append(30);

        //95th centile
        m_arrLineX6.append(20);
        m_arrLineX6.append(50);
        m_arrLineX6.append(100);
        m_arrLineX6.append(200);
        m_arrLineX6.append(300);
        m_arrLineX6.append(600);

        m_arrLineY6.append(7.5);
        m_arrLineY6.append(12.5);
        m_arrLineY6.append(17);
        m_arrLineY6.append(22.3);
        m_arrLineY6.append(25.9);
        m_arrLineY6.append(32.3);
    }
}

void mflowdatas::ReadSirokyParameter(bool __flowAve)
{
    m_arrLineX1.clear();
    m_arrLineX2.clear();
    m_arrLineX3.clear();
    m_arrLineX4.clear();
    m_arrLineX5.clear();
    m_arrLineX6.clear();

    m_arrLineY1.clear();
    m_arrLineY2.clear();
    m_arrLineY3.clear();
    m_arrLineY4.clear();
    m_arrLineY5.clear();
    m_arrLineY6.clear();

    m_arrAve.clear();
    m_arrMax.clear();

    if (__flowAve) {
        m_arrLineX1.append(75);
        m_arrLineX1.append(100);
        m_arrLineX1.append(125);
        m_arrLineX1.append(150);
        m_arrLineX1.append(175);
        m_arrLineX1.append(200);
        m_arrLineX1.append(225);
        m_arrLineX1.append(250);
        m_arrLineX1.append(275);
        m_arrLineX1.append(300);
        m_arrLineX1.append(325);
        m_arrLineX1.append(350);
        m_arrLineX1.append(375);
        m_arrLineX1.append(400);
        m_arrLineX1.append(425);
        m_arrLineX1.append(450);
        m_arrLineX1.append(475);
        m_arrLineX1.append(499);

        m_arrLineY1.append(1.2);
        m_arrLineY1.append(2.0);
        m_arrLineY1.append(2.65);
        m_arrLineY1.append(3.2);
        m_arrLineY1.append(3.7);
        m_arrLineY1.append(4.2);
        m_arrLineY1.append(4.6);
        m_arrLineY1.append(5);
        m_arrLineY1.append(5.4);
        m_arrLineY1.append(5.85);
        m_arrLineY1.append(6.2);
        m_arrLineY1.append(6.6);
        m_arrLineY1.append(6.9);
        m_arrLineY1.append(7.2);
        m_arrLineY1.append(7.5);
        m_arrLineY1.append(7.8);
        m_arrLineY1.append(8.0);
        m_arrLineY1.append(8.2);

        m_arrLineX2.append(25);
        m_arrLineX2.append(50);
        m_arrLineX2.append(75);
        m_arrLineX2.append(100);
        m_arrLineX2.append(125);
        m_arrLineX2.append(150);
        m_arrLineX2.append(175);
        m_arrLineX2.append(200);
        m_arrLineX2.append(225);
        m_arrLineX2.append(250);
        m_arrLineX2.append(275);
        m_arrLineX2.append(300);
        m_arrLineX2.append(325);
        m_arrLineX2.append(350);
        m_arrLineX2.append(375);
        m_arrLineX2.append(400);
        m_arrLineX2.append(425);
        m_arrLineX2.append(450);
        m_arrLineX2.append(475);
        m_arrLineX2.append(499);

        m_arrLineY2.append(1.2);
        m_arrLineY2.append(2.35);
        m_arrLineY2.append(3.3);
        m_arrLineY2.append(4.2);
        m_arrLineY2.append(5.0);
        m_arrLineY2.append(5.8);
        m_arrLineY2.append(6.5);
        m_arrLineY2.append(7.25);
        m_arrLineY2.append(8.0);
        m_arrLineY2.append(8.7);
        m_arrLineY2.append(9.35);
        m_arrLineY2.append(9.93);
        m_arrLineY2.append(10.5);
        m_arrLineY2.append(11.05);
        m_arrLineY2.append(11.65);
        m_arrLineY2.append(12.2);
        m_arrLineY2.append(12.7);
        m_arrLineY2.append(13);
        m_arrLineY2.append(13.2);
        m_arrLineY2.append(13.3);

        m_arrLineX3.append(18);
        m_arrLineX3.append(25);
        m_arrLineX3.append(50);
        m_arrLineX3.append(75);
        m_arrLineX3.append(100);
        m_arrLineX3.append(125);
        m_arrLineX3.append(150);
        m_arrLineX3.append(175);
        m_arrLineX3.append(200);
        m_arrLineX3.append(225);
        m_arrLineX3.append(250);
        m_arrLineX3.append(275);
        m_arrLineX3.append(300);
        m_arrLineX3.append(325);
        m_arrLineX3.append(350);
        m_arrLineX3.append(375);
        m_arrLineX3.append(400);
        m_arrLineX3.append(425);
        m_arrLineX3.append(450);
        m_arrLineX3.append(475);
        m_arrLineX3.append(499);

        m_arrLineY3.append(3.9);
        m_arrLineY3.append(4.3);
        m_arrLineY3.append(5.5);
        m_arrLineY3.append(6.7);
        m_arrLineY3.append(7.78);
        m_arrLineY3.append(8.8);
        m_arrLineY3.append(9.75);
        m_arrLineY3.append(10.55);
        m_arrLineY3.append(11.4);
        m_arrLineY3.append(12.2);
        m_arrLineY3.append(13);
        m_arrLineY3.append(13.8);
        m_arrLineY3.append(14.55);
        m_arrLineY3.append(15.12);
        m_arrLineY3.append(15.7);
        m_arrLineY3.append(16.2);
        m_arrLineY3.append(16.65);
        m_arrLineY3.append(17.05);
        m_arrLineY3.append(17.4);
        m_arrLineY3.append(17.6);
        m_arrLineY3.append(17.65);

        m_arrLineX4.append(18);
        m_arrLineX4.append(25);
        m_arrLineX4.append(50);
        m_arrLineX4.append(75);
        m_arrLineX4.append(100);
        m_arrLineX4.append(125);
        m_arrLineX4.append(150);
        m_arrLineX4.append(175);
        m_arrLineX4.append(200);
        m_arrLineX4.append(225);
        m_arrLineX4.append(250);
        m_arrLineX4.append(275);
        m_arrLineX4.append(300);
        m_arrLineX4.append(325);
        m_arrLineX4.append(350);
        m_arrLineX4.append(375);
        m_arrLineX4.append(400);
        m_arrLineX4.append(425);
        m_arrLineX4.append(450);
        m_arrLineX4.append(475);
        m_arrLineX4.append(499);

        m_arrLineY4.append(5.85);
        m_arrLineY4.append(6.3);
        m_arrLineY4.append(7.9);
        m_arrLineY4.append(9.25);
        m_arrLineY4.append(10.55);
        m_arrLineY4.append(11.85);
        m_arrLineY4.append(13.1);
        m_arrLineY4.append(14.22);
        m_arrLineY4.append(15.33);
        m_arrLineY4.append(16.45);
        m_arrLineY4.append(17.45);
        m_arrLineY4.append(18.35);
        m_arrLineY4.append(19.25);
        m_arrLineY4.append(19.99);
        m_arrLineY4.append(20.7);
        m_arrLineY4.append(21.25);
        m_arrLineY4.append(21.7);
        m_arrLineY4.append(22.05);
        m_arrLineY4.append(22.2);
        m_arrLineY4.append(22.3);
        m_arrLineY4.append(22.35);

        m_arrLineX5.append(20);
        m_arrLineX5.append(25);
        m_arrLineX5.append(50);
        m_arrLineX5.append(75);
        m_arrLineX5.append(100);
        m_arrLineX5.append(125);
        m_arrLineX5.append(150);
        m_arrLineX5.append(175);
        m_arrLineX5.append(200);
        m_arrLineX5.append(225);
        m_arrLineX5.append(250);
        m_arrLineX5.append(275);
        m_arrLineX5.append(300);
        m_arrLineX5.append(325);
        m_arrLineX5.append(350);
        m_arrLineX5.append(375);
        m_arrLineX5.append(400);
        m_arrLineX5.append(425);
        m_arrLineX5.append(450);
        m_arrLineX5.append(475);
        m_arrLineX5.append(499);

        m_arrLineY5.append(8.9);
        m_arrLineY5.append(9.2);
        m_arrLineY5.append(10.8);
        m_arrLineY5.append(12.32);
        m_arrLineY5.append(13.74);
        m_arrLineY5.append(15.15);
        m_arrLineY5.append(16.53);
        m_arrLineY5.append(17.88);
        m_arrLineY5.append(19.1);
        m_arrLineY5.append(20.3);
        m_arrLineY5.append(21.37);
        m_arrLineY5.append(22.4);
        m_arrLineY5.append(23.4);
        m_arrLineY5.append(24.2);
        m_arrLineY5.append(24.99);
        m_arrLineY5.append(25.6);
        m_arrLineY5.append(26.15);
        m_arrLineY5.append(26.6);
        m_arrLineY5.append(26.9);
        m_arrLineY5.append(26.95);
        m_arrLineY5.append(26.99);

        m_arrAve.append(8.7);
        m_arrAve.append(13.9);
        m_arrAve.append(18.2);
        m_arrAve.append(23.2);
        m_arrAve.append(27.4);
    }
    else {
        m_arrLineX1.append(50);
        m_arrLineX1.append(75);
        m_arrLineX1.append(100);
        m_arrLineX1.append(125);
        m_arrLineX1.append(150);
        m_arrLineX1.append(175);
        m_arrLineX1.append(200);
        m_arrLineX1.append(225);
        m_arrLineX1.append(250);
        m_arrLineX1.append(275);
        m_arrLineX1.append(300);
        m_arrLineX1.append(325);
        m_arrLineX1.append(350);
        m_arrLineX1.append(375);
        m_arrLineX1.append(400);
        m_arrLineX1.append(425);
        m_arrLineX1.append(450);
        m_arrLineX1.append(475);
        m_arrLineX1.append(499);

        m_arrLineY1.append(5.1);
        m_arrLineY1.append(5.95);
        m_arrLineY1.append(6.4);
        m_arrLineY1.append(6.7);
        m_arrLineY1.append(6.91);
        m_arrLineY1.append(7.1);
        m_arrLineY1.append(7.3);
        m_arrLineY1.append(7.42);
        m_arrLineY1.append(7.55);
        m_arrLineY1.append(7.7);
        m_arrLineY1.append(7.8);
        m_arrLineY1.append(7.9);
        m_arrLineY1.append(7.95);
        m_arrLineY1.append(7.99);
        m_arrLineY1.append(8);
        m_arrLineY1.append(8.05);
        m_arrLineY1.append(8.1);
        m_arrLineY1.append(8.15);
        m_arrLineY1.append(8.2);

        m_arrLineX2.append(50);
        m_arrLineX2.append(65);
        m_arrLineX2.append(90);
        m_arrLineX2.append(125);
        m_arrLineX2.append(160);
        m_arrLineX2.append(200);
        m_arrLineX2.append(250);
        m_arrLineX2.append(310);
        m_arrLineX2.append(410);
        m_arrLineX2.append(500);

        m_arrLineY2.append(7.4);
        m_arrLineY2.append(8);
        m_arrLineY2.append(9);
        m_arrLineY2.append(10);
        m_arrLineY2.append(11);
        m_arrLineY2.append(12);
        m_arrLineY2.append(13);
        m_arrLineY2.append(14);
        m_arrLineY2.append(15);
        m_arrLineY2.append(15.2);

        m_arrLineX3.append(50);
        m_arrLineX3.append(75);
        m_arrLineX3.append(100);
        m_arrLineX3.append(125);
        m_arrLineX3.append(150);
        m_arrLineX3.append(175);
        m_arrLineX3.append(200);
        m_arrLineX3.append(225);
        m_arrLineX3.append(250);
        m_arrLineX3.append(275);
        m_arrLineX3.append(300);
        m_arrLineX3.append(325);
        m_arrLineX3.append(350);
        m_arrLineX3.append(375);
        m_arrLineX3.append(400);
        m_arrLineX3.append(425);
        m_arrLineX3.append(450);
        m_arrLineX3.append(475);
        m_arrLineX3.append(500);

        m_arrLineY3.append(10);
        m_arrLineY3.append(11.4);
        m_arrLineY3.append(12.67);
        m_arrLineY3.append(13.8);
        m_arrLineY3.append(14.85);
        m_arrLineY3.append(15.9);
        m_arrLineY3.append(16.9);
        m_arrLineY3.append(17.9);
        m_arrLineY3.append(18.7);
        m_arrLineY3.append(19.53);
        m_arrLineY3.append(20.2);
        m_arrLineY3.append(20.8);
        m_arrLineY3.append(21.32);
        m_arrLineY3.append(21.7);
        m_arrLineY3.append(22);
        m_arrLineY3.append(22.15);
        m_arrLineY3.append(22.3);
        m_arrLineY3.append(22.4);
        m_arrLineY3.append(22.4);

        m_arrLineX4.append(50);
        m_arrLineX4.append(75);
        m_arrLineX4.append(100);
        m_arrLineX4.append(125);
        m_arrLineX4.append(150);
        m_arrLineX4.append(175);
        m_arrLineX4.append(200);
        m_arrLineX4.append(225);
        m_arrLineX4.append(250);
        m_arrLineX4.append(275);
        m_arrLineX4.append(300);
        m_arrLineX4.append(325);
        m_arrLineX4.append(350);
        m_arrLineX4.append(375);
        m_arrLineX4.append(400);
        m_arrLineX4.append(425);
        m_arrLineX4.append(450);
        m_arrLineX4.append(475);
        m_arrLineX4.append(500);

        m_arrLineY4.append(12);
        m_arrLineY4.append(13.7);
        m_arrLineY4.append(15.2);
        m_arrLineY4.append(16.75);
        m_arrLineY4.append(18.15);
        m_arrLineY4.append(19.5);
        m_arrLineY4.append(20.8);
        m_arrLineY4.append(22.05);
        m_arrLineY4.append(23.28);
        m_arrLineY4.append(24.38);
        m_arrLineY4.append(25.45);
        m_arrLineY4.append(26.4);
        m_arrLineY4.append(27.12);
        m_arrLineY4.append(27.85);
        m_arrLineY4.append(28.45);
        m_arrLineY4.append(28.9);
        m_arrLineY4.append(29.1);
        m_arrLineY4.append(29.2);
        m_arrLineY4.append(29.2);

        m_arrMax.append(8.7);
        m_arrMax.append(15.8);
        m_arrMax.append(23.1);
        m_arrMax.append(29.4);
    }
}

