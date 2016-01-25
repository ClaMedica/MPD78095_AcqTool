#include "printerserialport.h"
#include <QDebug>

printerserialport::printerserialport(QObject *parent) : QObject(parent)
{
    m_serialPortInfo = new QSerialPortInfo;
    QList<QSerialPortInfo> list;
    list = m_serialPortInfo->availablePorts();

    m_serialPort.setPortName("COM3");
    m_serialPort.setBaudRate(9600);
    m_serialPort.setFlowControl(QSerialPort::HardwareControl);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setStopBits(QSerialPort::OneStop);

    if (!m_serialPort.open(QIODevice::ReadWrite))
        qCritical()<<"Impossible to open serial port";
//    else
//    {
//        char pri_str[1];
//        pri_str[0]= LF;
//        int bt = m_serialPort.write(pri_str);
//        m_serialPort.flush();
//    }


}

void printerserialport::init_printer()
{
    Pri_Reset();
    Pri_Default();
}

void printerserialport::closeSerialPort()
{
    if (m_serialPort.isOpen())
        m_serialPort.close();
}


bool printerserialport::Pri_Str(int m_num_car, char *m_str_pri, unsigned char m_flag_lf)
{
   /*
      Invia alla Stampante "num_car" caratteri della stringa "str_pri"
      Se "flag_lf" invia il carattere LF alla fine
   */

    m_serialPort.write(m_str_pri,m_num_car);
    if(m_flag_lf)
    {
        char s[1];
        s[0] = LF;
        m_serialPort.write(s,1);
    }

    m_serialPort.flush();
    return true;
}


void printerserialport::Pri_justif(char m_mode)
{/* setta la giustificazione del testo*/
// 0 centered; 1 right justified; 2 left justified
    char pri_str[3];
    pri_str[0]=ESC;
    pri_str[1]='C'; 	// 0x43
    pri_str[2]=(char)m_mode;
    Pri_Str(3,pri_str,0);
}

void printerserialport::Pri_mode(char m_mode)
{	/* imposta la modalità: default, double, quadruple, underlined */
    // mode = 0x00 = 00000000 : default
    // mode = 0xYY = X0XX0XX0 : <underlined> <0> <double w.> <double h.> <0> <quadruple w.> <quadruple h.> <0> */
    char pri_str[3];
    pri_str[0]=ESC;
    pri_str[1]='!'; 	// 0x21
    pri_str[2]=(char)m_mode;
    Pri_Str(3,pri_str,0);
}

void printerserialport::Pri_forward(char m_dotlines)
{/* fa avanzare di "dotlines" righe la carta*/
    char pri_str[3];
    pri_str[0]=ESC;
    pri_str[1]='J';
    pri_str[2]=(char)m_dotlines;
    Pri_Str(3,pri_str,0);
}

void printerserialport::Pri_Reset()
{
   /*    resetta la stampante e la sua RAM, equivale ad un reset HW   */
   unsigned char pri_tip;
   char pri_str[3];
   pri_str[0]=ESC;
   pri_str[1]='@'; 	// 0x40
   pri_str[2]=(char)pri_tip;
   Pri_Str(3,pri_str,0);
}

void printerserialport::Pri_Font(char m_font)
{ /*Imposta il font desideratp della Printer   */
    /*    0 - 8x16  1 - 12x20 2 - 7x16  */
   char pri_str[3];
   pri_str[0]=ESC;
   pri_str[1]='%'; 	// 0x25
   pri_str[2]=(char)m_font;
   Pri_Str(3,pri_str,0);
}

void printerserialport::Pri_Intensity(char m_intens)
{
/* intens = 0x80 default	0x00 < intens < 0xFF
   intens < 0x80 lighter print
   intens > 0x80 higher print
*/
    char pri_str[3];
    pri_str[0]=GS;
    pri_str[1]='D'; 	// 0x44
    pri_str[2]=(char)m_intens;
    Pri_Str(3,pri_str,0);
}


void printerserialport::Pri_Default()
{
    unsigned char pri_tip;
    char pri_str[3];
    pri_str[0]=ESC;
    pri_str[1]='d';
    pri_str[2]=(char)pri_tip;
    Pri_Str(3,pri_str,0);
}

/* numero di pixel scaldati contemporaneamente --> (n+1)*8
    formula per consumo di corrente massimo Ca = 0,3 + V*(n+1)*8/Rdot
    dato il consumo in ampere desiderato Ca, la velocità massima si ha per n = ((Ca-0,3)*Rdot/(V*8) -1
    Ca = 1.4A @5V --> n =
*/    // default n = 5
void printerserialport::Pri_Speed(char m_speed )
{
    char pri_str[3];
    pri_str[0]=GS;
    pri_str[1]='/';     // 0x44
    pri_str[2]=(char)m_speed;     // 1<= speed <= 32, 0 max speed
    Pri_Str(3,pri_str,0);
}

/* setta la massima velocità di stampa settando il tempo di avanzamento
    T = 256*n1 + n2
    2080 < T < 25000 [us]
    vel[mm/sec] = 1/(8*T)   5 < vel < 60 [mm/sec]
 */
void printerserialport::Pri_Max_Speed(char m_n1, char m_n2)
{
    char pri_str[4];
    pri_str[0]=GS;
    pri_str[1]='s';
    pri_str[2]=(char)m_n1;
    pri_str[3]=(char)m_n2;
    Pri_Str(4,pri_str,0);
}

