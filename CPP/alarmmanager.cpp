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

void AlarmManager::addAlarm(int __code)
{

    if(m_confAla == NULL) {
        qDebug() /*qCritical()*/ << "No alarm configuration file loaded";
        return;
    }

    if(m_enabledAlarms.contains(__code))
        if(!m_enabledAlarms[__code]) {
            return;
        }               //allarme disabilitato

    VarMap ala;

    foreach (VarMap raisedAlarms, m_alarms) {
        if(raisedAlarms["code"] == __code)
            return;
    }

    ala["code"] = __code;
    Ancestry *child = m_confAla->getChild(m_vecMap[__code]);
    if(child == NULL) {
        qCritical() << m_vecMap[__code] << MEX_CHILD_NOT_ALIVE;
        return;
    }

    //Config_Alarms TESTO DA TRADURRE
    static const char* stringTraslated[] = {
        QT_TR_NOOP("Fatal Error"),                      //code 0 Text
        QT_TR_NOOP("Call the technical service"),       //code 0 Help
        QT_TR_NOOP("Cell not connected"),               // code 200 Text
        QT_TR_NOOP("Turn the cell off and on again"),   //code 200/201 Help
        QT_TR_NOOP("Interrupted acquisition"),          //code 201 Text
        QT_TR_NOOP("Allarm not present")                //code non previsto
    };

    int indexAllarmText = -1, indexAllarmHelp = -1;
    if (__code == 0 ) //fatal error
    {
        indexAllarmText = 0;
        indexAllarmHelp = 1;
    } else if (__code == 200 ) //cella non connessa
    {
        indexAllarmText = 2;
        indexAllarmHelp = 3;
    } else if (__code == 201 ) //acquisizione interrotta
    {
        indexAllarmText = 4;
        indexAllarmHelp = 3;
    } else
    {
        indexAllarmText = 5;
        indexAllarmHelp = 5;
    }

    ala["message"] = tr(stringTraslated[indexAllarmText]);
    ala["help"]    = tr(stringTraslated[indexAllarmHelp]);
    ala["color"]   = "red";
    ala["sound"]   = "file:///" + g_P7SettingsManager.appPath() + "/Alarm.wav";

    m_alarms.append(ala);
    updateAlarms();
    qDebug() << "Alarm! " << __code;
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

