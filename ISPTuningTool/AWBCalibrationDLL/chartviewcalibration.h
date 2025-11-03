#ifndef CHARTVIEWCALIBRATION_H
#define CHARTVIEWCALIBRATION_H

#include <QChart >
#include "QScatterSeries "
#include <QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include "QLabel"

struct PointInfo{
    float x;
    float y;
};

class ChartViewCalibration : public QChart
{
public:
    ChartViewCalibration(std::vector<PointInfo> vec,PointInfo middlePt);
    std::vector<PointInfo> getDestPointList();
protected:
    void    mousePressEvent(QGraphicsSceneMouseEvent *event);
    void    mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void    mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    std::vector<PointInfo> _vecPoint;
    std::vector<PointInfo> _vecResultPoint;
    PointInfo              _middlePt;
    QLineSeries*           _lineSeries;
    bool                   _isRect;
    QScatterSeries  *scatterSeries=0;
    QScatterSeries* redPointSeries =0;
    QValueAxis*axisX ;
    QValueAxis*axisY ;
    QLabel*           _firstX;
    QLabel*           _firstY;
    QLabel*           _secondX;
    QLabel*           _secondY;
};

#endif // CHARTVIEWCALIBRATION_H
