#ifndef SAFEDFM_H
#define SAFEDFM_H

#include "datafilemanager.h"
#include "global.h"

class SafeDFM
{//safe class for datafilemanager use
public:
    SafeDFM(QString __fileName, int32_t __type=5, QString __openMode="OPEN");
    ~SafeDFM();


    bool open(QString __fileName, int32_t __type=5);
    bool close();
    bool create(QString __fileName,int32_t __type=5,int32_t __numberOfChannels=1);
    bool setChannelName(int32_t __channelNumber=-1,QString __name="");
    bool addValue(float __value, int32_t __channelNumber, bool __commit=false);
    QString getPatientName();
    QList<VarMap> getOpMarkers();
    QList<VarMap> getDefiners();
    QList<VarMap> getAnMarkers();
private:
    void populate();
    QString m_state;
    uchar m_type;
    QString m_fileName;
    QVector<int32_t> m_allowedTypes;
    DatafileManager *m_mng;
    VarMapVec *m_opMarkers,*m_definers,m_anMarkers;
};

#endif // SAFEDFM_H
