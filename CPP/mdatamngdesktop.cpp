#include "mdatamngdesktop.h"
#include "medicalreport.h"
#include <QDesktopServices>
#include <QClipboard>
#include <QApplication>
#include <QPrinter>
#include <QTextDocument>
#include <QProcess>
#include <QDateTime>
#include <QPixmap>


#ifdef WIN32
#include <QAxObject>
#include <windows.h>
#endif

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
    m_nomeRefertoPdf = "";
    m_tipoReferto = REFHTM;
    m_template = "Standard";

    m_reportOpenedTimer = new QTimer(this);
    m_reportTimer = new QTimer(this);
    connect(m_reportOpenedTimer, SIGNAL(timeout()), this, SLOT(slot_checkReportOpened()));
    connect(m_reportTimer, SIGNAL(timeout()), this, SLOT(slot_startReport()));
}

void MDataMngDesktop::loadFile(QString __fileName)
{
    MDataManager::loadFile(__fileName);
    m_reportEdit = m_configPrinter.getSafeChild("Report");
    QStringList editWith = m_reportEdit->getSafeChild("WITH")->getSafeAttribute(ATT_VALUE).split("#");
    for (int i=0;i<editWith.length();i++) {
        if (editWith[i] == "true")
            m_tipoReferto = i;
    }
}

void MDataMngDesktop::saveImg(QQuickItem *__item, QString __nome)
{
    QString imgName = __nome;
    //caso nomogrammi
    if (__nome.startsWith("Live"))
        if (__nome.contains("Ave"))
            imgName = "GR203";
        else
            imgName = "GR202";
    else if (__nome.startsWith("Siro"))
        if (__nome.contains("Ave"))
            imgName = "GR204";
        else
            imgName = "GR205";
    else if (__nome.startsWith("Pediat"))
            imgName = "GR210";

    auto grabResult = __item->grabToImage();
    connect(grabResult.data(), &QQuickItemGrabResult::ready, [=]() {
        QImage img = grabResult.data()->image();
        QPixmap pix = QPixmap::fromImage(img);
        pix.save(m_pathData + imgName + ".jpg");
    });
}

void MDataMngDesktop::copyImg(QQuickItem *__item)
{
    auto grabResult = __item->grabToImage();
    connect(grabResult.data(), &QQuickItemGrabResult::ready, [=]() {
        QImage image = grabResult.data()->image();
        QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
    });

}

QString MDataMngDesktop::getNameOfObj(QVariantList __whoAmI)
{
    qulonglong whoAmI = __whoAmI.first().toULongLong();
    VarMap *elementToModify = (VarMap *) whoAmI;
    QString toRet = elementToModify->value("category").toString();
    QString trad = translate(toRet);
    return trad;
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
    setToSave("");
    saveDataAndUpdate(family, name, mrkAnVec,APPEND);

    updateInfoList();
    emit reloadingCompleted();
}

void MDataMngDesktop::addAnMarker(QVariant __key, QVariant __posX, QVariantList __chName)
{
    QString warning = "";
    QString chName = "";
    for (int i=0;i<__chName.length();i++) {
        QString chTemp = __chName.at(i).toString();
        if (chTemp.startsWith("Q"))
            chName = __chName.at(i).toString();
    }

    if (chName != "")
    {
        int posX = __posX.toInt();
        VarMapVec* elements = m_storage.getAll(CAT_DEFINER);
        bool defFound = false;
        VarMap *defMap = NULL;
        foreach (VarMap *curMap, (*elements)) {
            int evStart = curMap->value("xMin").toInt();
            int evEnd = curMap->value("xMax").toInt();

            if (evStart <= posX  &&  posX <= evEnd) {
                defFound = true;
                defMap = curMap;
                break;
            }
        }
        if (defFound)
        {
            //definitore trovato curMap, inserisco il marker analitico
            foreach(MSignal *sig, m_signalVector) {
                for (int i=0;i<__chName.length();i++)
                {
                    QString chName =  __chName[i].toString();
                    if (chName.startsWith("Q") && (sig->getName() == chName))
                    {
                        QString key = __key.toString();

                        QVariantList valuesY;
                        for (int i = 0; i < sig->size(); i++)
                            valuesY.append(sig->at(i));

                        VarMap *mrk = new VarMap;
                        VarMapVec *mrkAnVec = new VarMapVec;

                        (*mrk)["val"] = __posX;
                        (*mrk)["type"] = TYPE_ANALYTICAL;
                        (*mrk)["name"] = "Analitical";
                        (*mrk)["family"] = "Markers";
                        (*mrk)["nas"] = sig->getSamplingFrequency();
                        (*mrk)["valuesY"] = valuesY;
                        (*mrk)["graph"] = sig->getGraph();
                        (*mrk)["code"] = key;
                        (*mrk)["descr"] = defMap->value("descr");
                        (*mrk)["lock"] = false;
                        (*mrk)["channel"] = sig->getCh();
                        (*mrk)["defCode"] = (qulonglong)defMap->value("whoAmI").toULongLong();
                        (*mrk)["key"] = key.right(1).toInt();
                        (*mrk)["color"] = COLOR_ANALYTICAL;
                        (*mrk)["visible"] = true;
                        (*mrk)["category"] = CAT_MARKER;
                        (*mrk)["limDefMax"] = defMap->value("xMax").toDouble();
                        (*mrk)["limDefMin"] = defMap->value("xMin").toDouble();
                        mrkAnVec->append(mrk);
                        //lo associo al suo definitore
                        QList<QVariant> anM = defMap->value("anMarkers").toList();
                        anM.append((qulonglong)mrk);
                        (*defMap)["anMarkers"] = anM;

                        if(!mrkAnVec->isEmpty()) {
                            QString family = "Markers";
                            QString name = "Analitical";
                            setToSave("");
                            saveDataAndUpdate(family, name, mrkAnVec,APPEND);
                        }

                        updateInfoList();
                        emit reloadingCompleted();
                    }
                }
            }
        }
        else
        {
           emit sg_warning(m_messaggiUtente[AVV_ANMNOINSERT].at(0));//anmarker fuori da un definitore
        }
    }
    else
    {
        emit sg_warning(m_messaggiUtente[AVV_ANMOUTCH].at(0)); //anmarker in un un canale errato
    }
}

void MDataMngDesktop::addDefiner(QVariant __key, QVector<double> __pos)
{
    //prendo il definer corrispondete al __key
    VarMap defKey = m_markerMap[__key];
    VarMapVec* elements = m_storage.getAll(CAT_DEFINER);
    QVariantList En;
    int posQ = -1, posV = -1;
    int i = 0;
    foreach(MSignal *sig, m_signalVector) {
        QString name = sig->getName();
        if (name.startsWith("Q"))
            posQ = i;
        if (name.startsWith("VV") || name ==  "VLMv")
            posV = i;
        i++;
    }

    for (int i = 0; i < m_numChannels; i++)
        En << 0;

    En[posQ] = 1;
    if (posV > -1)
        En[posV] = 1;

    QString family = "Definers";
    QString name = defKey.value(ATT_DESCR).toString();
    VarMap *def = new VarMap;
    (*def)["key"] = __key;
    (*def)["family"] = family;
    (*def)["name"] = name;
    (*def)["xMin"] = __pos[0];
    (*def)["xMax"] = __pos[2];
    (*def)["yMin"] = __pos[1];
    (*def)["yMax"] = __pos[3];
    (*def)["num"] = elements->length();
    (*def)["enCh"] = En;
    (*def)["descr"] = name;
    (*def)["color"] = COLOR_DEFINER;
    (*def)["category"] = CAT_DEFINER;
    (*def)["resizeable"] = 1;

    VarMapVec *defVec = new VarMapVec;
    defVec->append(def);
    setToSave("");
    saveDataAndUpdate(family, name, defVec,APPEND);

    updateInfoList();
    emit reloadingCompleted();
}

void MDataMngDesktop::deleteAnMArkers()
{
    //-Cancellazione tutti AnMarkers
    VarMapVec *elements = m_storage.getAll(CAT_MARKER);

    foreach (VarMap *curMap, (*elements)) {
        if((curMap->value("color").toString() == COLOR_ANALYTICAL) && (curMap->value("visible").toBool() == true))
        {
            QVariantList mrk;
            mrk.append((qulonglong)curMap);
            changeObject(mrk);

        }
    }
    qDebug() << "Fine cancello tutti gli anmarker";

}

void MDataMngDesktop::setReportTemplate(QString __template)
{
    m_template = __template;
}

void MDataMngDesktop::openReport(QString __codSoft)
{
    createNamePDf();

    QString tipoFile = ".htm";
    if (m_tipoReferto == REFRTF)
        tipoFile = ".rtf";

    QString fileReferto = m_template;
    if (fileReferto == "")
        fileReferto = "Standard"+tipoFile;
    else
        fileReferto += tipoFile;

    if (!QDir().exists(QDir::toNativeSeparators(m_pathRef + "/tmp")))
        QDir(m_pathRef).mkdir("tmp");

#ifdef STATICO
    QString nomeR = fileReferto;
    MedicalReport m;
    int res = m.CreaMedicalReport(g_P7SettingsManager.dataPath().toLatin1(),g_P7SettingsManager.appPath().toLatin1(),m_copyFileName.toLatin1(), __codSoft.toInt(), nomeR.toLatin1(),g_P7SettingsManager.localization());
    if (res != 0)
    {
        qDebug() << "ERROR Medical report"<< res;
        emit sg_warnReport(tr("Problems in the report writing"));
        return;
    }
#else
#ifdef WIN32
    QLibrary reportLib("MedicalReport.dll");
#endif
#ifdef MAC
    QLibrary reportLib("libMedicalReport.1.8.0");
#endif
    if (reportLib.load())
    {
        bool refDone = false;
        QString nomeR = fileReferto;
        Init(g_P7SettingsManager.dataPath().toLatin1(),g_P7SettingsManager.appPath().toLatin1(),m_copyFileName.toLatin1(), __codSoft.toInt(), nomeR.toLatin1(),g_P7SettingsManager.localization());
        if (fillRef1())
            if (fillRef2())
                if (fillRef3())
                    refDone = true;
        if (!refDone)
        {
            emit sg_warnReport(tr("Problems in the report writing"));
            return;
        }
    }
#endif

    m_nomeReferto = QDir::toNativeSeparators(m_pathRef  + "/tmp/" +  "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a" + tipoFile);
    QString toEdit = m_reportEdit->getSafeChild("HTM")->getSafeAttribute(ATT_VALUE);

    if (toEdit == "false" || m_autoPrint) {
        if (m_tipoReferto == REFHTM)
        {
            //necessario trasformare il file referto htm in htm tradotto per leggere tutti i caratteri
            QFile f(m_nomeReferto);
            f.open(QIODevice::ReadOnly);
            QByteArray data = f.readAll();
            QTextCodec *codec = QTextCodec::codecForName(m_language);
            QString stringa = codec->codecForMib(106)->toUnicode(data);
            QTextDocument textDoc;
            textDoc.setHtml(stringa);
            f.close();

            QFile f1(QDir::toNativeSeparators(m_pathRef + "/tmp/prova" + tipoFile));
            f1.open(QIODevice::WriteOnly);
            f1.write(textDoc.toHtml("utf-8").toUtf8());
            f.remove();
            f1.close();
            f1.copy(m_nomeReferto);
            f1.remove();
        }
        createPdf();
    }
    else {
        if (m_tipoReferto == REFHTM)
        {
            // APERTURA FILE NS EDITOR
            //Nome analisi
            QString tempFile, resultFile, nomeAnalisi;
            QDir ResPath = QDir(m_pathData);
            QStringList filesList = ResPath.entryList(QStringList("*.xml"),QDir::Files);
            for (int i=0; i<filesList.size();i++)
            {
                QString file = filesList.at(i);
                if (file.contains(QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + ".xml"))
                {
                    tempFile = file;
                    break;
                }
                if (file.contains(QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a.xml"))
                    resultFile = file;
            }

            //if refert button is enabled MUST be a temp or result file of analysis
            nomeAnalisi = QDir::toNativeSeparators(m_pathData + "/");
            if (tempFile.size() != 0)
                nomeAnalisi += tempFile;
            else
                nomeAnalisi += resultFile;

            QStringList arg;
            arg << "file:///" +  m_nomeReferto << "r" << g_P7SettingsManager.localization() << nomeAnalisi;
            QString pth = QDir::toNativeSeparators(g_P7SettingsManager.progPath()+"/texteditor");
#ifndef MAC
            pth += ".exe";
#endif
            QProcess *editor = new QProcess();
            editor->setProgram(pth);
            editor->setArguments(arg);

            connect(editor,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus)
            {
                //riabilitazione eventuali pulsanti disabilitati
                emit sg_openReport(false);

                //creazione PDF
                createPdf();
                delete editor;
            });

            connect(editor, &QProcess::started,[=]()
            {
                //disabilitazione pulsante
                emit sg_openReport(true);
            });

            editor->start();
        }
        else //REFRTF
        {
            //APERUTRA FILE CON WORD (WIN) LIBREOFFICE (MAC)
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
    }
}
#ifdef WIN32
HANDLE hProc;
#endif

void MDataMngDesktop::slot_checkReportOpened()
{
#ifdef MAC
    QProcess lsof;
    bool found = false;
    QString output;
    const int MAX_WAIT_TO_OPEN_TIME = 15000;
    QElapsedTimer tOut; tOut.start();
    while (!found && tOut.elapsed() < MAX_WAIT_TO_OPEN_TIME) {
        lsof.start("lsof", QStringList() << m_nomeReferto);
        lsof.waitForFinished();
        output = QString(lsof.readAllStandardOutput());
        QStringList outputList = output.split("\n");
        qDebug() << "Processi con file referto " << outputList;
        if (outputList.length() > 1) {
            for (int i=1;i<outputList.length();i++){
                if (!outputList.at(i).contains("mdworker_") && !outputList.at(i).contains("finder") && outputList.at(i).length() > 5) {
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found) {
        //riabilitazine eventuali pulsanti disabilitati
        qDebug()<<"Referto non aperto";
        emit sg_openReport(false);
        return;
    }

    m_reportOpenedTimer->stop();
#endif
#ifdef WIN32
    QString namef = "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a";
    bool wordFound = false;
    QStringList output;
    int PIDpos = 1;
    const int MAX_WAIT_TO_OPEN_TIME = 15000;
    QElapsedTimer tOut; tOut.start();
    QString tmpOutput;

    m_winword = false;

    while (!wordFound && (tOut.elapsed() < MAX_WAIT_TO_OPEN_TIME))
    {
        qDebug() << tOut.elapsed();
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
#endif
    m_reportTimer->setInterval(1000);
    m_reportTimer->start();
}

void MDataMngDesktop::slot_startReport()
{
#ifdef MAC
    QProcess lsof;
    bool found = false;

    lsof.start("lsof", QStringList() << m_nomeReferto);
    lsof.waitForFinished();
    QString output = QString(lsof.readAllStandardOutput());
    QStringList outputList = output.split("\n");
    qDebug() << "Processi con file referto alla chiusura " << outputList;
    for (int i=1;i<outputList.length();i++){
        if (!outputList.at(i).contains("finder") && outputList.at(i).length() > 5) {
            found = true;
            break;
        }
    }

    qDebug() << found;
    if (!found) {
        qDebug()<<"Referto chiuso";
        m_reportTimer->stop();
        //riabilitazine eventuali pulsanti disabilitati
        emit sg_openReport(false);
    }

#endif
#ifdef WIN32
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
#endif
}

#ifdef WIN32
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
        emit sg_warnReport("MDataManager:checkReportFileOpen() ERROR !");
    }

    return false;
}
#endif


void MDataMngDesktop::createPdf()
{
    if (m_tipoReferto == REFHTM)
    {
        QFile FI(m_nomeReferto);
        FI.open(QIODevice::ReadOnly);
        QByteArray FIByte;
        QBuffer buffer(&FIByte);
        buffer.open(QIODevice::WriteOnly);
        QDataStream out(&buffer);
        out << FI.readAll();
        //rimuoviamo eventuali scritte per visualizzare all'utente nell'editor il page break
        FIByte.replace("Page Break"," ");

        QString string = QTextCodec::codecForMib(106)->toUnicode(FIByte);
        string.remove(0,4);

        QTextDocument textDoc;
        textDoc.setHtml(string);

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(m_nomeRefertoPdf);
        textDoc.print(&printer);

        FI.close();
        FI.remove();
    }
    else //REFRTF
    {
#ifdef WIN32
//      converte il report in PDF

        //Oggetto COM
        QAxObject *objPdfCreator = new QAxObject();
        objPdfCreator->setControl("PDFCreator.clsPDFCreator");

        objPdfCreator->setProperty("cVisible",true);
        if (!objPdfCreator->dynamicCall("cStart(String p)","/NoProcessingAtStartup").toBool())
        {
            if (!objPdfCreator->dynamicCall("cStart(String p, bool b)","/NoProcessingAtStartup",true).toBool())
            {
                //messaggio di avviso di chiudere una precedente sessione di pdfcreator
                emit sg_warnReport(tr("Problems with PDFCreator, please close precedent PDFCreator session"));
                delete objPdfCreator;
                return;
            }
            objPdfCreator->setProperty("cVisible",true);
        }

        //gestione opzioni
        objPdfCreator->setProperty("cShowOptionsDialog",false);
        QAxObject *opt = objPdfCreator->querySubObject("cOptions");
        QAxObject *optOld = opt;
        opt->setProperty("UseAutosave", 1);
        opt->setProperty("UseAutosaveDirectory", 1);
        QString nomeR = m_nomeRefertoPdf.mid(m_nomeRefertoPdf.lastIndexOf("\\")+1);
        opt->setProperty("AutosaveDirectory", QDir::toNativeSeparators(m_pathRef+"/tmp"));
        opt->setProperty("AutosaveFilename", nomeR);
        opt->setProperty("AutosaveFormat", 0);//0=PDF; 2=JPG
        objPdfCreator->setProperty("cOptions", opt->asVariant());
        objPdfCreator->dynamicCall("cClearCache()");

        //creo pdf
        objPdfCreator->dynamicCall("cPrintFile(String pathFile)",m_nomeReferto);
        int pr = objPdfCreator->property("cCountOfPrintjobs").toInt();
        int toexitwhile = 0;
        while (pr != 1 && toexitwhile < 10) {
            QThread::sleep(1);
            pr = objPdfCreator->property("cCountOfPrintjobs").toInt();
            toexitwhile++;
        }
        objPdfCreator->setProperty("cPrinterStop",false);
        pr = objPdfCreator->property("cCountOfPrintjobs").toInt();
        toexitwhile = 0;
        while (pr != 0 && toexitwhile < 10) {
            QThread::sleep(1);
            pr = objPdfCreator->property("cCountOfPrintjobs").toInt();
            toexitwhile++;
        }

        //ritorno a opzioni precedenti
        objPdfCreator->setProperty("cOptions", optOld->asVariant());
        objPdfCreator->dynamicCall("cClose()");

        bool cl = objPdfCreator->property("cIsClosed()").toBool();
        toexitwhile = 0;
        while (cl && toexitwhile < 10) {
            QThread::sleep(1);
            cl = objPdfCreator->property("cIsClosed()").toBool();
            toexitwhile++;
        }

        delete objPdfCreator;
        if (toexitwhile == 10)
            emit sg_warnReport(tr("Problems in the report writing"));
        else  //cancello rtf
            QFile::remove(m_nomeReferto);
#endif
#ifdef MAC

        QString command = "/Applications/LibreOffice.app/Contents/MacOS/soffice -norestore -headless --convert-to pdf --outdir "+ m_pathRef + "/tmp " + QDir::toNativeSeparators(m_nomeReferto);
        QByteArray ba = command.toLocal8Bit();
        system(ba.data());
        QFile::remove(m_nomeReferto); //cancello rtf
#endif
    }

    QString toEdit = m_reportEdit->getSafeChild("HTM")->getSafeAttribute(ATT_VALUE);
    if (toEdit == "false" || m_autoPrint) {
        QUrl urlFile = QUrl::fromLocalFile(m_nomeRefertoPdf);
        QProcess *viewer = new QProcess();
        QString namep = "/PDFviewer";
#ifndef MAC
        namep += ".exe";
#endif
        viewer->setProgram(QDir::toNativeSeparators(g_P7SettingsManager.progPath()+namep));
        viewer->setArguments(QStringList() << urlFile.toString() << g_P7SettingsManager.localization());
        if (!DebugAcqTool) viewer->startDetached();
    }

}

void MDataMngDesktop::startPrint(QString __codSoft)
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
    writer.writeTextElement("testNumber",QString::number(m_testNumber));
    foreach (int key, m_analysisMap.keys()) {
        QString ana = QString::number(m_analysisMap[key]);
        writer.writeTextElement("Analisi",ana);
    }
    for (int i=0;i<filesList.size();i++)
    {
        QString img = filesList.at(i);
        QFile *FI = new QFile(m_pathData + img);
        FI->open(QIODevice::ReadOnly);
        QByteArray FIByte = FI->readAll();
        QString txt = img.left(img.indexOf("."));
        if (m_tipoReferto == REFHTM) {
            QString imgBase64 = QString(FIByte.toBase64());
         //   QString txt = img.left(img.indexOf("."));
            writer.writeTextElement(txt,imgBase64);
        }
        else { //REFRTF
            QByteArray ArrayHex = FIByte.toHex();
            QString value = QString(ArrayHex);
            writer.writeTextElement(txt,value);
        }
        FI->close();

        dirImgs.remove(img);
    }

    writer.writeEndElement();//images
    writer.writeEndDocument();

    xmlFile->close();
    delete xmlFile;

    //stampo: apro il file
    if (m_autoPrint) {
        openReport(__codSoft);
        m_autoPrint = false; //non deve ristampare se l'utente riapre subito l'esame
    }
}

QString MDataMngDesktop::createNamePDf()
{
    //creo il nome per il PDF
    QString nomePDF = "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a" + ".pdf";
    m_nomeRefertoPdf = QDir::toNativeSeparators(m_pathRef + "/tmp/" + nomePDF);

    return nomePDF;
}

void MDataMngDesktop::exitFromReview()
{
    qDebug() << "Exit" << getToSave();

    if (g_File_LogGDPR.isOpen())
        g_File_LogGDPR.close();

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

        exit(0);
        return;
    }

    if (getToSave() == "")
        emit sg_exitFromReview();
    else
    {
        emit sg_clearResultForm();
        if (getToSave() == "yes")
        {
            //copio il file copy nell'originale
            if (QFile::exists(m_copyFileName))
            {
                saveChanges();
                qDebug()<<"cancello vecchio file"<<QFile::remove(m_fileName);
                qDebug()<<"copio le modifiche"<<QFile::rename(m_copyFileName,m_fileName);
            }

            if (QFile::exists(filenameAna))
            {
                QString newname = filenameAna;
                newname.replace("temp","an");
                newname.replace(".xml","1a.xml");
                if (QFile::exists(newname))
                    QFile::remove(newname);
                qDebug()<<"salvo file analisi"<<QFile::rename(filenameAna,newname);
            }

            //creazione PDF all'uscita nel caso di Edit del referto
            QString toEdit = m_reportEdit->getSafeChild("HTM")->getSafeAttribute(ATT_VALUE);
            if (m_tipoReferto == REFRTF && toEdit == "true" && !m_autoPrint)//da vedere bene sta cosa dell'autoprint qui
                createPdf();

            //sposto i referti da salvare nella cartella /Ref
            QString tmp = QDir::toNativeSeparators(m_pathRef + "/tmp");
            QStringList listref = QDir(tmp).entryList(QDir::Files);
            foreach (QString f, listref) {
                QString pathf = QDir::toNativeSeparators(m_pathRef + "/" + f);
                if (QFile::exists(pathf))
                    QFile::remove(pathf);
                QString pathTmp = QDir::toNativeSeparators(tmp + "/" + f);
                QFile(pathTmp).copy(pathf);
                QFile::remove(pathTmp);
            }
            QDir(m_pathRef).rmdir(tmp);

            if (m_analyzed) //è un test analizzato, devo scrivere sul database 1 nel campo Saved dei tests
                exit(E_SAVE);//ANALYZED
        }
        else //"no"
        {
            //cancello il file copy
            qDebug()<<"cancellata copia all'exit"<<QFile::remove(m_copyFileName);

            if (QFile::exists(filenameAna))
                qDebug()<<"cancello file temp analisi"<<QFile::remove(filenameAna);

            //cancello gli eventuali referti /Ref/tmp
            QString tmp = QDir::toNativeSeparators(m_pathRef + "/tmp/");
            QStringList listref = QDir(tmp).entryList(QDir::Files);
            foreach (QString f, listref) {
                qDebug() << "cancello referto" << QFile::remove(tmp + f);
            }
            QDir(m_pathRef).rmdir(tmp);

        }
        exit(0);
    }
}

QList<QString> MDataMngDesktop::getListReports()
{
    QList<QString> list;

    QString tipoFile = ".htm";
    if (m_tipoReferto == REFRTF)
        tipoFile = ".rtf";

    QString pathTemplate = QDir::toNativeSeparators(g_P7SettingsManager.dataPath() + "/grpbase/MRTemplate_" + g_P7SettingsManager.localization());
    QDir pathDir = QDir(pathTemplate);
    QFileInfoList entriesPath = pathDir.entryInfoList(QDir::Files);
    for(QList<QFileInfo>::iterator it = entriesPath.begin(); it!=entriesPath.end();++it)
    {
        QFileInfo &finfo = *it;
        QString name = finfo.baseName();
        if (name == "StandardSource") //questo è il template che teniamo come copia
            continue;
        QString suffix = "."+finfo.suffix();
        if (suffix != tipoFile)
            continue;

        list.push_back(name);
    }

    return list;

}

QString MDataMngDesktop::getTemplate()
{
    Ancestry* reportTemplate = m_configPrinter.getSafeChild("Template");
    QString value = reportTemplate->getSafeChild("Select")->getAttribute("value");
    return value;
}

void MDataMngDesktop::openExportTool(QString __codSoft)
{    
    //necessario salvare prima di fare l'esportazione per avere i dati risultati su pic
    saveChanges();
    QString pth;
#ifdef WIN32
    pth = QDir::toNativeSeparators(g_P7SettingsManager.progPath()+"/exportTool.exe");
#elif MAC
    pth = QDir::toNativeSeparators(g_P7SettingsManager.progPath()+"/ExportTool");
#endif
    QProcess *proc = new QProcess();
    proc->setProgram(pth);

    QStringList arg;
    QString typeImg = "h";
    if (m_tipoReferto == REFRTF)
        typeImg = "r";

    arg << "s" << __codSoft << QString::number(m_testNumber) << typeImg;
    proc->setArguments(arg);

    connect(proc,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [=](int exitCode, QProcess::ExitStatus exitStatus)
    {
     // qDebug()<< exitCode << exitStatus;
      delete proc;
    });

//    connect(proc, &QProcess::started,[=]()
//    {
//      qDebug()<< "STARTED";

//    });


    proc->start();
}


