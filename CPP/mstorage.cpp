#include "mstorage.h"
#include "qmath.h"

MStorage::MStorage(QObject *parent) :
    QObject(parent)
{

}

VarMapVec* MStorage::pickUp(QString __family, QString __name)
{
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            return m_storage[__family][__name];
    return NULL;
}

VarMapVec* MStorage::getAll(QString __category)
{
    if(m_allMap.contains(__category))
        return m_allMap[__category];
    else
    {qCritical()<<"Category"<<__category<<"not found!";return NULL;}
}

QStringList MStorage::getFamilies()
{
    return m_storage.keys();
}

QStringList MStorage::getNames(QStringList __families, QStringList __filterCategory)
{
    QStringList list;
    foreach (QString family, __families)
        if(m_storage.keys().contains(family))
            foreach (QString name, m_storage[family].keys())
                foreach (QString category, __filterCategory) {
                    VarMapVec *curVec=m_storage[family][name];
                    if(curVec!=NULL)//cosA? per sfizio
                    {
                        VarMap *curMap=curVec->at(0);
                        if(curMap->value("category")==category)
                            if(!list.contains(name))
                                list<<name;
                    }
                }

    return list;
}
/**
 * @brief MStorage::archive
 * @param __family
 * @param __name
 * @param __elements
 * @param __whatIfAlreadyPresent
 * @return
 */
bool MStorage::archive(QString __family, QString __name, VarMapVec *__elements, bool __whatIfAlreadyPresent)
{
    //verifichiamo se c'A? giA  qualcosa con lo stesso nome all'interno della famiglia
    bool present=false;
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            present=true;

    foreach (VarMap *curMap, (*__elements))
    {
        QString cat=curMap->value("category").toString();
        if(cat=="")
        {qCritical()<<"Category item not defined";return false;}
        if(!m_allMap.contains(cat))
        {qCritical()<<"Category"<<cat<<"not registered";return false;}

        if(!m_allMap[cat]->contains(curMap))
        {
            curMap->insert("whoAmI",(qulonglong)curMap);
            m_allMap[cat]->append(curMap);
        }

    }
    //in questo modo ogni elemento ha un suo codice identificativo uguale all'indirizzo a cui punta
    qDebug()<<"stampo all map"<<m_allMap;
    if(present)
    {
        if(__whatIfAlreadyPresent==OVERWRITE)
        {//sovrascrive
            m_storage[__family][__name]=__elements;
        }
        else
        {//appende
            VarMapVec *oldVec=m_storage[__family][__name];
            (*oldVec)<<(*__elements);
            m_storage[__family][__name]=oldVec;
        }
    }
    else
    {
        //memorizzo nella mappa il puntatore ai dati
        m_storage[__family][__name]=__elements;
    }
    return true;
}

bool MStorage::contains(QString __family, QString __name)
{
    if(m_storage.contains(__family))
        return m_storage[__family].contains(__name);

    return false;
}

bool MStorage::modifyElement(qulonglong __whoAmI, QVariantList __news)
{
    VarMap *elementToModify=(VarMap *)__whoAmI;
    foreach(QString cat,m_allMap.keys())
    {
        if(!m_allMap[cat]->contains(elementToModify))
            continue;

        if(__news.length()==0)
        {//provvedo a cavarlo via
            if(!m_allMap[cat]->removeOne(elementToModify))
                qCritical()<<"Data corrupted";

            QString fm = elementToModify->value("family").toString();
            QString nm = elementToModify->value("name").toString();

            //se ho cancellato un definitore devo provvedere a rimuovere
            //i markers analitici associati
            if (fm == "Definers")
            {
                QList<QVariant> anM = elementToModify->value("anMarkers").toList();
                for (int j=0;j<anM.length();j++)
                {
                    VarMap *anMarkerToDelete= (VarMap *)anM.at(j).toULongLong();
                    (*anMarkerToDelete)["visible"] = false;

//                    foreach(QString catAn,m_allMap.keys())
//                    {
//                        if(!m_allMap[catAn]->contains(anMarkerToDelete))
//                            continue;
//                        if(!m_allMap[catAn]->removeOne(anMarkerToDelete))
//                            qCritical()<<"Data corrupted";

//                        QString fmAn = anMarkerToDelete->value("family").toString();
//                        QString nmAn = anMarkerToDelete->value("name").toString();
//                        m_storage[fmAn][nmAn]->removeOne(anMarkerToDelete);
//                        delete anMarkerToDelete;
//                    }
                }
            }

            m_storage[fm][nm]->removeOne(elementToModify);
            delete elementToModify;
        }
        else
        {//provvedo a modificarlo
            for(int i=0;i<__news.length();i+=2)
            {
                QString role=__news.at(i).toString();
                QVariant value=__news.at(i+1);
                (*elementToModify)[role]=value;
            }
        }
    }
    return true;
}

void MStorage::addCategory(QString __category)
{
    if(__category!="")
        m_allMap[__category]=new VarMapVec;
}






