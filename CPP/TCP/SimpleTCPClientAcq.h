#ifndef SIMPLETCPCLIENTACQ_H
#define SIMPLETCPCLIENTACQ_H

#include <QtMessageHandler>
#include <QObject>
#include <QtNetwork>

class SimpleTCPClientAcq : public QObject
{
    Q_OBJECT

public:
    explicit SimpleTCPClientAcq(QHostAddress __address = QHostAddress::LocalHost, int __currentPort = 2000, QObject *__pParent = 0);
    ~SimpleTCPClientAcq();

    QHostAddress hostAddress(){return m_hostAddress;}
    int hostPort(){return m_hostPort;}

    void sendData(char* msg, quint32 len);
    bool setHostAddress(QHostAddress __address);
    bool setHostPort(int __port);
    bool connectToHost();
    bool connectToHost(QHostAddress __address, int __port);
    bool disconnectToHost();

    bool waitForConnected(int __timeout = 30000){return m_channelTcpSocket->waitForConnected(__timeout);}
    bool waitForDisconnected(int __timeout = 30000){return m_channelTcpSocket->waitForDisconnected(__timeout);}

    QTcpSocket::SocketState getSocketState(){return m_channelTcpSocket->state();}
    QTcpSocket::SocketError getSocketError(){return m_channelTcpSocket->error();}
    bool socketInUsed(){return m_channelInUse;}

    void registerDataReadyCallBack(void (*__pCallBAck)(QObject *__pParent, SimpleTCPClientAcq *__pTCP, QByteArray __blocco)){m_pFDataReadyCallBack = __pCallBAck;}

signals:
    void connectedToHost();
    void disconnectFromHost();
    void dataReady();
    void tcpError(QAbstractSocket::SocketError);

public slots:

protected slots:
    virtual void onDataReady();
    virtual void onTcpError(QAbstractSocket::SocketError);
    virtual void onConnectedToHost();
    virtual void onDisconnectedFromHost();

protected:
    //single socket of interest
    QTcpSocket *m_channelTcpSocket;

    QHostAddress m_hostAddress;
    int m_hostPort;
    bool m_channelInUse;

    QObject *m_pParent;
    void (*m_pFDataReadyCallBack)(QObject *__pParent, SimpleTCPClientAcq *__pTCP, QByteArray __blocco);
};

#endif
