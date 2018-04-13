#include "graficmanager.h"

GraficManager::GraficManager(QObject *parent) : QObject(parent)
{
    QString path;

    //costruisco il path
    path = g_P7SettingsManager.dataPath() + SUBDIR_CONFIG + GRAFIC_CONFIG;
    //e riempo la mia classe
    qDebug() << path;

    if(!QFile::exists(path)) {
        qDebug() << "Il file non esiste copio quello dalle risorse";
        QFile::copy(":/Config/Config_Grafic.xml", path);
    }
    m_graphics.loadFromXML(path);
}

QVariant GraficManager::valueOf(const QString __name, const QString __type)
{
    return m_graphics.getSafeChild(__name)->getSafeAttribute(__type);
}
