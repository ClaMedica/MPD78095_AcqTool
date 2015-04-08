#include "doppleranalysis.h"
#include "maudio.h"
#include "msignal.h"
#include "tabble.h"
#include "codebook.h"
#include "datafilemanager.h"

DopplerAnalysis::DopplerAnalysis()

{
    //qui dentro ci sono tutti ma proprio tutti i parametri delle analisi
    m_currentSignalName="custom_signal";
    m_pCurrentSignal=NULL;
    m_start=0;
    m_end=3600;//fine esame di default a 1 ora
}

DopplerAnalysis::~DopplerAnalysis()
{
    //    for(int i=0;i<m_signalVector.size();i++)
    //        if(m_signalVector[i]!=NULL)
    //            delete m_signalVector[i];
}

int DopplerAnalysis::anaType()
{
    return m_anaType;
}

void DopplerAnalysis::setAnaType(int __a)
{
    if(__a!=m_anaType)
    {
        m_anaType=(AnalysisType)__a;
        emit anaTypeChanged();
    }
}

QVariantList DopplerAnalysis::analysisNews()
{
    return m_morphPointer;
}



QVariant DopplerAnalysis::currentSignal()
{
    return m_currentSignalName;
}

bool DopplerAnalysis::addSignal(MSignal *__pSignal)
{

    if(__pSignal==NULL)
    {
        qDebug() << "The signal is empty in FunctionHandler::addSignal";
        return false;
    }

    for(int i=0;i<m_signalVector.size();i++)
        if(m_signalVector[i]->getName()==__pSignal->getName())
        {
            qDebug() << "File with the same name already exist in the vector";
            return false;
        }
    m_signalVector.append(__pSignal);
    VarMapVec *vec=new VarMapVec;
    VarMap *traccia=new VarMap;
    QString tag=TYP_SIGNAL;
    (*traccia)["popUp"]=__pSignal->getName();
    (*traccia)["color"]="red";
    (*traccia)["family"]=__pSignal->getName();
    (*traccia)["tag"]=tag;
    (*traccia)["pointer"]=(qulonglong)__pSignal;
    vec->append(traccia);
    sendNews(__pSignal->getName(),tag,(qulonglong)vec);
    emit availableTracksChanged();
    return true;

}

bool DopplerAnalysis::addSignals(qulonglong __pSignal)
{
    QVector<MSignal*> *vec=(QVector<MSignal*> *)__pSignal;
    for(int i=0;i<vec->size();i++)
    {
        this->addSignal((*vec)[i]);
    }
    return true;
}

void DopplerAnalysis::sendNews(QString __name, QString __tag, qulonglong __element)
{

    if(!m_data.keys().contains(__name))
    {
        m_data[__name].append(__tag);
        availableTracksChanged();
    }
    else
        if(!m_data[__name].contains(__tag))
            m_data[__name].append(__tag);

    m_morphPointer.clear();
    m_morphPointer.append(__name);
    m_morphPointer.append(__tag);
    m_morphPointer.append(__element);

    updateAvailableData();

    emit analysisNewsChanged();

}

void DopplerAnalysis::setCurrentSignal(QVariant __sig)
{
    MSignal *s=(MSignal *)__sig.toULongLong();
    if(s!=m_pCurrentSignal)
    {
        m_pCurrentSignal=s;
        m_currentSignalName=s->getName();
        emit currentSignalChanged();
    }
}

void DopplerAnalysis::analyze(QString __anaType, QVariant __parameters)
{
    if(__anaType=="Energy")
        calcEnergy(__parameters);
    if(__anaType=="Time Warping")
        calcTwi(__parameters);
    //        break;
    //    case Spectrum:
    //        if(m_pCurrentSignal!=NULL)
    //        {
    //            MSignal *fftResult=new MSignal;
    //            fft(m_pCurrentSignal,fftResult,m_parForAna[PAR_NPOINT]);
    //            QString str=ANA_SPECTRUM;str.append("-");
    //            sendNews(m_currentSignalName,str+TYP_SIGNAL,(qulonglong)fftResult);
    //        }
    //        break;
    //    case TimeSpectrum:
    //        if(m_pCurrentSignal!=NULL)
    //        {
    //            int col=m_pCurrentSignal->getDuration()/m_parForAna[PAR_WINDIM];
    //            int leng=m_pCurrentSignal->getSize()/col;
    //            Tabble *tab=new Tabble(m_parForAna[PAR_NPOINT]+1,col);
    //            int index=0;
    //            for(int i=0;i+leng<m_pCurrentSignal->getSize();i+=leng)
    //            {
    //                MSignal sig,res;
    //                if(m_pCurrentSignal->getSection(&sig,i,leng))
    //                {
    //                    fft(&sig,&res,m_parForAna[PAR_NPOINT]);
    //                    res.normalize(0,100);
    //                    for(int j=0;j<res.getSize();j++)
    //                        tab->setValue(j,index,res.getValue(j));
    //                    index++;
    //                }
    //            }
    //            QString str=ANA_SPETIME;str.append("-");
    //            sendNews(m_currentSignalName,str+TYP_MAP,(qulonglong)tab);
    //        }
    //        break;
    //    default:break;
    //    }
}

void DopplerAnalysis::calcEnergy(QVariant __paramenters)
{
    if(m_pCurrentSignal!=NULL)
    {
        VarMapVec *ener=new VarMapVec;
        VarMap *parameters=(VarMap *)__paramenters.toULongLong();
        MSignal *sig=new MSignal;

        energyFramesFinder(m_pCurrentSignal,ener,sig,(*parameters)[PAR_THPERC].toFloat(),(*parameters)[PAR_WINDIM].toFloat());
        //qDebug()<<"analizzo"<<ener<<sig;
        QString str=ANA_ENERGY;str.append("-");
        sendNews(m_currentSignalName,str+TYP_DEFINER,(qulonglong)ener);
        qDebug()<<"Energy frames founded"<<ener->size();
        VarMapVec *vec=new VarMapVec;
        VarMap *traccia=new VarMap;
        QString tag=str+TYP_SIGNAL;
        (*traccia)["popUp"]=sig->getName();
        (*traccia)["color"]="red";
        (*traccia)["family"]=m_currentSignalName;
        (*traccia)["tag"]=tag;
        (*traccia)["pointer"]=(qulonglong)sig;
        vec->append(traccia);
        sendNews(m_currentSignalName,tag,(qulonglong)vec);
    }
}

void DopplerAnalysis::calcTwi(QVariant __paramenters)
{
    if(m_pCurrentSignal!=NULL)
    {
        //VarMapVec *ener=new VarMapVec;
        VarMap *parameters=(VarMap *)__paramenters.toULongLong();
        VarMapVec *result=new VarMapVec;
        MSignal *sig=new MSignal;
        MSignal *sec=new MSignal;
        m_pCurrentSignal->getSection(sec,(float)0.66,(float)0.12);
        TWI(m_pCurrentSignal,sec,5,0.01,result);
        QString str=ANA_TWI;str.append("-");
        sendNews(m_currentSignalName,str+TYP_DEFINER,(qulonglong)result);
    }
}

void DopplerAnalysis::handleMenu(QString __s)
{
    if(__s=="Spectral")
    {
        setAnaType(Spectrum);
    }

    if(__s=="Energy")
    {
        setAnaType(Energy);
    }
    if(__s=="prova")
    {


    }

    if(__s=="Time Spectrum")
    {
        setAnaType(TimeSpectrum);
    }

    if(__s=="Plot Settings")
    {
        setAnaType(PlotSettings);
    }
}

QVariantList DopplerAnalysis::getPlotSettings()
{
    QString funMex="DopplerAnalysis::getPlotSettings:";
    if(m_pCurrentSignal==NULL)
    {
        QVariantList v;
        v<<0<<10<<-1<<1;
        qDebug() << funMex+"default";
        return v;
    }
    QVariantList v;
    v<<0<<m_pCurrentSignal->getDuration()<<m_pCurrentSignal->minimum()<<m_pCurrentSignal->maximum();
    return v;
}

void DopplerAnalysis::resetAll()
{
    m_data.clear();
    m_availableData.clear();
    for(int i=0;i<m_signalVector.size();i++)
        delete m_signalVector[i];
    m_signalVector.clear();
    emit availableTracksChanged();
    emit availableDataChanged();
}

void DopplerAnalysis::updateAvailableData()
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




