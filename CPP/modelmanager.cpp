#include "modelmanager.h"

ModelManager::ModelManager(QObject *parent) :
    QObject(parent)
{
}

void ModelManager::setType(QString __type)
{
    if(__type!=m_type)
    {
        m_type=__type;
        emit typeChanged();
    }
}

void ModelManager::setInfoList(QVariantList __list)
{
    m_infoList=__list;
    //qDebug()<<__list;
    for(int plotIndex=0;plotIndex<m_infoList.size();plotIndex++)
    {
        QVariantList plotInfo=m_infoList.at(plotIndex).toList();
        QString plotName=plotInfo.at(0).toString();
        QStringList info=plotInfo.at(1).toStringList();
        //qDebug()<<plotName<<info;
        m_infoMap[plotName]=info;
    }
    updateModelList();
}

void ModelManager::setRoles(QStringList __roles)
{
    __roles.sort();
    if(__roles!=m_roles)
    {
        m_roles=__roles;
        emit rolesChanged();
    }
}

void ModelManager::updateModelList()
{
    QStringList plotNames=m_infoMap.keys();
    QString sGroup="$"+m_type+"Group";
    QString eGroup="&"+m_type+"Group";
    QString sType="$"+m_type;
    QString eType="&"+m_type;

    m_drawList.clear();

    foreach(QString currentPlot,plotNames) {
        //per ogni plot che ho controllo cosa devo disegnare e se c'è qualcosa del mio type
        QStringList objList=m_infoMap[currentPlot];
        m_drawList<<sGroup;
        m_drawList<<currentPlot;

        foreach(QString item,objList) {
            //sto scorrendo la lista delle cose da disegnare nel plot
            //e controllo se c'è qualcosa del mio tipo
            QString family=item.section(":",0,0);
            QString name=item.section(":",1,1);

            //qDebug()<<"***Family & Name****";
            //qDebug()<<family<<name;


            if(m_pStore->contains(family,name))
            {
                VarMapVec *curVec=(VarMapVec *)m_pStore->pickUp(family,name);
                foreach(VarMap *curMap,(*curVec))
                {
                    //qDebug()<<curMap;
                    bool correct=true;
                    foreach (QString curRole, curMap->keys())
                    if(!m_roles.contains(curRole))
                        correct=false;

                    //qDebug()<<"Object"<<curMap<<"can be drawn?"<<correct;
                    if(!correct){
                    //qDebug()<<"m_roles"<<m_roles;
                    //qDebug()<<"cur roles"<<curMap->keys();
                    }
                    if(correct)
                    {
                        m_drawList<<sType;
                        m_drawList<<"family";
                        m_drawList<<family;
                        m_drawList<<"name";
                        m_drawList<<name;

                        foreach (QString curRole, curMap->keys())
                        {
                            m_drawList<<curRole;
                            m_drawList<<curMap->value(curRole);
                        }
                        m_drawList<<eType;
                    }

                }

            }
            else
            {
                qCritical()<<"No family or name founded!";return;
            }
        }
        m_drawList<<eGroup;
    }
    if(!m_drawList.isEmpty())
    {
        //qDebug()<<m_drawList;
        emit drawListChanged();
    }
}


