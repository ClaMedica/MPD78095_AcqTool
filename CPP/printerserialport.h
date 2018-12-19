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
    explicit printerserialport(QString __namefile, QObject *parent = 0);
    void closeSerialPort();
    void flushFile()    { m_file->flush(); m_file->seek(0); }

    bool Pri_Str(char *__str, int __str_len, bool __flag_lf);
    void Pri_justif(char __mode);
    void Pri_mode(char __mode);
    void Pri_forward(char __dotlines);
    void Pri_Font(char __font);
    void Pri_Intensity(char __intens);

private:
    QFile  *m_file;
};

#endif // PrinterSerialPort_H
