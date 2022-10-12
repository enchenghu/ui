#pragma once

#include <QChart>
#include <QChartView>
#include <QMouseEvent>
#include <QtCharts/QValueAxis>
#include <QGraphicsSimpleTextItem>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
	Q_OBJECT

public:
    ChartView(QChart *chart, QWidget *parent = nullptr);
	~ChartView(){};
	// 保存坐标区域，用于复位
	void saveAxisRange();

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	//void keyPressEvent(QKeyEvent *event);
	//void keyReleaseEvent(QKeyEvent *event); 这里两句是原博主保留的 我这里注释掉了

//以下3个为Qt Android下Qchart的缩放（单指触点时默认为鼠标点击，所以移动功能可正常使用）
   // bool event(QEvent *event) override;             //使用手势实现缩放
   // bool gestureEvent(QGestureEvent *event);
   // void pinchTriggered(QPinchGesture *gesture);     这三个为Android的操作，我也注释掉了

private:
	QPoint m_lastPoint;
	bool m_isPress = false;  //这里我把平移的判定初始化成了false，解决鼠标一移进QchartView就跟随移动的情况
	bool m_ctrlPress;
	bool m_alreadySaveRange;
	double m_xMin, m_xMax, m_yMin, m_yMax;
	QGraphicsSimpleTextItem* m_coordItem;
};


