#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "qqml.h"
#include <QtGui>
#include <QtQuick>
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
#include <MyMessageOutput.h>
#ifdef ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#include <androidmanager.h>
#endif


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qml);

    QGuiApplication app(argc, argv);
    //carico i settaggi
    g_P7SettingsManager.loadSettings();
#ifdef ANDROID
    QtAndroid::androidActivity().callMethod<void>("registerBroadcastReceiver", "()V");
#endif
    QString logFile="acqTool_log.htm";
#ifdef ANDROID
    gPath_log="/mnt/sdcard/"+logFile;
#else
    gPath_log=QApplication::applicationDirPath()+"/"+logFile;
#endif

    //dirotto il debug log
    MyMessageOutput outmsg;
    outmsg.init(gPath_log);
    qDebug()<<"Partiamo";
    //qDebug()<<fibonacci(5);

    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MAcqManager>("Managers",1,0,"MAcqManager");
    qmlRegisterType<MDataManager>("Managers",1,0,"MDataManager");

    qmlRegisterType<P7Settings>("Managers",1,0,"MSettings");

    QQmlApplicationEngine engine;

    engine.addPluginPath("qrc:/");
    engine.addImportPath("../../AcqTool/Resources");
#ifdef ANDROID
    engine.addImportPath("/mnt/sdcard/Medica");

    AndroidManager *androidmanager = new AndroidManager(&engine);
    engine.rootContext()->setContextProperty(QLatin1String("androidmanager"),
                                                         androidmanager);
    engine.rootContext()->setContextProperty(QLatin1String("platform"),"android");
#else
    engine.rootContext()->setContextProperty(QLatin1String("platform"),"win");
#endif
    engine.rootContext()->setContextProperty("settings",&g_P7SettingsManager);
    qDebug()<<engine.importPathList();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qDebug()<<"engine caricato";


    int ret=app.exec();
    f.open(QIODevice::Append);
    f.write("</BODY></HTML>");
    f.close();
    return ret;
}
