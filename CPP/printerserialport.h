#ifndef PrinterSerialPort_H
#define PrinterSerialPort_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>

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

//    void init_printer();
//    int status(bool print = true);

    bool Pri_Str(int __num_car, char *__str_pri, unsigned char __flag_lf);
    void Pri_justif(char __mode);
    void Pri_mode(char __mode);
    void Pri_forward(char __dotlines);
//    void Pri_Reset();
    void Pri_Font(char __font);
    void Pri_Intensity(char __intens);
//    void Pri_Default();
//    void Pri_Speed( char m_speed );
//    void Pri_Max_Speed(char m_n1, char m_n2);
//    void waiting();

signals:

public slots:

private:
    QSerialPortInfo *m_serialPortInfo;
//    QSerialPort m_serialPort;
    QFile  *m_file;
};

#endif // PrinterSerialPort_H
