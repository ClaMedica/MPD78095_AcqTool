#include "alarmmanager.h"

AlarmTimer::AlarmTimer(TimeoutAlarmRecord __code){
    m_code=(int)__code;
    m_tim=new QTimer;
    m_tim->setSingleShot(true);
    connect(m_tim,SIGNAL(timeout()),this,SLOT(send()));
}

AlarmTimer::~AlarmTimer(){delete m_tim;}

void AlarmTimer::reset(){start(TIMEOUT_AFTER_RESET);}

void AlarmTimer::start(int __msec){m_tim->start(__msec);}

void AlarmTimer::send(){qDebug()<<"Timeout Alarm code = "<<m_code;emit timeout(m_code);}

AlarmManager::AlarmManager(QObject *parent) : QObject(parent)
{
    m_confAla=NULL;
    for(int i=T_ALA_TIMEOUT_STATUS;i<T_ALA_NUM;i++)
    {
        TimeoutAlarmRecord curAla=(TimeoutAlarmRecord)i;
        m_ATMap[curAla]=new AlarmTimer(curAla);
        connect(m_ATMap[i],SIGNAL(timeout(int)),this,SLOT(addAlarm(int)));
    }


}

AlarmManager::~AlarmManager()
{
    if(m_confAla==NULL)
        delete m_confAla;
}

bool AlarmManager::load(QString __fileName)
{
    m_confAla=new Ancestry;
    if(!m_confAla->loadFromXML(__fileName))
    {
        qCritical()<<"File corrupted";
        delete m_confAla;
        m_confAla=NULL;
        return false;
    }
    foreach (Ancestry *alarm,m_confAla->getChildren()) {
            m_vecMap[alarm->getAttribute("code").toInt()]=alarm->name();
            m_enabledAlarms[alarm->getAttribute("code").toInt()]=true;
        }
    qDebug()<<"Alarms loaded:"<<m_confAla->childrenNames();
    return true;

}

void AlarmManager::addAlarm(int __code)
{

    if(m_confAla==NULL)
    {qCritical()<<"No alarm configuration file loaded";return;}
    if(m_enabledAlarms.contains(__code))
        if(!m_enabledAlarms[__code])
        {
            return;
        }//allarme disabilitato

    VarMap ala;

    foreach (VarMap raisedAlarms, m_alarms) {
        if(raisedAlarms["code"]==__code)
            return;
    }

    ala["code"]=__code;
    Ancestry *child=curAlarmSet->getChild(m_vecMap[__code]);
    if(child==NULL)
    {qCritical()<<m_vecMap[__code]<<MEX_CHILD_NOT_ALIVE;return;}
    qDebug()<<"allarme3";
    ala["message"]=child->getTextOfChild("Text");
    ala["help"]=child->getTextOfChild("Help");
    ala["color"]="red";
    qDebug()<<"allarme4";
    m_alarms.append(ala);
    updateAlarms();
    qDebug()<<"Alarm! "<<__code;
}

void AlarmManager::resetAlarms(QList<int> __codes)
{
    if(__codes.isEmpty())
    {
        foreach (AlarmTimer *tim, m_ATMap.values()) {
            tim->reset();
        }
        m_alarms.clear();
    }
    else
    {
        QVector<VarMap> old=m_alarms;
        m_alarms.clear();
        foreach (VarMap alarm,old)
        {
            if(!__codes.contains(alarm["code"].toInt()))
                m_alarms<<alarm;
        }
    }
    updateAlarms();
}

void AlarmManager::startTimeoutAlarms(int __code, int __time)
{
    if(__code==-1)
    {//partono tutti
        foreach (AlarmTimer *tim, m_ATMap.values()) {
            tim->start(TIMEOUT_TIME_ON_STATUS);
            m_enabledAlarms[tim->code()]=true;
        }
    }
    else
    {
        m_enabledAlarms[__code]=true;
        m_ATMap[__code]->start(__time);
    }
}

bool AlarmManager::manageAlarm(int __code, bool __enable)
{
    if(m_enabledAlarms.contains(__code))
    {
        m_enabledAlarms[__code]=__enable;
        return true;
    }
    else
    {qCritical()<<"Code "+QString::number(__code)+" not found";return false;}
}

void AlarmManager::enableAll()
{
    foreach (int code, m_enabledAlarms.keys()) {
        m_enabledAlarms[code]=true;
    }
}

void AlarmManager::disableAll()
{
    foreach (int code, m_enabledAlarms.keys()) {
        m_enabledAlarms[code]=false;
    }
}

void AlarmManager::updateAlarms()
{
    m_alarmList.clear();

    foreach (VarMap alarm, m_alarms) {
        m_alarmList<<"$Alarm";
        foreach(QString key,alarm.keys())
        {
            m_alarmList<<key;
            m_alarmList<<alarm[key];
        }
        m_alarmList<<"&Alarm";
    }

    //qDebug()<<"m_alarmList = "<<m_alarmList;
    emit alarmsUpdated(m_alarmList);
}

