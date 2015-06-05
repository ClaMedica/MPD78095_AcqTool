#ifndef FUNCTIONHANDLER_H
#define FUNCTIONHANDLER_H

#include <QObject>
#include <msignal.h>
#include <global.h>
#include <tabble.h>

QByteArray zip(float __number, int __precision=0,ZipMode __zip=Normal);
float unzip(QByteArray *__BA,ZipMode __zip=Normal);
int log2(int N);
int check(int n);
int reverse(int N,int n);
void ordina(complex<float> *f1,int N);
void transform(complex<float> *f,int N);
void subFFT(complex<float> *f,int N,float d);
bool energyFramesFinder(MSignal *__pSignal, VarMapVec *__pFrames, MSignal *__pEnergy, float __threshold, float __windowDimension);
bool fft(MSignal *__pSignal, MSignal *__pFFT, int __fftSize);
bool vectorize(Tabble *__pTab, MSignal *__pSignal, float __winDimension, int __coeffType);
bool lpc(QVector<float> *__pVec,MSignal *__pSignal,int __pMax,bool __choose);
bool checkrec(int,int,Tabble *);
float bestPathResearch(BudMap *__map,BudVector *__path);
float pathFinder(Tabble *__distorsionMatrix, Rule *__rule, QVector<Bud> *__path);
void posizione(Tabble *,int ,int,Tabble *);
bool germination(Tabble *__matrix, Rule *__rule, QVector<Bud> *__result);
void energy(Tabble *,int,Tabble *,Tabble *);
bool distanceMatrix(Tabble *__firstMatrix,Tabble *__secondMatrix,Tabble *__resultMatrix);
bool TWI(MSignal *__pSamglob, MSignal *__pSamcamp, int __threshold, float __windowsDimension,VarMapVec *__result);

#endif // FUNCTIONHANDLER_H
