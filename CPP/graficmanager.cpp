#include "graficmanager.h"

GraficManager::GraficManager(QObject *parent) : QObject(parent)
{
    QString path;
    //costruisco il path
#ifdef PICOFLOW
    path = g_P7SettingsManager.dataPath() + SUBDIR_CONFIG + GRAFIC_CONFIG_PICO;
    QFile f, f2;
    f.setFileName(":/Config/Config_Grafic_pico.xml");
    f2.setFileName(path);
    if(!f2.exists()) {  //se non esiste lo creo
        qDebug() << "Creo file" << f2.fileName();
        f2.open(QIODevice::WriteOnly);
        f.open(QIODevice::ReadOnly);
        f2.write(f.readAll());
        f.close();
        f2.close();
    }

#else
    path= g_P7SettingsManager.dataPath() + SUBDIR_CONFIG + GRAFIC_CONFIG;
    QFile f, f2;
    f.setFileName(":/Config/Config_Grafic.xml");
    f2.setFileName(path);
    if(!f2.exists()) {  //se non esiste lo creo
        qDebug() << "Creo file" << f2.fileName();
        f2.open(QIODevice::WriteOnly);
        f.open(QIODevice::ReadOnly);
        f2.write(f.readAll());
        f.close();
        f2.close();
    }

#endif

    //e riempo la mia classe

    m_graphics.loadFromXML(path);
}

QVariant GraficManager::valueOf(const QString __name, const QString __type)
{
    return m_graphics.getSafeChild(__name)->getSafeAttribute(__type);
}
