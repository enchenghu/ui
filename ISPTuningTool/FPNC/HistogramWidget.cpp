#include "HistogramWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <algorithm>
#include <cmath>

HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent)
{
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
    
    //setMinimumSize(800, 600);
}

void HistogramWidget::showAccurateHistogram(const QVector<int>& histogram, 
                                          double minEdge, double maxEdge, 
                                          const QString& title)
{
    // 清除之前的图表
    chart->removeAllSeries();
    chart->removeAxis(chart->axisX());
    chart->removeAxis(chart->axisY());
    
    // 创建折线图系列来准确表示直方图
    QLineSeries *series = new QLineSeries();
    //series->setName("Frequency");
    
    // 添加数据点 - 每个bin的中心点
    for (int i = 0; i < histogram.size(); ++i) {
        double binCenter = minEdge + i + 0.5; // bin的中心位置
        series->append(binCenter, histogram[i]);
    }
    
    // 创建X轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Pixel value");
    axisX->setRange(minEdge, maxEdge);
    axisX->setLabelFormat("%.0f");
    
    // 创建Y轴
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Number of pixels");
    axisY->setLabelFormat("%d");
    
    // 添加到图表
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    
    // 自动调整Y轴范围
    int maxCount = *std::max_element(histogram.constBegin(), histogram.constEnd());
    axisY->setRange(0, maxCount * 1.1);
    
    // 设置图表样式
    chart->setTheme(QChart::ChartThemeLight);
    QPen pen(Qt::blue);
    pen.setWidth(2);
    series->setPen(pen);
}

void HistogramWidget::showAccurateBarHistogram(const QVector<int>& histogram, 
                                         double minEdge, double maxEdge,
                                         const QString& title)
{
    // 清除之前的图表
    chart->removeAllSeries();
    chart->removeAxis(chart->axisX());
    chart->removeAxis(chart->axisY());
#if 1    
    // 创建折线图系列来模拟柱状图
    QLineSeries *series = new QLineSeries();
    series->setName(""); // 空名称，不显示图例
    
    // 为每个bin创建矩形柱子
    for (int i = 0; i < histogram.size(); ++i) {
        double binLeft = minEdge + i;
        double binRight = minEdge + i + 1;
        double binHeight = histogram[i];
        
        // 只在有数据的时候绘制柱子
        if (binHeight > 0) {
            // 绘制矩形柱子的五个点（形成闭合矩形）
            series->append(binLeft, 0);           // 左下角
            series->append(binLeft, binHeight);    // 左上角
            series->append(binRight, binHeight);   // 右上角
            series->append(binRight, 0);           // 右下角
            series->append(binLeft, 0);            // 回到左下角（闭合） 
        }
    }
    
    // 创建X轴（数值轴）
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Pixel value");
    axisX->setRange(minEdge - 0.5, maxEdge + 0.5);
    axisX->setLabelFormat("%.0f");     // 整数格式
    axisX->setTickCount(11);           // 显示约20个刻度

    axisX->setTickInterval(100);  // 设置刻度间隔
    axisX->setMinorTickCount(0); 
    
    // 创建Y轴
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Number of pixels");
    axisY->setLabelFormat("%d");
    
    // 将系列添加到图表
    chart->addSeries(series);
    chart->setTitle(title);
    
    // 隐藏图例
    chart->legend()->setVisible(false);
    
    // 添加坐标轴
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    
    // 将系列附加到坐标轴
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    
    // 自动调整Y轴范围
    int maxCount = 0;
    for (int count : histogram) {
        if (count > maxCount) {
            maxCount = count;
        }
    }
    axisY->setRange(0, maxCount * 1.1); // 留10%的边距
    
    // 设置系列样式
    QPen pen(Qt::blue);
    pen.setWidth(1);
    series->setPen(pen);
    series->setBrush(QBrush(QColor(65, 105, 225, 180))); // 半透明蓝色填充
    
    // 优化图表外观
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundVisible(false);
    chartView->setRenderHint(QPainter::Antialiasing);
#endif
}