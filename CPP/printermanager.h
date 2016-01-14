#ifndef PRINTERMANAGER_H
#define PRINTERMANAGER_H

#include <QObject>
#include "datafilemanager.h"
#include "printerserialport.h"

#define     NUMOF_X_PRINT_DOTS 752 // (96 mm - 2mm dovuti agli assi) * 8 bit al mm = 94 * 8 = 752
#define 	NUMOFMAX_LEGHT_PORTRAIT_REP 3000	// 10 sample/sec x 5min = 3000

#define NUM_POINTS	40 // nella versione nuova che corrispondono a 5sec, infatti 40/8 = 5 considerato che 8punti/mm e 8sample/sec

#define 	Fc_FLW        			10   	// frequenza di calcolo del flusso
            #define 	FREQ_ACQ      		10 		// in Hz -  frequenza di campionamento canale di volume
#define 	FREQ_EMG_ACQ  		100		// in Hz -  frequenza di campionamento canale di emg
#define 	MAX_DURATA_ESAME	20

#define 	PORTRAIT_MODE 		0
#define 	LANDSCAPE_MODE 		1

#define WIN			5

// un pò di costanti per la stampa dei label di flw, emg e vol
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
#define _NUM_LABEL		5	// 5 valori di label, tutti i fondoscala sono divisibili per 5, così ho sempre valori interi
#define _NUM_LABEL_only_FLW 10
#define _NUM_LABEL_VOL_EMG	3	// ci sono 3 grafici, quindi solo 2 laberl + lo zero per il volume
#define _NUM_LABEL_EMG			2	// ci sono 3 grafici, solo il label di mezza scala per l'emg + lo zero

// caratteristiche del font dei caratteri usati per i label (ruotati di 90°)
#define _HEIGHT_CHAR_LABEL 8
#define _WEIGHT_CHAR_LABEL 8
#define negativo  0
#define positivo 1

#define _NUM_BYTE_CMD 8 // numero di char del comando di stampa grafica previsto dal protocollo APS
#define LCMD 8 //char per comando
#define CLS 4		// 4 caratteri anche per il label sinistro
#define CLD 4		// 0 caratteri per label destro, che nel grafico ruotato ora non c'è, ma che potrebbe essere l'intestazione del grafico
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
#define pos_gra 	( LCMD + CLS*24 )
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
#define __DATI_GRAF_EMG__

#define __DEV_STD__
#define __LABEL_SD__
#define __POINT__

//per stampare
#define PRI_REP_INT     1    /* Flag Stampa Report : Intestazione          */
#define PRI_REP_IDE     1   /* Flag Stampa Report : Identificativi Esame  */
#define PRI_REP_MODAL	1	  /* Flag stampa Report : modalità d'esame		*/
#define PRI_REP_GRA     1     /* Flag Stampa Report : Grafico               */
#define PRI_REP_SRK     1     /* Flag Stampa Report : Siroky                */
#define PRI_REP_RIS     1     /* Flag Stampa Report : Risultati             */
#define PRI_REP_SPA		1	  /* Flag Stampa Report : Spazi per strappare   */
#define PRI_REP_POS     1     /* Flag Stampa Report : Postfazione e indicazione calibrazione, per debug */
//per le stringhe
#define NON_SOTTLINEA   "\x1bU\x0"	// \x 1B U \x 0 che significa 1B = ESC; U = modo sottolineatura; 0 = stampa normale
#define SOTTLINEA       "\x1bU\x1"
#define NON_RUOTA_90    "\x1bV\x0" // 1B V 0 : ESC V 0 = modo ruotato 90° non attivato
#define RUOTA_90        "\x1bV\x2"
#define REVERS_MODE     "\x1b{\x1"
#define NOT_REVERS_MODE "\x1b{\x0"
#define LINE          	"\x1b""d"
#define F_16x24         "\x1bR\x02"
#define F_24x32         "\x1bR\x03"
#define DUBLE_H_L       "\x1b!\x30"
#define DUBLE_H         "\x1b!\x10"
#define UNDUBLE         "\x1b!\x00"
#define DUBLE_L         "\x1b!\x20"
#define PRI_TIMEOUT     25000

#define F_8x16			0x30
#define F_12x20			0x31
#define F_7x16			0x32
#define F_center		0x30
#define F_right			0x31
#define F_left			0x32

/*
   Codici Ascii
*/
#define LF                0x0A            /* Line Feed                      */
#define ESC               0x1B            /* Escape                         */
#define GS				  0x1D			  /* SET impostazione				*/

#define F_LINE 			"\x0A""d"

#define	LOBYTE(x)             ((byte) ((x) & 0xff))
#define	HIBYTE(x)             ((byte) ((x) >> 8))

const char str_label_time[29][68]={
    "   0    3     6     9    12    15    18    21    24    27   30s",
    "   0    6     12   18    24    30    36    42    48    54   60s",
    "   0    9     18   27    36    45    54    63    72    81   90s",
    "   0    12    24   36    48    60    72    84    96   108  120s",
    "   0    15    30   45    60    75    90   105   120   135  150s",
    "   0    18    36   54    72    90   108   126   144   162  180s",
    "   0    21    42   63    84   105   126   147   168   189  210s",
    "   0    24    48   72    96   120   144   168   192   216  240s",
    "   0    27    54   81   108   135   162   189   216   243  270s",
    "   0    30    60   90   120   150   180   210   240   270  300s",
    "   0    36    72  108   144   180   216   252   288   324  360s",
    "   0    42    84  126   168   210   252   294   336   378  420s",
    "   0    48    96  144   192   240   288   336   384   432  480s",
    "   0    54   108  162   216   260   314   368   422   476  540s",
    "   0    60   120  180   240   300   360   420   480   540  600s",
    "   0    66   132  198   264   330   396   462   528   594  660s",
    "   0    72   144  216   288   360   432   504   586   658  720s",
    "   0    78   156  234   312   390   468   546   624   702  780s",
    "   0    84   168  252   336   420   504   588   668   752  840s",
    "   0    90   180  270   360   450   540   630   720   810  900s",/* 15*60=900*/
    "   0    96   192  288   384   480   576   672   768   864  960s",
    "   0   102   204  306   408   510   612   714   816   918 1020s",
    "   0   108   216  324   432   540   648   756   864   972 1080s",
    "   0   114   228  342   456   570   684   798   912  1026 1140s",
    "   0    2m    4m   6m    8m   10m   12m   14m   16m   18m   20m",
    "   0          9          18          27          36         45s",
    "   0          15         30          45          60         75s",
    "   0          21         42          63          84        105s",
    "   0          27         54          81         108        135s"
};

// numeri ruotati di 90° in senso orario
const byte print8x8Set[] = {
    0x3C, 0x7E, 0x81, 0x81, 0x7E, 0x3C, 0x00, 0x00,      /*   0 - 30 - 0 */
    0x84, 0x82, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x00,      /*   1 - 31 - 1 */
    0xC2, 0xE3, 0xB1, 0x99, 0x8F, 0x86, 0x00, 0x00,      /*   2 - 32 - 2 */
    0x42, 0xC3, 0x81, 0x99, 0xFF, 0x66, 0x00, 0x00,      /*   3 - 33 - 3 */
    0x30, 0x28, 0x24, 0x23, 0xFF, 0x20, 0x00, 0x00,      /*   4 - 34 - 4 */
    0x8F, 0x89, 0x89, 0xD9, 0x71, 0x00, 0x00, 0x00,     /*   5 - 35 - 5 */
    0x7E, 0x8B, 0x89, 0x89, 0xDB, 0x72, 0x00, 0x00,      /*   6 - 36 - 6 */
    0x81, 0xD1, 0x71, 0x39, 0x1D, 0x17, 0x03, 0x00,      /*   7 - 37 - 7 */
    0x66, 0x7E, 0x99, 0x99, 0x7E, 0x66, 0x00, 0x00,     /*   8 - 38 - 8 */
    0x4E, 0xDB, 0x91, 0x91, 0xDB, 0x7E, 0x00, 0x00,     /*   9 - 39 - 9 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0,     /*  .   	10 */
    0xF8, 0x1C, 0x1C, 0xF8, 0xF8, 0x1C, 0x1C, 0xF8,	/* m 	11 */
    0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0x00, 0x00,	/* L 	12 */ /* mL/s*/
    0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x0C,	/* / 	13 */
    0x00, 0x98, 0x9C, 0xB4, 0xB4, 0xE4, 0x64, 0x00,	/* s		14 */
    0x80, 0xFC, 0x7C, 0xE0, 0xC0, 0xE0, 0x7C, 0x3C,	/* u		15 */
    0x07, 0x1E, 0x70, 0xC0, 0xC0, 0x70, 0x1E, 0x07,	/* V 	16 */
    0x00, 0x20, 0x78, 0xa4, 0xa4, 0xa4, 0x18, 0x00,	/* e		17 */
    0x00, 0x00, 0x78, 0x84, 0x84, 0x84, 0x84, 0x00,	/* c		18 */
};
const byte print7x13Set[]  = {
        0x00, 0x00, 0x30, 0x48, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x48, 0x30, 0x00, 0x00,      /*   0 - 30 - 0 */
        0x00, 0x00, 0x30, 0x70, 0xB0, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00, 0x00,      /*   1 - 31 - 1 */
        0x00, 0x00, 0x78, 0xCC, 0xCC, 0x0C, 0x38, 0x60, 0xC0, 0xC0, 0xFC, 0x00, 0x00,      /*   2 - 32 - 2 */
        0x00, 0x00, 0x78, 0xCC, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0xCC, 0x78, 0x00, 0x00,      /*   3 - 33 - 3 */
        0x00, 0x00, 0x0C, 0x1C, 0x3C, 0x6C, 0xCC, 0xCC, 0xFC, 0x0C, 0x0C, 0x00, 0x00,      /*   4 - 34 - 4 */
        0x00, 0x00, 0xFC, 0xC0, 0xC0, 0xF8, 0xCC, 0x0C, 0x0C, 0xCC, 0x78, 0x00, 0x00,      /*   5 - 35 - 5 */
        0x00, 0x00, 0x78, 0xCC, 0xC0, 0xC0, 0xF8, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00,      /*   6 - 36 - 6 */
        0x00, 0x00, 0xFC, 0x0C, 0x0C, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0x00, 0x00,      /*   7 - 37 - 7 */
        0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00,      /*   8 - 38 - 8 */
        0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0x0C, 0xCC, 0x78, 0x00, 0x00,      /*   9 - 39 - 9 */
        };
const byte print8x10SetSD[]  = {
        0x00, 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E,	/* " 0" */
        0x00, 0x02, 0x06, 0x0A, 0x02, 0xF2, 0x02, 0x02, 0x02, 0x07,	/* "-1" */
        0x00, 0x06, 0x09, 0x01, 0x01, 0x72, 0x04, 0x08, 0x08, 0x0F,	/* "-2" */
        0x00, 0x06, 0x09, 0x01, 0x01, 0x77, 0x01, 0x01, 0x09, 0x06,	/* "-3" */
};

const byte SD[2][4][20] =
{{{50, 63, 74, 84, 95,105,114,123,132,140,147,154,160,166,170,174,176,178,178,179},
  {34, 44, 54, 62, 70, 78, 84, 91, 98,104,110,116,121,126,130,133,136,139,141,141},
  {10, 19, 26, 34, 40, 46, 52, 58, 64, 70, 75, 79, 84, 88, 93, 98,102,104,106,106},
  { 0,  0, 10, 16, 21, 26, 30, 34, 37, 40, 43, 47, 50, 53, 55, 58, 60, 62, 64, 66}},
 {{ 0, 96,110,122,134,145,156,166,176,186,195,204,211,217,223,228,231,233,234,234},
  { 0, 80, 91,101,110,119,127,135,143,150,156,162,166,171,174,176,177,178,179,179},
  { 0, 59, 67, 74, 80, 86, 91, 96,100,104,108,111,114,116,118,119,120,121,122,122},
  { 0, 41, 48, 51, 54, 55, 57, 58, 59, 60, 62, 62, 63, 64, 64, 64, 64, 65, 65, 66}}
};

typedef struct{
    int uw8;
    int uw9;
    int sample_adattato;
    bool verso_curva;
} Print_Graph_Parameters; 		// usata nella funzione di creazione stringa per stampa grafici


//extern bool Pri_Str(int num_car, char *str_pri, byte flag_lf);

class printermanager : public QObject
{
    Q_OBJECT
public:
    explicit printermanager(QString __namefile, QObject *parent = 0);

    void print();

    void setTempoAttesa(float __val){m_tem_att = __val;}
    void setFlussoMax(float __val){m_flu_max = __val;}
    void setFlussoMedio(float __val){m_flu_med = __val;}
    void setTempoMax(float __val){m_tem_max = __val;}
    void setTempo595(float __val){m_tem_595 = __val;}
    void setTempoSvuot(float __val){m_tem_svu = __val;}
    void setTempoFlusso(float __val){m_tem_flu = __val;}
    void setTempoDisc(float __val){m_tem_dis = __val;}
    void setAccelerazione(float __val){m_flu_acc = __val;}
    void setVolFlussoMax(unsigned int  __val){m_vol_max = __val;}
    void setVolVuotato(unsigned int  __val){m_vol_vuo = __val;}
    void setMode(byte  __val){m_modal_e = __val;}

    void setTipoEsame(byte  __val){m_test_type = __val;}


signals:

public slots:

private:
    printerserialport *m_port;

    QString m_namefile;
    DatafileManager *m_dfm;

    bool m_printMode;
    bool m_printModeUser;
    bool m_printSyroky;

    //dati paziente
    QString m_name;
    QString m_surname;
    QString m_dateofbirth;
    byte m_sex;				// M o F (+ eventualmente finestringa)
    QString m_ID;

    //dati esame
    int m_numchan;  			// 1(volume/flusso) 2(volume/flusso + EMG)(+ eventualmente finestringa)
    byte m_scaricato;			// indica se file scaricato o meno - non usato (0000 oppure 0001)(+ eventualmente finestringa)
    QString m_timeofstart;	//ora inizio esame (hhmmss)
    QString m_dateofexam;		// data del test
    int m_durata;				// durata esame in secondi
    int m_num_VOLsample;  	// scrive il val count_sample, che serve in print report
    bool m_test_type;			// discrimina se esame veloce oppure completo di dati paziente (servirà nel report di stampa)
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
    unsigned int  m_vol_max;              /* Volume al Flusso Massimo (ml)            */
    unsigned int  m_vol_vuo;              /* Volume Vuotato           (ml)            */
    float  m_flu_acc;              /* Accelerazione Flusso     (ml/sec2/10)    */
    byte  m_modal_e;				 /* Modalità dell'esame 0=auto; 2=manual	 */

    double *buffer_vol;		// da questo buffer la vengono raccolti i dati poi stampati nel report modalità vecchio Picoflow
    double *buffer_flw;    	// buffer per il report che contiene invece i dati di flusso
    double *buffer_emg;

    int m_chVol;
    int m_chFlw;
    int m_chEmg;

    //report
    unsigned short m_realDots;
    bool m_correct;
    unsigned short 	m_PointsToPrintout; //Numero di campioni da stampare ottenuto lanciando a fine esame SamplesToPrint
    double m_max_vol, m_max_emg;
    bool m_emgPresent;
    unsigned char	m_i_max_x;
    long  			m_max_x,m_max_y,m_max_y_gr2;
     int 		m_x1,m_y1,m_x2,m_y2;
    int   			m_num_xy;
     int   	m_x[25];
     int   	m_y[25];
    unsigned short 	m_uw3; // dimensioni in byte della riga n-esima dell'area grafico
     char 	m_str_gr[dim_string_rel2];	// allocazione dinamica ma all'inizio dello stringone che sarà usato per i label dei grafici
     char 	m_str_tr[dim_string_rel2];			// trasposizione stringa per ottenere matrice di punti per stampa grafica
    QVector<unsigned short int> m_flow_store;
    QVector<unsigned short int> m_vol_store;
    QVector<unsigned short int> m_emg_store;
    int m_init_time_to_print;
    short m_num_byte_x_gra;
    short m_pos_gra_flw,m_pos_gra_emg, m_pos_gra_vol;
    float m_num_dots_gra_vol, m_num_dots_gra_flw, m_num_dots_gra_emg;
    short m_num_byte_x_gra_emg, m_num_byte_x_gra_vol, m_num_byte_x_gra_flw;
    unsigned short m_cursore;

//    void readChanData();
//    void readTestData();
//    int findMDC_REVIEW();
//    float floatMax(float __a, float __b);
    void smooting_PRINT_flow();
    void pri_rep_review();
    //report
    void Pri_Rep();
    void Intest();
    void Report_data();
    void Report_flw();
    void Report_emg();
    void Report_Real_Time(short __num_sample);
    void Report_siroky();
    void Report_result();
    void Pri_Rep_Gra(int __num_riga);
    void Pri_Rep_Gra_Ini_Grid(int __n_riga);
    void Pri_Rep_Gra_EMG(byte __num_riga);
    void Pri_Rep_Gra_Landscape(short __num_cample);
    void Pri_Rep_asse_dx();
    void Pri_Rep_Label();
    void Pri_Rep_Gra_Siroky (byte __num_riga, byte __grap);
    void Pri_Rep_Gra_Ini_Grid_Sir( byte __num_rig, byte __curve );
    void Pri_Rep_Lin(char *__str_des, int __rep_dat, byte __num_dec, char *__str_udm, byte __flag_lf);
    void Plot_StdCurve_Siroky( byte __num_rig, byte __curve );
    void Plot_Point_Siroky( byte __num_rig, byte __curve, unsigned short __flu);
    void Plot_Quadro_Siroky(byte __ubstrt, byte __ubend, unsigned short __new_pos, int __vol_pt, unsigned char __uw4);
    byte Int_Pun(int __i4);
    void Gra_Line();
    void Str_Trasposta(byte __type, unsigned short __sx_byte, unsigned short __dx_byte); // crea la trasposta della stringa str_gr, ottenendo una matrice scritta per righe
    short adatta_buffer_dati(int __num_sample, long __fs_flw);
    void Calc_Max_RealReport_rel2(short __num_sample);
    void Calc_Max();
    void Calc_Max_EMG();
    long Calc_Max_Flw();
    void print_char_left_label(short __value, int __pos_in_string, short int __num_char, bool __pri_decim, short int __pre_char);
    void print_udm_label(int __ch_type, short __pos_in_string, short int __pre_char);
    bool check_stamp_label();

};

#endif // PRINTERMANAGER_H
