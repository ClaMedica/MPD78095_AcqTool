TEMPLATE = app
VERSION = 2.3.1

QT += qml quick widgets sql network multimedia xml core serialport gui

RESOURCES += Resources/qml.qrc
RESOURCES += Resources/Icone/icons.qrc

DEFINES += QT_MESSAGELOGCONTEXT
#debug campioni di acquisizione
DEFINES += DEBUG_ACQ

# Default rules for deployment.
include(deployment.pri)


TRANSLATIONS += acqtool.xlf \
               # acqtool_en.xlf \
                acqtool_it.xlf \
                acqtool_no.xlf \
                acqtool_pl.xlf \
                acqtool_fr.xlf \
                acqtool_pt.xlf \
                acqtool_es.xlf \
                acqtool_de.xlf

    lupdate_only{
        SOURCES += Resources/*.qml \
                 ../M8078027_MGlobal/MComponents/*.qml
    }

    SOURCES += \
    ../MPD78097_Database/msqloperators.cpp \
    ../MPD78097_Database/msqlsenders.cpp \
    CPP/TCP/SimpleTCPClientAcq.cpp \
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
        CPP/TCP/TcpServers.cpp \
        CPP/TCP/TcpSettingFile.cpp \
        CPP/graficmanager.cpp \
        ../M8078027_MGlobal/ancestry.cpp \
        ../M8078027_MGlobal/appbridge.cpp \
        ../M8078027_MGlobal/global.cpp \
        ../M8078027_MGlobal/layoutmanager.cpp \
        ../M8078027_MGlobal/msignal.cpp \
        ../M8078027_MGlobal/MyMessageOutput.cpp \
        ../M8078027_MGlobal/p7settingsmanager.cpp \
        ../M8078027_MGlobal/systemmanager.cpp \
        ../M8078027_MGlobal/UdmImpl.cpp \
        ../MPD78096_AnaUro/anauro.cpp \
        ../MPD78096_AnaUro/analysis.cpp \
        ../MPD78096_AnaUro/anautils.cpp \
        ../M8078027_MGlobal/fileio.cpp \
        ../M8078027_MGlobal/udpmsgs.cpp \
        ../MPD78097_Database/mdatabase.cpp \
        ../MPD78097_Database/msqlabstract.cpp \
        ../MPD78097_Database/msqlanalysistypes.cpp \
        ../MPD78097_Database/msqlarrangana.cpp \
        ../MPD78097_Database/msqlarrangements.cpp \
        ../MPD78097_Database/msqlboards.cpp \
        ../MPD78097_Database/msqlcategories.cpp \
        ../MPD78097_Database/msqlcomments.cpp \
        ../MPD78097_Database/msqldiagnosiscodes.cpp \
        ../MPD78097_Database/msqlpatientdiagnosiscodes.cpp \
        ../MPD78097_Database/msqlpatients.cpp \
        ../MPD78097_Database/msqlpatientsubcategory.cpp \
        ../MPD78097_Database/msqlresults.cpp \
        ../MPD78097_Database/msqlroutinearrangement.cpp \
        ../MPD78097_Database/msqlsubcategories.cpp \
        ../MPD78097_Database/msqltests.cpp

    HEADERS += \
    ../MPD78097_Database/msqloperators.h \
    ../MPD78097_Database/msqlsenders.h \
    CPP/TCP/SimpleTCPClientAcq.h \
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
        CPP/TCP/TcpServers.h \
        CPP/TCP/TcpSettingFile.h \
        CPP/bitmapsv.h \
        CPP/graficmanager.h \
        ../MPD78096_AnaUro/anauro.h\
        ../MPD78096_AnaUro/anauro_global.h \
        ../MPD78096_AnaUro/analysis.h \
        ../MPD78096_AnaUro/Anaerrorcodes.h \
        ../MPD78096_AnaUro/anautils.h \
        ../M8078027_MGlobal/ancestry.h \
        ../M8078027_MGlobal/appbridge.h \
        ../M8078027_MGlobal/global.h \
        ../M8078027_MGlobal/layoutmanager.h \
        ../M8078027_MGlobal/msignal.h \
        ../M8078027_MGlobal/p7settingsmanager.h \
        ../M8078027_MGlobal/systemmanager.h \
        ../M8078027_MGlobal/UdmImpl.h \
        ../M8078027_MGlobal/udpmsgs.h \
        ../M8078027_MGlobal/fileio.h \
        ../MPD78097_Database/dbStatements.h \
        ../MPD78097_Database/mdatabase.h \
        ../MPD78097_Database/msqlabstract.h \
        ../MPD78097_Database/msqlanalysistypes.h \
        ../MPD78097_Database/msqlarrangana.h \
        ../MPD78097_Database/msqlarrangements.h \
        ../MPD78097_Database/msqlboards.h \
        ../MPD78097_Database/msqlcategories.h \
        ../MPD78097_Database/msqlcomments.h \
        ../MPD78097_Database/msqldiagnosiscodes.h \
        ../MPD78097_Database/msqlpatientdiagnosiscodes.h \
        ../MPD78097_Database/msqlpatients.h \
        ../MPD78097_Database/msqlpatientsubcategory.h \
        ../MPD78097_Database/msqlresults.h \
        ../MPD78097_Database/msqlroutinearrangement.h \
        ../MPD78097_Database/msqlsubcategories.h \
        ../MPD78097_Database/msqltests.h


    INCLUDEPATH +=  CPP \
                    CPP/TCP \
                    ../MPD78096_AnaUro \
                    ../M8078027_MGlobal \
                    ../MPD78097_Database \
                    ../MPF78003_Picoflow2R3Supe \
                    ../MPD78098_DataFileManager \
                    ../SupeFlowBT

DISTFILES += \
    ../M8078027_MGlobal/MComponents/* \
    ../M8078027_MGlobal/MComponents/Images/*



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

    QT+=axcontainer

    SOURCES += CPP/mdatamngdesktop.cpp \

    HEADERS += CPP/mdatamngdesktop.h \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../M8078027_MGlobal \

    INCLUDEPATH +=  ../MPD78099_MedicalReport

    rootPath = E:\Piattaforma70
    #creo la cartella da copiare in giro
    #plugin.path = $${rootPath}\Lavoro\Software\Build\StandAlone
    #claudia
    plugin.path = $${rootPath}\build\standalone
    plugin.files += $$shell_path($$OUT_PWD)\release\acqTool.exe
    INSTALLS += plugin

    QT += printsupport
}

macx {
    DEFINES += MAC
    TARGET = acqTool

    SOURCES += CPP/mdatamngdesktop.cpp \

    HEADERS += CPP/mdatamngdesktop.h \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../M8078027_MGlobal \

    INCLUDEPATH +=  ../MPD78099_MedicalReport

    QT += printsupport
}

DINAMICO {
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/release/ -lDatafileManager
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DatafileManager/debug/ -lDatafileManager
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/MedicalReport/release/ -lMedicalReport
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/MedicalReport/debug/ -lMedicalReport

    macx:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/Release/DatafileManager/ -lDatafileManager
    else:macx:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/Debug/DatafileManager/ -lDatafileManager
    macx:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/Release/MedicalReport/ -lMedicalReport
    else:macx:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/Debug/MedicalReport/ -lMedicalReport
}

STATICO {
    RESOURCES += modules_picoflow.qrc
    win32:LIBS += -L$$PWD/../BuildStatic/DatafileManager/release/ -lDatafileManager
    win32:LIBS += -L$$PWD/../BuildStatic/Medicalreport/release/ -lMedicalReport
    win32:LIBS += -L$$PWD/../BuildStatic/QmlPlotter/release/ -lQmlPlotter

    macx:LIBS += -L$$PWD/../BuildStatic/DatafileManager/ -lDatafileManager
    macx:LIBS += -L$$PWD/../BuildStatic/Medicalreport/ -lMedicalReport
    macx:LIBS += -L$$PWD/../BuildStatic/QmlPlotter/ -lQmlPlotter
    DEFINES += STATICO
}

unix {
    DEFINES += TAR
}

#tastiera virtuale
CONFIG += link_pkgconfig



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

}

CLAUDIA {
    QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += PICOFLOW
    TARGET = PicoAcq

    SOURCES += \
        CPP/mdatamngpico.cpp

    HEADERS += \
        CPP/mdatamngpico.h

    RESOURCES += modules_picoflow.qrc

    LIBS        += -L/sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager/ -lDatafileManager
    INCLUDEPATH +=   /sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager
    DEPENDPATH  +=   /sviluppo/qt/Piattaforma70/Build-IMX6/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../M8078027_MGlobal \
                      /sviluppo/qt/Piattaforma70/Build-IMX6/CommonPlugin

    DEFINES += nullptr=NULL


}

PICOFLOW {
    QMAKE_CXXFLAGS += -Wno-psabi
    DEFINES += PICOFLOW
    TARGET = PicoAcq

    SOURCES += \
#        ../MGlobal/udpmsgs.cpp \
        CPP/mdatamngpico.cpp

    HEADERS += \
#        ../MGlobal/udpmsgs.h \
        CPP/mdatamngpico.h

    RESOURCES += modules_picoflow.qrc

    LIBS        += -L$$PWD/../Build-IMX6/DatafileManager/ -lDatafileManager
    INCLUDEPATH +=   $$PWD/../Build-IMX6/DatafileManager
    DEPENDPATH  +=   $$PWD/../Build-IMX6/DatafileManager

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = ../M8078027_MGlobal \
                      ../Build-IMX6/CommonPlugin


    DEFINES += nullptr=NULL


}








