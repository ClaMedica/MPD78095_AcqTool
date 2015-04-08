#include "functionhandler.h"
#include "QVector"

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------Algoritmi Principali@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------

bool TWI(MSignal *__pSamglob, MSignal *__pSamcamp, int __threshold, float __windowsDimension,VarMapVec *__result){
    /*
     Nome completo: time warping identification
     Parent: ---
     Ambiente: Time Warping

     Descrizione: analizza la distorsione di una finestra di segnale utile con un segnale campione
         in questo caso confronta un battito cardiaco

     Definizione dei parametri:

     __pSamglob         = esame globale
     __pSamcamp         = campione di cuore (1 battito di circa 3000 punti)
     __threshold        = soglia con cui determinare le finestre di energia
     __windowsDimension = dimensione della finestra con la quale vettorizzare le finestre
     nfin       =       numero di finestre analizzate che viene determinato alla fine dell'analisi



     */

    //char p[80];
    //int deg=0,cuo=0,sil=0;



    Tabble samCampVec;
    MSignal *energyResult=new MSignal;
    Rule rule(III);
//    QVector<float> lista;
//    lista<<0<<1<<2<<5<<9
//        <<5<<6<<9<<7<<2
//       <<2<<5<<9<<3<<0
//      <<6<<9<<5<<4<<7
//     <<0<<1<<2<<5<<9;
//    Tabble matrix;
//    BudVector path;
//    matrix.resize(5,5,0);
//    for(int i=0;i<5;i++)
//        for(int j=0;j<5;j++)
//            matrix.setValue(i,j,lista[i*5+j]);
//    pathFinder(&matrix,&rule,&path);

    energyFramesFinder(__pSamglob,__result,energyResult,__threshold,__windowsDimension);
    //energy(__pSamglob,256,stepp,h);
    //stepp->setIndex(0,stepp->Col()-1,ing.durata-1);

    //analisi=new EXAM_WINDOW[stepp->Col()-1];


    vectorize(&samCampVec,__pSamcamp,__windowsDimension,LPC);

    for(int winIndex=0;winIndex<__result->size();winIndex++)
    {
        float begin=__result->at(winIndex)->value("xMin").toFloat();
        float end=__result->at(winIndex)->value("xMax").toFloat();
        MSignal part;
        Tabble partVec,matrix;
        BudVector path;
        __pSamglob->getSection(&part,begin,end-begin);
        vectorize(&partVec,&part,__windowsDimension,LPC);

        distanceMatrix(&samCampVec,&partVec,&matrix);

        matrix.customOperator('*',1000);

        float distortion=pathFinder(&matrix,&rule,&path);
        (*(*__result)[winIndex])["popUp"]="Distorsione"+QString::number(round(distortion));
    }




    return true;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------Figli livello 1@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@----

bool distanceMatrix(Tabble *__firstMatrix, Tabble *__secondMatrix, Tabble *__resultMatrix)
{
    /*
     Nome completo: matrice delle distanze
     Parent: pathfinder
     Ambiente: Time Warping

     Descrizione: calcola la matrice delle distante spettrali

     Definizione dei parametri:

     __firstMatrix  = matrice numero 1
     __secondMatrix = matrice numero 2
     __resultMatrix = matrice di uscita

     */

    if(__firstMatrix->columns()!=__secondMatrix->columns())
    {
        qDebug()<<"distanceMatrix: different columns";
        return false;
    }

    int m=__firstMatrix->rows(),n=__secondMatrix->rows();
    __resultMatrix->resize(m,n,0);
    for(int i=0;i<m;i++)
    {
        Tabble pm1;
        __firstMatrix->tabblePart(&pm1,i,0,1,__firstMatrix->columns());
        for(int j=0;j<n;j++)
        {
            Tabble pm2;
            __secondMatrix->tabblePart(&pm2,j,0,1,__secondMatrix->columns());

            pm2.customOperator('-',&pm1);
            pm2.customOperator('^',2);
            float s=pm2.sum();
            __resultMatrix->setValue(i,j,s);
        }
    }
    return true;
}




float pathFinder(Tabble *__distorsionMatrix, Rule *__rule, BudVector *__path)
{
    /*
    Nome completo: pathfinder
    Parent: twi
    Ambiente: Time Warping

    Descrizione: è un algoritmo in grado di individuare il percorso a minima distorsione
    all'interno di una matrice di distanze spettrali

    Definizione dei parametri:

    __distorsionMatrix    =       matrice delle distanze spettrali
    __rule                =       matrice della regola degli spostamenti possibili



    */

    //si spiegano da sole

    int lunr=__distorsionMatrix->rows();
    int lunc=__distorsionMatrix->columns();
    //int maxNumOfBranches=__rule->branchNumber();
    /*
     * iniziamo col prelevare le info dalla rule come ad esempio la massima estensione
     * di un possibile ramo all'interno della stessa quindi chiamiamo queste due funzioni
     */
    int maxH=__rule->maxHorizontalBranchLength();
    int maxV=__rule->maxVerticalBranchLength();
    /*
     * A questo punto dobbiamo creare la nostra matrice di lavoro che dovrà avere dei bordi
     * estesi di maxH e maxV per poter permettere alla rule di espandersi fino al massimo in
     * ogni suo punto
     */

    __distorsionMatrix->addColumns(maxV);
    __distorsionMatrix->addRows(maxH);

    // E adesso un po' di inizializzazioni cominciamo con la matrice di distorsione
    // che è grande quanto mat ma piena di NAN
    Tabble disto(__distorsionMatrix->rows(),__distorsionMatrix->columns(),NAN);

    //e ne azzeriamo la posizione 0x0
    disto.setValue(0,0,0);
    /*
     * continuiamo con la tabella dei punti di partenza: questa tabella sostanzialmente mi
     * memorizza la fine dei vari rami per cui la chiameremo bud (gemma). Assieme a questo
     * inizializziamo anche il budIndex
     */

    BudVector oldGeneration,newGeneration,bestPath;
    BudMap budMap;
    Bud bestBud;
    bestBud.dist=0;
    bestBud.xPos=0;
    bestBud.yPos=0;
    oldGeneration.append(bestBud);



    //inizializzo la budMap

    for(int i=0;i<lunr+maxH;i++)
    {
        BudVector cur;
        for(int j=0;j<lunc+maxV;j++)
        {
            Bud nuova;
            nuova.dist=0;
            nuova.xPos=0;
            nuova.yPos=0;
            cur.append(nuova);
        }
        budMap.append(cur);
    }

    //indica se una diramazione è da aggiornare o meno

    int limitator=3;                    //limitatore del paralellogramma

    while(oldGeneration.size()>0)
    {
        /*
         * La regola di ingaggio è semplice ovvero rimani dentro fintanto che ci sono nuove
         * gemme su cui lavorare. Una volta che siamo entrati qua dobbiamo generare una nuova
         * diramazione per cui ci serve l'informazione della funzione minPath
         */

        Bud curBud=oldGeneration.takeFirst();
        if(curBud.xPos<lunr-1 && curBud.yPos<lunc-1)
        {
            //controllo che la gemma stia all'interno della safeZone e che non sia sul bordo
            Tabble miniMat;
            BudVector germ;
            int startX=curBud.xPos;
            int startY=curBud.yPos;
            //mi copio la sezione di lavoro dove si svilupperà la gemma
            __distorsionMatrix->tabblePart(&miniMat,startX,startY,maxH+1,maxV+1);
            germination(&miniMat,__rule,&germ);


            for (int germIndex=0;germIndex<germ.size();germIndex++)
            {
                /*
                 * scorre tutti i rami possibili della rule e va a vedere se il
                 * punto di arrivo del germoglio è libero: nel caso lo sia lo scriverà
                 * nella matrice se non lo è controlla se la distorsione introdotta è
                 * minore oppure no, se sì lo aggiorna se no lo scarta
                 */
                //contiene il fato della nuova gemma
                QString budFate;

                //e creo invece il bud per la mappacon la distorsione introdotta dal ramo nuovo
                //e la posizione da cui provengo
                Bud newBud;
                newBud.xPos=startX;
                newBud.yPos=startY;
                newBud.dist=germ[germIndex].dist+budMap[startX][startY].dist;

                int destX=startX+germ[germIndex].xPos;
                int destY=startY+germ[germIndex].yPos;

                //trovo il nuovo punto di arrivo e lo memorizzo
                Bud startPoint;
                startPoint.xPos=destX;
                startPoint.yPos=destY;
                startPoint.dist=germ[germIndex].dist+budMap[startX][startY].dist;

                //ora come procedo: verifico che la casella nella posizione della gemma sia
                //vuota

                if(budMap[destX][destY].dist==0)
                {
                    //la casella è vuota per cui posso scrivere la casella da dove
                    //provengo
                    budFate="added";
                    budMap[destX][destY]=newBud;
                }
                else
                {   //se la casella non è vuota ma già stata scritta come nel caso di
                    //rami coincidenti faccio vincere quello a minore distorsione
                    float oldDist=budMap[destX][destY].dist;
                    float newDist=newBud.dist;

                    if(newDist<oldDist)
                    {//distorsione minore quindi aggiorno
                        budMap[destX][destY]=newBud;
                        budFate="replaced";
                    }
                    else
                        budFate="rejected";//nel caso la nuova distorsione sia maggiore non modifica nulla
                }


                //ora devo aggiornare l'indice dei punti di partenza e per farlo
                //mi serve un indicatore, prima però controllo che il punto di
                //arrivo che diventerà di partenza sia un punto valido overo che stia nel limiti

                int diffX=abs(newBud.xPos-bestBud.xPos);
                int diffY=abs(newBud.yPos-bestBud.yPos);
                if(diffX>limitator || diffY>limitator)
                    budFate="rejected";




                //se la gemma non deve essere scartata e non è già stata aggiunta
                //e sta dentro alla matrice
                // e non vicino al confine allora la tengo altrimenti vado avanti
                //bool isNew=true;

                if(budFate!="rejected" && newBud.xPos<lunr-1 && newBud.yPos<lunc-1 )
                    newGeneration.append(startPoint);

                //se il fato della gemma è rejected allora non dovrò considerarlo poi per cui
                //riduco il numero di rami liberi
            }
        }

        /* allora adesso ho fatto e controllato la mia germinazione e aggiunto
  * i miei punti di partenza ora mi cerco quello a distorsione minore tra
  * quelli generati dalla mia generazione in modo da sapere dove è il bestBud
 */
        if(oldGeneration.size()==0)
        {   //la condizione di ingaggio è semplice: entro qua solo se sono alla fine
            //della vecchia generazione e lo faccio per determinare la bestBud che ho
            //in quella nuova
            if(newGeneration.size()>0)
            {
                float min=newGeneration[0].dist;
                bestBud=newGeneration[0];

                for(int i=0;i<newGeneration.size();i++)
                {
                    float m=newGeneration[i].dist;
                    if(m<min)
                    {
                        min=m;
                        bestBud=newGeneration[i];
                    }
                }
                oldGeneration=newGeneration;
                newGeneration.clear();
            }
        }

    }


    for(int i=0;i<maxH;i++)
        budMap.removeLast();
    for(int i=0;i<budMap.size();i++)
        for(int j=0;j<maxV;j++)
            budMap[i].removeLast();


    return bestPathResearch(&budMap,__path);

}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------Figli livello 2@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@----


/**
 * @brief germination generate a new matrix were are displayed the arrival
 * points following the rule path
 * @param __matrix on wich calculate the __result
 * @param __rule
 * @param branchNumber
 * @param _branchMaxLength each branch can have multiple step and this is
 * the max number of them for a single branch
 * @param __result
 */

bool germination(Tabble *__matrix,
                 Rule *__rule,
                 BudVector *__result)
{

    QString funMex="germination: ";

    if(__matrix==NULL)
    {
        qDebug()<<funMex+"__matrix not defined";
        return false;
    }

    if(__rule==NULL)
    {
        qDebug()<<funMex+"__rule not defined";
        return false;
    }

    if(__result==NULL)
    {
        qDebug()<<funMex+"__result not initialized";
        return false;
    }

    int branchNumber=__rule->branchNumber();



    for(int i=0;i<branchNumber;i++)
    {
        Branch curB=__rule->getBranch(i);

        int oldPosX=0;
        int oldPosY=0;
        int newPosX=0;
        int newPosY=0;
        float err=0;
        for(int k=0;k<curB.size();k++)
        {
            Jump curJ=curB.getJump(k);
            newPosX+=curJ.hor;
            newPosY+=curJ.ver;

            float a=__matrix->getValue(oldPosX,oldPosY);
            float b=__matrix->getValue(newPosX,newPosY);
            err+=abs(a+b)*curJ.dis;

            oldPosX=newPosX;
            oldPosY=newPosY;
        }
        Bud nuovo;
        nuovo.dist=err;
        nuovo.xPos=oldPosX;
        nuovo.yPos=oldPosY;

        __result->append(nuovo);
    }
    return true;

}

/**
 * @brief bestPathResearch
 * @param __map of the buddies where to find the path
 * @param __path
 */

float bestPathResearch(QVector<QVector<Bud> > *__map, QVector<Bud> *__path)
{
    QString funMex="bestPathResearch: ";
    if(__map==NULL)
    {
        qDebug()<<funMex+"map not defined";
        return 0.0;
    }
    if(__path==NULL)
    {
        qDebug()<<funMex+"path not created";
        return 0.0;
    }
    if(__map->size()<1)
    {
        qDebug()<<funMex+"map dimension too small";
        return 0.0;
    }
    if(__map->at(0).size()<1)
    {
        qDebug()<<funMex+"map dimension too small";
        return 0.0;
    }
    float min=INF;
    int rowNumber=__map->size();
    int columnNumber=__map->at(0).size();
    Bud luckyOne;

    //cerco il minimo sull'ultima colonna
    for(int i=0;i<rowNumber;i++)
        if(__map->at(i).at(columnNumber-1).dist!=0)
            if(min>__map->at(i).at(columnNumber-1).dist)
            {
                luckyOne.xPos=i;
                luckyOne.yPos=columnNumber-1;
                luckyOne.dist=__map->at(i).at(columnNumber-1).dist;
                min=luckyOne.dist;
            }

    //cerco il minimo sull'ultima riga
    for(int i=0;i<columnNumber;i++)
        if(__map->at(rowNumber-1).at(i).dist!=0)
            if(min>__map->at(rowNumber-1).at(i).dist)
            {
                luckyOne.xPos=rowNumber-1;
                luckyOne.yPos=i;
                luckyOne.dist=__map->at(rowNumber-1).at(i).dist;
                min=luckyOne.dist;
            }

    int r=luckyOne.xPos;
    int c=luckyOne.yPos;
    __path->append(luckyOne);

    while(r!=0 && c!=0)
    {
        int x=__map->at(r).at(c).xPos;
        int y=__map->at(r).at(c).yPos;
        Bud step;
        step=__map->at(r).at(c);
        step.dist=__map->at(x).at(y).dist;
        __path->append(step);
        r=x;
        c=y;
    }

    return luckyOne.dist;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------Figli livello 3@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@----

void posizione(Tabble *rule,int i,int NN,Tabble *p){
    //questa funzione serve per verificare il punto di arrivo del ramo i-esimo
    /*
    int l,germIndex;
    p->Iniz(1,2,1);
    for(int k;k<NN;k++){
        germIndex=1+2*k;
        l=2+2*k;
        p->setIndex(0,0,p.getValue(0,0)+(*rule).getValue(i,germIndex));
        p->setIndex(0,1,p.getValue(0,1)+(*rule).getValue(i,l));}
*/
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---------Funzioni utili@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-




//funzione per comprimere i numeri dato che il float non mi serve tutto
//precision indica il numero di cifre massime da usare default=0 vuol
//dire che lo tengo completo

/**
 * @brief zip
 * @param __number
 * @param __precision significant digits
 * @return
 */
QByteArray zip(float __number, int __precision, ZipMode __zip)
{
    /* il bestBud byte contiene un po' di roba:
     * i primi 3 bit mi dicono quanti ALTRI byte ci sono oltre l'header
     * il quarto mi dice il segno
     * il quinto mi dice il segno dell'esponente
     * 3 bit per l'esponente
     */
    QByteArray BA;

    if(__zip==Normal)
    {
        BA.append((char *)(&__number),4);
        return BA;
    }
    if(__precision<0)
    {
        return"";
    }
    if(__precision>10)
        __precision=10;

    if(abs(__number)>99999999 || abs(__number)<0.0000001)
    {
        QByteArray b;
        b.append((char)32);
        b.append((char)0);
        return b;
    }

    float d=abs(__number);

    int exp=0;
    if(d>1)
        for(exp=0;d>10;exp++)
            d/=10;
    else
        for(exp=0;d<1;exp--)
            d*=10;


    /*Ci sono tre possibili tipi di numeri
     * 10.1 -> tipo 1 numeri da entrambe le parti della virgola
     * 0.11 -> tipo 2 numeri solo a destra
     * 11.0 -> tipo 3 numeri solo a sinistra
     */
    bool go=__precision==0?true:false;
    for(int i=0;go||i<__precision;i++)
    {
        float resto=fmod(d*10,10);
        if(resto==0)
            break;
        d*=10;
    }
    d=round(d);


    int nbyte=0;
    for (nbyte=0;d/(pow(2,8*nbyte))>=1;nbyte++);

    float pp;
    pp=floor((float)(d/(pow(2,8*(nbyte-1)))));
    BA.append((char)pp);
    d-=pp*(pow(2,8*(nbyte-1)));

    for(int i=nbyte-2;i>=0;i--)
    {
        pp=floor((float)d/(pow(2,8*i)));
        BA.append((char)pp);
        float po=pow(2,8*i);
        float diff=pp*po;
        d-=diff;
    }

    char h;

    h=(char)(abs(exp));

    if(exp<0)
        h+=8;

    if(__number<0)
        h+=16;

    //char nb=(char)(nbyte+1);
    h+=(nbyte)*32;
    BA.prepend(h);

    return BA;
}

float unzip(QByteArray *__BA, ZipMode __zip)
{
    QString funMex="unzip";
    if(__BA->isEmpty())
    {
        qDebug()<<funMex+"ByteArray empty!!";
        return NAN;
    }

    if(__zip==Normal)
    {
        return *((float *)__BA->data());
    }

    char c=__BA->at(0);

    uint nbyte=c>>5&7;
    int vsign=((c>>4)&1)==1?-1:1;
    bool esign=(c>>3)&1;
    uint exp=c&7;

    float val=0;

    for(int i=1;i<__BA->size();i++)
    {
        uchar u=__BA->at(i);
        val+=u*pow(2,8*(nbyte-i));
    }

    while(val>=10){
        val/=10;
    }
    if(esign)
        val=vsign*val/pow(10,exp);
    else
        val=vsign*val*pow(10,exp);
    return val;
}

/**
 * @brief log2
 * @param N
 * @return
 */

int log2(int N)
{         //funzione per calcolare il logaritmo in base 2 di un intero
    int k=N,i=0;

    while(k){
        k>>=1;
        i++;
    }
    return i-1;
}

/**
 * @brief check
 * @param n
 * @return
 */

int check(int n)
{       //usato per controllare se il numero di componenti del vettore di input è una potenza di 2
    return n > 0 && (n & (n-1)) == 0;
}

/**
 * @brief reverse
 * @param N
 * @param n
 * @return
 */

int reverse(int N,int n)
{ //calcola il reverse number di ogni intero n rispetto al numero massimo N
    int germIndex,p=0;
    for(germIndex=1;germIndex<=log2(N);germIndex++){
        if(n&(1<<(log2(N)-germIndex)))
            p|=1<<(germIndex-1);
    }
    return p;
}

/**
 * @brief ordina
 * @param f1
 * @param N
 */

void ordina(complex<float> *f1,int N)
{ //dispone gli elementi del vettore ordinandoli per reverse order
    complex<float> f2[N];
    for(int i=0;i<N;i++)
        f2[i]=f1[reverse(N,i)];
    for(int germIndex=0;germIndex<N;germIndex++)
        f1[germIndex]=f2[germIndex];
}

void transform(complex<float> *f,int N)
{ //calcola il vettore trasformato
    ordina(f,N);  //dapprima lo ordina col reverse order
    complex<float> W[N/2]; //vettore degli zeri dell'unità. Prima N/2-1 ma genera errore con ciclo for successivo in quanto prova a copiare in una zona non allocata "W[N/2-1]"
    W[1]=polar(1.,-2.*M_PI/N);
    W[0]=1;
    for(int i=2;i<N/2;i++)
        W[i]=pow(W[1],i);
    int n=1;
    int a=N/2;
    for(int germIndex=0;germIndex<log2(N);germIndex++){
        for(int i=0;i<N;i++){
            if(!(i&n)){
                /*ad ogni step di raddoppiamento di n, vengono utilizzati gli indici */
                /*'i' presi alternativamente a gruppetti di n, una volta si e una no.*/
                complex<float> temp=f[i];
                complex<float> Temp=W[(i*a)%(n*a)]*f[i+n];
                f[i]=temp+Temp;
                f[i+n]=temp-Temp;
            }
        }
        n*=2;
        a=a/2;
    }
}


void subFFT(complex<float> *f,int N,float d)
{
    transform(f,N);
    for(int i=0;i<N;i++)
        f[i]*=d; //moltiplica il vettore per il passo in modo da avere il vettore trasformato effettivo
}

/**
 * @brief energyFramesFinder Questa funzione viene chiamata all'interno della funzione TWI per identificare
 * esclusivamente i pezzi di segnale in cui vi sia un evento e per farlo si applica una soglia
 * di energia e si valuta quando la finestra in considerazione ha una determinata energia tale
 * da non essere considerata silenzio
 * @param __pSignal
 * @param __threshold è in percentuale tra 0 e 100
 * @param __frameDimension è la dimensione della finestra espressa in secondi
 * @return
 */
bool energyFramesFinder(MSignal *__pSignal,
                        VarMapVec *__pFrames,
                        MSignal *__pEnergy,
                        float __threshold,
                        float __frameDimension)
{
    //Controlli
    QString funMex="energyFramesFinder";
    if(__pFrames==NULL)
    {
        qDebug() << funMex+"Energy Vector not created";
        return false;
    }

    if(__pSignal==NULL || __pEnergy==NULL)
    {
        qDebug() << funMex+"Signal not created";
        return false;
    }

    int size=__pSignal->getSize();

    if(size<1)
    {
        qDebug() << "The signal is empty";
        return true;
    }
    float max=0;

    VarMap *current=new VarMap;
    QVector<float>  en;
    __frameDimension*=__pSignal->getSamplingFrequency();

    if(__frameDimension<=0 || __frameDimension >= size)
    {
        qDebug() << "Frame dimensions out of bounds: " << __frameDimension<<". I use default settings";
        __frameDimension=DEF_FRAME_DIM/1000;
    }

    __pEnergy->setSize(__pSignal->getSize());
    __pEnergy->setSamplingFrequency(__pSignal->getSamplingFrequency());
    for(int i=0;i<size-__frameDimension;i+=__frameDimension)
    {

        //ricavo l'energia della finestra
        float e=__pSignal->energy(i,__frameDimension);
        __pEnergy->setValue(i,__frameDimension,e);
        //controllo il massimo
        if(e>max)
            max=e;
        en.append(e);
    }

    __pEnergy->normalize(0,100);

    if(__threshold<=0 || __threshold >= 100)
    {
        qDebug() << "Threshold out of bounds: " << __threshold;
        return true;
    }
    float soglia=max*__threshold/100;
    int framesNumber=en.size();
    //eseguo un filtraggio per evitare almeno le finestre singole

    (*current)["xMin"]=0;
    int index=0;
    int fc=__pSignal->getSamplingFrequency();
    QString str=ANA_ENERGY;str.append("-");
    for(int i=1;i<framesNumber-2;i++)
    {

        if(en[i-1]>=soglia&&en[i]>=soglia&&en[i+1]<soglia)
        {
            (*current)["xMax"]=__pSignal->getTime((i+1)*__frameDimension);
            float lengT=(*current)["xMax"].toFloat()-(*current)["xMin"].toFloat();
            int start=(*current)["xMin"].toFloat()*fc;
            int length=lengT*fc;
            (*current)["yMax"]=__pSignal->maximum(start,length);
            (*current)["yMin"]=__pSignal->minimum(start,length);
            (*current)["popUp"]="Energy = "+QString::number(en[i]);
            (*current)["color"]="yellow";
            (*current)["family"]=__pSignal->getName();
            (*current)["tag"]=str+TYP_DEFINER;
            (*current)["resizeable"]=true;
            index++;
            __pFrames->append(current);
            current=new VarMap;
        }
        if(en[i-1]<soglia && en[i]>=soglia&&en[i+1]>=soglia)
        {
            (*current)["xMin"]=__pSignal->getTime(i*__frameDimension);
            i++;
        }


    }

    return true;

}


/**
 * @brief vectorize
 * @param __pSignal
 * @param __winLen
 * @param __coeffType
 * @return
 */
bool vectorize(Tabble *__pTab,MSignal *__pSignal, float __winDimension, int __coeffType)
{
    /* Funzione in grado di vettorizzare un segnale dividendolo in finestre e associando ad ogni finestra un vettore di
     * coefficienti. I coefficienti possono essere o provenienti da una linear predictive coding (lpc) oppure dei coefficienti
     * di riflessione cepstrali, tuttavia i primi risultano far lavorare il programma più velocemente
     *
     */

    //Controlli
    QString funMex="vectorize: ";
    int __winLen=__winDimension*__pSignal->getSamplingFrequency();

    if(__pSignal==NULL)
    {
        qDebug() << funMex+"Signal not created";
        return false;
    }

    if(__pTab==NULL)
    {
        qDebug() << funMex+"Tabble not created";
        return false;
    }

    if(__winLen>__pSignal->getSize() || __winLen<3)
    {
        qDebug() << funMex+"Wrong parameters";
        qDebug() << "__winLen = "<<__winLen;
        return false;
    }


    //Rendo la finestra divisibile per 3
    if(__winLen%3!=0)
        __winLen-=__winLen%3;

    /*******************************************************
        Definizioni delle variabili locali
       *******************************************************/

    int M=__winLen/3,P=8,f=0,size=__pSignal->getSize();
    QVector<float> a,b,LPC_Coeff;
    MSignal hamWin,filsample;

    /*******************************************************
        Costruzioni delle variabili locali
       *******************************************************/

    hamWin.setSize(__winLen);
    filsample.setSize(size);


    /*******************************************************
        Algoritmo
       *******************************************************/

    //Realizzo un segnale a forma di finestra di hamming
    hamWin.hamming();

    //Clono questo segnale per poter lavorare sul segnale filtrato
    __pSignal->cloneTo(&filsample);

    //pre enfasi è una sorta di filtraggio molto blando

    a.append(0.95);
    b.append(1);
    filsample.filter(&b,&a);

    switch(__coeffType)
    {
    case LPC:
    {
        int c1,c2;
        c1=ceil((size-__winLen)/M)+1;
        c2=P;        //c2=P*(1/2*choose+1);
        __pTab->resize(c1,c2);
        bool go=true;
        for(int i=0;i<(size-__winLen)&&go;i+=M)
        {
            MSignal section;

            filsample.getSection(&section,i,__winLen);

            section.windowing(&hamWin);
            //Calcolo i coefficienti
            QVector<float> v;
            go=lpc(&v,&section,P,__coeffType);

            for(int k=0;k<v.size();k++)
                __pTab->setValue(f,k,v[k]);
            f++;
        }
        return go;
        break;
    }
    default:return false;
    }
    return true;
}

/**
 * @brief lpc
 * @param s Signal to analyze
 * @param X Valori di autocorrelazione
 * @param __pMax Poli massimi
 * @param choose Scelta se 8 o 12 poli
 * @return LPC Coefficienti
 */
bool lpc(QVector<float> *__pVec,MSignal *__pSignal,int __pMax,bool __choose)
{
    //Controlli
    QString funMex="lpc: ";

    if(__pSignal==NULL)
    {
        qDebug() << funMex+"Signal not created";
        return false;
    }

    if(__pVec==NULL)
    {
        qDebug() << funMex+"Vector not created";
        return false;
    }

    int i, germIndex;
    float r,error,*ref=NULL,*lpc=NULL;
    MSignal *autoCorr=NULL;


    if(!__choose)
        __pMax=3/2*__pMax;




    //------- calcolo dell'autocorrelazione
    int lag=__pMax+1;

    autoCorr=new MSignal(lag);
    while(lag--){
        float d=0; /* float needed for accumulator depth */
        for(i=lag;i<__pSignal->getSize();i++)
            d+=__pSignal->getValue(i)*__pSignal->getValue(i-lag);
        autoCorr->setValue(lag,d);
    }
    //------- fine autocorrelazione

    if (autoCorr->getValue(0) == 0)
    {
        qDebug()<<funMex+"autocorrelation error";
        delete autoCorr;
        return false;
    }

    ref=new float[__pMax];
    lpc=new float[__pMax];

    error=autoCorr->getValue(0);
    for (i = 0; i < __pMax; i++)
    {

        /* Sum up this iteration's reflection coefficient.
                */
        r = -autoCorr->getValue(i+1);
        for (germIndex = 0; germIndex < i; germIndex++) r -= lpc[germIndex] * autoCorr->getValue(i - germIndex);
        ref[i] = r /= error;

        /*  Update LPC coefficients and total error.
                */
        lpc[i] = r;
        for (germIndex = 0; germIndex < i/2; germIndex++) {
            float tmp  = lpc[germIndex];
            lpc[germIndex]     += r * lpc[i-1-germIndex];
            lpc[i-1-germIndex] += r * tmp;
        }
        if (i % 2) lpc[germIndex] += lpc[germIndex] * r;

        error *= 1.0 - r * r;
    }


    for(int i=0;i<__pMax;i++){
        if(!__choose)
            __pVec->append(lpc[i]);
        else
            __pVec->append(ref[i]);
    }
    delete autoCorr;
    delete ref;
    delete lpc;
    return true;
}



/**
 * @brief FFT implements the FFT of the MSignal sig
 * @param sig
 * @param N
 * @return
 */
bool fft(MSignal *__pSignal, MSignal *__pFFT, int __fftSize)
{
    //Controlli
    QString funMex="fft - ";

    if(__pSignal==NULL)
    {
        qDebug() << funMex+"Signal not created";
        return false;
    }

    if(__pFFT == NULL)
    {
        qDebug()<<funMex+"__pFFT not created";
        return false;
    }

    if(__fftSize%2 != 0)
    {
        qDebug()<<funMex+"Wrong parameters: __fftSize is not a power of two";
        return false;
    }

    //faccio in modo che il mio segnale sia lungo una potenza di 2
    int due=2;
    int inc=2;
    while(due*inc<__pSignal->getSize())
        inc*=2;
    if(due*inc<=__fftSize)
        inc=2;
    due*=inc;
    complex<float> *vec;
    vec=new complex<float>[due];

    for(int i=0;i<due;i++)
    {
        if(i<__pSignal->getSize())
            vec[i]=__pSignal->getValue(i);
        else
            vec[i]=0;
    }

    subFFT(vec,due,__pSignal->getSamplingPeriod());

    __pFFT->setSize(__fftSize+1);

    int index=0;
    bool ok=true;
    /* se due = 256 ho 128 + 1 elemento da valutare */
    __pFFT->setValue(index,pow(vec[0].real()*vec[0].real()+vec[0].imag()*vec[0].imag(),0.5));
    index++;
    for(int i=inc/__fftSize;i<due/2+1 && ok;i+=inc/__fftSize)
    {
        ok=__pFFT->setValue(index,pow(vec[i].real()*vec[i].real()+vec[i].imag()*vec[i].imag(),0.5));
        index++;
    }
    __pFFT->normalize();
    float min=__pFFT->minimum();
    __pFFT->customOperator('-',min);
    __pFFT->setSamplingPeriod(__pSignal->getSamplingFrequency()/(2*__fftSize));
    delete vec;
    return true;
}



