TEMPLATE = app
VERSION = 1.0.0

QT += qml quick widgets sql network multimedia xml core serialport

RESOURCES += Resources/qml.qrc
RESOURCES += Resources/Icone/icons.qrc

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

win32 {
    DEFINES += WIN32
    TARGET = acqTool
    rootPath = E:\Piattaforma70
    #creo la cartella da copiare in giro
    #plugin.path = $${rootPath}\Lavoro\Software\Build\StandAlone
    #claudia
    plugin.path = $${rootPath}\build\standalone
    plugin.files += $$shell_path($$OUT_PWD)\release\acqTool.exe
    INSTALLS += plugin

    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/release/ -lDatafileManager
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/debug/ -lDatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
                      Modules

    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/*
}

unix {
    DEFINES += TAR
}

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
QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += LINUXDESKTOP
    DEFINES += LINUX
    TARGET = PicoAcq
    RESOURCES += modules_LinuxDesktop.qrc

    #datafile manager
    LIBS        += -L$$PWD/../Build-Linux/DatafileManager -lDatafileManager
    INCLUDEPATH +=   $$PWD/../Build-Linux/DatafileManager
    DEPENDPATH  +=   $$PWD/../Build-Linux/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal
    QML_IMPORT_PATH +=  Modules

    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/* \
}

PICOFLOW {
QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += PICOFLOW
    TARGET = PicoAcq

    RESOURCES += modules_picoflow.qrc

    LIBS        += -L$$PWD/../Build-IMX6/DatafileManager/ -lDatafileManager
    INCLUDEPATH +=   $$PWD/../Build-IMX6/DatafileManager
    DEPENDPATH  +=   $$PWD/../Build-IMX6/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
                      ../Build-IMX6/CommonPlugin
    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/*
}








