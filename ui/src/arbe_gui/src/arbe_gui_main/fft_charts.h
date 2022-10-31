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
#include "pc_data.h"
QT_CHARTS_USE_NAMESPACE
class ChartFFT : public QWidget
{
    Q_OBJECT
    QCustomPlot *pCustomPlot;
	myTracer* plotTracer;
    QTimer* timer_;
public:
    ChartFFT(QWidget* parent = nullptr);
    ~ChartFFT(){}
    void setData(const QVector<double> &x, const QVector<double> &y);
    void showTracer(QMouseEvent* event);
    QCustomPlot* setChart(int xmin, int xmax, int ymin, int ymax);
    void setShowType(int);
    void setIfScale(bool);
    void setSingleShow(bool);
    void setContineFlag(bool);

private:
    bool rescale_;
    int showType_;
    bool singleShow_;
    bool contineFlag_;
};
#endif // CHART_H

