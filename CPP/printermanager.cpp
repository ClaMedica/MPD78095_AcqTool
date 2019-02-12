
#include "printermanager.h"
#include "udpmsgs.h"

const char *str_label_time[] = {
//    "   0          6          12          18          24         30s",
//    "   0          9          18          27          36         45s",
//    "   0         12          24          36          48         60s",
    "   0         0:15       0:30        0:45        1:00       1:15",
    "   0         0:18       0:36        0:54        1:12       1:30",
    "   0         0:21       0:42        1:03        1:24       1:45",
    "   0         0:24       0:48        1:12        1:36       2:00",
    "   0         0:27       0:54        1:21        1:48       2:15",
    "   0         0:30       1:00        1:30        2:00       2:30",
    "   0         0:36       1:12        1:48        2:24       3:00",
    "   0         0:42       1:24        2:06        2:48       3:30",
    "   0         0:48       1:36        2:24        3:12       4:00",
    "   0         0:54       1:48        2:42        3:36       4:30",
    "   0         1:00       2:00        3:00        4:00       5:00",
    "   0         1:12       1:24        3:36        4:48       6:00",
    "   0         1:24       2:48        4:12        5:36       7:00",
    "   0         1:36       3:12        4:48        6:24       8:00",
    "   0         1:48       3:36        5:24        7:12       9:00",
    "   0         2:00       4:00        6:00        8:00      10:00",
    "   0         2:12       4:24        6:36        8:48      11:00",
    "   0         2:24       4:48        7:12        9:36      12:00",
    "   0         2:36       5:12        7:48       10:24      13:00",
    "   0         2:48       5:36        8:24       11:12      14:00",
    "   0         3:00       6:00        9:00       12:00      15:00",/* 15*60=900*/
    "   0         3:12       6:24        9:36       12:48      16:00",
    "   0         3:24       6:48       10:12       13:36      17:00",
    "   0         3:36       7:12       10:48       14:24      18:00",
    "   0         3:48       7:36       11:24       15:12      19:00",
    "   0         4:00       8:00       12:00       16:00      20:00",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
};

// numeri ruotati di 90? in senso orario
const unsigned char  print8x8Set[] = {
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

const unsigned char  print7x13Set[]  = {
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

const unsigned char  print8x10SetSD[]  = {
        0x00, 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E,	/* " 0" */
        0x00, 0x02, 0x06, 0x0A, 0x02, 0xF2, 0x02, 0x02, 0x02, 0x07,	/* "-1" */
        0x00, 0x06, 0x09, 0x01, 0x01, 0x72, 0x04, 0x08, 0x08, 0x0F,	/* "-2" */
        0x00, 0x06, 0x09, 0x01, 0x01, 0x77, 0x01, 0x01, 0x09, 0x06,	/* "-3" */
};

const unsigned char  SD[2][4][20] = {
        {{50, 63, 74, 84, 95,105,114,123,132,140,147,154,160,166,170,174,176,178,178,179},
         {34, 44, 54, 62, 70, 78, 84, 91, 98,104,110,116,121,126,130,133,136,139,141,141},
         {10, 19, 26, 34, 40, 46, 52, 58, 64, 70, 75, 79, 84, 88, 93, 98,102,104,106,106},
         { 0,  0, 10, 16, 21, 26, 30, 34, 37, 40, 43, 47, 50, 53, 55, 58, 60, 62, 64, 66}},
        {{ 0, 96,110,122,134,145,156,166,176,186,195,204,211,217,223,228,231,233,234,234},
         { 0, 80, 91,101,110,119,127,135,143,150,156,162,166,171,174,176,177,178,179,179},
         { 0, 59, 67, 74, 80, 86, 91, 96,100,104,108,111,114,116,118,119,120,121,122,122},
         { 0, 41, 48, 51, 54, 55, 57, 58, 59, 60, 62, 62, 63, 64, 64, 64, 64, 65, 65, 66}}
    };


printermanager::printermanager(QString __namefile, QObject *parent) : QObject(parent)
{
    m_namefile    = __namefile;
    m_namefilePrn = __namefile.remove("_copy").remove(".pic").append(".prn");
    qDebug() << m_namefilePrn;

    m_dfm = new DatafileManager();
    m_dfm->SetFileName(m_namefile);
    m_dfm->SetFileType(7);

    m_printMode = PORTRAIT_MODE;
    m_printModeUser = PORTRAIT_MODE;

    m_realDots = 0;

    m_modal_e = 0;
    m_numTest = -1;
    m_test_type = false;
    m_emgPresent = false;
    m_i_max_x = -1;
    m_max_y = -1;
    m_uw3 = 1;
    m_cursore = 0;

    m_chVol = -1;
    m_chFlw = -1;
    m_chEmg = -1;

    m_printFirstHeader = "";
    m_printSecondHeader = "";

    m_port = new printerserialport(m_namefilePrn, this);

    m_resultBm_w = 824; // 103 bytes * 8 bit
    m_resultBm_h = 300;

    m_bitmapSiroky.resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapLiverpool.resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapSiroky.fill(0);
    m_bitmapLiverpool.fill(0);

//    m_flow_store = NULL;
//    m_vol_store = NULL;
//    m_emg_store = NULL;
    buffer_emg = NULL;
    buffer_flw = NULL;
    buffer_vol = NULL;
}

printermanager::~printermanager()
{
    if (m_port != NULL)
        m_port->closeSerialPort();
}

void printermanager::set_gra_Header_str_gr(int __sz, int __n4, int __dots)
{
    m_str_gr[0] = ESC;      // 0x1B;	// ESC
    m_str_gr[1] = '*';      // 0x2A;	// *
    m_str_gr[2] = __sz % 256         ;	// n1
    m_str_gr[3] = (__sz >>  8) & 0xff;	// n2
    m_str_gr[4] = (__sz >> 16) & 0xff;	// n3
    m_str_gr[5] = __n4;     // n4
    m_str_gr[6] = 0x00;     // n5	scrive a n5 byte dal bordo
    m_str_gr[7] = __dots;   // n6	larghezza dots in bytes
}

/**
 * @brief printermanager::printDigits
 * @param __val     : valore POSITIVO da convertire
 * @param __ndigits
 * @param __pos
 */
void printermanager::printDigits(int __val, int __ndigits, int __pos)
{
    char  * dst;
    char  * src;
    int   * digits = new int(__ndigits);

    for(int i = 0; i < __ndigits; i++ )
        digits[i] = -1;

    int i = __ndigits;
    do {
        digits[--i] = __val % 10;
        __val /= 10;
    } while(__val > 0);

    dst = (char *) & m_str_gr[ __pos ];   // clear
    for(int i = 0; i < 24*__ndigits; i++)
        *dst++ = 0;

    for(int n = 0; n < __ndigits; n++) {
        dst = (char *) & m_str_gr[ __pos + n*24];
        if(digits[n] >= 0) {
            src = (char *) & print7x13Set[ digits[n]*13 ];
            for(int i = 0; i < 13; i++ )
                *dst++ = *src++;
        }
    }

    delete digits;
}

void printermanager::closePrinter()
{
    m_port->closeSerialPort();
    if (m_port != NULL)
        delete m_port;
    m_port = NULL;
}

/**
Stampa automatica alla fine dell'esame: al momento della pressione dello stop, i dati sono analizzati, salvati e poi mandati in stampa.
*/
void printermanager::print()
{
    //dati paziente e esame
    m_dfm->Open();
    m_dfm->GetParameters();
    //paziente
    QString dataPaz = m_dfm->GetPatient();

    m_name = dataPaz.section(";", 0, 0);
    m_surname = dataPaz.section(";", 1, 1);
    m_dateofbirth = dataPaz.section(";", 2, 2);
    m_sex = *(dataPaz.section(";", 12, 12).toLatin1().data());
    m_ID = dataPaz.section(";", 3, 3);
    //esame
    m_numchan = m_dfm->GetChanNum();
    m_numTest = m_dfm->GetTestNum();

    QDate dateExam = QDate(1899, 12, 30).addDays(m_dfm->GetDataEsame());
    m_dateofexam = dateExam.toString("dd/MM/yyyy");

    QTime time = QTime(0,0,0).addSecs(m_dfm->GetStartTime());
    m_timeofstart = time.toString();	//ora inizio esame

    m_durata = m_dfm->GetDuration() / 1000; //in sec

     // discrimina se esame veloce oppure completo di dati paziente (servira nel report di stampa)
    if (m_surname == "Anonymous")
        m_test_type = true;

    if(m_printModeUser)
        m_printMode = LANDSCAPE_MODE;
    else
        m_printMode = PORTRAIT_MODE;

    int minNas = 5000;
    for (int i = 0; i < m_dfm->GetChanNum(); i++) {
        int nas = m_dfm->GetNAS(i);
        if (nas < minNas)
            minNas = nas;
    }
    m_num_sam = minNas * m_durata;
    m_realDots = m_num_sam;
    qDebug("m_num_sam: %d",m_num_sam);

    m_dfm->Close();

    pri_rep_review();	// qui va subito in stampa

#ifdef PICOFLOW
//    udpConn.sendPrn("print:" + m_namefilePrn.toLatin1());   // diretto
    udpConn.sendSup("Print:" + m_namefilePrn.toLatin1());   // gateway
#endif
}

void printermanager::pri_rep_review()
{
    qDebug("pri_rep_review()");
    int n_chEmg = 0, n_chFlw = 0, n_chVol = 0;

    m_dfm->Open();
    m_dfm->GetParameters();
    int numCh = m_numchan;

    m_chEmg = m_chFlw = m_chVol = -1;

    if(buffer_emg != NULL)
        delete buffer_emg;
    if(buffer_flw != NULL)
        delete buffer_flw;
    if(buffer_vol != NULL)
        delete buffer_vol;
    buffer_emg = buffer_flw = buffer_vol = NULL;

    // caricamento dati dei canali
    for (int c = 0; c < numCh; c++) {
        QString chName = m_dfm->GetChanName(c);
        if(chName.startsWith("EMG")) {
            m_chEmg = c;
            n_chEmg = m_dfm->GetSamplesNumber(c);
            buffer_emg = new double[n_chEmg];
            m_dfm->GetChVal(c, 0, n_chEmg, buffer_emg, 0);
            qDebug("emg: %d", n_chEmg);
        }
        if (chName.startsWith("Q")) {
            m_chFlw = c;
            n_chFlw = m_dfm->GetSamplesNumber(c);
            buffer_flw = new double[n_chFlw];
            m_dfm->GetChVal(c, 0, n_chFlw, buffer_flw, 0);
            qDebug("Flw: %d", n_chFlw);
        }
        if (chName.startsWith("VV") || chName ==  "VLMv" ) {
            m_chVol = c;
            n_chVol = m_dfm->GetSamplesNumber(c);
            buffer_vol = new double[n_chVol];
            m_dfm->GetChVal(c, 0, n_chVol, buffer_vol, 0);
            qDebug("vol: %d", n_chVol);
        }
    }

    // allineamento lunghezze tra canali
    int real_lenght = (n_chVol > n_chFlw) ? n_chFlw : n_chVol;
    if(real_lenght > 20*60*10)
        real_lenght = 20*60*10;
    qDebug("real_lenght: %d", real_lenght);

    // emg: allineamento lunghezze e decimazione valori
    if(m_chEmg >= 0) {
        int lim = real_lenght * 10;

        if(lim > n_chEmg) {                     // allineamento lunghezze
            double * p = new double[lim];
            for(int i = 0; i < n_chEmg; i++)
                p[i] = buffer_emg[i];
            for(int i = n_chEmg; i < lim; i++)  // azzeramento valori mancanti
                p[i] = 0.0;
            delete buffer_emg;
            buffer_emg = p;
        }

        double * p = new double[real_lenght];   // decimazione
        for(int i = 0; i < real_lenght; i++) {
            double v = 0.0;
            for(int m = i*10; m < (i+1)*10; m++)
                v += fabs(buffer_emg[m]);
            p[i] = v / 10.0;
        }
        delete buffer_emg;
        buffer_emg = p;

        n_chEmg = real_lenght;
        m_emgPresent = true;
    }

    double  xscale;
    if(m_printMode == PORTRAIT_MODE) {
        xscale = ((double) NUMOF_X_PRINT_DOTS) / real_lenght;
        if(xscale > 1.0)
            xscale = 1.0;
    }
    else
        xscale = 0.8;   // 1mm = 1sec, 1mm = 8 linee, 10 campioni --> 8 linee
    qDebug("real_lenght:%d xscale:%.6f", real_lenght, xscale);

    // compressione asse X con fattore xscale per i 3 buffer
    if(xscale != 1.0) {
        double *bufp[3];
        bufp[0] = buffer_emg;
        bufp[1] = buffer_flw;
        bufp[2] = buffer_vol;
        for(int ibuf = 0; ibuf < 3; ibuf++) {
            double * bufX = bufp[ibuf];
            if(bufX != NULL) {
                int ilast = -1; // indice destinazione su cui accumula la media, riferito a nsample
                int icnt  = 0;
                double isum = 0.0;
                for(int i = 0; i < real_lenght; i++) {
                    double v = bufX[i];
                    int ivirt = (int)(i*xscale);        // virtual destination index
                    if(ilast < 0)
                        ilast = ivirt;
                    if(ivirt != ilast) {                // nuova posizione: scaricare la media in [ilast]
                        bufX[ilast] = isum / icnt;      // scarica valore precedente
                        isum = 0.0;                     // nuova posizione: media a 0.0
                        icnt = 0;
                        ilast = ivirt;
                    }
                    // ivirt == ilast
                    isum += v;          // aggiorna la media
                    icnt++;
                }
            }
        }
    }

    m_num_sam = m_realDots = real_lenght * xscale;

    smooting_PRINT_flow(); // qui riempie il buffer di stampa con il set mediato dei campioni di flusso
    qDebug("dopo smooting_PRINT_flow()");
    
    int max_volume = 0;
    for (int i = 0; i < m_realDots; i++)	// cerco il massimo del buffer volume
        if (buffer_vol[i] > max_volume)
            max_volume = buffer_vol[i];

    for (int i = (m_realDots - 1); i < m_realDots; i++) {       // usare realDots
        buffer_vol[i] = max_volume;
        buffer_flw[i] = 0;
    }

    if (m_emgPresent) {
        for (int i = (m_realDots - 1); i < m_realDots; i++) {   // usare realDots
            buffer_emg[i] = buffer_emg[m_realDots - 2];
        }
    }

    m_dfm->Close();

    Pri_Rep(xscale);
//    Report_BitMap();
}

/**
il buffer dei campioni reali viene dato in pasto ad un algoritmo di media mobile
Alla fine ogni valore del buffer e il valore mediano di una finestra di 5 campioni originali a cavallo di questo, cioe i 2 precedenti, il campione stesso e i 2 successivi
In questo modo il primo significativo e il 3, cosi al primo e assegnato un valore pari ad 1/4 del valore del terzo, e al secondo un valore pari a 1/2 del terzo
Al penultimo un valore pari a 1/2 del terzultimo, all'ultimo un valore pari ad 1/4 del terzultimo
*/
void printermanager::smooting_PRINT_flow()
{
    double media = 0.0;

    // ripeto tutto per il buffer di stampa
    for (int i = 0; i < (m_realDots - WIN); i++) {
        double somma = 0.0;
        for (int j = i; j < (i + WIN); j++)
            somma += buffer_flw[j];
        media = somma / WIN;
        buffer_flw[i + WIN/2] = media;
        if (i == 0) {
            // i due campioni precedenti al primo calcolato, li metto a mano, smorzandoli della meta
            buffer_flw[(WIN/2) - 2] = media / 4; // primo o secondo campione della finestra
            buffer_flw[(WIN/2) - 1] = media / 2; // secondo o terzo campione della finestrau
        }
    }

    for (int j = (m_realDots - WIN/2); j < m_realDots; j++) {
        media = media / 2.0;
        if (media < 0)
            media = 0;
        buffer_flw[j] = media;
    }
}

//funzioni che erano nel file Report.cpp
/**
la stampa e prevista comunque sempre dopo il review, si avvale dei dati di review, cosi usiamo
gli stessi comandi sia che ci si trovi a fine esame, sia che sia un review di esamei in MMC.
Allora al posto della struttura Patient_Data, ci metiamo DatiPaziente.
questa funzione gestisce tutta la stampa del report, sia in modalita portrait che landscape
*/
#include "QApplication"
void printermanager::Pri_Rep(double xscale)
{
//    //test velocità stampa con n letto da file di testo
//    QString name = QApplication::applicationDirPath()+"/numdots";
//    QFile readSpeed(name);
//    bool read = readSpeed.open(QIODevice::ReadOnly);
//    if (read)
//    {
//        QTextStream in(&readSpeed);
//        QString line = in.readLine();
//        int n = line.toInt();
//        m_port->Pri_Speed(n);
//        readSpeed.close();
//    }
//    //test max velocità stampa con n letto da file di testo
//    name = QApplication::applicationDirPath()+"/maxspeed";
//    QFile readMaxSpeed(name);
//    read = readMaxSpeed.open(QIODevice::ReadOnly);
//    if (read)
//    {
//        QTextStream inM(&readMaxSpeed);
//        QString line = inM.readLine();
//        int num = line.toInt();
//        uint8_t  a = (num /256);
//        uint8_t  b = num & 0xff;
//        m_port->Pri_Max_Speed(a,b);
//        readMaxSpeed.close();
//    }


    //      Intestazione
    qDebug() << "Pri_Rep file:" << m_namefile;
    Intest();

    //     Identificativi Esame
    Report_data();		// scrive i dati del paziente e lo spazio per le note manuali
    m_port->Pri_Str(LINE2, 2, false);

    // Grafico FLW + VOL ed eventualmente EMG
    if(m_printMode == PORTRAIT_MODE)			// grafico trasversale con numero di punti fisso
    {
        m_max_vol = buffer_vol[0];
        m_max_emg = 0;

        for (int i = 0; i < m_num_sam; i++)	{	// cerco il massimo del buffer volume
            if (buffer_vol[i] > m_max_vol)
                m_max_vol = buffer_vol[i];
            if (m_emgPresent) {
                double v = buffer_emg[i];
                if (v > m_max_emg)
                    m_max_emg = v;
            }
        }
        qDebug() << "m_max_emg:"<<m_max_emg;

#if 0
        if (m_printModeUser)	// e settata la stampa in landscape ma l'esame e troppo lungo
        {
            m_port->Pri_justif(0);
            m_port->Pri_mode(0x80);	// sottolineato e doppia larghezza
            QString strToWrite = tr("! Examination longer than 5 minutes");
            char str[60];
            sprintf( str, "%s\n", strToWrite.toLatin1().data());
            m_port->Pri_Str(str, strlen(str), true );
            m_port->Pri_justif(2);	// bandiera a sinistra
        }
#endif

        Report_flw(xscale);	// finalmente stampiamo i grafici di volume e flusso
        if(m_emgPresent)
            Report_emg();						// EMG
    }
    else {              // print_mode = LANDSCAPE_MODE - grafico longitudinale, con lunghezza legata alla lunghezza dell'esame
        m_max_y = Calc_Max_Flw();			// fondo scala del flusso
        qDebug("dopo Calc_Max_Flw()");
//        short samples_to_print = adatta_buffer_dati(/*m_realDots*/ m_num_sam, m_max_y);
//        qDebug("dopo adatta_buffer_dati()");
        Calc_Max_RealReport_rel2(m_num_sam);
        qDebug("dopo Calc_Max_RealReport_rel2()");
        Report_Real_Time(m_num_sam, xscale);		// finalmente stampa
        qDebug("dopo Report_Real_Time()");
    }

    // Grafico Liverpool
    if(m_printLiverpool)
        Report_BitMap();
    // Grafico Siroky, stampato solo se paziente maschio oppure generico (cioe dove non indicato il sesso)
     if(m_printSiroky && m_sex != 'F')
         Report_BitMap(true);

    // 	Risultati dell'esame ricavati dall'analisi semplificata, implementata nel firmware
    Report_result();		// scrive in elenco i dati calcolati dall'analisi dell'esame
    //m_port->status();
    m_port->Pri_Str(LINE2, 2, false); // LINE"\x3",0);

    //	Scrive i dati riguardanti versione firmware e date/ora ultima calibrazione
       //m_port->Pri_justif(F_center);
       //Report_dati_macchina(numCurve);	// scrive data e ora della stampa e la versione attuale del FW

    // fa avanzare la carta per consentire lo strappo
    int npix = (int) (25 / 0.125);
    m_port->Pri_forward(npix);
    m_port->flushFile();

//    m_port->Pri_Reset();	// resetta RAM della stampante: equivale ad un reset HW
    qDebug() << "Pri_Rep FINE file:" << m_namefile;
}

/**
E' stampata l'intestazione del report, con intestazione clinica, logo and so on
*/
void printermanager::Intest()
{
    m_port->Pri_Str(LINE2, 2, false); // LINE"\x1",0);  			// scrive una riga vuota
    m_port->Pri_justif(F_center);				// scrittura al centro
    m_port->Pri_Font(1);						// font 12x20
    m_port->Pri_mode(0x30);						// modo doppia dimensione non sottolienata

    char str[60];
    QString ditta_pers = m_printFirstHeader;
    sprintf( str, "%s\n", ditta_pers.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), false);			// a capo con il "\n"

    m_port->Pri_mode(0x14);						// modo altezza doppia larghezza quadrupla
    QString logo = m_printSecondHeader;
    sprintf( str, "%s", logo.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), true);			// a capo con il "\n"

    m_port->Pri_Font(1); 						// font 12x20
    m_port->Pri_mode(0x90);						// modo altezza doppia sottolineata
    QString msg_title = tr("Urodynamic Equipment");
    sprintf( str, "%s\n", msg_title.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), false);			// a capo con il "\n"

    m_port->Pri_mode(0);
}

/**
Sono stampati i dati del report paziente, nome, cognome, data di nascita, sesso, data esame, ....
*/
void printermanager::Report_data()
{
    char str[200];

    m_port->Pri_mode(0x00); 					// modo default
    m_port->Pri_justif(F_left);					// tutto a sinistra
    m_port->Pri_Str(LINE2, 2, false); 	// scrive una riga vuota

    m_port->Pri_Font(1);							// font 12x20

    const char * puntini = ". . . . . . . . . . . . . . . . . . . . .\n";
    const char * space_bar = "                    ";

    QString strToWrite;
    strToWrite = tr("Test Number ....:");
    sprintf( str, " %s %s\n", strToWrite.toLatin1().data(), QString::number(m_numTest).toLatin1().data());

    m_port->Pri_Str(str, strlen(str), false);

    strToWrite = tr("Test Date ......:");
    sprintf(str," %s %s\n", strToWrite.toLatin1().data(), m_dateofexam.toLatin1().data());

    m_port->Pri_Str(str, strlen(str), false);

    // cognome
    strToWrite = tr("Surname ........:");
    if(m_test_type )
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini);
    else
        sprintf( str, " %s %s\n", strToWrite.toLatin1().data(), m_surname.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), false);

    // nome
    strToWrite = tr("Name ...........:");
    if(m_test_type)
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini);
    else
        sprintf( str, " %s %s\n", strToWrite.toLatin1().data(), m_name.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), false);

    // data di nascita stampata nei due formati: uno per ita e spa e uno per eng
    strToWrite = tr("Birth Date .....:");
    if(m_test_type)
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini);
    else
        sprintf(str," %s %s\n",strToWrite.toLatin1().data(), m_dateofbirth.toLatin1().data());

    m_port->Pri_Str(str, strlen(str), false);

    // sesso
    strToWrite = tr("Gender .........:");
    if(m_test_type){
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
        m_port->Pri_Str(str, strlen(str), false);
    }
    else {
        sprintf( str, " %s %c", strToWrite.toLatin1().data(), m_sex );
        m_port->Pri_Str(str, strlen(str), true);
    }

    // operatore
    strToWrite = tr("Investigator ...:");
    sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
    m_port->Pri_Str(str, strlen(str), false);

    // commenti
    strToWrite = tr("Comments .......:");
    sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
    m_port->Pri_Str(str, strlen(str), false);
    for (int i = 0; i < 3; i++)	{	// tre righe vuote per eventuali commenti
        m_port->Pri_Str((char *)space_bar, 19, false );
        strcpy(str, puntini);
        m_port->Pri_Str(str, strlen(str), false);
    }

    m_port->Pri_Str(LINE2, 2, false); //riga vuota

    // scritta relativa al tipo di modalita
    if(m_modal_e == 2) {
        strToWrite = tr("MANUAL   MODALITY");
        sprintf( str, " %s\n", strToWrite.toLatin1().data());
    }
    else
        if(m_modal_e == 0) {
            strToWrite = tr("AUTOMATIC  MODALITY ");
            sprintf( str, " %s\n", strToWrite.toLatin1().data() );
        }
    m_port->Pri_Str(str, strlen(str), false);

    m_port->Pri_Intensity(0x80);		// intensita di default
}

/**
stampa del grafico di flusso/volume nella versione a grafici in portrait mode
*/
void printermanager::Report_flw(double xscale)
{
    Calc_Max(xscale);	// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) cosi da individuare il giusto fondoscala

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);

//    QString strToWrite = tr("- Flowmetry  -");
//    QString strToWrite = tr("Flowmetry   ");
//    char str[90];
//    sprintf( str, "   Q ( ml/s )             %s              Vol ( ml )", strToWrite.toLatin1().data());

    QString strToWrite = tr("  Q ( ml/s )              Flowmetry               Vol ( ml )");
    char str[90];
    sprintf( str,strToWrite.toLatin1().data());

    m_port->Pri_Str(str, strlen(str), true);

    // label solo su righe pari
    for(int i = 0; i < 10; i++ ) {                       /* scompone la griglia in 10 righe*/
        Pri_Rep_Gra(i);
    }

    m_port->Pri_mode(0x00);
    m_port->Pri_Font(1);
    m_port->Pri_Str((char *)str_label_time[m_i_max_x], strlen(str_label_time[m_i_max_x]), true);

    m_port->Pri_Str(LINE2, 2, false); // modifica per risparm carta e tempo: da riattivare
}

/**
Calcola "i_max_x" , "max_x" e "max_y"
*/
void printermanager::Calc_Max(double xscale)
{
    static int v_max_x[] = {
                                // 30, 45, 60,
                                75, 90, 105, 120, 135, 150, 180,
                                210, 240, 270, 300, 360, 420, 480, 540, 600, 660,
                                720, 780, 840, 900, 960, 1020, 1080, 1140, 1200,
                                -1
                            };
    int max_x = (m_num_sam / xscale) / 10;
    m_i_max_x = 0;
    qDebug("m_num_sam:%d m_max_x:%d",m_num_sam,max_x);

    if(max_x < 0)
        max_x = 1;
    if(max_x > 1200)
        max_x = 1200;
    for(int i = 0; v_max_x[i] > 0; i++)
        if(max_x <= v_max_x[i]) {
            max_x = v_max_x[i];
            m_i_max_x = i;
            break;
        }
    qDebug("m_max_x:%d m_i_max_x:%d",max_x,m_i_max_x);
    qDebug("label time >>%s<<",str_label_time[m_i_max_x]);

    int decina = m_flu_max / 10;
    m_max_y = (decina + 1) * 10;			// trovo la decina minima superiore al valore max

    if (m_max_y <=  10) m_max_y =  10; else
    if (m_max_y <=  15) m_max_y =  15; else
    if (m_max_y <=  20) m_max_y =  20; else
    if (m_max_y <=  25) m_max_y =  25; else
    if (m_max_y <=  30) m_max_y =  30; else
    if (m_max_y <=  40) m_max_y =  40; else
    if (m_max_y <=  50) m_max_y =  50; else
    if (m_max_y <=  65) m_max_y =  65; else
    if (m_max_y <=  80) m_max_y =  80; else
    if (m_max_y <= 100) m_max_y = 100; else
    if (m_max_y <= 120) m_max_y = 120; else
    m_max_y = 150;

    m_max_y_gr2 = ((long) ((m_max_vol + 10) / 100) + 1) * 100;	// 05 dicembre
}

/**
GRAFICI IN PORTRAIT MODE - modo tradizionale
funzione chiamata ciclicamente per stampare le 10 righe in cui e suddiviso il grafico del flusso/volume
a seconda del parametro num_rig si stampano o meno i label degli assi di flusso e volume
*/
void printermanager::Pri_Rep_Gra(int __num_riga)
{
    bool label = false;
    unsigned short uw8;
    short int max_y_flw, max_x_vol;

    for(int i = 0; i < (LCMD + a_flw); i++ )
        m_str_gr[i] = 0;

    // setta la stampa grafica
    int sz = 96 + 2256 + 96;    // 	num_byte = 96 + 2256 + 96 = 2448 = ( 65536 * n3 ) + ( 256 * n2 ) + n1
    int dots = 102;             // larghezza 4+94+4=102 byte
    set_gra_Header_str_gr(sz, 0x02, dots);    // n4	doppia altezza

    // se label e' true devo scrivere i valori sugli assi di flw e vol
    // la prima riga del grafico si compone di 24 righe di 94byte ciascuna alle quali antepongo un carattere vuoto, le 24 righe di 3 byte del dato di flw
    // i 3 byte aggiuntivi sono per le tre cifre di dato
    label = ((__num_riga == 0) || ((__num_riga & 1) == 0));  // scrive i label solo nelle righe 0, 2, 4, 6, 8
    if(label) {
#ifdef __ZERO_ASSE_FLUSSO__
        if(__num_riga != 9)   // se e' l'ultima riga evito scrittura
#endif
        {
            max_y_flw = (int) (m_max_y - (__num_riga / 2) * (m_max_y / 5));
            printDigits(max_y_flw, 3, LCMD + CLS - 3);
        }
    }

    // costruzione assi grafico: quattro sono i caratteri anteposti senza byte dei comandi
    Pri_Rep_Gra_Ini_Grid(__num_riga); /*crea griglia e inizializa m_m_str_gr*/

    // inserimento pixel curve
    m_str_gr[ pos_gra + 2255 ] = (char)0xFF;

#ifdef __DATI_GRAF_FLUSSO__
    /* ********************     Grafico    ************************* */
    if (m_tem_svu ) {

        int x_pt	=	0;
        int flow_pt	= 240;
        int vol_pt	= 240;
        int uw5		= m_num_sam-1;		// correzione per evitare l'andata a zero // 23-03
        int i4		= __num_riga*24;

        int graf_g_x	=752/*(10*m_max_x)*/;			//asse dei tempi in funzione della durata dell'esame
        int graf_g_fl	=(m_max_y);
        int graf_g_vl	=(m_max_y_gr2);

//        QVector<double> tmp;
//        for (int gg=0; gg<NUMOF_X_PRINT_DOTS;gg++)
//            tmp.append(buffer_flw[gg]);

        for (int i = 1; i < uw5; i++ ) {	     /*trovo estremi della retta passante per il campione uw6 uw6+1*/
            int x_pt_prec = x_pt;                    /*94*8=752 punti x  i2=x1 della retta*/
            m_x1 = x_pt;

            x_pt = (int) ((752 * i) / graf_g_x);             /*x_pt=x2 retta */
            m_y1 = flow_pt;

            flow_pt = 239 - (int) ((239 * buffer_flw[i]) / graf_g_fl); /*y2*/

            if (flow_pt > 239)   // flow_pt = _y1;
                while (flow_pt <= 239) {
                    uw8 = i - 1;
                    flow_pt	= 239 - (int) ((239 * buffer_flw[uw8]) / (graf_g_fl));//239; modifica 31-03
                }
            else
                if (flow_pt < 0)
                    flow_pt	= 0;

            /*ogni riga composta da 1 linee di stampa m_str_gr*/
            m_x2 = x_pt;
            m_y2 = flow_pt;
            if( Int_Pun(i4) ) { //questa funzione utilizza m_x2 e m_y2
                if (m_num_xy > 25)
                    m_num_xy = 25;
                for(int j = 0; j < m_num_xy; j++) {
                    m_y[j] -= i4;
                    int i33 = 24 * (m_x[j] / 8) + m_y[j];
//                    int i6 = i33 / 24;
//                    int i7 = i33 % 24;
                    m_str_gr[ pos_gra + i33/*24 * i6 + i7*/] |= (0x80 >> (m_x[j] % 8));
                }
            }
            m_y1 = vol_pt;

            vol_pt = 239 - (int) ((239 * buffer_vol[i]) / (graf_g_vl));
            if( vol_pt > 239 )
                vol_pt = m_y1; // se campione sfarlocco, scrive quello precedente

            m_x1 = x_pt_prec;
            m_x2 = x_pt;
            m_y2 = vol_pt;
            if(Int_Pun(i4)) {
                if (m_num_xy > 25)
                    m_num_xy = 25;
                for(int i22 = 0; i22 < m_num_xy; i22++) {
                    m_y[i22] -= i4;
                    int i33 = 24 * (m_x[i22] / 8) + m_y[i22];
                    unsigned char ub3 = 0x80 >> (unsigned char)(m_x[i22] % 8);
                    int i6 = i33 / 24;
                    int i7 = i33 % 24;
                    m_str_gr[ pos_gra + 24*i6 + i7] |= (char)ub3;
                }
            }
        }
    }


    // inserimento label volume
    if( label ) {
        max_x_vol = (int) (m_max_y_gr2 - (__num_riga / 2) * (m_max_y_gr2 / 5));
        // inserimento cifre volume 4 caratteri dalla posizone 'pos_lab_vol'
        printDigits(max_x_vol, 4, pos_lab_vol);
    }

#endif

    // trasposizione
    Str_Trasposta(0, CLS, CLD); // fa la trasposta di m_str_gr che e' costruita per colonne mentre noi si stampa per righe

    // finalmente stampa
//    m_port->Pri_Str((char *)m_str_gr, (LCMD + CLS*24 + m_uw3 + CLD*24), false);

    // bitmap completa spezzata in
    // una riga di bit per volta
    const int ByteXline = 102;      // 0x66
    m_str_gr[2] = ByteXline;        // n1 = n6:larghezza 4+94+4=102 byte
    m_str_gr[3] = 0;                // n2
    m_str_gr[4] = 0;                // n3
    char tbuf[LCMD + ByteXline];
    memcpy(tbuf, m_str_gr, LCMD);   // comando di stampa
    char  * p = m_str_gr + LCMD;    // ptr a bitmap
    for(int i = 0; i < sz; ) {
        memcpy(tbuf+LCMD, p, ByteXline);
        m_port->Pri_Str(tbuf, sizeof(tbuf), false);
        i += ByteXline;
        p += ByteXline;
    }
}

void printermanager::Pri_Rep_Gra_Ini_Grid(int __n_riga)
{
    m_uw3 = 24 * 94;	// dimensioni in byte della riga n-esima dell'area grafico

    // azzeriamo tutto lo stringone
    for(int i = 0; i < m_uw3; i++)
        m_str_gr[ pos_gra + i ] = 0;

    // ognuna delle 10 fascie e' costruita per 94 colonne da 24 punti, dove da str_gr[0] a str_gr[23] e' la prima colonna e cosi via
    for(int i = 0; i < 24; i++)	{
        m_str_gr[ pos_gra + i ] = (char)0x80;  /*     Bordo Verticale Sinistro   */
        m_str_gr[ pos_gra + m_uw3 - 24 + i ] = (char)0x01;   /*    Bordo Verticale Destro   */
    }
    /*    Bordo Superiore   */
    if(__n_riga == 0) {
        for(int i = 0; i < m_uw3; i += 24 )
            m_str_gr[ pos_gra + i ] = (char)0xFF;
    }
    /*     Bordo Inferiore   */
    if(__n_riga == 9) {
        for(int i = 23; i < m_uw3; i += 24 )
            m_str_gr[ pos_gra + i ] = (char)0xFF;
    }
    /*      Griglia Verticale   */
    for(int i = 1; i < 10; i++) {
        int i2 = (int) (((94 * 8) * i + 5) / 10);
        for(int i3 = 0; i3 < 24; i3 += 4) {
            int i4 = 24 * (i2 / 8) + i3;
            unsigned char ub3 = 0x80 >> (i2 & 7);// 10000000 >> byte(752%8)
            m_str_gr[ pos_gra + i4 ] |= (char)ub3;
        }
    }
    /*      Griglia Orizzontale   */
    if((__n_riga & 1) == 0) {
        for(int i = 0; i < m_uw3; i += 24 )
            m_str_gr[ pos_gra + i] |= (char)0x10;
    }
}  // fine Pri_Rep_Gra_Ini_Grid

unsigned char printermanager::Int_Pun(int __i4)
{
    /*
        Interpola i Punti ("x1","_y1") e ("x2","y2") rispetto alla Banda
      di Ordinate "i4" e "i4"+48.
      Ritorna :
         -   1  :  Se la Banda     e' Interessata dai Punti
         -   0  :  Se la Banda non e' Interessata dai Punti
   */
    /*  Entrambi i Punti Dentro   */
    if ((m_y1 >= __i4) && (m_y1 < (__i4+24)) && (m_y2 >= __i4) && (m_y2 < (__i4+24))) {
        Gra_Line();
        return(1);
    }
    /* Entrambi i Punti Fuori dalla Stessa Parte   */
    if (((m_y1 < __i4) && (m_y2 < __i4)) || ((m_y1 >= (__i4+24)) && (m_y2 >= (__i4+24))))
        return(0);

    if (m_y1 < __i4) {
        /* Primo Punto Sotto  */
        if (m_y2 < (__i4+24)) {
            /*  Secondo Punto Dentro */
            m_x1 = m_x1+((m_x2-m_x1)*(__i4-m_y1))/(m_y2-m_y1);
            m_y1 = __i4;
        }
        else {
            /*  Secondo Punto Sopra  */
            int i6 = (m_x2 - m_x1);
            int i7 = (m_y2 - m_y1);
            m_x1 = m_x1 + (i6 * (__i4 - m_y1)) / i7;
            m_x2 = m_x2 + (i6 * (__i4 + 23 - m_y2)) / i7;
            m_y1 = __i4;
            m_y2 = __i4 + 23;
        }
        Gra_Line();
        return(1);
    }
    if (m_y1 < (__i4 + 24)) {
        /* Primo Punto Dentro */
        if (m_y2 < __i4){
            /*   Secondo Punto Sotto  */
            m_x2 = m_x2 + (m_x1 - m_x2) * (__i4 - m_y2) / (m_y1 - m_y2);
            m_y2 = __i4;
        }
        else {
            /*  Secondo Punto Sopra  */
            m_x2 = m_x2 + (m_x1 - m_x2) * (__i4 + 23 - m_y2) / (m_y1 - m_y2);
            m_y2 = __i4+23;
        }
        Gra_Line();
        return(1);
    }
    /*   Primo Punto Sopra   */
    if (m_y2 < __i4) {
        /*   Secondo Punto Sotto   */
        int i6 = (m_x2 - m_x1);
        int i7 = (m_y2 - m_y1);
        m_x2 = m_x2 + (i6 * (__i4 - m_y2)) / i7;
        m_x1 = m_x1 + (i6 * (__i4 + 23 - m_y1)) / i7;
        m_y2 = __i4;
        m_y1 = __i4 + 23;
    }
    else {
        /*     Secondo Punto Dentro   */
        m_x1 = m_x1 + (m_x2 - m_x1) * (__i4 + 23 - m_y1) / (m_y2 - m_y1);
        m_y1 = __i4 + 23;
    }
    Gra_Line();
    return(1);
}

void printermanager::Gra_Line()
{
    /*
      Disegna il Segmento definito da ["x1","_y1"] - ["x2","y2"]
   */
    int xmin,xmax,ymin,ymax;
    if ((m_x2 == m_x1) && (m_y2 == m_y1)) {
        m_num_xy = 1;
        m_x[0] = m_x1;
        m_y[0] = m_y1;
        return;
    }
    m_num_xy = 0;
    unsigned char flag_incr = 0;
    if (m_y2 != m_y1) {
        if ((abs(m_x2 - m_x1) / abs(m_y2 - m_y1)) < 1)
            flag_incr = 1;
    }
    if (flag_incr == 0) {
        if (m_x2 > m_x1) {
            xmin = m_x1; ymin = m_y1;
            xmax = m_x2; ymax = m_y2;
        }
        else {
            xmin = m_x2; ymin = m_y2;
            xmax = m_x1; ymax = m_y1;
        }
        for (int i5 = xmin; i5 <= xmax; i5++) {
            int i6 = (i5 - xmin) * (ymax - ymin);
            i6 /= (xmax - xmin);
            int i7 = ymin + i6;
            m_x[m_num_xy] = i5;
            m_y[m_num_xy] = i7;
            m_num_xy++;
        }
    }
    else {
        if (m_y2 > m_y1) {
            ymin = m_y1; xmin = m_x1;
            ymax = m_y2; xmax = m_x2;
        }
        else {
            ymin = m_y2; xmin = m_x2;
            ymax = m_y1; xmax = m_x1;
        }
        for (int i5 = ymin; i5 <= ymax; i5++) {
            int i6 = (i5 - ymin) * (xmax - xmin);
            i6 /= (ymax - ymin);
            int i7 = xmin + i6;
            m_x[m_num_xy] = i7;
            m_y[m_num_xy] = i5;
            m_num_xy++;
        }
    }
}

// crea la trasposta della stringa str_gr, ottenendo una matrice scritta per righe
void printermanager::Str_Trasposta(unsigned char __type, unsigned short __sx_byte, unsigned short __dx_byte)
{
    int num_byte;

    if( __type == 0 )				// trasposizione della stringa del grafico di flusso o emg
        num_byte = __sx_byte + 94 + __dx_byte;	// numero di byte della riga orizzontale del grafico comprensivo dei label
    else                                        //  if( __type == 1 ) trasposizione della stringa dei grafici di siroky
        num_byte = __sx_byte + w_ave + w_SD + __sx_byte + w_max + w_SD;


    for(int j = 0; j < (num_byte * 24); j++ )
        m_str_tr[j] = 0;	// azzero tutta questa stringona

    for(int num_col = 0; num_col < 24; num_col++) {
        for(int num_rig = 0; num_rig < num_byte; num_rig++) {
            int src = LCMD + (24 * num_rig) + num_col;
            int dst = (num_byte * num_col) + num_rig;
            m_str_tr[dst] = m_str_gr[src];
        }
    }

    for(int j = 0; j < (num_byte * 24); j++)
        m_str_gr[ LCMD + j ] = m_str_tr[ j ];
}

/**
stampa del grafico di EMG nella versione a grafici in portrait mode
*/
void printermanager::Report_emg()
{
    Calc_Max_EMG();
    qDebug() << "m_max_emg:" << m_max_emg << "m_max_y:" << m_max_y;

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);
    char str[60];
    QString emg_title = tr(" EMG Diagram ");
    sprintf(str,"   EMG ( uV )              %s\n",emg_title.toLatin1().data());
    m_port->Pri_Str(str, strlen(str), false);

    for(int i = 0; i < 10; i++)          /*scompone la griglia in 10 righe*/
        Pri_Rep_Gra_EMG( i );

    m_port->Pri_mode(0);
    m_port->Pri_Font(1);
    m_port->Pri_Str((char *) str_label_time[m_i_max_x], strlen(str_label_time[m_i_max_x]), true);
    m_port->Pri_Str(LINE2, 2, false); // LINE"\x1", 0 );// spazio singolo
}

void printermanager::Calc_Max_EMG()
{
    static int v_max_y[] = {
                            50,   100,   150,   200,   250,   300,   350,   400,   450,
                           500,   550,   600,   650,   700,   750,   800,   850,   900,   950,
                          1000,  1100,  1200,  1300,  1400,  1500,  1600,  1700,  1800,  1900,
                          2000,  2250,  2500,  2750,  3000,    -1
                        };

    for(int i = 0; v_max_y[i] > 0; i++)
        if(m_max_emg <= v_max_y[i]) {
            m_max_y = v_max_y[i];
            return;
        }
    m_max_y = 3250;
    return;
}

void printermanager::Pri_Rep_Gra_EMG(int __num_riga)
{
    qDebug() << "Pri_Rep_Gra_EMG(" << __num_riga << ")";
    bool label = false;
    int max_emg = (int)(m_max_y - (__num_riga / 2) * (m_max_y / 5));	// valori in unita

    for(int i = 0; i < ( LCMD + a_emg); i++ )
        m_str_gr[ i ] = 0;

    int sz = 96 + 2256 + 96;    // 	num_byte = 96 + 2256 + 96 = 2448 = ( 65536 * n3 ) + ( 256 * n2 ) + n1
    int dots = 102;             // larghezza 4+4+94=102 byte
    set_gra_Header_str_gr(sz, 0x02, dots);    // n4	doppia altezza

    if( (__num_riga == 0) || ( !(__num_riga % 2) ) )
        label = true;  // scrive i label solo nelle righe 0, 2, 4, 6, 8
    else
        label = false; // nelle righe 1, 3, 5, 7 metter caratteri vuoti

    if( label )	// se label e true devo scrivere i valori sull'asse di emg, altrimenti riempio con m caratterei vuoti
    {
        // la prima riga del grafico si compone di 24 righe di 94byte ciascuna alle quali antepongo le 24 righe di m byte del dato di emg
        // gli m byte aggiuntivi sono per le quattre cifre di dato
        printDigits(max_emg, 4, LCMD);
    }
#ifdef __ZERO_ASSE_EMG__
    else {                      // niente label m caratteri vuoti
        if( __num_riga == 9 )   // se e' l'ultima riga sovrascrivo lo "0" al terzo carattere
            for(int i = 0; i < 13; i++ )	// spazio
                m_str_gr[ LCMD + (CLD-1)*24 + (i+10) ] = (char)print7x13Set[ 0*13 + i];
    }
#endif

    // costruzione assi grafico
    Pri_Rep_Gra_Ini_Grid(__num_riga); /*crea griglia e inizializa str_gr*/

    // inserimento pixel curve
    m_str_gr[ pos_gra + 2255 ] = (char)0xFF;

    /*      Grafico   */
    if( m_tem_svu )
    {
        int x_pt = 0;
        int flow_pt = 240;
        int i4 = __num_riga * 24;
        int graf_g_x = 752/*( 10 * m_max_x )*/;
        int graf_g_fl = ( m_max_y );

        for (int ix = 1; ix < (m_num_sam - 1); ix++ ) 	     /*trovo estremi della retta passante per il campione uw6 uw6+1*/
        {
            m_x1 = x_pt;
            x_pt = (int)((752 * ix) / graf_g_x);                     /*x_pt=x2 retta */
            m_y1 = flow_pt;

            flow_pt = 239 - (int)(239 * fabs(buffer_emg[ix]) / graf_g_fl); /*y2*/
            if( flow_pt > 239 )
                flow_pt = 239;
            /*ogni riga composta da 1 linee di stampa str_gr*/
            m_x2 = x_pt;
            m_y2 = flow_pt;
            if(Int_Pun(i4))
            {
                if (m_num_xy > 25)                    //Fatal_Error(FATAL_ERROR_PRI);
                    m_num_xy = 25;
                for(int i22 = 0; i22 < m_num_xy; i22++)
                {
                    m_y[i22] -= i4;
                    int i33 = 24 * ( m_x[i22] / 8 ) + m_y[i22];
                    unsigned char v = 0x80 >> (unsigned char)(m_x[i22] % 8);
                    m_str_gr[ pos_gra + i33] |= (char)v;
                }
            }
        }
    }

    // spazi vuoti dove invece nel grafic del flw ci andavano i label del vol
    // niente label quattro caratteri vuoti dalla posizione xxx

    Str_Trasposta( 0, CLS, CLD ); // fa la trasposta di str_gr che e costruita per colonne mentre noi si stampa per righe

    m_port->Pri_Str((char *)m_str_gr, LCMD + a_emg, false );
}

/**
Nella stampa del report tipo reale, individuo il fondoscala del flusso, parametro che serve per adattare i valori
degli array dati volume e emg (e anche flusso)
*/
long printermanager::Calc_Max_Flw()
{
    long flw_max = 0;
    flw_max = (long)m_flu_max;
    flw_max = ((flw_max/10)+1)*10;	// trovo la decina minima superiore al valore max
    // FS del grafico del flusso	// puo assumere i valori 25, 50, 75, 100
    if(flw_max <= 25)
        flw_max = 25;
    else if(flw_max <= 50)
        flw_max = 50;
    else if(flw_max <= 75)
        flw_max = 75;
    else //if(flw_max <= 160)
        flw_max = 100;
    return flw_max;	// mi servira per la graficazione dei dati
}

/**
Nella stampa del report tipo reale, qui si cercano i massimi dei vari array per settare i fondoscala dei grafici
*/
void printermanager::Calc_Max_RealReport_rel2(short __num_sample)
{
    short vol_max = 0;

    for(int j = 0; j < __num_sample; j++)		// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) cosi da individuare il giusto fondoscala
        if(buffer_vol[j] > vol_max)
            vol_max = buffer_vol[j];
    // FS del grafico volume
    if( vol_max <= 250)
        vol_max = 250;
    else if( vol_max <= 500)
        vol_max = 500;
    else if( vol_max <= 750)
        vol_max = 750;
    else
        vol_max = 1000;
    m_max_vol = vol_max;
    qDebug("vmax:%d", vol_max);

    if(m_emgPresent) {
        short emg_max = 0;
        for(int j = 0; j < __num_sample; j++) {
            short v = buffer_emg[j];
            if(v < 0)
                v = -v;
            if(v > emg_max)
                emg_max = v;
        }
        if(emg_max <= 250)
            emg_max = 250;
        else if(emg_max <= 500)
            emg_max = 500;
        else if(emg_max <= 1000)
            emg_max = 1000;
        else if(emg_max <= 2000)
            emg_max = 2000;
        else if(emg_max <= 2500)
            emg_max = 2500;
        else if(emg_max <= 3000)
            emg_max = 3000;
        else if(emg_max <= 3500)
            emg_max = 3500;
        m_max_emg = emg_max;
        qDebug("emgMax:%d", emg_max);
    }
}

/**
disegna il/i grafici. prima costruisce il/gli asse/i delle ordinate della parte sinistra del grafico, dove saranno mostrati i fondoscala di flusso ed emg,
con 3 valori intermedi (1/4 FS, 1/2 FS, 3/4 FS).
poi viene costruito il grafico a pezzi di 5secondi ciascuno (40righe)
infine e cotruito l'asse delle ordinate destro, dove appare l'udm del volume e ik suo fs
*/
void printermanager::Report_Real_Time(short __num_sample, double xscale)
{
    int num_righe;
    m_init_time_to_print = 0;
    m_cursore = 0;

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);
    m_port->Pri_justif(2);	// bandiera a sinistra

    // STAMPA DELLE LABEL E DELL'ASSE SINISTRO (label EMG E FLUSSO)
    m_pos_gra_emg = 0;
    m_pos_gra_flw = 50;
    m_num_byte_x_gra_flw = 50;
    m_num_dots_gra_flw = 400;
    if(m_emgPresent) {
        m_pos_gra_vol = 20;
        m_num_dots_gra_emg = 160;
        m_num_dots_gra_vol = 240;
        m_num_byte_x_gra_emg = 20;
        m_num_byte_x_gra_vol = 30;
    }
    else {
        m_pos_gra_vol = 0;
        m_num_dots_gra_emg = 0;
        m_num_dots_gra_vol = 400;
        m_num_byte_x_gra_emg = 0;
        m_num_byte_x_gra_vol = 50;
    }
    m_num_byte_x_gra = 800;
    Pri_Rep_Label();	// stampa label del flusso ed eventualmente anche l'emg nel caso di grafici sovrapposti

    // STAMPA DEI GRAFICI DI ACQUISIZIONE, 8 RIGHE PER SECONDO DI ACQUSIZIONE
    // alloco lo spazio per la stringona del grafico
    //	righe = 40 (8 punti per secondo = 5sec)
    //	colonne = 1(label tempo) + 1(spazio) + 50(grafici) + 1(spazio) + 50(grafici) + 1(spazio)= 104byte

    // SCOMPONGO LA STAMPA DEL GRAFICO IN TANTE STAMPE DA 5 SECONDI CIASCUNA
    num_righe = (int)(__num_sample / NUM_POINTS) + 1;	// numero intero di blocchi da 40 righe (5sec), il restante e stampato in un altro blocco da 40
    Pri_Rep_Gra_Landscape(-1, xscale);      // forza init delle variabili
    for(int riga = 0; riga < num_righe; riga++ )                        // scompone la griglia in tante righe
        Pri_Rep_Gra_Landscape(__num_sample, xscale); // scrive label di tempo ad ogni accesso

    // ora stampo l'asse destro del grafico
    Pri_Rep_asse_dx();
    m_port->Pri_mode(0x00);
    m_port->Pri_Font(1);
    m_port->Pri_Str(LINE2, 2, false);
}

/**
STAMPO in corrispondenza dell'asse ascisse grafico flusso, i label del flusso, dove lo zero sara posto in corrispondenza della prima linea del grafico
si tratta di un'immagine grafica composta da una matrice di punti larga 808dots e alta (8char*8puntiperchar)=64righe, quindi 808*64=12800punti = 1600caratteri
*/
void printermanager::Pri_Rep_Label()
{
    //bool decimal = false;
    short int num_char_to_print, max_y;
    int pos_4_this_string;
    int dots3D = _NUM_CHAR_X_LABEL_rel2 * _HEIGHT_CHAR_LABEL;
    short int dim_label = 1;	// nel caso dello "0" nessun label ma scriviamo lontano dal bordo

    // riempio la stringa di zeri
    for(int i = 0; i < dim_string_rel2; i++ )
        m_str_gr[ i ] = 0;

    // setta la stampa grafica
    int sz = 6464;      // num_byte = dim_string_rel2 - 8 = 6472-8= 6464
    int dots = 101;     // larghezza 808 dots = 101 byte
    set_gra_Header_str_gr(sz, 0, dots);

    // stampo lo zero dell'emg senza udm
    if(m_emgPresent) {
        pos_4_this_string = m_pos_gra_emg;	// inizio dell'asse delle emg
        print_char_left_label(0, pos_4_this_string, 1, false, dim_label);
    }
    // stampo lo zero del vol senza udm
    pos_4_this_string =  m_pos_gra_vol * dots3D;	// inizio dell'asse del flw
    print_char_left_label(0, pos_4_this_string, 1, false, dim_label);
    // stampo lo zero del flw  senza udm
    pos_4_this_string = m_pos_gra_flw * dots3D;	// inizio dell'asse del flw
    print_char_left_label(0, pos_4_this_string, 1, false, dim_label);

    // in posizione 400-8 metto il valore massimo che potrebbe essere in 1, 2 o 3 caratteri; valore massimo ammesso 160ml/sec
    //	Imposto fondoscala fissi per avere divisioni dei valori pari, tenendo conto che ho 400 punti
    //	40ml/sec -> 1decimo per punti, 20ml/sec -> 2punti per decimo, 80ml/sec -> 2 decimi per punto, 120ml/sec -> 3 decimi punto, 160ml/sec -> 4 decimi punti
    for(int i = 1; i <= _NUM_LABEL; i++) {
        if(i == _NUM_LABEL)	{       // 5 label + lo zero
            pos_4_this_string = _POS_FC_FLW * dots3D;
            max_y = m_max_y;
        }
        else {
            pos_4_this_string = (m_pos_gra_flw + (m_num_byte_x_gra_flw/_NUM_LABEL)*i )*dots3D;
            max_y = (m_max_y / _NUM_LABEL ) * i;
        }
        if(max_y < 10)
            num_char_to_print = 1;
        else
            if(max_y < 100)
                num_char_to_print = 2;
            else
                num_char_to_print = 3;
        dim_label = 5;											// valore + " ml/s"
        print_char_left_label(max_y, pos_4_this_string, num_char_to_print, false, dim_label);
        print_udm_label(m_chFlw, pos_4_this_string, dim_label);		// scritta dell'unita di misura dell'flw, cioe ml/s, scritta a fianco di ogni label numerica
    }

    if(m_emgPresent) {							// stampo lo zero dell'emg senza udm
                                                // label dell'asse dell'volume
        for(int i = 1; i < _NUM_LABEL_VOL_EMG; i++) {			// se c'e l'emg solo 2 label piu lo zero
                                                                // non scrivo il label del fondoscala, che sarebbe troppo vicino allo zero successivo, quindi scrivo solo
            pos_4_this_string = (m_pos_gra_vol + (m_num_byte_x_gra_vol/_NUM_LABEL_VOL_EMG)*i )* dots3D;
            max_y = (m_max_vol / _NUM_LABEL_VOL_EMG ) * i;
            if(max_y < 10)
                num_char_to_print = 1; 							// stampo anche " uV" a fianco del numero
            else
                if(max_y < 100)
                    num_char_to_print = 2;
                else
                    if(max_y < 1000)
                        num_char_to_print = 3;
                    else
                        num_char_to_print = 4;
            dim_label = 3;										// valore + " ml"
            print_char_left_label(max_y, pos_4_this_string, num_char_to_print, false, dim_label);
            print_udm_label(m_chVol, pos_4_this_string, dim_label);	// scritta dell'unita di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
        // label dell'asse dell'emg
        for(int i = 1; i < _NUM_LABEL_EMG; i++) {				// un solo label oltre allo zero
            pos_4_this_string = (m_pos_gra_emg + (m_num_byte_x_gra_emg/_NUM_LABEL_EMG)*i )* dots3D;
            max_y = (m_max_emg/ _NUM_LABEL_EMG ) * i;
            if(max_y < 10)
                num_char_to_print = 1; 							// stampo anche " uV" a fianco del numero
            else
                if(max_y < 100)
                    num_char_to_print = 2;
                else
                    if(max_y < 1000)
                        num_char_to_print = 3;
                    else
                        num_char_to_print = 4;
            dim_label = 3; 										// valore + " uV"
            print_char_left_label(max_y, pos_4_this_string, num_char_to_print, false, dim_label);
            print_udm_label(m_chEmg, pos_4_this_string, dim_label);	// scritta dell'unita di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
    }
    else {														// se non c'e l'emg, meta grafico va al volume
                                                                // label dell'asse dell'volume
        for(int i = 1; i < _NUM_LABEL; i++) {					// se c'e l'emg solo 2 label piu lo zero
                                                                // non scrivo il label del fondoscala, che sarebbe troppo vicino allo zero successivo, quindi scrivo solo
            pos_4_this_string = (m_pos_gra_vol + (m_num_byte_x_gra_vol/_NUM_LABEL)*i )* dots3D;
            max_y = (m_max_vol / _NUM_LABEL ) * i;
            if(max_y < 10)
                num_char_to_print = 1; 							// stampo anche " uV" a fianco del numero
            else
                if(max_y < 100)
                    num_char_to_print = 2;
                else
                    if(max_y < 1000)
                        num_char_to_print = 3;
                    else
                        num_char_to_print = 4;
            dim_label = 3;										// valore + " ml"
            print_char_left_label(max_y, pos_4_this_string, num_char_to_print, false, dim_label);
            print_udm_label(m_chVol, pos_4_this_string, dim_label);	// scritta dell'unita di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
    }
    // aggiungo la linea dell'asse  delle ordinate, in ogni colonna l'ultimo bit viene acceso
    for(int i = 3; i < 105; i++) {
        m_str_gr[_NUM_BYTE_CMD + i*(dots3D) - 2] = (char)0xFF;
        m_str_gr[_NUM_BYTE_CMD + i*(dots3D) - 1] = (char)0xFF;
    }
    // trasposizione
    unsigned short num_col_max = dots3D;	// 32
    unsigned short num_rig_max = (dim_string_rel2 - 8) / num_col_max;	// 50

    for(int i = 0; i < (dim_string_rel2 - 8); i++ )
        m_str_tr[i] = 0;	// azzero tutta questa stringona
    for(int num_col = 0; num_col < num_col_max; num_col++ )
        for(int num_rig = 0; num_rig < num_rig_max; num_rig++)
            m_str_tr[(num_rig_max * num_col) + num_rig] = m_str_gr[_NUM_BYTE_CMD + (num_col_max * num_rig) + num_col];
    for( int str_byte = 0; str_byte < (dim_string_rel2 - 8); str_byte++ )
        m_str_gr[ _NUM_BYTE_CMD + str_byte ] = m_str_tr[ str_byte ];

    // finalmente stampa
//    m_port->Pri_Str((char *)m_str_gr, dim_string_rel2, false);
    // bitmap completa spezzata in una riga di bit per volta
    const int ByteXline = m_str_gr[7];      // bytes x linea
    qDebug() << "spezzo bitmap da" << (m_str_gr[3] * 256 + m_str_gr[2]) << "in pezzi da" << m_str_gr[2];
    m_str_gr[2] = ByteXline;        // n1
    m_str_gr[3] = 0;                // n2
    m_str_gr[4] = 0;                // n3
    char tbuf[LCMD + 256];
    memcpy(tbuf, m_str_gr, LCMD);   // comando di stampa
    char  * p = m_str_gr + LCMD;    // ptr a bitmap
    for(int i = 0; i < sz; ) {
        memcpy(tbuf+LCMD, p, ByteXline);
        m_port->Pri_Str(tbuf, LCMD+ByteXline, false);
        i += ByteXline;
        p += ByteXline;
    }
}

/**
funzione dedicata alla stampa dei caratteri dell'asse a sinistra nel caso del report reale, che
possono essere volume e flusso, oppure emg, volume e flusso
le unita di misura sono invece inserite nella funzione principale
*/
void printermanager::print_char_left_label(short __value, int __pos_in_string, short int __num_char, bool __pri_decim, short int __pre_char)
{
    int init_pos_in_string;
    short int first_char, second_char, third_char, fourth_char;
    int i;
    short int orriz_pos = _NUM_CHAR_X_LABEL_rel2 - __pre_char;
    //print_char_left_label(val_max, pos_4_this_string, num_char_label, False);
    init_pos_in_string = _NUM_BYTE_CMD + __pos_in_string + (orriz_pos - __num_char)*_HEIGHT_CHAR_LABEL;// laquarta cifra (primo carattere)

    if(__num_char == 4)		// devo scrivere 1000 o 2000
    {
        first_char = __value / 1000;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	// scrivo il carattere delle centinaia per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ first_char*_WEIGHT_CHAR_LABEL + i];
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// terza cifra
        second_char = (__value - (first_char * 1000)) / 100;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	// scrivo il carattere delle centinaia per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ second_char*_WEIGHT_CHAR_LABEL + i];
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
        third_char = ( __value - ( first_char * 1000 ) - ( second_char * 100 ) );
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
        fourth_char = (__value - (first_char * 1000 ) - ( second_char * 100 ) - third_char * 10 );
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ fourth_char*_WEIGHT_CHAR_LABEL + i];
        }
    }
    else
        if(__num_char == 3)
        {
            first_char = __value / 100;
            for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
                m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ first_char*_WEIGHT_CHAR_LABEL + i];
            }
            init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
            second_char = ( __value - ( first_char * 100 ) ) / 10;
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ second_char*_WEIGHT_CHAR_LABEL + i];
            }
            init_pos_in_string += _HEIGHT_CHAR_LABEL;
            third_char = ( __value - ( first_char * 100 ) - ( second_char * 10 ) );
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
        else if(__num_char == 2)
        {
            second_char = ( __value / 10);
            if(__pri_decim == true)	// s devo mettere il punto del decimale, sposto di una riga il carattere intero e poi accendo i dots del punto decimale, nota che c'e l'OR
            {
                __pri_decim = false;
                init_pos_in_string = init_pos_in_string - 2;	// sposto indietro per fare posto al punto del decimale
                m_str_gr[ init_pos_in_string + _HEIGHT_CHAR_LABEL - 1] |= (char)0xC0;
                m_str_gr[ init_pos_in_string + _HEIGHT_CHAR_LABEL ] |= (char)0xC0;
            }
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ init_pos_in_string + i ] |= (char)print8x8Set[ second_char*_WEIGHT_CHAR_LABEL + i];
            }
            // prima cifra (terzo carattere 2)
            init_pos_in_string += _HEIGHT_CHAR_LABEL;
            third_char = ( __value - ( second_char * 10 ) );	//third_char = ( max_y_flw - ( first_char * 100 ) - ( second_char * 10 ) );
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
        else if(__num_char == 1)	// un solo carattere, scrivo solo le unita
        {
            third_char = __value;	//third_char = ( max_y_flw - ( first_char * 100 ) - ( second_char * 10 ) );
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
}

/**
Inserisce nella stringa le udm delle curve che possono essere: ml/s, ml, uV
*/
void printermanager::print_udm_label(int ch_type, short __pos_in_string, short int __pre_char)
{
    short init_pos_in_string;
    short int orriz_pos = _NUM_CHAR_X_LABEL_rel2 - __pre_char + 1; // con il "+1" ho gia messo lo spazio vuoto fra numero e label
    int i;

    init_pos_in_string = _NUM_BYTE_CMD + __pos_in_string + orriz_pos*_HEIGHT_CHAR_LABEL - 4;// la terza cifra (primo carattere)

    if (ch_type == m_chVol)
    {
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] |= (char)print8x8Set[ 11 *_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// // passo al secondo carattere dell'u.d.m.
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] |= (char)print8x8Set[ 12 *_WEIGHT_CHAR_LABEL + i];
        }
    }
    else if (ch_type == m_chFlw)
    {
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] |= (char)print8x8Set[ 11 *_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// // passo al secondo carattere dell'u.d.m.
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] |= (char)print8x8Set[ 12 *_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ 13*_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ 14*_WEIGHT_CHAR_LABEL + i];
        }
    }
    else if (ch_type == m_chEmg)
    {
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 15*_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;	// passo alla cifra suvvessiva
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ 16*_WEIGHT_CHAR_LABEL + i];
        }
    }
}

/**
GRAFICI IN LANDASCAPE MODE in accordo con specifiche ISC (indifferentemente dal fondoscala,
l'area sottesa al grafico del flusso, deve essere proporzionale al valore di flusso, quindi anche l'asse
dei tempi si adegua al fondoscala.
funzione chiamata ciclicamente per stampare le righe in cui e' suddiviso il grafico del flusso/volume
a seconda del parametro num_rig si stampano o meno i label degli assi di flusso e volume
*/
void printermanager::Pri_Rep_Gra_Landscape(short __num_sample, double xscale)
{
    (void) xscale;
    static Print_Graph_Parameters PriGraParam_FLW;
    static Print_Graph_Parameters PriGraParam_VOL;
    static Print_Graph_Parameters PriGraParam_EMG;
    Print_Graph_Parameters PriGraParam_OLD;

    if(__num_sample < 0) {      // init
        static Print_Graph_Parameters zero = { 0, 0, 0, false };
        PriGraParam_FLW = zero;
        PriGraParam_VOL = zero;
        PriGraParam_EMG = zero;
        return;
    }

    bool stamp_x_label = false;
    int init_pos_in_string;
    float fattore_scala;

    for(int i = 0; i < dim_string_gr; i++ )	// riempio di zeri le stringhe, cioe "spengo" tutti i pixel
        m_str_gr[ i ] = m_str_tr[ i ] = 0;

    // setta la stampa grafica: i primi 8 caratteri della stringa sono comandi
    int sz   = 4160;    // 	num_byte = 4160
    int dots = 104;     //	larghezza 1+50+1+50+1=104 byte
    set_gra_Header_str_gr(sz, 0, dots);

    // incrementando init_time_to_print di 5 ad ogni chiamata di questa funzione, sara sempre un multiplo di 5
    // incrementando init_time_to_print del valore in secondi, corrispondente a 40punti stampati, ad ogni chiamata di questa funzione,
    // sara sempre un multiplo della divisione elementare (dipendente dal F.S. del flusso impostato)
    if(m_cursore == 0) {    // stampo anche il carattere "s" del secondo
        init_pos_in_string = _NUM_BYTE_CMD + (0)*_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ m_init_time_to_print*_WEIGHT_CHAR_LABEL + i];	// carattere "0"
        }
        init_pos_in_string = _NUM_BYTE_CMD + (1)*_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 14*_WEIGHT_CHAR_LABEL + i];					// carattere "S"
        }
        init_pos_in_string +=_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 17*_WEIGHT_CHAR_LABEL + i];					// carattere "e"
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 18*_WEIGHT_CHAR_LABEL + i];					// carattere "c"
        }
    }
    else {
//        stamp_x_label = check_stamp_label();	// verifica se si deve stampare il label e ne definisce il valore a seconda della scala adottata
        int num_sec = (m_cursore / 8);          // 1 secondo ogni 8 punti
        if((num_sec % 10) == 0) {           // e numero divisibile per 10sec quindi e uno dei label da stampare
            m_init_time_to_print = num_sec; // puo assumere i valori 10,20,30,40....
            stamp_x_label = true;
        }
        int t_init_time_to_print = m_init_time_to_print;

        if(stamp_x_label == true) {
            bool doit = false;
            int dig[3];
            dig[0] = t_init_time_to_print / 100;
            dig[1] = (t_init_time_to_print - (dig[0] * 100)) / 10;
            dig[2] = t_init_time_to_print - (dig[0] * 100) - (dig[1] * 10);
            for(int d = 0; d < 3; d++)
                if((dig[d] > 0) || doit || (d == 2)) {
                    doit = true;
                    init_pos_in_string = _NUM_BYTE_CMD + d*_HEIGHT_CHAR_LABEL;
                    for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )
                        m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ dig[d]*_WEIGHT_CHAR_LABEL + i];
                }
        }
    }

    // inserisco i trattini riferiti agli intervalli di 1 sec
    for(int i = 0; i < 5; i++ ) {
        if(stamp_x_label && (i == 0)) { // trattino + grosso per i multipli di cinque
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8    ] |= (char)0x0f;	// trattino corrispondente ad ogni secondo sull'asse del tempo
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8 + 1] |= (char)0x0f;
        }
        else
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8]     |= (char)0x07;
    }

    // inserisco bordo  grafico emg e flw
    for(int i = NUM_POINTS; i < 2*NUM_POINTS; i++ )
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo inferiore riga 1
    for(int i = 2*NUM_POINTS; i < 3*NUM_POINTS; i++ )
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore riga2
    for(int i = 102*NUM_POINTS; i < 103*NUM_POINTS; i++ )
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0xC0;	// bordo superiore flw doppia

    // linea separazione grafico volume e flusso a meta scala
    for(int i = 51*NUM_POINTS; i < 52*NUM_POINTS; i++ )
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo superiore volume
    for(int i = 52*NUM_POINTS; i < 53*NUM_POINTS; i++ )
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore flusso

    // inserisco tratteggi verticali, riferiti agli istanti multipli di 10sec
    if(stamp_x_label)
        for(int i = 0; i < 100; i++ )
            m_str_gr[ _NUM_BYTE_CMD + (2 + i)*NUM_POINTS] |= (char)0x44;	// tratteggio verticale istante di tempo

    // inserisco tratteggi orizzontali, riferiti ai valori della grandezza mostrata,
    // partendo dal basso le prime 4 linee tratteggiate ci sono sempre e a quota fissa
    // sulla scala 0-800 (40, 80, 120, 160). Se riferito alla matrice di 102byte
    // si tratta di (5+2, 10+2, 15+2, 20+2) cioe 6*NUM_POINTS, 10*NUM_POINTS....
    // Se c'e l'emg, allora altre 4 linee tratteggiate distanti tra loro 40,
    // a quota (240, 280, 320, 360) poi 24 linee distanti tra loro 16
    // delle quali, ogni 5, la quinta e intera, le altre tratteggiate
    // se non c'e l'emg, allora 24 linee distanti tra loro 24 delle quali, ogni 5,
    // la quinta e intera, le altre tratteggiate
    // primo grafico 4 tratteggi: 1/5, 2/5, 3/5, 4/5 del val max
    for(int i = 0; i < 5; i++ ) {
        for(int z = 0; z < 8; z++ ) {
            m_str_gr[ _NUM_BYTE_CMD + 62*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 72*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 82*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 92*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            if((z == 1) || (z == 5))
            {	// tratteggi nell'area vol+emg
                m_str_gr[ _NUM_BYTE_CMD + 12*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 22*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 32*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 42*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                // tratteggi nell'area del flusso
                m_str_gr[ _NUM_BYTE_CMD + 54*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 56*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 58*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 60*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 64*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 66*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 68*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 70*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 74*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 76*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 78*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 80*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 84*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 86*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 88*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 90*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 94*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 96*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 98*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 100*NUM_POINTS + i*_WEIGHT_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            }
        }
    }
    // se c'e anche l'emg allora devo creare i due bordi separatori dei grafici
    if(m_emgPresent) {
        for(int i = 21*NUM_POINTS; i < 22*NUM_POINTS; i++ )
            m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo superiore emg
        for(int i = 22*NUM_POINTS; i < 23*NUM_POINTS; i++ )
            m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore volume
    }

    // costruzione assi grafico: quattro sono i caratteri anteposti senza byte dei comandi
    // inserimento pixel curve
    //str_gr[ pos_gra + BYTE_GRAPH ] = (char)0xFF;
    // inserimento pixel flusso: ho 600 o 400 punti a seconda del numero di grafici
    //cursore = init_time_to_print*NUM_DOTS_X_SEC;

    fattore_scala = m_num_dots_gra_flw/(float)(10*m_max_y);	// dove max_y = 25, 50, 75, 100, quindi il rapporto = 4, 2, 1, 0.5, 0.25
//    qDebug("fattore scala FLW: %d, %f", __num_sample, fattore_scala);
    for(int i = 0; i < NUM_POINTS; i++) {
        PriGraParam_OLD.uw8 = PriGraParam_FLW.uw8;
        PriGraParam_OLD.uw9 = PriGraParam_FLW.uw9;
        PriGraParam_OLD.sample_adattato = PriGraParam_FLW.sample_adattato;				// salviamo il valore precedente
        if((i+m_cursore) < __num_sample) {
            PriGraParam_FLW.sample_adattato = (int)(buffer_flw[i + m_cursore] * fattore_scala);	// adattamento del valore corrente al fondoscala corrente

            if(PriGraParam_FLW.sample_adattato > 0) {
                PriGraParam_FLW.uw8 = PriGraParam_FLW.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                PriGraParam_FLW.uw9 = PriGraParam_FLW.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo pero da MSB nel byte individuato
                if(PriGraParam_FLW.sample_adattato > PriGraParam_OLD.sample_adattato) {
                    PriGraParam_FLW.verso_curva = true;
                    if(PriGraParam_FLW.uw8 > PriGraParam_OLD.uw8) {
                        for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_FLW.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_FLW.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                        for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                    else {
                        for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_FLW.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                }
                else {
                    if(PriGraParam_FLW.sample_adattato < PriGraParam_OLD.sample_adattato) {
                        if(PriGraParam_FLW.verso_curva == true) {
                            // niente riga perche si sovrapporrebbe con quella del precedente
                            PriGraParam_FLW.verso_curva = false;
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_FLW.uw9); // qui niente riga solo un punto.
                        }
                        else {	// qui invece la riga dal valore prec all'attuale
                            if( PriGraParam_FLW.uw8 < PriGraParam_OLD.uw8) {
                                for(int j = (PriGraParam_FLW.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                                for(int j = PriGraParam_FLW.uw9; j < 8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                            else {
                                for(int j = PriGraParam_FLW.uw9; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                        }
                    }
                    else {	// qui niente riga solo un punto.
                        PriGraParam_FLW.verso_curva = false;
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_FLW.uw9); // qui niente riga solo un punto.
                    }
                }
            }
            else {	// il nuovo valore e 0 (reale o adattato) comunque sta sull'asse
                PriGraParam_FLW.uw8 = PriGraParam_FLW.uw9 = 0;
                PriGraParam_FLW.verso_curva = true;
                if(PriGraParam_OLD.sample_adattato > 0) { // allora riga da 0 a sample_adattato_prec
                    for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);
                }
                else
                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in piu allo zero per evidenziare il valore a zero
            }
        }
        else
            break;
    }

    fattore_scala = m_num_dots_gra_vol / (m_max_vol);	// dove max_vol = 250, 500, 750, 1000
//    qDebug("fattore scala VOL: %f", fattore_scala);
    for(int i = 0; i < NUM_POINTS; i++) {
        PriGraParam_OLD.uw8 = PriGraParam_VOL.uw8;
        PriGraParam_OLD.uw9 = PriGraParam_VOL.uw9;
        PriGraParam_OLD.sample_adattato = PriGraParam_VOL.sample_adattato;				// salviamo il valore precedente
        if((i+m_cursore) < __num_sample) {
            PriGraParam_VOL.sample_adattato = (int)(buffer_vol[i + m_cursore] * fattore_scala);	// adattamento del valore corrente al fondoscala corrente

            if(PriGraParam_VOL.sample_adattato > 0) {
                PriGraParam_VOL.uw8 = PriGraParam_VOL.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                PriGraParam_VOL.uw9 = PriGraParam_VOL.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo pero da MSB nel byte individuato
                if(PriGraParam_VOL.sample_adattato > PriGraParam_OLD.sample_adattato) {
                    PriGraParam_VOL.verso_curva = true;
                    if(PriGraParam_VOL.uw8 > PriGraParam_OLD.uw8) {
                        for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_VOL.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_VOL.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                    else {
                        for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_VOL.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                }
                else
                    if(PriGraParam_VOL.sample_adattato < PriGraParam_OLD.sample_adattato) {
                        if(PriGraParam_VOL.verso_curva == true) { // niente riga perche si sovrapporrebbe con quella del precedente
                            PriGraParam_VOL.verso_curva = false;
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_VOL.uw9); // qui niente riga solo un punto.
                        }
                        else { // qui invece la riga dal valore prec all'attuale
                            if(PriGraParam_VOL.uw8 < PriGraParam_OLD.uw8) {
                                for(int j = (PriGraParam_VOL.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                for(int j = PriGraParam_VOL.uw9; j < 8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                            else {
                                for(int j = PriGraParam_VOL.uw9; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                        }
                    }
                    else { // qui niente riga solo un punto.
                        PriGraParam_VOL.verso_curva = false;
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_VOL.uw9); // qui niente riga solo un punto.
                    }
            }
            else { // il nuovo valore e 0 (reale o adattato) comunque sta sull'asse
                PriGraParam_VOL.uw8 = PriGraParam_VOL.uw9 = 0;
                PriGraParam_VOL.verso_curva = true;
                if(PriGraParam_OLD.sample_adattato > 0) { // allora riga da 0 a sample_adattato_prec
                    for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);
                }
                else {
                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in piu allo zero per evidenziare il valore a zero
                }
            }
        }
        else
            break;
    }

    if(m_emgPresent) {
        fattore_scala = m_num_dots_gra_emg / (m_max_emg);	// dove max_y = 10, 20, 40, 80, 160, quindi il rapporto = 4, 2, 1, 0.5, 0.25
//        qDebug()<<"m_num_dots_gra_emg:"<<m_num_dots_gra_emg<<"m_max_emg:"<<m_max_emg;
//        qDebug("fattore scala EMG:%f m_max_emg:%f __num_sample:%d m_cursore:%d", fattore_scala, m_max_emg, __num_sample, m_cursore);
        for(int i = 0; i < NUM_POINTS; i++) {
            PriGraParam_OLD.uw8 = PriGraParam_EMG.uw8;
            PriGraParam_OLD.uw9 = PriGraParam_EMG.uw9;
            int absV = PriGraParam_EMG.sample_adattato;
            PriGraParam_OLD.sample_adattato = absV;				// salviamo il valore precedente
            if((i+m_cursore) < __num_sample) {
                absV = (int)(buffer_emg[i + m_cursore]);	// adattamento del valore corrente al fondoscala corrente
                absV = (int)(absV * fattore_scala);
                absV = (absV < 0) ? -absV : absV;
                if(absV >= m_num_dots_gra_emg)
                    absV = m_num_dots_gra_emg - 1;
                PriGraParam_EMG.sample_adattato = absV;

                if(PriGraParam_EMG.sample_adattato > 0) {
                    PriGraParam_EMG.uw8 = PriGraParam_EMG.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                    PriGraParam_EMG.uw9 = PriGraParam_EMG.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo pero da MSB nel byte individuato
                    if(PriGraParam_EMG.sample_adattato > PriGraParam_OLD.sample_adattato) {
                        PriGraParam_EMG.verso_curva = true;
                        if(PriGraParam_EMG.uw8 > PriGraParam_OLD.uw8) {
                            for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_EMG.uw8; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                            for(int j = 0; j < PriGraParam_EMG.uw9; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        }
                        else {
                            for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_EMG.uw9; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        }
                    }
                    else
                        if(PriGraParam_EMG.sample_adattato < PriGraParam_OLD.sample_adattato) {
                            if(PriGraParam_EMG.verso_curva == true) { // niente riga perche si sovrapporrebbe con quella del precedente
                                PriGraParam_EMG.verso_curva = false;
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_EMG.uw9); // qui niente riga solo un punto.
                            }
                            else { // qui invece la riga dal valore prec all'attuale
                                if(PriGraParam_EMG.uw8 < PriGraParam_OLD.uw8) {
                                    for(int j = (PriGraParam_EMG.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                    for(int j = PriGraParam_EMG.uw9; j < 8; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                }
                                else {
                                    for(int j = PriGraParam_EMG.uw9; j < PriGraParam_OLD.uw9; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                }
                            }
                        }
                        else { // qui niente riga solo un punto.
                            PriGraParam_EMG.verso_curva = false;
                            m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_EMG.uw9); // qui niente riga solo un punto.
                        }
                }
                else { // il nuovo valore e 0 (reale o adattato) comunque sta sull'asse
                    PriGraParam_EMG.uw8 = PriGraParam_EMG.uw9 = 0;
                    PriGraParam_EMG.verso_curva = true;
                    if(PriGraParam_OLD.sample_adattato > 0) { // allora riga da 0 a sample_adattato_prec
                        for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);
                    }
                    else {
                        m_str_gr[_NUM_BYTE_CMD + (2)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in piu allo zero per evidenziare il valore a zero
                    }
                }
            }
            else
                break;
        }
//        qDebug("fattore scala EMG: FINE");
    }

    m_cursore += NUM_POINTS;	// incrementato di 40 punti (quindi sample) ogni giro

    // trasposizione
    int c_max = NUM_POINTS;	// 40
    int r_max = (dim_string_gr -8) / c_max;	// 104

    for(int i = 0; i < (dim_string_gr - 8); i++ )
        m_str_tr[i] = 0;	// azzero tutta questa stringona
    for(int c = 0; c < c_max; c++ )
        for(int r = 0; r < r_max; r++) {
            int src = ( c_max * r ) + c + _NUM_BYTE_CMD;
            int dst = ( r_max * c ) + r ;
            m_str_tr[dst] = m_str_gr[src];
        }

    for(int i = 0; i < (dim_string_gr - 8); i++ )
        m_str_gr[ _NUM_BYTE_CMD + i ] = m_str_tr[ i ];

    // finalmente stampa
//    m_port->Pri_Str((char *)m_str_gr, dim_string_gr, false);
    // bitmap completa spezzata in una riga di bit per volta
    const int ByteXline = m_str_gr[7];      // bytes x linea
    qDebug() << "spezzo bitmap da" << (m_str_gr[3] * 256 + m_str_gr[2]) << "in pezzi da" << m_str_gr[2];
    m_str_gr[2] = ByteXline;        // n1
    m_str_gr[3] = 0;                // n2
    m_str_gr[4] = 0;                // n3
    char tbuf[LCMD + 256];
    memcpy(tbuf, m_str_gr, LCMD);   // comando di stampa
    char  * p = m_str_gr + LCMD;    // ptr a bitmap
    for(int i = 0; i < sz; ) {
        memcpy(tbuf+LCMD, p, ByteXline);
        m_port->Pri_Str(tbuf, LCMD+ByteXline, false);
        i += ByteXline;
        p += ByteXline;
    }
}

/**
a seconda del massimo del flusso, si sceglie un fondo scala verticale e il conseguente asse dei tempi.
A seconda dei sample fin qui stampati,
si risale al tempo per comandare la stampa del label (in secondi) o meno
*/
bool printermanager::check_stamp_label()
{
    int num_sec;

    switch(m_max_y)	// i F.S. previsti per il flusso in ml/sec
    {
    case 25:	// ml/sec
        num_sec = (m_cursore / 16);         // 1 secondo ogni 16 punti
        if((num_sec % 5) == 0) {            // e numero divisibile per 5sec quindi e uno dei label da stampare
            m_init_time_to_print = num_sec; // puo assumere i valori 5,10,15,20,25,30....
            return true;
        }
        break;
    case 50:
        num_sec = (m_cursore / 8);          // 1 secondo ogni 8 punti
        if((num_sec % 10) == 0) {           // e numero divisibile per 10sec quindi e uno dei label da stampare
            m_init_time_to_print = num_sec; // puo assumere i valori 10,20,30,40....
            return true;
        }
        break;
    case 75:
        num_sec = (m_cursore * 3 / 16);     // 3 second1 ogni 16 punti
        if((num_sec % 15) == 0) {           // e numero divisibile per 10sec quindi e uno dei label da stampare
            m_init_time_to_print = num_sec; // puo assumere i valori 10,20,30,40....
            return true;
        }
        break;
    case 100:
        num_sec = (m_cursore / 4);          // 3 second1 ogni 16 punti
        if((num_sec % 20) == 0) {           // e numero divisibile per 10sec quindi e uno dei label da stampare
            m_init_time_to_print = num_sec;		// puo assumere i valori 10,20,30,40....
            return true;
        }
        break;
    }
    return false;
}

/**
Deve stampare l'asse che chiude il grafico e sopra stmpare l label del volume
*/
void printermanager::Pri_Rep_asse_dx()
{
    // riempio la stringa di zeri
    for(int i = 0; i < dim_string_solo_ax; i++ )
        m_str_gr[ i ] = 0;

    // setta la stampa grafica
    int sz   = 816;    // 	num_byte = dim_string_solo_ax - 8 = 816
    int dots = 102;     // larghezza 816 dots = 102 byte
    set_gra_Header_str_gr(sz, 0, dots);

    // aggiungo la linea dell'asse  delle ordinate, in ogni colonna il primo bit viene acceso
    m_str_gr[_NUM_BYTE_CMD] = (char)0x01;
    m_str_gr[_NUM_BYTE_CMD + 1] = (char)0x01;
    for(int i = 2; i < 105; i++)
    {	// riga doppia
        m_str_gr[_NUM_BYTE_CMD + i*(_NUM_CHAR_X_AXES_DX * _HEIGHT_CHAR_LABEL)] = (char)0xFF;
        m_str_gr[_NUM_BYTE_CMD + i*(_NUM_CHAR_X_AXES_DX * _HEIGHT_CHAR_LABEL) + 1] = (char)0xFF;
    }
    m_str_gr[_NUM_BYTE_CMD + 101*(_NUM_CHAR_X_AXES_DX * _HEIGHT_CHAR_LABEL)] = (char)0xc0;
    m_str_gr[_NUM_BYTE_CMD + 101*(_NUM_CHAR_X_AXES_DX * _HEIGHT_CHAR_LABEL) + 1] = (char)0xc0;
    // trasposizione
    unsigned short num_col_max = _HEIGHT_CHAR_LABEL * _NUM_CHAR_X_AXES_DX;	// 32
    unsigned short num_rig_max = (dim_string_solo_ax - 8) / num_col_max;	// 50

    for(int i = 0; i < (dim_string_solo_ax - 8); i++ )
        m_str_tr[i] = 0;	// azzero tutta questa stringona
    for(int num_col = 0; num_col < num_col_max; num_col++ )
    {
        for(int num_rig = 0; num_rig < num_rig_max; num_rig++)
        {
            m_str_tr[ ( num_rig_max * num_col ) + num_rig ] = m_str_gr[ _NUM_BYTE_CMD + ( num_col_max * num_rig ) + num_col ];
        }
    }
    for(int str_byte = 0; str_byte < (dim_string_solo_ax - 8); str_byte++ )
        m_str_gr[ _NUM_BYTE_CMD + str_byte ] = m_str_tr[ str_byte ];

    // finalmente stampa
    m_port->Pri_Str((char *)m_str_gr, dim_string_solo_ax, false);

}

/**
Stampa dei nomogrammi
*/
void printermanager::Report_BitMap(bool __isSiro)
{
    qDebug("inizio pr bitm");
    int     sz;
    char  * p;
    if (__isSiro)
    {
        sz = m_bitmapSiroky.size();
        p = m_bitmapSiroky.data();
    }
    else
    {
        sz = m_bitmapLiverpool.size();
        p = m_bitmapLiverpool.data();
    }

    int     szchunk = 103;  // 824/8
    char    head[8+103];
    qDebug("sz:%d szch:%d p:%p", sz,szchunk,p);

    head[0] = ESC;	//0x1B;	// ESC
    head[1] = '*';	//0x2A;	// *
    head[2] = szchunk         & 0xff;	// n1
    head[3] = (szchunk >>  8) & 0xff;	// n2
    head[4] = (szchunk >> 16) & 0xff;	// n3
    head[5] =  0;	// n4	singola altezza
    head[6] =  0;	// n5	scrive a n5 byte dal bordo
    head[7] = szchunk;	// n6

    for(int s = 0; s < sz; ) {
        //for(int n = 0; (n < 1000) && (m_port->status(false) & (1 << 3)); n++);
        //m_port->status(false);
        memcpy(head+8, p, szchunk);
        m_port->Pri_Str(head, 8+szchunk, false);
        p += szchunk;
        s += szchunk;
    }

    m_port->Pri_Font(1);
    m_port->Pri_mode(0x00);
    m_port->Pri_justif(F_left);
    m_port->Pri_Str(LINE2, 2, true);
    qDebug("fine pr bitm");
}

/**
stampa l'elenco dei dati di analisi delle curve
*/
void printermanager::Report_result()
{
    m_port->Pri_Font(1);
    m_port->Pri_mode(0x10);
    m_port->Pri_justif(F_center);
    QString flures = tr("Flowmetry Results");
    m_port->Pri_Str(flures.toLatin1().data(), strlen(flures.toLatin1().data()), true);

    m_port->Pri_justif(F_left);
    // tolgo l'acapo automatico della Pri_Rep_Lin e lo metto manuale
    m_port->Pri_Font(1);
    m_port->Pri_mode(0x00);

    if (m_modal_e == 2) {   // il tempo di attesa e' graficato solo se esame manuale
        Pri_Rep_Lin(tr("Waiting time ................"), (int)((m_tem_att)*Fc_FLW), 1, "s\n", false);
    }
    if (m_flu_med > m_flu_max) // piccolo controllo per gestire flussi abnormali, tipici di prove da laboratorio
        if (m_tem_flu < 30) // se la flussata e molto breve e intensa, l'algoritmo sbaglia e puo risultare flu_med > flu_max
            m_flu_med = m_flu_max;

    Pri_Rep_Lin(tr("Maximum flow rate ..........."), (int)(m_flu_max * Fc_FLW), 1, "ml/s\n", false);
    Pri_Rep_Lin(tr("Average flow rate ..........."), (int)(m_flu_med * Fc_FLW), 1, "ml/s\n", false);
    Pri_Rep_Lin(tr("Time to maximum flow ........"), (int)(m_tem_max * Fc_FLW), 1, "s\n",    false);
    Pri_Rep_Lin(tr("Time between 5% and 95% ....."), (int)(m_tem_595 * Fc_FLW), 1, "s\n",    false);
    Pri_Rep_Lin(tr("Flow time ..................."), (int)(m_tem_flu * Fc_FLW), 1, "s\n",    false);
    Pri_Rep_Lin(tr("Descent time ................"), (int)(m_tem_dis * Fc_FLW), 1, "s\n",    false);
    Pri_Rep_Lin(tr("Voiding time ................"), (int)(m_tem_svu * Fc_FLW), 1, "s\n",    false);
    Pri_Rep_Lin(tr("Volume to maximum flow ......"), (int)(m_vol_max * Fc_FLW), 1, "ml\n",   false);
    Pri_Rep_Lin(tr("Voided Volume ..............."), (int)(m_vol_vuo * Fc_FLW), 1, "ml\n",   false);
    Pri_Rep_Lin(tr("Corrected maximum flow ......"), (int)(m_cQ      * Fc_FLW), 1, "ml^(1/2)/s\n", false);
    Pri_Rep_Lin(tr("Flow acceleration ..........."), (int)(m_flu_acc * Fc_FLW), 1, "ml/s^2\n", false);    //?=2 apice
    Pri_Rep_Lin(tr("Maximum contraction speed ..."), (int)(m_vDetMax * Fc_FLW), 1, "mm/s\n", false);
    Pri_Rep_Lin(tr("Residual volume ............."), (int)(m_resVol  * Fc_FLW), 1, "ml\n",   false);
}

/**
      Stampa una linea del Report con :
         -   "str_des"  =  Stringa Descrizione
         -   "rep_dat"  =  Dato del Report
         -   "num_dec"  =  Numero di Decimali
         -   "str_udm"  =  Stringa Unita' di Misura
         -   "flag_lf"  =  Flag LF Finale
*/
void printermanager::Pri_Rep_Lin(QString __descr, int __rep_dat, int __num_dec, const char *__str_udm, bool __flag_lf)
{
    char str2[40];

    if(__rep_dat < 0)
        sprintf(str2, " n.c.");
    else {
        if (__num_dec == 0)
            sprintf(str2,"%5u", __rep_dat);
        else {
            int dec = 1;
            for (int i = 0; i < __num_dec; i++)
                dec *= 10;
            sprintf(str2, "%u.%1u", __rep_dat / dec, __rep_dat % dec);
        }
    }
    sprintf(m_str_gr, "  %s :", __descr.toLatin1().data());		// allontano la riga dal bordo sinistro di 2 spazi

    char space_bar[21] = "                    ";
    if (strlen(str2) > 5) {// quando esami lunghissimi, si puo arrivare a lavorare con valori in migliaia di secondi e col decimale la stringa e lunga 6
        int d_strlen = strlen(str2) - 5; // termine di correzione in funzione della lunghezza della stringa str2
        strcat(m_str_gr, space_bar + (15 - d_strlen) + strlen(str2));
    }
    else
        strcat(m_str_gr, space_bar + 15 + strlen(str2));

    strcat(m_str_gr, str2);
    strcat(m_str_gr, "  ");
    strcat(m_str_gr, __str_udm);
    m_port->Pri_Str(m_str_gr, strlen(m_str_gr), __flag_lf);
}

// conversione da RGB 8*3 = 24 bit a RGB 4*3 = 12 bit
// ignorati i 4 bit bassi di ogni colore * ridurre le sfumature ad un colore di base
#define NCOLORS 16*16*16
inline uint16_t decimazioneColore(uint32_t rgb24)
{
    uint32_t  colormask = 0x00f0f0f0;
    uint32_t rgb12 = rgb24 & colormask;

    rgb12 = ((rgb12 & 0x000000f0) >>  4) *   1 |
            ((rgb12 & 0x0000f000) >> 12) *  16 |
            ((rgb12 & 0x00f00000) >> 20) * 256;

    return (uint16_t) rgb12;
}

void printermanager::getGrabbedImage(QObject *__gi, QString __nome)
{
    QQuickItemGrabResult *item = qobject_cast<QQuickItemGrabResult *>(__gi);
    getImage(item->image(),__nome);
}

void printermanager::getImage(QImage __img, QString __nome)
{
    bool isSiro = __nome.startsWith("Siro");
    bool isLive = __nome.startsWith("Live");

    QByteArray *cur_bitmap;
    if (isSiro && m_printSiroky)
        cur_bitmap = &m_bitmapSiroky;
    else if (isLive && m_printLiverpool)
        cur_bitmap = &m_bitmapLiverpool;
    else
        return;

    bool isAve = __nome.contains(" Ave");         // test Average / QMax
    int  xoffs = isAve  ? 8 : (8 + 400 + 16);   // horizz pixel offset

    QSize   qs = __img.size(); 
    int     w0 = qs.width();
    int     h0 = qs.height();

    // frequenza colori
    int     bmsz = w0 * h0;         // image size
    uint8_t *bm = (uint8_t *) malloc(bmsz); // per non aggiungere 120k allo stack
                                            // non viene azzerata perche ogni singolo byte viene comunque assegnato
    int32_t pale_cnt[NCOLORS];      // n.occorrenze del colore decimato
    int32_t pale_indx[NCOLORS];     // da colore decimato a n.progressivo
    int     pale_revindx[NCOLORS];  // da n.progressivo a colore decimato
    int     pale_seq = 0;

    memset((void *) & pale_cnt,0,sizeof(pale_cnt));
    for(int i = 0; i < NCOLORS; i++)
        pale_indx[i] = -1;

    // decimazione colori per eliminare le sfumature
    uint16_t rgbSiro1 = decimazioneColore(0xff90ee90);   // colore area
    uint16_t rgbSiro2 = decimazioneColore(0xffffdab9);   // colore area
    uint16_t rgbGrid  = decimazioneColore(0xff808080);   // colore griglia
    uint8_t *slider = bm;
    for(int h = 0; h < h0; h++)                             // immagine scandita in modo raster
        for(int w = 0; w < w0; w++) {
            uint16_t rgb12 = decimazioneColore(__img.pixel(w, h));
            int cur_color;
            if(pale_cnt[rgb12] == 0) {                      // prima occorrenza del colore
                pale_indx[rgb12] = cur_color = pale_seq++;  // n. progressivo colore
                pale_revindx[cur_color] = rgb12;            // da n.colore a colore
            }
            else
                cur_color = pale_indx[rgb12];
            *slider++ = cur_color;                          // serializzazione colori ridotti
            pale_cnt[rgb12]++;                              // statistica
        }

 //   for (int i = 0; i < NCOLORS; i++) if(pale_cnt[i]) qDebug("palette %3.3x cnt:%d", i, pale_cnt[i]);
 //   for(int i = 0; i < pale_seq; i++) qDebug("col:%d %3.3x cnt:%d", i, pale_revindx[i], pale_cnt[pale_revindx[i]]);

    int nblack = 0;
    char  * d = cur_bitmap->data();
    slider = bm;
    for(int h = 0; h < h0; h++) {                           // raster
        int rowstart  = (h * m_resultBm_w + xoffs) / 8;
        for(int w = 0; w < w0; w++) {
            if((w & 7) == 0)                                // clear byte 8bit risultato in B/N
                d[rowstart] = 0;
            int ncolor = *slider++;                         // de-serializzazione
            int rgb12 = pale_revindx[ncolor];
            bool blackdot = (pale_cnt[rgb12] < 3000) || (rgb12 == rgbGrid); // immagine standard

            if( ! blackdot && isSiro) {                     // livelli di grigio per Siroky
                if(rgb12 == rgbSiro2) {
                    if(((w % 3) == 1) && ((h % 3) >= 1))
                        blackdot = true;
                }
                if(rgb12 == rgbSiro1) {
                    if(((w % 3) == 1) && ((h % 3) == 1))
                        blackdot = true;
                }
            }

            if(blackdot) {                  // dot in stampa
                int byteinrow = w >> 3;
                int bitinbyte = 7 - (w & 7);
                d[rowstart + byteinrow] |= 1 << bitinbyte;
                nblack++;                   // statistica debug
            }
        }
    }

    free(bm);
    qDebug("fine getGrabbed, nblack:%d", nblack);
}

