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

int main(int argc, char *argv[])
{
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


    return app.exec();
}
