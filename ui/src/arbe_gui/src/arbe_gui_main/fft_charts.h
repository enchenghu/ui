#ifndef CHART_H
#define CHART_H

#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QHBoxLayout>
#include <QValueAxis>
#include "fft_chartview.h"
#include "qcustomplot.h"
#include <QTimer>
#include <unistd.h>
#include <iostream>
#include "plot_tracer.h"
#include <QGridLayout>

QT_CHARTS_USE_NAMESPACE
class ChartFFT : public QWidget
{
    Q_OBJECT

    QCustomPlot *pCustomPlot;
	myTracer* plotTracer;

    QChart *qchart;
    ChartView *chartview;
    QSplineSeries *series;

    QHBoxLayout *layout;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QString chartname;
    //坐标轴参数
    QString xname;
    qreal xmin;
    qreal xmax;
    int xtickc;
    QString yname;
    qreal ymin;
    qreal ymax;
    int ytickc;
    QTimer* timer_;

public:
    ChartFFT(QWidget* parent = nullptr);
    ~ChartFFT(){}
    void setData(const QVector<double> &x, const QVector<double> &y, bool rescalse_, int type);

    void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
                 QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
    void buildChart();
    void updateChart();
    void showTracer(QMouseEvent* event);
    void setChart(int xmin, int xmax, int ymin, int ymax, QGridLayout* chartLayout);

private:
    QVector<double> x_FFT;
	QVector<double> y_FFT;
	QVector<double> x_FFT_1;
	QVector<double> y_FFT_1;

};
#endif // CHART_H

