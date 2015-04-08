#ifndef MYAUDIO_H
#define MYAUDIO_H
#include <QString>
#include <../MGlobal/msignal.h>
#include <../MGlobal/global.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QtMultimedia/QAudio>
#include <QBuffer>

class MAudio
{
public:

    MAudio();
    ~MAudio();

    int fromFileToSignal(MSignal **__pChannels,
                          QString __fileName);

    bool fromFileToBuffer(QBuffer *__pBuffer,
                          QString __fileName);

    bool fromSignalToFile(QString __fileName,
                          QVector<MSignal> *__pChannels,
                          AudioFileType __type,
                          BitXSample __bitXSample=Bit8);

    bool fromSignalToBuffer(QBuffer *__pBuffer,
                            QVector<MSignal> *__pChannels,
                            AudioFileType __type,
                            BitXSample __bitXSample=Bit8);

    void setFileName(QString __fileName);
    QAudioFormat *getFormat();
private:
    bool getFormatFromFile(QByteArray *__byteArray);
    bool setFormat(QVector<MSignal> *__pChannels, AudioFileType __type, BitXSample __bitXSample=Bit8);
    WAVE_FORMAT *p_waveFormat;
    QAudioFormat *p_format;
    int m_typeFormat;
};



QByteArray toLittleEndian(int __n, int __kj);
#endif // MYAUDIO_H
