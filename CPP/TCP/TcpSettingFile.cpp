#include "TcpSettingFile.h"

TcpSettingFile::TcpSettingFile(QString __fileName, QObject *parent) :
    QObject(parent)
{
    m_fileName = QCoreApplication::applicationDirPath() + "/" + __fileName;
    m_settingFile.setFileName(m_fileName);
    m_num = 0;
    m_settings.clear();

    tcpSetting_t tcp;
    m_settings.append(tcp);
    m_num = 1;
    createStringList();
}

TcpSettingFile::~TcpSettingFile()
{
    if(m_settingFile.isOpen())
        m_settingFile.close();
}

void TcpSettingFile::setFileName(QString __fileName)
{
    m_fileName = QCoreApplication::applicationDirPath() + "/" + __fileName;
    m_settingFile.setFileName(m_fileName);
    emit fileNameChanged();
}

void TcpSettingFile::setNum(int __num)
{
    if (__num <= 0)
        __num = 1;
    if(__num != m_num)
    {
        if(__num > m_num)
        {
            tcpSetting_t newTcp;
            for (int i = m_num; i < __num; i++)
                m_settings.append(newTcp);
        }else if (__num < m_num)
        {
            for (int i = m_num-1; i <= __num; i--)
                m_settings.removeAt(i);
        }
        m_num = __num;
        createStringList();
        emit settingsChanged();
    }
}

int TcpSettingFile::setSettings()
{
    QString str;
    if (m_num <= 0)
        return -1;

    m_settingFile.remove();
    if (!m_settingFile.open(QIODevice::ReadWrite))
        return -2;

    str = QString("Num\t%1\r\n").arg(m_num);

    for(int i = 0; i < m_num; i++)
    {
        str += QString("Name:\t")+ m_settings.at(i).name;
        str += QString("\tAddress:\t")+ m_settings.at(i).address.toString();
        str +=  QString("\tPort:\t%1").arg(m_settings.at(i).port);
        str += QString("\r\n");
    }
    if (m_settingFile.write(str.toUtf8().data(), str.length()) != str.length())
    {
         m_settingFile.close();
        return -3;
    }
    m_settingFile.flush();
    m_settingFile.close();

    createStringList();
    emit settingsChanged();
    return 1;
}

int TcpSettingFile::settings()
{
    QString str, add;
    QHostAddress hostAdd;
    tcpSetting_t tcp;
    int num, index1, index2, i;
    bool ok = false;

    if (!m_settingFile.open(QIODevice::ReadWrite))
        return -1;

    str = QString(m_settingFile.readAll());
    m_settingFile.close();

    index1 = 0;
    index2 = str.indexOf("\t", index1);
    index2++;
    num = str.mid(index2, str.indexOf("\r\n", index2) -index2).toInt(&ok);
    index1 = index2;
    if(!ok)
        return -1;
    m_num = 0;
    m_settings.clear();
    for(i = 0; i < num; i++)
    {
        if(index2 >= str.length())
            return -2;

        index2 = str.indexOf("\t", index1);
        index2++;
        index1 = str.indexOf("\t", index2);
        tcp.name = str.mid(index2, index1 - index2);
        index1++;

        index2 = str.indexOf("\t", index1);
        index2++;
        index1 = str.indexOf("\t", index2);
        add = str.mid(index2, index1 - index2);
        hostAdd.setAddress(add);
        tcp.address = hostAdd;

        index1++;
        index2 = str.indexOf("\t", index1);
        index2++;
        index1 = str.indexOf("\r\n", index2);
        tcp.port = str.mid(index2, index1 - index2).toInt();
        index1++;

        m_settings.append(tcp);
        m_num++;
    }

    createStringList();
    emit settingsChanged();
    return 1;
}

void TcpSettingFile::setName(QString __name, int __i)
{
    if(__i >= m_num)
        return;
    tcpSetting_t tcp = m_settings.at(__i);
    tcp.name = __name;

    m_settings.replace(__i, tcp);
}

void TcpSettingFile::setAddress(QString __address, int __i)
{
    if(__i >= m_num)
        return;
    tcpSetting_t tcp = m_settings.at(__i);
    tcp.address = QHostAddress(__address);

    m_settings.replace(__i, tcp);
}

void TcpSettingFile::setAddress(QHostAddress __address, int __i)
{
    if(__i >= m_num)
        return;
    tcpSetting_t tcp = m_settings.at(__i);
    tcp.address = __address;

    m_settings.replace(__i, tcp);
}

void TcpSettingFile::setPort(int __port, int __i)
{
    if(__i >= m_num)
        return;
    tcpSetting_t tcp = m_settings.at(__i);
    tcp.port = __port;

    m_settings.replace(__i, tcp);
}

void TcpSettingFile::createStringList()
{
    m_serverPort.clear();
    m_serverAddress.clear();
    m_serverName.clear();
    for(int i = 0; i < m_num; i++)
    {
        m_serverName.append(m_settings.at(i).name);
        m_serverAddress.append(m_settings.at(i).address.toString());
        m_serverPort.append(QString().sprintf("%d", m_settings.at(i).port));
    }
}









