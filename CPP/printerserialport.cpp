#include "printerserialport.h"
#include <QDebug>
#include <QThread>

printerserialport::printerserialport(QObject *parent) : QObject(parent)
{
//    m_serialPort.setPortName("/dev/ttyUSB0");
    m_serialPort.setPortName("/dev/ttyS0");
    m_serialPort.setBaudRate(115200);
    m_serialPort.setFlowControl(QSerialPort::HardwareControl);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setStopBits(QSerialPort::OneStop);

    if ( ! m_serialPort.open(QIODevice::ReadWrite))
        qWarning() << "Unable to open serial port";
}

void printerserialport::init_printer()
{
    system("/root/PicoFlow/initprinter.sh");
    Pri_Reset();
//    Pri_Default();
    m_serialPort.flush();
}

void printerserialport::closeSerialPort()
{
    if (m_serialPort.isOpen())
        m_serialPort.close();
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

/*
   Invia alla Stampante "num_car" caratteri della stringa "str_pri"
   Se "flag_lf" invia il carattere LF alla fine
*/
bool printerserialport::Pri_Str(int m_num_car, char *m_str_pri, unsigned char m_flag_lf)
{
#ifdef  _PRISTR_SPLIT_
    static bool recurse = false;
    unsigned char * p = (unsigned char *) m_str_pri;
    int    n = m_num_car;
    if((recurse == false) && (p[0] == 0x1b) && (p[1] == 0x2a)) {
        int ltot = p[2] | (p[3] << 8) | (p[4] << 16);
        int lrow = p[7];
        if((n == (ltot + 8)) && (ltot > lrow)) {
            m_str_pri[2] = lrow;
            m_str_pri[3] = 0;
            m_str_pri[4] = 0;
            p = (unsigned char *)m_str_pri + 8;

#ifdef _HDUMP_
            FILE * fp = fopen("/home/gio/pritrace", "a");
            fprintf(fp, "\n{\nrecurse pristr() n:%d sz:%d, l:%d\n", n,ltot,lrow);
            fclose(fp);
#endif

            recurse = true;
            {
                while(ltot > lrow && ltot > 0) {
                    Pri_Str(8, m_str_pri, 0);
                    Pri_Str(lrow, (char *)p, 0);
                    p    += lrow;
                    ltot -= lrow;
                }
                if(ltot > 0) {
                    m_str_pri[2] = ltot;
                    Pri_Str(8, m_str_pri, 0);
                    Pri_Str(ltot, (char *)p, 0);
                }
            }
            recurse = false;

#ifdef _HDUMP_
            fp = fopen("/home/gio/pritrace", "a");
            fprintf(fp, "\n}\n");
            fclose(fp);
#endif

            m_serialPort.flush();
            return true;
        }
    }
#endif

#ifdef  _PRISTR_BINDUMP_
    {
        FILE * fp = fopen("/tmp/printer", "a");
        fwrite(m_str_pri, m_num_car, 1, fp);
        if(m_flag_lf) {
            char s[1] = { LF };
            fwrite(s, 1, 1, fp);
        }
        fclose(fp);
    }
#endif

#ifdef  _HDUMP_
    {
        char * p = m_str_pri;
        int    n = m_num_car;
        FILE * fp = fopen("/home/gio/pritrace", "a");
        fprintf(fp, "%c[%4.4d] '%s'\n", recurse ? '!' : ' ', m_num_car, m_flag_lf ? "LF" : "nolf");
        if(p[0] == 0x1b && p[1] == 0x2a) {
            int z = hdump(fp, p, 8);
            p += z;
            n -= z;
        }
        hdump(fp, p, n);
        fclose(fp);
    }
#endif

//    while((m_serialPort.pinoutSignals() & QSerialPort::ClearToSendSignal) == 0)
//        QThread::currentThread()->msleep(10);

    m_serialPort.write(m_str_pri, m_num_car);

    if(m_flag_lf) {
        char s[1] = { LF };
        m_serialPort.write(s, 1);
    }

#if 1
    while(m_serialPort.waitForBytesWritten(100) == false)
        ;
    m_serialPort.flush();
#else
#ifdef  _PRISTR_SPLIT_
    if(recurse == false)
#endif
        m_serialPort.flush();
#endif

    return true;
}


void printerserialport::Pri_justif(char m_mode)
{
    /* setta la giustificazione del testo*/
    // 0 centered; 1 right justified; 2 left justified

    char pri_str[] = { ESC, 'C', m_mode} ;
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_mode(char m_mode)
{
    /* imposta la modalita: default, double, quadruple, underlined */
    // mode = 0x00 = 00000000 : default
    // mode = 0xYY = X0XX0XX0 : <underlined> <0> <double w.> <double h.> <0> <quadruple w.> <quadruple h.> <0> */

    char pri_str[] = { ESC, '!', m_mode };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_forward(char m_dotlines)
{
    /* fa avanzare di "dotlines" righe la carta*/

    char pri_str[] = { ESC, 'J', m_dotlines };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_Reset()
{
    /*    resetta la stampante e la sua RAM, equivale ad un reset HW   */

//    char pri_str[] = { ESC, '@' };

//    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
//    Pri_Str(sizeof(pri_str), pri_str, 0);
//    m_serialPort.setFlowControl(QSerialPort::HardwareControl);
}

void printerserialport::Pri_Font(char m_font)
{
    /*Imposta il font desideratp della Printer   */
    /*    0 - 8x16  1 - 12x20 2 - 7x16  */

    char pri_str[] = { ESC, '%', m_font };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::Pri_Intensity(char m_intens)
{
//   intens = 0x80 default	0x00 < intens < 0xFF
//   intens < 0x80 lighter print
//   intens > 0x80 higher print

    char pri_str[] = { GS, 'D', m_intens };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}


void printerserialport::Pri_Default()
{
//    char pri_str[] = { ESC, 'd' };
//    Pri_Str(sizeof(pri_str), pri_str, 0);
}

/* numero di pixel scaldati contemporaneamente --> (n+1)*8
    formula per consumo di corrente massimo Ca = 0,3 + V*(n+1)*8/Rdot
    dato il consumo in ampere desiderato Ca, la velocita massima si ha per n = ((Ca-0,3)*Rdot/(V*8) -1
    Ca = 1.4A @5V --> n =
*/    // default n = 5
void printerserialport::Pri_Speed(char m_speed )
{
    char pri_str[] = { GS, '/', m_speed };     // 1 <= speed <= 32, 0 max speed
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

/* setta la massima velocita di stampa settando il tempo di avanzamento
    T = 256*n1 + n2
    2080 < T < 25000 [us]
    vel[mm/sec] = 1/(8*T)   5 < vel < 60 [mm/sec]
 */
void printerserialport::Pri_Max_Speed(char m_n1, char m_n2)
{
    char pri_str[] = { GS, 's', m_n1, m_n2 };
    Pri_Str(sizeof(pri_str), pri_str, 0);
}

void printerserialport::waiting()
{
    QThread::currentThread()->msleep(50);
}
