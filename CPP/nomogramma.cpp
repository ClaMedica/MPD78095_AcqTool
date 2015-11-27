#include "nomogramma.h"



Nomogramma::Nomogramma(QString __tipoAna, int __tipoNomograma)
{
    m_tipoAna = __tipoAna;
    m_tipoNomogramma = __tipoNomograma;
}

Nomogramma::~Nomogramma()
{
}

void Nomogramma::setNumLines(int __num)
{
    m_numLines = __num;
}


void Nomogramma::addToLiney(int __i, double __val)
{
    m_pLiney[__i].append(__val);
}


QVector<double> Nomogramma::getLineY(int __num)
{
    QVector<double> datas;
    for (int i=0; i<m_pLiney[__num].length(); i++)
        datas<<m_pLiney[__num].at(i);

    return datas;
}

QVariantList Nomogramma::getXpoints()
{
    QVariantList data;
    for (int i=0; i< m_pLiney[0].length();i++)
        data<<i;
    return data;
}

QVariantList Nomogramma::getYpoints()
{
    QVariantList data;
    for (int j=0; j<m_aline.length();j++)
        for (int i=0; i< m_pLiney[m_aline.at(j)].size();i++)
            data<<m_pLiney[m_aline.at(j)].at(i);

    return data;
}
