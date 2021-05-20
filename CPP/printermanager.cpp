
#include "printermanager.h"
#include <QPainter>
#include "udpmsgs.h"
#include <QFontDatabase>

const char *str_label_time[] = {
//  "0    6   12    18    24   30s",
//  "0    9   18    27    36   45s",
//  "0   12   24    36    48   60s",
    "0 0:15 0:30  0:45  1:00  1:15",
    "0 0:18 0:36  0:54  1:12  1:30",
    "0 0:21 0:42  1:03  1:24  1:45",
    "0 0:24 0:48  1:12  1:36  2:00",
    "0 0:27 0:54  1:21  1:48  2:15",
    "0 0:30 1:00  1:30  2:00  2:30",
    "0 0:36 1:12  1:48  2:24  3:00",
    "0 0:42 1:24  2:06  2:48  3:30",
    "0 0:48 1:36  2:24  3:12  4:00",
    "0 0:54 1:48  2:42  3:36  4:30",
    "0 1:00 2:00  3:00  4:00  5:00",
    "0 1:12 1:24  3:36  4:48  6:00",
    "0 1:24 2:48  4:12  5:36  7:00",
    "0 1:36 3:12  4:48  6:24  8:00",
    "0 1:48 3:36  5:24  7:12  9:00",
    "0 2:00 4:00  6:00  8:00 10:00",
    "0 2:12 4:24  6:36  8:48 11:00",
    "0 2:24 4:48  7:12  9:36 12:00",
    "0 2:36 5:12  7:48 10:24 13:00",
    "0 2:48 5:36  8:24 11:12 14:00",
    "0 3:00 6:00  9:00 12:00 15:00",/* 15*60=900*/
    "0 3:12 6:24  9:36 12:48 16:00",
    "0 3:24 6:48 10:12 13:36 17:00",
    "0 3:36 7:12 10:48 14:24 18:00",
    "0 3:48 7:36 11:24 15:12 19:00",
    "0 4:00 8:00 12:00 16:00 20:00",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
};


printermanager::printermanager(QString __namefile, QObject *parent) : QObject(parent)
{
    m_file = NULL;
    m_namefile    = __namefile;
    m_namefilePrn = __namefile.remove("_copy").remove(".pic").append(".prn");

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

    m_chVol = -1;
    m_chFlw = -1;
    m_chEmg = -1;

    m_printFirstHeader = "";
    m_printSecondHeader = "";

    m_printSiroky = false;
    m_printLiverpool = false;
    m_printMiskolc = false;

    m_rangeChVV = -1;
    m_rangeChEMG = -1;
    m_rangeChQ = -1;

    m_resultBm_w = 103*8; // 103 bytes * 8 bit
    m_resultBm_h = 300;
    m_bitmapSiroky.   resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapLiverpool.resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapSiroky.fill(0);
    m_bitmapLiverpool.fill(0);
    m_bitmapMiskolc.resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapMiskolc.fill(0);

    buffer_emg = NULL;
    buffer_flw = NULL;
    buffer_vol = NULL;

    m_tem_att = -999;

    qDebug("new imageBm");
    imageQsz = QSize(832, 6000);
    imageBm = QImage(imageQsz, QImage::Format_Mono);
    imagePainter = new QPainter(&imageBm);
    imageInit();
    qDebug("fine init");
}

printermanager::~printermanager()
{
    qDebug("uscita printermanager");
}

void printermanager::imageFontInit(T_Font &font, QString label, int size, QFont::Weight weight, bool fixedPitch)
{
    QFont tmp = imagePainter->font();

    font.label  = label;
    font.size   = size;
    font.weight = weight;

    font.qf     = QFont(font.label, font.size, font.weight);
    font.qf.setFixedPitch(fixedPitch);

    imagePainter->setFont(font.qf);
    font.charH = imagePainter->fontMetrics().lineSpacing();
    font.charW = imagePainter->fontMetrics().width('A');

    imagePainter->setFont(tmp);
}

void printermanager::imageSetFont(T_Font &font)
{
    imagePainter->setFont(font.qf);
    tf_current = font;
}

void printermanager::imageTestFont()
{
    int y = imagePt.ry();
    imagePainter->drawLine(0,y, 800,y);
    imageSetFont(tf_base);       y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_header22);   y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_header28);   y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_header20);   y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_graphLand);  y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_graphPortr); y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_infoLabel);  y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imageSetFont(tf_infoValue);  y += tf_current.charH + 5; imagePainter->drawText(5, y, tf_current.label);
    imagePainter->drawLine(0,y, 800,y);
    imagePt.ry() += y;
}

void printermanager::imageInit()
{
    qDebug("imageInit()");

    imageBm.fill(Qt::white);
    imagePainter->fillRect(QRect(QPoint(0,0), imageQsz), Qt::white);

    // fontInit() richiede imagePainter impostato
    imageFontInit(tf_base,       "Bitstream Vera Sans Mono", 10, QFont::Thin);
    imageFontInit(tf_header22,   "Bitstream Vera Sans Mono", 22, QFont::Medium);
    imageFontInit(tf_header28,   "Bitstream Vera Sans Mono", 28, QFont::Bold);
    imageFontInit(tf_header20,   "Bitstream Vera Sans Mono", 20, QFont::Normal);
    imageFontInit(tf_graphPortr, "Bitstream Vera Sans Mono",  8, QFont::Thin);
    imageFontInit(tf_graphLand,  "Bitstream Vera Sans Mono",  8, QFont::Thin);
    imageFontInit(tf_infoLabel,  "Bitstream Vera Sans Mono", 12, QFont::Bold);
    imageFontInit(tf_infoValue,  "Bitstream Vera Sans Mono", 12, QFont::Normal);
    imageFontInit(tf_calDate,    "Bitstream Vera Sans Mono",  9, QFont::Bold);

    tf_header20.qf.setUnderline(true);

    imagePainter->setFont(tf_base.qf);

    imagePt = QPoint(0, 0);
//    imageTestFont();
}

void printermanager::imagePrintPixLine(uchar *p, int sz)
{
    uchar buf[256];
    buf[0] = 0x1b;  //  ESC;
    buf[1] = '*';   // 0x2A;
    buf[2] = 0;     // n1   siz
    buf[3] = 0;     // n2   siz
    buf[4] = 0;     // n3   siz
    buf[5] = 0;     // n4   option
    buf[6] = 0;     // n5	scrive a n5 byte dal bordo
    buf[7] = 0;     // n6	larghezza dots in bytes = sz

    // ottimizzazione bytes vuoti a sinistra
    // per ridurre il tempo di trasmissione
    int skip = 0;
    while(p[skip] == 0)
        skip++;
    p      += skip;
    buf[6]  = skip;
    sz     -= skip;
    buf[2] = buf[7] = sz;

    memcpy(buf+8, p, sz);
    imagePrintStr((char *)buf, 8+sz);
}

void printermanager::imagePrintForward(int __dotlines)
{
    /* fa avanzare di "dotlines" righe la carta*/

    char pri_str[] = { 0x1b, 'J', (char) __dotlines };
    imagePrintStr(pri_str, 3);
}

void printermanager::imagePrintStr(char *__str, int __str_len)
{
    static char trailer[1] = { 0x03 };
    char header[6] = { 0x5a, static_cast<char>(0xa5), static_cast<char>(0xa5), 0x5a, static_cast<char>(__str_len & 0xff), static_cast<char>((__str_len >> 8) & 0xff) };

    m_file->write(header, sizeof(header));
    m_file->write(__str, __str_len);
    m_file->write(trailer, sizeof(trailer));
}

void printermanager::imagePrint()
{
#ifdef PICOFLOW

    m_file = new QFile(m_namefilePrn);
    if(m_file->exists())
        m_file->remove();
    m_file->open(QIODevice::WriteOnly);
    qDebug() << "file aperto in scrittura:" << m_namefilePrn;

    uchar   tmp[256];
    int lastNotEmpty = 0;
    int h = imagePt.ry() < imageBm.height() ? imagePt.ry() : imageBm.height();
    int w = imageBm.width();
    int wBytes = w / 8;
    int emptyLines = 0;
    for (int y = 0; y < h; ++y) {
        uchar * dst = tmp;
        uchar * pbm = imageBm.scanLine(y);
        // immagine invertita
        for(int i = 0; i < wBytes; i++)
            *dst++ = *pbm++ ^ 0xff;
        // ottimizzazione spazi bianchi a destra
        // ottimizzazione per righe vuote
        int rightZeroED = 0;
        for(int x = wBytes - 1; (x >= 0) && (tmp[x] == 0); x--)
            rightZeroED++;
        if(rightZeroED == wBytes)   // linea vuota
            emptyLines++;
        else {
            if(emptyLines > 0) {    // linee vuote precedenti
                imagePrintForward(emptyLines);
                emptyLines = 0;
            }
            imagePrintPixLine(tmp, wBytes-rightZeroED);
            lastNotEmpty = y;
        }
    }
    if(emptyLines > 0) {    // linee vuote precedenti
        imagePrintForward(emptyLines);
        emptyLines = 0;
    }
    qDebug("linee effettive:%d", lastNotEmpty);
    imageInit();    // immagine azzerata

    // riga di strappo
    for(int i = 0; i < 103; i++)
        tmp[i] = 0xcc;
    imagePrintPixLine(tmp, 103);
    // fa avanzare la carta 27 mm per consentire lo strappo
    imagePrintForward((int) (27 / 0.125));

    qDebug() << "file chiuso in scrittura" << m_file->fileName();
    m_file->close();
    delete m_file;
    m_file = NULL;

    qDebug("fine print: sendSup()");
//    udpConn.sendPrn("print:" + m_namefilePrn.toLatin1());   // diretto
    udpConn.sendSup("Print:" + m_namefilePrn.toLatin1());   // gateway

#endif
}

void printermanager::imageText(QString txt, int fontSize, bool restoreFont)
{
    QFont savedFont, tfont;

    if(fontSize > 0) {
        tfont = savedFont = imagePainter->font();
        tfont.setPointSize(fontSize);
        imagePainter->setFont(tfont);
    }

    int deltaY = imagePainter->fontMetrics().lineSpacing();
    imagePt.ry() += deltaY;
    imagePainter->drawText(imagePt, txt);
//    imagePt.ry() += deltaY;

    if((fontSize > 0) && restoreFont)
        imagePainter->setFont(savedFont);
}

void printermanager::imageGraph(QString head, QString baset, double xscale, int maxL, int maxR, double *bufL, double *bufR)
{
    qDebug()<<"head,baset,xscale:"<<head<<baset<<xscale;
    QStringList baseLst = baset.split(" ", QString::SkipEmptyParts);
    imagePt.rx() = 5;

    QFont savedFont, tfont;
    tfont = savedFont = imagePainter->font();
    imageText(head, 12, true);
    tfont.setPixelSize(15);
    imagePainter->setFont(tfont);
    {
        imagePt.rx() = 5;
        imagePt.ry() += 10;

        int digitsW = imagePainter->fontMetrics().width("9999");
        int left = (digitsW + 1);
        int h = 240;
        int w = (imageQsz.width()-imagePt.rx()) - (left + digitsW+5 /* destra */ ); // logical width
        qDebug("digitsW:%d w:%d",digitsW,w);

        QPoint lt(imagePt.rx() + left, imagePt.ry());         // leftTop
        QPoint lb(imagePt.rx() + left, imagePt.ry() + h);     // leftBottom
        imageRectFlw = QRect(lt, QSize(w, h));
        imagePainter->drawRect(imageRectFlw);

        // griglia
        imagePainter->setPen(Qt::DotLine);
        for(int i = 1; i < 10; i++) {
            int x = lb.x() + i * (w / 10);
            imagePainter->drawLine(x,lb.y(), x, lb.y()-h);
        }
        for(int i = 0; (i <= 10) && (i/2 < baseLst.size()); i += 2) {
            int x = lb.x() + i * (w / 10);
            QString txt =  baseLst.at(i/2);
            int txtsz = imagePainter->fontMetrics().width(txt) / 2;
            imagePainter->drawText(x-txtsz, lb.y()+20, txt);
        }
        for(int i = 1; i <= 5; i++) {
            int y = lb.y() - i * (h / 5);
            if(i < 5)
                imagePainter->drawLine(lb.x(),y, lb.x()+w,y);
            if(maxL > 0)
                imagePainter->drawText(QPoint(         5, y+10), QString::asprintf("%4d", (maxL * i) / 5));
            if(maxR > 0)
                imagePainter->drawText(QPoint(lb.x()+w+1, y+10), QString::asprintf("%4d", (maxR * i) / 5));
        }
        imagePainter->setPen(Qt::SolidLine);
        imagePt = lb;

//        xscale *= (w / 752.0);
        if(m_printMode == PORTRAIT_MODE) {
            const int f = 103*8;  // max physical pixel
            if (w < f)
                w = f;
            xscale = ((double)(NUMOF_X_PRINT_DOTS) / w);
        }
        QPoint * points = new QPoint[w];
        qDebug()<<"w,m_num_sam:"<<w<<m_num_sam<<" --> xscale:"<<xscale;
        imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxL, bufL, h);
        if(maxR > 0)
            imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxR, bufR, h);
        delete points;
    }
    imagePainter->setFont(savedFont);

    imagePt.ry() += 50;
}

void printermanager::imageGraphSingle(QPoint leftBottom, QPoint *pts, int npts, double kx, double ky, double *bufV, int maxy)
{
    qDebug()<<"npts,kx:"<<npts<<kx;
    for (int ix = 0; ix < npts; ix++ ) {
        int x  = leftBottom.rx() + (int)(kx * ix);
        int ty = (int)(ky * bufV[ix]);
        if(ty > maxy)
            ty = maxy;
        int y = leftBottom.ry() - ty;
        pts[ix] = QPoint(x, y);
    }
    imagePainter->drawPolyline(pts, npts);
}

/**
Stampa automatica alla fine dell'esame: al momento della pressione dello stop, i dati sono analizzati, salvati e poi mandati in stampa.
*/
void printermanager::print(QString __datiCalib)
{
    m_datiCalib = __datiCalib;
    qDebug() << m_namefilePrn << "calib:" << __datiCalib;

    //dati paziente e esame
    m_dfm->Open();
    m_dfm->GetParameters();
    //paziente
    QString dataPaz = m_dfm->GetPatient();
   // qDebug()<<"DATI PAZIENTE"<<dataPaz;
    m_surname = dataPaz.section(";", 0, 0);
    m_name = dataPaz.section(";", 1, 1);
    m_dateofbirth = dataPaz.section(";", 2, 2);
    m_sex = dataPaz.section(";", 12, 12);
    m_ID = dataPaz.section(";", 3, 3);
    m_operatore = dataPaz.section(";",7,7);
    m_commenti = dataPaz.section(";",11,11);
    //esame
    m_numchan = m_dfm->GetChanNum();
    m_numTest = m_dfm->GetTestNum();

    QDate dateExam = QDate(1899, 12, 30).addDays(m_dfm->GetDataEsame());
    m_dateofexam = dateExam.toString("dd/MM/yyyy");

    QTime time = QTime(0,0,0).addSecs(m_dfm->GetStartTime());
    m_timeofstart = time.toString();	//ora inizio esame

    m_durata = m_dfm->GetDuration() / 1000; //in sec

     // discrimina se esame veloce oppure completo di dati paziente (servira nel report di stampa)
    if (m_name == "Anonymous")
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

    imagePrint();
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
    qDebug("fine pri_rep_review()");
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

/**
la stampa e prevista comunque sempre dopo il review, si avvale dei dati di review, cosi usiamo
gli stessi comandi sia che ci si trovi a fine esame, sia che sia un review di esamei in MMC.
Allora al posto della struttura Patient_Data, ci metiamo DatiPaziente.
questa funzione gestisce tutta la stampa del report, sia in modalita portrait che landscape
*/
#include "QApplication"
void printermanager::Pri_Rep(double xscale)
{
    qDebug("xscale:%f",xscale);
    Intest();       // Intestazione
    Report_data();  // scrive i dati del paziente e lo spazio per le note manuali

    // Grafico FLW + VOL ed eventualmente EMG
    if(m_printMode == PORTRAIT_MODE) {			// grafico trasversale con numero di punti fisso
        qDebug("Pri_Rep Portrait");
        Report_flw(xscale);	// stampa grafici di volume e flusso
        if(m_emgPresent)
            Report_emg(xscale);						// EMG
    }
    else {              // print_mode = LANDSCAPE_MODE - grafico longitudinale, con lunghezza legata alla lunghezza dell'esame
        qDebug("Pri_Rep Landscape");
        m_max_y = Calc_Max_Flw();			// fondo scala del flusso
        Calc_Max_RealReport_rel2(m_num_sam);
        Report_Real_Time(/*xscale*/ 1.0);
    }
    qDebug() << "dopo Report XXX";

    if(m_printLiverpool)
        Report_BitMap(1);           // Grafico Liverpool

    if(m_printSiroky && m_sex != "F")
        Report_BitMap(0);            // Grafico Siroky, solo se paziente maschio oppure generico (sesso non indicato)

    if (m_printMiskolc)
        Report_BitMap(2);

    // 	Risultati dell'esame ricavati dall'analisi semplificata, implementata nel firmware
    Report_result();    // scrive in elenco i dati calcolati dall'analisi dell'esame

    //	Scrive i dati riguardanti versione firmware e date/ora ultima calibrazione
    //Report_dati_macchina(numCurve);	// scrive data e ora della stampa e la versione attuale del FW

    qDebug() << "Pri_Rep FINE file:" << m_namefile;
}

/**
Intestazione del report, con intestazione clinica, logo and so on
*/
void printermanager::Intest()
{
    QPoint rb = QPoint(imageBm.width()-1, imagePt.ry());
    imagePt.rx() = 8;

    imageSetFont(tf_header22);
    rb.ry() = imagePt.ry() + tf_current.charH;
    imagePainter->drawText(QRect(imagePt, rb), Qt::AlignCenter, m_printFirstHeader);
    imagePt.ry() = rb.y() + 10;

    imageSetFont(tf_header28);
    rb.ry() = imagePt.ry() + tf_current.charH;
    imagePainter->drawText(QRect(imagePt, rb), Qt::AlignCenter, m_printSecondHeader);
    imagePt.ry() = rb.y() + 10;

    imageSetFont(tf_header20);
    rb.ry() = imagePt.ry() + tf_current.charH;
    imagePainter->drawText(QRect(imagePt, rb), Qt::AlignCenter, tr("UROFLOWMETER"));
    imagePt.ry() = rb.y() + 10;

    imagePt.ry() += 80;
}

/**
Sono stampati i dati del report paziente, nome, cognome, data di nascita, sesso, data esame, ....
*/
void printermanager::Report_row(QString label, QString value)
{
    imagePt.ry() += tf_infoLabel.charH;

    imageSetFont(tf_infoLabel);
    imagePainter->drawText(imagePt, label);

    imageSetFont(tf_infoValue);
    int x = imagePt.x() + tf_infoLabel.charW * label.size();
    int y = imagePt.y();
    imagePainter->drawText(x, y, value);
}
void printermanager::Report_data()
{
    imagePt.rx() = 5;

    const char puntini[] = ". . . . . . . . . . . . . . . . . . . . .";

    Report_row(tr("Test Number ....: "), QString::number(m_numTest));
    Report_row(tr("Test Date ......: "), m_dateofexam);
    Report_row(tr("Surname ........: "), m_test_type ? puntini : m_surname);
    Report_row(tr("Name ...........: "), m_test_type ? puntini : m_name);
    Report_row(tr("Birth Date .....: "), m_test_type ? puntini : m_dateofbirth);
    Report_row(tr("Gender .........: "), m_test_type ? puntini : m_sex);
    Report_row(tr("Operator .......: "), m_test_type ? puntini : m_operatore);
    Report_row(tr("Comments .......: "), m_test_type ? puntini : m_commenti);

    int deltaY = tf_infoLabel.charH;
    int deltaX = tf_infoLabel.charW * tr("Comments .......: ").size();

    for (int i = 0; i < 3; i++)	{	// tre righe vuote per eventuali commenti
        imagePt.ry() += deltaY;
        imagePainter->drawText(imagePt.x()+deltaX, imagePt.y(), puntini);
    }


    // scritta relativa al tipo di modalita
    imageSetFont(tf_header22);
    imagePt.ry() += tf_header22.charH * 2;
    imagePainter->drawText(imagePt, (m_modal_e == 2) ? tr("MANUAL   MODALITY") : tr("AUTOMATIC  MODALITY "));
    imagePt.ry() += tf_header22.charH;
}

/**
stampa del grafico di flusso/volume nella versione a grafici in portrait mode
*/
void printermanager::Report_flw(double xscale)
{
    Calc_Max(xscale);	// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) cosi da individuare il giusto fondoscala

    imageGraph(tr("  Q (ml/s)             Flowmetry              Vol (ml)"),
               str_label_time[m_i_max_x],
               xscale,
               m_max_y,
               m_max_y_gr2,
               buffer_flw,
               buffer_vol
               );
}

/**
stampa del grafico di EMG nella versione a grafici in portrait mode
*/
void printermanager::Report_emg(double xscale)
{
    Calc_Max_EMG();
    qDebug() << "m_max_emg:" << m_max_emg << "m_max_y:" << m_max_y;

    QString EMGTitle = " EMG (uV)             "+tr("EMG Diagram");
    imageGraph(EMGTitle,
               str_label_time[m_i_max_x],
               xscale,
               m_max_y,
               0,
               buffer_emg,
               NULL
               );
}

/**
GRAFICI IN LANDASCAPE MODE in accordo con specifiche ISC (indifferentemente dal fondoscala,
l'area sottesa al grafico del flusso, deve essere proporzionale al valore di flusso, quindi anche l'asse
dei tempi si adegua al fondoscala.

disegna il/i grafici. prima costruisce il/gli asse/i delle ordinate della parte sinistra del grafico, dove saranno mostrati i fondoscala di flusso ed emg,
con 3 valori intermedi (1/4 FS, 2/4 FS, 3/4 FS).
*/
void printermanager::Report_Real_Time(double xscale)
{
    int residuo = (m_num_sam % (10*FREQ_ACQ));
    int boxWincr = (residuo ? ((10*FREQ_ACQ) - residuo) : 0);
    const int boxW = m_num_sam + boxWincr + 10;
    const int dotXtratt = 4;
    QPoint psave = imagePt;
    imagePt.rx() = tf_graphLand.charH + dotXtratt + 5;  // spazio x label in stampa landscape
    imagePt.ry() += 10;
    imagePainter->save();
    imagePainter->translate(imagePt);
    imagePainter->rotate(90);

    int txtW = tf_graphLand.charW * QString("9999 ml/s").size() + 10;
    imageSetFont(tf_graphLand);

    QPoint * points = new QPoint[m_num_sam];

    imagePainter->drawRect(txtW,-800, boxW*xscale,800);
    Report_Real_TimeSingle(    "Flow",   points, xscale, dotXtratt, boxW, 400,  400,  5,  m_max_y,   buffer_flw, " mL/s");
    if(m_emgPresent) {
        Report_Real_TimeSingle("Volume", points, xscale, dotXtratt, boxW, 160,  240,  3,  m_max_vol, buffer_vol,   " mL");
        Report_Real_TimeSingle("Emg",    points, xscale, dotXtratt, boxW,   0,  160,  2,  m_max_emg, buffer_emg,   " μV");
    }
    else
        Report_Real_TimeSingle("Volume", points, xscale, dotXtratt, boxW,   0,  400,  5,  m_max_vol, buffer_vol,   " mL");

    delete points;

    imagePt.rx() = txtW+m_num_sam*xscale+3;

    imagePainter->restore();
    psave.ry() += imagePt.x();
    imagePt = psave;

    imagePt.ry() += 90;
    qDebug("fine Report_Real_Time()");
}

void printermanager::Report_Real_TimeSingle(QString msg, QPoint * points, double xscale, int dotXtratt, int boxW, int y0, int n_dots, int n_label, int max_val, double * buffer, QString txt)
{
    int nl  = n_label;
    int nd  = (n_dots / nl);
    int cw  = tf_graphLand.charW;
    int ch  = tf_graphLand.charH;
    int txtW = cw * QString("9999 ml/s").size() + 10;               // larghezza label piu lunga
    int freqXscale = FREQ_ACQ * 0.8;

    imagePainter->drawText(txtW-20, -(y0), "0");                    // stampo lo zero senza udm
    imagePainter->drawText(txtW+5, -(y0+n_dots-ch-2), msg);         // stampo lo zero senza udm
    imagePainter->setPen(Qt::DotLine);
    for(int i = 1; i <= nl; i++) {
        int val = (max_val * i) / nl; // NB: ( *i)/nl per evitare probl.arrotondamento con i == nl
        QString label = QString::number(val) + txt;
        int y = y0 + nd * i;
        imagePainter->drawText((txtW - 10) - (cw * label.size()), -(y - ch), label);
        imagePainter->drawLine(txtW, -y, txtW + boxW*xscale, -y);   // tratteggio orizz. allineato a label
    }
    imagePainter->setPen(Qt::SolidLine);
    imagePainter->drawLine(txtW, -y0, txtW + boxW*xscale, -y0);     // riga orizz. dello 0

    if(y0 == 0) {
        for(int i = freqXscale; i < boxW; i += freqXscale) {
            int x = txtW + i;
            imagePainter->drawLine(x, -(y0), x, -(y0 - dotXtratt));     // trattini verticali dei secondi
        }
        for(int i = (10*freqXscale); i < boxW; i += (10*freqXscale)) {
            int x = txtW + i;
            imagePainter->drawLine(x, -(y0), x, -(y0 - dotXtratt*2));   // trattini verticali multipli 10 secondi

            imagePainter->setPen(Qt::DotLine);
            imagePainter->drawLine(x, -(y0), x, -(y0 + 800));           // tratteggio vert. allineato a label
            imagePainter->setPen(Qt::SolidLine);

            int ndec = (i / 0.8) / 10;                                          // label multipli 10 secondi
            QString label = QString::number(ndec / 60) + ":" + QString::number(ndec % 60);
            x -= (tf_graphLand.charW * label.size()) / 2;
            int y = (y0 - dotXtratt - ch);
            imagePainter->drawText(x, -y, label);
        }
        int x = txtW + m_num_sam ;
        imagePainter->setPen(Qt::DashLine);
        imagePainter->drawLine(x, -(y0), x, -(y0 + 800));               // tratteggio fine esame
        imagePainter->setPen(Qt::SolidLine);
    }

    imageGraphSingle(QPoint(txtW,-y0), points, m_num_sam, 1.0, n_dots/(double)max_val, buffer, n_dots);
}

/**
Stampa dei nomogrammi
*/
void printermanager::Report_BitMap(int __nomo) //0 = Siro; 1 = Liver; 2 = Misk
{
    qDebug("inizio pr bitm");
    int     sz;
    char  * p;

    if (__nomo == 0) { //siroky
        sz = m_bitmapSiroky.size();
        p = m_bitmapSiroky.data();
    }
    else if (__nomo == 1) { //liverpool
        sz = m_bitmapLiverpool.size();
        p = m_bitmapLiverpool.data();
    }
    else { //nomo = 2 pediatrico
        sz = m_bitmapMiskolc.size();
        p = m_bitmapMiskolc.data();
    }

    int szchunk = 103;  // 824/8
    int row = imagePt.ry();
    for(int s = 0; s < sz; ) {
        for(int i = 0; i < 103*8; i++)
            if(p[i / 8] & (1 << (7 - (i & 7))))
                imagePainter->drawPoint(i, row);
        p += szchunk;
        s += szchunk;
        row++;
    }
    imagePt.ry() = row + 50;

    qDebug("fine pr bitm imagePt.ry():%d", imagePt.ry());
}

/**
stampa l'elenco dei dati di analisi delle curve
*/
void printermanager::Report_result()
{
    imagePt.rx() = 8;

    imageSetFont(tf_header22);
    imagePt.ry() += tf_header22.charH;
    imagePainter->drawText(imagePt, tr("Flowmetry Results"));
    imagePt.ry() += tf_header22.charH;

    QStringList txstrList;
    if (m_modal_e == 2) {     // il tempo di attesa e' graficato solo se esame manuale
        txstrList.append(tr("Waiting time ................")); txstrList.append(QString::asprintf(" : %5.1f s", m_tem_att));
    }

    if (m_flu_med > m_flu_max) // piccolo controllo per gestire flussi abnormali, tipici di prove da laboratorio
        if (m_tem_flu < 30) // se la flussata e molto breve e intensa, l'algoritmo sbaglia e puo risultare flu_med > flu_max
            m_flu_med = m_flu_max;

    txstrList.append(tr("Maximum flow rate ...........")); txstrList.append(QString::asprintf(" : %5.1f ml/s"      , m_flu_max));
    txstrList.append(tr("Average flow rate ...........")); txstrList.append(QString::asprintf(" : %5.1f ml/s"      , m_flu_med));
    txstrList.append(tr("Time to maximum flow ........")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_max));
    txstrList.append(tr("Time between 5% and 95% .....")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_595));
    txstrList.append(tr("Flow time ...................")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_flu));
    txstrList.append(tr("Descent time ................")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_dis));
    txstrList.append(tr("Voiding time ................")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_svu));
    txstrList.append(tr("Volume to maximum flow ......")); txstrList.append(QString::asprintf(" : %5.1f ml"        , m_vol_max));
    txstrList.append(tr("Voided Volume ...............")); txstrList.append(QString::asprintf(" : %5.0f ml"        , (double) m_vol_vuo));
    txstrList.append(tr("Corrected maximum flow ......")); txstrList.append(QString::asprintf(" : %5.1f ml^(1/2)/s", m_cQ     ));
    txstrList.append(tr("Flow acceleration ...........")); txstrList.append(QString::asprintf(" : %5.1f ml/s^2"    , m_flu_acc));    //?=2 apice
    txstrList.append(tr("Maximum contraction speed ...")); txstrList.append(QString::asprintf(" : %5.1f mm/s"      , m_vDetMax));
    txstrList.append(tr("Residual volume .............")); txstrList.append(QString::asprintf(" : %5.0f ml"        , (double) m_resVol ));

    for(int i = 0; i < txstrList.size(); i += 2) {
        imagePt.rx() = 8;
        imagePt.ry() += tf_infoLabel.charH;
        imageSetFont(tf_infoLabel); imagePainter->drawText(imagePt, txstrList.at(i));        
        imagePt.rx() = tf_infoLabel.charW *txstrList.at(i).size();
        imageSetFont(tf_infoValue); imagePainter->drawText(imagePt, txstrList.at(i+1));
    }

    imagePt.rx() = 8;
    imagePt.ry() += tf_infoLabel.charH;
    imagePainter->drawLine(imagePt, QPoint(imageBm.width()-1-8, imagePt.ry()));

    QString now;
    now = "<< " + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss") + " Rev: " + m_versione + " >>";
    imageSetFont(tf_calDate);
    imagePt.ry() += 1.5 * tf_calDate.charH;
    imagePt.rx() = (102*8 - tf_calDate.charW *now.size()) / 2;
    imagePainter->drawText(imagePt, now);
    now = "<< " + tr("Calibration date") + ": " + m_datiCalib.replace(".weight", "") + " >>";
    imagePt.ry() += tf_calDate.charH;
    imagePt.rx() = (102*8 - tf_calDate.charW *now.size()) / 2;
    imagePainter->drawText(imagePt, now);

    imagePt.rx() = 8;
    imagePt.ry() += tf_calDate.charH;
    imagePainter->drawLine(imagePt, QPoint(imageBm.width()-1-8, imagePt.ry()));
    imagePt.ry() += tf_calDate.charH;
}

#define NCOLORS 16*16*16
inline uint16_t decimazioneColore(uint32_t rgb24)
{
    // conversione da RGB 8*3 = 24 bit a RGB 4*3 = 12 bit
    // ignorati i 4 bit bassi di ogni colore * ridurre le sfumature ad un colore di base
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
    bool isMisk = __nome.startsWith("Pedi");

    QByteArray *cur_bitmap;
    if (isSiro && m_printSiroky)
        cur_bitmap = &m_bitmapSiroky;
    else if (isLive && m_printLiverpool)
        cur_bitmap = &m_bitmapLiverpool;
    else if (isMisk && m_printMiskolc)
        cur_bitmap = &m_bitmapMiskolc;
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

/**
Calcola "i_max_x" , "max_x" e "max_y"
*/
void printermanager::Calc_Max(double xscale)
{
    m_max_emg = 0;
    m_max_vol = buffer_vol[0];
    for (int i = 0; i < m_num_sam; i++)	{	// cerco il massimo del buffer volume
        double v = buffer_vol[i];
        if (v > m_max_vol)
            m_max_vol = v;
    }

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

    if (m_rangeChQ == -1) //auto-range abilitato
    {
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
            m_max_y = 100;
    }
    else //auto-range disabilitato
        m_max_y = m_rangeChQ;

    if (m_rangeChVV == -1)//auto-range abilitato
        m_max_y_gr2 = ((long) ((m_max_vol + 10) / 100) + 1) * 100;	// 05 dicembre
    else //auto-range disabilitato
        m_max_y_gr2 = m_rangeChVV;
}

void printermanager::Calc_Max_EMG()
{
    for (int i = 0; i < m_num_sam; i++)	{	// cerco il massimo del buffer emg
        double v = buffer_emg[i];
        if (v > m_max_emg)
            m_max_emg = v;
    }
    qDebug() << "m_max_emg:"<<m_max_emg;

    static int v_max_y[] = {
                            50,   100,   150,   200,   250,   300,   350,   400,   450,
                           500,   550,   600,   650,   700,   750,   800,   850,   900,   950,
                          1000,  1100,  1200,  1300,  1400,  1500,  1600,  1700,  1800,  1900,
                          2000,  2250,  2500,  2750,  3000,    -1
                        };


    //auto-range abilitato
    if (m_rangeChEMG == -1) {
        for(int i = 0; v_max_y[i] > 0; i++)
            if(m_max_emg <= v_max_y[i]) {
                m_max_y = v_max_y[i];
                qDebug() << "m_max_y:"<<m_max_y;
                return;
            }
    }
    else if (m_rangeChEMG < 3250) //auto-range disabilitato
    {
        m_max_y = m_rangeChEMG;
        qDebug() << "m_max_y autono:"<<m_max_y;
        return;
    }

    m_max_y = 3250;
    return;
}

/**
Nella stampa del report tipo reale, individuo il fondoscala del flusso, parametro che serve per adattare i valori
degli array dati volume e emg (e anche flusso)
*/
long printermanager::Calc_Max_Flw()
{
    long flw_max = ((((long)m_flu_max)/10) + 1) * 10;	// decina minima superiore al valore max

    // FS del grafico del flusso	// puo assumere i valori 25, 50, 75, 100
    if(flw_max <= 25)
        flw_max = 25;
    else
        flw_max = 50;
//    else if(flw_max <= 50)
//        flw_max = 50;
//    else if(flw_max <= 75)
//        flw_max = 75;
//    else //if(flw_max <= 160)
//        flw_max = 100;

    if (m_rangeChQ > 0 && m_rangeChQ < 50) //auto-range disabilitato
        flw_max = m_rangeChQ;

    return flw_max;
}

/**
Nella stampa del report tipo reale, qui si cercano i massimi dei vari array per settare i fondoscala dei grafici
*/
void printermanager::Calc_Max_RealReport_rel2(short __num_sample)
{
    int vol_max = 0;
    int emg_max = 0;

    for(int j = 0; j < __num_sample; j++) {		// calcolo FC curva di flusso (max_y) e FC curva di volume (max_x) cosi da individuare il giusto fondoscala
        int v = buffer_vol[j];
        if(v > vol_max)
            vol_max = v;
    }

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

    if(m_emgPresent) {
        for(int j = 0; j < __num_sample; j++) {
            int v = buffer_emg[j];
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
    }
    m_max_emg = emg_max;

    if (m_rangeChVV > 0 && m_rangeChVV < 1000) //auto-range disabilitato
        m_max_vol = m_rangeChVV;

    if (m_rangeChEMG > 0 && m_rangeChEMG < 3500) //auto-range disabilitato
        m_max_emg = m_rangeChEMG;

    qDebug("vmax:%d emgMax:%d", vol_max, emg_max);
}
