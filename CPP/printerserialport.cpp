#include "printerserialport.h"
#include <QDebug>
#include <QThread>

printerserialport::printerserialport(QObject *parent) : QObject(parent)
{
    m_file = new QFile("/tmp/file2print");
    if(m_file->exists())
        m_file->remove();
    m_file->open(QIODevice::WriteOnly);
////    m_serialPort.setPortName("/dev/ttyUSB0");
//    m_serialPort.setPortName("/dev/ttyS0");
//    m_serialPort.setBaudRate(115200);
//    m_serialPort.setFlowControl(QSerialPort::HardwareControl);
//    m_serialPort.setParity(QSerialPort::NoParity);
//    m_serialPort.setDataBits(QSerialPort::Data8);
//    m_serialPort.setStopBits(QSerialPort::OneStop);

//    if ( ! m_serialPort.open(QIODevice::ReadWrite))
//        qWarning() << "Unable to open serial port";
}

//void printerserialport::init_printer()
//{
//    system("/root/PicoFlow/initprinter.sh");
//    Pri_Reset();
////    Pri_Default();
//    m_serialPort.flush();
//}

void printerserialport::closeSerialPort()
{
    m_file->close();
    delete m_file;
    m_file = NULL;
}

//#define _HDUMP_
//#define _PRISTR_SPLIT_
//#define _PRISTR_BINDUMP_

#ifdef _HDUMP_
int hdump(FILE *fp, char *p, int n)
{
    int    x = 0;
    while(n > 0) {
        char h[50] = {0};
        char a[18] = {0};
        int nn = ((n < 16) ? n : 16);
        for(int i = 0; i < nn; i++) {
            int c = *p++ & 0xff;
            sprintf(&h[i*3], "%2.2x ", c);
            a[i] = (c >= ' ' && c < 0x7f) ? c : '.';
            a[i+1] = 0;
        }
        fprintf(fp, "\t[%4.4x] %-48.48s    %s\n", x, h, a);
        n -= nn;
        x += nn;
    }
    return(x);
}
#endif

//int printerserialport::status(bool print)
//{
//    (void) m_serialPort.readAll();
//    // status test
//    char escst[2] = { ESC, 'v' };
//    m_serialPort.write(escst, 2);
//    m_serialPort.flush();

//    m_serialPort.waitForReadyRead(100);
//    char ret[2];
//    int  sz = 0;
//    while(sz <= 0) {
//        sz = m_serialPort.read(ret, 1);
//        if(sz > 0 && print) {
//            int v = ret[0] & 0xff;
//            qDebug("ESC_V = cutterfail:%c hole-mark:%c on-line:%c in-use:%c PWR:%c paper-out:%c head-up:%c head:%c",
//                   (v & (1 << 7)) ? 'C' : 'c',
//                   (v & (1 << 6)) ? 'H' : 'h',
//                   (v & (1 << 5)) ? 'O' : 'o',
//                   (v & (1 << 4)) ? 'R' : 'r',
//                   (v & (1 << 3)) ? 'S' : 's',
//                   (v & (1 << 2)) ? 'P' : 'p',
//                   (v & (1 << 1)) ? 'H' : 'h',
//                   (v & (1 << 0)) ? 'T' : 't'
//                  );
//        }
//        else
//            m_serialPort.waitForReadyRead(100);
//    }
//    return ret[0] & 0xff;
//}

/*
   Invia alla Stampante "num_car" caratteri della stringa "str_pri"
   Se "flag_lf" invia il carattere LF alla fine
*/
bool printerserialport::Pri_Str(int __num_car, char *__str_pri, unsigned char __flag_lf)
{
    int sz = __num_car + (__flag_lf ? 1 : 0);
    char header[6] = { 0x5a, 0xa5, 0xa5, 0x5a, sz & 0xff, (sz >> 8) & 0xff };
    m_file->write(header, sizeof(header));

    m_file->write(__str_pri, __num_car);

    if(__flag_lf) {
        char s[1] = { LF };
        m_file->write(s, 1);
    }

    char trailer[1] = { 0x03 };
    m_file->write(trailer, sizeof(trailer));
    m_file->flush();

    return true;
}


void printerserialport::Pri_justif(char __mode)
{
    /* setta la giustificazione del testo*/
    // 0 centered; 1 right justified; 2 left justified

    char pri_str[] = { ESC, 'C', __mode} ;
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_mode(char __mode)
{
    /* imposta la modalita: default, double, quadruple, underlined */
    // mode = 0x00 = 00000000 : default
    // mode = 0xYY = X0XX0XX0 : <underlined> <0> <double w.> <double h.> <0> <quadruple w.> <quadruple h.> <0> */

    char pri_str[] = { ESC, '!', __mode };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_forward(char __dotlines)
{
    /* fa avanzare di "dotlines" righe la carta*/

    char pri_str[] = { ESC, 'J', __dotlines };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_Font(char __font)
{
    /* Imposta il font desiderato della Printer   */
    /*    0 - 8x16  1 - 12x20 2 - 7x16  */

    char pri_str[] = { ESC, '%', __font };
    Pri_Str(3, pri_str, 0);
}

void printerserialport::Pri_Intensity(char __intens)
{
//   intens = 0x80 default	0x00 < intens < 0xFF
//   intens < 0x80 lighter print
//   intens > 0x80 higher print

    char pri_str[] = { GS, 'D', __intens };
    Pri_Str(3, pri_str, 0);
}


/* numero di pixel scaldati contemporaneamente --> (n+1)*8
    formula per consumo di corrente massimo Ca = 0,3 + V*(n+1)*8/Rdot
    dato il consumo in ampere desiderato Ca, la velocita massima si ha per n = ((Ca-0,3)*Rdot/(V*8) -1
    Ca = 1.4A @5V --> n =
*/    // default n = 5
//void printerserialport::Pri_Speed(char m_speed )
//{
//    char pri_str[] = { GS, '/', m_speed };     // 1 <= speed <= 32, 0 max speed
//    Pri_Str(3, pri_str, 0);
//}

/* setta la massima velocita di stampa settando il tempo di avanzamento
    T = 256*n1 + n2
    2080 < T < 25000 [us]
    vel[mm/sec] = 1/(8*T)   5 < vel < 60 [mm/sec]
 */
//void printerserialport::Pri_Max_Speed(char m_n1, char m_n2)
//{
//    char pri_str[] = { GS, 's', m_n1, m_n2 };
//    Pri_Str(4, pri_str, 0);
//}

//void printerserialport::waiting()
//{
//    QThread::currentThread()->msleep(50);
//}
