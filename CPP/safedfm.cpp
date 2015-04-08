#include "safedfm.h"

SafeDFM::SafeDFM(QString __fileName, int32_t __type,QString __openMode)
{
    m_allowedTypes<<5<<4;
    m_mng=NULL;

    m_state="IDLE";
    if(!__fileName.isEmpty() && m_allowedTypes.contains(__type))
    {
        if(QFile::exists(__fileName))
        {
            m_mng=new DatafileManager;
            m_mng->SetFileName(__fileName);
            m_mng->SetFileType(__type);
            if(__openMode=="OPEN")
            {
                if(m_mng->Open())
                    m_mng->GetParameters();
            }
            else if(__openMode=="CREATE")
                m_mng->Create();
        }
    }
}

SafeDFM::~SafeDFM()
{
    if(m_mng!=NULL)
    {
        m_mng->Close();
        delete m_mng;
    }
}

bool SafeDFM::open(QString __fileName, int32_t __type)
{
    if(!__fileName.isEmpty() && m_allowedTypes.contains(__type) && m_mng==NULL)
    {
        if(QFile::exists(__fileName))
        {
            m_mng=new DatafileManager;
            m_mng->SetFileName(__fileName);
            m_mng->SetFileType(__type);
            if(m_mng->Open())
            {
                m_state="OPEN";
                return true;
            }
            qDebug()<<"Error in open";
            return false;
        }
    }
    qDebug()<<"Wrong parameters"<<__fileName<<__type;
    return false;
}

bool SafeDFM::close()
{
    if(m_mng!=NULL)
        m_mng->Close();
    m_state="IDLE";
}

bool SafeDFM::create(QString __fileName, int32_t __type, int32_t __numberOfChannels)
{
    if(!__fileName.isEmpty() && m_allowedTypes.contains(__type))
    {
        if(QFile::exists(__fileName))
        {
            m_mng=new DatafileManager;
            m_mng->SetFileName(__fileName);
            m_mng->SetFileType(__type);
            if(m_mng->Create())
            {
                if(__numberOfChannels>=0)
                    m_mng->SetChanNum(__numberOfChannels);
                m_mng->CommitParameters();
                m_state="ADD";
                return true;
            }
            qDebug()<<"Error in create";
            return false;
        }
    }
    qDebug()<<"Wrong parameters"<<__fileName<<__type<<__numberOfChannels;
    return false;
}

bool SafeDFM::setChannelName(int32_t __channelNumber, QString __name)
{
    if(m_mng!=NULL && m_state=="ADD")
    {
        if(__channelNumber>=0 && __channelNumber<m_mng->GetChanNum() && !__name.isEmpty())
        {
            m_mng->SetChanName(__channelNumber,__name);
            m_mng->CommitParameters();
            return true;
        }
        qDebug()<<"Wrong parameters"<<__channelNumber<<__name;
        return false;
    }
    qDebug()<<"Wrong state"<<m_mng<<m_state;
    return false;
}

bool SafeDFM::addValue(float __value, int32_t __channelNumber,bool __commit)
{
    if(m_mng!=NULL && m_state=="ADD")
    {
        if(__channelNumber>=0 && __channelNumber<m_mng->GetChanNum())
        {
            m_mng->AppendValue(&__channelNumber,&__value,1);
            if(__commit)
                m_mng->CommitValues();
            return true;
        }
        qDebug()<<"Wrong parameters"<<__channelNumber<<__name;
        return false;
    }
    qDebug()<<"Wrong state"<<m_mng<<m_state;
    return false;
}

QString SafeDFM::getPatientName()
{
    if(m_mng!=NULL)
        return m_mng->GetPatientName().section(";",0,1);
    qDebug()<<"Wrong state"<<m_mng;
    return false;
}

int32_t SafeDFM::GetNumOperativeMarkers()
{
    if(m_mng!=NULL)
        return m_mng->GetNumOperativeMarkers();
    qDebug()<<"Wrong state"<<m_mng;
    return 0;
}

QList<VarMap> SafeDFM::getOpMarkers(void)
{
    QList<VarMap> markers;
    if(m_mng!=NULL)
    {
        VarMap mrk;
        for(int i=0;i<m_mng->GetNumOperativeMarkers();i++)
        {
        uchar key;
        int32_t numSamp;
        QString descr;
        m_mng->GetOpMarker(i,&key,numSamp,&descr);
        mrk["key"]=key;
        mrk["val"]=(float)numSamp[i]/m_mng->GetNAS(0);
        mrk["des"]=descr;
        markers.append(mrk);
        }
    }
    return markers;
}

QList<VarMap> SafeDFM::getAnMarkers(void)
{
    QList<VarMap> markers;
    if(m_mng!=NULL)
    {
        VarMap mrk;
        for(int i=0;i<m_mng->GetNumAnalyticalMarkers();i++)
        {
        uchar key;
        int32_t numSamp;
        QString descr;
        m_mng->GetAnMarker(i,&key,numSamp,&descr,);
        mrk["key"]=key;
        mrk["val"]=(float)numSamp[i]/m_mng->GetNAS(0);
        mrk["des"]=descr;
        markers.append(mrk);
        }
    }
    return markers;
}

QList<VarMap> SafeDFM::getDefiners(void)
{
    QList<VarMap> markers;
    if(m_mng!=NULL)
    {
        VarMap mrk;
        for(int i=0;i<m_mng->GetNumOperativeMarkers();i++)
        {
        uchar key;
        int32_t numSamp;
        QString descr;
        if(__markerNumber>=0 && __markerNumber<m_mng->GetNumOperativeMarkers())
            m_mng->GetOpMarker(i,&key,numSamp,&descr);
        mrk["key"]=key;
        mrk["val"]=(float)numSamp[i]/m_mng->GetNAS(0);
        mrk["des"]=descr;
        markers.append(mrk);
        }
    }
    return markers;
}


