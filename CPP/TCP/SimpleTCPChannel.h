/**
 * This module defines a very simple TCP channel, with the follwing features:
 *  - it accepts a single active connection on the port it's listening on.
 *  - it gives basic functions to read and write binary data
 */

#ifndef SIMPLETCPCHANNEL_H
#define SIMPLETCPCHANNEL_H

#include <QtMessageHandler>
#include <QtNetwork>

class SimpleTCPChannel : public QObject {
    Q_OBJECT

public:
    /**
     * Basic constructor. It sets TCP server to be listening on currentPort.
     */
    explicit SimpleTCPChannel(QHostAddress __address = QHostAddress::LocalHost,
                              int __currentPort = 2000,
                              QObject *__pParent = 0);

    ~SimpleTCPChannel();

    /**
     * It sends given data on socket.
     * Length of message is copied at the top of the message, using a qint32 variable.
     *
     * @param msg   data to be sent
     * @param len   length of message.
     */
    bool sendData(char* __pMsg, quint32 __len);
    void sendData(QByteArray *__pData);
    /**
     * it returns TCP-server object used for current channel
     */
    QTcpServer* getQTcpServer();

    /**
     * it returns TCP-socket object used for current channel
     */
    QTcpSocket* getQTcpSocket();

    QHostAddress serverAddress() { return m_address; }
    int serverPort() { return m_port; }

    void setServerAddress(QHostAddress __address);
    void setServerAddress(QString __address);
    void setServerPort(int __port);

    void listen();
    void close();

    void registerDataReadyCallBack(void (*__pCallBAck)(QObject *__pParent, SimpleTCPChannel *__pTCP, QByteArray __block)){m_pFDataReadyCallBack = __pCallBAck;}

public slots:
    /**
     * Slot to manage incoming connection
     */
    void manageIncomingConnection();

    /**
     * Slot invoked for disconnection of client on channel -> it sets corresponding
     * bool status properly
     */
    void managechannelOneDisconnection();

signals:

    /**
     * This signal indicates that a client has connectected to current TCP server, and so a TCP socket
     * is available
     */
    void signal_clientConnected();

    /**
     * This signal indicates that client has connectected from current TCP socket
     */
    void signal_clientDisconnected();

protected slots:
    virtual void onDataReady();

protected:
     void (*m_pFDataReadyCallBack)(QObject *__pParent, SimpleTCPChannel*__pTCP, QByteArray __block);

private:
    //// information for channel - BEGIN ////

    QObject *m_pParent;

    //tcp server
    QTcpServer *m_channelTcpServer;

    //single socket of interest
    QTcpSocket *m_channelTcpSocket;

    //flag to check whether socket of current channel is in use or not.
    // This is used to ignore incoming connection occurred when channel is
    // already in use
    bool m_channelInUse;

    QHostAddress m_address;
    int m_port;
    //// information for channel - END////


};

#endif // SIMPLESERVER_H
