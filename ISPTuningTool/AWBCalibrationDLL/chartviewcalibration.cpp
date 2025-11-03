#include "chartviewcalibration.h"
#include "qgraphicssceneevent.h"
#include "windows.h"

ChartViewCalibration::ChartViewCalibration(std::vector<PointInfo> vec,PointInfo middlePt)
{
    _isRect=false;
    _vecPoint=vec;
    _middlePt=middlePt;
    axisX = new QValueAxis;
    axisY = new QValueAxis;
    //设置坐标范围
    sort(_vecPoint.begin(),_vecPoint.end(),[&](const PointInfo&a,const PointInfo&b)->bool{
        return a.x<b.x;
    });
    float minWidth=_vecPoint[0].x;
    float maxWidth=_vecPoint[_vecPoint.size()-1].x;
    float width=maxWidth-minWidth;

    axisX->setRange(minWidth-width*0.1,maxWidth+width*0.1);
    axisX->setTickCount(10);

    sort(_vecPoint.begin(),_vecPoint.end(),[&](const PointInfo&a,const PointInfo&b)->bool{
        return a.y<b.y;
    });
    float minHeight=_vecPoint[0].y;
    float maxHeight=_vecPoint[_vecPoint.size()-1].y;
    float height=maxHeight-minHeight;
    axisY->setRange(minHeight-height*0.1,maxHeight+height*0.1);
    axisY->setTickCount(10);
    //将坐标加入图表,并设置坐标位置
    this->addAxis(axisX,Qt::AlignBottom);
    this->addAxis(axisY,Qt::AlignLeft);



    //将序列加入图表
    //关联xy轴,将序列加入自己的xy坐标(attachAxis)
    scatterSeries = new QScatterSeries;
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);//设置散点形状为圆
    scatterSeries->setColor(QColor(0,255,0));
    scatterSeries->setBorderColor(QColor(0,255,0));
    for(int i = 0; i < _vecPoint.size(); i++)// 添加点
    {
        scatterSeries->append(_vecPoint[i].x,_vecPoint[i].y);
    }
    scatterSeries->setUseOpenGL(true);
    scatterSeries->setMarkerSize(2);
    this->addSeries(scatterSeries);
    scatterSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisY);

    _lineSeries = new QLineSeries;
    this->addSeries(_lineSeries);
    _lineSeries->attachAxis(axisX);
    _lineSeries->attachAxis(axisY);

    //中心红点
    redPointSeries = new QScatterSeries;
    redPointSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);//设置散点形状为圆
    redPointSeries->setColor(QColor(255,0,0));
    redPointSeries->setBorderColor(QColor(255,0,0));
    redPointSeries->setMarkerSize(6);
    redPointSeries->setUseOpenGL(true);
    redPointSeries->append(_middlePt.x,_middlePt.y);
    this->addSeries(redPointSeries);
    redPointSeries->attachAxis(axisX);
    redPointSeries->attachAxis(axisY);
    this->legend()->hide();
}

std::vector<PointInfo> ChartViewCalibration::getDestPointList()
{
    if(_vecResultPoint.size()<2)
    {
        return std::vector<PointInfo>(0);
    }
    int nCount=0;
    double left=_vecResultPoint[0].x<_vecResultPoint[1].x?_vecResultPoint[0].x:_vecResultPoint[1].x;
    double right=_vecResultPoint[0].x>_vecResultPoint[1].x?_vecResultPoint[0].x:_vecResultPoint[1].x;
    double top=_vecResultPoint[0].y>_vecResultPoint[1].y?_vecResultPoint[0].y:_vecResultPoint[1].y;
    double bottom=_vecResultPoint[0].y<_vecResultPoint[1].y?_vecResultPoint[0].y:_vecResultPoint[1].y;
    for (int var = 0; var < _vecPoint.size(); ++var) {
        if(_vecPoint[var].x>left&&_vecPoint[var].y<top&&
            _vecPoint[var].x<right&&_vecPoint[var].y>bottom)
        {
            nCount++;
        }
    }
    float fNum=(nCount*1.0)/(_vecPoint.size()*1.0);
    if(fNum<0.6)
    {
        return std::vector<PointInfo>(0);
    }
    return _vecResultPoint;
}


void ChartViewCalibration::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //选取范围点
    if (event->button() == Qt::LeftButton) {
        QPointF point = event->pos();
        point=this->mapToValue(point,_lineSeries);
        PointInfo info;
        info.x=point.x();
        info.y=point.y();
        _vecResultPoint.resize(0);
        _vecResultPoint.push_back(info);
        _lineSeries->clear();
        _lineSeries->append(info.x,info.y);
        _isRect=true;
    }
    update();
    return;
}

void ChartViewCalibration::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF point = event->pos();
    point=this->mapToValue(point,_lineSeries);
    if(_isRect)
    {

        _lineSeries->clear();
        _lineSeries->append(_vecResultPoint[0].x,_vecResultPoint[0].y);
        _lineSeries->append(point.x(),_vecResultPoint[0].y);
        _lineSeries->append(point.x(),point.y());
        _lineSeries->append(_vecResultPoint[0].x,point.y());
        _lineSeries->append(_vecResultPoint[0].x,_vecResultPoint[0].y);
    }
    update();
}

void ChartViewCalibration::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(_isRect)
    {
        QPointF point = event->pos();
        point=this->mapToValue(point,_lineSeries);
        _lineSeries->clear();
        _lineSeries->append(_vecResultPoint[0].x,_vecResultPoint[0].y);
        _lineSeries->append(point.x(),_vecResultPoint[0].y);
        _lineSeries->append(point.x(),point.y());
        _lineSeries->append(_vecResultPoint[0].x,point.y());
        _lineSeries->append(_vecResultPoint[0].x,_vecResultPoint[0].y);
        PointInfo info;
        info.x=point.x();
        info.y=point.y();
        _vecResultPoint.push_back(info);
    }
    _isRect=false;
    update();
}
