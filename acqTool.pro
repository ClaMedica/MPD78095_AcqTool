TEMPLATE = app
VERSION = 1.0.0.8

QT += qml quick widgets sql network multimedia xml core serialport gui

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
    ../MGlobal/fileio.cpp \
    CPP/graficmanager.cpp

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
    CPP/TCP/SimpleTCPChannel.h \
    CPP/TCP/SimpleTCPClient.h \
    CPP/TCP/TcpServers.h \
    CPP/TCP/TcpSettingFile.h \
    CPP/bitmapsv.h \
    CPP/graficmanager.h \
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
    ../MGlobal/UdmImpl.h \
    ../MGlobal/fileio.h


INCLUDEPATH +=  CPP \
                CPP/TCP \
                ../AnaUro \
                ../MGlobal \
                ../MPF78003-Picoflow2R3Supe \
                ../DataFileManager \
                ../SupeFlowBT

# Default rules for deployment.
include(deployment.pri)


TRANSLATIONS += acqtool.xlf \
               # acqtool_en.xlf \
                acqtool_it.xlf

lupdate_only{
    SOURCES += Resources/*.qml \
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

    SOURCES += CPP/mdatamngdesktop.cpp
    HEADERS += CPP/mdatamngdesktop.h

    INCLUDEPATH +=  ../MedicalReport

    rootPath = E:\Piattaforma70
    #creo la cartella da copiare in giro
    #plugin.path = $${rootPath}\Lavoro\Software\Build\StandAlone
    #claudia
    plugin.path = $${rootPath}\build\standalone
    plugin.files += $$shell_path($$OUT_PWD)\release\acqTool.exe
    INSTALLS += plugin

    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/release/ -lDatafileManager
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/debug/ -lDatafileManager
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/MedicalReport/release/ -lMedicalReport
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/MedicalReport/debug/ -lMedicalReport


    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
                      Modules

    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/*

    QT += printsupport
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

CLAUDIA {
    QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += PICOFLOW
    TARGET = PicoAcq

    SOURCES += \
        ../MGlobal/udpmsgs.cpp \
        CPP/mdatamngpico.cpp

    HEADERS += \
        ../MGlobal/udpmsgs.h \
        CPP/mdatamngpico.h

    RESOURCES += modules_picoflow.qrc

    LIBS        += -L/sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager/ -lDatafileManager
    INCLUDEPATH +=   /sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager
    DEPENDPATH  +=   /sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../MGlobal \
                      /sviluppo/qt/Piattaforma70/Build-IMX6/CommonPlugin
    DISTFILES += ../MGlobal/MComponents/* \
                 ../MGlobal/MComponents/Images/*
}

PICOFLOW {
    QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += PICOFLOW
    TARGET = PicoAcq

    SOURCES += \
        ../MGlobal/udpmsgs.cpp \
        CPP/mdatamngpico.cpp

    HEADERS += \
        ../MGlobal/udpmsgs.h \
        CPP/mdatamngpico.h

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








