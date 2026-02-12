#include "TcpServers.h"

TcpServers::TcpServers(QObject *__pParent):
    QObject(__pParent)
{
    m_pTCPServer = NULL;
    m_numTCPservers = 0;
    m_serverPort.clear();
    m_serverAddress.clear();
    m_serverStatus.clear();
    m_serverError.clear();
}

TcpServers::~TcpServers()
{
    if(m_pTCPServer != NULL)
    {
        for (int i  = 0; i < m_numTCPservers; i++)
        {
            if(m_pTCPServer[i] != NULL) {
                delete m_pTCPServer[i];
                m_pTCPServer[i] = NULL;
            }
        }
        delete m_pTCPServer;
        m_pTCPServer = NULL;
    }
}

void TcpServers::updateTcpInfo()
{
    if(m_pTCPServer != NULL)
    {
        for(int i = 0; i < m_numTCPservers; i++)
        {
            if(m_pTCPServer[i] != NULL)
            {
                if(m_pTCPServer[i]->getQTcpSocket() != NULL)
                {
                    setServerStatus(i, socketStateToStr(m_pTCPServer[i]->getQTcpSocket()->state()));
                }else
                {
                    if(m_pTCPServer[i]->getQTcpServer()->isListening())
                        setServerStatus(i, "Listening");
                    else
                        setServerStatus(i, "???");
                }
                setServerError(i, m_pTCPServer[i]->getQTcpServer()->errorString());
            }
        }
    }
}

SimpleTCPChannel* TcpServers::getServer(int __i)
{
    if(m_pTCPServer == NULL)
        return NULL;
    if(__i >= m_numTCPservers)
        return NULL;
    return m_pTCPServer[__i];
}

void TcpServers::createServers(int __num)
{
    int i;

    m_numTCPservers = __num;

    m_serverPort.clear();
    m_serverAddress.clear();
    m_serverStatus.clear();
    m_serverError.clear();
    if(m_pTCPServer != NULL)
    {
        for (i  = 0; i < m_numTCPservers; i++)
        {
            if(m_pTCPServer[i] != NULL) {
                delete m_pTCPServer[i];
                m_pTCPServer[i] = NULL;
            }
        }
        delete m_pTCPServer;
        m_pTCPServer = NULL;
    }

    if(m_numTCPservers != 0)
    {
        m_pTCPServer = new SimpleTCPChannel*[m_numTCPservers];
       for (i  = 0; i < m_numTCPservers; i++)
       {
            m_pTCPServer[i] = new SimpleTCPChannel(QHostAddress::LocalHost, 7600+i, this);
            m_serverAddress.append(QHostAddress(QHostAddress::LocalHost).toString());
            m_serverPort.append(QString().sprintf("%d", 7600+i));
            m_serverStatus.append(QString(""));
            m_serverError.append(QString(""));
       }
    }
    emit serverChanged();
    emit serverStatusChanged();
    emit serverErrorChanged();
}

void TcpServers::setServerNum(int __num)
{
    createServers(__num);
}

void TcpServers::setServerPort(QStringList __port)
{
    int i;
    bool changed = false;

    for (i = 0; i < m_numTCPservers; i++)
    {
        if(__port.length() > i && m_serverPort.length() > i)
        {
            if(__port.at(i) !=  m_serverPort.at(i))
            {
                m_serverPort.replace(i, __port.at(i));
                changed = true;
            }

        }
    }
    if(changed)
        setTCP();
}

void TcpServers::setServerAddress(QStringList __address)
{
    int i;
    bool changed = false;

    for (i = 0; i < m_numTCPservers; i++)
    {
        if(__address.length() > i && m_serverAddress.length() > i)
            if(__address.at(i) !=  m_serverAddress.at(i))
            {
                m_serverAddress.replace(i, __address.at(i));
                changed = true;
            }
    }
    if(changed)
        setTCP();
}

void TcpServers::setTcpEnable(bool __en)
{
    if(m_pTCPServer == NULL)
        return;
    if(__en != m_tcpEnable)
    {
        m_tcpEnable = __en;
        enableTCP();
    }
}

void TcpServers::setTCP()
{
    int i;
    bool changed = false;

    if(m_pTCPServer == NULL)
        return;

    for (i = 0; i < m_numTCPservers; i++)
    {
        if(m_pTCPServer[i] != NULL)
        {
            if(m_pTCPServer[i]->serverPort() !=  m_serverPort.at(i).toInt())
            {
                m_pTCPServer[i]->close();
                m_pTCPServer[i]->setServerPort(m_serverPort.at(i).toInt());
                changed = true;
            }
        }
    }

    for (i = 0; i < m_numTCPservers; i++)
    {
       if(m_pTCPServer[i] != NULL)
        {
            if(m_pTCPServer[i]->serverAddress().toString() !=  m_serverAddress.at(i))
            {
                m_pTCPServer[i]->close();
                m_pTCPServer[i]->setServerAddress(m_serverAddress.at(i));
                changed = true;
            }
        }
    }

    for (i = 0; i < m_numTCPservers; i++)
    {
       if(m_pTCPServer[i] != NULL)
       {
           if(!m_pTCPServer[i]->getQTcpServer()->isListening())
                m_pTCPServer[i]->listen();
       }
    }

    if(changed)
    {
        emit serverChanged();
        emit tcpEnableChanged();
    }
}

void TcpServers::enableTCP()
{
    int i;

    if(m_tcpEnable)
    {
        for (i = 0; i < m_numTCPservers; i++)
            if(m_pTCPServer[i] != NULL)
                m_pTCPServer[i]->listen();
    }else
    {
        for (i = 0; i < m_numTCPservers; i++)
            if(m_pTCPServer[i] != NULL)
                m_pTCPServer[i]->close();
    }
    emit tcpEnableChanged();
}

void TcpServers::setServerStatus(int __num, QString __state)
{
    if(__num >= m_numTCPservers)
        return;
    if(__num >= m_serverStatus.length())
        return;
    if(__state != m_serverStatus.at(__num))
    {
        m_serverStatus.replace(__num, __state);
        emit serverStatusChanged();
    }
}

void TcpServers::setServerError(int __num, QString __error)
{
    if(__num >= m_numTCPservers)
        return;
    if(__num >= m_serverError.length())
        return;
    if(__error != m_serverError.at(__num))
    {
        m_serverError.replace(__num, __error);
        emit serverErrorChanged();
    }
}

QString TcpServers::socketStateToStr(QAbstractSocket::SocketState __state)
{
    switch(__state)
    {
        case QAbstractSocket::UnconnectedState: return "UNCONNECTED";
        case QAbstractSocket::HostLookupState: return "HOST LOOKUP";
        case QAbstractSocket::ConnectingState: return "CONNECTING";
        case QAbstractSocket::ConnectedState: return "CONNECTED";
        case QAbstractSocket::BoundState: return "BOUND";
        case QAbstractSocket::ClosingState: return "CLOSING";
        case QAbstractSocket::ListeningState: return "LISTENING";
    }
    return "UNKNOWN";
}



































