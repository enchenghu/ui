#include "fft_charts.h"

Chart::Chart(QWidget* parent, QString _chartname){
    setParent(parent);
    chartname = _chartname;
    series = new QSplineSeries(this);
    qchart = new QChart;
    chartview = new ChartView(qchart);
    layout = new QHBoxLayout(this);
    axisX = new QValueAxis(this);
    axisY = new QValueAxis(this);
    //在ui里面添加了一个Widget并把曲线图添加进去
    layout->addWidget(chartview);
    setLayout(layout);
    chartview->setRenderHint(QPainter::Antialiasing);//防止图形走样
}

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
}

void Chart::buildChart()
{
    //创建数据源
    series->setPen(QPen(Qt::blue,3,Qt::SolidLine));
    series->clear();
    series->setUseOpenGL(true);//openGl 加速
    //qchart->setTitle(chartname);
    //qchart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
    qchart->legend()->hide(); //隐藏图例
    //qchart->addSeries(series);//输入数据
    qchart->setAxisX(axisX, series);
    qchart->setAxisY(axisY, series);
    updateChart();
    qchart->addSeries(series);//输入数据
    timer_  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateChart(void)));
    timer_->start(1000);
}

void Chart::updateChart()
{
    //sleep(1);
    //qchart->removeSeries(series);//clear all data from last frame
    //delete series;
    //series = new QSplineSeries(this);
    QList<QPointF> points;
	for (int i = 0; i < 100; i++)
	{ 
		points.append(QPointF(i, qrand() % 100));
	}
	series->replace(points);
	//qchart->addSeries(series);//添加新的折线图

}

