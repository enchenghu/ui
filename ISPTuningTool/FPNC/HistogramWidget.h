#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QVector>
#include <QVector2D>


class HistogramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget *parent = nullptr);
    
    void showAccurateHistogram(const QVector<int>& data, 
                            double minEdge, double maxEdge, 
                            const QString& title);

    void showAccurateBarHistogram(const QVector<int>& data, 
                      double minEdge, double maxEdge, 
                      const QString& title);                                            

private:
    QChartView *chartView;
    QChart *chart;
    
    // QVector<double> flattenMatrix(const QVector<QVector<double>>& matrix);
    // QVector<int> computeHistogram(const QVector<double>& data, 
    //                             double minEdge, double maxEdge, 
    //                             int& binCount);
};

#endif // HISTOGRAMWIDGET_H