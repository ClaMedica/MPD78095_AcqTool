
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

// numeri ruotati di 90? in senso orario
const unsigned char  print8x8Set[] = {
    0x3C, 0x7E, 0x81, 0x81, 0x7E, 0x3C, 0x00, 0x00,      /*   0 - 30 - 0 */
    0x84, 0x82, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x00,      /*   1 - 31 - 1 */
    0xC2, 0xE3, 0xB1, 0x99, 0x8F, 0x86, 0x00, 0x00,      /*   2 - 32 - 2 */
    0x42, 0xC3, 0x81, 0x99, 0xFF, 0x66, 0x00, 0x00,      /*   3 - 33 - 3 */
    0x30, 0x28, 0x24, 0x23, 0xFF, 0x20, 0x00, 0x00,      /*   4 - 34 - 4 */
    0x8F, 0x89, 0x89, 0xD9, 0x71, 0x00, 0x00, 0x00,      /*   5 - 35 - 5 */
    0x7E, 0x8B, 0x89, 0x89, 0xDB, 0x72, 0x00, 0x00,      /*   6 - 36 - 6 */
    0x81, 0xD1, 0x71, 0x39, 0x1D, 0x17, 0x03, 0x00,      /*   7 - 37 - 7 */
    0x66, 0x7E, 0x99, 0x99, 0x7E, 0x66, 0x00, 0x00,      /*   8 - 38 - 8 */
    0x4E, 0xDB, 0x91, 0x91, 0xDB, 0x7E, 0x00, 0x00,      /*   9 - 39 - 9 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0,      /*     .     10 */
    0xF8, 0x1C, 0x1C, 0xF8, 0xF8, 0x1C, 0x1C, 0xF8,	     /* m 	11 */
    0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0x00, 0x00,	     /* L 	12 */ /* mL/s*/
    0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x0C,	     /* / 	13 */
    0x00, 0x98, 0x9C, 0xB4, 0xB4, 0xE4, 0x64, 0x00,	     /* s	14 */
    0x80, 0xFC, 0x7C, 0xE0, 0xC0, 0xE0, 0x7C, 0x3C,	     /* u	15 */
    0x07, 0x1E, 0x70, 0xC0, 0xC0, 0x70, 0x1E, 0x07,	     /* V 	16 */
    0x00, 0x20, 0x78, 0xa4, 0xa4, 0xa4, 0x18, 0x00,	     /* e	17 */
    0x00, 0x00, 0x78, 0x84, 0x84, 0x84, 0x84, 0x00,	     /* c	18 */
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
    m_file = NULL;
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

    m_resultBm_w = 103*8; // 103 bytes * 8 bit
    m_resultBm_h = 300;
    m_bitmapSiroky.   resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapLiverpool.resize((m_resultBm_w * m_resultBm_h) / 8);
    m_bitmapSiroky.   fill(0);
    m_bitmapLiverpool.fill(0);

    buffer_emg = NULL;
    buffer_flw = NULL;
    buffer_vol = NULL;

    qDebug("new imageBm");
    imageQsz = QSize(832, 6000);
    imageBm = QImage(imageQsz, QImage::Format_Mono);
    imagePainter = new QPainter(&imageBm);
    imageInit();
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
    font.charW = imagePainter->fontMetrics().width('9');

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
    imageFontInit(tf_graphPortr, "Bitstream Vera Sans Mono", 12, QFont::Thin);
    imageFontInit(tf_graphLand,  "Bitstream Vera Sans Mono", 12, QFont::Thin);
    imageFontInit(tf_infoLabel,  "Bitstream Vera Sans Mono", 12, QFont::Bold);
    imageFontInit(tf_infoValue,  "Bitstream Vera Sans Mono", 12, QFont::Normal);

    tf_header20.qf.setUnderline(true);

    imagePainter->setFont(tf_base.qf);

    imagePt = QPoint(0, 0);
//    imageTestFont();
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
                Pri_forward(emptyLines);
                emptyLines = 0;
            }
            printPixLine(tmp, wBytes-rightZeroED);
            lastNotEmpty = y;
        }
    }
    if(emptyLines > 0) {    // linee vuote precedenti
        Pri_forward(emptyLines);
        emptyLines = 0;
    }
    qDebug("linee effettive:%d", lastNotEmpty);
    imageInit();    // immagine azzerata

    // riga di strappo
    tmp[0] = 0;
    for(int i = 1; i < 103; i++)
        tmp[i] = 0xcc;
    printPixLine(tmp, 103);
    // fa avanzare la carta 30mm per consentire lo strappo
    Pri_forward((int) (26 / 0.125));

    qDebug() << "file chiuso in scrittura" << m_file->fileName();
    m_file->close();
    delete m_file;
    m_file = NULL;

    qDebug("fine print: sendSup()");
//    udpConn.sendPrn("print:" + m_namefilePrn.toLatin1());   // diretto
    udpConn.sendSup("Print:" + m_namefilePrn.toLatin1());   // gateway

#endif
}

void printermanager::printPixLine(uchar *p, int sz)
{
    uchar buf[256];
    buf[0] = ESC;   // 0x1B;
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
    Pri_Str((char *)buf, LCMD+sz);
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
    QStringList baseLst = baset.split(" ", QString::SkipEmptyParts);
    imagePt.rx() = 5;

    QFont savedFont, tfont;
    tfont = savedFont = imagePainter->font();
    imageText(head, -1, false);
    tfont.setPixelSize(15);
    imagePainter->setFont(tfont);
    {
        imagePt.rx() = 5;
        imagePt.ry() += 10;

        int digitsW = imagePainter->fontMetrics().width("9999");
        int left = (digitsW + 1);
        int h = 240;
        int w = (imageQsz.width()-imagePt.rx()) - (left + digitsW+5 /* destra */ );
        qDebug("digitsW:%d w:%d",digitsW,w);

        QPoint lt(imagePt.rx() + left, imagePt.ry());         // leftTop
        QPoint lb(imagePt.rx() + left, imagePt.ry() + h);     // leftBottom
        imageRectFlw = QRect(lt, QSize(w, h));
        imagePainter->drawRect(imageRectFlw);

        // griglia
        imagePainter->setPen(Qt::DashLine);
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

        QPoint * points = new QPoint[w];
        xscale *= (w / 752.0);
        imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxL, bufL);
        if(maxR > 0)
            imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxR, bufR);
        delete points;
    }
    imagePainter->setFont(savedFont);

    imagePt.ry() += 50;
}

void printermanager::imageGraphSingle(QPoint leftBottom, QPoint *pts, int npts, double kx, double ky, double *bufV)
{
    for (int ix = 0; ix < npts; ix++ ) {
        int x = leftBottom.rx() + (int)(kx * ix);
        int y = leftBottom.ry() - (int)(ky * bufV[ix]);
        pts[ix] = QPoint(x, y);
    }
    imagePainter->drawPolyline(pts, npts);
}

void printermanager::imageGraphL(QString head, QString baset, double xscale, int maxL, int maxR, double *bufL, double *bufR)
{
    QStringList baseLst = baset.split(" ", QString::SkipEmptyParts);
    imagePt.rx() = 5;

    QFont savedFont, tfont;
    tfont = savedFont = imagePainter->font();
    imageText(head, -1, false);
    tfont.setPixelSize(15);
    imagePainter->setFont(tfont);
    {
        imagePt.rx() = 5;
        imagePt.ry() += 10;

        int digitsW = imagePainter->fontMetrics().width("9999");
        int left = (digitsW + 1);
        int h = 240;
        int w = (imageQsz.width()-imagePt.rx()) - (left + digitsW+5 /* destra */ );
        qDebug("digitsW:%d w:%d",digitsW,w);

        QPoint lt(imagePt.rx() + left, imagePt.ry());         // leftTop
        QPoint lb(imagePt.rx() + left, imagePt.ry() + h);     // leftBottom
        imageRectFlw = QRect(lt, QSize(w, h));
        imagePainter->drawRect(imageRectFlw);

        // griglia
        imagePainter->setPen(Qt::DashLine);
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

        QPoint * points = new QPoint[w];
        xscale *= (w / 752.0);
        imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxL, bufL);
        if(maxR > 0)
            imageGraphSingle(lb, points, m_num_sam, xscale, (double)h/maxR, bufR);
        delete points;
    }
    imagePainter->setFont(savedFont);

    imagePt.ry() += 50;
}

void printermanager::imageGraphSingleL(QPoint leftBottom, QPoint *pts, int npts, double kx, double ky, double *bufV)
{
    for (int ix = 0; ix < npts; ix++ ) {
        int x = leftBottom.rx() + (int)(kx * ix);
        int y = leftBottom.ry() - (int)(ky * bufV[ix]);
        pts[ix] = QPoint(x, y);
    }
    imagePainter->drawPolyline(pts, npts);
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
    //      Intestazione
    qDebug() << "Pri_Rep file:" << m_namefile;
    Intest();

    //     Identificativi Esame
    Report_data();		// scrive i dati del paziente e lo spazio per le note manuali
//    m_port->Pri_Str(LINE2, 2, false);

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

        Report_flw(xscale);	// finalmente stampiamo i grafici di volume e flusso
        if(m_emgPresent)
            Report_emg(xscale);						// EMG
    }
    else {              // print_mode = LANDSCAPE_MODE - grafico longitudinale, con lunghezza legata alla lunghezza dell'esame
        m_max_y = Calc_Max_Flw();			// fondo scala del flusso
        qDebug("dopo Calc_Max_Flw()");
        Calc_Max_RealReport_rel2(m_num_sam);
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
//    m_port->Pri_Str(LINE2, 2, false); // LINE"\x3",0);

    //	Scrive i dati riguardanti versione firmware e date/ora ultima calibrazione
       //Report_dati_macchina(numCurve);	// scrive data e ora della stampa e la versione attuale del FW

    qDebug() << "Pri_Rep FINE file:" << m_namefile;
}

/**
E' stampata l'intestazione del report, con intestazione clinica, logo and so on
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
    imagePainter->drawText(QRect(imagePt, rb), Qt::AlignCenter, tr("Urodynamic Equipment"));
    imagePt.ry() = rb.y() + 10;

    imagePt.ry() += 80;
}

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

/**
Sono stampati i dati del report paziente, nome, cognome, data di nascita, sesso, data esame, ....
*/
void printermanager::Report_data()
{
    imagePt.rx() = 5;

    const char puntini[] = ". . . . . . . . . . . . . . . . . . . . .";

    Report_row(tr("Test Number ....: "), QString::number(m_numTest));
    Report_row(tr("Test Date ......: "), m_dateofexam);
    Report_row(tr("Surname ........: "), m_test_type ? puntini : m_surname);
    Report_row(tr("Name ...........: "), m_test_type ? puntini : m_name);
    Report_row(tr("Birth Date .....: "), m_test_type ? puntini : m_dateofbirth);
    Report_row(tr("Gender .........: "), m_test_type ? puntini : QString(m_sex));
    Report_row(tr("Investigator ...: "), puntini);
    Report_row(tr("Comments .......: "), puntini);

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
}

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
void printermanager::Report_emg(double xscale)
{
    Calc_Max_EMG();
    qDebug() << "m_max_emg:" << m_max_emg << "m_max_y:" << m_max_y;

    imageGraph(QString("   EMG ( uV )              ")+tr(" EMG Diagram "),
               str_label_time[m_i_max_x],
               xscale,
               m_max_y,
               0,
               buffer_emg,
               NULL
               );
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

/**
Nella stampa del report tipo reale, individuo il fondoscala del flusso, parametro che serve per adattare i valori
degli array dati volume e emg (e anche flusso)
*/
long printermanager::Calc_Max_Flw()
{
    long flw_max = (long)m_flu_max;

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
    m_init_time_to_print = 0;
    m_cursore = 0;

    // STAMPA DELLE LABEL E DELL'ASSE SINISTRO (label EMG E FLUSSO)
    m_pos_gra_emg        = 0;
    m_pos_gra_vol        = (m_emgPresent) ?  20 :   0;
    m_pos_gra_flw        = 50;
    m_num_dots_gra_emg   = (m_emgPresent) ? 160 :   0;
    m_num_dots_gra_vol   = (m_emgPresent) ? 240 : 400;
    m_num_dots_gra_flw   = 400;
    m_num_byte_x_gra_emg = (m_emgPresent) ?  20 :   0;
    m_num_byte_x_gra_vol = (m_emgPresent) ?  30 :  50;
    m_num_byte_x_gra_flw = 50;
    m_num_byte_x_gra = 800;

    {
        QPoint psave = imagePt;
        imagePt.rx() = 10;
        imagePt.ry() += 10;
        imagePainter->save();
        imagePainter->translate(imagePt);
        imagePainter->rotate(90);

        Pri_Rep_Label();	// stampa label del flusso ed eventualmente anche l'emg nel caso di grafici sovrapposti

//        // SCOMPONGO LA STAMPA DEL GRAFICO IN TANTE STAMPE DA 5 SECONDI CIASCUNA
//        int num_righe;
//        num_righe = (int)(__num_sample / NUM_POINTS) + 1;	// numero intero di blocchi da 40 righe (5sec), il restante e stampato in un altro blocco da 40
//        Pri_Rep_Gra_Landscape(-1, xscale);      // forza init delle variabili
//        for(int riga = 0; riga < num_righe; riga++ )                        // scompone la griglia in tante righe
//            Pri_Rep_Gra_Landscape(__num_sample, xscale); // scrive label di tempo ad ogni accesso
//        Pri_Rep_asse_dx();    // ora stampo l'asse destro del grafico

        imagePainter->restore();
        psave.ry() += imagePt.x();
        imagePt = psave;
    }
    imagePt.ry() += 90;
}

void printermanager::Pri_Rep_Label()
{
    int y;
    int txtW = tf_graphLand.charW * QString("9999 ml/s").size() + 3;
    imageSetFont(tf_graphLand);

    if(m_emgPresent)
        imagePainter->drawText(txtW-10, -m_pos_gra_emg*8, "0");     // stampo lo zero dell'emg senza udm
    imagePainter->drawText(txtW-10, -m_pos_gra_vol*8, "0");         // stampo lo zero del  vol senza udm
    imagePainter->drawText(txtW-14, -m_pos_gra_flw*8, "0");         // stampo lo zero del  flw senza udm

    imagePainter->drawLine(txtW, 0, txtW, -(800-1));
    imagePainter->drawLine(txtW, -(800), txtW +40, -(800));
    for(int i = 1; i <= _NUM_LABEL; i++) {
        int val = (m_max_y / _NUM_LABEL ) * i;
        QString txt = QString::asprintf("%4d ml/s", val);
        y = m_pos_gra_flw*8 + (m_num_dots_gra_flw / _NUM_LABEL) * i;
        imagePainter->drawText(0, -y, txt);
        imagePainter->setPen(Qt::DashLine);
        imagePainter->drawLine(txtW, -y, txtW + 40, -y);
        imagePainter->setPen(Qt::SolidLine);
    }
    y = m_pos_gra_flw*8;
    imagePainter->drawLine(txtW, -y, txtW + 40, -y);
    imagePainter->drawLine(txtW, -(800), txtW +40, -(800));

    imagePainter->setPen(Qt::DashLine);
    imagePainter->drawLine(txtW + 40, 0, txtW + 40, -(800-1));
    imagePainter->setPen(Qt::SolidLine);

    imagePt.rx() = txtW+43;
    return;

    int num_char_to_print, max_y;
    int pos_4_this_string;
    int dots3D = _NUM_CHAR_X_LABEL_rel2 * _HEIGHT_CHAR_LABEL;
    int dim_label = 1;	// nel caso dello "0" nessun label ma scriviamo lontano dal bordo
    if(m_emgPresent) {							// stampo lo zero dell'emg senza udm
                                                // label dell'asse dell'volume
        for(int i = 1; i < _NUM_LABEL_VOL_EMG; i++) {			// se c'e l'emg solo 2 label piu lo zero
                                                                // non scrivo il label del fondoscala, che sarebbe troppo vicino allo zero successivo, quindi scrivo solo
            pos_4_this_string = (m_pos_gra_vol + (m_num_byte_x_gra_vol/_NUM_LABEL_VOL_EMG)*i )* dots3D;
            max_y = (m_max_vol / _NUM_LABEL_VOL_EMG ) * i;
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

    printBitMap_unaRigaPerVolta();
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
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ m_init_time_to_print*_WIDTH_CHAR_LABEL + i];	// carattere "0"
        }
        init_pos_in_string = _NUM_BYTE_CMD + (1)*_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 14*_WIDTH_CHAR_LABEL + i];					// carattere "S"
        }
        init_pos_in_string +=_HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 17*_WIDTH_CHAR_LABEL + i];					// carattere "e"
        }
        init_pos_in_string += _HEIGHT_CHAR_LABEL;
        for(int i = 0; i < _HEIGHT_CHAR_LABEL; i++) { // scrivo il carattere delle unita per colonne come 1Bx13righe
            m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ 18*_WIDTH_CHAR_LABEL + i];					// carattere "c"
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
                        m_str_gr[ init_pos_in_string + i ] = (char)print8x8Set[ dig[d]*_WIDTH_CHAR_LABEL + i];
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
            m_str_gr[ _NUM_BYTE_CMD + 62*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 72*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 82*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            m_str_gr[ _NUM_BYTE_CMD + 92*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
            if((z == 1) || (z == 5))
            {	// tratteggi nell'area vol+emg
                m_str_gr[ _NUM_BYTE_CMD + 12*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 22*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 32*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 42*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                // tratteggi nell'area del flusso
                m_str_gr[ _NUM_BYTE_CMD + 54*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 56*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 58*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 60*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 64*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 66*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 68*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 70*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 74*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 76*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 78*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 80*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 84*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 86*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 88*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 90*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 94*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 96*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 98*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
                m_str_gr[ _NUM_BYTE_CMD + 100*NUM_POINTS + i*_WIDTH_CHAR_LABEL + z] |= (char)0x80;	// tratteggio
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

    printBitMap_unaRigaPerVolta();
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
//    m_port->Pri_Str((char *)m_str_gr, dim_string_solo_ax, false);

}

/**
Stampa dei nomogrammi
*/
void printermanager::Report_BitMap(bool __isSiro)
{
    qDebug("inizio pr bitm");
    int     sz;
    char  * p;

    if (__isSiro) {
        sz = m_bitmapSiroky.size();
        p = m_bitmapSiroky.data();
    }
    else {
        sz = m_bitmapLiverpool.size();
        p = m_bitmapLiverpool.data();
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
        txstrList.append(tr("Waiting time ..............")); txstrList.append(QString::asprintf(" : %5.1f s", m_tem_att));
    }

    if (m_flu_med > m_flu_max) // piccolo controllo per gestire flussi abnormali, tipici di prove da laboratorio
        if (m_tem_flu < 30) // se la flussata e molto breve e intensa, l'algoritmo sbaglia e puo risultare flu_med > flu_max
            m_flu_med = m_flu_max;

    txstrList.append(tr("Maximum flow rate .........")); txstrList.append(QString::asprintf(" : %5.1f ml/s"      , m_flu_max));
    txstrList.append(tr("Average flow rate .........")); txstrList.append(QString::asprintf(" : %5.1f ml/s"      , m_flu_med));
    txstrList.append(tr("Time to maximum flow ......")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_max));
    txstrList.append(tr("Time between 5% and 95% ...")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_595));
    txstrList.append(tr("Flow time .................")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_flu));
    txstrList.append(tr("Descent time ..............")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_dis));
    txstrList.append(tr("Voiding time ..............")); txstrList.append(QString::asprintf(" : %5.1f s"         , m_tem_svu));
    txstrList.append(tr("Volume to maximum flow ....")); txstrList.append(QString::asprintf(" : %5.1f ml"        , m_vol_max));
    txstrList.append(tr("Voided Volume .............")); txstrList.append(QString::asprintf(" : %5.1f ml"        , (double) m_vol_vuo));
    txstrList.append(tr("Corrected maximum flow ....")); txstrList.append(QString::asprintf(" : %5.1f ml^(1/2)/s", m_cQ     ));
    txstrList.append(tr("Flow acceleration .........")); txstrList.append(QString::asprintf(" : %5.1f ml/s^2"    , m_flu_acc));    //?=2 apice
    txstrList.append(tr("Maximum contraction speed .")); txstrList.append(QString::asprintf(" : %5.1f mm/s"      , m_vDetMax));
    txstrList.append(tr("Residual volume ...........")); txstrList.append(QString::asprintf(" : %5.1f ml"        , (double) m_resVol ));

    for(int i = 0; i < txstrList.size(); i += 2) {
        imagePt.rx() = 8;
        imagePt.ry() += tf_infoLabel.charH;
        imageSetFont(tf_infoLabel); imagePainter->drawText(imagePt, txstrList.at(i));
        imagePt.rx() = tf_infoLabel.charW * tr("Maximum flow rate .........").size();
        imageSetFont(tf_infoValue); imagePainter->drawText(imagePt, txstrList.at(i+1));
    }

    imagePt.rx() = 8;
    imagePt.ry() += tf_infoLabel.charH;
    imagePainter->drawLine(imagePt, QPoint(imageBm.width()-1-8, imagePt.ry()));
    imagePt.ry() += tf_infoLabel.charH;
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

void printermanager::printBitMap_unaRigaPerVolta()
{
    // bitmap completa spezzata in una riga di bit per volta

    const int ByteXline = m_str_gr[7];      // bytes x linea

    int sz = 0;     // dimensione originale
    sz += (unsigned) m_str_gr[4];
    sz *= 256;
    sz += (unsigned) m_str_gr[3];
    sz *= 256;
    sz += (unsigned) m_str_gr[2];

//    qDebug("spezzo bitmap di:%d in pezzi da:%d",sz, ByteXline);

    m_str_gr[2] = ByteXline;        // n1
    m_str_gr[3] = 0;                // n2
    m_str_gr[4] = 0;                // n3

    char tbuf[LCMD + 256];
    memcpy(tbuf, m_str_gr, LCMD);   // header comando di stampa

    char  * p = m_str_gr + LCMD;    // ptr a bitmap
    for(int i = 0; i < sz; ) {
        memcpy(tbuf+LCMD, p, ByteXline);
//        m_port->Pri_Str(tbuf, LCMD+ByteXline);
        i += ByteXline;
        p += ByteXline;
    }
}

/**
 * @brief printermanager::printDigits
 * @param __val     : valore POSITIVO da convertire
 * @param __ndigits
 * @param __pos
 */
void printermanager::printDigits(int __val, int __ndigits, int __pos)
{
    printNumber(__val, __ndigits, __pos, print7x13Set, 13, 7);
}

void printermanager::printNumber(int __val, int __ndigits, int __pos, const unsigned char * fontBm, int fontH, int fontW)
{
    (void) fontW;   // per ora non usato
    qDebug("traccia print val:%d ndig:%d pos:%d", __val,__ndigits,__pos);

    char  * dst;
    char  * src;
    int     digits[10];

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
            src = (char *) & fontBm[ digits[n]*fontH ];
            for(int i = 0; i < fontH; i++ )
                *dst++ = *src++;
        }
    }
}

/**
funzione dedicata alla stampa dei caratteri dell'asse a sinistra nel caso del report reale, che
possono essere volume e flusso, oppure emg, volume e flusso
le unita di misura sono invece inserite nella funzione principale
*/
void printermanager::print_char_left_label(int __value, int __pos_in_string, int __num_char, bool __pri_decim, int __pre_char)
{
    qDebug("traccia print val:%d pos:%d nch:%d pri_decim:%d pre_char:%d",__value,__pos_in_string,__num_char,__pri_decim,__pre_char);
    (void) __pri_decim; // eliminato perche mai invocato con true
//    printNumber(__value, __num_char, __pos, print7x13Set, 8, 8);
    char  * dst;
    char  * src;
    int     digits[10];

    for(int i = 0; i < __num_char; i++ )
        digits[i] = -1;

    int i = __num_char;
    do {
        digits[--i] = __value % 10;
        __value /= 10;
    } while(__value > 0);

    int oriz_pos = _NUM_CHAR_X_LABEL_rel2 - __pre_char;
    int pos = _NUM_BYTE_CMD + __pos_in_string + (oriz_pos - __num_char)*_HEIGHT_CHAR_LABEL;
    dst = (char *) & m_str_gr[ pos ];   // clear
    for(int i = 0; i < 8*__num_char; i++)
        *dst++ = 0;

    int h = _HEIGHT_CHAR_LABEL;
    int w = _WIDTH_CHAR_LABEL;
    for(int n = 0; n < __num_char; n++) {
        int v = digits[n];
        if(v >= 0) {
            dst = (char *) & m_str_gr[ pos + h*n];   // clear
            src = (char *) & print8x8Set[ v*w];
            for(int i = 0; i < h; i++ )
                dst[i] = *src++;
        }
    }
}

/**
Inserisce nella stringa le udm delle curve che possono essere: ml/s, ml, uV
*/
void printermanager::print_udm_label(int ch_type, int __pos_in_string, int __pre_char)
{
    qDebug("traccia print ch_type:%d pos:%d pre_char:%d",ch_type,__pos_in_string,__pre_char);
    int h = _HEIGHT_CHAR_LABEL;
    int w = _WIDTH_CHAR_LABEL;
    int oriz_pos = _NUM_CHAR_X_LABEL_rel2 - __pre_char + 1; // con il "+1" ho gia messo lo spazio vuoto fra numero e label
    int pos = _NUM_BYTE_CMD + __pos_in_string + oriz_pos*h - 4;// la terza cifra (primo carattere)

    int ch, cnt = 0;
    if (ch_type == m_chVol) { // stampa 'mL'
        ch  = 11; cnt = 2;
    }
    else if (ch_type == m_chFlw) { // stampa 'mL/s'
        ch  = 11; cnt = 4;
    }
    else if (ch_type == m_chEmg) { // stampa 'uV'
        ch  = 15; cnt = 2;
    }
    for(int n = 0; n < cnt; n++) {
        for( int i = 0; i < h; i++ )
            m_str_gr[ pos + i ] |= (char) print8x8Set[ ch*w + i];
        pos += h;
        ch++;
    }
}

void printermanager::Pri_forward(char __dotlines)
{
    /* fa avanzare di "dotlines" righe la carta*/

    char pri_str[] = { ESC, 'J', __dotlines };
    Pri_Str(pri_str, sizeof(pri_str));
}

bool printermanager::Pri_Str(char *__str, int __str_len)
{
    if(m_file && m_file->isOpen()) {
        static char trailer[1] = { 0x03 };
        char header[6] = { 0x5a, 0xa5, 0xa5, 0x5a, __str_len & 0xff, (__str_len >> 8) & 0xff };

        m_file->write(header, sizeof(header));
        m_file->write(__str, __str_len);
        m_file->write(trailer, sizeof(trailer));
    }

    return true;
}

