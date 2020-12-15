#include "mdatamngdesktop.h"
#include "medicalreport.h"
#include <QErrorMessage>
#include <QDesktopServices>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QPrinter>
#include <QTextDocument>
#include <QProcess>

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

bool MDataMngDesktop::checkReport()
{
    QString nomePDF = "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a" + ".pdf";
    m_nomeRefertoPdf = g_P7SettingsManager.dataPath() + "\\ref\\" + nomePDF;
    QFile filePdf(m_nomeRefertoPdf);
    if (filePdf.exists())
        return true;
    else
        return false;
}

void MDataMngDesktop::openReport(QString __nomeReport)
{
    if (__nomeReport == "")
        __nomeReport = "Standard";

    QLibrary reportLib("MedicalReport.dll");
    if (reportLib.load())
    {
        bool refDone = false;
        QString nomeR = __nomeReport+".htm";
        Init(g_P7SettingsManager.dataPath().toLatin1(),g_P7SettingsManager.appPath().toLatin1(),m_copyFileName.toLatin1(), 4,nomeR.toLatin1(),g_P7SettingsManager.localization());
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

    m_nomeReferto = g_P7SettingsManager.dataPath() + "\\ref\\" +  "rf" + QString("%1").arg(m_testNumber,5,10,QLatin1Char('0')) + "1a" + ".htm";

    m_reportEdit = m_configPrinter.getSafeChild("Report");
    QString toEdit = m_reportEdit->getSafeChild("HTM")->getSafeAttribute(ATT_VALUE);

    if (toEdit == "false") {
        //necessario trasformare il file referto htm in htm tradotto per leggere tutti i caratteri
        QFile f(m_nomeReferto);
        f.open(QIODevice::ReadOnly);
        QByteArray data = f.readAll();
        QTextCodec *codec = QTextCodec::codecForName(m_language);
        QString stringa = codec->codecForMib(106)->toUnicode(data);
        QTextDocument textDoc;
        textDoc.setHtml(stringa);
        f.close();
        QFile f1(g_P7SettingsManager.dataPath() + "\\ref\\" + "prova.html");
        f1.open(QIODevice::WriteOnly);
        f1.write(textDoc.toHtml("utf-8").toUtf8());
        f.remove();
        f1.close();
        f1.copy(m_nomeReferto);
        f1.remove();

        createPdf();
    }
    else {
        // APERTURA FILE NS EDITOR              

        //Nome analisi
        QString tempFile, resultFile, nomeAnalisi;
        QDir ResPath = QDir(g_P7SettingsManager.datafilePath());
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
        if (tempFile.size() != 0)
            nomeAnalisi = g_P7SettingsManager.datafilePath() + "\\" + tempFile;
        else
            nomeAnalisi = g_P7SettingsManager.datafilePath() + "\\" + resultFile;

        QStringList arg;
        arg << "file:///" +  m_nomeReferto << "r" << g_P7SettingsManager.localization() << nomeAnalisi;
        QString pth = g_P7SettingsManager.progPath()+"/texteditor.exe";

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
}

void MDataMngDesktop::createPdf()
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

    QString toEdit = m_reportEdit->getSafeChild("HTM")->getSafeAttribute(ATT_VALUE);
    if (toEdit == "false") {
        QUrl urlFile = QUrl::fromLocalFile(m_nomeRefertoPdf);
        QProcess *viewer = new QProcess();
        viewer->setProgram(g_P7SettingsManager.progPath()+"/PDFviewer.exe");
        viewer->setArguments(QStringList() << urlFile.toString());
        bool ret = viewer->startDetached();
    }

    FI.close();
    FI.remove();
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

        QString imgBase64 = QString(FIByte.toBase64());
        QString txt = img.left(img.indexOf("."));
        writer.writeTextElement(txt,imgBase64);

        FI->close();

        dirImgs.remove(img);
    }

    writer.writeEndElement();//images
    writer.writeEndDocument();

    xmlFile->close();
    delete xmlFile;

    //stampo: apro il file in word
    if (m_autoPrint) {
        openReport("Standard");
        m_autoPrint = false; //non deve ristampare se l'utente riapre subito l'esame
    }
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

            if (QFile::exists(filenameAna))
            {
                QString newname = filenameAna;
                newname.replace("temp","an");
                newname.replace(".xml","1a.xml");
                if (QFile::exists(newname))
                    QFile::remove(newname);
                qDebug()<<"salvo file analisi"<<QFile::rename(filenameAna,newname);
            }

            if (m_analyzed) //è un test analizzato, devo scrivere sul database 1 nel campo Saved dei tests
                g_mainAppBridge->testsAnalyzed();
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

QList<QString> MDataMngDesktop::getListReports()
{
    QList<QString> list;

    QString pathTemplate = g_P7SettingsManager.progPath() + "/grpbase/MRTemplate_" + g_P7SettingsManager.localization();
    QDir pathDir = QDir(pathTemplate);
    QFileInfoList entriesPath = pathDir.entryInfoList(QDir::Files);
    for(QList<QFileInfo>::iterator it = entriesPath.begin(); it!=entriesPath.end();++it)
    {
        QFileInfo &finfo = *it;
        QString name = finfo.baseName();
        if (name == "StandardSource") //questo è il template che teniamo come copia
            continue;

        list.push_back(name);
    }

    return list;

}

void MDataMngDesktop::openExportTool()
{    
    //necessario salvare prima di fare l'esportazione per avere i dati risultati su pic
    saveChanges();
    QString pth = g_P7SettingsManager.progPath()+"/exportTool.exe";
    QProcess *proc = new QProcess();
    proc->setProgram(pth);

    QStringList arg;
    arg << "s" << QString::number(m_testNumber);
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


