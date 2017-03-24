TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-psabi

#TARGET = acqTool
TARGET = PicoAcq
QT += qml quick widgets sql network multimedia xml core serialport

RESOURCES += Resources/qml.qrc

DEFINES += QT_MESSAGELOGCONTEXT
#debug campioni di acquisizione
DEFINES += DEBUG_ACQ

SOURCES += \
    main.cpp \
    CPP/alarmmanager.cpp \
    CPP/androidmanager.cpp \
    CPP/mabstractmanager.cpp \
    CPP/macqmanager.cpp \
    CPP/manatablemodel.cpp \
    CPP/mdatamanager.cpp \
    CPP/mflowdatas.cpp \
    CPP/modelmanager.cpp \
    CPP/mstorage.cpp \
    CPP/native.cpp \
    CPP/nomogramma.cpp \
    CPP/parametermanager.cpp \
    CPP/printermanager.cpp \
    CPP/printerserialport.cpp \
    CPP/TCP/SimpleTCPChannel.cpp \
    CPP/TCP/SimpleTCPClient.cpp \
    CPP/TCP/TcpServers.cpp \
    CPP/TCP/TcpSettingFile.cpp \
    ../MGlobal/ancestry.cpp \
    ../MGlobal/appbridge.cpp \
    ../MGlobal/global.cpp \
    ../MGlobal/layoutmanager.cpp \
    ../MGlobal/msignal.cpp \
    ../MGlobal/MyMessageOutput.cpp \
    ../MGlobal/p7settingsmanager.cpp \
    ../MGlobal/systemmanager.cpp \
    ../MGlobal/UdmImpl.cpp \
    ../AnaUro/anauro.cpp \
    ../AnaUro/analysis.cpp \
    ../AnaUro/anautils.cpp \ 

HEADERS += \
    CPP/alarmmanager.h \
    CPP/androidmanager.h \
    CPP/mabstractmanager.h \
    CPP/macqmanager.h \
    CPP/manatablemodel.h \
    CPP/mdatamanager.h \
    CPP/mflowdatas.h \
    CPP/modelmanager.h \
    CPP/mstorage.h \
    CPP/nomogramma.h \
    CPP/parametermanager.h \
    CPP/printermanager.h \
    CPP/printerserialport.h \
    CPP/TCP/SimpleTCPChannel.h \
    CPP/TCP/SimpleTCPClient.h \
    CPP/TCP/TcpServers.h \
    CPP/TCP/TcpSettingFile.h \
    ../AnaUro/anauro.h\
    ../AnaUro/anauro_global.h \
    ../AnaUro/analysis.h \
    ../AnaUro/Anaerrorcodes.h \
    ../AnaUro/anautils.h \
    ../MGlobal/ancestry.h \
    ../MGlobal/appbridge.h \
    ../MGlobal/global.h \
    ../MGlobal/layoutmanager.h \
    ../MGlobal/msignal.h \
    ../MGlobal/p7settingsmanager.h \
    ../MGlobal/systemmanager.h \
    ../MGlobal/UdmImpl.h

INCLUDEPATH +=  CPP \
                CPP/TCP \                
                ../AnaUro \
                ../MGlobal \
                ../MPF78003-Picoflow2R3Supe \
                ../DataFileManager \
                ../SupeFlowBT

# Default rules for deployment.
include(deployment.pri)

TRANSLATIONS += acqtool_it.ts
lupdate_only{
    SOURCES = Resources/*.qml \
             ../MGlobal/MComponents/*.qml
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/AnaUro7Build/release/ -lAnaUro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/AnaUro7Build/debug/ -lAnaUro

#INCLUDEPATH += $$PWD/../Build/DataBuild/release
#DEPENDPATH += $$PWD/../Build/DataBuild/release

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    DEFINES += ANDROID
    QT += androidextras
        ANDROID_EXTRA_LIBS = \
            $$PWD/../Build/Emulator/DataBuild/libDatafileManager.so \
            $$PWD/../Build/Emulator/MPlotModule/libQmlPlotter.so

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

    unix:!macx: LIBS += -L$$PWD/../../build/DatafileManager/Debug/Desktop_Qt_5_5_1_GCC_64bit -lDatafileManager

    #INCLUDEPATH += $$PWD/../Build/Emulator/DataBuild
    #DEPENDPATH += $$PWD/../Build/Emulator/DataBuild
}

#percorso tecnoideal
#rootPath = M:
#percorso acer luca
#rootPath = C:\Users\Mez
#percorso claudia

win32 {
    rootPath = E:\Piattaforma70
    #creo la cartella da copiare in giro
    #plugin.path = $${rootPath}\Lavoro\Software\Build\StandAlone
    #claudia
    plugin.path = $${rootPath}\Build\StandAlone
    plugin.files += $$shell_path($$OUT_PWD)\release\acqTool.exe
    INSTALLS += plugin

    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DataBuild/release/ -lDatafileManager
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DataBuild/debug/ -lDatafileManager

    INCLUDEPATH += $$PWD/../Build/DataBuild/release
    DEPENDPATH += $$PWD/../Build/DataBuild/release
}

unix {
    DEFINES += TAR
}
#unix:!macx: LIBS += -L$$PWD/../Build/DataFileManager/ -lDatafileManager

#INCLUDEPATH += $$PWD/../Build/DataFileManager
#DEPENDPATH += $$PWD/../Build/DataFileManager

#unix:!macx: LIBS += -L$$PWD/../Build-Linux/DatafileManager -lDatafileManager

#tastiera virtuale
CONFIG += link_pkgconfig

#linux-buildroot-g++ {
#    deployment.files = *.qml *.otf *.ttf *.svg
#    target.path = /vktest
#    deployment.path = /vktest
#    INSTALLS += target deployment
#} else {
    #target.path = $$[QT_INSTALL_PLUGINS]/virtualkeyboard
    #INSTALLS += target
#}

LINUXDESKTOP {
    DEFINES += LINUXDESKTOP
    DEFINES += LINUX
    TARGET = PicoAcq
    RESOURCES += modules_LinuxDesktop.qrc
    RESOURCES += Resources/Icone/icons.qrc

    #datafile manager
    LIBS        += -L$$PWD/../Build-Linux/DatafileManager -lDatafileManager
    INCLUDEPATH +=   $$PWD/../Build-Linux/DatafileManager
    DEPENDPATH  +=   $$PWD/../Build-Linux/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
    QML_IMPORT_PATH +=  Modules

    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/* \
}

PICOFLOW {
    DEFINES += PICOFLOW
    TARGET = PicoAcq
    RESOURCES += modules_picoflow.qrc
    RESOURCES += Resources/Icone/icons.qrc

    LIBS        += -L$$PWD/../Build-IMX6/DatafileManager/ -lDatafileManager
    INCLUDEPATH +=   $$PWD/../Build-IMX6/DatafileManager
    DEPENDPATH  +=   $$PWD/../Build-IMX6/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
                      ../Build-IMX6/CommonPlugin
    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/*
}

#per debugare solo acqtool
#DEFINES += DEBUGACQTOOL







