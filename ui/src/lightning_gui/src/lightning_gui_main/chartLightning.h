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
#include <vector>
QT_CHARTS_USE_NAMESPACE

static QVector<QColor> colotList = {{0,255,255}, {0,255,0}, {0,0,255}, {255,0,0},{255,0,255}, \
                                    {255,125,64}, {255,255,0}, {128,42,42}, {218,112,214}, {34,139,34}};

class ChartLighting : public QWidget
{
    Q_OBJECT
    QCustomPlot *pCustomPlot;
	myTracer* plotTracer;
    QTimer* timer_;
public:
    ChartLighting(QWidget* parent = nullptr, showModel type = FFT_ORI);
    ~ChartLighting(){}
    //void setData(const QVector<double> &x, const QVector<double> &y);
    void setData(const QVector<double> &x, const QVector<double> &y, uint8_t index_graph = 0);

    void showTracer(QMouseEvent* event);
    QCustomPlot* setChart(int xmin, int xmax, int ymin, int ymax);
    void setShowType(showModel);
    void setIfScale(bool);
    void setSingleShow(bool);
    void setContineFlag(bool);
    void setXChart(int xmin, int xmax);
    void setGraph(uint8_t index_graph);
    void setGraphShow(uint8_t index_graph, bool show);

private:
    bool rescale_;
    showModel showType_;
    bool singleShow_;
    bool contineFlag_;
    QCPGraph* pgraph_motor[MOTOR_ITEMS_NUM];
};
#endif // CHART_H

