TEMPLATE = app
TARGET = acqTool
QT += qml quick widgets sql network multimedia xml core

RESOURCES += Resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=  ../Build/CommonPlugin\
                    ../AcqTool \

DEFINES += QT_MESSAGELOGCONTEXT
#debug campioni di acquisizione
DEFINES += DEBUG_ACQ
# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    CPP/parametermanager.cpp \
    ../MGlobal/msignal.cpp \
    ../MGlobal/global.cpp \
    CPP/modelmanager.cpp \
    CPP/TCP/SimpleTCPChannel.cpp \
    CPP/TCP/SimpleTCPClient.cpp \
    CPP/TCP/TcpServers.cpp \
    CPP/TCP/TcpSettingFile.cpp \
    ../MGlobal/ancestry.cpp \
    CPP/macqmanager.cpp \
    CPP/mdatamanager.cpp \
    CPP/mstorage.cpp \
    CPP/alarmmanager.cpp \
    ../MGlobal/p7settingsmanager.cpp \
    CPP/mabstractmanager.cpp \
    CPP/native.cpp \
    CPP/androidmanager.cpp \
    CPP/mflowdatas.cpp \
    ../MGlobal/MyMessageOutput.cpp \
    CPP/manatablemodel.cpp \
    CPP/nomogramma.cpp

HEADERS += \
    CPP/parametermanager.h \
    ../MGlobal/global.h \
    ../MGlobal/msignal.h \
    CPP/modelmanager.h \
    CPP/TCP/SimpleTCPChannel.h \
    CPP/TCP/SimpleTCPClient.h \
    CPP/TCP/TcpServers.h \
    CPP/TCP/TcpSettingFile.h \
    ../MGlobal/ancestry.h \
    CPP/macqmanager.h \
    CPP/mdatamanager.h \
    CPP/mstorage.h \
    CPP/alarmmanager.h \
    ../MGlobal/p7settingsmanager.h \
    CPP/mabstractmanager.h \
    CPP/androidmanager.h \
    CPP/mflowdatas.h \
    CPP/manatablemodel.h \
    CPP/nomogramma.h

SOURCES += ../AnaUro/anauro.cpp \
    ../AnaUro/analysis.cpp \
    ../AnaUro/anautils.cpp \

HEADERS += ../AnaUro/anauro.h\
        ../AnaUro/anauro_global.h \
    ../AnaUro/analysis.h \
    ../AnaUro/Anaerrorcodes.h \
    ../AnaUro/anautils.h \

INCLUDEPATH +=  CPP \
                ../AnaUro \
                ../MGlobal \
                ../SuperFlowBt \
                ../DataFileManager \
                CPP/TCP \
                ../SupeFlowBT



# Default rules for deployment.
include(deployment.pri)



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

unix:!macx: LIBS += -L$$PWD/../Build/Emulator/DataBuild/ -lDatafileManager

#INCLUDEPATH += $$PWD/../Build/Emulator/DataBuild
#DEPENDPATH += $$PWD/../Build/Emulator/DataBuild

}



#percorso tecnoideal
#rootPath = M:
#percorso acer luca
rootPath = C:\Users\Mez
#percorso claudia
rootPath = E:\Piattaforma_70

win32{
    #creo la cartella da copiare in giro
    #plugin.path = $${rootPath}\Lavoro\Software\Build\StandAlone
    #claudia
    plugin.path = $${rootPath}\Build\StandAlone
    plugin.files += $$shell_path($$OUT_PWD)\release\acqTool.exe
    INSTALLS += plugin

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DataBuild/release/ -lDatafileManager1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DataBuild/debug/ -lDatafileManager1

INCLUDEPATH += $$PWD/../Build/DataBuild/release
DEPENDPATH += $$PWD/../Build/DataBuild/release
}

unix:!macx: LIBS += -L$$PWD/../Build/DataFileManager/ -lDatafileManager

INCLUDEPATH += $$PWD/../Build/DataFileManager
DEPENDPATH += $$PWD/../Build/DataFileManager
