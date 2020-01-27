#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <global.h>
#include <ancestry.h>
#include <p7settingsmanager.h>
#include "flowBT_types.h"
enum AlarmRecord
{
    ALA_NOT_CONNECTED=200,
    ALA_NOT_ACQUIRING,
    ALA_NO_BEAKER,
    ALA_FULL_BEAKER,
    ALA_STABILIZE,
    ALA_TIMEOUT_STATUS=300,
    ALA_NUM
};
class AlarmTimer : public QObject
{
    Q_OBJECT
public:
    AlarmTimer(AlarmRecord __code);
    ~AlarmTimer();
    void reset();
    void start(int __msec);
    int code(){return m_code;}
    void stop();
signals:
    void timeout(int);
public slots:
    void send();
private:
    QTimer *m_tim;
    int m_code;

};

class AlarmManager : public QObject
{
    Q_OBJECT
public:
    explicit AlarmManager(QObject *parent = 0);
    ~AlarmManager();

    bool loadAllarm(QMap<int, QStringList> __msg);
    QVector<VarMap> * alarmVector(){return &m_alarms;}
    void resetAlarms();
    void resetAlarm(int __code);
    void startTimeoutAlarm(int __code, int __time=TIMEOUT_TIME, bool __repeat=false);
    void stopTimeoutAlarm(int __code);
    bool manageAlarm(int __code,bool __enable);
    void enableAll();
    void disableAll();
signals:
    void alarmsUpdated(QVariantList);

public slots:
    bool addAlarm(int __code);
private:

    QMap<int,AlarmTimer *> m_ATMap;
    QVector<VarMap> m_alarms;
    QVariantList m_alarmList;
    QMap<int,QStringList> m_vecMap;
    QMap<int,bool> m_enabledAlarms;
    QList<int> m_repeatAlarms;
    void updateAlarms();
};

#endif // ALARMMANAGER_H
