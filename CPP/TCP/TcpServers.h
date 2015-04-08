#ifndef TCPSERVERS_H
#define TCPSERVERS_H

#include "SimpleTCPChannel.h"

class TcpServers : public QObject{
    Q_OBJECT

    Q_PROPERTY (int numTCPservers READ numTCPservers WRITE setServerNum NOTIFY serverChanged)
    Q_PROPERTY (QStringList serverPort READ serverPort WRITE setServerPort NOTIFY serverChanged)
    Q_PROPERTY (QStringList serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverChanged)
    Q_PROPERTY (QStringList serverStatus READ serverStatus NOTIFY serverStatusChanged)
    Q_PROPERTY (QStringList serverError READ serverError NOTIFY serverErrorChanged)
    Q_PROPERTY (bool tcpEnable READ tcpEnable WRITE setTcpEnable NOTIFY tcpEnableChanged)

public:
    explicit TcpServers(QObject *__pParent = 0);
    ~TcpServers();

    int numTCPservers(){return m_numTCPservers;}
    QStringList serverPort(){return m_serverPort;}
    QStringList serverAddress(){return m_serverAddress;}
    QStringList serverStatus(){return m_serverStatus;}
    QStringList serverError(){return m_serverError;}
    bool tcpEnable(){return m_tcpEnable;}

    Q_INVOKABLE void updateTcpInfo();
    Q_INVOKABLE void setTCP();
    Q_INVOKABLE void enableTCP();
    SimpleTCPChannel *getServer(int __i);
    Q_INVOKABLE qint64 getPointer(){return ((qint64)this);}

signals:
    void serverChanged();
    void serverStatusChanged();
    void serverErrorChanged();
    void tcpEnableChanged();

public slots:
    void setServerNum(int __num);
    void setServerPort(QStringList __port);
    void setServerAddress(QStringList __address);
    void setTcpEnable(bool __en);

protected:
    void createServers(int __num);

    QString socketStateToStr(QAbstractSocket::SocketState __state);
    void setServerStatus(int __num, QString __state);
    void setServerError(int __num, QString __error);

    SimpleTCPChannel **m_pTCPServer;
    int m_numTCPservers;
    QStringList m_serverPort;
    QStringList m_serverAddress;
    QStringList m_serverStatus;
    QStringList m_serverError;
    bool m_tcpEnable;

};

#endif // TCPSERVERS_H
