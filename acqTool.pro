TEMPLATE = app

QT += qml quick widgets sql multimedia network

RESOURCES += qml.qrc



# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=   ../Build/CommonPlugin \
                    ../UsefulOBJ \
                    ../UsefulOBJ/MComponents/images.js\
                    ../UsefulOBJ/MComponents/Models\
                    QML

QT +=multimedia
QT +=xml
QT +=core


DEFINES += QT_MESSAGELOGCONTEXT
# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    CPP/maudio.cpp \
    CPP/mplayer.cpp \
    CPP/parametermanager.cpp \
    CPP/tabble.cpp \
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
    CPP/maudio.h \
    CPP/mplayer.h \
    CPP/parametermanager.h \
    CPP/tabble.h \
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



OTHER_FILES += \
    QML/MyForms/AnalysisForm.qml \
    QML/MyForms/RealTimeForm.qml \
    QML/MyForms/HomeForm.qml \
    ../../UsefulOBJ/MComponents/MButton.qml \
    ../../UsefulOBJ/MComponents/MMenu.qml \
    ../../UsefulOBJ/MComponents/MMenuBar.qml \
    ../../UsefulOBJ/MComponents/MMenuButton.qml \
    ../../UsefulOBJ/MComponents/MSubMenu.qml \
    ../../UsefulOBJ/MComponents/MForm.qml \
    QML/MTools/MDialog.qml

INCLUDEPATH += CPP\
                ../MGlobal \
                ../Source/Libcpp \
                ../Source/Plot \
                ../Source/Plot/Tools \
                ../Source/QML \
                ../Source/QML/Models \
                ../Source/QML/Tools \
                ../DataFileManager \
                CPP/TCP \
                ../CellaBluetooth/Supervisor

# Default rules for deployment.
include(deployment.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Build/DataBuild/release/ -lDatafileManager1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Build/DataBuild/debug/ -lDatafileManager1

INCLUDEPATH += $$PWD/../Build/DataBuild/release
DEPENDPATH += $$PWD/../Build/DataBuild/release

DISTFILES += \
    ../UsefulOBJ/MComponents/MDialog.qml \
    ../UsefulOBJ/MComponents/MGridView.qml \
    ../UsefulOBJ/MComponents/MListSelector.qml \
    ../UsefulOBJ/MComponents/MMessageYesNo.qml \
    ../UsefulOBJ/MComponents/MNewName.qml \
    ../UsefulOBJ/MComponents/MPopUp.qml \
    ../UsefulOBJ/MComponents/Parameter.qml \
    ../UsefulOBJ/MComponents/ParameterBox.qml \
    ../UsefulOBJ/MComponents/Player.qml \
    ../UsefulOBJ/MComponents/MAlarmBox.qml \
    ../UsefulOBJ/MComponents/func.js \
    ../UsefulOBJ/MComponents/images.js \
    ../UsefulOBJ/MComponents/Models/AlarmModel.qml \
    ../UsefulOBJ/MComponents/MAlarm.qml \
    QML/MForms/AnalysisForm.qml \
    QML/MForms/HomeForm.qml \
    QML/MForms/RealTimeForm.qml \
    QML/MForms/SupervisorForm.qml
