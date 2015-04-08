#ifndef CODEBOOK_H
#define CODEBOOK_H

#include <QVector>
#include <CPP/tabble.h>
#include <../MGlobal/msignal.h>
#include <../MGlobal/global.h>
#include <QString>
//#include <markov.h>
#include <QDate>

/**
 * @brief The utterance class provides a cluster of a tabble for the conversion of the mysignal
 * the mysignal and a name of reference
 */
class Utterance
{
public:
    Utterance();
    bool toByte(QByteArray *__BA, ZipMode __zip=Normal);
    bool fromByte(QByteArray *__BA, ZipMode __zip=Normal);

    MSignal m_sig;
    QString m_name;
    Tabble m_tab;
};

/**
 * @brief The codebook class provides to a set of function that can menage a vector of utterance
 */
class Codebook
{
public:
    Codebook();
    Codebook(int __winLen);
    ~Codebook();
    bool appSignal(MSignal *__pSignal,QString name);
    bool loadFromFile(QString __path);
    bool saveToFile(QString __path, ZipMode __zip=Normal);
    Utterance *getUtterance(int __position);
    int size();

private:
    int m_winLen;
    QVector<Utterance*> m_code;
};

#endif // CODEBOOK_H
