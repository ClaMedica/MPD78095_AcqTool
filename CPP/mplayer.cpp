#include "mplayer.h"

#define NOTIFY_INTERVAL 50
#define BUFFER_SIZE 2000

/**
 * @brief MPlayer::MPlayer
 */
MPlayer::MPlayer(QObject */*parent*/)
{
    m_realTimeEnabled=false;
    m_fileLoadedToBuffer=false;
    m_sliderPos=0;
    m_pBuffer=new QBuffer;
    m_startPos=0;
    m_endPos=0;
    m_pAudioOutput=new QAudioOutput;
}

/**
 * @brief MPlayer::~MPlayer
 */

MPlayer::~MPlayer()
{
    delete m_pAudioOutput;
    delete m_pBuffer;
}

/**
 * @brief MPlayer::sliderPos
 * @return
 */

uint MPlayer::sliderPos()
{
    return m_sliderPos;
}

/**
 * @brief MPlayer::setSliderPos
 * @param __pos
 */

void MPlayer::setSliderPos(uint __pos)
{
    if(__pos<=m_duration)
    {
        if(m_pAudioOutput!=NULL)
            if(m_pAudioOutput->state()!=QAudio::ActiveState)
            {
                //non sto riproducendo
                if(__pos!=m_sliderPos)
                {
                    m_sliderPos=__pos;
                    if(__pos<m_startPos)
                        m_sliderPos=m_startPos;
                    else if(__pos>m_endPos)
                        m_sliderPos=m_endPos;

                    sliderPosChanged();
                }
            }
    }
}

QStringList MPlayer::fileList()
{
    return m_fileMap.keys();
}

/**
 * @brief MPlayer::startPos
 * @return
 */
uint MPlayer::startPos()
{
    return m_startPos;
}

/**
 * @brief MPlayer::setStart
 * @param __start
 */
void MPlayer::setStartPos(uint __start)
{
    if(m_startPos!=__start)
    {
        if(__start<=m_duration)
        {
            m_startPos=__start;
            setSliderPos(m_sliderPos-1);
            emit startPosChanged();
        }
    }
}

uint MPlayer::endPos()
{
    return m_endPos;
}

void MPlayer::setEndPos(uint __end)
{
    if(m_endPos!=__end)
    {
        if(__end<=m_duration)
        {
            m_endPos=__end;
            setSliderPos(m_sliderPos+1);
            emit endPosChanged();
        }
    }
}

/**
 * @brief MPlayer::duration
 * @return
 */

uint MPlayer::duration()
{
    return m_duration;
}

/**
 * @brief MPlayer::setSignalToPlay
 * @param __pChannels
 * @param __type
 * @return
 */

bool MPlayer::setSignalToPlay(QVector<MSignal> *__pChannels, AudioFileType __type)
{

    m_audio.fromSignalToBuffer(m_pBuffer,__pChannels,__type);
    if(m_pBuffer == NULL)
    {
        qDebug() << "File not loaded to buffer!";
        return false;
    }

    if(m_pAudioOutput != NULL)
        delete m_pAudioOutput;

    m_pAudioOutput = new QAudioOutput(*m_audio.getFormat());

    if(m_pAudioOutput == NULL)
    {
        qDebug() << "The audioOutput is empty";
        return false;
    }

    if(!m_realTimeEnabled)
    {
        m_pAudioOutput->setNotifyInterval(NOTIFY_INTERVAL);
        connect(m_pAudioOutput,SIGNAL(notify()),this,SLOT(updateSlider()));
    }
    connect(m_pAudioOutput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateManager(QAudio::State)));

    return true;
}

/**
 * @brief MPlayer::loadSignalFromFile
 * @param filename
 */
bool MPlayer::loadSignalFromFile(QString __fileName)
{
    if(__fileName.contains(".wav") || __fileName.contains(".mp3"))
    {//se è un formato audio
        __fileName.remove("file://");
        QStringList list=__fileName.split(QRegularExpression("\\b"));
        QString name=list[list.size()-4];
        m_fileMap[name]=__fileName;

        if(m_pBuffer!=NULL)
        {
            delete m_pBuffer;
            m_pBuffer=new QBuffer;
        }
        m_audio.fromFileToBuffer(m_pBuffer,__fileName);

        if(m_pBuffer==NULL)
        {
            qDebug() << "The buffer is empty";
            return false;
        }

        m_frequency=m_audio.getFormat()->sampleRate();

        if(m_frequency==0)
        {
            qDebug() << "The format is empty";
            return false;
        }
        int size=m_pBuffer->size();
        int samplesize=m_audio.getFormat()->sampleSize();
        m_duration=1000*(size-44)/(m_frequency*samplesize/8);


        //setStartPos(0);
        setEndPos(m_duration);

        emit durationChanged();

        m_pAudioOutput = new QAudioOutput(*m_audio.getFormat());

        if(m_pAudioOutput == NULL)
        {
            qDebug() << "The audioOutput is empty";
            return false;
        }

        if(!m_realTimeEnabled)
        {
            m_pAudioOutput->setNotifyInterval(NOTIFY_INTERVAL);
            connect(m_pAudioOutput,SIGNAL(notify()),this,SLOT(updateSlider()));
        }
        connect(m_pAudioOutput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateManager(QAudio::State)));

        return true;
    }
    else
        return false;
}

bool MPlayer::loadSignalFromName(QString __name)
{
    if(!m_fileMap[__name].isEmpty())
        return loadSignalFromFile(m_fileMap[__name]);
    return false;
}



/**
 * @brief startPlaying Starts to playing the audio file
 */
void MPlayer::startPlaying()
{
    if(m_pBuffer==NULL)
    {
        qDebug() << "No file loaded into buffer";
    }

    if(m_pAudioOutput->state()!=QAudio::ActiveState)//se è stato caricato un file sul buffer
    {
        int realTrackSize=(m_pBuffer->size()-44);
        int nByte=m_audio.getFormat()->bytesPerFrame();
        int trackPos=(realTrackSize*sliderPos()/m_duration);
        while(trackPos%nByte)
        {//onde evitare che sbagli posizione
            trackPos++;
        }
        m_pBuffer->open(QIODevice::ReadOnly);
        qDebug()<<trackPos;
        m_pBuffer->seek(trackPos);
        m_pAudioOutput->start(m_pBuffer);
    }

}

void MPlayer::suspendPlaying()
{
    m_pAudioOutput->suspend();
}

/**
 * @brief MPlayer::stateManager manage the states changing
 * @param state
 */

void MPlayer::stateManager(QAudio::State __state)
{
    if (__state == QAudio::IdleState)
    {
        m_pAudioOutput->stop();
        m_sliderPos=m_duration;
        emit sliderPosChanged();
        m_pBuffer->close();
    }
}

/**
 * @brief MPlayer::updateSlider obvious
 */

void MPlayer::updateSlider()
{
    m_sliderPos+=NOTIFY_INTERVAL;
    if(m_sliderPos>=m_endPos)
    {
        m_sliderPos=m_endPos;
        m_pAudioOutput->stop();
        emit endReached();
    }

    emit sliderPosChanged();
}

QAudio::State MPlayer::getState()
{
    return m_pAudioOutput->state();
}




