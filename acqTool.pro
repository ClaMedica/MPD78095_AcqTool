TEMPLATE = app
TARGET = acqTool
QT += qml quick widgets sql network multimedia xml core serialport

RESOURCES += Resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=  ../build/CommonPlugin\
                    ../AcqTool \

DEFINES += QT_MESSAGELOGCONTEXT
#debug campioni di acquisizione
DEFINES += DEBUG_ACQ

win32:DEFINES+= WIN32
unix:DEFINES+= LINUX


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
    CPP/nomogramma.cpp \
    CPP/printermanager.cpp \
    CPP/printerserialport.cpp \
    ../MGlobal/UdMImpl.cpp


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
    CPP/nomogramma.h \
    CPP/printermanager.h \
    CPP/printerserialport.h \
    ../MGlobal/UDMImpl.h \

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
                ../DataFileManager \
                CPP/TCP \
                ../SuperFlowBT



# Default rules for deployment.
include(deployment.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/AnaUro7Build/release/ -lAnaUro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/AnaUro7Build/debug/ -lAnaUro


contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
DEFINES += ANDROID
QT += androidextras
    ANDROID_EXTRA_LIBS = \
        $$PWD/../Build/Emulator/DataBuild/libDatafileManager.so \
        $$PWD/../Build/Emulator/MPlotModule/libQmlPlotter.so

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

unix:!macx: LIBS += -L$$PWD/../Build/Emulator/DataBuild/ -lDatafileManager
}

#percorso tecnoideal
#rootPath = M:
#percorso acer luca
#rootPath = C:\Users\Mez
#percorso claudia
win32:rootPath = E:\Piattaforma_70
unix:rootPath = /media/user/Dati/Piattaforma_70

win32{
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

unix:!macx: LIBS += -L$$PWD/../build/DataBuild/ -lDatafileManager
unix:!macx: LIBS += -L$$PWD/../build/AnaUro/ -lAnaUro

unix:INCLUDEPATH += $$PWD/../build/DataBuild
unix:DEPENDPATH += $$PWD/../build/DataBuild


#tastiera virtuale
CONFIG += link_pkgconfig

#linux-buildroot-g++ {
#    deployment.files = *.qml *.otf *.ttf *.svg
#    target.path = /vktest
#    deployment.path = /vktest
#    INSTALLS += target deployment
#} else {
    target.path = $$[QT_INSTALL_PLUGINS]/virtualkeyboard
    INSTALLS += target
#}
