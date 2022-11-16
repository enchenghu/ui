#include "fft_charts.h"

ChartFFT::ChartFFT(QWidget* parent): QWidget(parent), showType_(FFT_ORI), \
rescale_(true), contineFlag_(true), singleShow_(false)
{
	pCustomPlot = new QCustomPlot(parent);
	QCPGraph* pgraph = pCustomPlot->addGraph();
    this->hide();  
	//添加一条曲线
}

QCustomPlot* ChartFFT::setChart(int xmin, int xmax, int ymin, int ymax){
	//设置数据
	//this->graph(0)->setData(x_FFT,y_FFT);
	//设置Y轴范围
	pCustomPlot->yAxis->setRange(ymin, ymax);
	pCustomPlot->xAxis->setRange(xmin, xmax);
	//x轴名字
	//pCustomPlot->xAxis->setLabel("X");
	//Y轴名字
	pCustomPlot->yAxis->setLabel("Y");
	//设置大小
	//pCustomPlot->resize(ui->label->width(),ui->label->height());
	pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	//pCustomPlot->xAxis->setNumberPrecision(1);
	plotTracer = new myTracer(pCustomPlot, pCustomPlot->graph(0), TracerType::DataTracer, showType_);
	connect(pCustomPlot, &QCustomPlot::mouseMove, this, &ChartFFT::showTracer);
	pCustomPlot->replot();
    return pCustomPlot;
}

void ChartFFT::setShowType(showModel t)
{
    showType_ = t;
}
void ChartFFT::setIfScale(bool t)
{
    rescale_ = t;
}
void ChartFFT::setSingleShow(bool t)
{
    singleShow_ = t;
}
void ChartFFT::setContineFlag(bool t)
{
    contineFlag_ = t;
}


void ChartFFT::setData(const QVector<double> &x, const QVector<double> &y)
{
    if(singleShow_ || contineFlag_ ) pCustomPlot->graph(0)->setData(x, y);

    if(singleShow_) singleShow_  = false;

    if(showType_ == FFT_ORI) 
        pCustomPlot->yAxis->setLabel("amplitude");
    else if(showType_ == FFT_DB)
        pCustomPlot->yAxis->setLabel("amplitude/dB");        

    if(rescale_) {
        pCustomPlot->rescaleAxes(true);
        rescale_ = false;
    }
    pCustomPlot->replot();
}


void ChartFFT::showTracer(QMouseEvent* event)
{
    double x = pCustomPlot->xAxis->pixelToCoord(event->pos().x());
    double y = 0;
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = pCustomPlot->graph(0)->data();
    int low = 0, high = tmpContainer->size();
    while(high > low)
    {
        int middle = (low + high) / 2;
        if(x < tmpContainer->constBegin()->mainKey() ||
                x > (tmpContainer->constEnd()-1)->mainKey())
            break;

        if(x == (tmpContainer->constBegin() + middle)->mainKey())
        {
            y = (tmpContainer->constBegin() + middle)->mainValue();
            break;
        }
        if(x > (tmpContainer->constBegin() + middle)->mainKey())
        {
            low = middle;
        }
        else if(x < (tmpContainer->constBegin() + middle)->mainKey())
        {
            high = middle;
        }
        if(high - low <= 1)
        {   //差值计算所在位置数据
            y = (tmpContainer->constBegin()+low)->mainValue() + ( (x - (tmpContainer->constBegin() + low)->mainKey()) *
                ((tmpContainer->constBegin()+high)->mainValue() - (tmpContainer->constBegin()+low)->mainValue()) ) /
                ((tmpContainer->constBegin()+high)->mainKey() - (tmpContainer->constBegin()+low)->mainKey());
            break;
        }
    }
    //qDebug()<<"y="<<y;
    //显示x轴的鼠标动态坐标
    //m_TraserX->updatePosition(x, 0);
    //m_TraserX->setText(QString::number(x, 'f', 0));
    //显示y轴的鼠标动态坐标，缺点无法定位xy所以无法附加单位，附加单位仍需继续修改setText传参
    //m_TracerY->updatePosition(x, y);
    //m_TracerY->setText(QString::number(y, 'f', 2));
    //由原来的x，y分别显示改为x，y显示在一起，xy单位直接在setText中设置好
    plotTracer->updatePosition(x, y);
    double real_X;
    if(showType_ == ADC_ORI)
        real_X = x;
    else
        real_X = x * 0.4;
    plotTracer->setText(QString::number(real_X, 'f', 2), QString::number(y, 'f', 2));//x轴取整数，y轴保留两位小数
    pCustomPlot->replot();
}



