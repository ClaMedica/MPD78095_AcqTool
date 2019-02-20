#include "mdatamngdesktop.h"
#include "medicalreport.h"
#include <QErrorMessage>
#include <QDesktopServices>
#include <QMessageBox>

extern bool DebugAcqTool;

class MyException : public QException
{
public:
    void raise() const override { throw *this; }
    MyException *clone() const override { return new MyException(*this); }
};

MDataMngDesktop::MDataMngDesktop(QObject *parent)
{
    (void) parent;

    m_nomeReferto = "";
    m_winword = false;
    m_reportOpenedTimer = new QTimer(this);
    m_reportTimer = new QTimer(this);
    connect(m_reportOpenedTimer, SIGNAL(timeout()), this, SLOT(slot_checkReportOpened()));
    connect(m_reportTimer, SIGNAL(timeout()), this, SLOT(slot_startReport()));
}

void MDataMngDesktop::saveImg(QQuickItem *__item, QString __nome)
{
    QString imgName = __nome;
    //caso nomogrammi
    if (__nome.startsWith("Live"))
        if (__nome.contains("Ave"))
            imgName = "GR202";
        else
            imgName = "GR203";
    else if (__nome.startsWith("Siro"))
        if (__nome.contains("Ave"))
            imgName = "GR204";
        else
            imgName = "GR210";

    auto grabResult = __item->grabToImage();
    connect(grabResult.data(), &QQuickItemGrabResult::ready, [=]() {
        QImage img = grabResult.data()->image();
        QPixmap pix = QPixmap::fromImage(img);
        //QString imgName = imgName;
        pix.save(m_pathData + imgName + ".jpg");
    });

}

QString MDataMngDesktop::getNameOfObj(QVariantList __whoAmI)
{
    qulonglong whoAmI = __whoAmI.first().toULongLong();
    VarMap *elementToModify = (VarMap *) whoAmI;
    QString toRet = elementToModify->value("category").toString();
    return toRet.toLower();
}

void MDataMngDesktop::addOpMarker(QVariant __key,QVariant __posX)
{
    //prendo il marker operativo corrispondete al __key
    VarMap mark = m_markerMap[__key];

    VarMapVec *mrkAnVec = new VarMapVec;
    VarMap *mrk = new VarMap;

    (*mrk)["val"] = __posX;
    (*mrk)["type"] = mark.value(ATT_TYPE);
    (*mrk)["name"] = "Operative";
    (*mrk)["family"] = "Markers";
    (*mrk)["code"] = mark.value(ATT_CODE);
    (*mrk)["descr"] = mark.value(ATT_DESCR);
    (*mrk)["lock"] = false;
    (*mrk)["key"] = __key;
    (*mrk)["color"] = COLOR_OPERATIVE;
    (*mrk)["visible"] = true;
    (*mrk)["category"] = CAT_MARKER;
    mrkAnVec->append(mrk);
    QString family = "Markers";
    QString name = "Operative";
    saveDataAndUpdate(family, name, mrkAnVec,APPEND);

    updateInfoList();
    emit reloadingCompleted();
}

void MDataMngDesktop::openReport()
{
    QLibrary reportLib("MedicalReport.dll");
    if (reportLib.load())
    {
        bool refDone = false;

        Init(g_P7SettingsManager.dataPath().toLatin1(),g_P7SettingsManager.appPath().toLatin1(),m_copyFileName.toLatin1(), 4,"Standard.rtf");
        if (fillRef1())
            if (fillRef2())
                if (fillRef3())
                    refDone = true;

        if (!refDone)
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage(tr("problems in the report writing"));
            errorMessage.exec();
            return;
        }
    }

    QString namef = "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a" + ".rtf";
    m_nomeReferto = g_P7SettingsManager.dataPath() + "\\ref\\" + namef;
    QUrl urlFile = QUrl::fromLocalFile(m_nomeReferto);
    bool returnValue = QDesktopServices::openUrl(urlFile);

    if (returnValue)
    {
        //disabilitazione pulsante
        emit sg_openReport(true);
        m_reportOpenedTimer->setInterval(1000);
        m_reportOpenedTimer->start();
    }
}

HANDLE hProc;
void MDataMngDesktop::slot_checkReportOpened()
{
    QStringList output;
    bool wordFound = false;
    QString namef = "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a";

    int PIDpos = 1;
    const int MAX_WAIT_TO_OPEN_TIME = 100000;
    QTime tOut; tOut.start();
    QString tmpOutput;

    m_winword = false;

    while (!wordFound && (tOut.elapsed() < MAX_WAIT_TO_OPEN_TIME))
    {
        QProcess tasklist;
        tasklist.start(
                    "tasklist",
                    QStringList() << "/V"
                    << "/FO" << "CSV");
        tasklist.waitForFinished();
        output = QString(tasklist.readAllStandardOutput()).split("\n");
        //Si verifica se si apre il report con MICORSOFT WORD o OPENOFFICE
        for (int h= 0; h<output.length(); h++)
        {
            tmpOutput =  output.at(h);
            if (tmpOutput.toUpper().indexOf("WINWORD.EXE") > -1)
            {
                m_winword = true;
                break;
            }
        }
        //cerco la posizione del PID in tabella
        QStringList titoliTabella = output.at(0).split(",");
        PIDpos = titoliTabella.indexOf("\"PID\"");
        QStringList foundFiles = output.filter(namef);
        wordFound =  foundFiles.size() > 0;
    }

    m_reportOpenedTimer->stop();

    if (!wordFound || PIDpos == -1)
    {
        //riabilitazine eventuali pulsanti disabilitati
        emit sg_openReport(false);
        return;
    }

    QStringList proc;
    for (int i=0;i<output.length();i++){
        if (output.at(i).contains(namef)) {
            proc  = QString(output.at(i)).split(",");
           break;
        }
    }

    QString pidOutput = proc.at(PIDpos);//Posizione del PID
    QString pidString = pidOutput.remove("\"");
    int pid = pidString.toInt();
    hProc = OpenProcess(PROCESS_QUERY_INFORMATION ,FALSE,pid);

    m_reportTimer->setInterval(1000);
    m_reportTimer->start();
}

void MDataMngDesktop::slot_startReport()
{
    //Sleep(1000);
    static int counter = 0;
    if (m_winword)
    {
        // Gestione report nel caso si usi MICROSOFT WORD
        if ( !checkReportFileOpen())
            counter += 1;
        else
            counter = 0;

        if (counter > 2)
        {
            // Report file chiuso: inizializzazione controllo apertura file report
            counter = 0;
            m_reportTimer->stop();
            //riabilitazine eventuali pulsanti disabilitati
            emit sg_openReport(false);
        }
    }
    else
    {
        // Gestione report nel caso si usi programmi OPENOFFICE/LIBREOFFICE, etc...
        unsigned long exitCode = STILL_ACTIVE;
        GetExitCodeProcess(hProc,&exitCode);
        if (exitCode != STILL_ACTIVE)
        {
            m_reportTimer->stop();
            //riabilitazine eventuali pulsanti disabilitati
            emit sg_openReport(false);
        }
    }
}

// Verifica se report File e' aperto o chiuso
bool MDataMngDesktop::checkReportFileOpen()
{
    QFile reportPtrFile;

    // Si considera il REFERT_FILE (path completo del report file aperto)
    reportPtrFile.setFileName(m_nomeReferto);

    try
    {
        if (!reportPtrFile.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            return true;
        }
        else
        {
            // Chiusura del report file.
            reportPtrFile.close();
            return false;
        }
    }
    catch (MyException &e)
    {
        QMessageBox msgBox;
        msgBox.setText("MDataManager:checkReportFileOpen() ERROR !");
        msgBox.exec();
    }
}

void MDataMngDesktop::startPrint()
{
    //le immagini vanno salvate dentro un file .xml nella stessa cartella degli esami: an000001a.xml
    //e poi cancellate
    QDir dirImgs(m_pathData);
    QStringList filesList = dirImgs.entryList(QStringList("*.jpg"),QDir::Files);
    //suppongo che se in Data/ ci sono dei jpg .. questi siano immagini da mettere nel report

    QString testnumber;
    testnumber = QString("%1").arg(m_testNumber,5,10,QLatin1Char('0'));

    QString path = m_pathData;
    path.append("temp");
    path.append(testnumber);
    path.append(".xml");

    QFile* xmlFile = new QFile(path);
    if (!xmlFile->open(QIODevice::WriteOnly)) {
        return;
    }

    QXmlStreamWriter writer(xmlFile);
    writer.writeStartElement("images");

    for (int i=0;i<filesList.size();i++)
    {
        QString img = filesList.at(i);
        QFile *FI = new QFile(m_pathData + img);
        FI->open(QIODevice::ReadOnly);
        QByteArray FIByte;
        QBuffer buffer(&FIByte);
        buffer.open(QIODevice::WriteOnly);
        QDataStream out(&buffer);
        out << FI->readAll();

        QByteArray ArrayHex = FIByte.toHex();
        QString txt = img.left(img.indexOf("."));
        QString value = QString(ArrayHex);
        writer.writeTextElement(txt,value);
        FI->close();

        dirImgs.remove(img);
    }

    writer.writeEndElement();//images
    writer.writeEndDocument();

    xmlFile->close();
    delete xmlFile;

    //stampo
    if (m_autoPrint)
        openReport();
}

void MDataMngDesktop::exitFromReview()
{
    qDebug() << "Exit" << getToSave();

    //devo resettare il parametro di flusso automatico a false per non far partire sempre l'analisi in automatico all'apertura in review di un file
    Ancestry *autoflow = m_configUser.getSafeChild("AutomaticFlow");
    Ancestry *child = autoflow->getSafeChild("Auto");
    QString valueAuto = child->getAttribute("value");
    if (valueAuto == "true")
    {
        child->setAttribute("value","false");
        QString configUser = g_P7SettingsManager.userSettings();
        m_configUser.saveToXML(configUser);
    }

    //file analisi temporaneo
    //copio il file temp dell'analisi in anTESTNUM1a.xml
    QString testnumber;
    testnumber = QString("%1").arg(m_testNumber,5,10,QLatin1Char('0'));
    QString filenameAna = m_pathData;
    filenameAna.append("temp");
    filenameAna.append(testnumber);
    filenameAna.append(".xml");

    if (getToSave() == "ret") {
        qDebug()<<"cancellata copia all'exit"<<QFile::remove(m_copyFileName);

        if (QFile::exists(filenameAna))
            qDebug()<<"cancello file temp analisi"<<QFile::remove(filenameAna);

        if(DebugAcqTool == false) {
            g_mainAppBridge->sendExitReview();
            g_mainAppBridge->sendSwitch();  //send(MEX_SHOW);
        }
        else
            exit(0);

        return;
    }

    if (getToSave() == "")
        emit sg_exitFromReview();
    else
    {
        if (getToSave() == "yes")
        {
            //copio il file copy nell'originale
            if (QFile::exists(m_copyFileName))
            {
                saveChanges();
                qDebug()<<"cancello vecchio file"<<QFile::remove(m_fileName);
                qDebug()<<"copio le modifiche"<<QFile::rename(m_copyFileName,m_fileName);
            }

            if (QFile::exists(filenameAna)) {
                QString newname = filenameAna;
                newname.replace("temp","an");
                qDebug()<<"salvo file analisi"<<QFile::rename(filenameAna,newname);
            }
        }
        else //"no"
        {
            //cancello il file copy
            qDebug()<<"cancellata copia all'exit"<<QFile::remove(m_copyFileName);

            if (QFile::exists(filenameAna))
                qDebug()<<"cancello file temp analisi"<<QFile::remove(filenameAna);

        }
        if(DebugAcqTool == false) {
            g_mainAppBridge->sendExitReview();
            g_mainAppBridge->sendSwitch(); //poi dovra tornare al modulo database
        }
        else
            exit(0);
    }
}
