#include "fft_charts.h"

extern QStringList motorDataString;
ChartFFT::ChartFFT(QWidget* parent, showModel type): QWidget(parent), showType_(type), \
rescale_(true), contineFlag_(true), singleShow_(false)
{
	pCustomPlot = new QCustomPlot(parent);
    uint8_t graph_num = 1;
    if(type == MOTOR_ORI)
    {
        graph_num = MOTOR_ITEMS_NUM;
    }
    for(int i = 0; i < graph_num; i++){
        pCustomPlot->addGraph();
    }
    this->hide();  
	//添加一条曲线
}

void ChartFFT::setXChart(int xmin, int xmax)
{
    pCustomPlot->xAxis->setRange(xmin, xmax);
}


void ChartFFT::setGraph(uint8_t index_graph)
{
    QPen pen;
    pen.setWidth(2);//曲线的粗细
    pen.setColor(colotList[index_graph]);
    pCustomPlot->graph(index_graph)->setPen(pen);//曲线颜色设置
    pCustomPlot->graph(index_graph)->setAntialiasedFill(true);//设置抗锯齿
    pCustomPlot->graph(index_graph)->setName(motorDataString[index_graph]);//设置画布曲线名称
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


void ChartFFT::setData(const QVector<double> &x, const QVector<double> &y, uint8_t index_graph)
{
    if(singleShow_ || contineFlag_ ) {
        if(showType_ == FFT_ORI || showType_ == FFT_DB){
            pCustomPlot->graph(index_graph)->setData(x, y);
        }else if(showType_ == ADC_ORI) {
            pCustomPlot->graph(index_graph)->setData(x, y);
            pCustomPlot->xAxis->setRange(x[0], x[x.size() - 1]);
        }else if(showType_ == MOTOR_ORI){
            pCustomPlot->graph(index_graph)->addData(x, y);
            setGraph(index_graph);
            pCustomPlot->legend->setVisible(true);
            pCustomPlot->legend->setBrush(QColor(255, 255, 255, 150));
            //pCustomPlot->xAxis->setRange(x[0], x[x.size() - 1]);
        }   
    }

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



