#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <global.h>
#include <ancestry.h>
#include "flowBT_types.h"
enum StaticAlarmRecord
{
    S_ALA_NOT_CONNECTED=200,
    S_ALA_NOT_ACQUIRING,
    S_ALA_NUM
};

enum TimeoutAlarmRecord
{
    T_ALA_TIMEOUT_STATUS=300,

    T_ALA_NUM
};
class AlarmTimer : public QObject
{
    Q_OBJECT
public:
    AlarmTimer(TimeoutAlarmRecord __code);
    ~AlarmTimer();
    void reset();
    void start(int __msec);
    int code(){return m_code;}
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

    bool load(QString __fileName);
    QVector<VarMap> * alarmVector(){return &m_alarms;}
    void resetAlarms(QList<int> __codes=QList<int>());
    void startTimeoutAlarms(int __code=-1, int __time=TIMEOUT_TIME_ON_STATUS);
    bool manageAlarm(int __code,bool __enable);
    void enableAll();
    void disableAll();
signals:
    void alarmsUpdated(QVariantList);

public slots:
    void addAlarm(int __code);
private:

    QMap<int,AlarmTimer *> m_ATMap;
    Ancestry *m_confAla;
    QVector<VarMap> m_alarms;
    QVariantList m_alarmList;
    QMap<int,QString> m_vecMap;
    QMap<int,bool> m_enabledAlarms;
    void updateAlarms();
};

#endif // ALARMMANAGER_H
