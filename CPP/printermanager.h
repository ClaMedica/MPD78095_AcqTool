#ifndef printermanager_H
#define printermanager_H

#include <QObject>
#include "datafilemanager.h"
#include "printerserialport.h"
#include <QQuickItemGrabResult>
#include "stdint.h"
#include <QImageWriter>


#define     NUMOF_X_PRINT_DOTS 752 // (96 mm - 2mm dovuti agli assi) * 8 bit al mm = 94 * 8 = 752
#define 	NUMOFMAX_LEGHT_PORTRAIT_REP 3000	// 10 sample/sec x 5min = 3000

#define NUM_POINTS	40 // nella versione nuova che corrispondono a 5sec, infatti 40/8 = 5 considerato che 8punti/mm e 8sample/sec

#define 	Fc_FLW        		10   	// frequenza di calcolo del flusso
#define 	FREQ_ACQ      		10 		// in Hz -  frequenza di campionamento canale di volume
#define 	FREQ_EMG_ACQ  		100		// in Hz -  frequenza di campionamento canale di emg
#define 	MAX_DURATA_ESAME	20

#define 	PORTRAIT_MODE 		false
#define 	LANDSCAPE_MODE 		true

#define WIN			5

// un po di costanti per la stampa dei label di flw, emg e vol
#define _NUM_CHAR_X_LABEL	4
#define _NUM_CHAR_X_LABEL_rel2 8
#define _NUM_CHAR_X_LABEL_VOL 5
#define _NUM_CHAR_X_AXES_DX 1
#define _n_char_time		3
#define _POS_LABEL_FLW 0x35		// posizione in esadecimale lungo la larghezza della carta: dal 55-esimo byte sui 104 disponibili
#define _POS_FC_FLW	100		// posizione in numero di byte dull'asse per il FS del flusso
#define _POS_FC_FLW_rel2 	_POS_FC_FLW
#define _POS_FC_only_FLW	99
#define _POS_FC_only_FLW_rel2	_POS_FC_FLW_rel2
#define _POS_UDM_FLW	(_POS_FC_FLW - 2)		// posizione in numero di byte sull'asse ordinate per l'udm del flusso
#define _POS_UDM_only_FLW (_POS_FC_only_FLW - 2)
#define _POS_FS_EMG	49
#define _POS_FS_EMG_rel2 24
#define _POS_UDM_EMG	(_POS_FS_EMG - 2)
#define _POS_FS_VOL		_POS_FC_FLW
#define _POS_FS_VOL_rel2	49
#define _POS_FS_only_VOL_rel2 24
#define _POS_UDM_VOL	(_POS_FS_VOL - 2)
#define _WIDTH_LABEL_FLW 0x32	// larghezza in esadecimale dei byte dell'immagine grafica che stampa i label asse ordinate: 50 byte
#define _NUM_LABEL		5	// 5 valori di label, tutti i fondoscala sono divisibili per 5, cosi ho sempre valori interi
#define _NUM_LABEL_only_FLW 10
#define _NUM_LABEL_VOL_EMG	3	// ci sono 3 grafici, quindi solo 2 laberl + lo zero per il volume
#define _NUM_LABEL_EMG			2	// ci sono 3 grafici, solo il label di mezza scala per l'emg + lo zero

// caratteristiche del font dei caratteri usati per i label (ruotati di 90?)
#define _HEIGHT_CHAR_LABEL 8
#define _WIDTH_CHAR_LABEL  8

#define _NUM_BYTE_CMD 8 // numero di char del comando di stampa grafica previsto dal protocollo APS
#define LCMD 8 //char per comando
#define CLS 4		// 4 caratteri anche per il label sinistro
#define CLD 4		// 0 caratteri per label destro, che nel grafico ruotato ora non c'e, ma che potrebbe essere l'intestazione del grafico
#define w_flw	94	// report normale
#define a_flw	( CLS*24 + w_flw*24 + CLD*24)	// area in byte del grafico di flusso
#define a_emg	a_flw			// scrivo i label in 4 cifre e non solo 3


#define dim_string_rel2 ((50 + 50 + 1)* _HEIGHT_CHAR_LABEL * _NUM_CHAR_X_LABEL_rel2 + _NUM_BYTE_CMD) // 6472 byte complessivi
#define dim_string_gr  ((50 + 50 + 1 + 1 + 1 + 1) * NUM_POINTS +_NUM_BYTE_CMD)	// 4168byte,  numero di byte dello stringone
#define dim_string_solo_ax ((50 + 50 + 1 + 1)* _HEIGHT_CHAR_LABEL * _NUM_CHAR_X_AXES_DX + _NUM_BYTE_CMD) // 816 byte complessivi

#define w_ave	45		// larghezza grafico siroky
#define w_max	w_ave
#define w_SD 1
#define a_ave	( (CLS-1)*24 + w_ave*24 + w_SD*24)// area in byte del grafico siroky average
#define a_max	a_ave
#define pos_gra ( LCMD + CLS*24 )
#define pos_lab_vol ( pos_gra + 24*94 ) // prima colonna libera a dx del grafico
#define pos_lab_SD_ave	( LCMD + (CLS-1)*24 + w_ave*24 )
#define pos_lab_Q_max	( pos_lab_SD_ave + w_SD*24 )
#define pos_lab_SD_max	( pos_lab_Q_max + (CLS-1)*24 + w_max*24 )
#define pos_gra_Qmed	( LCMD + (CLS-1)*24 )
#define pos_gra_Qmax ( pos_gra_Qmed + a_ave )

#define w_ave	45		// larghezza grafico siroky
#define w_max	w_ave
#define w_SD 1
#define a_ave	( (CLS-1)*24 + w_ave*24 + w_SD*24)// area in byte del grafico siroky average
#define a_max	a_ave

#define __ZERO_ASSE_FLUSSO__
#define __ZERO_ASSE_EMG__
#define __ZERO_ASSE_SIROKY__
#define __DATI_GRAF_FLUSSO__

#define __DEV_STD__
#define __LABEL_SD__
#define __POINT__

#define LINE                "\n\n"          // "\x1b""d"
#define LINE2               (char *) "\n\n"

#define F_center		0x00
#define F_right			0x01
#define F_left			0x02

/*
   Codici Ascii
*/
#define LF                0x0A            /* Line Feed                      */
#define ESC               0x1B            /* Escape                         */
#define GS				  0x1D			  /* SET impostazione				*/



typedef struct{
    int uw8;
    int uw9;
    int sample_adattato;
    bool verso_curva;
} Print_Graph_Parameters; 		// usata nella funzione di creazione stringa per stampa grafici


class printermanager : public QObject
{
    Q_OBJECT
public:
    explicit printermanager(QString __namefile, QObject *parent = 0);
    explicit printermanager(/*QObject *parent = 0*/) {}
    ~printermanager();

    void imageInit();
    void imagePrint();
    void imageGraph(QString head, QString baset, int maxL, int maxR, double *bufL, double *bufR);
    void imageText(QString txt, int fontSize, bool restoreFont);
    void print();
    void closePrinter();

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

    void set_gra_Header_str_gr(int __sz, int __n4, int __dots);
    void printDigits(int __val, int __ndigits, int __pos);
    void printNumber(int __val, int __ndigits, int __pos, const unsigned char *fontBm, int fontH, int fontW);
    void printBitMap_unaRigaPerVolta();

    QSize        imageQsz;
    QString      imageFont;
    int          imageFontSize;
    QImage       imageBm;
    QPainter    *imagePainter;
    QPoint       imagePt;
    QRect        imageRectFlw;
    QRect        imageRectEmg;

signals:

public slots:

private:
    printerserialport *m_port;

    QString m_namefile;
    QString m_namefilePrn;
    DatafileManager *m_dfm;

    bool m_printMode;
    bool m_printModeUser;
    bool m_printSiroky;
    bool m_printLiverpool;

    QString m_printFirstHeader;
    QString m_printSecondHeader;

    //dati paziente
    QString m_name;
    QString m_surname;
    QString m_dateofbirth;
    unsigned char  m_sex;				// M o F (+ eventualmente finestringa)
    QString m_ID;

    //dati esame
    int m_numchan;  			// 1(volume/flusso) 2(volume/flusso + EMG)(+ eventualmente finestringa)
    unsigned char  m_scaricato;			// indica se file scaricato o meno - non usato (0000 oppure 0001)(+ eventualmente finestringa)
    QString m_timeofstart;	//ora inizio esame (hhmmss)
    QString m_dateofexam;		// data del test
    int m_durata;				// durata esame in secondi
    int m_num_VOLsample;  	// scrive il val count_sample, che serve in print report
    bool m_test_type;			// discrimina se esame veloce oppure completo di dati paziente (servira nel report di stampa)
    int m_numTest;

    int m_num_sam; //numero campioni minimo nas (vol)

    //dati risultati analisi
    float  m_tem_att;              /* Tempo di Attesa          (Decimi di Sec) */
    float  m_flu_max;              /* Flusso Massimo           (ml/sec)        */
    float  m_flu_med;              /* Flusso Medio             (ml/sec)        */
    float  m_tem_max;              /* Tempo al Flusso Massimo  (Decimi di Sec) */
    float  m_tem_595;              /* Tempo tra 5% e 95%       (Decimi di Sec) */
    float  m_tem_flu;              /* Tempo di Flusso          (Decimi di Sec) */
    float  m_tem_dis;              /* Tempo di Discesa         (Decimi di Sec) */
    float  m_tem_svu;              /* Tempo di Svuotamento     (Decimi di Sec) */
    float  m_vol_max;              /* Volume al Flusso Massimo (ml)            */
    float  m_cQ;                   /* Flow corrective factor  */
    unsigned int  m_vol_vuo;              /* Volume Vuotato           (ml)            */
    float  m_flu_acc;              /* Accelerazione Flusso     (ml/sec2/10)    */
    unsigned int    m_resVol;                /*residual volume inserted by the user (ml) */
    float m_vDetMax;              /* detrusor contraction maximum speed  */

    unsigned char   m_modal_e;				 /* Modalita dell'esame 0=auto; 2=manual	 */

    double *buffer_vol;		// da questo buffer la vengono raccolti i dati poi stampati nel report modalita vecchio Picoflow
    double *buffer_flw;    	// buffer per il report che contiene invece i dati di flusso
    double *buffer_emg;

    int m_chVol;
    int m_chFlw;
    int m_chEmg;

    //report
    unsigned short m_realDots;
    unsigned short 	m_PointsToPrintout; //Numero di campioni da stampare ottenuto lanciando a fine esame SamplesToPrint
    double m_max_vol, m_max_emg;
    bool m_emgPresent;
    int         	m_i_max_x;
    long  			m_max_y,m_max_y_gr2;
    int             m_x1,m_y1,m_x2,m_y2;
    int   			m_num_xy;
    int   	m_x[25];
    int   	m_y[25];
    unsigned short 	m_uw3; // dimensioni in byte della riga n-esima dell'area grafico
    char 	m_str_gr[dim_string_rel2];	// allocazione dinamica ma all'inizio dello stringone che sara usato per i label dei grafici
    char 	m_str_tr[dim_string_rel2];			// trasposizione stringa per ottenere matrice di punti per stampa grafica
    int m_init_time_to_print;
    short m_num_byte_x_gra;
    short m_pos_gra_flw,m_pos_gra_emg, m_pos_gra_vol;
    short m_num_dots_gra_vol, m_num_dots_gra_flw, m_num_dots_gra_emg;
    short m_num_byte_x_gra_emg, m_num_byte_x_gra_vol, m_num_byte_x_gra_flw;
    unsigned short m_cursore;

    int         m_resultBm_w, m_resultBm_h;
    QByteArray m_bitmapSiroky;
    QByteArray m_bitmapLiverpool;

    void smooting_PRINT_flow();
    void pri_rep_review();
    //report
    void Pri_Rep(double xscale);
    void Intest();
    void Report_data();
    void Report_flw(double xscale);
    void Report_emg();
    void Report_Real_Time(short __num_sample, double xscale);

    void Report_result();
    void Pri_Rep_Gra(int __num_riga);
    void Pri_Rep_Gra_Ini_Grid(int __n_riga);
    void Pri_Rep_Gra_EMG(int __num_riga);
    void Pri_Rep_Gra_Landscape(short __num_sample, double xscale);
    void Pri_Rep_asse_dx();
    void Pri_Rep_Label();
    void Pri_Rep_Lin(QString __descr, int __rep_dat, int __num_dec, const char *__str_udm, bool __flag_lf);

    unsigned char  Int_Pun(int __i4);
    void Gra_Line();
    void Str_Trasposta(unsigned char __type, unsigned short __sx_byte, unsigned short __dx_byte); // crea la trasposta della stringa str_gr, ottenendo una matrice scritta per righe
    short adatta_buffer_dati(int __num_sample, long __fs_flw);
    void Calc_Max_RealReport_rel2(short __num_sample);
    void Calc_Max(double xscale);
    void Calc_Max_EMG();
    long Calc_Max_Flw();
    void print_char_left_label(int __value, int __pos_in_string, int __num_char, bool __pri_decim, int __pre_char);
    void print_udm_label(int __ch_type, int __pos_in_string, int __pre_char);
    bool check_stamp_label();

};

#endif // printermanager_H
