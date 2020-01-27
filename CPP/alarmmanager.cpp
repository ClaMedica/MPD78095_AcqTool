#include "alarmmanager.h"

AlarmTimer::AlarmTimer(AlarmRecord __code) {
    m_code = (int) __code;
    m_tim = new QTimer;
    m_tim->setSingleShot(true);
    connect(m_tim, SIGNAL(timeout()), this, SLOT(send()));
}

AlarmTimer::~AlarmTimer() {delete m_tim;}

void AlarmTimer::reset() {start(TIMEOUT_AFTER_RESET);}

void AlarmTimer::start(int __msec) {m_tim->start(__msec);}

void AlarmTimer::stop() {m_tim->stop();}

void AlarmTimer::send() {qDebug() << "Timeout Alarm code = " << m_code; emit timeout(m_code);}

AlarmManager::AlarmManager(QObject *parent) : QObject(parent)
{

}

AlarmManager::~AlarmManager()
{
    foreach(AlarmTimer *a, m_ATMap)
        delete a;
}

bool AlarmManager::loadAllarm(QMap<int,QStringList> __msg)
{
    QMap<int,QStringList>::iterator map;
    for (map = __msg.begin(); map != __msg.end(); ++map)
    {
        m_enabledAlarms[map.key()] = true;
        m_vecMap[map.key()] = map.value();
    }

    return true;
}

bool AlarmManager::addAlarm(int __code)
{
    if(m_enabledAlarms.contains(__code))
        if(!m_enabledAlarms[__code]) {
            return false;
        }               //allarme disabilitato

    VarMap ala;

    foreach (VarMap raisedAlarms, m_alarms) {
        if(raisedAlarms["code"] == __code)
            return false;
    }

    ala["code"] = __code;
    QStringList listMessage = m_vecMap[__code];
    if(listMessage.length() == 0) {
        qWarning() << m_vecMap[__code] << "Alarm not found";
        return false;
    }

    ala["message"] = m_vecMap[__code].value(0);
    ala["help"]    = m_vecMap[__code].value(1);
    ala["color"]   = "red";
    ala["sound"]   = "file:///" + g_P7SettingsManager.appPath() + "/Alarm.wav";

    m_alarms.append(ala);
    updateAlarms();
    qDebug() << "Alarm! " << __code;
    return true;
}

void AlarmManager::resetAlarms()
{
    foreach (int code, m_ATMap.keys()) {
        if(m_repeatAlarms.contains(code))
            m_ATMap[code]->reset();
    }
    m_alarms.clear();

    updateAlarms();
}

void AlarmManager::resetAlarm(int __code)
{
    if(m_repeatAlarms.contains(__code))
        m_ATMap[__code]->reset();

    foreach (VarMap raisedAlarms, m_alarms) {
        if(raisedAlarms["code"] == __code) {
            int index = m_alarms.indexOf(raisedAlarms);
            m_alarms.remove(index);
            break;
        }
    }

    updateAlarms();
}


void AlarmManager::startTimeoutAlarm(int __code, int __time,bool __repeat)
{
    //qDebug()<<"Alarm"<<__code<<"will come in"<<__time<<"seconds";
    if(!m_ATMap.contains(__code)) {
        m_ATMap[__code] = new AlarmTimer((AlarmRecord) __code);
        connect(m_ATMap[__code], SIGNAL(timeout(int)), this, SLOT(addAlarm(int)));
    }
    m_enabledAlarms[__code] =true;
    m_repeatAlarms << __repeat;
    m_ATMap[__code]->start(__time);
}

void AlarmManager::stopTimeoutAlarm(int __code)
{
    if(m_ATMap.contains(__code))
        m_ATMap[__code]->stop();
}

bool AlarmManager::manageAlarm(int __code, bool __enable)
{
    if(m_enabledAlarms.contains(__code)) {
        m_enabledAlarms[__code] = __enable;
        return true;
    }
    else {
        qDebug() << "Code " + QString::number(__code) + " not found";
        return false;
    }
}

void AlarmManager::enableAll()
{
    foreach (int code, m_enabledAlarms.keys()) {
        m_enabledAlarms[code] = true;
    }
}

void AlarmManager::disableAll()
{
    foreach (int code, m_enabledAlarms.keys()) {
        m_enabledAlarms[code] = false;
    }
}

void AlarmManager::updateAlarms()
{
    m_alarmList.clear();

    foreach (VarMap alarm, m_alarms) {
        m_alarmList << "$Alarm";
        foreach(QString key, alarm.keys()) {
            m_alarmList << key;
            m_alarmList << alarm[key];
        }
        m_alarmList << "&Alarm";
    }

    //qDebug()<<"m_alarmList = "<<m_alarmList;
    emit alarmsUpdated(m_alarmList);
}

