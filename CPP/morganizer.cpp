#include "morganizer.h"
#include "msignal.h"
MOrganizer::MOrganizer(QObject *parent) :
    QObject(parent)
{    
}

void MOrganizer::resetAll()
{

}

void MOrganizer::setInfoList(QVariantList __list)
{
    //non controllo perchè voglio che ridisegni tutto
    m_infoList=__list;
    emit infoListChanged();
}

bool MOrganizer::storeNews(QVariantList __news)
{
    if(__news.length()!=3)
    {
        /* ci devono essere esattamente tre elementi:
         * family: famiglia identificativa dell'oggetto che sto immagazzinando
         * name: nome di oggetto come segnale, marker definers o altro
         * puntatore: è il puntatore alla VarMapVec del mio oggetto
         */
        qDebug()<<"MOrganizer::storeNews news dimensions wrong"<<__news.size();
        return false;
    }

    QString family=__news[0].toString();
    QString name=__news[1].toString();
    qulonglong pointer=__news[2].toULongLong();

    VarMapVec *elements=(VarMapVec*)pointer;

    if(elements->isEmpty())
    {
        qDebug()<<"MOrganizer::storeNews no data in pointer";
        return false;
    }

    if(m_storage.archive(family,name,pointer))
    {//se siamo qua dentro vuol dire che tutto è andato liscio e possiamo visualizzare le info all'utente
        if(!m_data.keys().contains(family))
        {
            m_data[family].append(name);
            emit availableTracksChanged();
        }
        else
            if(!m_data[family].contains(name))
                m_data[family].append(name);
        updateAvailableData();
        return true;
    }
    else
        return false;
}

/**
 * @brief MOrganizer::addCustomObj add a custom object like a marker or a definer linked to __families,
 * @param __families is the list of families were to insert the new obj
 * @param __type marker definer ecc.
 * @param __info to add to the object
 * @return
 */
bool MOrganizer::addCustomObj(QStringList __families,
                              QString __name,
                              QString __type,
                              QVariantList __info)
{
    if(!m_possibleTypes.contains(__type))
    {
        qDebug()<<"MOrganizer::addCustomObj unknown type "<<__type;
        return false;
    }

    foreach (QString family, __families) {
        //devo aggiungere il mio nuovo oggetto ad ogni famiglia che ho scelto
        VarMapVec *objVec=NULL;

        if(__type==TYP_MARKER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *mrk=new VarMap;
                (*mrk)["val"]=1;
                (*mrk)["popUp"]="New Marker";
                (*mrk)["lock"]=false;
                (*mrk)["key"]=COLOR_CUSTOM;
                (*mrk)["visible"]=true;
                (*mrk)["type"]=TYP_MARKER;
                objVec->append(mrk);
            }
        }

        if(__type==TYP_DEFINER)
        {
            objVec=new VarMapVec;
            if(__info.isEmpty())
            {//link==none vuol dire che non prende info da nessuno
                VarMap *def=new VarMap;
                (*def)["xMin"]=0;
                (*def)["xMax"]=1;
                (*def)["yMin"]=0;
                (*def)["yMax"]=1;
                (*def)["popUp"]="New Definer";
                (*def)["color"]="white";
                (*def)["type"]=TYP_DEFINER;
                (*def)["resizeable"]=1;
                //(*def)["moveable"]=1;
                objVec->append(def);
            }
        }
        if(objVec!=NULL)
        {//se abbiamo scritto qualcosa lo salviamo
            if(m_data.keys().contains(family))
            {//giusto per sicurezza ma sono certo che family è contenuto
                if(!m_data[family].contains(__name))
                {//ora controllo se ho già qualcosa con questo nome e se non ce l'ho lo appendo altrimenti lascio stare
                    m_data[family].append(__name);
                    emit availableTracksChanged();
                }
            }
            else
                return false;//vuol dire che stiamo aggiungendo qualcosa ad una famiglia non contenuta

            updateAvailableData();
            if(m_storage.archive(family,__name,(qulonglong)objVec,APPEND))
                continue;
            else
                return false;
        }
        else
            return false;
    }
    return false;
}
/**
 * @brief MOrganizer::addModel adds a model to the map
 * @param __type of the model marker, definer, track ...
 * @param __roles is the list of roles of the current model
 */
void MOrganizer::addModel(QString __type, QStringList __roles)
{
    m_possibleTypes<<__type;
    m_modelMap[__type]=__roles;
}

void MOrganizer::save(void)
{
    //è ora di salvare un po' di roba ma nulla di più facile basta mandare il data storage alla datafile manager
    //o meglio avvisare il datafile manager che il datastorage è cambiato
    emit saveStoreChanges();
}

QVariantList MOrganizer::getData(QString __type)
{
    ModelManager mng;
    mng.setType(__type);
    mng.setRoles(m_modelMap[__type]);
    mng.setStore(&m_storage);
    mng.setInfoList(m_infoList);
    return mng.drawList();
}

QVariantList MOrganizer::getPlotLimits()
{
    QMap<QString, QStringList> infoMap,plotMap;

    for(int plotIndex=0;plotIndex<m_infoList.size();plotIndex++)
    {//dalle info che ci arrivano da dialog creiamo una mappa dove ad ogni plot assegniamo
        //quello che ci va disegnato
        QVariantList plotInfo=m_infoList.at(plotIndex).toList();
        QString plotName=plotInfo.at(0).toString();
        QStringList info=plotInfo.at(1).toStringList();
        infoMap[plotName]=info;
    }

    foreach (QString plotName, infoMap.keys()) {
        foreach (QString data, infoMap[plotName]) {
            if(data.contains("Signal"))//cerco segnali originali
                plotMap[plotName]<<data.split(":").at(0);
        }
    }

    //ora abbiamo dentro plotMap l'elenco dei nomi dei segnali segnali che ci servono

    QVariantList limits;

    foreach (QString plotName, plotMap.keys()) {
        QStringList families=plotMap[plotName];
        limits<<"$Limit";
        limits<<plotName;
        if(!families.isEmpty())
        {
            float xMin=INF,xMax=-INF,yMin=INF,yMax=-INF;
            foreach (QString family, families) {
                VarMapVec *cur=(VarMapVec *)m_storage.pickUp(family,"Signal");

                foreach(VarMap *map,*cur)
                {
                    qulonglong p=(*map)["pointer"].toULongLong();
                    MSignal *sig=(MSignal*)p;
                    if(sig->getT0()<xMin)xMin=sig->getT0();
                    if(sig->getDuration()>xMax)xMax=sig->getDuration();
                    if(sig->minimum()<yMin)yMin=sig->minimum();
                    if(sig->maximum()>yMax)yMax=sig->maximum();
                }
            }
            limits<<xMin<<xMax<<yMin<<(yMax+abs(yMax*0.05));
        }
        limits<<"&Limit";
    }
    qDebug()<<limits;
    return limits;
}

bool MOrganizer::changeObject(QVariantList __curObj)
{
    if(__curObj.contains("code"))
    {
        int codeIndex=__curObj.indexOf("code")+1;

        QVariantList fullCode=__curObj.at(codeIndex).toList();
        QString family=fullCode.at(0).toString();
        QString name=fullCode.at(1).toString();
        qulonglong code=fullCode.at(2).toULongLong();
        //tolgo il fullcode e lascio solo le proprietà
        __curObj.removeFirst();
        __curObj.removeFirst();
        //qDebug()<<"Richiesta di modifica per "<<family<<name<<code;
        m_storage.modifyElement(family,name,code,__curObj);
    }
    return false;
}

QVariant MOrganizer::getSignal(QString __name)
{
    VarMapVec *v=(VarMapVec*) m_storage.pickUp(__name,"Signal");
    return v->at(0)->value("pointer");
}
/**
 * @brief MOrganizer::getLinks
 * @param __what
 * @param __filter
 * @return
 */
QStringList MOrganizer::getLinks(QString __what, QStringList __filterFamily, QStringList __filterType)
{
    //i filters servono quando devo ottenere i nomi date le famiglie
    QStringList list;
    if(__what=="Families")
        list<<m_storage.getFamilies();
    if(__what=="Names")
        list<<m_storage.getNames(__filterFamily,__filterType);

    return list;
}



void MOrganizer::updateAvailableData()
{
    m_availableData.clear();
    QStringList signalNames=m_data.keys();
    for(int i=0;i<signalNames.size();i++)
    {
        m_availableData<<"$Group";
        //m_availableData<<signalNames[i];
        m_availableData<<m_data[signalNames[i]];
        m_availableData<<"&Group";
    }

    emit availableDataChanged();
}
