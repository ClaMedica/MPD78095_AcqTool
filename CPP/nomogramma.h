#ifndef NOMOGRAMMA_H
#define NOMOGRAMMA_H

#include <QQuickItem>
#include <QMap>

//FLW ADV
#define G_LIVERPOOL_MAX 2
#define G_LIVERPOOL_AVE 3

class Nomogramma : public QObject
{
    Q_OBJECT
public:
    Nomogramma(QString __tipoAna, int __tipoNomograma);
    ~Nomogramma();

    Q_PROPERTY(QVariantList tracce READ getTracce WRITE setTracce)
    Q_INVOKABLE QVariantList getTracce(){return m_tracce;}
    Q_INVOKABLE void setTracce(QVariantList __tr){m_tracce = __tr;}

    Q_PROPERTY(QVariantList colors READ getColors WRITE setColors)
    Q_INVOKABLE QVariantList getColors(){return m_colors;}
    Q_INVOKABLE void setColors(QVariantList __cl){m_colors = __cl;}

    QVector<int> getLineX();
    QVector<double> getLineY(int __num);

    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_INVOKABLE void setTitle(QString __title){m_title = __title;}
    Q_INVOKABLE QString getTitle(){return m_title;}

    Q_PROPERTY(QString unitx READ getUnitx WRITE setUnitx)
    Q_INVOKABLE void setUnitx(QString __unitx){m_unit_x = __unitx;}
    Q_INVOKABLE QString getUnitx(){return m_unit_x;}

    Q_PROPERTY(QString unity READ getUnity WRITE setUnity)
    Q_INVOKABLE void setUnity(QString __unity){m_unit_y = __unity;}
    Q_INVOKABLE QString getUnity(){return m_unit_y;}

    Q_PROPERTY(double datox READ getDatoX WRITE setDatoX)
    Q_INVOKABLE void setDatoX(double __dato){m_datoX = __dato;}
    Q_INVOKABLE double getDatoX(){return m_datoX;}

    Q_PROPERTY(double datoy READ getDatoY WRITE setDatoY)
    Q_INVOKABLE void setDatoY(double __dato){m_datoY = __dato;}
    Q_INVOKABLE double getDatoY(){return m_datoY;}

    Q_PROPERTY(double xmax READ getXmax WRITE setXmax)
    Q_INVOKABLE double getXmax(){return m_xMax;}
    Q_INVOKABLE void setXmax(double __max){m_xMax = __max;}

    Q_PROPERTY(double xmin READ getXmin WRITE setXmin)
    Q_INVOKABLE double getXmin(){return m_xMin;}
    Q_INVOKABLE void setXmin(double __min){m_xMin = __min;}

    Q_PROPERTY(double ymax READ getYmax WRITE setYmax)
    Q_INVOKABLE double getYmax(){return m_yMax;}
    Q_INVOKABLE void setYmax(double __max){m_yMax = __max;}

    Q_PROPERTY(double ymin READ getYmin WRITE setYmin)
    Q_INVOKABLE double getYmin(){return m_yMin;}
    Q_INVOKABLE void setYmin(double __min){m_yMin = __min;}

    void setNumLines(int __num);
    int getNumLines(){return m_numLines;}

    void addToLinex(int __i, double __val);
    void addToLiney(int __i, double __val);

signals:

public slots:
    //QVariantList getData(QString __type);//ottengo la lista per disegnare le tracce

protected:
    //QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    //void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    QString m_tipoAna;
    int m_tipoNomogramma;
    QString m_title;

    int m_numLines;
    QMap<int,QVector<double> > m_pLinex;
    QMap<int,QVector<double> > m_pLiney;

    double m_xMax;
    double m_xMin;
    double m_yMax;
    double m_yMin;

    double m_datoX;
    double m_datoY;

    QString m_unit_y;
    QString m_unit_x;

    QVariantList m_tracce;
    QVariantList m_colors;
};







#endif
