#include "mapmanager.h"

MapManager::MapManager(QObject *parent) :
    QObject(parent)
{
}

MapManager::~MapManager()
{
    QStringList list=m_map.keys();
    for(int i=0;i<m_map.size();i++)
        if(m_map[list[i]]!=NULL)
            delete m_map[list[i]];
}

void MapManager::setMapNews(QVariantList __news)
{
    if(__news.size()>0)
    {
        for(int i=0;i<__news.size();i++)
        {
            QVariantList list=__news[i].toList();
            if(list.size()==2)
                m_drawVector[list[0].toString()]=list[1].toList();
        }
        updateMaps();
    }
}

QVariantList MapManager::mapNews()
{
    return m_list;
}

QStringList MapManager::nameList()
{
    return m_map.keys();
}

void MapManager::updateMaps()
{
    QStringList plotKeys=m_drawVector.keys();//nomi dei plot
    QStringList colorKeys=m_map.keys();//nomi delle analisi

    m_list.clear();

    for(int i=0;i<m_drawVector.size();i++)
    {
        QVector<QVariant> attuale;
        for(int k=0;k<m_drawVector[plotKeys[i]].size();k++)
        {//per tutto ciò che c'è da disegnarci dentro
            for(int j=0;j<m_map.size();j++)
            {//controllo se nella lista delle cose da graficare c'è qualcosa che compete a me
                if(colorKeys[j]==m_drawVector[plotKeys[i]][k])
                {// ho trovato un analisi che conosco
                    Tabble *vec;
                    vec=m_map[colorKeys[j]];
                    int tot=vec->columns();
                    attuale.fill(QVariant(),tot);

                    for(int t=0;t<vec->columns();t++){
                        QVariantList sub;
                        for(int g=0;g<vec->rows();g++){
                            QVariant v;
                            v.setValue(vec->getValue(g,t));
                            sub.append(v);
                        }
                        attuale[t]=sub;
                    }

                    //                    for(int t=0;t<vec->columns();t++)
                    //                    {
                    //                        m_list.append("$Col");
                    //                        for(int g=0;g<vec->rows();g++)
                    //                        {
                    //                            QVariant v;
                    //                            v.setValue(vec->getValue(g,t));
                    //                            m_list.append(v);
                    //                        }
                    //                        m_list.append("&Col");
                    //                    }
                }
            }
        }

        //per ogni plot
        attuale.prepend(plotKeys[i]);
        attuale.prepend("$MapGroup");
        //siamo al plot chiamato plotKeys[i]
        attuale.append("&MapGroup");
        m_list.append(attuale.toList());

    }
    emit mapNewsChanged();
}

void MapManager::addMap(QVariantList __news)
{
//    if(__news.length()<4)
//    {
//        //ci devono essere almeno 4 elementi: nome tipo di dato, tipo di analisi e puntatore
//        qDebug()<<"MapManager::addMapVec news dimensions wrong";
//        return;
//    }
//    if(__news[1].toInt()==tMap)
//    {
//        if(__news[2].toInt()==TimeSpectrum)
//        {
//            Tabble *vec;
//            vec=(Tabble *)__news[3].toULongLong();
//            QString name=__news[0].toString();
//            name.append("_TimeSpectrum");

//            //se ho già un analisi con questo nome cancello il puntatore vecchio
//            if(m_map.contains(name))
//                delete m_map[name];

//            m_map[name]=vec;
//            emit nameListChanged();
//        }
//        updateMaps();
//    }
}
