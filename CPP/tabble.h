#ifndef TABBLE_H
#define TABBLE_H

#include <math.h>
#include <QVector>

/**
 * @brief La struct point_Tabble definisce un valore con posizione
 */
struct PointTabble
{
    float val;
    int row;
    int col;
};
class Tabble
{
public:
    Tabble();
    Tabble(int __nRows, int __nColumns, float __defValue=0);
    ~Tabble();
    bool resize(int __nRows, int __nColumns, float __defValue=0);
    int columns(void);
    int rows(void);
    float getValue(int __row,int __column);
    bool setValue(int __row,int __column, float v);
    bool addColumns(int num,float __defValue=NAN);
    bool addRows(int __count, float __defValue=NAN);
    bool removeRows(int __fromRow,int __rowCount);
    bool removeColumns(int __fromColumn,int __columnCount);
    bool fill(int __fromRow,int __fromColumn,int __rowCount,int __columnCount,float __value);
    bool tabblePart(Tabble *__pTab,int __fromRow,int __fromColumn,int __rowCount,int __columnCount);
    bool copyTo(Tabble *__pTab);
    bool transpose(Tabble *__pTab);
    float sum(int __fromRow,int __fromColumn,int __rowCount,int __columnCount);
    float sum();
    PointTabble min();
    PointTabble max();
    bool customOperator(char __operator, Tabble *__pTab);
    bool customOperator(char __operator, float __value);
    bool logaritm(Tabble *__pTab);
    bool distance(Tabble *__pTab,Tabble *__pTabDistance);

private:
    /// Puntatore doppio della tabella di float
    QVector< QVector<float> > m_tab;
    /// Numero di colonne
    int m_col;
    /// Numero di righe
    int m_row;
    /// Minimo
    PointTabble m_min;
    /// Massimo
    PointTabble m_max;
    /// Funzione per l'aggiornamento dei valori speciali
    void updateData(void);
    bool m_needsToUpdate;


};

#endif // TABBLE_H
