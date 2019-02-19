#include "printerserialport.h"
#include <QDebug>
#include <QThread>

printerserialport::printerserialport(QString __namefile, QObject *parent) : QObject(parent)
{
    m_file = new QFile(__namefile);
    if(m_file->exists())
        m_file->remove();
    m_file->open(QIODevice::WriteOnly);
    qDebug() << "file aperto in scrittura:" << __namefile;
}

void printerserialport::closeSerialPort()
{
    if(m_file == NULL) {
        qDebug() << "file GIA' chiuso in scrittura" << m_file->fileName();
        return;
    }
    qDebug() << "file chiuso in scrittura" << m_file->fileName();
    m_file->close();
    delete m_file;
    m_file = NULL;
}

/*
   Invia alla Stampante "num_car" caratteri della stringa "str_pri"
*/
bool printerserialport::Pri_Str(char *__str, int __str_len)
{
    if(m_file->isOpen()) {
        static char trailer[1] = { 0x03 };
        char header[6] = { 0x5a, 0xa5, 0xa5, 0x5a, __str_len & 0xff, (__str_len >> 8) & 0xff };

        m_file->write(header, sizeof(header));
        m_file->write(__str, __str_len);
        m_file->write(trailer, sizeof(trailer));
    }

    return true;
}

void printerserialport::Pri_justif(char __mode)
{
    /* setta la giustificazione del testo*/
    // 0: centered; 1: right justified; 2: left justified

    char pri_str[] = { ESC, 'C', __mode} ;
//    Pri_Str(pri_str, sizeof(pri_str));
}

void printerserialport::Pri_mode(char __mode)
{
    /* imposta la modalita: default, double, quadruple, underlined */
    // mode = 0x00 = 00000000 : default
    // mode = 0xYY = X0XX0XX0 : <underlined> <0> <double w.> <double h.> <0> <quadruple w.> <quadruple h.> <0> */

    char pri_str[] = { ESC, '!', __mode };
//    Pri_Str(pri_str, sizeof(pri_str));
}

void printerserialport::Pri_forward(char __dotlines)
{
    /* fa avanzare di "dotlines" righe la carta*/

    char pri_str[] = { ESC, 'J', __dotlines };
    Pri_Str(pri_str, sizeof(pri_str));
}

void printerserialport::Pri_Font(char __font)
{
    /* Imposta il font desiderato della Printer   */
    /*    0 - 8x16  1 - 12x20 2 - 7x16  */

    char pri_str[] = { ESC, '%', __font };
//    Pri_Str(pri_str, 3);
}

void printerserialport::Pri_Intensity(char __intens)
{
//   intens = 0x80 default	0x00 < intens < 0xFF
//   intens < 0x80 lighter print
//   intens > 0x80 higher print

    char pri_str[] = { GS, 'D', __intens };
//    Pri_Str(pri_str, 3);
}


/* numero di pixel scaldati contemporaneamente --> (n+1)*8
    formula per consumo di corrente massimo Ca = 0,3 + V*(n+1)*8/Rdot
    dato il consumo in ampere desiderato Ca, la velocita massima si ha per n = ((Ca-0,3)*Rdot/(V*8) -1
    Ca = 1.4A @5V --> n =
*/    // default n = 5
//void printerserialport::Pri_Speed(char m_speed )
//{
//    char pri_str[] = { GS, '/', m_speed };     // 1 <= speed <= 32, 0 max speed
//        Pri_Str(pri_str);
//}

/* setta la massima velocita di stampa settando il tempo di avanzamento
    T = 256*n1 + n2
    2080 < T < 25000 [us]
    vel[mm/sec] = 1/(8*T)   5 < vel < 60 [mm/sec]
 */
//void printerserialport::Pri_Max_Speed(char m_n1, char m_n2)
//{
//    char pri_str[] = { GS, 's', m_n1, m_n2 };
//        Pri_Str(pri_str, 4);
//}

//void printerserialport::waiting()
//{
//    QThread::currentThread()->msleep(50);
//}
