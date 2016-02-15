#include "printermanager.h"


printermanager::printermanager(QString __namefile, QObject *parent) : QObject(parent)
{
    m_namefile = __namefile;

    m_dfm = new DatafileManager();
    m_dfm->SetFileName(m_namefile);
    m_dfm->SetFileType(7);

    m_printMode = PORTRAIT_MODE;
    m_printModeUser = PORTRAIT_MODE;
    m_printSyroky = false;
    m_realDots = 0;

    m_modal_e = 0;
    m_numTest = -1;
    m_test_type = false;
    m_correct = false;
    m_emgPresent = false;
    m_i_max_x = -1;
    m_max_x = -1;
    m_max_y = -1;
    m_uw3 = 1;
    m_cursore = 0;

    m_chVol = -1;
    m_chFlw = -1;
    m_chEmg = -1;

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
    m_name = m_dfm->GetPatient().section(";",0,0);
    m_surname = m_dfm->GetPatient().section(";",1,1);
    QDate date = QDate(1899,12,30).addDays(m_dfm->GetPatient().section(";",3,3).toInt());
    m_dateofbirth = date.toString();
    m_sex = *( m_dfm->GetPatient().section(";",12,12).toLatin1().data());
    m_ID = m_dfm->GetPatient().section(";",3,3);
    //esame
    m_numchan = m_dfm->GetChanNum();
    m_numTest = m_dfm->GetTestNum();

    QDate dateExam = QDate(1899,12,30).addDays(m_dfm->GetDataEsame());
    m_dateofexam = dateExam.toString();

    QTime time = QTime(0,0,0).addSecs(m_dfm->GetStartTime());
    m_timeofstart = time.toString();	//ora inizio esame

    m_durata = m_dfm->GetDuration()/1000; //in sec

     // discrimina se esame veloce oppure completo di dati paziente (servirà nel report di stampa)
    if (m_name == "Anonymous")
        m_test_type = true;

    //dove leggo cosa ha scelto l'utente
    //m_printModeUser = lettura;
    //m_printSyroky = lettura;
    if(m_printModeUser)
        m_printMode = LANDSCAPE_MODE;
    else
        m_printMode = PORTRAIT_MODE;

    int minNas = 5000;
    for (int i=0; i<m_dfm->GetChanNum(); i++)
    {
        int nas = m_dfm->GetNAS(i);
        if (nas < minNas)
            minNas = nas;
    }
    m_num_sam = minNas*m_durata;
    m_realDots = m_num_sam;

    // Dati relativi alla stampa
    if (m_num_sam >= NUMOF_X_PRINT_DOTS)
    {
        if(m_printMode == PORTRAIT_MODE)
            m_realDots = NUMOF_X_PRINT_DOTS;
        else	// cioè nel caso di Report_Print_Mode = Landscape
        {
            if(m_num_sam > NUMOFMAX_LEGHT_PORTRAIT_REP)	// esame troppo lungo per la modalità di stampa LANDSCAPE (5minuti)
            {
                m_printMode = PORTRAIT_MODE;
                m_realDots = NUMOF_X_PRINT_DOTS;
            }
        }
    }

    m_dfm->Close();


//    printerserialport *port;
     m_port = new printerserialport(this);
     m_port->init_printer();

    pri_rep_review();	// qui va subito in stampa

     //m_port->closeSerialPort();
    return ;
}

void printermanager::pri_rep_review()
{
    int max_volume = 0;

    m_dfm->Open();
    m_dfm->GetParameters();
    int numCh = m_numchan;

    buffer_emg = new double[NUMOF_X_PRINT_DOTS];
    buffer_flw = new double[NUMOF_X_PRINT_DOTS];
    buffer_vol = new double[NUMOF_X_PRINT_DOTS];

    if (m_printMode == PORTRAIT_MODE)
    {
        for (int c=0; c<numCh;c++)
        {
            QString chName = m_dfm->GetChanName(c);
            if(chName == "EMG")
            {
                m_dfm->GetChVal(c,0,m_realDots,buffer_emg,0);
                m_emgPresent = true;
                m_chEmg = c;
            }
            if(chName == "Q")
            {
                m_dfm->GetChVal(c,0,m_realDots,buffer_flw,0);
                m_chFlw =c;
            }
            if(chName == "VLMv")
            {
                m_chVol =c;
                m_dfm->GetChVal(c,0,m_realDots,buffer_vol,0);
            }

        }
        smooting_PRINT_flow(); // qui riempe il buffer di stampa con il set mediato dei campioni di flusso

        for (int i=0; i<(m_realDots - 1);i++)	// cerco il massimo del buffer volume
        {
            if (buffer_vol[i] > max_volume)
                max_volume = buffer_vol[i];
        }
        for (int i=(m_realDots - 1); i<NUMOF_X_PRINT_DOTS; i++)	// usare realDots
        {
            buffer_vol[i] = max_volume;
            buffer_flw[i] = 0;
        }
        if (m_emgPresent)
        {
            for (int i=(m_realDots - 1); i<NUMOF_X_PRINT_DOTS; i++){ 	// usare realDots
                buffer_emg[i] = buffer_emg[m_realDots - 2];
            }
        }
    }

   m_dfm->Close();

   Pri_Rep();




}

/**
il buffer dei campioni reali viene dato in pasto ad un algoritmo di media mobile
Alla fine ogni valore del buffer è il valore mediano di una finestra di 5 campioni originali a cavallo di questo, cioè i 2 precedenti, il campione stesso e i 2 successivi
In questo modo il primo significativo è il 3, così al primo è assegnato un valore pari ad 1/4 del valore del terzo, e al secondo un valore pari a 1/2 del terzo
Al penultimo un valore pari a 1/2 del terzultimo, all'ultimo un valore pari ad 1/4 del terzultimo
*/
void printermanager::smooting_PRINT_flow()
{
    short media;
    short somma;

    // ripeto tutto per il buffer di stampa
    for (int i=0; i<(m_realDots - WIN); i++)
    {
        somma = 0;
        media = 0;
        for (int j=i; j<i + WIN; j++)
            somma = somma + buffer_flw[j];
        media = somma/WIN;
        buffer_flw[i + WIN/2] = media;
        if (i == 0)
        {
            // i due campioni precednti al primo calcolato, li metto a mano, smorzandoli della metà
            buffer_flw[(WIN/2) - 2] = media/4; // primo o secondo campione della finestra
            buffer_flw[(WIN/2) - 1] = media/2; // secondo o terzo campione della finestraù
        }
    }

    for (int j=(m_realDots - WIN/2); j< m_realDots; j++) {
        media = media/2;
        if (media < 0)
            media = 0;
        buffer_flw[j] = media;
    }

}


//funzioni che erano nel file Report.cpp
/**
la stampa è prevista cmunque sempre dopo il review, si avvale dei dati di review, così usiamo
gli stessi comandi sia che ci si trovi a fine esame, sia che sia un review di esamei in MMC. Allora al posto della struttura Patient_Data, ci metiamo DatiPaziente.
questa funzione gestisce tutta la stampa del report, sia in modalita portrait che landscape
*/
void printermanager::Pri_Rep()//byte numCurve, char* num_file_to_print, bool print_mode)
{	/*	Stampa il Report	*/

    m_port->Pri_Speed(0);

    //      Intestazione
#if PRI_REP_INT
    Intest();
#endif

    //     Identificativi Esame
#if PRI_REP_IDE
    Report_data();		// scrive i dati del paziente e lo spazio per le note manuali
     m_port->Pri_Str(3,(char*)LINE"\x1",0);
#endif

    // Grafico FLW + VOL ed eventualmente EMG
#if PRI_REP_GRA

    if(m_printMode == PORTRAIT_MODE)			// grafico trasversale con numero di punti fisso
    {
        m_correct = false;
        m_PointsToPrintout = m_num_sam;			//Deve stampare solo quelli necessari, quelli calcolati nella funzione review
        // il numero di campioni da stampare è il numero di blocchi scritti su card; il -1 è perchè per qualche motivo i campioni buoni nel buffer_vol vanno dallo 0 allo realDots-2
        if( m_PointsToPrintout > NUMOF_X_PRINT_DOTS )
        {
            m_PointsToPrintout = NUMOF_X_PRINT_DOTS;
            //tem_exm_corr = rep.num_sam;
            m_correct = true;
        }
        m_max_vol = buffer_vol[0];
        m_max_emg = buffer_emg[0];

        for (int uw7=0; uw7<NUMOF_X_PRINT_DOTS; uw7++)	{	// cerco il massimo del buffer volume
            if (buffer_vol[uw7] > m_max_vol)
                m_max_vol = buffer_vol[uw7];
            if (buffer_emg[uw7] > m_max_emg)
                m_max_emg = buffer_emg[uw7];  // supponiamo 4000 in uVolt
        }

        if (m_printModeUser)	// è settata la stampa in landscape ma l'esame è trooppo lungo
        {
            m_port->Pri_justif(0);
            m_port->Pri_mode(0x80);	// sottolineato e doppia larghezza
            QString strToWrite = tr("! Examination longer than 5 minutes");
            char str[60];
            sprintf( str, "%s\n", strToWrite.toLatin1().data());
            m_port->Pri_Str( strlen(str), str, 1 );
            m_port->Pri_justif(2);	// bandiera a sinistra
        }

        Report_flw();	// finalmente stampiamo i grafici di volume e flusso
        if(m_emgPresent)
            Report_emg();						// EMG
    }
    else		// print_mode = LANDSCAPE_MODE - grafico longitudinale, con lunghezza legata alla lunghezza dell'esame
    {
        short samples_to_print;
        m_max_y = Calc_Max_Flw();			// fondo scala del flusso
        samples_to_print = adatta_buffer_dati(m_realDots, m_max_y);
        Calc_Max_RealReport_rel2(samples_to_print);
        Report_Real_Time(samples_to_print);		// finalmente stampa
    }
#endif

    // Grafico Siroky, stampato solo se paziente maschio oppure generico (cioè dove non indicato il sesso)

#if PRI_REP_SRK

    Report_siroky();

//    if(m_printSyroky )	{
//        if( ( m_sex != 'F' ) || (m_test_type != 0x00 ))		// così stampa sempre nel caso di esame veloce e paziente generico
//        {
//            Report_siroky();
//        }
//    }
#endif

//    // scritta relativa al tipo di modalità dell'esame

#ifndef __NEW_IND_MOD__
#if PRI_REP_MODAL
    m_port->Pri_justif(F_center);
    m_port->Pri_mode(0x10);
    char str[60];
    if (m_modal_e == 2)
    {
        QString modal = tr("MANUAL   MODALITY");
        sprintf(str,"%s\n",modal.toLatin1().data());
    }
    else
        if (m_modal_e == 0)
        {
            QString modal = tr("AUTOMATIC  MODALITY ");
            sprintf(str,"%s\n",modal.toLatin1().data());
        }
    m_port->Pri_Str(strlen(str),str,0);
    QString line = LINE"\x1";
    m_port->Pri_Str(3,line.toLatin1().data(),0);
    m_port->Pri_justif(F_left);
#endif
#endif

//    // 	Risultati dell'esame ricavati dall'analisi semplificata, implementata nel firmware

#if PRI_REP_RIS
    Report_result();		// scrive in elenco i dati calcolati dall'analisi dell'esame
    m_port->Pri_Str(3,(char*)LINE"\x3",0);
#endif

    //	Scrive i dati riguardanti versione firmware e date/ora ultima calibrazione

#if PRI_REP_POS
       //m_port->Pri_justif(F_center);
       //Report_dati_macchina(numCurve);	// scrive data e ora della stampa e la versione attuale del FW
#endif

    // fa avanzare la carta per consentire lo strappo

#if PRI_REP_SPA
    m_port->Pri_forward(0xDA);
#endif

    m_port->Pri_Reset();	// resetta RAM della stampante: equivale ad un reset HW
}


/**
E' stampata l'intestazione del report, con intestazione clinica, logo and so on
*/
void printermanager::Intest()
{
    m_port->Pri_Str(3,(char*)LINE"\x1",0);  			// scrive una riga vuota
    m_port->Pri_justif(F_center);				// scrittura al centro
    m_port->Pri_Font(1);						// font 12x20
    m_port->Pri_mode(0x30);						// modo doppia dimensione non sottolienata

    char str[60];
    QString ditta_pers = tr("MENFIS BIOMEDICA");
    sprintf( str, "%s\n", ditta_pers.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,0);			// a capo con il "\n"

    m_port->Pri_mode(0x14);						// modo altezza doppia larghezza quadrupla
    QString logo = tr("PICO FLOW 2");
    sprintf( str, "%s", logo.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,1);			// a capo con il "\n"

    m_port->Pri_Font(1); 						// font 12x20
    m_port->Pri_mode(0x90);						// modo altezza doppia sottolineata
    QString msg_title = tr("Urodynamic Equipment");
    sprintf( str, "%s\n", msg_title.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,0);			// a capo con il "\n"



}

/**
Sono stampati i dati del report paziente, nome, cognome, data di nascita, sesso, data esame, ....
*/
void printermanager::Report_data()
{
    char str[60];

    m_port->Pri_mode(0x00); 					// modo default
    m_port->Pri_justif(F_left);					// tutto a sinistra
    m_port->Pri_Str(3,(char*)LINE"\x1",0);  			// scrive una riga vuota

    m_port->Pri_Font(1);							// font 12x20

    const char * puntini = ". . . . . . . . . . . . . . . . . . . . .\n";
    const char * space_bar = "                    ";

    QString strToWrite;
    strToWrite = tr("Test Number ....:");
    sprintf( str, " %s %s\n",strToWrite.toLatin1().data(),QString::number(m_numTest).toLatin1().data());

    m_port->Pri_Str(strlen(str),str,0);

    strToWrite = tr("Test Date ......:");
    sprintf(str," %s %s\n",strToWrite.toLatin1().data(), m_dateofexam.toLatin1().data());

    m_port->Pri_Str(strlen(str),str,0);

    // cognome
    strToWrite = tr("Surname ........:");
    if(m_test_type )
        sprintf( str, " %s %s", strToWrite.toLatin1().data(),puntini);
    else
        sprintf( str, " %s %s\n", strToWrite.toLatin1().data(), m_surname.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,0);

    // nome
    strToWrite = tr("Name ...........:");
    if(m_test_type)
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini);
    else
        sprintf( str, " %s %s\n", strToWrite.toLatin1().data(), m_name.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,0);

    // data di nascita stampata nei due formati: uno per ita e spa e uno per eng
    strToWrite = tr("Birth Date .....:");
    if(m_test_type)
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini);
    else
        sprintf(str," %s %s\n",strToWrite.toLatin1().data(), m_dateofbirth.toLatin1().data());

    m_port->Pri_Str(strlen(str),str,0);

    // sesso
    strToWrite = tr("Gender .........:");
    if(m_test_type){
        sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
        m_port->Pri_Str(strlen(str),str,0);
    }
    else {
        sprintf( str, " %s %c", strToWrite.toLatin1().data(), m_sex );
        m_port->Pri_Str(strlen(str),str,1);
    }

    // operatore
    strToWrite = tr("Investigator ...:");
    sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
    m_port->Pri_Str(strlen(str),str,0);

    // commenti
    strToWrite = tr("Comments .......:");
    sprintf( str, " %s %s", strToWrite.toLatin1().data(), puntini );
    m_port->Pri_Str( strlen( str ), str, 0);
    for (int i=0; i< 3; i++)	{	// tre righe vuote per eventuali commenti
        m_port->Pri_Str( 19, (char *)space_bar, 0 );
        sprintf( str, puntini );
        m_port->Pri_Str( strlen( str ), str, 0);
    }

#if PRI_REP_MODAL		// scritta relativa al tipo di modalità

    if(m_modal_e == 2 ){
        strToWrite = tr("MANUAL   MODALITY");
        sprintf( str, " %s\n", strToWrite.toLatin1().data());
    }
    else
        if(m_modal_e == 0 ){
            strToWrite = tr("AUTOMATIC  MODALITY ");
            sprintf( str, " %s\n", strToWrite.toLatin1().data() );
        }
    m_port->Pri_Str( strlen( str ), str, 0 );
#endif

    m_port->Pri_Intensity(0x80);		// intensità di default
}


/**
stampa del grafico di flusso/volume nella versione a grafici in portrait mode
*/
void printermanager::Report_flw()
{

    Calc_Max();	// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) così da individuare il giusto fondoscala

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);

    QString strToWrite = tr("Flowmetry   ");
    char str[90];
    sprintf( str, "   Q ( ml/s )              %s              Vol ( ml )", strToWrite.toLatin1().data());
    m_port->Pri_Str( strlen(str), str, 1 );
    for(int ub = 0; ub < 5; ub++ )                        /*scompone la griglia in 10 righe*/
    {
        Pri_Rep_Gra( ub * 2 ); // sì label
        m_port->waiting();
        Pri_Rep_Gra( ub * 2 + 1 ); // no label
        m_port->waiting();
    }

    m_port->Pri_mode(0x00);
    m_port->Pri_Font(1);
    m_port->Pri_Str( strlen( str_label_time[ m_i_max_x ] ), (char *)str_label_time[ m_i_max_x ], 1 );
    m_port->Pri_Str( 3, (char*)LINE, 0 );		// modifica per risparm carta e tempo: da riattivare
}

/**
Calcola "i_max_x" , "max_x" e "max_y"
*/
void printermanager::Calc_Max()
{
    m_max_x = m_durata;

    while (1){
        if (m_max_x< 30L){
            m_i_max_x=0;		m_max_x=30L;		break;
        }
        if (m_max_x< 45L){
            m_i_max_x=25;	m_max_x=45L;		break;
        }
        if (m_max_x< 60L){
            m_i_max_x=1;		m_max_x=60L;		break;
        }
        if (m_max_x< 75L){
            m_i_max_x=26;	m_max_x=75L;		break;
        }
        if (m_max_x<= 90L){
            m_i_max_x=2;		m_max_x=90L;		break;
        }
        if (m_max_x<= 105L){
            m_i_max_x=27;	m_max_x=105L;		break;
        }
        if (m_max_x<= 120L){
            m_i_max_x=3;		m_max_x=120L;		break;
        }
        if (m_max_x<= 135L){
            m_i_max_x=28;	m_max_x=135L;		break;
        }
        if (m_max_x<= 150L){
            m_i_max_x=4;		m_max_x=150L;		break;
        }
        if (m_max_x<= 180L){
            m_i_max_x=5;		m_max_x=180L;		break;
        }
        if (m_max_x<= 210L){
            m_i_max_x=6;		m_max_x=210L;		break;
        }
        if (m_max_x<= 240L){
            m_i_max_x=7;		m_max_x=240L;		break;
        }
        if (m_max_x<= 270L){
            m_i_max_x=8;		m_max_x=270L;		break;
        }
        if (m_max_x<= 300L) {
            m_i_max_x=9;		m_max_x=300L;		break;
        }
        if (m_max_x<= 360L){	// 6 minuti
            m_i_max_x=10;		m_max_x=360L;		break;
        }
        if (m_max_x<= 420L){  // 7 minuti
            m_i_max_x=11;		m_max_x=420L;		break;
        }
        if (m_max_x<= 480L){	// 8 min
            m_i_max_x=12;		m_max_x=480L;		break;
        }
        if (m_max_x<= 540L){	// 9 min
            m_i_max_x=13;		m_max_x=540L;		break;
        }
        if (m_max_x<= 600L){	// 10 min
            m_i_max_x=14;		m_max_x=600L;		break;
        }
        if (m_max_x<= 660L){	// 11 min
            m_i_max_x=15;		m_max_x=660L;		break;
        }
        if (m_max_x<= 720L){	// 12 min
            m_i_max_x=16;		m_max_x=720L;		break;
        }
        if (m_max_x<= 780L){	// 13 min
            m_i_max_x=17;		m_max_x=780L;		break;
        }
        /*
      if (m_max_x<= 840L){	// 14 min
        m_i_max_x=18;		m_max_x=840L;		break;
      }
      */
        if (m_max_x<= 900L){	// 15 min
            m_i_max_x=19;		m_max_x=900L;		break;
        }
        if (m_max_x<= 960L){	//16 min
            m_i_max_x=20;		m_max_x=960L;		break;
        }
        /*
      if (m_max_x<= 1020L){	// 17 min
        m_i_max_x=21;		m_max_x=1020L;	break;
      }
      */
        if (m_max_x<= 1080L){	// 18 min
            m_i_max_x=22;		m_max_x=1080L;	break;
        }
        /*
      if (m_max_x<= 1140L){	// 19 min
        m_i_max_x=23;		m_max_x=1140L;	break;
      }
      */
        if (m_max_x<= 1200L){	// 20 min
            m_i_max_x=24;		m_max_x=1240L;	break;
        }
    }

    m_max_y=((m_flu_max/10)+1)*10;			// trovo la decina minima superiore al valore max

    while (1){
        if (m_max_y <= 10L){
            m_max_y=10L;		break;
        }
        if (m_max_y <= 15L){
            m_max_y=15L;		break;
        }
        if (m_max_y <= 20L){
            m_max_y=20L;		break;
        }
        if (m_max_y <= 25L){
            m_max_y=25L;		break;
        }
        if (m_max_y <= 30L){
            m_max_y=30L;		break;
        }
        if (m_max_y <= 40L){
            m_max_y=40L;		break;
        }
        if (m_max_y <= 50L){
            m_max_y=50L;		break;
        }
        if (m_max_y <= 65L){
            m_max_y=65L;		break;
        }
        if (m_max_y <= 80L){
            m_max_y=80L;		break;
        }
        if (m_max_y <= 100L){
            m_max_y=100L;	break;
        }
        if (m_max_y <= 120L){
            m_max_y=120L;	break;
        }
        m_max_y=150L;
        break;
    }

    m_max_y_gr2 = ( (long)( ( m_max_vol + 10 ) / 100 ) + 1 ) * 100;// 05 dicembre
}


/**
GRAFICI IN PORTRAIT MODE - modo tradizionale
funzione chiamata ciclicamente per stampare le 10 righe in cui è suddiviso il grafico del flusso/volume
a seconda del parametro num_rig si stampano o meno i label degli assi di flusso e volume
*/
void printermanager::Pri_Rep_Gra(int __num_riga)
{
    static bool label = false;
    unsigned short uw8;
    short int max_y_flw, max_x_vol;
    short int first_char, second_char, third_char, fourth_char;

    for(int i=0; i<(LCMD + a_flw); i++ )
        m_str_gr[ i ] = 0x00;
    // setta la stampa grafica
    m_str_gr[0]=ESC;	//0x1B;	// ESC
    m_str_gr[1]='*';	//0x2A;	// *
    m_str_gr[2]=0x90;	// n1	num_byte = 96 + 2256 + 96 = 2448 = ( 65536 * n3 ) + ( 256 * n2 ) + n1
    m_str_gr[3]=0x09;	// n2
    m_str_gr[4]=0x00;	// n3
    m_str_gr[5]=0x02;	// n4	doppia altezza
    m_str_gr[6]=0x02;	// n5	scrive a 1 byte dal bordo
    m_str_gr[7]=0x66;	// n6	larghezza 4+94+4=102 byte

    if( (__num_riga == 0) || ( !(__num_riga % 2) ) )
        label = true;  // scrive i label solo nelle righe 0, 2, 4, 6, 8
    else
        label = false; // nelle righe 1, 3, 5, 7 metter caratteri vuoti

    if( label )		// se label è true devo scivere i valori sugli assi di flw e vol, altrimenti riempio con caratterei vuoti
    {
        // la prima riga del grafico si compone di 24 righe di 94byte ciascuna alle quali antepongo un carattere vuoto, le 24 righe di 3 byte del dato di flw
        // i 3 byte aggiuntivi sono per le tre cifre di dato
        // if( flusso > 99 ) allora 3 cifre quindi 3 byte dal CHARSET
        max_y_flw = (int)(m_max_y - (__num_riga/2)*( m_max_y / 5 ));
        if( max_y_flw > 99L )
        {
            // sono 3 i caratteri da 1Bx13 da inserire
            // la terza cifra (primo carattere)
            first_char = max_y_flw / 100;
            for(int i=0; i<13; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLS-3)*24 + i ] = (char)print7x13Set[ first_char*13 + i];
            }
            // seconda cifra (secondo carattere 1)
            second_char = ( max_y_flw - ( first_char * 100 ) ) / 10;
            for(int i=0; i<13; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLS-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
            }
            // prima cifra (terzo carattere 2)
            third_char = ( max_y_flw - ( first_char * 100 ) - ( second_char * 10 ) );
            for(int i=0; i<13; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLS-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
        }
        else {
            if( max_y_flw > 9 ) {
                // seconda cifra (secondo carattere 1)
                second_char = ( max_y_flw) / 10;
                for(int i=0; i<13; i++)	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLS-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
                }
                // prima cifra (terzo carattere 2)
                third_char = max_y_flw - ( second_char * 10 );
                for(int i=0; i<13; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLS-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                }
            }
            else {		// 2 byte di spazio, 1 di valore
                for(int  i=0; i<24; i++ )	// spazio (secondo carattere)
                    m_str_gr[ LCMD + (CLS-2)*24 + i] = (char)0x00;
                // prima cifra (terzo carattere 2)
                third_char = max_y_flw /* - ( second_char * 10 )*/;
                for(int i=0; i<13; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLS-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                }
                for(int i=13; i < 24; i++ )	{
                    m_str_gr[ LCMD + (CLS-1)*24 + i ] = (char)0x00;
                }
            }
        }
    }
#ifdef __ZERO_ASSE_FLUSSO__
    else {		// niente label m caratteri vuoti
        if( __num_riga == 9 )// se è l'ultima riga sovrascrivo lo "0" al terzo carattere
            for(int i=0; i<13; i++ ) {	// spazio
                m_str_gr[ LCMD + (CLS-1)*24 + (i+10) ] = (char)print7x13Set[ 0*13 + i];
            }
    }
#endif

    // costruzione assi grafico: quattro sono i caratteri anteposti senza byte dei comandi
    Pri_Rep_Gra_Ini_Grid(__num_riga); /*crea griglia e inizializa m_m_str_gr*/

    // inserimento pixel curve
    m_str_gr[ pos_gra + 2255 ] = (char)0xFF;

#ifdef __DATI_GRAF_FLUSSO__
    /* ********************     Grafico    ************************* */
    if (m_tem_svu ){

        int x_pt	=	0;
        int flow_pt	= 240;
        int vol_pt	= 240;
        int uw5		= m_PointsToPrintout-1;		// correzione per evitare l'andata azero // 23-03
        int i4		= __num_riga*24;

        int graf_g_x	=(10*m_max_x);			//asse dei tempi in funzione della durata dell'esame
        int graf_g_fl	=(m_max_y*10);
        int graf_g_vl	=(m_max_y_gr2);

        for (int uw6 = 1; uw6 < uw5; uw6++ ){	     /*trovo estremi della retta passante per il campione uw6 uw6+1*/
            int x_pt_prec = x_pt;                    /*94*8=752 punti x  i2=x1 della retta*/
            m_x1 = x_pt;

            if (m_correct){
                x_pt = (int)((m_num_sam * uw6) / graf_g_x); // correzione nel caso di esami + lunghi di 752 campioni
            }
            else {
                x_pt =(int)( ( 752L*uw6 ) / graf_g_x );             /*x_pt=x2 retta */
            }
            m_y1 = flow_pt;

            flow_pt = 239L - (int)(( 239L*buffer_flw[ uw6 ] ) / ( graf_g_fl )); /*y2*/

            if (flow_pt > 239)   // flow_pt = _y1;
                while (flow_pt <= 239)	{
                    uw8 = uw6-1;
                    flow_pt	= 239L-(int)((239L*buffer_flw[uw8])/(graf_g_fl));//239; modifica 31-03
                }
            else if (flow_pt < 0)
                flow_pt	= 0;
            /*ogni riga composta da 1 linee di stampa m_str_gr*/
            m_x2 = x_pt;
            m_y2 = flow_pt;
            if( Int_Pun(i4 ) )
            {
                if (m_num_xy > 25)
                    //Fatal_Error(FATAL_ERROR_PRI);
                    m_num_xy = 25;
                for(int i22=0; i22<m_num_xy; i22++){
                    m_y[i22] -= i4;
                    int i33 = 24 * ( m_x[i22] / 8 ) + m_y[i22];
                    unsigned char ub3 = 0x80 >> (byte)(m_x[i22] % 8);
                    int i6 = i33 / 24;
                    int i7 = i33 % 24;
                    m_str_gr[ pos_gra + 24 * i6 + i7] |= (char)ub3;
                }
            }
            m_y1 = vol_pt;

            vol_pt = 239L - (int)(( 239L * buffer_vol[uw6] ) / ( graf_g_vl ));
            if( vol_pt > 239 )
                //vol_pt = 239;
                vol_pt = m_y1; // se campione sfarlocco, scrve quello precedente

            m_x1 = x_pt_prec;
            m_x2 = x_pt;
            m_y2 = vol_pt;
            if( Int_Pun(i4 ) ) {
                if (m_num_xy > 25)
                    m_num_xy = 25;
                for(int i22 = 0; i22 < m_num_xy; i22++){
                    m_y[i22] -= i4;
                    int i33 = 24 * ( m_x[i22] / 8 ) + m_y[i22];
                    unsigned char ub3 = 0x80 >> (byte)(m_x[i22] % 8);
                    int i6 = i33 / 24;
                    int i7 = i33 % 24;
                    m_str_gr[ pos_gra + 24*i6 + i7] |= (char)ub3;
                }
            }
        }
    }
#endif
    // inserimento label volume
    if( label ) {
        max_x_vol = (int)(m_max_y_gr2 - (__num_riga/2)*( m_max_y_gr2 / 5 ));
        if( max_x_vol > 999 ){
            // inserimento cifre volume 4 caratteri dalla posizone xxx
            first_char = max_x_vol / 1000;	// cifra delle migliaia
            second_char = ( max_x_vol - ( first_char*1000 ) ) / 100; // cifra delle centinaia
            third_char =  ( max_x_vol - ( first_char*1000 ) - ( second_char*100 ) ) / 10; // cifra delle decine
            fourth_char = ( max_x_vol - ( first_char*1000 ) - ( second_char*100 ) - ( third_char*10 ) ); // cifra delle unità
            //for( j = 0; j < p; j++ )	{
            for(int i = 0; i < 13; i++ )
                m_str_gr[ pos_lab_vol + (CLD-4)*24 + i ] = (char)print7x13Set[ first_char*13 + i];
            for(int i = 13; i < 24; i++)
                m_str_gr[ pos_lab_vol + (CLD-4)*24 + i ] = (char)0x00;

            for(int i = 0; i < 13; i++ )
                m_str_gr[ pos_lab_vol + (CLD-3)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
            for(int i = 13; i < 24; i++)
                m_str_gr[ pos_lab_vol + (CLD-3)*24 + i ] = (char)0x00;

            for(int i = 0; i < 13; i++ )
                m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            for(int i = 13; i < 24; i++)
                m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)0x00;

            for(int i = 0; i < 13; i++ )
                m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)print7x13Set[ fourth_char*13 + i];
            for(int i = 13; i < 24; i++)
                m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)0x00;
        }
        else {
            for(int i = 0; i < 24; i++ )	// spazio
                m_str_gr[ pos_lab_vol + (CLD-4)*24 + i] = (char)0x00;
            if( max_x_vol > 99 ) {
                // inserimento cifre volume 3 caratteri
                second_char = ( max_x_vol /*- ( first_char*1000 )*/) / 100; // cifra delle centinaia
                for(int i = 0; i < 13; i++ )
                    m_str_gr[ pos_lab_vol + (CLD-3)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
                for(int i = 13; i < 24; i++)
                    m_str_gr[ pos_lab_vol + (CLD-3)*24 + i ] = (char)0x00;

                third_char =  ( max_x_vol /*- ( first_char*1000 ) */- ( second_char*100 ) ) / 10; // cifra delle decine
                for(int i = 0; i < 13; i++ )
                    m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                for(int i = 13; i < 24; i++)
                    m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)0x00;

                fourth_char = ( max_x_vol /*- ( first_char*1000 ) */- ( second_char*100 ) - ( third_char*10 ) ); // cifra delle unità
                for(int i = 0; i < 13; i++ )
                    m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)print7x13Set[ fourth_char*13 + i];
                for(int i = 13; i < 24; i++)
                    m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)0x00;
            }
            else {
                for(int i = 0; i < 24; i++ )	// spazio
                    m_str_gr[ pos_lab_vol + (CLD-3)*24 + i] = (char)0x00;
                if( max_x_vol > 9 ) {
                    // inserimento cifre volume 2 caratteri
                    third_char =  ( max_x_vol /*- ( first_char*1000 ) - ( second_char*100 ) */) / 10; // cifra delle decine
                    for(int i = 0; i < 13; i++ )
                        m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                    for(int i = 13; i < 24; i++)
                        m_str_gr[ pos_lab_vol + (CLD-2)*24 + i ] = (char)0x00;

                    fourth_char = ( max_x_vol /*- ( first_char*1000 ) - ( second_char*100 ) */- ( third_char*10 ) ); // cifra delle unità
                    for(int i = 0; i < 13; i++ )
                        m_str_gr[  pos_lab_vol + (CLD-1)*24 + i ] = (char)print7x13Set[ fourth_char*13 + i];
                    for(int i = 13; i < 24; i++)
                        m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)0x00;
                }
                else {
                    for(int i = 0; i < 24; i++ )	// spazio
                        m_str_gr[ pos_lab_vol + (CLD-2)*24 + i] = (char)0x00;
                    // inserimento cifre volume 1 carattere
                    fourth_char = ( max_x_vol /*- ( first_char*1000 ) - ( second_char*100 ) x- ( third_char*10 ) */); // cifra delle unità
                    for(int i = 0; i < 13; i++ )
                        m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)print7x13Set[ fourth_char*13 + i];
                    for(int i = 13; i < 24; i++)
                        m_str_gr[ pos_lab_vol + (CLD-1)*24 + i ] = (char)0x00;
                }
            }
        }
    }
    // trasposizione
    Str_Trasposta(0, CLS, CLD); // fa la trasposta di m_str_gr che è costruita per colonne mentre noi si stampa per righe
    // finalmente stampa
    m_port->Pri_Str( (LCMD + CLS*24 + m_uw3 + CLD*24), (char *)m_str_gr, 0);
}

void printermanager::Pri_Rep_Gra_Ini_Grid(int __n_riga)
{
    m_uw3 = 24*94;	// dimensioni in byte della riga n-esima dell'area grafico

    // azzeriamo tutto lo stringone
    for(int uw4 = 0; uw4 < m_uw3; uw4++)
        m_str_gr[ pos_gra + uw4 ] = 0x00;

    // ognuna delle 10 fascie è costruita per 94 colonne da 24 punti, dove da str_gr[0] a str_gr[23] è la prima colonna e così via
    for(int uw4 = 0; uw4 < 24; uw4++)	{
        m_str_gr[ pos_gra + uw4 ] = (char)0x80;  /*     Bordo Verticale Sinistro   */
        m_str_gr[ pos_gra + m_uw3 - 24 + uw4 ] = (char)0x01;   /*    Bordo Verticale Destro   */
    }
    /*    Bordo Superiore   */
    if( __n_riga == 0)	{
        for(int uw4 = 0; uw4 < m_uw3; uw4+=24 )
            m_str_gr[ pos_gra + uw4 ] = (char)0xFF;
    }
    /*     Bordo Inferiore   */
    if( __n_riga == 9 )	{
        for(int uw4 = 23; uw4 < m_uw3; uw4+=24 )
            m_str_gr[ pos_gra + uw4 ] = (char)0xFF;
    }
    /*      Griglia Verticale   */
    for(int i = 1; i < 10; i++)		{
        int i2 = (int)((( 94 * 8 ) * i + 5) / 10);
        for(int i3 = 0; i3 < 24; i3+=4){
            int i4 = 24 * (i2 / 8) + i3;
            unsigned char ub3 = 0x80 >> (byte)( i2 % 8 );// 10000000 >> byte(752%8)
            m_str_gr[ pos_gra + i4 ] |= (char)ub3;
        }
    }
    /*      Griglia Orizzontale   */
    if( __n_riga % 2 == 0 )		{
        for(int uw4 = 0; uw4 < m_uw3; uw4+= 24 )
            m_str_gr[ pos_gra + uw4] |= (char)0x10;
    }
}  // fine Pri_Rep_Gra_Ini_Grid



byte printermanager::Int_Pun(int __i4){
    /*
        Interpola i Punti ("x1","_y1") e ("x2","y2") rispetto alla Banda
      di Ordinate "i4" e "i4"+48.
      Ritorna :
         -   1  :  Se la Banda     e' Interessata dai Punti
         -   0  :  Se la Banda non e' Interessata dai Punti
   */
    /*  Entrambi i Punti Dentro   */
    if ((m_y1 >= __i4) && (m_y1 < (__i4+24)) && (m_y2 >= __i4) && (m_y2 < (__i4+24))){
        Gra_Line();
        return(1);
    }
    /* Entrambi i Punti Fuori dalla Stessa Parte   */
    if (((m_y1 < __i4) && (m_y2 < __i4)) || ((m_y1 >= (__i4+24)) && (m_y2 >= (__i4+24))))
        return(0);

    if (m_y1 < __i4){
        /* Primo Punto Sotto  */
        if (m_y2 < (__i4+24)){
            /*  Secondo Punto Dentro */
            m_x1=m_x1+((m_x2-m_x1)*(__i4-m_y1))/(m_y2-m_y1);
            m_y1=__i4;
        }else{
            /*  Secondo Punto Sopra  */
            int i6 =(m_x2-m_x1);
            int i7 =(m_y2-m_y1);
            m_x1 =m_x1 +(i6*(__i4-m_y1))/i7;
            m_x2 =m_x2+(i6*(__i4+23-m_y2))/i7;
            m_y1 =__i4;
            m_y2=__i4+23;
        }
        Gra_Line();
        return(1);
    }
    if (m_y1 < (__i4+24)){
        /* Primo Punto Dentro */
        if (m_y2 < __i4){
            /*   Secondo Punto Sotto  */
            m_x2=m_x2+(m_x1-m_x2)*(__i4-m_y2)/(m_y1-m_y2);
            m_y2=__i4;
        }else{
            /*  Secondo Punto Sopra  */
            m_x2=m_x2+(m_x1-m_x2)*(__i4+23-m_y2)/(m_y1-m_y2);
            m_y2=__i4+23;
        }
        Gra_Line();
        return(1);
    }
    /*   Primo Punto Sopra   */
    if (m_y2 < __i4){
        /*   Secondo Punto Sotto   */
        int i6 =(m_x2-m_x1);
        int i7 =(m_y2-m_y1);
        m_x2=m_x2+(i6*(__i4-m_y2))/i7;
        m_x1 =m_x1 +(i6*(__i4+23-m_y1))/i7;
        m_y2=__i4;
        m_y1 =__i4+23;
    }else{
        /*     Secondo Punto Dentro   */
        m_x1=m_x1+(m_x2-m_x1)*(__i4+23-m_y1)/(m_y2-m_y1);
        m_y1=__i4+23;
    }
    Gra_Line();
    return(1);
}

void printermanager::Gra_Line(){
    /*
      Disegna il Segmento definito da ["x1","_y1"] - ["x2","y2"]
   */
    int   				xmin,xmax,ymin,ymax;
    if ((m_x2 == m_x1) && (m_y2 == m_y1)){
        m_num_xy=1;
        m_x[0]=m_x1;
        m_y[0]=m_y1;
        return;
    }
    m_num_xy=0;
    unsigned char flag_incr=0;
    if (m_y2 != m_y1){
        if ((abs(m_x2-m_x1)/abs(m_y2-m_y1)) < 1)
            flag_incr=1;
    }
    if (flag_incr == 0){
        if (m_x2 > m_x1){
            xmin=m_x1;ymin=m_y1;
            xmax=m_x2;ymax=m_y2;
        }else{
            xmin=m_x2;ymin=m_y2;
            xmax=m_x1;ymax=m_y1;
        }
        for (int i5=xmin; i5<=xmax; i5++){
            int i6=(i5-xmin)*(ymax-ymin);
            i6 /= (xmax-xmin);
            int i7=ymin+i6;
            m_x[m_num_xy]=i5;
            m_y[m_num_xy]=i7;
            m_num_xy++;
        }
    }else{
        if (m_y2 > m_y1){
            ymin=m_y1;xmin=m_x1;
            ymax=m_y2;xmax=m_x2;
        }else{
            ymin=m_y2;xmin=m_x2;
            ymax=m_y1;xmax=m_x1;
        }
        for (int i5=ymin; i5<=ymax; i5++){
            int i6=(i5-ymin)*(xmax-xmin);
            i6 /= (ymax-ymin);
            int i7=xmin+i6;
            m_x[m_num_xy]=i7;
            m_y[m_num_xy]=i5;
            m_num_xy++;
        }
    }
}

// crea la trasposta della stringa str_gr, ottenendo una matrice scritta per righe
void printermanager::Str_Trasposta(byte __type, unsigned short __sx_byte, unsigned short __dx_byte)
{
    static short num_byte, num_col, num_rig;
    static int j;

    if( __type == 0 )				// trasposizione della stringa del grafico di flusso o emg
        num_byte = __sx_byte + 94 + __dx_byte;	// numero di byte della riga orrizzontale del grafico comprensivo dei label
    else if( __type == 1 )		// trasposizione della stringa dei grafici di siroky
        num_byte = __sx_byte + w_ave + w_SD + __sx_byte + w_max + w_SD;

    //static char  str4[ n + 24*(sx_byte+dx_byte+94) ];

    for( j = 0; j < ( num_byte*24 ); j++ )
        m_str_tr[j] = 0x00;	// azzero tutta questa stringona
    for( num_col = 0; num_col < 24; num_col++ )
    {
        for( num_rig = 0; num_rig < num_byte; num_rig++)
        {
            m_str_tr[ ( num_byte * num_col ) + num_rig ] = m_str_gr[ LCMD + ( 24 * num_rig ) + num_col ];
        }
    }
    for( j = 0; j < ( num_byte*24 ); j++ )
        m_str_gr[ LCMD + j ] = m_str_tr[ j ];
}


/**
stampa del grafico di EMG nella versione a grafici in portrait mode
*/
void printermanager::Report_emg()
{
    Calc_Max_EMG();

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);
    char str[60];
    QString emg_title = tr(" EMG Diagram ");
    sprintf(str,"   EMG ( uV )              %s\n",emg_title.toLatin1().data());
    m_port->Pri_Str( strlen( str ), str, 0 );
    for(int ub = 0; ub < 5; ub++)          /*scompone la griglia in 5 righe*/
    {
        Pri_Rep_Gra_EMG( ub * 2 );
        Pri_Rep_Gra_EMG( (ub * 2) + 1 );
    }
    m_port->Pri_mode(0);
    m_port->Pri_Font(1);
    m_port->Pri_Str( strlen( str_label_time[ m_i_max_x ] ),(char *)str_label_time[ m_i_max_x ], 1 );
    m_port->Pri_Str( 3, (char*)LINE"\x1", 0 );// spazio singolo
}


void printermanager::Calc_Max_EMG()
{
    while (1){
        if (m_max_emg <= 50L){
            m_max_y=50L;
            break;
        }
        if (m_max_emg <= 100L){
            m_max_y=100L;		break;
        }
        if (m_max_emg <= 150L){
            m_max_y=150L;		break;
        }
        if (m_max_emg <= 200L){
            m_max_y=200L;		break;
        }
        if (m_max_emg <= 250L){
            m_max_y=250L;		break;
        }
        if (m_max_emg <= 300L){
            m_max_y=300L;		break;
        }
        if (m_max_emg <= 350L){
            m_max_y=350L;		break;
        }
        if (m_max_emg <= 400L){
            m_max_y=400L;		break;
        }
        if (m_max_emg <= 450L){
            m_max_y=450L;		break;
        }
        if (m_max_emg <= 500L){
            m_max_y=500L;	break;
        }
        if (m_max_emg <= 550L){
            m_max_y=550L;	break;
        }
        if (m_max_emg <= 600L){
            m_max_y=600L;	break;
        }
        if (m_max_emg <= 650L){
            m_max_y=650L;	break;
        }
        if (m_max_emg <= 700L){
            m_max_y=700L;	break;
        }
        if (m_max_emg <= 750L){
            m_max_y=750L;	break;
        }
        if (m_max_emg <= 800L){
            m_max_y=800L;	break;
        }
        if (m_max_emg <= 850L){
            m_max_y=850L;	break;
        }
        if (m_max_emg <= 900L){
            m_max_y=900L;	break;
        }
        if (m_max_emg <= 950L){
            m_max_y=950L;	break;
        }
        if (m_max_emg <= 1000L){
            m_max_y=1000L;	break;
        }
        if (m_max_emg <= 1100L){
            m_max_y=1100L;	break;
        }
        if (m_max_emg <= 1200L){
            m_max_y=1200L;	break;
        }
        if (m_max_emg <= 1300L){
            m_max_y=1300L;	break;
        }
        if (m_max_emg <= 1400L){
            m_max_y=1400L;	break;
        }
        if (m_max_emg <= 1500L){
            m_max_y=1500L;	break;
        }
        if (m_max_emg <= 1600L){
            m_max_y=1600L;	break;
        }
        if (m_max_emg <= 1700L){
            m_max_y=1700L;	break;
        }
        if (m_max_emg <= 1800L){
            m_max_y=1800L;	break;
        }
        if (m_max_emg <= 1900L){
            m_max_y=1900L;	break;
        }
        if (m_max_emg <= 2000L){
            m_max_y=2000L;	break;
        }
        if (m_max_emg <= 2250L){
            m_max_y=2250L;	break;
        }
        if (m_max_emg <= 2500L){
            m_max_y=2500L;	break;
        }
        if (m_max_emg <= 2750L){
            m_max_y=2750L;	break;
        }
        if (m_max_emg <= 3000L){
            m_max_y=3000L;	break;
        }
        m_max_y=3250L;
        break;
    }
}


void printermanager::Pri_Rep_Gra_EMG(byte __num_riga)
{
    static bool label = false;
    static short int max_emg;
    static short int first_char, second_char, third_char, fourth_char;

    for(int i = 0; i < ( LCMD + a_emg); i++ )
        m_str_gr[ i ] = 0x00;

    m_str_gr[0]=ESC;	//0x1B;	// ESC
    m_str_gr[1]='*';	//0x2A;	// *
    m_str_gr[2]=0x90;	// n1	num_byte = 96 + 2256 = 2328 = ( 65536 * n3 ) + ( 256 * n2 ) + n1 // larghezza fissa tipo grafico flusso
    m_str_gr[3]=0x09;	// n2
    m_str_gr[4]=0x00;	// n3
    m_str_gr[5]=0x02;	// n4	doppia altezza
    // con label di 4 cifre
    m_str_gr[6]=0x02;	// n5	scrive a n5 byte dal bordo
    m_str_gr[7]=0x66;	// n6	larghezza 4+4+94=102 byte

    if( (__num_riga == 0) || ( !(__num_riga % 2) ) )
        label = true;  // scrive i label solo nelle righe 0, 2, 4, 6, 8
    else
        label = false; // nelle righe 1, 3, 5, 7 metter caratteri vuoti

    if( label )	// se label è true devo scivere i valori sull'asse di emg, altrimenti riempio con m caratterei vuoti
    {
        // la prima riga del grafico si compone di 24 righe di 94byte ciascuna alle quali antepongo le 24 righe di m byte del dato di emg
        // i m byte aggiuntivi sono per le quattre cifre di dato
        // if( emg > 99 ) allora 3 cifre quindi 3 byte dal CHARSET

        max_emg = (int)(  m_max_y - (__num_riga / 2 )*( m_max_y / 5 ));	// valori in unità
        if( max_emg > 999L )
        {
            // sono m i caratteri da 1Bx13 da inserire
            // la quarta cifra (primo carattere)
            first_char = max_emg / 1000;
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle migliaia per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLD-4)*24 + i ] = (char)print7x13Set[ first_char*13 + i];
            }
            for(int i = 13; i < 24; i++ )	{
                m_str_gr[ LCMD + (CLD-4)*24 + i ] = (char)0x00;
            }
            // la terza cifra (primo carattere)
            second_char = ( max_emg - ( first_char * 1000)) / 100;
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLD-3)*24 + i ] = (char)print7x13Set[ second_char *13 + i];
            }
            for(int i = 13; i < 24; i++ )	{
                m_str_gr[ LCMD + (CLD-3)*24 + i ] = (char)0x00;
            }
            // seconda cifra (secondo carattere 1)
            third_char = ( max_emg - ( first_char * 1000 ) - ( second_char * 100 ) ) / 10;
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
            for(int i = 13; i < 24; i++ )	{
                m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)0x00;
            }
            // prima cifra (terzo carattere 2)
            fourth_char = max_emg - ( first_char * 1000 ) - ( second_char * 100 ) - ( third_char * 10 );
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)print7x13Set[ fourth_char*13 + i];
            }
            for(int i = 13; i < 24; i++ )	{
                m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)0x00;
            }
        }
        else
        {
            for(int i = 0; i < 24; i++ )	// spazio (primo carattere)
                m_str_gr[ LCMD + (CLD-4)*24 + i] = (char)0x00;
            if( max_emg > 99L )
            {
                // sono m i caratteri da 1Bx13 da inserire
                // la terza cifra (primo carattere)
                first_char = max_emg / 100;
                for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLD-3)*24 + i ] = (char)print7x13Set[ first_char*13 + i];
                }
                for(int i = 13; i < 24; i++ )	{
                    m_str_gr[ LCMD + (CLD-3)*24 + i ] = (char)0x00;
                }
                // seconda cifra (secondo carattere 1)
                second_char = ( max_emg - ( first_char * 100 ) ) / 10;
                for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
                }
                for(int i = 13; i < 24; i++ )	{
                    m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)0x00;
                }
                // prima cifra (terzo carattere 2)
                third_char = max_emg - ( first_char * 100 ) - ( second_char * 10 );
                for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                    m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                }
                for(int i = 13; i < 24; i++ )	{
                    m_str_gr[LCMD + (CLD-1)*24 + i ] = (char)0x00;
                }
            }
            // else if( FC > 9) allora 2 byte di spazio, 2 di valore
            else
            {
                for(int i = 0; i < 24; i++ )	// spazio (primo carattere)
                    m_str_gr[ LCMD + (CLD-3)*24 + i] = (char)0x00;
                if( max_emg > 9 )
                {
                    // seconda cifra (secondo carattere 1)
                    second_char = ( max_emg/* - 100 */) / 10;
                    for(int i = 0; i < 13; i++ )	{// scrivo il carattere "1" delle centinaia per colonne come 1Bx13righe
                        m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
                    }
                    for(int i = 13; i < 24; i++ )	{
                        m_str_gr[ LCMD + (CLD-2)*24 + i ] = (char)0x00;
                    }
                    // prima cifra (terzo carattere 2)
                    third_char = max_emg - ( second_char * 10 );
                    for(int i = 0; i < 13; i++ )	{// scrivo il carattere "1" delle centinaia per colonne come 1Bx13righe
                        m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                    }
                    for(int i = 13; i < 24; i++ )	{
                        m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)0x00;
                    }
                }
                // else 2 byte di spazio, 1 di valore
                else
                {
                    for(int i = 0; i < 24; i++ )	// spazio (secondo carattere)
                        m_str_gr[ LCMD + (CLD-2)*24 + i] = (char)0x00;
                    // prima cifra (terzo carattere 2)
                    third_char = max_emg /* - ( second_char * 10 )*/;
                    for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                        m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
                    }
                    for(int i = 13; i < 24; i++ )	{
                        m_str_gr[ LCMD + (CLD-1)*24 + i ] = (char)0x00;
                    }
                }
            }
        }
    }
#ifdef __ZERO_ASSE_EMG__
    else {// niente label m caratteri vuoti
        if( __num_riga == 9 )// se è l'ultima riga sovrascrivo lo "0" al terzo carattere
            for(int i = 0; i < 13; i++ )	// spazio
                m_str_gr[ LCMD + (CLD-1)*24 + (i+10) ] = (char)print7x13Set[ 0*13 + i];
    }
#endif

    // costruzione assi grafico
    Pri_Rep_Gra_Ini_Grid(__num_riga); /*crea griglia e inizializa str_gr*/

    // inserimento pixel curve
    m_str_gr[ pos_gra + 2255 ] = (char)0xFF;

#ifdef __DATI_GRAF_EMG__
    /*      Grafico   */
    if( m_tem_svu )
    {
        int x_pt = 0;
        int flow_pt = 240;                  /*dovrei cambare le var *_fl con *_emg ma non vale la pena ridefinire altre variabili*/
        int uw5 = m_PointsToPrintout - 1;// correione per evitare l'andata azero //23-03

        int i4 = __num_riga * 24;
        int graf_g_x = ( 10 * m_max_x ); // disperato
        int graf_g_fl = ( m_max_y );

        for (int uw6 = 1; uw6 < uw5; uw6++ ) 	     /*trovo estremi della retta passante per il campione uw6 uw6+1*/
        {
            m_x1 = x_pt;
            if( m_correct )	{
                x_pt = (int)( m_num_sam * uw6 / graf_g_x); // correzione nel caso di esami + lunghi di 752 campioni
            } else {
                x_pt =(int)(( 752L * uw6 ) / graf_g_x);                     /*x_pt=x2 retta */
            }
            m_y1 = flow_pt;

            flow_pt = 239L - (int)( ( 239L * buffer_emg[ uw6 ] ) / ( graf_g_fl ) ); /*y2*/
            if( flow_pt > 239 )
                flow_pt = 239;
            /*ogni riga composta da 1 linee di stampa str_gr*/
            m_x2 = x_pt;
            m_y2 = flow_pt;
            if(Int_Pun(i4))
            {
                if (m_num_xy > 25)
                    //Fatal_Error(FATAL_ERROR_PRI);
                    m_num_xy = 25;
                for(int i22 = 0; i22 < m_num_xy; i22++)
                {
                    m_y[i22] -= i4;
                    int i33 = 24 * ( m_x[i22] / 8 ) + m_y[i22];
                    unsigned char ub3 = 0x80 >> (byte)(m_x[i22] % 8);
                    int i6 = i33 / 24;
                    int i7 = i33 % 24;
                    m_str_gr[ pos_gra + 24*i6 + i7] |= (char)ub3;
                }
            }
        }
    }
#endif
    // spazi vuoti dove invece nel grafic del flw ci andavano i label del vol
    // niente label quattro caratteri vuoti dalla posizione xxx

    Str_Trasposta( 0, CLS, CLD ); // fa la trasposta di str_gr che è costruita per colonne mentre noi si stampa per righe

    m_port->Pri_Str( ( LCMD + a_emg ), (char *)m_str_gr, 0 );
}


/**
Nella stampa del report tipo reale, individuo il fondoscala del flusso, parametro che serve per adattare i valori degli array dati volume e emg (e anche flusso)
*/
long printermanager::Calc_Max_Flw()
{
    long flw_max = 0;
    flw_max = (long)m_flu_max;
    flw_max = ((flw_max/10)+1)*10;	// trovo la decina minima superiore al valore max
    // FS del grafico del flusso	// può assumere i valori 25, 50, 75, 100
    if(flw_max <= 25)
        flw_max = 25;
    else if(flw_max <= 50)
        flw_max = 50;
    else if(flw_max <= 75)
        flw_max = 75;
    else //if(flw_max <= 160)
        flw_max = 100;
    return flw_max;	// mi servirà per la grafica zione dei dati
}


/**
In stampa devo avere una distribuzione dell'asse del tempo dipendente dalla scala della curva del flusso1sec/mm; ho 8 punti/mm e per ogni risoluzione dipendente
dal valore di FS del flusso, dovrò adattare, dal caso minore di 5 sample ogni 8punti, fino al 20sample ogni 8 punti.
Il paramentro num_sample è il valore assoluto di dati reali acqusiti, e dividendo per 10 questo valore ho i secondi effettivi di acquisizione.
ritorna il numero di punti da stampare, ricavati dall'adattamento del num_sample a seconda della risoluzione dell'asse tempo richiesta
*/
short printermanager::adatta_buffer_dati(int __num_sample, long __fs_flw)
{
    unsigned short j, k;		// valore massimo = 20min*60sec*10sample/sec = 12000
    unsigned short num_max_sample = __num_sample;

    for(int i = 0; i < (FREQ_ACQ * MAX_DURATA_ESAME *60); i++)
    {
        m_vol_store.append(0);
        m_flow_store.append(0);
    }

    for (int i=0; i<(FREQ_ACQ*MAX_DURATA_ESAME * 60);i++ )
        m_emg_store.append(0);

    j = 0;
    k= 0;
    switch(__fs_flw)
    {
    case 25:		// 5 sample al mm, cioè 5 sample effettivi per 8 punti: ne devo aggiungrere 3 fittizi
        for(int i = 0; i < (2*__num_sample); i++)
        {
            if(k < __num_sample)
            {
                j++;
                //buffer_vol[i] = *(ExamParam.PRINTPhysValBuf[READ_VOL] + i);
                m_vol_store[i] = buffer_vol[k];
                m_flow_store[i] = (unsigned short)buffer_flw[k];
                //*( vol_store + i ) = *( m_PRINTPhysValBuf[READ_VOL] + k );
                //*( flow_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_FLW] + k );
                if (m_emgPresent)
                    m_emg_store[i] = buffer_emg[k];
                //*( s3 + i ) = *( ExamParam.PRINTPhysValBuf[READ_EMG] + k );

                if((j == 2) || (j == 4) || (j == 5) || (j == 7))
                    k++;
                else if(j == 8)
                {
                    k++;
                    j = 0;
                }
            }
            else
            {
                num_max_sample = i - 1;
                break;
            }
        }
        break;
    case 50:		// rapporto 1:1 cioè 1mm per 1sec, allora 10 sample per mm, cioè ne devo eliminare 2 ogni 10

        for(int i = 0; i < __num_sample; i++)
        {
            if( (i + k) < (__num_sample - 1))
            {
                j++;
                if( 5 == j )
                {
                    j = 1;
                    k++;
                }
                m_vol_store[i] = buffer_vol[i+k];
                m_flow_store[i] = buffer_flw[i+k];
                if (m_emgPresent)
                    m_emg_store[i] = buffer_emg[i+k];
                //                *( vol_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + i + k );
                //                *( flow_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_FLW] + i + k );
                //                if(__num_curve == NUMOF_READ_CHAN)
                //                    *( s3 + i ) = *( ExamParam.PRINTPhysValBuf[READ_EMG] + i + k );
            }
            else
            {
                num_max_sample = i;
                break;
            }
        }
        if(num_max_sample > 0)	// le celle preallocate ma non usate causa compressione buffer dati, le fisso a valori precisi
            for(int i = (num_max_sample - 1); i < __num_sample; i++)
            {
                m_vol_store[i] = buffer_vol[num_max_sample + k -1];// fissata ala valore finale perchè questo non varia
                m_flow_store[i] = 0; // fissata a zero, perchè non c'è flusso
                if (m_emgPresent)
                    m_emg_store[i] = 0; // fissata a zero, perchè qui non devo avere attività alettrica registrata, o meglio non devo inventarmi dei valori io

                /**( vol_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + (num_max_sample + k -1)); // fissata ala valore finale perchè questo non varia
                *( flow_store + i ) = 0;		// fissata a zero, perchè non c'è flusso
                if(__num_curve == NUMOF_READ_CHAN)
                    *( s3 + i ) = 0;	*/		// fissata a zero, perchè qui non devo avere attività alettrica registrata, o meglio non devo inventarmi dei valori io
            }
        break;
    case 75:		// rapporto 1,5sec per mm, cioè 15sample per mm, 8punti per mm, cioè 15 sample per 8 punti, allora 1 sample ogni 2
        j = 1;	// devo prendere il primo valore
        k = 0;
        for(int i = 0; i < __num_sample; i++)
        {
            j++;
            if(j == 2)	// poi un campione ogni due
            {
                m_vol_store[k] = buffer_vol[i];
                m_flow_store[k] = buffer_flw[i];
                if (m_emgPresent)
                    m_emg_store[k] = buffer_emg[i];
                //                *( vol_store + k) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + i );
                //                *( flow_store + k ) = *( ExamParam.PRINTPhysValBuf[READ_FLW] + i );
                //                if(__num_curve == NUMOF_READ_CHAN)
                //                    *( s3 + k ) = *( ExamParam.PRINTPhysValBuf[READ_EMG] + i);
                k++;
                j = 0;
            }
        }
        num_max_sample = k;	// se num_sample=21, num_max_sample è 12, uno ogni due ma compreso il primo...
        // le celle preallocate ma non usate causa compressione buffer dati, le fisso a valori precisi
        for(int i = num_max_sample; i < __num_sample; i++)
        {
            m_vol_store[i] = buffer_vol[k -1];
            m_flow_store[i] =0;
            if (m_emgPresent)
                m_emg_store[i] = 0;
            //            *( vol_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + k -1); // fissata ala valore finale perchè questo non varia
            //            *( flow_store + i ) = 0;		// fissata a zero, perchè non c'è flusso
            //            if(__num_curve == NUMOF_READ_CHAN)
            //                *( s3 + i ) = 0;			// fissata a zero, perchè qui non devo avere attività alettrica registrata, o meglio non devo inventarmi dei valori io
        }
        break;
    case 100:	// rapporto 20 sample per mm, allora 20 sample per 8 punti, ogni 5 prendo il secondo e il quinto
        // il primo campione da prendere è il secondo, poi il quinto, il settimo, il decimo, il dodicesimo e così via
        j = k = 0;
        for(int i = 0; i < __num_sample; i++)
        {
            j++;
            if((j == 2) || (j == 5))
            {
                m_vol_store[k] = buffer_vol[i];
                m_flow_store[k] = buffer_flw[i];
                if (m_emgPresent)
                    m_emg_store[k] = buffer_emg[i];
                //                *( vol_store + k) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + i );
                //                *( flow_store + k ) = *( ExamParam.PRINTPhysValBuf[READ_FLW] + i );
                //                if(__num_curve == NUMOF_READ_CHAN)
                //                    *( s3 + k ) = *( ExamParam.PRINTPhysValBuf[READ_EMG] + i);
                if(j == 5)	j = 0;
                k++;

            }
        }
        num_max_sample = k;	// se num_sample=21, num_max_sample è 12, uno ogni due ma compreso il primo...
        // le celle preallocate ma non usate causa compressione buffer dati, le fisso a valori precisi
        for(int i = num_max_sample; i < __num_sample; i++)
        {
            m_vol_store[i] = buffer_vol[k-1];
            m_flow_store[i] = 0;
            if (m_emgPresent)
                m_emg_store[i] = 0;
            //            *( vol_store + i ) = *( ExamParam.PRINTPhysValBuf[READ_VOL] + k -1); // fissata ala valore finale perchè questo non varia
            //            *( flow_store + i ) = 0;		// fissata a zero, perchè non c'è flusso
            //            if(__num_curve == NUMOF_READ_CHAN)
            //                *( s3 + i ) = 0;			// fissata a zero, perchè qui non devo avere attività alettrica registrata, o meglio non devo inventarmi dei valori io
        }
        break;
    }

    return num_max_sample;
}

/**
Nella stampa del report tipo reale, qui si cercano i massimi dei vari array per settare i fondoscala dei grafici
*/
void printermanager::Calc_Max_RealReport_rel2(short __num_sample)
{
    short vol_max = 0;
    short emg_max = 0;

    for(int j = 0; j < __num_sample; j++)		// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) così da individuare il giusto fondoscala
    {
        if(m_vol_store[j] > vol_max)
            vol_max = m_vol_store[j];

        if(m_emgPresent)
        {
            if(m_emg_store[j] > emg_max)
                emg_max = m_emg_store[j];
        }
    }
    vol_max = ((vol_max/10)+1)*10;	// trovo la decina minima superiore al valore max
    emg_max = ((emg_max/10)+1)*10;	// trovo la decina minima superiore al valore max
    // FS del grafico volume
    if( vol_max <= 250)	// può assumere i valori della decina superiore al valore massimo: se 135 -> val_max = 140
        vol_max = 250;
    else if( vol_max <=500)	// può assumere i valori della decina superiore al valore massimo: se 135 -> val_max = 140
        vol_max = 500;
    else if( vol_max <= 750)	// può assumere i valori della decina superiore al valore massimo: se 135 -> val_max = 140
        vol_max = 750;
    else
        vol_max = 1000;
    m_max_vol = vol_max;
    // FS del grafico dell'EMG
    if(m_emgPresent)
    {
        if(emg_max <= 250)	// può assumere i valori 500, 1000, 2000, 2500
            emg_max = 250;
        else
            if(emg_max <= 500)	// può assumere i valori 500, 1000, 2000, 2500
                emg_max = 500;
            else
            {
                if(emg_max <= 1000)
                    emg_max = 1000;
                else if(emg_max <= 2000)
                    emg_max = 2000;
                else //if(emg_max <= 2500)
                    emg_max = 2500;
            }
        m_max_emg = emg_max;
    }
}

/**
disegna il/i grafici. prima costruisce il/gli asse/i delle ordinate della parte sinistra del grafico, dove saranno mostrati i fondoscala di flusso ed emg,
con 3 valori intermedi (1/4 FS, 1/2 FS, 3/4 FS).
poi viene costruito il grafico a pezzi di 5secondi ciascuno (40righe)
infine è cotruito l'asse delle ordinate destro, dove appare l'udm del volume e ik suo fs
*/
void printermanager::Report_Real_Time(short __num_sample)
{
    int num_righe;
    m_init_time_to_print = 0;
    m_cursore = 0;

    m_port->Pri_Font(1);		// altezza carattere 20 punti (righe) 0x18 in HEX
    m_port->Pri_mode(0x10);
    m_port->Pri_justif(2);	// bandiera a siistra

    // STAMPA DEI LABEL E DELL'ASSE SINISTRO (label EMG E FLUSSO)
    m_pos_gra_emg = 0;
    m_pos_gra_flw = 50;
    m_num_byte_x_gra_flw = 50;
    m_num_dots_gra_flw = 400.0;
    if(m_emgPresent)
    {
        m_pos_gra_vol = 20;
        m_num_dots_gra_emg = 160.0;
        m_num_dots_gra_vol = 240.0;
        m_num_byte_x_gra_emg = 20;
        m_num_byte_x_gra_vol = 30;
    }
    else
    {
        m_pos_gra_vol = 0;
        m_num_dots_gra_emg = 0;
        m_num_dots_gra_vol = 400.0;
        m_num_byte_x_gra_emg = 0;
        m_num_byte_x_gra_vol = 50;
    }
    m_num_byte_x_gra = 800;
    Pri_Rep_Label();	// stampa label del flusso ed eventualmente anche l'emg nel caso di grafici sovrapposti

    // STAMPA DEI GRAFICI DI ACQUISIZIONE, 8 RIGHE PER SECONDO DI ACQUSIZIONE
    // alloco lo spazio per la stringona del grafico
    //	righe = 40 (8 punti per secondo = 5sec)
    //	colonne = 1(label tempo) + 1(spazio) + 50(grafici) + 1(spazio) + 50(grafici) + 1(spazio)= 104byte

    //mem_alloc( dim_string_gr*sizeof(char), (void **)&str_tr);		// trasposizione stringa per ottenere matrice di punti per stampa grafica
    // SCOMPONGO LA STAMPA DEL GRAFICO IN TANTE STAMPE DA 5 SECONDI CIASCUNA
    num_righe = (int)(__num_sample / NUM_POINTS) + 1;	// numero intero di blocchi da 40 righe (5sec), il restante è stampato in un altro blocco da 40
    for(int riga = 0; riga < num_righe; riga++ )                        // scompone la griglia in tante righe
        Pri_Rep_Gra_Landscape(__num_sample); // scrive label di tempo ad ogni accesso

    // ora stampo l'asse destro del grafico
    Pri_Rep_asse_dx();
    m_port->Pri_mode(0x00);
    m_port->Pri_Font(1);
    m_port->Pri_Str( 3, (char*)LINE"\x1", 0 );		// modifica per risparm carta e tempo: da riattivare
}


/**
STAMPO in corrispondenza dell'asse ascisse grafico flusso, i label del flusso, dove lo zero sarà posto in corrispondenza della prima linea del grafico
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
        m_str_gr[ i ] = 0x00;

    // setta la stampa grafica
    m_str_gr[0]=ESC;	//0x1B;	// ESC
    m_str_gr[1]='*';	//0x2A;	// *
    m_str_gr[2]=0x40;	// n1	num_byte = dim_string_rel2 - 8 = 6472-8= 6464 = ( 65536 * n3 ) + ( 256 * n2 ) + n1 = 256*0x19 + 0x40
    m_str_gr[3]=0x19;	// n2
    m_str_gr[4]=0x00;	// n3
    m_str_gr[5]=0x00;	// n4	normal
    m_str_gr[6]=0x03;	// n5	scrive a n5 byte dal bordo
    m_str_gr[7]=0x65;	// n6	larghezza 808dots=101 byte
    // stampo lo zero dell'emg senza udm
    if(m_emgPresent)
    {
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
    for(int i = 1; i <= _NUM_LABEL; i++)
    {
        if(i == _NUM_LABEL)	// 5 label + lo zero
        {
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
        print_udm_label(m_chFlw, pos_4_this_string, dim_label);		// scritta dell'unità di misura dell'flw, cioe ml/s, scritta a fianco di ogni label numerica
    }

    if(m_emgPresent)							// stampo lo zero dell'emg senza udm
    {	// label dell'asse dell'volume
        for(int i = 1; i < _NUM_LABEL_VOL_EMG; i++)						// se c'è l'emg solo 2 label più lo zero
        {														// non scrivo il label del fondoscala, che sarebbe troppo vicino allo zero successivo, quindi scrivo solo
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
            print_udm_label(m_chVol, pos_4_this_string, dim_label);	// scritta dell'unità di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
        // label dell'asse dell'emg
        for(int i = 1; i < _NUM_LABEL_EMG; i++)							// un solo label oltre allo zero
        {
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
            print_udm_label(m_chEmg, pos_4_this_string, dim_label);	// scritta dell'unità di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
    }
    else															// se non c'è l'emg, metà grafico va al volume
    {// label dell'asse dell'volume
        for(int i = 1; i < _NUM_LABEL; i++)						// se c'è l'emg solo 2 label più lo zero
        {														// non scrivo il label del fondoscala, che sarebbe troppo vicino allo zero successivo, quindi scrivo solo
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
            print_udm_label(m_chVol, pos_4_this_string, dim_label);	// scritta dell'unità di misura dell'emg, cioe uV, scritta a fianco di ogni label numerica
        }
    }
    // aggiungo la linea dell''asse  delle ordinate, in ogni colonna l'ultimo bit viene acceso
    for(int i = 1; i < 101; i++)
    {
        m_str_gr[_NUM_BYTE_CMD + i*(dots3D) - 2] = (char)0xFF;
        m_str_gr[_NUM_BYTE_CMD + i*(dots3D) - 1] = (char)0xFF;
    }
    // trasposizione
    unsigned short num_col_max = dots3D;	// 32
    unsigned short num_rig_max = (dim_string_rel2 - 8) / num_col_max;	// 50

    for(int i = 0; i < (dim_string_rel2 - 8); i++ )
        m_str_tr[i] = (char)0x00;	// azzero tutta questa stringona
    for(int num_col = 0; num_col < num_col_max; num_col++ )
    {
        for(int num_rig = 0; num_rig < num_rig_max; num_rig++)
        {
            m_str_tr[ ( num_rig_max * num_col ) + num_rig ] = m_str_gr[ _NUM_BYTE_CMD + ( num_col_max * num_rig ) + num_col ];
        }
    }
    for( int str_byte = 0; str_byte < (dim_string_rel2 - 8); str_byte++ )
        m_str_gr[ _NUM_BYTE_CMD + str_byte ] = m_str_tr[ str_byte ];

    // finalmente stampa
    m_port->Pri_Str( dim_string_rel2, (char *)m_str_gr, 0);

}


/**
funzioncina dedicata alla stampa dei caratteri dell'asse a sinistra nel caso del report reale, che possono essere volume e flusso,
oppure emg, volume e flusso
le unità di misura sono invece inserite nella funzione principale
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
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
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
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
        else if(__num_char == 2)
        {
            second_char = ( __value / 10);
            if(__pri_decim == true)	// s devo mettere il punto del decimale, sposto di una riga il carattere intero e poi accendo i dots del punto decimale, nota che c'è l'OR
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
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
        else if(__num_char == 1)	// un solo carattere, scrivo solo le unità
        {
            third_char = __value;	//third_char = ( max_y_flw - ( first_char * 100 ) - ( second_char * 10 ) );
            for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
            }
        }
}


/**
Inserisce nella stringona le udm delle curve che possono essere: ml/s, ml, uV
*/
void printermanager::print_udm_label(int ch_type, short __pos_in_string, short int __pre_char)
{
    short init_pos_in_string;
    short int orriz_pos = _NUM_CHAR_X_LABEL_rel2 - __pre_char + 1; // con il "+1" ho già messo lo spazio vuoto fra numero e label
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
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ 13*_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;// seconda cifra (secondo carattere 1)
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] |= (char)print8x8Set[ 14*_WEIGHT_CHAR_LABEL + i];
        }
    }
    else if (ch_type == m_chEmg)
    {
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 15*_WEIGHT_CHAR_LABEL + i];
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;	// passo alla cifra suvvessiva
        for( i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
            m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ 16*_WEIGHT_CHAR_LABEL + i];
        }
    }
}


/**
GRAFICI IN LANDASCAPE MODE in accordo con specifiche ISC (indifferentemente dal fondoscala,
l'area sottesa al grafico del flussso, deve essere proporzionale al valore di flusso, quindi anche l'asse dei tempi si adegua al fondoscala
funzione chiamata ciclicamente per stampare le righe in cui è suddiviso il grafico del flusso/volume
a seconda del parametro num_rig si stampano o meno i label degli assi di flusso e volume
*/
void printermanager::Pri_Rep_Gra_Landscape(short __num_cample)
{
    bool stamp_x_label = false;
    //word i, z, j;
    short int first_char, second_char, third_char, num_char_label; //short fourth_char;
    int init_pos_in_string;
    static Print_Graph_Parameters PriGraParam_FLW;	// inizializzate a zero automaticamente all'inizializzazione
    static Print_Graph_Parameters PriGraParam_VOL;
    static Print_Graph_Parameters PriGraParam_EMG;
    Print_Graph_Parameters PriGraParam_OLD;

    float fattore_scala;

    for(int i = 0; i < dim_string_gr; i++ )	// rimepio di zeri le stringhe, cioè "spengo" tutti i pixel
    {
        m_str_gr[ i ] = 0x00;
        m_str_tr[ i ] = 0x00;
    }
    // setta la stampa grafica: i primi 8 caratteri della stringa sono comandi
    m_str_gr[0]=ESC;
    m_str_gr[1]='*';
    m_str_gr[2]=0x40;	// n1 = 64	num_byte = 4160 = ( 65536 * n3 ) + ( 256 * n2 ) + n1
    m_str_gr[3]=0x10;	// n2 = 16
    m_str_gr[4]=0x00;	// n3 = 0
    m_str_gr[5]=0x00;	// n4	normal
    m_str_gr[6]=0x01;	// n5 = 1	scrive a n5 byte dal bordo
    m_str_gr[7]=0x68;	// n6 = 104	larghezza 1+50+1+50+1=104 byte

    // incrementando init_time_to_print di 5 ad ogni chiamata di questa funzione, sarà sempre un multimplo di 5
    // incrementando init_time_to_print del valore in secondi, corrispondente a 40punti stampati, ad ogni chiamata di questa funzione,
    // sarà sempre un multimplo della divisione elementare (dipendente dal F.S. del flusso impostato)
    if(m_cursore == 0)	// stampo anche il carattere "s" del secondo
    {
        init_pos_in_string = _NUM_BYTE_CMD + (0)*_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ m_init_time_to_print*_WEIGHT_CHAR_LABEL + i];	// carattere "0"
        }
        init_pos_in_string = _NUM_BYTE_CMD + (1)*_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 14*_WEIGHT_CHAR_LABEL + i];					// carattere "S"
        }
        init_pos_in_string +=_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 17*_WEIGHT_CHAR_LABEL + i];					// carattere "e"
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 18*_WEIGHT_CHAR_LABEL + i];					// carattere "c"
        }
    }
    else
    {
        stamp_x_label = check_stamp_label();	// verifica se si deve stampare il label e ne definisce il valore a seconda della scala adottata

        if(stamp_x_label == true)
        {
            if( m_init_time_to_print > 99L )
            {
                num_char_label = 3;
                first_char = m_init_time_to_print / 100;
                init_pos_in_string = _NUM_BYTE_CMD + (num_char_label -3)*_HEIGHT_CHAR_LABEL;
                for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle centinaia per colonne come 1Bx13righe
                    m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ first_char*_WEIGHT_CHAR_LABEL + i];
                }
                init_pos_in_string = _NUM_BYTE_CMD + (num_char_label -2)*_HEIGHT_CHAR_LABEL;
                second_char = ( m_init_time_to_print - ( first_char * 100 ) ) / 10;
                for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                    m_str_gr[init_pos_in_string + i ] = (char)print8x8Set[ second_char*_WEIGHT_CHAR_LABEL + i];
                }
                init_pos_in_string = _NUM_BYTE_CMD + (num_char_label - 1)*_HEIGHT_CHAR_LABEL;
                third_char = ( m_init_time_to_print - ( first_char * 100 ) - ( second_char * 10 ) );
                for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unità per colonne come 1Bx13righe
                    m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
                }
            }
            else
            {
                if( m_init_time_to_print > 9 ) 	// da 10 a 99 quindi 2 char
                {
                    num_char_label = 2;
                    init_pos_in_string = _NUM_BYTE_CMD + (num_char_label-2)*_HEIGHT_CHAR_LABEL;
                    second_char =  m_init_time_to_print / 10;
                    for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                        m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ second_char*_WEIGHT_CHAR_LABEL + i];
                    }
                    init_pos_in_string = _NUM_BYTE_CMD + (num_char_label-1)*_HEIGHT_CHAR_LABEL;
                    third_char = m_init_time_to_print - ( second_char * 10 );
                    for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                        m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
                    }
                }
                else {		// < 10 cioè un char
                    num_char_label = 1;
                    init_pos_in_string = _NUM_BYTE_CMD + (num_char_label-1)*_HEIGHT_CHAR_LABEL;
                    third_char = m_init_time_to_print;
                    for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                        m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ third_char*_WEIGHT_CHAR_LABEL + i];
                    }
                }
            }
        }
    }
    // inserisco i trattini riferitiagli intervalli di 1 sec
    for(int i = 0; i < 5; i++ )
    {
        if((stamp_x_label == true) && (i == 0))	// trattino + grosso per i multpli di cinque
        {
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8] |= (char)0x0f;	// trattino corrispondente ad ogni secondo sull'asse del tempo
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8 + 1] |= (char)0x0f;
        }
        else
            m_str_gr[ _NUM_BYTE_CMD + NUM_POINTS + i*8] |= (char)0x07;
    }
    // inserisco bordo  grafico emg e flw
    for(int i = NUM_POINTS; i < 2*NUM_POINTS; i++ )
    {
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo inferiore riga 1
    }
    for(int i = 2*NUM_POINTS; i < 3*NUM_POINTS; i++ )
    {
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore riga2
    }
    for(int i = 102*NUM_POINTS; i < 103*NUM_POINTS; i++ )
    {
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0xC0;	// bordo superiore flw doppia
    }
    // linea separazione grafico volume e flusso a metà scala
    for(int i = 51*NUM_POINTS; i < 52*NUM_POINTS; i++ )
    {
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo superiore volume
    }
    for(int i = 52*NUM_POINTS; i < 53*NUM_POINTS; i++ )
    {
        m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore flusso
    }
    // inserisco tratteggi verticali, riferiti agli istanti multipli di 10sec
    if(stamp_x_label == true)
    {
        for(int i = 0; i < 100; i++ )
        {
            m_str_gr[ _NUM_BYTE_CMD + (2 + i)*NUM_POINTS] |= (char)0x44;	// tratteggio verticale istante di tempo
        }
    }
    // inserisco tratteggi orrizzontali, riferiti ai valori della grandezza mostrata,
    // partendo dal basso le prime 4 linee tratteggiate ci sono sempre e a quota fissa sulla scala 0-800 (40, 80, 120, 160). Se riferito alla matrice di 102byte
    //si tratta di (5+2, 10+2, 15+2, 20+2) cioè 6*NUM_POINTS, 10*NUM_POINTS....
    // Se c'è l'emg, allora altre 4 linee tratteggiate distanti tra loro 40, a quota (240, 280, 320, 360) poi 24 linee distanti tra loro 16
    // delle quali, ogni 5, la quinta è intera, le altre tratteggiate
    // se non c'è l'emg, allora 24 linee distanti tra loro 24 delle quali, ogni 5, la quinta è intera, le altre tratteggiate
    // primo grafico 4 tratteggi: 1/5, 2/5, 3/5, 4/5 del val max
    for(int i = 0; i < 5; i++ )
    {
        for(int z = 0; z < 8; z++ )		//for( z = 2; z < 6; z++ )
        {
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
    // se c'è anche il emg allora devo creare i due bordi separatori dei e grafic
    if(m_emgPresent)
    {
        for(int i = 21*NUM_POINTS; i < 22*NUM_POINTS; i++ )
        {
            m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x01;	// bordo superiore emg
        }
        for(int i = 22*NUM_POINTS; i < 23*NUM_POINTS; i++ )
        {
            m_str_gr[ _NUM_BYTE_CMD + i] |= (char)0x80;	// bordo inferiore volume
        }
    }
    // costruzione assi grafico: quattro sono i caratteri anteposti senza byte dei comandi
    // inserimento pixel curve
    //str_gr[ pos_gra + BYTE_GRAPH ] = (char)0xFF;
    // inserimento pixel flusso: ho 600 o 400 punti a seconda del numero di grafici
    //cursore = init_time_to_print*NUM_DOTS_X_SEC;

    fattore_scala = m_num_dots_gra_flw/(float)(10*m_max_y);	// dove max_y = 25, 50, 75, 100, quindi il rapporto = 4, 2, 1, 0.5, 0.25
    for(int i = 0; i < NUM_POINTS; i++)
    {
        PriGraParam_OLD.uw8 = PriGraParam_FLW.uw8;
        PriGraParam_OLD.uw9 = PriGraParam_FLW.uw9;
        PriGraParam_OLD.sample_adattato = PriGraParam_FLW.sample_adattato;				// salviamo il valor eprecedente
        if((i+m_cursore) < __num_cample)
        {
            PriGraParam_FLW.sample_adattato = (int)(m_flow_store[i + m_cursore] * fattore_scala);	// adattamento del valore corrente al fondoscala corrente

            if(PriGraParam_FLW.sample_adattato > 0)
            {
                PriGraParam_FLW.uw8 = PriGraParam_FLW.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                PriGraParam_FLW.uw9 = PriGraParam_FLW.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo però da MSB nel byte individuato
                if(PriGraParam_FLW.sample_adattato > PriGraParam_OLD.sample_adattato)
                {
                    PriGraParam_FLW.verso_curva = positivo;
                    if(PriGraParam_FLW.uw8 > PriGraParam_OLD.uw8)
                    {
                        for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_FLW.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_FLW.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                        for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                    else
                    {
                        for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_FLW.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                }
                else
                    if(PriGraParam_FLW.sample_adattato < PriGraParam_OLD.sample_adattato)
                    {
                        if(PriGraParam_FLW.verso_curva == positivo)	// niente riga perchè si sovrapporrebbe con quella del precedente
                        {
                            PriGraParam_FLW.verso_curva = negativo;
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_FLW.uw9); // qui niente riga solo un punto.
                        }
                        else	// qui invece la riga dal valore prec all'attuale
                        {
                            if( PriGraParam_FLW.uw8 < PriGraParam_OLD.uw8)
                            {
                                for(int j = (PriGraParam_FLW.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                                for(int j = PriGraParam_FLW.uw9; j < 8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                            else
                            {
                                for(int j = PriGraParam_FLW.uw9; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                        }
                    }
                    else	// qui niente riga solo un punto.
                    {
                        PriGraParam_FLW.verso_curva = negativo;
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_FLW.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_FLW.uw9); // qui niente riga solo un punto.
                    }
            }
            else	// il nuovo valore è 0 (reale o adattato) comunque sta sull'asse
            {
                PriGraParam_FLW.uw8 = PriGraParam_FLW.uw9 = 0;
                PriGraParam_FLW.verso_curva = positivo;
                if(PriGraParam_OLD.sample_adattato > 0)	// allora riga da 0 a sample_adattato_prec
                {
                    for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> j);
                }
                else
                {
                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_flw)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in più allo zero per evidenziare il valore a zero
                }
            }
        }
        else
            break;
    }

    fattore_scala = m_num_dots_gra_vol/(float)(m_max_vol);	// dove max_vol = 250, 500, 750, 1000
    for(int i = 0; i < NUM_POINTS; i++)
    {
        PriGraParam_OLD.uw8 = PriGraParam_VOL.uw8;
        PriGraParam_OLD.uw9 = PriGraParam_VOL.uw9;
        PriGraParam_OLD.sample_adattato = PriGraParam_VOL.sample_adattato;				// salviamo il valor eprecedente
        if((i+m_cursore) < __num_cample)
        {
            PriGraParam_VOL.sample_adattato = (int)(m_vol_store[i + m_cursore] * fattore_scala);	// adattamento del valore corrente al fondoscala corrente

            if(PriGraParam_VOL.sample_adattato > 0)
            {
                PriGraParam_VOL.uw8 = PriGraParam_VOL.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                PriGraParam_VOL.uw9 = PriGraParam_VOL.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo però da MSB nel byte individuato
                if(PriGraParam_VOL.sample_adattato > PriGraParam_OLD.sample_adattato)
                {
                    PriGraParam_VOL.verso_curva = positivo;
                    if(PriGraParam_VOL.uw8 > PriGraParam_OLD.uw8)
                    {
                        for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_VOL.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_VOL.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                    else
                    {
                        for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_VOL.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                    }
                }
                else
                    if(PriGraParam_VOL.sample_adattato < PriGraParam_OLD.sample_adattato)
                    {
                        if(PriGraParam_VOL.verso_curva == positivo)	// niente riga perchè si sovrapporrebbe con quella del precedente
                        {
                            PriGraParam_VOL.verso_curva = negativo;
                            m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_VOL.uw9); // qui niente riga solo un punto.
                        }
                        else	// qui invece la riga dal valore prec all'attuale
                        {
                            if(PriGraParam_VOL.uw8 < PriGraParam_OLD.uw8)
                            {
                                for(int j = (PriGraParam_VOL.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                for(int j = PriGraParam_VOL.uw9; j < 8; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                            else
                            {
                                for(int j = PriGraParam_VOL.uw9; j < PriGraParam_OLD.uw9; j++)
                                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            }
                        }
                    }
                    else	// qui niente riga solo un punto.
                    {
                        PriGraParam_VOL.verso_curva = negativo;
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_VOL.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_VOL.uw9); // qui niente riga solo un punto.
                    }
            }
            else	// il nuovo valore è 0 (reale o adattato) comunque sta sull'asse
            {
                PriGraParam_VOL.uw8 = PriGraParam_VOL.uw9 = 0;
                PriGraParam_VOL.verso_curva = positivo;
                if(PriGraParam_OLD.sample_adattato > 0)	// allora riga da 0 a sample_adattato_prec
                {
                    for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                        m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);
                }
                else
                {
                    m_str_gr[_NUM_BYTE_CMD + (2 + m_pos_gra_vol)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in più allo zero per evidenziare il valore a zero
                }
            }
        }
        else
            break;
    }
    if(m_emgPresent)
    {
        fattore_scala = m_num_dots_gra_emg/(float)(m_max_emg);	// dove max_y = 10, 20, 40, 80, 160, quindi il rapporto = 4, 2, 1, 0.5, 0.25
        for(int i = 0; i < NUM_POINTS; i++)
        {
            PriGraParam_OLD.uw8 = PriGraParam_EMG.uw8;
            PriGraParam_OLD.uw9 = PriGraParam_EMG.uw9;
            PriGraParam_OLD.sample_adattato = PriGraParam_EMG.sample_adattato;				// salviamo il valor eprecedente
            if((i+m_cursore) < __num_cample)
            {
                PriGraParam_EMG.sample_adattato = (int)(m_emg_store[i + m_cursore] * fattore_scala);	// adattamento del valore corrente al fondoscala corrente

                if(PriGraParam_EMG.sample_adattato > 0)
                {
                    PriGraParam_EMG.uw8 = PriGraParam_EMG.sample_adattato / 8;	// individuo di quanti byte mi devo spostare a destra, a partire dall'asse sinistro
                    PriGraParam_EMG.uw9 = PriGraParam_EMG.sample_adattato % 8;	// il resto della divisione mi da il dot dell'ultimo byte che devo accendere, individuo il bit (o dot) da accendere partendo però da MSB nel byte individuato
                    if(PriGraParam_EMG.sample_adattato > PriGraParam_OLD.sample_adattato)
                    {
                        PriGraParam_EMG.verso_curva = positivo;
                        if(PriGraParam_EMG.uw8 > PriGraParam_OLD.uw8)
                        {
                            for(int j = (PriGraParam_OLD.uw8 + 1); j < PriGraParam_EMG.uw8; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                            for(int j = 0; j < PriGraParam_EMG.uw9; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                            for(int j = PriGraParam_OLD.uw9; j < 8; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        }
                        else
                        {
                            for(int j = (PriGraParam_OLD.uw9 + 1); j <= PriGraParam_EMG.uw9; j++)
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                        }
                    }
                    else
                        if(PriGraParam_EMG.sample_adattato < PriGraParam_OLD.sample_adattato)
                        {
                            if(PriGraParam_EMG.verso_curva == positivo)	// niente riga perchè si sovrapporrebbe con quella del precedente
                            {
                                PriGraParam_EMG.verso_curva = negativo;
                                m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_EMG.uw9); // qui niente riga solo un punto.
                            }
                            else	// qui invece la riga dal valore prec all'attuale
                            {
                                if(PriGraParam_EMG.uw8 < PriGraParam_OLD.uw8)
                                {
                                    for(int j = (PriGraParam_EMG.uw8 + 1); j < PriGraParam_OLD.uw8; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                                    for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                    for(int j = PriGraParam_EMG.uw9; j < 8; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                }
                                else
                                {
                                    for(int j = PriGraParam_EMG.uw9; j < PriGraParam_OLD.uw9; j++)
                                        m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);	// creo la riga dei dots dell'ultimo byte
                                }
                            }
                        }
                        else	// qui niente riga solo un punto.
                        {
                            PriGraParam_EMG.verso_curva = negativo;
                            m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_EMG.uw8)*NUM_POINTS + i ] |= (char)(0xC0 >> PriGraParam_EMG.uw9); // qui niente riga solo un punto.
                        }
                }
                else	// il nuovo valore è 0 (reale o adattato) comunque sta sull'asse
                {
                    PriGraParam_EMG.uw8 = PriGraParam_EMG.uw9 = 0;
                    PriGraParam_EMG.verso_curva = positivo;
                    if(PriGraParam_OLD.sample_adattato > 0)	// allora riga da 0 a sample_adattato_prec
                    {
                        for(int j = 0; j < PriGraParam_OLD.uw8; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + j)*NUM_POINTS + i ] |= (char)0xFF;	// creo la riga dei byte interi
                        for(int j = 0; j < PriGraParam_OLD.uw9; j++)
                            m_str_gr[_NUM_BYTE_CMD + (2 + PriGraParam_OLD.uw8)*NUM_POINTS + i ] |= (char)(0x80 >> j);
                    }
                    else
                    {
                        m_str_gr[_NUM_BYTE_CMD + (2)*NUM_POINTS + i ] |= (char)0xC0; // accendo solo un pixel in più allo zero per evidenziare il valore a zero
                    }
                }
            }
            else
                break;
        }
    }
    m_cursore += NUM_POINTS;	// incrementato di 40 punti (quindi sample) ogni giro
    // trasposizione

    unsigned short num_col_max = NUM_POINTS;	// 40
    unsigned short num_rig_max = (dim_string_gr -8) / num_col_max;	// 104

    for(int i = 0; i < (dim_string_gr - 8); i++ )
        m_str_tr[i] = (char)0x00;	// azzero tutta questa stringona
    for(int num_col = 0; num_col < num_col_max; num_col++ )
    {
        for(int num_rig = 0; num_rig < num_rig_max; num_rig++)
        {
            m_str_tr[ ( num_rig_max * num_col ) + num_rig ] = m_str_gr[ _NUM_BYTE_CMD + ( num_col_max * num_rig ) + num_col ];
        }
    }
    for(int str_byte = 0; str_byte < (dim_string_gr -8); str_byte++ )
        m_str_gr[ _NUM_BYTE_CMD + str_byte ] = m_str_tr[ str_byte ];

    // finalmente stampa
    m_port->Pri_Str( (dim_string_gr), (char *)m_str_gr, 0);

}

/**
a seconda del massimo del flusso, si sceglie un fondo scala verticale e il conseguente asse dei tempi. A seconda dei sample fin qui stampati,
si risale al tempo per comandare la stampa del label (in secondi) o meno
*/
bool printermanager::check_stamp_label()
{
    int num_sec;

    switch(m_max_y)	// i F.S. previsti per il flusso in ml/sec
    {
    case 25:	// ml/sec
        num_sec = (m_cursore / 16);		// 1 secondo ogni 16 punti
        if((num_sec % 5) == 0)				// è numero divisibile per 5sec quindi è uno dei label da stampare
        {
            m_init_time_to_print = num_sec;		// può assumere i valori 5,10,15,20,25,30....
            return true;
        }
        break;
    case 50:
        num_sec = (m_cursore / 8);			// 1 secondo ogni 8 punti
        if((num_sec % 10) == 0)				// è numero divisibile per 10sec quindi è uno dei label da stampare
        {
            m_init_time_to_print = num_sec;		// può assumere i valori 10,20,30,40....
            return true;
        }
        break;
    case 75:
        num_sec = (m_cursore * 3 / 16);		// 3 second1 ogni 16 punti
        if((num_sec % 15) == 0)				// è numero divisibile per 10sec quindi è uno dei label da stampare
        {
            m_init_time_to_print = num_sec;		// può assumere i valori 10,20,30,40....
            return true;
        }
        break;
    case 100:
        num_sec = (m_cursore / 4);		// 3 second1 ogni 16 punti
        if((num_sec % 20) == 0)				// è numero divisibile per 10sec quindi è uno dei label da stampare
        {
            m_init_time_to_print = num_sec;		// può assumere i valori 10,20,30,40....
            return true;
        }
        break;
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
        m_str_gr[ i ] = 0x00;

    // setta la stampa grafica
    m_str_gr[0]=ESC;	//0x1B;	// ESC
    m_str_gr[1]='*';	//0x2A;	// *
    m_str_gr[2]=0x30;	// n1	num_byte = dim_string_solo_ax - 8 = 816 = ( 65536 * n3 ) + ( 256 * n2 ) + n1 = 256*0x03 + 0x30
    m_str_gr[3]=0x03;	// n2
    m_str_gr[4]=0x00;	// n3
    m_str_gr[5]=0x00;	// n4	normal
    m_str_gr[6]=0x02;	// n5	scrive a n5 byte dal bordo
    m_str_gr[7]=0x66;	// n6	larghezza 816dots=102 byte

    // aggiungo la linea dell''asse  delle ordinate, in ogni colonna il primo bit viene acceso
    m_str_gr[_NUM_BYTE_CMD] = (char)0x01;
    m_str_gr[_NUM_BYTE_CMD + 1] = (char)0x01;
    for(int i = 1; i < 101; i++)
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
        m_str_tr[i] = (char)0x00;	// azzero tutta questa stringona
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
    m_port->Pri_Str( dim_string_solo_ax, (char *)m_str_gr, 0);

}

/**
Stampa dei grafici di Syroki, sono 2 uno fianco l'altro
*/
void printermanager::Report_siroky()
{
    m_port->Pri_Font(1);
    m_port->Pri_mode(0x10);

    //Pri_Str(19,(char *)space_bar,0);
    m_port->Pri_justif(0);
    QString siroky = tr("Siroky Diagram");
    m_port->Pri_Str(strlen(siroky.toLatin1().data()),siroky.toLatin1().data(),1);

    m_port->Pri_Str(3,(char*)LINE"\x1",0);
    m_port->Pri_Font(1);

    m_port->Pri_justif(2);
    m_port->Pri_mode(0x00);
    QString ave = tr(" Average Flow ");
    QString max = tr(" Maximum Flow ");
    char str[90];
    sprintf(str,   "  Q(ml/s)   %s    SD        %s       SD", ave.toLatin1().data(),max.toLatin1().data());
    m_port->Pri_Str(strlen(str),str,1);//Pri_Str(strlen(str),str,0);

    m_port->Pri_Speed(0);
    for(int ub = 0; ub < 10; ub++) {  /*scompone la griglia in 10 righe*/
        Pri_Rep_Gra_Siroky(ub,0);  // qui passo sempre grap=0 così lo resetto ad ogni giro

    }

    m_port->Pri_Font(0);	// carico per l'ccasione il font + piccolino
    m_port->Pri_mode(0x00);
    sprintf(str, "    0     100     200     300     400     500  0      100     200     300     400     500\n");
    m_port->Pri_Str(strlen(str),str,0);
    // stampo l'indicazione volume sull'asse ordinate dei grafici siroky
    QString voidvol = tr("     Voided Volume (mL)");
    m_port->Pri_Str(strlen(voidvol.toLatin1().data()),voidvol.toLatin1().data(),1);
    // ora devo stampare o l'indicazione di furoi scala per uno o entrambi i grafici, oppure le scritta "valido solo per maschi adulti"

    QString out = tr("Out of Range");
    if((m_max_vol >= 500) || ((m_flu_med >= 30) && (m_flu_max >= 30)) )	// entrambi fuori scala, messaggio al centrp
    {
        m_port->Pri_justif(F_center);
        m_port->Pri_Str(strlen(out.toLatin1().data()),out.toLatin1().data(),1);
    }
    else
    {
        if(m_flu_med >= 30)	// fuori scala il grafico flusso medio, scritta a sinistra
        {
            m_port->Pri_justif(F_left);
            sprintf(str, "                     %s", out.toLatin1().data());
            m_port->Pri_Str(strlen(str),str,1);
        }
        else
            if( m_flu_max >= 30 )		// fuori scala flusso massimo, scritta a destra
            {
                m_port->Pri_justif(F_right);
                sprintf(str, "%s                     ", out.toLatin1().data());
                m_port->Pri_Str(strlen(str),str,1);
            }
            else 	// nessuno fuori scala, scrivo centrato valido solo per maschi adulti
            {
                m_port->Pri_justif(F_center);
                QString valid = tr("  Valid for male adult only      ");
                m_port->Pri_Str(strlen(valid.toLatin1().data()),valid.toLatin1().data(),1);
            }
    }
    m_port->Pri_Str(3,(char*)LINE"\x1",0);//Pri_Str(3,LINE"\x3",0);
}


/**
disegna i diagrammi di Siriolokky
*/
void  printermanager::Pri_Rep_Gra_Siroky (byte __num_riga, byte __grap)
{
    byte z = 0;

    static bool label = false;
    static short int max_y_flw;
    static short int second_char, third_char;

    // azzeramento stringone
    for( int uw4 = 0; uw4 < ( LCMD + a_ave + a_max ); uw4++)  // per aggiungere spazietti fra i due grafici
        m_str_gr[ uw4 ] = 0x00;

    m_uw3 = 24 * 45;

    m_str_gr[0]=ESC;		//0x1B;	// ESC
    m_str_gr[1]='*';		//0x2A;	// *
    m_str_gr[2]=0x30;	// n1	num_byte = 1080 = ( 65536 * n3 ) + ( 256 * n2 ) + n1
    m_str_gr[3]=0x09;	// n2
    m_str_gr[4]=0x00;	// n3
    m_str_gr[5]=0x00;	// n4	// singola altezza
    m_str_gr[6]=0x02;	// n5
    m_str_gr[7]=0x62;	// n6	// larghezza a_ave + a_max

    // inserimento label Q_ave
    if( (__num_riga == 0) || ( !(__num_riga % 2) ) )
        label = true;  // scrive i label solo nelle righe 0, 2, 4, 6, 8
    else
        label = false; // nelle righe 1, 3, 5, 7 metter caratteri vuoti

    if( label )	// se label è true devo scivere i valori sugli assi di flw e vol, altrimenti riempio con caratterei vuoti
    {
        // la prima riga del grafico si compone di 24 righe di 45byte ciascuna alle quali antepongo le 24 righe di 3 byte del dato di flw
        max_y_flw = (int)( 30 - (__num_riga/2)*( 30 / 5 ));
        // NOTA BENE: il Qmax è sempre 30, perciò 2 cifre. Allora la prima cifra del label è sempre 0
        // e non devo riempirlo questo spazio perchè str_gr[n]-->str_gr[n+(m-2)*24] è già azzerata
        if( max_y_flw > 9 )
        {
            // seconda cifra (secondo carattere 1)
            second_char = ( max_y_flw) / 10;
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ LCMD + ((CLS-1)-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
            }
            // prima cifra (terzo carattere 2)
            third_char = max_y_flw - ( second_char * 10 );
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[ LCMD + ((CLS-1)-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
        }
        else {	// 2 byte di spazio, 1 di valore
            third_char = max_y_flw /* - ( second_char * 10 )*/;
            for(int i = 0; i < 13; i++ )	{// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[ LCMD + ((CLS-1)-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
        }
    }
#ifdef __ZERO_ASSE_SIROKY__
    else {												// niente label m caratteri vuoti
        if( __num_riga == 9 )								// se è l'ultima riga sovrascrivo lo "0" al terzo carattere
            for(int i = 0; i < 11; i++ ) {	// ho tarato i numeri per avere lo zero esattamente in fondo al grafico
                m_str_gr[ LCMD + ((CLS-1)-1)*24 + (i+12) ] = (char)print7x13Set[ 0*13 + i];
            }
    }
#endif

    /* assi e griglie */
    Pri_Rep_Gra_Ini_Grid_Sir( __num_riga, __grap );
#ifdef __DEV_STD__
    /* Curve Dev Standard   */
    if( ( __num_riga > 1 ) && ( __grap == 0 ) )
    {
        Plot_StdCurve_Siroky( __num_riga, __grap ); // disegna le 4 curve di deviazione standard sul grafico del Q_ave
    }
#endif
#ifdef __LABEL_SD__
    /* scrittura label SD	*/
    if( __num_riga == 2)
    {
        z = 10; // tarato per avere il numerino proprio in prossimità della fine della curva
        for(int i = 0; i < 10; i++ )	{
            m_str_gr[ pos_lab_SD_ave + i + z ] = (char)print8x10SetSD[ 0*10 + i ];	// copio il carattere "0" che è un (1x10)byte
        }
    }else {
        if( __num_riga == 4) {
            z = 0;// tarato per avere il numerino proprio in prossimità della fine della curva
            for(int i = 0; i < 10; i++ )
                m_str_gr[ pos_lab_SD_ave + i + z] = (char)print8x10SetSD[ 1*10 + i ];	// copio il carattere "-1" che è un (1x10)byte
        } else {
            if( __num_riga == 5) {
                z = 10;// tarato per avere il numerino proprio in prossimità della fine della curva
                for(int i = 0; i < 10; i++ )// tarato per avere il numerino proprio in prossimità della fine della curva
                    m_str_gr[ pos_lab_SD_ave + i + z] = (char)print8x10SetSD[ 2*10 + i ];	// copio il carattere "-2" che è un (1x10)byte
            } else {
                if( __num_riga == 7){
                    z = 0;// tarato per avere il numerino proprio in prossimità della fine della curva
                    for(int i = 0; i < 10; i++ )
                        m_str_gr[ pos_lab_SD_ave + i + z] = (char)print8x10SetSD[ 3*10 + i ];	// copio il carattere "-3" che è un (1x10)byte
                }
            }
        }
    }
#endif
#ifdef __POINT__
    /*      Punti paziente   */
    Plot_Point_Siroky( __num_riga, __grap, (short unsigned)( m_flu_med ) );
#endif
    // costruzione Grafico Q_max
    __grap = 1;

    /* label Q_max	*/
    if( label )
    {
        max_y_flw = (int)( 30 - (__num_riga/2)*( 30 / 5 ));

        if( max_y_flw > 9 ) {								// 1 byte di spazio , 2 di valore
            second_char = ( max_y_flw) / 10;				// seconda cifra (secondo carattere 1)
            for(int i = 0; i < 13; i++ )	{					// scrivo il carattere delle decine per colonne come 1Bx13righe
                m_str_gr[ pos_lab_Q_max + ((CLS-1)-2)*24 + i ] = (char)print7x13Set[ second_char*13 + i];
            }

            third_char = max_y_flw - ( second_char * 10 );	// prima cifra (terzo carattere 2)
            for(int i = 0; i < 13; i++ )	{					// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[ pos_lab_Q_max + ((CLS-1)-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
        }
        else {												// 2 byte di spazio, 1 di valore
            third_char = max_y_flw;							// prima cifra (terzo carattere 2)
            for(int i = 0; i < 13; i++ )	{					// scrivo il carattere delle unita per colonne come 1Bx13righe
                m_str_gr[ pos_lab_Q_max + ((CLS-1)-1)*24 + i ] = (char)print7x13Set[ third_char*13 + i];
            }
        }
    }
#ifdef __ZERO_ASSE_SIROKY__
    else {													// niente label m caratteri vuoti
        if( __num_riga == 9 )									// se è l'ultima riga scrivo lo "0" come terzo carattere
            for(int i = 0; i < 13; i++ )
                m_str_gr[ pos_lab_Q_max + ((CLS-1)-1)*24 + (i+10) ] = (char)print7x13Set[ 0*13 + i];
    }
#endif

    /* assi e griglie */
    Pri_Rep_Gra_Ini_Grid_Sir( __num_riga, __grap );
#ifdef __DEV_STD__
    /* Curve Dev Standard   */
    Plot_StdCurve_Siroky( __num_riga, __grap ); // disegna le 4 curve di deviazione standard sul grafico del Q_ave
#endif
#ifdef __LABEL_SD__
    /* scrittura label SD	*/
    if( __num_riga == 0) {
        for(int i = 0; i < 10; i++ )	{
            m_str_gr[ pos_lab_SD_max + i ] = (char)print8x10SetSD[ 0*10 + i ];	// copio il carattere "0" che è un (1x10)byte
        }
    }else {
        if( __num_riga == 2) {
            z = 10;// tarato per avere il numerino proprio in prossimità della fine della curva
            for(int i = 0; i < 10; i++ )// tarato per avere il numerino proprio in prossimità della fine della curva
                m_str_gr[ pos_lab_SD_max + i + z] = (char)print8x10SetSD[ 1*10 + i ];	// copio il carattere "-1" che è un (1x10)byte
        } else {
            if( __num_riga == 4) {
                z = 14;// tarato per avere il numerino proprio in prossimità della fine della curva
                for(int i = 0; i < 10; i++ )// tarato per avere il numerino proprio in prossimità della fine della curva
                    m_str_gr[ pos_lab_SD_max + i +z] = (char)print8x10SetSD[ 2*10 + i ];	// copio il carattere "-2" che è un (1x10)byte
            } else {
                if( __num_riga == 7)
                    for(int i = 0; i < 10; i++ )
                        m_str_gr[ pos_lab_SD_max + i ] = (char)print8x10SetSD[ 3*10 + i ];	// copio il carattere "-3" che è un (1x10)byte
            }
        }
    }
#endif
#ifdef __POINT__
    /*      Punti paziente   */
    Plot_Point_Siroky( __num_riga, __grap, (unsigned short)m_flu_max );
#endif

    Str_Trasposta( 1 , CLS-1, CLD); // fa la trasposta di str_gr che è costruita per colonne mentre noi si stampa per righe

    /* STAMPA DELLA num_riga-esima riga dei grafici siroky	*/
    m_port->Pri_Str(( LCMD + a_ave + a_max ), (char *)m_str_gr, 0);

}	// fine funzione Pri_rep_gra_Siroky




void printermanager::Pri_Rep_Gra_Ini_Grid_Sir(byte __num_rig, byte __curve )
{
    static unsigned short pos_gra_corr;

    if( __curve == 0 )
        pos_gra_corr = pos_gra_Qmed;	// curve sul grafico Q_ave
    else if( __curve == 1 )
        pos_gra_corr = pos_gra_Qmax;	// curve sul grafico Q_max quindi spostate di a_ave	+ 2 spaziettini
    /* Bordo Verticale Sinistro e Bordo Verticale Destro */
    for(int uw4 = 0; uw4 < 24; uw4++)
    {
        m_str_gr[ pos_gra_corr + uw4 ] = (char)0x80;
        m_str_gr[ pos_gra_corr + m_uw3 - 24 + uw4 ] = (char)0x01;
    }
    if( __num_rig % 2 == 0 )
    {
        /* Bordo Superiore   */
        if( __num_rig == 0)
        {
            for(int uw4 = 0; uw4 < m_uw3; uw4 += 24)
                m_str_gr[ pos_gra_corr + uw4 ] = (char)0xFF;
        }else{
            /* Griglia Orizzontale   */
            for(int uw4 = 0; uw4 < m_uw3; uw4 += 24)
                m_str_gr[ pos_gra_corr + uw4 ] |= (char)0x10;
        }
    }
    /*     Bordo Inferiore   */
    if( __num_rig == 9)	{
        for(int uw4 = 23; uw4 < m_uw3; uw4 += 24)
            m_str_gr[ pos_gra_corr + uw4 ] = (char)0xFF;
    }
    /*      Griglia Verticale   */
    for(int i = 1; i < 5; i++)	{
        int i4 = 216 * i;
        for(int i3 = 0; i3 < 24; i3 += 4)	{
            m_str_gr[ pos_gra_corr + i4 + i3 ] |= 0x80;
        }
    }
}	// fine funzione Pri_Rep_Gra_Ini_Grid_Sir


void printermanager::Plot_StdCurve_Siroky(byte __num_rig, byte __curve )
{
    static unsigned short pos_gra_corr;

    if( __curve == 0 )
        pos_gra_corr = pos_gra_Qmed;	// curve sul grafico Q_ave
    else if( __curve == 1 )
        pos_gra_corr = pos_gra_Qmax;	// curve sul grafico Q_max quindi spostate di a_ave + 2 spaziettini

    for(int i22 = 0; i22 < 4; i22++ )	// sono 4 le curve
    {
        int i5 = ( ( i22 ) % 2 ) + 1;
        for(int ub3 = 1; ub3 < 20; ub3++ )
        {
            int flow_pt_prec = 240 - SD[ __curve ][ i22 ][ ub3 - 1 ];
            if( flow_pt_prec < 240 )
            {
                int flow_pt = 240 - SD[ __curve ][ i22 ][ ub3 ];
                unsigned char ub2 = (byte)( flow_pt / 24 );
                unsigned char ub4 = (byte)( flow_pt_prec / 24);
                if( ( ub2 == __num_rig ) || ( ub4 == __num_rig ))
                {
                    int vol_pt_prec = ( 18 * ub3 );
                    int vol_pt = vol_pt_prec + 18;
                    int i4 = ( flow_pt_prec - flow_pt );
                    for(int i3 = vol_pt_prec; i3 < vol_pt; i3 += i5)
                    {
                        flow_pt = flow_pt_prec - ( i3 - vol_pt_prec ) * i4 / 18;
                        ub4 = (byte)( flow_pt / 24);
                        if( __num_rig == ub4)
                        {
                            ub4 = 0x80 >> (byte)(i3 % 8);
                            unsigned short uw4 = (unsigned short)( i3 / 8);
                            ub2 = ( flow_pt % 24 );
                            uw4 = uw4 * 24;
                            m_str_gr[ pos_gra_corr + uw4 + ub2 ] |= (char)ub4;
                            if( ( i3 % 8) == 0 )
                            {
                                if( i22 == 2)
                                    for (int i = 16; i > ub2; i -= 8)
                                        m_str_gr[ pos_gra_corr + uw4 + i ] |= 0x40;
                            }
                        }
                    }
                }
                else{
                    if(( ub2 < __num_rig ) && ( ub4 < __num_rig ))
                    {
                        int vol_pt_prec = ( 9 * ub3 );   /* int((vol*18)/8)*24   */
                        int vol_pt = vol_pt_prec + 9;
                        vol_pt_prec /= 4;
                        vol_pt /= 4;
                        vol_pt_prec *= 24;
                        vol_pt *= 24;
                        if( i22 == 2)
                        {
                            for(int i3 = vol_pt_prec; i3 < vol_pt; i3 += 24)
                                for(int i = 0; i < 24; i += 8 )
                                    m_str_gr[ pos_gra_corr + i + i3 ] |= 0x40;
                        }
                    }
                }
            }
        }
    }
} // fine funzione Plot_StdCurve_Siroky


/**
Verifica se deve inserire e dove, il punto syroki con il suo tratteggio
*/
void printermanager::Plot_Point_Siroky(byte __num_rig, byte __curve, unsigned short __flu )
{
    static unsigned short pos_gra_corr;

    if( __curve == 0 )
        pos_gra_corr = pos_gra_Qmed;	// curve sul grafico Q_ave
    else if( __curve == 1 )
        pos_gra_corr = pos_gra_Qmax;	// curve sul grafico Q_max quindi spostate di a_ave + 2 spaziettini

    int flow_pt = 240 - __flu * 8;                              /*y  240 pix =30ml/s gain =240/30=8; pix 0 =30ml/s pix 240=0ml/s*/

    if( ( flow_pt > 0 ) && ( __flu > 0 ) && ( m_max_vol < 500 ))
    {
        unsigned short uw4 = (unsigned short)( m_max_vol * 18 ) / 25;
        int vol_pt = (int)uw4;                             /*x  grap 1 46*8=368 pix =500ml gain =368/500=92/125; x0= pix 400 =0ml pix 368=500ml*/
        unsigned char ub4 = (byte)( flow_pt / 24);                     /*n_r=int(y/24)  Num riga*/
        unsigned char ub3 = (byte)( flow_pt % 24);                     /*y_st=y(mod)24  scostameto riga*/
        unsigned char ub2 = 0x80 >> (byte)( vol_pt % 8);             /*byt=x(mod)8   u=x_st*24+y_st*/
        uw4 = (unsigned short)( vol_pt / 8);               /*x_st=int(x/8)  n¦ byte */
        uw4 *= 24;          /*non fare uw4=vol_pt*3*/  /*x_st*24*/
        if( __num_rig >= ub4)                           /* riga grafica attiva point*/
        {                                         /*Pix(x,y) <=> P(u,byt,n_r)*/
            if( __num_rig == ub4)                        /* riga grafica appartiene point*/
            {
                for(int uw5 = ub3; uw5 < uw4; uw5 += 24) /*1200=(400/8)*24 =int(x0/8)*24*/
                    m_str_gr[ pos_gra_corr + uw5 ] |= (char)0xF0;         /* tratteggio da x0 a x     proiexione coordinata y*/
                /*area del punto*/
                for(int i3 = 16; i3 > ub3; i3 -= 8)
                {
                    m_str_gr[ pos_gra_corr + uw4 + i3] |= (char)ub2;
                    m_str_gr[(pos_gra_corr+1) + uw4 + i3] |= (char)ub2;
                    m_str_gr[(pos_gra_corr+2) + uw4 + i3] |= (char)ub2;
                    m_str_gr[(pos_gra_corr+3) + uw4 + i3] |= (char)ub2;
                }
                if( ub3 < 3)
                    Plot_Quadro_Siroky( 0, ub3 + 4, pos_gra_corr,vol_pt,uw4); /*fare quadratino ub2 si distrugge*/
                else
                    Plot_Quadro_Siroky( ub3 - (4-1),ub3 + 4, pos_gra_corr,vol_pt,uw4);
            }else
            {
                for(int i3 = 0; i3 < 24; i3 += 8)
                {              /*proiexione coordinata x*/
                    m_str_gr[ pos_gra_corr + uw4 + i3]  |= (char)ub2;
                    m_str_gr[(pos_gra_corr+1) + uw4 + i3] |= (char)ub2;
                    m_str_gr[(pos_gra_corr+2) + uw4 + i3] |= (char)ub2;
                    m_str_gr[(pos_gra_corr+3) + uw4 + i3] |= (char)ub2;
                }
                if( ( __num_rig == ( ub4 + 1 )) && ( ub3 > 20)){
                    Plot_Quadro_Siroky( 0, ub3 - (24-4), pos_gra_corr,vol_pt,uw4);       /*fare quadratino ub2 si distrugge*/
                }
            }
        }else{
            if( ( ( __num_rig + 1 ) == ub4 ) && ( ub3 < 3 ) )
                Plot_Quadro_Siroky( 21 + ub3, 24, pos_gra_corr,vol_pt,uw4);         /*fare quadratino ub2 si distrugge*/
        }
    }                                      /*non plot point fuori grafico*/
}


void printermanager::Plot_Quadro_Siroky(byte __ubstrt, byte __ubend, unsigned short __new_pos, int __vol_pt, unsigned char __uw4)
{
    unsigned char ub2 = 0x80 >> (byte)( __vol_pt % 8);             /*byt=x(mod)8   u=x_st*24+y_st*/
    unsigned short uw6 = (unsigned short)( ub2 * 256);
    unsigned short uw5 = ub2;
    for(int i3 = 0; i3 < 3; i3++){                            /*riempie +-3 pix dest sin dal punto*/
        uw6 |= ( uw6 >> 1);                               /*priempimento pi¨ usire a destra o sinis sforando nel altro byte*/
        uw5 |= ( uw5 << 1);
    }                                               /*riempie +-3 pix su giu dal punto*/
    ub2 = HIBYTE( uw6 ) | LOBYTE(uw5);                    /*if(num_rig==ub4  )&&(ub3<3)  ubstrt=0      ubend=ub3+4  */
    for(int i3 = __ubstrt; (( i3 < __ubend ) && ( i3 < 24)); i3++)     /*if(num_rig==ub4  )&&(ub3>=3) ubstrt=ub3-3  ubend=ub3+4  */
    {
        if( __vol_pt > 7)
            m_str_gr[ __uw4 + i3 - (24 - __new_pos) ] |= HIBYTE(uw5);  /*if(num_rig==ub4+1)&&(ub3>20) ubstrt=0      ubend=ub3-20 */

        m_str_gr[ __new_pos + __uw4 + i3] |= (char)ub2;                 /*if(num_rig+1==ub4)&&(ub3<3)  ubstrt=21+ub3 ubend=24     */
        if( __vol_pt < 352 )
            m_str_gr[ (24 + __new_pos) + __uw4 + i3] |= LOBYTE(uw6);/*riempimento pu= uscire dalla riga su o gi¨*/
    }
}


/**
stampa l'elenco dei dati di analidi delle curve
*/
void printermanager::Report_result()
{
    m_port->Pri_Font(1);
    m_port->Pri_mode(0x10);
    m_port->Pri_justif(F_center);
    QString flures = tr("Flowmetry Results");
    m_port->Pri_Str(strlen(flures.toLatin1().data()),flures.toLatin1().data(),1);

    m_port->Pri_justif(F_left);
    // tolgo l'accapo automatico della Pri_Rep_Lin e lo metto manuale
    m_port->Pri_Font(1);
    m_port->Pri_mode(0x00);
    if (m_modal_e == 2)	// il tempo di attesa è graficato solo se esame manuale
    {
        QString tempoAttesa = tr("Waiting Time ..............");
        Pri_Rep_Lin(tempoAttesa.toLatin1().data(),(short unsigned)((m_tem_att)*Fc_FLW),1,(char *)"s\n",    0);
    }
    if (m_flu_med > m_flu_max) // piccolo controllo per gestire flussi abnormali, tipici di prove da laboratorio poco furbe
        if (m_tem_flu < 30) // se la flussata è molto breve e intensa, l'algoritmo sbaglia e può risultare flu_med > flu_max
            m_flu_med = m_flu_max;

    QString message = tr("Maximum Flow Rate .........");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_flu_max * Fc_FLW ), 1, (char *)"ml/s\n", 0);
    message = tr("Average Flow Rate .........");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_flu_med * Fc_FLW ), 1, (char *)"ml/s\n", 0);
    message = tr("Time to Maximum Flow ......");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_tem_max * Fc_FLW ), 1, (char *)"s\n",    0);
    message = tr( "Time between 5% and 95% ...");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_tem_595 * Fc_FLW ), 1, (char *)"s\n",    0);
    message = tr("Flow Time .................");
    Pri_Rep_Lin(message.toLatin1().data(), ( int)( m_tem_flu * Fc_FLW ), 1, (char *)"s\n",    0);
    message = tr("Descent Time ..............");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_tem_dis * Fc_FLW ), 1, (char *)"s\n",    0);
    message = tr("Voiding Time ..............");
    Pri_Rep_Lin(message.toLatin1().data(), (int)( m_tem_svu * Fc_FLW ), 1, (char *)"s\n",    0);
    message = tr("Volume to Maximum Flow ....");
    Pri_Rep_Lin(message.toLatin1().data(), (int)m_vol_max , 0, (char *)"ml\n",   0);
    message = tr("Voided Volume .............");
    Pri_Rep_Lin(message.toLatin1().data(),m_vol_vuo,0,(char *)"ml\n",   0);
    // per ora il flus max corretto non lo metto perchè non ho la funzione radice quadrata
    //"Corrected Maximum Flow ...."Pri_Rep_Lin((char *)msg_flu_cor[Language_selected],(rep.flu_cor)*Fc_FLW,1,"ml½/s",1);    //½=1/2 apice
    message = tr( "Flow Acceleration .........");
    Pri_Rep_Lin(message.toLatin1().data(),(short unsigned)((m_flu_acc)*Fc_FLW),2,(char *)"ml/s^2\n",0);    //²=2 apice
}

/**
      Stampa una linea del Report con :
         -   "str_des"  =  Stringa Descrizione
         -   "rep_dat"  =  Dato del Report
         -   "num_dec"  =  Numero di Decimali
         -   "str_udm"  =  Stringa Unita' di Misura
         -   "flag_lf"  =  Flag LF Finale
*/
void printermanager::Pri_Rep_Lin(char *__str_des, int __rep_dat, byte __num_dec, char *__str_udm, byte __flag_lf)
{
    static short d_strlen;
    char str2[40];
    if(__rep_dat < 0)
        sprintf(str2," n.c.");
    else
    {
        if (__num_dec == 0)
            sprintf(str2,"%5u",__rep_dat);
        else
        {
            int uw = 1;
            for (int ub = 0; ub < __num_dec; ub++)
                uw *= 10;
            sprintf(str2,"%u.%1u", __rep_dat /uw, __rep_dat % uw);
        }
    }
    sprintf(m_str_gr, "  %s", __str_des);		// allontano la riga dal bordo sinistro di 2 spazi

    strcat(m_str_gr," :");
    char space_bar[21]="                    ";
    if (strlen(str2) > 5) {// quando esami lunghissimi, si può arrivare a lavorare con valori in migliaia di secondi e col decimale la stringa è lunga 6
        d_strlen = strlen(str2) - 5; // termine di correzione in funzione della lunghezza della stringa str2
        strcat(m_str_gr,space_bar+(15-d_strlen)+strlen(str2));
    } else
        strcat(m_str_gr,space_bar+15+strlen(str2));
    strcat(m_str_gr,str2);
    strcat(m_str_gr,"  ");
    strcat(m_str_gr,__str_udm);
    m_port->Pri_Str(strlen(m_str_gr),m_str_gr,__flag_lf);
}
