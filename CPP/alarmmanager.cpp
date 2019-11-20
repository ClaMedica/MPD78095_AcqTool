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
    m_confAla = NULL;
}

AlarmManager::~AlarmManager()
{
    if(m_confAla == NULL)
        delete m_confAla;

    foreach(AlarmTimer *a, m_ATMap)
        delete a;
}

bool AlarmManager::load(QString __fileName)
{
    m_confAla = new Ancestry;
    if(!m_confAla->loadFromXML(__fileName)) {
        qWarning() << "File corrupted";
        delete m_confAla;
        m_confAla = NULL;
        return false;
    }
    foreach (Ancestry *alarm, m_confAla->getChildren()) {
        m_vecMap[alarm->getAttribute("code").toInt()] = alarm->name();
        m_enabledAlarms[alarm->getAttribute("code").toInt()] = true;
    }

    qDebug() << "Alarms loaded:" << m_confAla->childrenNames();

    return true;
}

bool AlarmManager::addAlarm(int __code)
{

    if(m_confAla == NULL) {
        qDebug() /*qCritical()*/ << "No alarm configuration file loaded";
        return false;
    }

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
    Ancestry *child = m_confAla->getChild(m_vecMap[__code]);
    if(child == NULL) {
        qWarning() << m_vecMap[__code] << MEX_CHILD_NOT_ALIVE;
        return false;
    }

    //Config_Alarms TESTO DA TRADURRE
    static const char* stringTraslated[] = {
        QT_TR_NOOP("Fatal Error"),                      //0 code 0 Text ALA_NOT_CONNECTED
        QT_TR_NOOP("Call the technical service"),       //1 code 0 Help
        QT_TR_NOOP("Cell not connected"),               //2 code 200 Text ALA_NOT_CONNECTED
        QT_TR_NOOP("Turn the cell off and on again"),   //3 code 200/201 Help ALA_NOT_CONNECTED/ALA_NOT_ACQUIRING
        QT_TR_NOOP("Interrupted acquisition"),          //4 code 201 Text ALA_NOT_ACQUIRING
        QT_TR_NOOP("Beaker removed"),                   //5 code 202 Text ALA_NO_BEAKER
        QT_TR_NOOP("Replace the Beaker"),               //6 code 202 Help ALA_NO_BEAKER
        QT_TR_NOOP("The Beaker is full"),               //7 code 203 Text ALA_FULL_BEAKER
        QT_TR_NOOP("Empty the Beaker"),                 //8 code 203 Help ALA_FULL_BEAKER
        QT_TR_NOOP("Allarm not present")                //9 code non previsto
    };

    int indexAllarmText = -1, indexAllarmHelp = -1;
    if (__code == 0 ) //fatal error
    {
        indexAllarmText = 0;
        indexAllarmHelp = 1;
    } else if (__code == ALA_NOT_CONNECTED ) //cella non connessa
    {
        indexAllarmText = 2;
        indexAllarmHelp = 3;
    } else if (__code == ALA_NOT_ACQUIRING ) //acquisizione interrotta
    {
        indexAllarmText = 4;
        indexAllarmHelp = 3;
    } else if (__code == ALA_NO_BEAKER)
    {
        indexAllarmText = 5;
        indexAllarmHelp = 6;
    } else if (__code == ALA_FULL_BEAKER)
    {
        indexAllarmText = 7;
        indexAllarmHelp = 8;
    } else
    {
        indexAllarmText = 9;
        indexAllarmHelp = 9;
    }

    ala["message"] = tr(stringTraslated[indexAllarmText]);
    ala["help"]    = tr(stringTraslated[indexAllarmHelp]);
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

