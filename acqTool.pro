TEMPLATE = app
TARGET = acqTool
QT += qml quick widgets sql network

RESOURCES += Resources/qml.qrc




# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=  ../Build/CommonPlugin\
                    ../AcqTool \

QT +=multimedia
QT +=xml
QT +=core

DEFINES += ANDROID
DEFINES += QT_MESSAGELOGCONTEXT
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
    CPP/mabstractmanager.cpp

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
    CPP/mabstractmanager.h

INCLUDEPATH += CPP\
                ../MGlobal \
                ../DataFileManager \
                CPP/TCP \
                ../CellaBluetooth/Supervisor

# Default rules for deployment.
include(deployment.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DataBuild/release/ -lDatafileManager1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DataBuild/debug/ -lDatafileManager1

INCLUDEPATH += $$PWD/../Build/DataBuild/release
DEPENDPATH += $$PWD/../Build/DataBuild/release

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../Build/Emulator/DataBuild/libDatafileManager.so \
        $$PWD/../Build/Emulator/MPlotModule/libQmlPlotter.so
}

unix:!macx: LIBS += -L$$PWD/../Build/Emulator/DataBuild/ -lDatafileManager

INCLUDEPATH += $$PWD/../Build/Emulator/DataBuild
DEPENDPATH += $$PWD/../Build/Emulator/DataBuild
