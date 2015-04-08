#include "parametermanager.h"
#include "QXmlStreamReader"
ParameterManager::ParameterManager(QObject *parent) :
    QObject(parent)
{
    QXmlStreamReader xml;
    QFile f;
    QString field;
    QMap<QString,VarMap> nuovo;
    f.setFileName("../Doppler/Config/parameters.xml");
    if(f.open(QIODevice::ReadOnly))
    {
        xml.setDevice(&f);
        while(!xml.atEnd() &&  !xml.hasError()) {
            /* Read next element.*/
            QXmlStreamReader::TokenType token = xml.readNext();
            /* If token is just StartDocument, we'll go to next.*/
            if(token == QXmlStreamReader::StartDocument)continue;
            /* If token is StartElement, we'll see if we can read it.*/
            if(token == QXmlStreamReader::StartElement) {
                QString name=xml.name().toString();
                if(name=="Document")continue;
                QXmlStreamAttributes attributes=xml.attributes();
                if(attributes.length()==1)
                {
                    if(nuovo.size()>0)
                    {//salvo quello vecchio
                    m_parameterMap[field]=nuovo;
                    nuovo.clear();
                    }
                    field=attributes.at(0).value().toString();
                }
                else
                {
                    VarMap var;
                    var["name"]=name;
                    for(int i=0;i<attributes.length();i++)
                    {
                        QString tag=attributes.at(i).name().toString();
                        QVariant value=attributes.at(i).value().toString();
                        var[tag]=value;
                    }
                    nuovo[name]=var;
                }
            }
        }
        if(nuovo.size()>0)
        {//salvo quello vecchio
        m_parameterMap[field]=nuovo;
        nuovo.clear();
        }
        f.close();
    }
}

QVariantList ParameterManager::items()
{
    return m_items;
}

QVariant ParameterManager::curPar()
{
    VarMap *par=new VarMap;

    QStringList keys=m_current.keys();
    for(int i=0;i<keys.length();i++)
    {
        (*par)[keys[i]]=m_current[keys[i]]["val"];
    }

    return (qulonglong)par;
}

QVariantList ParameterManager::lastParMod()
{
    return m_lastParMod;
}

void ParameterManager::setLastParMod(QVariantList __l)
{
    if(m_lastParMod!=__l)
    {
        m_lastParMod=__l;
        if(m_lastParMod.size()==3)
        {
            QString anaName=m_lastParMod[0].toString();
            QString parName=m_lastParMod[1].toString();
            float val=m_lastParMod[2].toFloat();
            m_parameterMap[anaName][parName]["val"]=val;
            m_current=m_parameterMap[anaName];
        }
        emit lastParModChanged();
    }
}

QString ParameterManager::curAnalysis()
{
    return m_curAnalysis;
}

void ParameterManager::setCurAnalysis(QString __cur)
{
    if(m_curAnalysis!=__cur)
    {
        m_curAnalysis=__cur;
        if(!m_parameterMap.contains(__cur))
        {
            qDebug()<<"ParameterManager::setType curAnalysis not recognized";
            return;
        }
        m_current=m_parameterMap[__cur];
        emit curAnalysisChanged();        
    }
    updateItems();
}

void ParameterManager::setRoles(QStringList __roles)
{
    if(__roles!=m_roles)
    {
        m_roles=__roles;
        emit rolesChanged();
    }
}

void ParameterManager::updateItems()
{

    QString sTag="$Parameter";
    QString eTag="&Parameter";

    m_items.clear();
    QStringList parNames=m_current.keys();
    for(int vecIndex=0;vecIndex<parNames.length();vecIndex++)
    {
        m_items<<sTag;
        for(int r=0;r<m_roles.size();r++)
        {
            QString role=m_roles[r];
            m_items<<role;
            m_items<<m_current[parNames[vecIndex]][role];
        }
        m_items<<eTag;
    }

    emit itemsChanged();
}


