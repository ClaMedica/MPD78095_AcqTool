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
#include <mdatamngpico.h>
#include <mdatamngdesktop.h>
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
#include "graficmanager.h"
#include <QSplashScreen>
#include <fileio.h>
#include <systemmanager.h>

#ifdef ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#include <androidmanager.h>
#endif

char strvers[] = "AcqTool del " __DATE__ " alle " __TIME__;

bool DebugAcqTool = false;

AcqBridge *g_mainAppBridge;
#ifdef PICOFLOW
#endif

int main(int argc, char *argv[])
{
    bool bool_false = false; (void) bool_false;
    bool bool_true  = true;  (void) bool_true;

    Q_INIT_RESOURCE(qml);
#ifdef STATICO
    Q_IMPORT_PLUGIN(QmlPlotterPlugin)
    Q_INIT_RESOURCE(qmlplotter);
#endif

    QGuiApplication app(argc, argv);

#ifdef ANDROID
    QtAndroid::androidActivity().callMethod<void>("registerBroadcastReceiver", "()V");
#endif

#ifdef ANDROID
    gPath_log = "/mnt/sdcard/" + logFile;
#endif

#ifdef PICOFLOW
#ifdef MESSAGE2SYSLOG
    MyMessageOutput::init();
#else
    QString logFile = "acqTool_log_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")+".htm";
    gPath_log = "/tmp/" + logFile;
    qDebug() << "Start. log:" << gPath_log;
    //dirotto il debug log
    MyMessageOutput::init(gPath_log);
#endif
#endif

#ifdef WIN32
     QString logFile = "acqTool_log_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")+".htm";
     gPath_log = QApplication::applicationDirPath() + "/"+ logFile;
     qDebug() << "Start. log:" << gPath_log;
     //dirotto il debug log
     //MyMessageOutput::init(gPath_log);
#endif



    //carico i settaggi
    g_P7SettingsManager.loadSettings(true);
    //localizzazione
    QString local = g_P7SettingsManager.localization();
    QString pathTranslations = g_P7SettingsManager.appPath();
#ifdef STATICO
    pathTranslations += "/translations";
#endif
    qDebug()<<"Language to load: "<< pathTranslations + "/acqtool_" + local;
    QTranslator translator;
    qDebug() << translator.load(pathTranslations + "/acqtool_" + local);
    QTranslator translatorPlotter;
    translatorPlotter.load(pathTranslations + "/QmlPlotter_" + local);
    app.installTranslator(&translatorPlotter);
    app.installTranslator(&translator);
    //versione
    qDebug() << "Vers. " << g_P7SettingsManager.loadVersione();

    QStringList arguments;
    //leggiamo gli argomenti
    for(int i = 0; i < argc; i++)
        arguments << QString(argv[i]);
    qDebug() << "Argomenti" << arguments;

    if((argc > 1) && (strcmp(argv[argc - 1], (const char *)"debug") == 0))
        DebugAcqTool = true;
    if(DebugAcqTool == false)
        g_mainAppBridge = new AcqBridge(QStringList() << argv[1] << argv[2]);   //definisco un bridge tra app di tipo server

    qmlRegisterType<ParameterManager>("Managers", 1, 0, "ParameterManager");
    qmlRegisterType<ModelManager>("Managers", 1, 0, "ModelManager");
    qmlRegisterType<MAcqManager>("Managers", 1, 0, "MAcqManager");
#ifdef PICOFLOW
    qmlRegisterType<MDataMngPico>("Managers", 1, 0, "MDataManager");
#else
    qmlRegisterType<MDataMngDesktop>("Managers", 1, 0, "MDataManager");
#endif
 //   qmlRegisterType<MDataManager>("Managers", 1, 0, "MDataManager");
    qmlRegisterType<fileIO>("FileIO", 1, 0, "FileIO");

    qmlRegisterUncreatableType<mflowdatas>("Managers", 1, 0, "Mflowdata", "error on anaFlwAdv creation");
    qmlRegisterUncreatableType<mflowdatasModel>("Managers", 1, 0, "Mflowdatamodel", "error on anaFlwAdv creation");
    qmlRegisterUncreatableType<Nomogramma>("Managers", 1, 0, "Nomogramma", "error on Nomogramma creation");

    QQmlApplicationEngine engine;

    //Carico il layout di default del programma
    LayoutManager mngLayout;
    GraficManager mngGrafic;

    engine.addImportPath("qrc:/Modules");
    engine.addImportPath("qrc:/");
#ifndef STATICO
    engine.addImportPath(QApplication::applicationDirPath());
#endif

#ifdef PICOFLOW//metto questo altrimenti su target non carica il plugin cpp
    engine.rootContext()->setContextProperty(QLatin1String("platform"), "linux");
    engine.rootContext()->setContextProperty("screenH", 480);
    engine.rootContext()->setContextProperty("screenW", 640);
    engine.rootContext()->setContextProperty("isTouch", bool_true);
    engine.rootContext()->setContextProperty("PicoFlow", bool_true);
#endif

#ifdef ANDROID
    engine.addImportPath("/mnt/sdcard/Medica");
    AndroidManager *androidmanager = new AndroidManager(&engine);
    engine.rootContext()->setContextProperty(QLatin1String("androidmanager"),
                                             androidmanager);
    engine.rootContext()->setContextProperty(QLatin1String("platform"), "android");
#endif

#ifdef LINUXDESKTOP
    engine.rootContext()->setContextProperty(QLatin1String("platform"), "linux");
    engine.rootContext()->setContextProperty("screenH", 480);
    engine.rootContext()->setContextProperty("screenW", 640);
    engine.rootContext()->setContextProperty("isTouch", bool_false);
    engine.rootContext()->setContextProperty("PicoFlow", bool_false);
#endif

#ifdef WIN32
    engine.rootContext()->setContextProperty(QLatin1String("platform"), "window");
    engine.rootContext()->setContextProperty("isTouch", bool_false);
    QSize size = app.primaryScreen()->size();
    engine.rootContext()->setContextProperty("screenH", size.height());
    engine.rootContext()->setContextProperty("screenW", size.width());
    engine.rootContext()->setContextProperty("PicoFlow", bool_false);
#ifndef STATICO
    if(DebugAcqTool)
        engine.addImportPath(QApplication::applicationDirPath() + "/Modules");
#endif
#endif

    engine.rootContext()->setContextProperty("layout", &mngLayout);
    engine.rootContext()->setContextProperty("grafic", &mngGrafic);
    //engine.rootContext()->setContextProperty("settings", &g_P7SettingsManager);
    engine.rootContext()->setContextProperty("bridgeMain", g_mainAppBridge);    
    engine.rootContext()->setContextProperty("mngSys", &g_systemManager);

    qDebug() << engine.importPathList();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qDebug() << "engine caricato";


    if(DebugAcqTool == false)
        g_mainAppBridge->setRootObjects(engine.rootObjects());

    int ret = app.exec();

    qDebug() << "Uscito" << ret;
    return ret;
}
