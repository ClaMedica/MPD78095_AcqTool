#include "tabble.h"
#include "QVector"
#include "qdebug.h"

Tabble::Tabble()
{
    m_row=1;
    m_col=1;

    QVector<float> vec;
    vec.append(0);
    m_tab.append(vec);
    m_needsToUpdate=true;
}

/**
 * @brief Tabble::Tabble
 * @param __nRows
 * @param __nColumns
 * @param __defValue
 */

Tabble::Tabble(int __nRows, int __nColumns,float __defValue)
{
    if(__nRows <=0 || __nColumns <=0)
    {
        qDebug() << "Tabble::Tabble: Wrong parameters";
        qDebug() << "__nRows" << __nRows;
        qDebug() << "__nColumns"<<__nColumns;
        return;
    }

    m_row=__nRows;
    m_col=__nColumns;

    for(int i=0;i<__nRows;i++)
    {
        QVector<float> vec;
        vec.fill(__defValue,__nColumns);
        m_tab.append(vec);
    }
    m_needsToUpdate=true;
}

Tabble::~Tabble()
{

}

/**
 * @brief Tabble::resize it is a destructive function, what was previous storend in the tabble will be lost
 * @param __nRow
 * @param __nCol
 * @return
 */

bool Tabble::resize(int __nRows, int __nColumns, float __defValue)
{
    if(__nRows <=0 || __nColumns <=0)
    {
        qDebug() << "Tabble::resize: Wrong parameters";
        qDebug() << "__nRows" << __nRows;
        qDebug() << "__nColumns"<<__nColumns;
        return false;
    }

    m_row=__nRows;
    m_col=__nColumns;

    m_tab.clear();

    for(int i=0;i<__nRows;i++)
    {
        QVector<float> vec;
        vec.fill(__defValue,__nColumns);
        m_tab.append(vec);
    }
    m_needsToUpdate=true;
    return true;
}

/**
 * @brief Tabble::columns
 * @return
 */
int Tabble::columns(void)
{
    return m_col;
}
/**
 * @brief Tabble::rows
 * @return
 */
int Tabble::rows(void)
{
    return m_row;
}
/**
 * @brief Tabble::getVal
 * @param r Riga
 * @param c Colonna
 * @return Valore della riga <b>r</b> e colonna <b>c</b>
 */
float Tabble::getValue(int __row,int __column)
{
    if(__row > m_row || __row < 0 || __column > m_col || __column < 0)
    {
        qDebug() << "Tabble::getValue: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__row"<<__row;
        qDebug() << "__column"<<__column;
        return NAN;
    }
    return m_tab.at(__row).at(__column);
}

/**
 * @brief Tabble::setValue
 * @param __row
 * @param __column
 * @param __value
 * @return
 */

bool Tabble::setValue(int __row,int __column, float __value)
{
    if(__row > m_row || __row < 0 || __column > m_col || __column < 0)
    {
        qDebug() << "Tabble::setValue: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__row"<<__row;
        qDebug() << "__column"<<__column;
        return false;
    }
    m_tab[__row].replace(__column,__value);
    m_needsToUpdate=true;
    return true;
}

/**
 * @brief Tabble::addColumns
 * @param __count
 * @param __defValue
 * @return
 */

bool Tabble::addColumns(int __count,float __defValue)
{
    if(__count <= 0)
    {
        qDebug() << "Tabble::addColumns: Wrong parameters";
        qDebug() << "__count"<<__count;
        return false;
    }

    for(int i=0;i<m_tab.size();i++)
    {
        m_tab[i].insert(m_col,__count,__defValue);
    }
    m_col+=__count;

    if(__defValue > m_max.val)
    {
        m_max.col=__defValue;
        m_max.col=m_col-1;
        m_max.row=m_row-1;
    }
    if(__defValue < m_min.val)
    {
        m_min.val=__defValue;
        m_min.col=m_col-1;
        m_min.row=m_row-1;
    }

    return true;
}

/**
 * @brief Tabble::addRows
 * @param __count
 * @param __defValue
 * @return
 */

bool Tabble::addRows(int __count, float __defValue)
{
    if(__count <= 0)
    {
        qDebug() << "Tabble::addRows: Wrong parameters";
        qDebug() << "__count"<<__count;
        return false;
    }

    QVector<float> vec;
    vec.fill(__defValue,m_col);

    for(int i=0;i<__count;i++)
        m_tab.append(vec);

    if(__defValue > m_max.val)
    {
        m_max.col=__defValue;
        m_max.col=m_col-1;
        m_max.row=m_row-1;
    }
    if(__defValue < m_min.val)
    {
        m_min.val=__defValue;
        m_min.col=m_col-1;
        m_min.row=m_row-1;
    }

    m_row+=__count;
    return true;
}

/**
 * @brief Tabble::fill
 * @param __fromRow
 * @param __fromColumn
 * @param __rowCount
 * @param __columnCount
 * @param __value
 * @return
 */

bool Tabble::fill(int __fromRow, int __fromColumn, int __rowCount, int __columnCount, float __value)
{
    if(__fromRow < 0 || __fromColumn < 0 || __rowCount <= 0 || __columnCount <= 0 ||
            __fromRow + __rowCount >= m_row || __fromColumn + __columnCount >= m_col)
    {
        qDebug() << "Tabble::fill: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() <<"__fromRow"<<__fromRow;
        qDebug() <<"__fromColumn"<<__fromColumn;
        qDebug() <<"__rowCount"<<__rowCount;
        qDebug() <<"__columnCount"<<__columnCount;
        return false;
    }

    for(int i=0;i<__rowCount;i++)
        for(int j=0;j<__columnCount;j++)
            m_tab[__fromRow+i].replace(__fromColumn+j,__value);

    if(__value > m_max.val)
    {
        m_max.col=__value;
        m_max.col=m_col-1;
        m_max.row=m_row-1;
    }
    if(__value < m_min.val)
    {
        m_min.val=__value;
        m_min.col=m_col-1;
        m_min.row=m_row-1;
    }

    return true;
}

/**
 * @brief Tabble::tabblePart
 * @param __tab
 * @param __fromRow
 * @param __fromColumn
 * @param __rowCount
 * @param __columnCount
 * @return
 */

bool Tabble::tabblePart(Tabble *__pTab, int __fromRow, int __fromColumn, int __rowCount, int __columnCount)
{
    if(__fromRow < 0 || __fromColumn < 0 || __rowCount <= 0 || __columnCount <= 0 ||
            __fromRow + __rowCount > m_row || __fromColumn + __columnCount > m_col)
    {
        qDebug() << "Tabble::tabblePart: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() <<"__fromRow"<<__fromRow;
        qDebug() <<"__fromColumn"<<__fromColumn;
        qDebug() <<"__rowCount"<<__rowCount;
        qDebug() <<"__columnCount"<<__columnCount;
        return false;
    }

    if(__pTab == NULL)
    {
        qDebug() << "Tabble not created in Tabble::tabblePart";
        return false;
    }

    __pTab->resize(__rowCount,__columnCount);

    for(int i=0;i<__rowCount;i++)
        for(int j=0;j<__columnCount;j++)
            __pTab->setValue(i,j,m_tab[i+__fromRow].at(j+__fromColumn));

    return true;
}
/**
 * @brief Tabble::copyTo
 * @param __tab
 * @return
 */

bool Tabble::copyTo(Tabble *__pTab)
{
    if(__pTab == NULL)
    {
        qDebug() << "Tabble not created in Tabble::copyTo";
        return false;
    }

    return this->tabblePart(__pTab,0,0,m_row,m_col);
}

/**
 * @brief Tabble::update_data
 */

void Tabble::updateData(void)
{
    if(m_needsToUpdate)
    {
        m_min.val=m_tab[0].value(0);
        m_max.val=m_tab[0].value(0);
        m_min.col=0;
        m_min.row=0;
        m_max.col=0;
        m_max.row=0;

        for(int i=0;i<m_row;i++)
            for(int j=0;j<m_col;j++)
            {
                if(m_min.val>m_tab[i].at(j))
                {
                    m_min.val=m_tab[i].at(j);
                    m_min.row=i;
                    m_min.col=j;
                }
                if(m_max.val<m_tab[i].at(j))
                {
                    m_max.val=m_tab[i].at(j);
                    m_max.row=i;
                    m_max.col=j;
                }
            }
        m_needsToUpdate=false;
    }
}

/**
 * @brief Tabble::min
 * @return
 */

PointTabble Tabble::min()
{
    updateData();
    return m_min;
}

/**
 * @brief Tabble::max
 * @return
 */

PointTabble Tabble::max()
{
    updateData();
    return m_max;
}

/**
 * @brief Tabble::transpose Esegue la trasposta della Tabble come se fosse una matrice
 */

bool Tabble::transpose(Tabble *__pTab)
{

    if(__pTab == NULL)
    {
        qDebug() << "Tabble not created in Tabble::transpose";
        return false;
    }

    __pTab->resize(m_col,m_row);

    for(int i=0;i<m_row;i++)
        for(int j=0;j<m_col;j++)
        {
            __pTab->setValue(j,i,m_tab[i].at(j));
        }

    return true;
}

/**
 * @brief Tabble::sum
 * @param __fromRow
 * @param __fromColumn
 * @param __rowCount
 * @param __columnCount
 * @return
 */

float Tabble::sum(int __fromRow, int __fromColumn, int __rowCount, int __columnCount)
{
    if(__fromRow < 0 || __fromColumn < 0 || __rowCount <= 0 || __columnCount <= 0 ||
            __fromRow + __rowCount > m_row || __fromColumn + __columnCount > m_col)
    {
        qDebug() << "Tabble::sum: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__fromRow"<<__fromRow;
        qDebug() << "__fromColumn"<<__fromColumn;
        qDebug() << "__rowCount"<<__rowCount;
        qDebug() << "__columnCount"<<__columnCount;
        return NAN;
    }

    float somma=0;

    for(int i=0;i<__rowCount;i++)
        for(int j=0;j<__columnCount;j++)
            somma+=this->getValue(i+__fromRow,j+__fromColumn);

    return somma;

}

/**
 * @brief Tabble::sum
 * @return
 */

float Tabble::sum()
{
    return this->sum(0,0,m_row,m_col);
}

/**
 * @brief Tabble::customOperator
 * @param __value
 * @param __operator
 */

bool Tabble::customOperator(char __operator,float __value)
{    

    if(__operator=='/' && __value==0)
    {
        qDebug() << "Tabble::customOperator: Division by 0";
        return false;
    }
    for(int i=0;i<m_row;i++)
        for(int j=0;j<m_col;j++)
        {
            switch(__operator)
            {
            case '+':
                this->setValue(i,j,this->getValue(i,j)+__value);
                break;
            case '-':
                this->setValue(i,j,this->getValue(i,j)-__value);
                break;
            case '*':
                this->setValue(i,j,this->getValue(i,j)*__value);
                break;
            case '/':
                this->setValue(i,j,this->getValue(i,j)/__value);
                break;
            case '^':
                this->setValue(i,j,pow(this->getValue(i,j),__value));
                break;
            default:return false;
            }
        }

    return true;
}

/**
 * @brief Tabble::removeRows
 * @param __fromRow
 * @param __rowCount
 * @return
 */

bool Tabble::removeRows(int __fromRow, int __rowCount)
{
    if(__fromRow < 0 || __rowCount <= 0  || __fromRow + __rowCount >= m_row)
    {
        qDebug() << "Tabble::removeRows: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__fromRow"<<__fromRow;
        qDebug() << "__rowCount"<<__rowCount;

        return false;
    }
    for(int i=0;i<__rowCount;i++)
        m_tab.remove(__fromRow+i);
    m_row-=__rowCount;
    return true;
}

/**
 * @brief Tabble::removeColumns
 * @param __fromColumn
 * @param __columnCount
 * @return
 */

bool Tabble::removeColumns(int __fromColumn, int __columnCount)
{
    if(__fromColumn < 0 || __columnCount <= 0  || __fromColumn + __columnCount >= m_col)
    {
        qDebug() << "Tabble::removeColumns: Wrong parameters";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__fromColumn"<<__fromColumn;
        qDebug() << "__columnCount"<<__columnCount;
        return false;
    }
    for(int i=0;i<__columnCount;i++)
        m_tab.remove(__fromColumn+i);
    m_col-=__columnCount;
    return true;
}

/**
 * @brief Tabble::customOperator
 * @param __operator
 * @param __tab
 * @return
 */

bool Tabble::customOperator(char __operator, Tabble *__pTab)
{
    if(__pTab == NULL)
    {
        qDebug() << "Tabble not created in Tabble::customOperator";
        return false;
    }

    if(m_col != __pTab->columns() || m_row != __pTab->rows())
    {
        qDebug() << "Tabble::customOperator: Tabbles sizes do not match";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__pTab size"<<__pTab->rows()<<"x"<<__pTab->columns();
        return false;
    }

    if(__operator=='/')
        for(int i=0;i<__pTab->rows();i++)
            for(int j=0;j<__pTab->columns();j++)
                if(__pTab->getValue(i,j)==0)
                {
                    qDebug()<<"Tabble::customOperator: Division by 0 at pos"<<i<<"x"<<j;
                    return false;
                }

    if(__operator=='^')
        for(int i=0;i<__pTab->rows();i++)
            for(int j=0;j<__pTab->columns();j++)
                if(__pTab->getValue(i,j)<=0)
                {
                    qDebug()<<"Tabble::customOperator: power less than 0 at pos"<<i<<"x"<<j<<"val="<<__pTab->getValue(i,j);
                    return false;
                }

    for(int i=0;i<m_row;i++)
        for(int j=0;j<m_col;j++)
        {
            float a=this->getValue(i,j);
            float b=__pTab->getValue(i,j);

            switch(__operator)
            {
            case '+':
                this->setValue(i,j,a+b);
                break;
            case '-':
                this->setValue(i,j,a-b);
                break;
            case '*':
                this->setValue(i,j,a*b);
                break;
            case '/':
                this->setValue(i,j,a/b);
                break;
            case 'p':
                this->setValue(i,j,pow(a,b));
                break;
            default:return false;
            }
        }
    return true;
}

/**
 * @brief Tabble::log
 * @param __tab
 */

bool Tabble::logaritm(Tabble *__pTab)
{
    if(__pTab == NULL)
    {
        qDebug() << "Tabble not created in Tabble::log";
        return false;
    }
    __pTab->resize(m_row,m_col);

    for(int i=0;i<m_row;i++)
        for(int j=0;j<m_col;j++)
            __pTab->setValue(i,j,log(this->getValue(i,j)));

    return true;
}

/**
 * @brief Tabble::distance
 * @param __tab
 * @param __tabDistance
 */

bool Tabble::distance(Tabble *__pTab, Tabble *__pTabDistance)
{
    if(__pTab == NULL || __pTabDistance == NULL)
    {
        qDebug() << "Tabble not created in Tabble::distance";
        return false;
    }

    if(m_col != __pTab->columns())
    {
        qDebug() << "Tabble::distance: Tabbles sizes do not match";
        qDebug() << "Tabble size"<<m_row<<"x"<<m_col;
        qDebug() << "__pTab size"<<__pTab->rows()<<"x"<<__pTab->columns();
        return false;
    }

    int m=m_row;
    int n=__pTab->rows();
    float s;

    __pTabDistance->resize(m,n);

    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
        {
            s=0;
            for(int k=0;k<m_col;k++)
                s+=pow(__pTab->getValue(j,k)-this->getValue(i,k),2);
            __pTabDistance->setValue(i,j,s);

        }

    return true;
}

