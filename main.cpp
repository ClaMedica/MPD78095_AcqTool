#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qqml.h"
#include <doppleranalysis.h>
#include <parametermanager.h>
#include <msignal.h>
#include <global.h>
#include <mapmanager.h>
#include <modelmanager.h>
#include <mdatafilemanager.h>
#include <morganizer.h>
#include <QProcess>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<DopplerAnalysis>("Analysis",1,0,"DopplerAnalysis");
    qmlRegisterType<MPlayer>("Audio",1,0,"MPlayer");
    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<MapManager>("Managers",1,0,"MapManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MDataFileManager>("Managers",1,0,"MDataFileManager");
    qmlRegisterType<MOrganizer>("Managers",1,0,"MOrganizer");

    QQmlApplicationEngine engine;


    engine.addImportPath("../../UsefulOBJ");
    engine.addImportPath("../../AcqTool/QML");
    engine.addImportPath("../CommonPlugin");

    engine.addImportPath("../../../UsefulOBJ");
    engine.addImportPath("../../../AcqTool/QML");
    engine.addImportPath("../../CommonPlugin");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    return app.exec();
}
