
#include "SimpleTCPChannel.h"
#include <iostream>
#include <QString>

//for endianess functions

// DEFINITION OF SimpleTCPChannel class - begin //
SimpleTCPChannel::SimpleTCPChannel(QHostAddress __address, int __currentPort, QObject *__pParent):QObject()
{

    // Initializing socket
    m_channelTcpSocket = NULL;
    m_channelInUse = false;

    m_pParent = __pParent;
    // Setting tcp_server UP
    m_channelTcpServer = new QTcpServer(this);

    // To be defined, for example, in a config file.
    m_address = __address;
    m_port = __currentPort;

    m_pFDataReadyCallBack = NULL;
    connect(m_channelTcpServer, SIGNAL(newConnection()), this, SLOT(manageIncomingConnection()), Qt::DirectConnection);
}

SimpleTCPChannel::~SimpleTCPChannel(){

    //Closing TCP socket to client, only if it has been opened and it's not
    // already in UnconnectedState
    if(this->m_channelInUse)
        m_channelTcpSocket->close();

    //closing TCP server for current channel.
    m_channelTcpServer->close();
    delete m_channelTcpServer;
}

void SimpleTCPChannel::listen()
{
    if(this->m_channelInUse)
    {
        qDebug("SimpleTCPChannel: Channel in use can not perform listen operation");
        return;
    }
    if(m_channelTcpServer->listen(m_address , m_port))
    {
        qDebug("SimpleTCPChannel: Server listening on address: %s and port: %d",m_address.toString().toUtf8().data(),m_port);
    }else
    {
        qDebug("SimpleTCPChannel: Problems on listen operation. address: %s and port: %d",m_address.toString().toUtf8().data(),m_port);
    }
}

void  SimpleTCPChannel::close()
{
    if(this->m_channelInUse)
        m_channelTcpSocket->close();

    //closing TCP server for current channel.
     m_channelTcpServer->close();
     m_channelInUse = false;
     qDebug("SimpleTCPChannel: close");
}
/**
 * It sends given message. Dimension of msg is put at the head of stream, in a
 * quint32 variable.
 */
bool SimpleTCPChannel::sendData(char* __msg, quint32 __len){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    //writing size, and content 'char by char'

    //////////
    // ATTENTION: usage of QDataStream allows us to write data directly in big-endian order
    // (that is network byte order). For this reason no conversion using htonl() function
    // is performed here (and no conversion is necessary in a Qt client using a QDataStream
    // with byte order QDataStream::BigEndian neither). By the way all non-Qt clients must
    // be aware that data sent are in network byte order.
    //////////
    out<<__len;
    for(quint32 i = 0;i<__len;i++){
        block.append(__msg[i]);
    }

    if(this->m_channelInUse){
        m_channelTcpSocket->write(block);
        m_channelTcpSocket->flush();
        return true;
    }
    else
        return false;
}

void SimpleTCPChannel::sendData(QByteArray *__pData){
    //writing size, and content 'char by char'

    //////////
    // ATTENTION: usage of QDataStream allows us to write data directly in big-endian order
    // (that is network byte order). For this reason no conversion using htonl() function
    // is performed here (and no conversion is necessary in a Qt client using a QDataStream
    // with byte order QDataStream::BigEndian neither). By the way all non-Qt clients must
    // be aware that data sent are in network byte order.
    //////////
    //quint32 len = __pData->length();
    //__pData->insert(0, QByteArray((char*)&len, sizeof(quint32)));
    if(this->m_channelInUse){
        m_channelTcpSocket->write(*__pData);
        m_channelTcpSocket->flush();
    }
}

QTcpServer* SimpleTCPChannel::getQTcpServer(){
    return this->m_channelTcpServer;
}


QTcpSocket* SimpleTCPChannel::getQTcpSocket(){
    return this->m_channelTcpSocket;
}

void SimpleTCPChannel::setServerAddress(QHostAddress __address)
{
    m_address = __address;
}

void SimpleTCPChannel::setServerAddress(QString __address)
{
    m_address = QHostAddress(__address);
}

void SimpleTCPChannel::setServerPort(int __port)
{
    m_port = __port;
}

// slots

void SimpleTCPChannel::manageIncomingConnection(){
    QTcpSocket *clientConnection = m_channelTcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

    qDebug("SimpleTCPChannel: Incoming connection to manage. Address: %s - port: %d",
           m_channelTcpServer->serverAddress().toString().toUtf8().data(),m_channelTcpServer->serverPort());
    fflush(stdout);
    if(!m_channelInUse){
        //establishing new connection
        m_channelTcpSocket = clientConnection;
        connect(m_channelTcpSocket, SIGNAL(disconnected()), this, SLOT(managechannelOneDisconnection()));
        connect(m_channelTcpSocket, SIGNAL(readyRead()), this, SLOT(onDataReady()), Qt::DirectConnection);
        m_channelInUse=true;
        qDebug() << "SimpleTCPChannel: New connection established";
        fflush(stdout);

        emit signal_clientConnected();
    }else{
        //connection for current channel is already established.
        // further request are just ignored.
        clientConnection->disconnectFromHost();
        qDebug() << "SimpleTCPChannel: Port already binded. This request is ignored.";
        fflush(stdout);
    }
}

void SimpleTCPChannel::managechannelOneDisconnection(){
    if(this->m_channelInUse){
        this->m_channelInUse=false;
        disconnect(m_channelTcpSocket, SIGNAL(readyRead()), this, SLOT(onDataReady()));
        m_channelTcpSocket = NULL;
        emit signal_clientDisconnected();
    }
}

void SimpleTCPChannel::onDataReady()
{
    if(m_pFDataReadyCallBack != NULL)
    {
        QByteArray block;
        block = m_channelTcpSocket->readAll();
        m_pFDataReadyCallBack(m_pParent, this, block);
    }
}

// DEFINITION OF SimpleTCPChannel class - end //
