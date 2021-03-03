#ifndef SIMPLETCPCLIENT_H
#define SIMPLETCPCLIENT_H

#include <QtMessageHandler>
#include <QObject>
#include <QtNetwork>

class SimpleTCPClient : public QObject
{
    Q_OBJECT

public:
    explicit SimpleTCPClient(QHostAddress __address = QHostAddress::LocalHost, int __currentPort = 2000, QObject *__pParent = 0);
    inline ~SimpleTCPClient();

    QHostAddress hostAddress(){return m_hostAddress;}
    int hostPort(){return m_hostPort;}

    inline void sendData(char* msg, quint32 len);
    inline bool setHostAddress(QHostAddress __address);
    inline bool setHostPort(int __port);
    inline bool connectToHost();
    inline bool connectToHost(QHostAddress __address, int __port);
    bool waitForConnected(int __timeout = 30000){return m_channelTcpSocket->waitForConnected(__timeout);}
    inline bool disconnectToHost();
    bool waitForDisconnected(int __timeout = 30000){return m_channelTcpSocket->waitForDisconnected(__timeout);}

    QTcpSocket::SocketState getSocketState(){return m_channelTcpSocket->state();}
    QTcpSocket::SocketError getSocketError(){return m_channelTcpSocket->error();}
    bool socketInUsed(){return m_channelInUse;}

    void registerDataReadyCallBack(void (*__pCallBAck)(QObject *__pParent, SimpleTCPClient *__pTCP, QByteArray __block)){m_pFDataReadyCallBack = __pCallBAck;}

signals:
    void connectedToHost();
    void disconnectFromHost();
    void dataReady();
    void tcpError(QAbstractSocket::SocketError);

public slots:

protected slots:
    void onDataReady();
    void onTcpError(QAbstractSocket::SocketError);
    void onConnectedToHost();
    void onDisconnectedFromHost();

protected:
    //single socket of interest
    QTcpSocket *m_channelTcpSocket;

    QHostAddress m_hostAddress;
    int m_hostPort;
    bool m_channelInUse;

    QObject *m_pParent;
    void (*m_pFDataReadyCallBack)(QObject *__pParent, SimpleTCPClient *__pTCP, QByteArray __block);
};

#endif // SIMPLETCPCLIENT_H
