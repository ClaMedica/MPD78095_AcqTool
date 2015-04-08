#include "codebook.h"
#include "CPP/functionhandler.h"
#include "QFile"

Utterance::Utterance()
{

}

/**
 * @brief Utterance::toByte
 * @return The ByteArray that contains m_code of the class
 */
bool Utterance::toByte(QByteArray *__BA,ZipMode __zip)
{
    //Controlli
    QString funMex="Utterance::toByte";
    if(__BA==NULL)
    {
        qDebug() << funMex+"ByteArray not created";
        return false;
    }

    //mettiamo intanto la size del segnale
    __BA->append(zip(m_sig.getSize(),0,__zip));
    //poi copiamo i dati del segnale
    for(int i=0;i<m_sig.getSize();i++)
        __BA->append(zip(m_sig.getValue(i),5,__zip));
    //appendiamo il nome
    char c=m_name.length();
    __BA->append(c);
    __BA->append(m_name);

    //appendiamo le dimensioni della tabella
    __BA->append(zip(m_tab.rows(),0,__zip));
    __BA->append(zip(m_tab.columns(),0,__zip));

    //e i suoi valori
    for(int i=0;i<m_tab.rows();i++)
        for(int j=0;j<m_tab.columns();j++)
            __BA->append(zip(m_tab.getValue(i,j),5,__zip));

    //e infine prepend della lunghezza totale della baracca
    __BA->prepend(zip(__BA->length(),0,__zip));
    return true;
}

/**
 * @brief Utterance::fromByte set the Utterance parameters from the information
 * in BA
 * @param BA
 */
bool Utterance::fromByte(QByteArray *__BA,ZipMode __zip)
{
    //Controlli
    QString funMex="Utterance::fromByte";
    if(__BA==NULL)
    {
        qDebug() << funMex+"ByteArray not created";
        return false;
    }




    if(__zip==Normal)
    {
        char c;
        int off=0;
        QByteArray park;
        park=__BA->mid(off,4);
        off+=4;

        int size=(int)unzip(&park);
        m_sig.setSize(size);
        for(int i=0;i<size;i++)
        {
            float v;
            park.clear();
            park=__BA->mid(off,4);
            off+=4;
            v=unzip(&park);
            m_sig.setValue(i,v);
        }
        // e il segnale è fatto


        park.clear();
        c=(*__BA)[off];

        park=__BA->mid(off,c);
        off+=c+1;
        m_name.append(park);

        //leggo il numero di colonne
        park.clear();
        park=__BA->mid(off,4);
        off+=4;
        int rig=(int)unzip(&park);

        //leggo il numero di righe
        park.clear();
        park=__BA->mid(off,4);
        off+=4;
        int col=(int)unzip(&park);

        m_tab.addColumns(col-1);
        m_tab.addRows(rig-1);

        for(int i=0;i<rig;i++)
            for(int k=0;k<col;k++)
            {
                float v;
                park.clear();
                park=__BA->mid(off,4);
                off+=4;
                v=unzip(&park);
                m_tab.setValue(i,k,v);
            }

    }
    else
    {
        char c=(*__BA)[0];
        c=(c>>5)&7; //numero di byte da prendere
        int off=1;
        QByteArray park;
        for(int i=0;i<c+1;i++)
            park.append((*__BA)[i+off]);
        off+=c+1;

        int size=(int)unzip(&park);
        m_sig.setSize(size);
        for(int i=0;i<size;i++)
        {
            float v;
            park.clear();
            c=(*__BA)[off];
            c=(c>>5)&7; //numero di byte da prendere
            for(int j=0;j<c+1;j++)
                park.append((*__BA)[off+j]);
            off+=c+1;
            v=unzip(&park);
            m_sig.setValue(i,v);
        }
        // e il segnale è fatto


        park.clear();
        c=(*__BA)[off];

        for(int j=0;j<c;j++)
            park.append((*__BA)[off+j+1]);
        off+=c+1;
        m_name.append(park);

        //leggo il numero di colonne
        park.clear();
        c=(*__BA)[off];
        c=(c>>5)&7; //numero di byte da prendere
        for(int j=0;j<c+1;j++)
            park.append((*__BA)[off+j]);
        off+=c+1;
        int rig=(int)unzip(&park);

        //leggo il numero di righe
        park.clear();
        c=(*__BA)[off];
        c=(c>>5)&7; //numero di byte da prendere
        for(int j=0;j<c+1;j++)
            park.append((*__BA)[off+j]);
        off+=c+1;
        int col=(int)unzip(&park);

        m_tab.addColumns(col-1);
        m_tab.addRows(rig-1);

        for(int i=0;i<rig;i++)
            for(int k=0;k<col;k++)
            {
                float v;
                park.clear();
                c=(*__BA)[off];
                c=(c>>5)&7; //numero di byte da prendere
                for(int j=0;j<c+1;j++)
                    park.append((*__BA)[off+j]);
                off+=c+1;
                v=unzip(&park);
                m_tab.setValue(i,k,v);
            }

    }
    return true;
}
Codebook::Codebook()
{
    m_winLen=84;
}

Codebook::Codebook(int __winLen)
{
    m_winLen=__winLen;
}

Codebook::~Codebook()
{
    for(int i=0;i< m_code.size();i++)
        delete m_code[i];
}

/**
 * @brief Codebook::appSignal append a new Utterance to the Codebook
 * calculating it from the signal
 * @param __pSignal
 * @param name
 */

bool Codebook::appSignal(MSignal *__pSignal,QString name)
{
    QString funMex="Codebook::appSignal";
    if(__pSignal==NULL)
    {
        qDebug() << funMex+"Signal not created";
        return false;
    }

    Utterance *u=new Utterance;
    u->m_sig=*__pSignal;
    u->m_name=name;

    vectorize(&(u->m_tab),__pSignal,m_winLen,LPC);

    m_code.append(u);
    return true;
}

/**
 * @brief Codebook::loadFromFile load the Codebook from the path file
 */
bool Codebook::loadFromFile(QString __path)
{

    QFile f;
    f.setFileName(__path);
    if(f.open(QIODevice::ReadOnly))
    {
        m_code.clear();
        QByteArray BA,park,Ut;
        BA=f.readAll();
        char c=BA[0];
        int off=0;
        if(c=='N')
        {
            off=1;
            park.clear();
            park=BA.mid(off,4);
            off+=4;


            int size=(int)unzip(&park);

            for(int i=0;i<size;i++)
            {
                Utterance *u=new Utterance;

                park.clear();
                park=BA.mid(off,4);
                off+=4;
                int section=(int)unzip(&park);
                Ut=BA.mid(off,section);
                u->fromByte(&Ut);
                off+=section;
                m_code.append(u);
            }
        }
        else
        {
            off=1;
            c=(c>>5)&7; //numero di byte da prendere


            for(int i=0;i<c+1;i++)
                park.append(BA[i+off]);
            off+=c+1;

            int size=(int)unzip(&park);

            for(int i=0;i<size;i++)
            {
                Utterance *u=new Utterance;
                park.clear();
                c=BA[off];
                c=(c>>5)&7; //numero di byte da prendere
                for(int j=0;j<c+1;j++)
                    park.append(BA[off+j]);
                off+=c+1;
                int section=(int)unzip(&park);
                Ut.clear();
                for(int y=0;y<section;y++)
                    Ut[y]=BA[y+off];
                u->fromByte(&Ut);
                off+=section;
                m_code.append(u);
            }
        }

        f.close();
    }
    return true;
}

/**
 * @brief Codebook::saveToFile save the Codebook to the path file
 */
bool Codebook::saveToFile(QString __path,ZipMode __zip)
{
    QFile f;
    f.setFileName(__path);
    if(f.open(QIODevice::WriteOnly))
    {
        QByteArray BA;

        for(int i=0;i<m_code.size();i++)
        {
            QByteArray b;
            m_code[i]->toByte(&b);
            BA.append(b);
        }

        BA.prepend(zip(m_code.size()));
        if(__zip==Normal)
            BA.prepend('N');
        else
            BA.prepend('Z');
        f.write(BA);
        f.close();
        return true;
    }
    return false;
}

Utterance *Codebook::getUtterance(int __position)
{
    QString funMex="Codebook::getUtterance";

    if(__position<0 || __position>=m_code.size())
    {
        qDebug() << funMex+"Wrong parameters";
        qDebug() << "__position"<<__position;
        qDebug() << "m_code.size"<<m_code.size();
        return NULL;
    }

    return m_code[__position];
}


