#include "mstorage.h"
#include "qmath.h"

MStorage::MStorage(QObject *parent) :
    QObject(parent)
{
    m_all=new VarMapVec;
}

VarMapVec* MStorage::pickUp(QString __family, QString __name)
{
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            return m_storage[__family][__name];
    return 0;
}

QStringList MStorage::getFamilies()
{
    return m_storage.keys();
}

QStringList MStorage::getNames(QStringList __families, QStringList __filterType)
{
    QStringList list;
    foreach (QString family, __families)
        if(m_storage.keys().contains(family))
            foreach (QString name, m_storage[family].keys())
                foreach (QString type, __filterType) {
                    VarMapVec *curVec=m_storage[family][name];
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
 * @brief MStorage::archive a new item in the storage
 * @param __family of the item
 * @param __name of the item
 * @param __pointer of the item data
 * @return
 */
bool MStorage::archive(QString __family, QString __name, VarMapVec *__elements, bool __whatIfAlreadyPresent)
{
    //verifichiamo se c'è già qualcosa con lo stesso nome all'interno della famiglia
    bool present=false;
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            present=true;    

    foreach (VarMap *curMap, (*__elements))
        if(!m_all->contains(curMap))
            m_all->append(curMap);

    //qDebug()<<*m_all;
    if(present)
    {
        if(__whatIfAlreadyPresent==OVERWRITE)
        {
            m_storage[__family][__name]=__elements;
        }
        else
        {
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

    //ora assegno un codice ad ognuno degli oggetti appena immagazzinati

    foreach (VarMap *curMap,(*m_storage[__family][__name])) {
        (*curMap)["code"]=(qulonglong)curMap;
        //qDebug()<<__family<<__name<<curMap;
    }

    //in questo modo ogni elemento ha un suo codice identificativo uguale all'indirizzo a cui punta
    return true;
}

bool MStorage::contains(QString __family, QString __name)
{
    if(m_storage.contains(__family))
        return m_storage[__family].contains(__name);

    return false;
}

bool MStorage::modifyElement(QString __family, QString __name, qulonglong __code, QVariantList __news)
{

    VarMapVec *curVec=NULL;
    if(m_storage.keys().contains(__family))
        if(m_storage[__family].keys().contains(__name))
            curVec=m_storage[__family][__name];

    if(curVec==NULL)
    {qCritical()<<"No element found with "+__family+" and "+__name;return false;}

    VarMap *elementToModify=NULL;
    int indexToModify=-1;

    foreach (VarMap *curMap, (*curVec)) {
        if(curMap->value("code") == __code)
        {
            if(elementToModify==NULL)
            {
                indexToModify=curVec->indexOf(curMap);
                elementToModify=curMap;
            }
            else
            {
                qCritical()<<"Data corrupted!!!";return false;
            }
        }
    }

    if(indexToModify==-1)
    {qCritical()<<"Code not found";return false;}

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






