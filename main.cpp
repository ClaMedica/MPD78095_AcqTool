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
#include <p7settingsmanager.h>



int main(int argc, char *argv[])
{
    //creo il file di report
    qInstallMessageHandler(myMessageOutput); //install : set the callback
    QGuiApplication app(argc, argv);
    QFile f;
    f.setFileName(QDir::currentPath()+"/"+QGuiApplication::applicationDisplayName()+"_log.htm");
    f.open(QIODevice::WriteOnly);
    f.write(LOG_HEADER);
    QString curDateTime=QDateTime::currentDateTime().toString()+"<BR>";
    f.write(curDateTime.toLatin1());
    f.close();




    qmlRegisterType<MPlayer>("Audio",1,0,"MPlayer");
    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MAcqManager>("Managers",1,0,"MAcqManager");
    qmlRegisterType<MDataManager>("Managers",1,0,"MDataManager");
    qmlRegisterType<P7Settings>("Managers",1,0,"MSettings");

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
