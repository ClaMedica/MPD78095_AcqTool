#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qqml.h"
#include <doppleranalysis.h>
#include <parametermanager.h>
#include <msignal.h>
#include <global.h>
#include <mapmanager.h>
#include <modelmanager.h>
#include <macqmanager.h>
#include <mdatamanager.h>
#include <QProcess>
#include <QtGlobal>
#include <QtMessageHandler>
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{
    //creo il file di report


    qInstallMessageHandler(myMessageOutput); //install : set the callback
    QFile f;
    f.setFileName(QDir::currentPath()+LOG_FILE);
    f.open(QIODevice::WriteOnly);
    f.write(LOG_HEADER);
    QString curDateTime=QDateTime::currentDateTime().toString()+"<BR>";
    f.write(curDateTime.toLatin1());
    f.close();


    QGuiApplication app(argc, argv);

    qmlRegisterType<DopplerAnalysis>("Analysis",1,0,"DopplerAnalysis");
    qmlRegisterType<MPlayer>("Audio",1,0,"MPlayer");
    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<MapManager>("Managers",1,0,"MapManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MAcqManager>("Managers",1,0,"MAcqManager");
    qmlRegisterType<MDataManager>("Managers",1,0,"MDataManager");


    QQmlApplicationEngine engine;


    engine.addImportPath("../../UsefulOBJ");
    engine.addImportPath("../../AcqTool/QML");
    engine.addImportPath("../CommonPlugin");

    engine.addImportPath("../../../UsefulOBJ");
    engine.addImportPath("../../../AcqTool/QML");
    engine.addImportPath("../../CommonPlugin");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));



    int ret=app.exec();
    f.open(QIODevice::Append);
    f.write("</BODY></HTML>");
    f.close();
    return ret;
}
