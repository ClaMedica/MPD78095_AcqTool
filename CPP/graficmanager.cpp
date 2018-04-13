#include "graficmanager.h"

GraficManager::GraficManager(QObject *parent) : QObject(parent)
{
    QString path;
    //costruisco il path
#ifdef PICOFLOW
    path = g_P7SettingsManager.dataPath() + SUBDIR_CONFIG + GRAFIC_CONFIG_PICO;
    if(!QFile::exists(path)) {
        qDebug() << "Il file non esiste copio quello dalle risorse";
        QFile::copy(":/Config/Config_Grafic_pico.xml", path);
    }
#else
    path= g_P7SettingsManager.dataPath() + SUBDIR_CONFIG + GRAFIC_CONFIG;
    if(!QFile::exists(path)) {
        qDebug() << "Il file non esiste copio quello dalle risorse";
        QFile::copy(":/Config/Config_Grafic.xml", path);
    }
#endif

    //e riempo la mia classe

    m_graphics.loadFromXML(path);
}

QVariant GraficManager::valueOf(const QString __name, const QString __type)
{
    return m_graphics.getSafeChild(__name)->getSafeAttribute(__type);
}
