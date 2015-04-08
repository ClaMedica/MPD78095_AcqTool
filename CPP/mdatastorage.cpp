#include "mdatastorage.h"
#include "qmath.h"

MDataStorage::MDataStorage(QObject *parent) :
    QObject(parent)
{
    m_all=new VarMapVec;
}

qulonglong MDataStorage::pickUp(QString __family, QString __name)
{
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            return m_storage[__family][__name];
    return 0;
}

QStringList MDataStorage::getFamilies()
{
    return m_storage.keys();
}

QStringList MDataStorage::getNames(QStringList __families, QStringList __filterType)
{
    QStringList list;
    foreach (QString family, __families)
        if(m_storage.keys().contains(family))
            foreach (QString name, m_storage[family].keys())
                foreach (QString type, __filterType) {
                    VarMapVec *curVec=(VarMapVec *)m_storage[family][name];
                    if(curVec!=NULL)//così per sfizio
                    {
                        VarMap *curMap=curVec->at(0);
                        if(curMap->value("type")==type)
                            if(!list.contains(name))
                                list<<name;
                    }
                }

    return list;
}
/**
 * @brief MDataStorage::archive a new item in the storage
 * @param __family of the item
 * @param __name of the item
 * @param __pointer of the item data
 * @return
 */
bool MDataStorage::archive(QString __family, QString __name, qulonglong __pointer, bool __whatIfAlreadyPresent)
{
    //verifichiamo se c'è già qualcosa con lo stesso nome all'interno della famiglia
    bool present=false;
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            present=true;

    VarMapVec *curVec=(VarMapVec *)__pointer;

    foreach (VarMap *curMap, (*curVec))
        if(!m_all->contains(curMap))
            m_all->append(curMap);

    //qDebug()<<*m_all;
    if(present)
    {
        if(__whatIfAlreadyPresent==OVERWRITE)
        {
            m_storage[__family][__name]=__pointer;
        }
        else
        {
            VarMapVec *oldVec=(VarMapVec *)m_storage[__family][__name];
            (*oldVec)<<(*curVec);
            m_storage[__family][__name]=(qulonglong)oldVec;
        }
    }
    else
    {
        //memorizzo nella mappa il puntatore ai dati
        m_storage[__family][__name]=__pointer;
    }

    //Riassegno al vettore corrente il puntatore al nuovo vettore
    //ora assegno un codice ad ognuno degli oggetti appena immagazzinati
    curVec=(VarMapVec *)m_storage[__family][__name];

    foreach (VarMap *curMap, (*curVec)) {
        (*curMap)["code"]=(qulonglong)curMap;
        //qDebug()<<__family<<__name<<curMap;
    }

    //in questo modo ogni elemento del CurVec ha un suo codice identificativo uguale all'indirizzo a cui punta
    return true;
}

bool MDataStorage::contains(QString __family, QString __name)
{
    if(m_storage.contains(__family))
        return m_storage[__family].contains(__name);

    return false;
}

bool MDataStorage::modifyElement(QString __family, QString __name, qulonglong __code, QVariantList __news)
{
    VarMapVec *curVec=NULL;
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            curVec=(VarMapVec *)m_storage[__family][__name];

    if(curVec==NULL)
        return error("MDataStorage::modifyElement","No element found with "+__family+" and "+__name);

    VarMap *elementToModify=NULL;
    int indexToModify=-1;

    foreach (VarMap *curMap, (*curVec)) {
        if(curMap->value("code")==__code)
        {
            if(elementToModify==NULL)
            {
                indexToModify=curVec->indexOf(curMap);
                elementToModify=curMap;
            }
            else
            {
                qDebug()<<"MDataStorage::modifyElement data corrupted!!!";
                return false;
            }
        }
    }

    if(indexToModify==-1)
        return error("MDataStorage::modifyElement","Code not found");

    //qDebug()<<"modify"<<__family<<__name<<__code<<elementToModify;
    if(__news.length()==0)
    {//provvedo a cavarlo via
        delete elementToModify;
        curVec->removeAt(indexToModify);
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
    return true;
}






