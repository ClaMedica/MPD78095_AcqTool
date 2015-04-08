#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <../MGlobal/msignal.h>
#include <../MGlobal/global.h>
#include <CPP/maudio.h>

/*
 *
 *      IMPORTANTE TUTTO CIò CHE RIGUARDA IL TEMPO è IN MILLISECONDI
 */

enum TrackPos
{
    start=0,
    end=1
};

class MPlayer : public QObject
{    
    Q_OBJECT
public:


    MPlayer(QObject *parent=0);
    ~MPlayer();

    Q_PROPERTY(uint sliderPos READ sliderPos WRITE setSliderPos NOTIFY sliderPosChanged)
    Q_PROPERTY(uint startPos READ startPos WRITE setStartPos NOTIFY startPosChanged)
    Q_PROPERTY(uint endPos READ endPos WRITE setEndPos NOTIFY endPosChanged)
    Q_PROPERTY(uint duration  READ duration  NOTIFY durationChanged)    

    uint sliderPos();
    void setSliderPos(uint __pos);



    uint duration();

    uint startPos();
    void setStartPos(uint __start);
    uint endPos();
    void setEndPos(uint __end);
signals:
    void sliderPosChanged();
    void startPosChanged();
    void endPosChanged();
    void durationChanged();
    void endReached();

public slots:

    bool setSignalToPlay(QVector<MSignal> *__pChannels, AudioFileType __type);
    bool loadSignalFromFile(QString __fileName);
    bool loadSignalFromName(QString __name);
    void startPlaying();
    void suspendPlaying();
    void stateManager(QAudio::State __state);
    void updateSlider();
    QAudio::State getState();
    QStringList fileList();

private:
    /* Ci vogliono sia informazioni su base intera di numero di campioni
     * che informazioni sulla base del tempo per poter regolare lo slider
     */

    uint  m_frequency;     //serve per risalire alla base dei tempi
    uint  m_duration;      //indica i millisecondi che dura la traccia
    uint  m_sliderPos;     //contiene i millisecondi a cui è posizionata la riproduzione
    uint  m_startPos;  //contiene l'inizio riproduzione in millisecondi
    uint  m_endPos;    //contiene la fine riproduzione in millisecondi
    bool   m_exist;         //true se m_AO è stato creato false se è distrutto
    bool   m_realTimeEnabled;

    QString rootPath;
    QString path_1;
    QString path_2;
    QString name;
    bool    m_fileSelected;
    bool    m_fileLoadedToBuffer;
    bool    m_playResume;
    MAudio  m_audio;
    QAudioOutput *m_pAudioOutput;
    QBuffer *m_pBuffer;
    QMap<QString,QString> m_fileMap;


};

#endif // PLAYER_H
