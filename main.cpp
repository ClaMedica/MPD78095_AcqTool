#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "qqml.h"
#include <parametermanager.h>
#include <msignal.h>
#include <global.h>
#include <modelmanager.h>
#include <macqmanager.h>
#include <mdatamanager.h>
#include <QProcess>
#include <QtGlobal>
#include <QtMessageHandler>
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include <QObject>
#include <p7settingsmanager.h>



int main(int argc, char *argv[])
{
    //creo il file di report
    qInstallMessageHandler(myMessageOutput); //install : set the callback
    QGuiApplication app(argc, argv);
    QString logFile="acqTool_log.htm";
#ifdef ANDROID
    gPath_log="/mnt/sdcard/"+logFile;
#else
    gPath_log=QApplication::applicationDirPath()+"/"+logFile;
#endif

    QFile f;
    f.setFileName(gPath_log);
    f.open(QIODevice::WriteOnly);
    f.write(LOG_HEADER);
    QString curDateTime=QDateTime::currentDateTime().toString()+"<BR>";
    f.write(curDateTime.toLatin1());
    f.close();
    qDebug()<<"Partiamo";


    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MAcqManager>("Managers",1,0,"MAcqManager");
    qmlRegisterType<MDataManager>("Managers",1,0,"MDataManager");
    qmlRegisterType<P7Settings>("Managers",1,0,"MSettings");

    QQmlApplicationEngine engine;

    engine.addImportPath("../CommonPlugin");
    engine.addImportPath("../../AcqTool");



    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));



    int ret=app.exec();
    f.open(QIODevice::Append);
    f.write("</BODY></HTML>");
    f.close();
    return ret;
}
