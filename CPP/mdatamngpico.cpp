#include "mdatamngpico.h"

MDataMngPico::MDataMngPico(QObject *parent)
{
    (void) parent;

    m_mngPrint = NULL;
    spoolerQueueLen = 0;

   connect(&udpConn, SIGNAL(receivedUdp(enum WHO, QByteArray)), this, SLOT(udpMdmBtDecode(WHO,QByteArray)));
}

MDataMngPico::~MDataMngPico()
{
    if (m_mngPrint != NULL) {
        delete m_mngPrint;
        m_mngPrint = NULL;
    }
}

void MDataMngPico::loadFile(QString __fileName)
{
    MDataManager::loadFile(__fileName);
    Ancestry *autoloop = m_configUser.getSafeChild("AutomaticFlow");
    m_autoLoop = (autoloop->getSafeChild("Loop")->getSafeAttribute(ATT_VALUE) == "true" ? true : false);

    initPrinter();
}

void MDataMngPico::initPrinter()
{
    m_mngPrint = new printermanager(m_copyFileName);
    m_mngPrint->setPrintSiroky(m_Siroky);
    m_mngPrint->setPrintLiverpool(m_Liverpool);
    if (m_etaPatient <=18 && m_etaPatient >= 3 && m_peso > 0 && m_altezza > 0)
        m_mngPrint->setPrintMiskolc(m_Miskolc);
    else
        m_mngPrint->setPrintMiskolc(false);
    m_mngPrint->setPrintModeUser(m_landscape);
    m_mngPrint->setPrintHeaders(m_firstHead,m_secondHead);
    m_mngPrint->setRangeChQ(m_rangeChQ);
    m_mngPrint->setRangeChVV(m_rangeChVV);
    m_mngPrint->setRangeChEMG(m_rangeChEMG);
}

void MDataMngPico::sendToPrint()
{
    qDebug()<<"Check OTHER Load" << "sendToPrint() m_datiCalib:" << m_datiCalib;

    m_mngPrint->print(m_datiCalib);
    qDebug() << "stampato";
    if (m_autoLoop)
    {
        //modalita'flusso loop automatico
        //dopo la stampa deve tornare a medica e far ripartire un'altra flussimetria utomatica
        exitFromReview();
    }
}

void MDataMngPico::getGrabbedImage(QObject *gi, QString __nome)
{
    qDebug()<<"Immagine"<<__nome<<gi;
    if (__nome != "grafo")
        m_mngPrint->getGrabbedImage(gi, __nome);
}

void MDataMngPico::send_Command(int __command)  // replicato da macqmanager perche' non lo si puo' invocare
{
    QByteArray msg = (__command == 4) ? "suspBt" : "restartBt";

    udpConn.sendSup(msg);
}

void MDataMngPico::startPrint()
{
    //qml
    qDebug() << "inizializzo printer";

    m_mngPrint->setTempoAttesa((float)(qRound(m_aflwdatas.at(0)->getWaitingTime()*10))/10);
    m_mngPrint->setFlussoMax((float)(qRound(m_aflwdatas.at(0)->getQMax()*10))/10);
    m_mngPrint->setFlussoMedio((float)(qRound(m_aflwdatas.at(0)->getQAve()*10))/10);
    m_mngPrint->setTempoMax((float)(qRound(m_aflwdatas.at(0)->getTimeAtQmax()*10))/10);
    m_mngPrint->setTempo595((float)(qRound(m_aflwdatas.at(0)->getTime90()*10))/10);
    m_mngPrint->setTempoFlusso((float)(qRound(m_aflwdatas.at(0)->getFlowTime()*10))/10);
    m_mngPrint->setTempoDisc((float)(qRound(m_aflwdatas.at(0)->getDescTime()*10))/10);
    m_mngPrint->setTempoSvuot((float)(qRound(m_aflwdatas.at(0)->getVoidingTime()*10))/10);
    m_mngPrint->setVolFlussoMax((float)(qRound(m_aflwdatas.at(0)->getVolAtQqmax()*10))/10);
    m_mngPrint->setVolVuotato((float)(m_aflwdatas.at(0)->getVoidedVolume()*10)/10);
    m_mngPrint->setAccelerazione((float)(qRound(m_aflwdatas.at(0)->getAcceleration()*10))/10);
    m_mngPrint->setFlussoCor((float)(qRound(m_aflwdatas.at(0)->getCQ()*10))/10);
    m_mngPrint->setVolRes((float)(m_aflwdatas.at(0)->getResidualVolume()*10)/10);
    m_mngPrint->setDetContrMax((float)(qRound(m_aflwdatas.at(0)->getVDetMax()*10))/10);

    QString vers = g_P7SettingsManager.getVersione();
    m_mngPrint->setVersione(vers);

    //mi dice se la flussimetria automatica o manuale
    m_mngPrint->setMode(m_autoFlow);
    //stampo

    if (m_autoPrint) {
        sendToPrint();
        m_autoPrint = false; //non deve ristampare se l'utente riapre subito l'esame
    }
}


void MDataMngPico::sendPrintTest()
{
//    udpConn.sendSup("Print:/root/PicoFlow/urodata/UDSData/printTest.prn");
}

void MDataMngPico::udpMdmBtDecode(enum WHO __from, QByteArray __msg)
{
 //  qDebug() << "udpMdmBtDecode" << __from << __msg;

    char cmd = __msg.at(0);
    switch(__from) {
    case E_SUP:
//                if((cmd == 'S') && (__msg == "Suspended")) emit udpMdmBtStatus(__from, cmd);
//                if((cmd == 'R') && (__msg == "Restarted")) emit udpMdmBtStatus(__from, cmd);
//                if((cmd == 'U') && (__msg == "UseBt"))     emit udpMdmBtStatus(__from, cmd);
//                if((cmd == 'N') && (__msg == "NoBt"))      emit udpMdmBtStatus(__from, cmd);
                if((cmd == 'q') && __msg.startsWith("queue:")) spoolerQueueLen = __msg.remove(0,6).toInt();
                break;
    case E_PRN:
//                if((cmd == 'R') && (__msg == "Ready"))     emit udpMdmPrnStatus(__from, cmd);
//                if((cmd == 'F') && (__msg == "Fail"))      emit udpMdmPrnStatus(__from, cmd);
//                if((cmd == 'D') && (__msg == "Done"))      emit udpMdmPrnStatus(__from, cmd);
                break;
    case E_MED:
                break;
    default:
        break;
    }
}

void MDataMngPico::exitFromReview()
{
    qDebug() << "Exit" << getToSave();

    if (!m_autoLoop) //se non sono in un loop
    {
        //devo resettare il parametro di flusso automatico a false per non far partire sempre l'analisi in automatico all'apertura in review di un file
        Ancestry *autoflow = m_configUser.getSafeChild("AutomaticFlow");
        Ancestry *child = autoflow->getSafeChild("Auto");
        QString valueAuto = child->getAttribute("value");
        if (valueAuto == "true")
        {
            child->setAttribute("value","false");
            QString configUser = g_P7SettingsManager.userSettings();
            m_configUser.saveToXML(configUser);
            system("sync");
        }
    }

    if (getToSave() == "ret")  //non ci sono state modifiche
    {
        qDebug()<<"cancellata copia all'exit"<<QFile::remove(m_copyFileName);
    }
    else //devo salvare
    {
        //copio il file copy nell'originale
        if (QFile::exists(m_copyFileName))
        {
            saveChanges();
            qDebug()<<"cancello vecchio file"<<QFile::remove(m_fileName);
            qDebug()<<"copio le modifiche"<<QFile::rename(m_copyFileName,m_fileName);
        }
    }

    resetAll();
    emit sg_clearResultForm();
   // qDebug()<<"AUTOLOOP"<<m_autoLoop;
    send_Command(5);   // STARTBT
    if (m_autoLoop)
        g_mainAppBridge->sendLoop();
    else
        g_mainAppBridge->sendExitReview();
    g_mainAppBridge->sendSwitch(); //poi dovra tornare al modulo database
}
