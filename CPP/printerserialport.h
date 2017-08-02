#ifndef PrinterSerialPort_H
#define PrinterSerialPort_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/*
   Codici Ascii
*/
#define LF                0x0A            /* Line Feed        */
#define ESC               0x1B            /* Escape           */
#define GS				  0x1D			  /* SET impostazione */

class printerserialport : public QObject
{
    Q_OBJECT
public:
    explicit printerserialport(QObject *parent = 0);
    void closeSerialPort();

    void init_printer();
    int status(bool print = true);

    bool Pri_Str(int m_num_car, char *m_str_pri, unsigned char m_flag_lf);
    void Pri_justif(char m_mode);
    void Pri_mode(char m_mode);
    void Pri_forward(char m_dotlines);
    void Pri_Reset();
    void Pri_Font(char m_font);
    void Pri_Intensity(char m_intens);
    void Pri_Default();
    void Pri_Speed( char m_speed );
    void Pri_Max_Speed(char m_n1, char m_n2);
    void waiting();

signals:

public slots:

private:
    QSerialPortInfo *m_serialPortInfo;
    QSerialPort m_serialPort;


};

#endif // PrinterSerialPort_H
