#include "fft_charts.h"

ChartFFT::ChartFFT(QWidget* parent): QWidget(parent)
{
	pCustomPlot = new QCustomPlot(parent);
	QCPGraph* pgraph = pCustomPlot->addGraph();
	//添加一条曲线
}

void ChartFFT::setChart(int xmin, int xmax, int ymin, int ymax, QGridLayout* chartLayout){
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
	//可以进行鼠标位置 放大缩小 拖拽  放大缩小坐标系！！！功能非常强大
	pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	//pCustomPlot->xAxis->setNumberPrecision(1);
	plotTracer = new myTracer(pCustomPlot, pCustomPlot->graph(0), TracerType::DataTracer);
	//重绘 每次改变完以后都要调用这个进行重新绘制
	connect(pCustomPlot, &QCustomPlot::mouseMove, this, &ChartFFT::showTracer);
	pCustomPlot->replot();  
    if(chartLayout) chartLayout->addWidget(pCustomPlot, 0, 0); 
}

void ChartFFT::setData(const QVector<double> &x, const QVector<double> &y, bool rescalse_, int type_)
{
    pCustomPlot->graph(0)->setData(x, y);
    if(!type_) 
        pCustomPlot->yAxis->setLabel("amplitude");
    else
        pCustomPlot->yAxis->setLabel("amplitude/dB");

    if(rescalse_) pCustomPlot->rescaleAxes(true);
    pCustomPlot->replot();
}


void ChartFFT::showTracer(QMouseEvent* event)
{
    double x = pCustomPlot->xAxis->pixelToCoord(event->pos().x());
 
    //for(int i=0;i<1;i++)//ui->widget9->graph(0)->dataCount()
    //{
       double y = 0;
       QSharedPointer<QCPGraphDataContainer> tmpContainer;
       tmpContainer = pCustomPlot->graph(0)->data();
       //使用二分法快速查找所在点数据！！！敲黑板，下边这段是重点
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
	   double real_X = x * 0.4;
       plotTracer->setText(QString::number(real_X, 'f', 2), QString::number(y, 'f', 2));//x轴取整数，y轴保留两位小数
 
    //}
    pCustomPlot->replot();
}

#if 0
void Chart::setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
             QString _yname, qreal _ymin, qreal _ymax, int _ytickc){
    xname = _xname; xmin = _xmin; xmax = _xmax; xtickc = _xtickc;
    yname = _yname; ymin = _ymin; ymax = _ymax; ytickc = _ytickc;

    axisX->setRange(xmin, xmax);    //设置范围
    axisX->setLabelFormat("%u");   //设置刻度的格式
    axisX->setGridLineVisible(true);   //网格线可见
    axisX->setTickCount(xtickc);       //设置多少个大格
    axisX->setMinorTickCount(1);   //设置每个大格里面小刻度线的数目
    //axisX->setTitleText(xname);  //设置描述

    axisY->setRange(ymin, ymax);
    axisY->setLabelFormat("%u");
    axisY->setGridLineVisible(true);
    axisY->setTickCount(ytickc);
    axisY->setMinorTickCount(1);
    axisY->setTitleText(yname);

    qchart->addAxis(axisX, Qt::AlignBottom); //下：Qt::AlignBottom  上：Qt::AlignTop
    qchart->addAxis(axisY, Qt::AlignLeft);   //左：Qt::AlignLeft    右：Qt::AlignRight
    qchart->setAxisX(axisX, series);
    qchart->setAxisX(axisY, series);
}

void Chart::buildChart()
{
    //创建数据源
    series->setPen(QPen(Qt::blue,3,Qt::SolidLine));
    series->clear();
    //qchart->setTitle(chartname);
    //qchart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
    qchart->legend()->hide(); //隐藏图例
    //qchart->addSeries(series);//输入数据
    std::chrono::duration<double> elapsed;
    auto start = std::chrono::steady_clock::now();

    updateChart();
    qchart->addSeries(series);//输入数据
    qchart->setAxisX(axisX, series);
    qchart->setAxisY(axisY, series);
    series->setUseOpenGL(true);//openGl 加速
    auto end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "time for updateChart total: " <<  elapsed.count() * 1000 << " ms" << std::endl;   
#if 0
    timer_  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateChart(void)));
    timer_->start(1000);
#endif
}

void Chart::updateChart()
{
    //sleep(1);
    //qchart->removeSeries(series);//clear all data from last frame
    //delete series;
    //series = new QSplineSeries(this);
    QList<QPointF> points;
	for (int i = 0; i < 8191; i++)
	{ 
		points.append(QPointF(i, qrand() % 100));
	}
	series->replace(points);
	//qchart->addSeries(series);//添加新的折线图

}


#endif

