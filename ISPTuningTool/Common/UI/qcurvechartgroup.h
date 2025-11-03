#ifndef QCURVECHARTGROUP_H
#define QCURVECHARTGROUP_H

#include "Common/Others/qcurvefitting.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QPointer>

namespace Ui {
class QCurveChartGroup;
}

class QCurveChartGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QCurveChartGroup(QWidget *parent = nullptr);
    ~QCurveChartGroup();
    void init();
    void initConnect();

    // 设置全部点信息
    void setPoints(std::vector<QPoint> point);
    // 获取全部点信息
    std::vector<QPoint> getPoints();
    // 添加点信息
    void addPoint(QPoint point);
    // 删除指定x坐标的点信息
    void removePointByX(int x);
    // 删除指定y坐标的点信息
    void removePointByY(int y);
    // 获取point的数量
    int getPointNum();

    // 设置曲线上用于拟合的点的x坐标
    void setCurveFittingPointX(std::vector<int> x);
    // 设置曲线上用于拟合的点的索引（x从小到大排序）
    void setCurveFittingPointIndex(std::vector<int> index);
    // 设置曲线拟合的类型（贝塞尔曲线拟合，三次样条插值拟合）
    void setCurveFittingType(CurveFittingType type);

    // 向x较小方向切换高亮点
    int switchHighlightPointToLeft();
    // 向x较大方向切换高亮点
    int switchHighlightPointToRight();
    // 获取高亮点
    QPoint getHighlightPoint();
    // 设置高亮点
    void setHighlightPoint(QPoint point);
    // 获取高亮点的y坐标
    int getHighlightPointY();
    // 获取高亮点的索引
    int getHighlightPointIndex();
    // 设置高亮点的y坐标
    void setHighlightPointY(int y);
    // 高亮点y坐标增加
    int highlightPointYIncrease(uint32_t delta = 1);
    // 高亮点y坐标减少
    int highlightPointYDecrease(uint32_t delta = 1);

    // 设置x坐标的最大值
    void setXMax(int max);
    // 设置x坐标的最小值
    void setXMin(int min);
    // 设置y坐标的最大值
    void setYMax(int max);
    // 设置y坐标的最小值
    void setYMin(int min);

    // 设置x轴y轴的显示范围
    void setAxisRange(int minX, int maxX, int minY, int maxY);
    // 设置自动调整x轴y轴范围
    void setAutoAdjustAxisRange(bool autoAdjust);

    // 设置点是否可以手动调整
    void setPointChangeEnable(bool enable);
    // 设置Y轴显示信息
    void setYStringInfo(QStringList list, Qt::AlignmentFlag type = Qt::AlignCenter);
    // 设置X轴显示信息
    void setXStringInfo(QStringList list, Qt::AlignmentFlag type = Qt::AlignCenter);

    // 设置x轴数字格式
    void setXAxisLabelFormat(QString format);

    // 设置曲线单点调节
    bool setCurvePointType(int type);
signals:
    void signalPointChanged(QPoint point);
    void signalAllPointsChanged(std::vector<QPoint> points);
    void signalDoubleClickedString(QString string);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QSharedPointer<Ui::QCurveChartGroup> ui;
    QPointer<QChartView>                 chartLineView        = nullptr;
    QPointer<QLineSeries>                fitLineSeries        = nullptr;
    QPointer<QLineSeries>                secondLineSeries     = nullptr;
    QPointer<QScatterSeries>             pointSeries          = nullptr;
    QPointer<QScatterSeries>             highlightPointSeries = nullptr;
    QPointer<QVBoxLayout>                layoutYInfo          = nullptr;
    QPointer<QHBoxLayout>                layoutXInfo          = nullptr;
    QStringList                          listYInfo;
    QStringList                          listXInfo;

    std::vector<QPoint>              points;
    std::vector<QPoint>              controlPoints;
    int                              highlightPointIndex = 0;
    CurveFittingType                 curveFittingType    = CurveFittingType::NONE;
    bool                             autoAdjustAxisRange = false;
    bool                             pointChangeEnable   = true;
    int                              xMin = 0, xMax = 255, yMin = 0, yMax = 4096;
    int                              xShowMin = 0, xShowMax = 255, yShowMin = 0, yShowMax = 4096;
    int                              pressedPointIndex = -1;
    std::map<double, double>         pointsByCurve;
    std::vector<std::pair<int, int>> pointsToCalc;
    int                              curveChangeType   = 6;
    bool                             curveRefreshFit   = false;
    bool                             curveNoWrite   = false;
    bool                             curveChangeSingle   = false;

    void fitCurveByType();
    void updateScatterSeries();
    void updatePointsByCurve();
    void updatePointsByCurveNew();

    double calculateDistance(double ax, double ay, double bx, double by);

signals:
    void signalUpdateChart();
    void signalUpdateChartNew();

private slots:
    void slotUpdateChart();
    void slotUpdateChartNew();


};

#endif // QCURVECHARTGROUP_H
