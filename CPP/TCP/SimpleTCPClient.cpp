#include "SimpleTCPClient.h"

SimpleTCPClient::SimpleTCPClient(QHostAddress __address, int __port, QObject *__pParent) :
    QObject(__pParent)
{
    m_hostAddress = __address;
    m_hostPort = __port;

    m_channelTcpSocket = new QTcpSocket();
    m_channelInUse = false;

    m_pFDataReadyCallBack = NULL;
    connect(m_channelTcpSocket, SIGNAL(readyRead()), this, SLOT(onDataReady()), Qt::DirectConnection);
    connect(m_channelTcpSocket, SIGNAL(connected()), this, SLOT(onConnectedToHost()));
    connect(m_channelTcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnectedFromHost()));
    connect(m_channelTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onTcpError(QAbstractSocket::SocketError)));
}

SimpleTCPClient::~SimpleTCPClient()
{
    if(m_channelInUse)
        m_channelTcpSocket->close();

    delete m_channelTcpSocket;
}

void SimpleTCPClient::sendData(char* msg, quint32 len){
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
    out<<len;
    for(quint32 i = 0;i<len;i++){
        block.append(msg[i]);
    }

    if(m_channelInUse){
        m_channelTcpSocket->write(block);
        m_channelTcpSocket->flush();
    }
}


bool SimpleTCPClient::setHostAddress(QHostAddress __address)
{
    if(m_channelInUse)
        return false;
    m_hostAddress = __address;
    return true;
}

bool SimpleTCPClient::setHostPort(int __port)
{
    if(m_channelInUse)
        return false;
    m_hostPort = __port;
    return true;
}

bool SimpleTCPClient::connectToHost()
{
    bool ret = false;
    if( ! m_channelInUse) {
        m_channelInUse = true;
        m_channelTcpSocket->connectToHost(m_hostAddress, m_hostPort);
        ret = true;
    }
    qDebug() << m_hostAddress << m_hostPort << "ret:" << ret;
    return ret;
}

bool SimpleTCPClient::connectToHost(QHostAddress __address, int __port)
{
    if(m_channelInUse)
        return false;
    m_hostAddress = __address;
    m_hostPort = __port;
    m_channelInUse = true;
    m_channelTcpSocket->connectToHost(m_hostAddress, m_hostPort);
    return true;
}

bool SimpleTCPClient::disconnectToHost()
{
    if(!m_channelInUse)
        return false;
    m_channelTcpSocket->disconnectFromHost();
    return true;
}

void SimpleTCPClient::onTcpError(QAbstractSocket::SocketError __error)
{
    qDebug() <<"Address: "<<m_hostAddress.toString()<<" Port: "<<m_hostPort<<" Error: "<<m_channelTcpSocket->errorString()<<__error;
    switch (__error)
    {
        case QAbstractSocket::ConnectionRefusedError://	The connection was refused by the peer (or timed out).
        case QAbstractSocket::RemoteHostClosedError://	The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.
        case QAbstractSocket::HostNotFoundError://	The host address was not found.
        case QAbstractSocket::SocketAccessError://	The socket operation failed because the application lacked the required privileges.
        case QAbstractSocket::SocketResourceError://	The local system ran out of resources (e.g., too many sockets).
        case QAbstractSocket::SocketTimeoutError://	The socket operation timed out.
        case QAbstractSocket::DatagramTooLargeError://	The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).
        case QAbstractSocket::NetworkError://	An error occurred with the network (e.g., the network cable was accidentally plugged out).
        case QAbstractSocket::AddressInUseError://	The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.
        case QAbstractSocket::SocketAddressNotAvailableError://	The address specified to QAbstractSocket::bind() does not belong to the host.
        case QAbstractSocket::UnsupportedSocketOperationError://	The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).
        case QAbstractSocket::ProxyAuthenticationRequiredError://	The socket is using a proxy, and the proxy requires authentication.
        case QAbstractSocket::SslHandshakeFailedError://	The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)
        case QAbstractSocket::UnfinishedSocketOperationError://	Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).
        case QAbstractSocket::ProxyConnectionRefusedError://	Could not contact the proxy server because the connection to that server was denied
        case QAbstractSocket::ProxyConnectionClosedError://	The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)
        case QAbstractSocket::ProxyConnectionTimeoutError://	The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.
        case QAbstractSocket::ProxyNotFoundError://	The proxy address set with setProxy() (or the application proxy) was not found.
        case QAbstractSocket::ProxyProtocolError://	The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.
        case QAbstractSocket::OperationError://	An operation was attempted while the socket was in a state that did not permit it.
        case QAbstractSocket::SslInternalError://	The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.
        case QAbstractSocket::SslInvalidUserDataError://	Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.
        case QAbstractSocket::TemporaryError://	A temporary error occurred (e.g., operation would block and socket is non-blocking).
        case QAbstractSocket::UnknownSocketError://
              m_channelInUse = false;
            break;
    }
    emit tcpError(__error);
}

void SimpleTCPClient::onConnectedToHost()
{
    emit connectedToHost();
}

void SimpleTCPClient::onDisconnectedFromHost()
{
    m_channelInUse = false;
    emit disconnectFromHost();
}


void SimpleTCPClient::onDataReady()
{
    if(m_pFDataReadyCallBack != NULL)
    {
        QByteArray block;
        block = m_channelTcpSocket->readAll();
        m_pFDataReadyCallBack(this->parent(), this, block);
    }
    emit dataReady();
}



