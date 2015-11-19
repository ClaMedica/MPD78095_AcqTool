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
//    for (int i=0; i<__num; i++)
//    {
//        m_pLinex.
//    }

}

void Nomogramma::addToLinex(int __i, double __val)
{
    m_pLinex[__i].append(__val);
}

void Nomogramma::addToLiney(int __i, double __val)
{
    m_pLiney[__i].append(__val);
}

QVector<int> Nomogramma::getLineX()
{
    QVector<int> datas;
    for (int i=0; i<m_pLinex.first().length(); i++)
        datas<<m_pLinex.first().at(i);

    return datas;
}

QVector<double> Nomogramma::getLineY(int __num)
{
    QVector<double> datas;
    for (int i=0; i<m_pLiney[__num].length(); i++)
        datas<<m_pLiney[__num].at(i);

    return datas;
}

