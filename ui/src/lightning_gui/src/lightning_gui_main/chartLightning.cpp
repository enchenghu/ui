#include "chartLightning.h"
#include <fstream>

extern QStringList motorDataString;
ChartLighting::ChartLighting(QWidget* parent, showModel type): QWidget(parent), showType_(type), \
rescale_(true), contineFlag_(true), singleShow_(false)
{
	pCustomPlot = new QCustomPlot(parent);
    //pCustomPlot->setOpenGl(true); 
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

void ChartLighting::setXChart(int xmin, int xmax)
{
    pCustomPlot->xAxis->setRange(xmin, xmax);
}


void ChartLighting::setGraphShow(uint8_t index_graph, bool show)
{
    pCustomPlot->graph(index_graph)->setVisible(show);//曲线颜色设置
}

void ChartLighting::setGraph(uint8_t index_graph)
{
    QPen pen;
    pen.setWidth(2);//曲线的粗细
    pen.setColor(colotList[index_graph]);
    pCustomPlot->graph(index_graph)->setPen(pen);//曲线颜色设置
    pCustomPlot->graph(index_graph)->setAntialiasedFill(true);//设置抗锯齿
    pCustomPlot->graph(index_graph)->setName(motorDataString[index_graph]);//设置画布曲线名称
}

QCustomPlot* ChartLighting::setChart(int xmin, int xmax, int ymin, int ymax){
	//设置数据
	//this->graph(0)->setData(x_FFT,y_FFT);
	//设置Y轴范围
	pCustomPlot->yAxis->setRange(ymin, ymax);
	pCustomPlot->xAxis->setRange(xmin, xmax);
	//x轴名字
	if(showType_ == FFT_ORI || showType_ == FFT_DB)pCustomPlot->xAxis->setLabel("Frequency / MHz");
	//Y轴名字
	pCustomPlot->yAxis->setLabel("amplitude");
	//设置大小
	//pCustomPlot->resize(ui->label->width(),ui->label->height());
	pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	//pCustomPlot->xAxis->setNumberPrecision(1);
	plotTracer = new myTracer(pCustomPlot, pCustomPlot->graph(0), TracerType::DataTracer, showType_);
	connect(pCustomPlot, &QCustomPlot::mouseMove, this, &ChartLighting::showTracer);
	pCustomPlot->replot();
    return pCustomPlot;
}

void ChartLighting::setShowType(showModel t)
{
    showType_ = t;
}
void ChartLighting::setIfScale(bool t)
{
    rescale_ = t;
}
void ChartLighting::setSingleShow(bool t)
{
    singleShow_ = t;
}
void ChartLighting::setContineFlag(bool t)
{
    contineFlag_ = t;
}


void ChartLighting::setData(const QVector<double> &x, const QVector<double> &y, uint8_t index_graph)
{
    if(singleShow_ || contineFlag_) {
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
            if(x[x.size() - 1] > 100) pCustomPlot->xAxis->setRange(x[x.size() - 1] - 200, x[x.size() - 1]);
        }   
    }

    if(singleShow_) {
        singleShow_  = false;
        if(showType_ == ADC_ORI){
            static int f_index = 0;
            time_t rawtime;
            struct tm *ptminfo;
            time(&rawtime);
            ptminfo = localtime(&rawtime);
            printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
            std::string csvPath;
            csvPath =  "./adc_data_" + 
            std::to_string(ptminfo->tm_year + 1900) + 
            "-" + std::to_string(ptminfo->tm_mon + 1) +
            "-" + std::to_string(ptminfo->tm_mday) +
            "-" + std::to_string(ptminfo->tm_hour) +
            "-" + std::to_string(ptminfo->tm_min) +
            "-" + std::to_string(ptminfo->tm_sec) + "-" +std::to_string(f_index++)
            +".csv";
            printf("csvPath is %s \n", csvPath.c_str());
            std::ofstream csvfile; 
            csvfile.open(csvPath, std::ios::out); 
            for(int i  = 0; i < y.size(); i++){
                csvfile << y[i] << "\n";	               
            }
        }
    }

    if(showType_ == FFT_ORI) 
        pCustomPlot->yAxis->setLabel("amplitude");
    else if(showType_ == FFT_DB)
        pCustomPlot->yAxis->setLabel("amplitude/dB");        

    if(rescale_) {
        pCustomPlot->rescaleAxes(true);
        rescale_ = false;
    }
    pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void ChartLighting::showTracer(QMouseEvent* event)
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
/*     double real_X = x;;
    if(showType_ == ADC_ORI)
        real_X = x;
    else
        real_X = x * 0.6; */
    plotTracer->setText(QString::number(x, 'f', 2), QString::number(y, 'f', 2));//x轴取整数，y轴保留两位小数
    pCustomPlot->replot();
}



