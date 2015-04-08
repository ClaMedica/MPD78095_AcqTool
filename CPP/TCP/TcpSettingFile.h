#ifndef TCPSETTINGFILE_H
#define TCPSETTINGFILE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QString>
#include <QHostAddress>
#include <QVector>
#include <QCoreApplication>

#define FILE_TCP_SETTING "TcpSetting.txt"

class TcpSettingFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY (int num READ num WRITE setNum NOTIFY settingsChanged)
    Q_PROPERTY (QStringList name READ name NOTIFY settingsChanged)
    Q_PROPERTY (QStringList serverPort READ serverPort NOTIFY settingsChanged)
    Q_PROPERTY (QStringList serverAddress READ serverAddress NOTIFY settingsChanged)

public:
    struct tcpSetting_t{
        QString name;
        QHostAddress address;
        int port;

        tcpSetting_t(){this->clr();}
        tcpSetting_t(const tcpSetting_t &__setting){*this = __setting;}
        tcpSetting_t& operator= (const tcpSetting_t &__setting)
        {
            this->name = __setting.name;
            this->address = __setting.address;
            this->port = __setting.port;
            return *this;
        }
        void clr()
        {
            this->name = "unknown";
            this->address = QHostAddress::LocalHost;
            this->port = 2000;
        }
    };

    explicit TcpSettingFile(QString __fileName = FILE_TCP_SETTING, QObject *parent = 0);
    ~TcpSettingFile();

    QString fileName(){return m_fileName;}
    int num(){return m_num;}

    QStringList serverPort(){return m_serverPort;}
    QStringList serverAddress(){return m_serverAddress;}
    QStringList name(){return m_serverName;}

    Q_INVOKABLE int setSettings();
    Q_INVOKABLE int settings();

    Q_INVOKABLE void setName(QString __name, int __i);
    Q_INVOKABLE void setAddress(QString __address, int __i);
    Q_INVOKABLE void setAddress(QHostAddress __address, int __i);
    Q_INVOKABLE void setPort(int __port, int __i);

    Q_INVOKABLE QList<tcpSetting_t> tcpList(){return m_settings;}

signals:
    void fileNameChanged();
    void settingsChanged();

public slots:
    void setFileName(QString __fileName);
    void setNum(int __num);

protected:

    void createStringList();

    QFile m_settingFile;
    QString m_fileName;
    QList<tcpSetting_t> m_settings;
    QStringList m_serverName;
    QStringList m_serverPort;
    QStringList m_serverAddress;

    int m_num;
};

#endif // TCPSETTINGFILE_H



















