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
#include <mflowdatas.h>
#include <QProcess>
#include <QtGlobal>
#include <QtMessageHandler>
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include <QObject>
#include <p7settingsmanager.h>
#include <MyMessageOutput.h>
#include <appbridge.h>
#include <QApplication>
#include <QQuickWindow>
#include "layoutmanager.h"
#include <QSplashScreen>
#ifdef ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#include <androidmanager.h>
#endif
AcqBridge *g_mainAppBridge;

int main(int argc, char *argv[])
{
    // Load virtualkeyboard input context plugin
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

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
    MyMessageOutput::init(gPath_log);
    qDebug()<<"Partiamo";
    QStringList arguments;
    //leggiamo gli argomenti
    for(int i=0;i<argc;i++)
        arguments<<QString(argv[i]);
    qDebug()<<"Argomenti"<<arguments;
    //qDebug()<<fibonacci(5);
    g_mainAppBridge = new AcqBridge(QStringList() << argv[1] << argv[2]);//definisco un bridge tra app di tipo server

    qmlRegisterType<ParameterManager>("Managers",1,0,"ParameterManager");
    qmlRegisterType<ModelManager>("Managers",1,0,"ModelManager");
    qmlRegisterType<MAcqManager>("Managers",1,0,"MAcqManager");
    qmlRegisterType<MDataManager>("Managers",1,0,"MDataManager");
    qmlRegisterType<P7Settings>("Managers",1,0,"MSettings");

    qmlRegisterUncreatableType<mflowdatas>("Managers",1,0,"mflowdata","error on anaFlwAdv creation");
    qmlRegisterUncreatableType<mflowdatasModel>("Managers",1,0,"mflowdatamodel","error on anaFlwAdv creation");
    qmlRegisterUncreatableType<Nomogramma>("Managers",1,0,"nomogramma","error on Nomogramma creation");

    QQmlApplicationEngine engine;
    //Carico il layout di default del programma
    LayoutManager mngLayout;
    engine.addImportPath("qrc:/Modules/");
    engine.addImportPath("../standalone/");
    engine.addImportPath(QApplication::applicationDirPath());
#ifdef PICOFLOW//metto questo altrimenti su target non carica il plugin cpp
    engine.rootContext()->setContextProperty("screenH",480);
    engine.rootContext()->setContextProperty("screenW",640);
    engine.rootContext()->setContextProperty("isTouch",true);
    engine.addPluginPath("../PicoFlow");
#endif
#ifdef ANDROID
    engine.addImportPath("/mnt/sdcard/Medica");

    AndroidManager *androidmanager = new AndroidManager(&engine);
    engine.rootContext()->setContextProperty(QLatin1String("androidmanager"),
                                             androidmanager);
    engine.rootContext()->setContextProperty(QLatin1String("platform"),"android");
#else
    engine.rootContext()->setContextProperty(QLatin1String("platform"),"linux");
#endif
    engine.rootContext()->setContextProperty("layout",&mngLayout);
    engine.rootContext()->setContextProperty("settings",&g_P7SettingsManager);
    engine.rootContext()->setContextProperty("bridgeMain",g_mainAppBridge);



    qDebug()<<engine.importPathList();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qDebug()<<"engine caricato";
    g_mainAppBridge->setRootObjects(engine.rootObjects());
    int ret = app.exec();
    qDebug() << "Uscito" << ret;
    return ret;
}
