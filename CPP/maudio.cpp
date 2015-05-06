#include <CPP/maudio.h>
//#include <QSlider>
//#include <QToolButton>
//#include <plotter.h>
#include <QFile>
#include <math.h>

QByteArray toLittleEndian(int __n, int __kj)
{
    QByteArray ba;


    if(__n>3){ba[3]=__kj/pow(2,32);__kj-=ba[3]*pow(2,32);}
    if(__n>2){ba[2]=__kj/pow(2,16);__kj-=ba[2]*pow(2,16);}
    if(__n>1){ba[1]=__kj/pow(2,8);__kj-=ba[1]*pow(2,8);}
    ba[0]=__kj;

    return ba;
}

/**
 * @brief MAudio::MAudio
 * @param type identifies the type of the sound stream
 */
MAudio::MAudio()
{
    p_waveFormat= new WAVE_FORMAT;
    p_format=new QAudioFormat;
}

/**
 * @brief MAudio::~MAudio
 */

MAudio::~MAudio()
{
    delete p_waveFormat;
    delete p_format;
}

/**
 * @brief MAudio::setFormat
 * @param __channels
 * @param __type
 * @param __bitXSample
 * @return
 */

bool MAudio::setFormat(QVector<MSignal> *__pChannels,
                       AudioFileType __type,
                       BitXSample __bitXSample)
{
    m_typeFormat=__type;

    if(__pChannels!=NULL)
    {
        QVector<MSignal> s=*__pChannels;
        switch(__type)
        {
        case MP3:break;
        case WAVE:
        {
            if(p_waveFormat==NULL)
            {
                qDebug() << "p_waveFormat not created!";
                return false;
            }
            p_waveFormat->ChunkID="RIFF";
            p_waveFormat->Format="WAVE";
            p_waveFormat->Subchunk1ID="fmt ";
            p_waveFormat->Subchunk1IDSize=16;
            p_waveFormat->AudioFormat=1;
            p_waveFormat->Subchunk2ID="data";
            p_waveFormat->BitsPerSample=__bitXSample; //è garantito dall'enumeratore dunque difficilmente ci saranno errori

            if(s[0].getSamplingFrequency()>0)
                p_waveFormat->SampleRate=s[0].getSamplingFrequency();
            else
            {
                qDebug() << "Wrong frequency value : " << s[0].getSamplingFrequency();
                return false;
            }

            if(s.size()>0 && s.size()<3)
                p_waveFormat->NumChannels=s.size();
            else
            {
                qDebug() << "Wrong number of channels : " << s.size();
                return false;
            }
            p_waveFormat->BlockAlign=p_waveFormat->NumChannels*p_waveFormat->BitsPerSample/8;
            p_waveFormat->ByteRate=s[0].getSamplingFrequency()*p_waveFormat->NumChannels*p_waveFormat->BitsPerSample/8;

            if(s[0].size()>0)
                p_waveFormat->Subchunk2IDSize=s[0].size()*p_waveFormat->NumChannels*p_waveFormat->BitsPerSample/8;
            else
            {
                qDebug() << "Wrong signal length : " << s[0].size();
                return false;
            }
            p_waveFormat->ChunkSize=36+p_waveFormat->Subchunk2IDSize;

            if(p_format==NULL)
            {
                qDebug() << "Format not created!";
                return false;
            }
            p_format->setSampleRate(p_waveFormat->SampleRate);
            p_format->setChannelCount(p_waveFormat->NumChannels);
            p_format->setSampleSize(p_waveFormat->BitsPerSample);
            p_format->setCodec("audio/pcm");
            p_format->setByteOrder(QAudioFormat::LittleEndian);
            p_format->setSampleType(QAudioFormat::UnSignedInt);
            break;
            default: qDebug() << "Should not be here!!!";return false;
        }
        }
    }
    else
    {
        qDebug() << "The signal is empty";
        return false;
    }

    return true;
}

/**
 * @brief MAudio::fromFileToSignal reads a wav file from path
 * @param __filename
 * @param __signal
 * @return false if something wrong
 */

int MAudio::fromFileToSignal(MSignal **__pChannels, QString __fileName)
{
    if(*__pChannels!=NULL)
    {
        qDebug() << "__pchannel corrupted";
        return 0;
    }

    QFile f;
    char c[4];
    f.setFileName(__fileName);
    if(!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "File not found: " << __fileName ;
        return 0;
    }

    QByteArray BA;
    BA=f.readAll();
    f.close();
    if(BA.size()==0)
    {
        qDebug() << "File empty";
        return 0;
    }

    getFormatFromFile(&BA);



    int k=0;
    for(int which=0;which<p_waveFormat->NumChannels;which++)
    {
        (*__pChannels)=new MSignal[p_waveFormat->NumChannels];
        (*__pChannels)[which].resize(p_waveFormat->Subchunk2IDSize/(p_waveFormat->NumChannels*p_waveFormat->BitsPerSample/8));
        (*__pChannels)[which].setSamplingFrequency(p_waveFormat->SampleRate);
        int index=0;
        for(int i=44+k;index<(*__pChannels)[which].size();i+=p_waveFormat->BitsPerSample/8)
        {
            float park;
            c[3]=0;
            c[2]=0;
            c[1]=0;
            c[0]=0;

            c[0]=BA[i];
            if(p_waveFormat->BitsPerSample/8>1)
                c[1]=BA[i+1];
            if(p_waveFormat->BitsPerSample/8>2)
                c[2]=BA[i+2];
            if(p_waveFormat->BitsPerSample/8>3)
                c[3]=BA[i+3];

            park=c[3]*pow(2,32)+c[2]*pow(2,16)+c[1]*pow(2,8)+c[0];
            (*__pChannels)[which].replace(index,park);

            index++;
        }
        k=p_waveFormat->BitsPerSample/8;
        (*__pChannels)[which].normalize(-1,1);
        //int d=ceil((*__pChannels)[which].getDuration());
        //nuovo.addSilenceTo(d);        

    }
    return p_waveFormat->NumChannels;
}

/**
 * @brief MAudio::fromFileToBuffer
 * @param __buffer
 * @param __fileName
 * @return
 */

bool MAudio::fromFileToBuffer(QBuffer *__pBuffer, QString __fileName)
{
    //da aggiungere controllo sull'estensione in futuro
    if(__pBuffer==NULL)
    {
        qDebug() << "Buffer not created before fromFileToBuffer call";
        return false;
    }
    QByteArray All;
    QFile f;

    if(__fileName.isEmpty())
    {
        qDebug() << "File name empty";
        return false;
    }
    f.setFileName(__fileName);
    f.open(QIODevice::ReadOnly);
    All=f.readAll();
    getFormatFromFile(&All);
    if(All.size()==0)
    {
        qDebug() << "File empty";
        return false;
    }
    __pBuffer->open(QIODevice::WriteOnly);
    __pBuffer->write(All);
    __pBuffer->seek(0);
    __pBuffer->close();
    return true;
}

/**
 * @brief MAudio::fromSignalToFile
 * @param __filename
 * @param __channels
 * @param __type
 * @param __bitXSample
 * @return
 */

bool MAudio::fromSignalToFile(QString __filename,
                              QVector<MSignal> *__pChannels,
                              AudioFileType __type,
                              BitXSample __bitXSample)
{
    if(!setFormat(__pChannels,__type,__bitXSample))
    {
        qDebug() << "Wrong format parameters";
        return false;
    }

    QVector<MSignal> s=*__pChannels;
    QFile f;
    f.setFileName(__filename);
    f.open(QIODevice::WriteOnly);

    QByteArray Header,Body,All;
    Header.append(p_waveFormat->ChunkID);
    Header.append(toLittleEndian(4,p_waveFormat->ChunkSize));
    Header.append(p_waveFormat->Format);
    Header.append(p_waveFormat->Subchunk1ID);
    Header.append(toLittleEndian(4,p_waveFormat->Subchunk1IDSize));
    Header.append(toLittleEndian(2,p_waveFormat->AudioFormat));
    Header.append(toLittleEndian(2,p_waveFormat->NumChannels));
    Header.append(toLittleEndian(4,p_waveFormat->SampleRate));
    Header.append(toLittleEndian(4,p_waveFormat->ByteRate));
    Header.append(toLittleEndian(2,p_waveFormat->BlockAlign));
    Header.append(toLittleEndian(2,p_waveFormat->BitsPerSample));
    Header.append(p_waveFormat->Subchunk2ID);
    Header.append(toLittleEndian(4,p_waveFormat->Subchunk2IDSize));

    int w=0;
    for(int i=0;i<s[0].size();i++)
        for(w=0;w<p_waveFormat->NumChannels;w++)
        {
            float d=s[w].value(i);
            int v;
            d=d*pow(2,p_waveFormat->BitsPerSample-1)+pow(2,p_waveFormat->BitsPerSample-1)-1;
            v=d;
            Body.append(toLittleEndian(p_waveFormat->BitsPerSample/8,v));
        }
    All.append(Header);
    All.append(Body);
    f.write(All);
    f.close();

    return true;
}

/**
 * @brief MAudio::fromSignalToBuffer
 * @param __buffer to fill
 * @param __channels
 * @param __type
 * @param __bitXSample
 * @return
 */
bool MAudio::fromSignalToBuffer(QBuffer *__pBuffer,
                                QVector<MSignal> *__pChannels,
                                AudioFileType __type,
                                BitXSample __bitXSample)
{
    if(!setFormat(__pChannels,__type,__bitXSample))
    {
        qDebug() << "Wrong format parameters";
        return false;
    }

    if(__pBuffer==NULL)
    {
        qDebug() << "Buffer not created before fromSignalToBuffer call";
        return false;
    }
    for(int i=0;i<__pChannels->size();i++)
        (*__pChannels)[i].normalize();

    QByteArray Header,Body,All;
    Header.append(p_waveFormat->ChunkID);
    Header.append(toLittleEndian(4,p_waveFormat->ChunkSize));
    Header.append(p_waveFormat->Format);
    Header.append(p_waveFormat->Subchunk1ID);
    Header.append(toLittleEndian(4,p_waveFormat->Subchunk1IDSize));
    Header.append(toLittleEndian(2,p_waveFormat->AudioFormat));
    Header.append(toLittleEndian(2,p_waveFormat->NumChannels));
    Header.append(toLittleEndian(4,p_waveFormat->SampleRate));
    Header.append(toLittleEndian(4,p_waveFormat->ByteRate));
    Header.append(toLittleEndian(2,p_waveFormat->BlockAlign));
    Header.append(toLittleEndian(2,p_waveFormat->BitsPerSample));
    Header.append(p_waveFormat->Subchunk2ID);
    Header.append(toLittleEndian(4,p_waveFormat->Subchunk2IDSize));

    int w=0;
    for(int i=0;i<(*__pChannels)[0].size();i++)
        for(w=0;w<p_waveFormat->NumChannels;w++)
        {
            float d=(*__pChannels)[w].value(i);
            int v;
            d=d*pow(2,p_waveFormat->BitsPerSample-1)+pow(2,p_waveFormat->BitsPerSample-1)-1;
            v=d;
            Body.append(toLittleEndian(p_waveFormat->BitsPerSample/8,v));
        }
    All.append(Header);
    All.append(Body);


    __pBuffer->open(QIODevice::WriteOnly);
    __pBuffer->write(All);
    __pBuffer->seek(0);
    __pBuffer->close();

    return true;
}

/**
 * @brief MAudio::getFormat
 * @return
 */

QAudioFormat* MAudio::getFormat()
{
    return p_format;
}

bool MAudio::getFormatFromFile(QByteArray *__byteArray)
{

    int off=40;
    unsigned char c[4];
    c[3]=(*__byteArray)[off+3];
    c[2]=(*__byteArray)[off+2];
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];

    if(p_waveFormat==NULL)
    {
        qDebug() << "p_waveFormat not created!";
        return false;
    }

    p_waveFormat->Subchunk2IDSize=c[3]*pow(2,32)+c[2]*pow(2,16)+c[1]*pow(2,8)+c[0];
    if(p_waveFormat->Subchunk2IDSize<1)
    {
        qDebug() << "Wrong Subchunk2IDSize: " << p_waveFormat->Subchunk2IDSize;
        return false;
    }
    p_waveFormat->ChunkSize=p_waveFormat->Subchunk2IDSize+36;
    off=16;
    c[3]=(*__byteArray)[off+3];
    c[2]=(*__byteArray)[off+2];
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];
    p_waveFormat->Subchunk1IDSize=c[3]*pow(2,32)+c[2]*pow(2,16)+c[1]*pow(2,8)+c[0];
    if(p_waveFormat->Subchunk1IDSize<1)
    {
        qDebug() << "Wrong Subchunk1IDSize: " << p_waveFormat->Subchunk1IDSize;
        return false;
    }
    off=20;
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];
    p_waveFormat->AudioFormat=c[1]*pow(2,8)+c[0];
    if(p_waveFormat->AudioFormat<1)
    {
        qDebug() << "Wrong AudioFormat: " << p_waveFormat->AudioFormat;
        return false;
    }
    off=22;
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];
    p_waveFormat->NumChannels=c[1]*pow(2,8)+c[0];
    if(p_waveFormat->NumChannels<1)
    {
        qDebug() << "Wrong NumChannels: " << p_waveFormat->NumChannels;
        return false;
    }
    off=24;
    c[3]=(*__byteArray)[off+3];
    c[2]=(*__byteArray)[off+2];
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];
    p_waveFormat->SampleRate=c[3]*pow(2,32)+c[2]*pow(2,16)+c[1]*pow(2,8)+c[0];
    if(p_waveFormat->SampleRate<1)
    {
        qDebug() << "Wrong SampleRate: " << p_waveFormat->SampleRate;
        return false;
    }
    off=28;
    c[3]=(*__byteArray)[off+3];
    c[2]=(*__byteArray)[off+2];
    c[1]=(*__byteArray)[off+1];
    c[0]=(*__byteArray)[off];
    p_waveFormat->ByteRate=c[3]*pow(2,32)+c[2]*pow(2,16)+c[1]*pow(2,8)+c[0];
    if(p_waveFormat->ByteRate<1)
    {
        qDebug() << "Wrong ByteRate: " << p_waveFormat->ByteRate;
        return false;
    }
    p_waveFormat->BitsPerSample=(BitXSample)((p_waveFormat->ByteRate*8)/(p_waveFormat->NumChannels*p_waveFormat->SampleRate));
    p_waveFormat->BlockAlign=p_waveFormat->NumChannels*p_waveFormat->BitsPerSample/8;

    if(p_format==NULL)
    {
        qDebug() << "Format not created!";
        return false;
    }
    p_format->setSampleRate(p_waveFormat->SampleRate);
    p_format->setChannelCount(p_waveFormat->NumChannels);
    p_format->setSampleSize(p_waveFormat->BitsPerSample);
    p_format->setCodec("audio/pcm");
    p_format->setByteOrder(QAudioFormat::LittleEndian);
    p_format->setSampleType(QAudioFormat::UnSignedInt);
    return true;
}
