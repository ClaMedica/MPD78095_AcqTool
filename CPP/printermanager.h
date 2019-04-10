#ifndef printermanager_H
#define printermanager_H

#include <QObject>
#include "datafilemanager.h"
#include <QQuickItemGrabResult>
#include "stdint.h"
#include <QFont>


#define NUMOF_X_PRINT_DOTS  752 // (96 mm - 2mm dovuti agli assi) * 8 bit al mm = 94 * 8 = 752

#define FREQ_ACQ      		10 		// in Hz -  frequenza di campionamento canale di volume
#define FREQ_EMG_ACQ  		100		// in Hz -  frequenza di campionamento canale di emg
#define MAX_DURATA_ESAME	20

#define PORTRAIT_MODE 		false
#define LANDSCAPE_MODE 		true

#define WIN             5



typedef struct {
    QString     label;
    int         size;
    int         weight;
    int         charW;
    int         charH;
    QFont       qf;
} T_Font;

class printermanager : public QObject
{
    Q_OBJECT
public:
    explicit printermanager(QString __namefile, QObject *parent = 0);
    ~printermanager();

    void imageFontInit(T_Font &font, QString label, int size, QFont::Weight weight = QFont::Normal, bool fixedPitch = true);
    void imageSetFont(T_Font &font);
    void imageTestFont();
    void imageInit();
    void imagePrintPixLine(uchar *p, int sz);
    void imagePrintForward(int __dotlines);
    void imagePrintStr(char *__str, int __str_len);
    void imagePrint();
    void imageGraph(QString head, QString baset, double xscale, int maxL, int maxR, double *bufL, double *bufR);
    void imageGraphSingle(QPoint leftBottom, QPoint *pts, int npts, double kx, double ky, double *bufV);
    void imageText(QString txt, int fontSize, bool restoreFont);
    void print(QString __datiCalib);

    void setTempoAttesa(float __val)            {m_tem_att = __val;}
    void setFlussoMax(float __val)              {m_flu_max = __val;}
    void setFlussoMedio(float __val)            {m_flu_med = __val;}
    void setTempoMax(float __val)               {m_tem_max = __val;}
    void setTempo595(float __val)               {m_tem_595 = __val;}
    void setTempoSvuot(float __val)             {m_tem_svu = __val;}
    void setTempoFlusso(float __val)            {m_tem_flu = __val;}
    void setTempoDisc(float __val)              {m_tem_dis = __val;}
    void setAccelerazione(float __val)          {m_flu_acc = __val;}
    void setVolFlussoMax(float __val)           {m_vol_max = __val;}
    void setVolVuotato(unsigned int  __val)     {m_vol_vuo = __val;}
    void setFlussoCor(float  __val)             {m_cQ = __val;}
    void setVolRes(unsigned int  __val)         {m_resVol = __val;}
    void setDetContrMax(float  __val)           {m_vDetMax = __val;}

    void setMode(unsigned char   __val)         {m_modal_e = __val;}
    void setPrintSiroky(bool __val)             {m_printSiroky = __val;}
    void setPrintLiverpool(bool __val)          {m_printLiverpool = __val;}
    void setPrintModeUser(bool __val)           {m_printModeUser = __val;}
    void setTipoEsame(unsigned char __val)      {m_test_type = __val;}

    void Report_BitMap(bool __isSiro = false);
    void getGrabbedImage(QObject *__gi, QString __nome);
    void getImage(QImage __img, QString __nome);
    void setPrintHeaders(QString __first, QString __second)     {m_printFirstHeader = __first; m_printSecondHeader = __second;}

    T_Font       tf_current,
                 tf_base,
                 tf_header22,
                 tf_header28,
                 tf_header20,
                 tf_graphPortr,
                 tf_graphLand,
                 tf_infoLabel,
                 tf_infoValue,
                 tf_header,
                 tf_calDate;

    QSize        imageQsz;
    QImage       imageBm;
    QPainter    *imagePainter;
    QPoint       imagePt;
    QRect        imageRectFlw;
    QRect        imageRectEmg;

signals:

public slots:

private:
    DatafileManager *m_dfm;
    QFile     * m_file;
    QString     m_namefile;
    QString     m_namefilePrn;

    bool    m_printMode;
    bool    m_printModeUser;
    bool    m_printSiroky;
    bool    m_printLiverpool;

    QString m_printFirstHeader;
    QString m_printSecondHeader;

    //dati paziente
    QString m_name;
    QString m_surname;
    QString m_dateofbirth;
    QString m_sex;				// M o F
    QString m_ID;

    //dati esame
    int     m_numchan;  			// 1(volume/flusso) 2(volume/flusso + EMG)(+ eventualmente finestringa)
    QString m_timeofstart;	//ora inizio esame (hhmmss)
    QString m_dateofexam;		// data del test
    int     m_durata;				// durata esame in secondi
    int     m_num_VOLsample;  	// scrive il val count_sample, che serve in print report
    bool    m_test_type;			// discrimina se esame veloce oppure completo di dati paziente (servira nel report di stampa)
    int     m_numTest;
    QString m_datiCalib;

    int     m_num_sam; //numero campioni minimo nas (vol)

    //dati risultati analisi
    float   m_tem_att;              /* Tempo di Attesa          (Decimi di Sec) */
    float   m_flu_max;              /* Flusso Massimo           (ml/sec)        */
    float   m_flu_med;              /* Flusso Medio             (ml/sec)        */
    float   m_tem_max;              /* Tempo al Flusso Massimo  (Decimi di Sec) */
    float   m_tem_595;              /* Tempo tra 5% e 95%       (Decimi di Sec) */
    float   m_tem_flu;              /* Tempo di Flusso          (Decimi di Sec) */
    float   m_tem_dis;              /* Tempo di Discesa         (Decimi di Sec) */
    float   m_tem_svu;              /* Tempo di Svuotamento     (Decimi di Sec) */
    float   m_vol_max;              /* Volume al Flusso Massimo (ml)            */
    float   m_cQ;                   /* Flow corrective factor  */
    unsigned int  m_vol_vuo;              /* Volume Vuotato           (ml)            */
    float   m_flu_acc;              /* Accelerazione Flusso     (ml/sec2/10)    */
    unsigned int    m_resVol;                /*residual volume inserted by the user (ml) */
    float   m_vDetMax;              /* detrusor contraction maximum speed  */

    unsigned char   m_modal_e;				 /* Modalita dell'esame 0=auto; 2=manual	 */

    double *buffer_vol;		// da questo buffer la vengono raccolti i dati poi stampati nel report modalita vecchio Picoflow
    double *buffer_flw;    	// buffer per il report che contiene invece i dati di flusso
    double *buffer_emg;

    int     m_chVol;
    int     m_chFlw;
    int     m_chEmg;

    //report
    int     m_realDots;
    double  m_max_vol, m_max_emg;
    bool    m_emgPresent;
    int     m_i_max_x;
    int     m_max_y, m_max_y_gr2;

    int        m_resultBm_w, m_resultBm_h;
    QByteArray m_bitmapSiroky;
    QByteArray m_bitmapLiverpool;

    void smooting_PRINT_flow();
    void pri_rep_review();
    //report
    void Pri_Rep(double xscale);
    void Intest();
    void Report_result();
    void Report_row(QString label, QString value);
    void Report_data();
    void Report_flw(double xscale);
    void Report_emg(double xscale);
    void Report_Real_Time(double xscale);
    void Report_Real_TimeSingle(QString msg, QPoint *points, double xscale, int dotXtratt, int boxW, int y0, int n_dots, int n_label, int max_val, double * buffer, QString txt);

    void Pri_Rep_Gra(int __num_riga);
    void Pri_Rep_Gra_EMG(int __num_riga);

    void Gra_Line();
    short adatta_buffer_dati(int __num_sample, long __fs_flw);
    void Calc_Max_RealReport_rel2(short __num_sample);
    void Calc_Max(double xscale);
    void Calc_Max_EMG();
    long Calc_Max_Flw();
};

#endif // printermanager_H
