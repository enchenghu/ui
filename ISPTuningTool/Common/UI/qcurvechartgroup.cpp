#include "qcurvechartgroup.h"
#include "ui_qcurvechartgroup.h"
#include <QGraphicsLayout>
#include <QLabel>
#include <QSplineSeries>
#include <QValueAxis>
#include <cmath>
#include <vector>

QCurveChartGroup::QCurveChartGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QCurveChartGroup)
{
    ui->setupUi(this);
    init();
}

QCurveChartGroup::~QCurveChartGroup() { }

void QCurveChartGroup::init()
{

    QPointer<QChart> lineChart = new QChart();
    lineChart->setTitle("");
    lineChart->setTitleFont(QFont("黑体", 14, QFont::Weight::Bold));

    lineChart->setAnimationOptions(QChart::NoAnimation);
    lineChart->legend()->setVisible(false);
    lineChart->setBackgroundVisible(false);
    lineChart->setTitleBrush(QColor(0x80, 0x83, 0x96));
    lineChart->layout()->setContentsMargins(0, 0, 0, 0);
    lineChart->setMargins(QMargins(0, 0, 0, 0));
    lineChart->setBackgroundRoundness(0);

    QPointer<QValueAxis> axisX = new QValueAxis();
    axisX->setRange(xShowMin, xShowMax);
    axisX->setTickCount(10);
    axisX->setLabelFormat("%d");
    axisX->setGridLineVisible(true);
    axisX->setGridLineColor(QColor(0x80, 0x83, 0x96));
    axisX->setGridLinePen(QPen(QColor(0x80, 0x83, 0x96)));
    axisX->setLabelsColor(QColor(0x80, 0x83, 0x96));
    axisX->setLinePen(QPen(QColor(0x80, 0x83, 0x96)));

    QPointer<QValueAxis> axisY = new QValueAxis();
    axisY->setRange(yShowMin, yShowMax);
    axisY->setTickCount(10);
    axisY->setLabelFormat("%d");
    axisY->setGridLineVisible(true);
    axisY->setGridLineColor(QColor(0x80, 0x83, 0x96));
    axisY->setGridLinePen(QPen(QColor(0x80, 0x83, 0x96)));
    axisY->setLabelsColor(QColor(0x80, 0x83, 0x96));
    axisY->setLinePen(QPen(QColor(0x80, 0x83, 0x96)));
    lineChart->addAxis(axisX, Qt::AlignBottom);
    lineChart->addAxis(axisY, Qt::AlignLeft);

    chartLineView = new QChartView(this);
    chartLineView->setRenderHint(QPainter::Antialiasing);
    chartLineView->setRenderHint(QPainter::SmoothPixmapTransform);
    chartLineView->setVisible(true);
    chartLineView->setAttribute(Qt::WA_TranslucentBackground);
    chartLineView->setStyleSheet("background:transparent");
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    layoutYInfo = new QVBoxLayout(this);
    layoutXInfo = new QHBoxLayout(this);
    ui->gridLayout->addLayout(layoutYInfo, 0, 0);
    ui->gridLayout->addLayout(layoutXInfo, 1, 1);
    ui->gridLayout->addWidget(chartLineView, 0, 1);
    chartLineView->setChart(lineChart);

    pointSeries = new QScatterSeries(chartLineView->chart());
    pointSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    pointSeries->setMarkerSize(10);
    pointSeries->setColor(QColor(0x00, 0x00, 0xFF));
    pointSeries->setBorderColor(QColor(0xFF, 0xFF, 0xFF));
    lineChart->addSeries(pointSeries);
    highlightPointSeries = new QScatterSeries(chartLineView->chart());
    highlightPointSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    highlightPointSeries->setMarkerSize(10);
    highlightPointSeries->setColor(QColor(0xFF, 0x00, 0x00));
    highlightPointSeries->setBorderColor(QColor(0xFF, 0xFF, 0xFF));
    lineChart->addSeries(highlightPointSeries);

    initConnect();

#if 0
	std::vector<QPoint> points;
	// random 33 points
	srand(time(nullptr));
	std::vector<int> x;
	for (int i = 0; i < 33; ++i)
	{
		auto tmp = rand() % 256;
		while (std::find(x.begin(), x.end(), tmp) != x.end())
		{
			tmp = rand() % 256;
		}
		x.push_back(tmp);
	}
	std::sort(x.begin(), x.end());
	std::vector<int> y;
	for (int i = 0; i < 33; ++i)
	{
		auto tmp = rand() % 4096;
		while (std::find(y.begin(), y.end(), tmp) != y.end())
		{
			tmp = rand() % 4096;
		}
		y.push_back(tmp);
	}
	std::sort(y.begin(), y.end());
	for (int i = 0; i < 33; ++i)
	{
		points.push_back(QPoint(x[i], y[i]));
	}
	setPoints(points);
	setCurveFittingType(CurveFittingType::BEZIER);
#endif
}

void QCurveChartGroup::initConnect()
{
    QCoreApplication::instance()->installEventFilter(this);
    connect(this, &QCurveChartGroup::signalUpdateChart, this, &QCurveChartGroup::slotUpdateChart, Qt::DirectConnection);
    connect(this, &QCurveChartGroup::signalUpdateChartNew, this, &QCurveChartGroup::slotUpdateChartNew, Qt::DirectConnection);

}

void QCurveChartGroup::setPoints(std::vector<QPoint> point)
{
    if(point.size()!=controlPoints.size())
        curveRefreshFit=true;

    controlPoints.clear();
    points = point;
    emit signalUpdateChartNew();
}

std::vector<QPoint> QCurveChartGroup::getPoints()
{
    return points;
}

void QCurveChartGroup::addPoint(QPoint point)
{
    for (auto &p : points) {
        if (p.x() == point.x()) {
            p.setY(point.y());
            return;
        }
    }
    points.push_back(point);
    if (controlPoints.size() == points.size() - 1) {
        controlPoints.push_back(point);
    }
    emit signalUpdateChart();
}

void QCurveChartGroup::removePointByX(int x)
{
    for (auto it = points.begin(); it != points.end(); ++it) {
        if (it->x() == x) {
            points.erase(it);
            return;
        }
    }
    emit signalUpdateChart();
}

void QCurveChartGroup::removePointByY(int y)
{
    for (auto it = points.begin(); it != points.end();) {
        if (it->y() == y) {
            it = points.erase(it);
        } else {
            ++it;
        }
    }
    emit signalUpdateChart();
}

int QCurveChartGroup::getPointNum()
{
    return static_cast<int>(points.size());
}

void QCurveChartGroup::setCurveFittingPointX(std::vector<int> x)
{
    controlPoints.clear();
    for (auto &px : x) {
        for (auto &point : points) {
            if (point.x() == px) {
                controlPoints.push_back(point);
                break;
            }
        }
    }
    highlightPointIndex = 0;
    emit signalUpdateChart();
}

void QCurveChartGroup::setCurveFittingPointIndex(std::vector<int> index)
{
    curveRefreshFit=true;
    curveNoWrite=true;
    controlPoints.clear();
    for (auto &idx : index) {
        controlPoints.push_back(points[idx]);
    }
    highlightPointIndex = 0;
    emit signalUpdateChartNew();
}

void QCurveChartGroup::setCurveFittingType(CurveFittingType type)
{
    curveFittingType = type;
    emit signalUpdateChart();
}

int QCurveChartGroup::switchHighlightPointToLeft()
{
    highlightPointIndex--;
    if (highlightPointIndex < 0) {
        highlightPointIndex = static_cast<int>(controlPoints.size()) - 1;
    }
    updateScatterSeries();
    return highlightPointIndex;
}

int QCurveChartGroup::switchHighlightPointToRight()
{
    highlightPointIndex++;
    if (highlightPointIndex >= controlPoints.size()) {
        highlightPointIndex = 0;
    }
    updateScatterSeries();
    return highlightPointIndex;
}

QPoint QCurveChartGroup::getHighlightPoint()
{
    return controlPoints[highlightPointIndex];
}

void QCurveChartGroup::setHighlightPoint(QPoint point)
{
    for (int i = 0; i < controlPoints.size(); i++) {
        if (controlPoints[i].x() == point.x()) {
            highlightPointIndex = i;
            controlPoints[i].setY(point.y());
            emit signalUpdateChart();
            return;
        }
    }
}

int QCurveChartGroup::getHighlightPointY()
{
    return controlPoints[highlightPointIndex].y();
}

int QCurveChartGroup::getHighlightPointIndex()
{
    return highlightPointIndex;
}

void QCurveChartGroup::setHighlightPointY(int y)
{
    if (y > yMax) {
        y = yMax;
    }
    if (y < yMin) {
        y = yMin;
    }
    controlPoints[highlightPointIndex].setY(y);
    emit signalUpdateChart();
}

int QCurveChartGroup::highlightPointYIncrease(uint32_t delta)
{
    setHighlightPointY(controlPoints[highlightPointIndex].y() + static_cast<int>(delta));
    return controlPoints[highlightPointIndex].y();
}

int QCurveChartGroup::highlightPointYDecrease(uint32_t delta)
{
    setHighlightPointY(controlPoints[highlightPointIndex].y() - static_cast<int>(delta));
    return controlPoints[highlightPointIndex].y();
}

void QCurveChartGroup::setXMax(int max)
{
    xMax = max;
}

void QCurveChartGroup::setXMin(int min)
{
    xMin = min;
}

void QCurveChartGroup::setYMax(int max)
{
    yMax = max;
}

void QCurveChartGroup::setYMin(int min)
{
    yMin = min;
}

void QCurveChartGroup::setAxisRange(int minX, int maxX, int minY, int maxY)
{
    xShowMax = maxX;
    xShowMin = minX;
    yShowMax = maxY;
    yShowMin = minY;
}

void QCurveChartGroup::setAutoAdjustAxisRange(bool autoAdjust)
{
    autoAdjustAxisRange = autoAdjust;
}

void QCurveChartGroup::setPointChangeEnable(bool enable)
{
    pointChangeEnable = enable;

    if (pointChangeEnable)
        highlightPointSeries->setColor(QColor(0xFF, 0x00, 0x00));
    else
        highlightPointSeries->setColor(QColor(0x00, 0x00, 0xFF));
}

void QCurveChartGroup::setYStringInfo(QStringList list, Qt::AlignmentFlag type)
{
    QPalette color;
    color.setColor(QPalette::WindowText, QColor(0x80, 0x83, 0x96));
    for (int i = 0; i < list.size(); i++) {
        QPointer<QLabel> labelInfo = new QLabel(this);
        labelInfo->setText(list[list.size() - i - 1]);
        labelInfo->setPalette(color);
        layoutYInfo->addWidget(labelInfo, 0, type);
        listYInfo.append(labelInfo->text());
    }
}

void QCurveChartGroup::setXStringInfo(QStringList list, Qt::AlignmentFlag type)
{
    QPalette color;
    color.setColor(QPalette::WindowText, QColor(0x80, 0x83, 0x96));
    for (auto info : list) {
        QPointer<QLabel> labelInfo = new QLabel(this);
        labelInfo->setText(info);
        labelInfo->setPalette(color);
        layoutXInfo->addWidget(labelInfo, 0, type);
        listXInfo.append(labelInfo->text());
    }
}

void QCurveChartGroup::setXAxisLabelFormat(QString format) { 
    auto axisX = dynamic_cast<QValueAxis *>(chartLineView->chart()->axes(Qt::Horizontal).at(0));
    if (axisX) {
        axisX->setLabelFormat(format);
    }
}

bool QCurveChartGroup::setCurvePointType(int type)
{
    if(curveChangeType==type)
    {
        return false;
    }
    curveChangeType=type;
    return true;
}

bool QCurveChartGroup::eventFilter(QObject *watched, QEvent *event)
{
    if (!isVisible()) {
        return QWidget::eventFilter(watched, event);
    }
    switch (event->type()) {
    case QEvent::MouseMove: {
        auto mouseEvent = static_cast<QMouseEvent *>(event);
        auto localPos   = chartLineView->mapFromGlobal(mouseEvent->globalPosition());
        auto seriesPos  = chartLineView->chart()->mapToValue(localPos);
        auto y          = seriesPos.y();
        if (pressedPointIndex != -1) {
            // 坐标转换
            if (y > yMax) {
                y = yMax;
            }
            if (y < yMin) {
                y = yMin;
            }
            controlPoints[pressedPointIndex].setY(static_cast<int>(y));
            highlightPointSeries->replace(0, controlPoints[pressedPointIndex].x(),
                controlPoints[pressedPointIndex].y());
            curveRefreshFit=true;
            if(curveChangeType==7)//POINT_33_Single
                curveChangeSingle=true;
            return true;
        }
        break;
    }
    case QEvent::MouseButtonPress: {
        if (pointChangeEnable) {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                auto mouseEvent = static_cast<QMouseEvent *>(event);
                // 找到点击位置最近的点
                int    idx         = -1;
                double minDistance = 5;
                for (size_t i = 0; i < controlPoints.size(); ++i) {
                    auto point     = controlPoints[i];
                    auto chartPos  = chartLineView->chart()->mapToPosition(point);
                    auto globalPos = chartLineView->mapToGlobal(chartPos.toPoint());
                    auto distance  = calculateDistance(mouseEvent->globalPosition().x(),
                         mouseEvent->globalPosition().y(), globalPos.x(), globalPos.y());
                    if (distance < minDistance) {
                        minDistance = distance;
                        idx         = static_cast<int>(i);
                    }
                }
                if (idx != -1) {
                    pressedPointIndex   = idx;
                    highlightPointIndex = idx;
                    updateScatterSeries();
                    return true;
                }
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        auto mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (pressedPointIndex != -1) {
                emit signalUpdateChartNew();
                pressedPointIndex = -1;
                return true;
            }
        }
        break;
    }
    case QEvent::MouseButtonDblClick: {
        auto mouseEvent = static_cast<QMouseEvent *>(event);
        int  x          = mouseEvent->globalPosition().x();
        int  y          = mouseEvent->globalPosition().y();
        // 找到X轴点击位置最近的字符串
        for (size_t i = 0; i < layoutXInfo->count(); ++i) {
            QLayoutItem *it        = layoutXInfo->itemAt(static_cast<int>(i));
            auto         globalPos = it->widget()->mapToGlobal(QPoint(0, 0));
            if (((x - globalPos.x()) <= it->widget()->width()) && ((x - globalPos.x()) >= 0)
                && ((y - globalPos.y()) <= it->widget()->height()) && ((y - globalPos.y()) >= 0)) {
                emit signalDoubleClickedString(listXInfo[i]);
                return true;
            }
        }
        // 找到Y轴点击位置最近的字符串
        for (size_t i = 0; i < layoutYInfo->count(); ++i) {
            QLayoutItem *it        = layoutYInfo->itemAt(static_cast<int>(i));
            auto         globalPos = it->widget()->mapToGlobal(QPoint(0, 0));
            if (((x - globalPos.x()) <= it->widget()->width()) && ((x - globalPos.x()) >= 0)
                && ((y - globalPos.y()) <= it->widget()->height()) && ((y - globalPos.y()) >= 0)) {
                emit signalDoubleClickedString(listYInfo[i]);
                return true;
            }
        }
        break;
    }
    default: {
        break;
    }
    }
    return QWidget::eventFilter(watched, event);
}

void QCurveChartGroup::fitCurveByType()
{
    if (controlPoints.empty()) {
        return;
    }
    if (!fitLineSeries) {
        fitLineSeries = new QLineSeries(chartLineView->chart());
        chartLineView->chart()->addSeries(fitLineSeries);
    }
    fitLineSeries->clear();
    if (!secondLineSeries) {
        secondLineSeries = new QLineSeries(chartLineView->chart());
        chartLineView->chart()->addSeries(secondLineSeries);
    }
    secondLineSeries->clear();

    std::vector<std::pair<double, double>> controlPointsStd;
    for (auto &point : controlPoints) {
        controlPointsStd.emplace_back(point.x(), point.y());
    }
    pointsToCalc.clear();
    for (auto &point : points) {
        pointsToCalc.push_back({ point.x(), point.y() });
    }
    auto curvePoints = QCurveFitting::generateCurve(controlPointsStd, pointsToCalc, 0.5, curveFittingType);
    pointsByCurve.clear();
    for (auto &p : curvePoints) {
        fitLineSeries->append(p.first, p.second);
    }
    if (curveFittingType == CurveFittingType::BEZIER) {
        for (auto &point : controlPoints) {
            secondLineSeries->append(point.x(), point.y());
        }
    }
}

void QCurveChartGroup::updateScatterSeries()
{
    if (controlPoints.empty()) {
        return;
    }
    pointSeries->clear();
    highlightPointSeries->clear();
    if (highlightPointIndex >= controlPoints.size()) {
        highlightPointIndex = 0;
    }
    for (int idx = 0; idx < controlPoints.size(); ++idx) {
        if (idx == highlightPointIndex) {
            continue;
        }
        pointSeries->append(controlPoints[idx].x(), controlPoints[idx].y());
    }
    highlightPointSeries->append(controlPoints[highlightPointIndex].x(), controlPoints[highlightPointIndex].y());
}

void QCurveChartGroup::updatePointsByCurve()
{
    if (points.empty())
        return;

    emit signalPointChanged(controlPoints[highlightPointIndex]);
    if (controlPoints.size() == points.size()) {
        points[highlightPointIndex] = controlPoints[highlightPointIndex];
        emit signalAllPointsChanged(points);
        return;
    }

    for (int idx = 0; idx < points.size(); ++idx) {
        if (points[idx].y() != pointsToCalc[idx].second) {
            points[idx].setY(pointsToCalc[idx].second);
            emit signalPointChanged(points[idx]);
        }
    }
    emit signalAllPointsChanged(points);
}

void QCurveChartGroup::updatePointsByCurveNew()
{
    if (points.empty())
        return;
    emit signalPointChanged(controlPoints[highlightPointIndex]);

    if(curveRefreshFit){
        if(curveChangeSingle)
        {
            points[highlightPointIndex] = controlPoints[highlightPointIndex];
        }
        else
        {
            for (int idx = 0; idx < points.size(); ++idx) {
                if (points[idx].y() != pointsToCalc[idx].second) {
                    points[idx].setY(pointsToCalc[idx].second);
                    emit signalPointChanged(points[idx]);
                }
            }
        }
        curveRefreshFit=false;
        curveChangeSingle=false;
    }else{
        points[highlightPointIndex] = controlPoints[highlightPointIndex];
    }


    if(curveNoWrite)
    {
        curveNoWrite=false;
        return;
    }

    emit signalAllPointsChanged(points);

}

double QCurveChartGroup::calculateDistance(double ax, double ay, double bx, double by)
{
    return sqrt(pow((ax - bx), 2) + pow((ay - by), 2));
}

void QCurveChartGroup::slotUpdateChart()
{
    if (controlPoints.size() == 0 || controlPoints.size() > points.size()) {
        controlPoints.clear();
        for (auto &point : points) {
            controlPoints.push_back(point);
        }
    }
    updateScatterSeries();
    fitCurveByType();
    if (autoAdjustAxisRange && !points.empty()) {
        xShowMin = controlPoints[0].x();
        xShowMax = controlPoints[0].x();
        yShowMin = controlPoints[0].y();
        yShowMax = controlPoints[0].y();
        for (auto &point : controlPoints) {
            if (point.x() < xShowMin) {
                xShowMin = point.x();
            }
            if (point.x() > xShowMax) {
                xShowMax = point.x();
            }
            if (point.y() < yShowMin) {
                yShowMin = point.y();
            }
            if (point.y() > yShowMax) {
                yShowMax = point.y();
            }
        }
    }
    auto axisX = chartLineView->chart()->axes(Qt::Horizontal).at(0);
    auto axisY = chartLineView->chart()->axes(Qt::Vertical).at(0);
    axisX->setRange(xShowMin, xShowMax);
    axisY->setRange(yShowMin, yShowMax);
    if (pointSeries) {
        pointSeries->attachAxis(axisX);
        pointSeries->attachAxis(axisY);
    }
    if (highlightPointSeries) {
        highlightPointSeries->attachAxis(axisX);
        highlightPointSeries->attachAxis(axisY);
    }
    if (fitLineSeries) {
        fitLineSeries->attachAxis(axisX);
        fitLineSeries->attachAxis(axisY);
    }
    if (secondLineSeries) {
        secondLineSeries->attachAxis(axisX);
        secondLineSeries->attachAxis(axisY);
    }
    updatePointsByCurve();
}

void QCurveChartGroup::slotUpdateChartNew()
{
    if (controlPoints.size() == 0 || controlPoints.size() > points.size()) {
        controlPoints.clear();
        for (auto &point : points) {
            controlPoints.push_back(point);
        }
    }
    updateScatterSeries();
    fitCurveByType();
    if (autoAdjustAxisRange && !points.empty()) {
        xShowMin = controlPoints[0].x();
        xShowMax = controlPoints[0].x();
        yShowMin = controlPoints[0].y();
        yShowMax = controlPoints[0].y();
        for (auto &point : controlPoints) {
            if (point.x() < xShowMin) {
                xShowMin = point.x();
            }
            if (point.x() > xShowMax) {
                xShowMax = point.x();
            }
            if (point.y() < yShowMin) {
                yShowMin = point.y();
            }
            if (point.y() > yShowMax) {
                yShowMax = point.y();
            }
        }
    }
    auto axisX = chartLineView->chart()->axes(Qt::Horizontal).at(0);
    auto axisY = chartLineView->chart()->axes(Qt::Vertical).at(0);
    axisX->setRange(xShowMin, xShowMax);
    axisY->setRange(yShowMin, yShowMax);
    if (pointSeries) {
        pointSeries->attachAxis(axisX);
        pointSeries->attachAxis(axisY);
    }
    if (highlightPointSeries) {
        highlightPointSeries->attachAxis(axisX);
        highlightPointSeries->attachAxis(axisY);
    }
    if (fitLineSeries) {
        fitLineSeries->attachAxis(axisX);
        fitLineSeries->attachAxis(axisY);
    }
    if (secondLineSeries) {
        secondLineSeries->attachAxis(axisX);
        secondLineSeries->attachAxis(axisY);
    }
    updatePointsByCurveNew();
}
