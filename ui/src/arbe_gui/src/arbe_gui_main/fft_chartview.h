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
	void saveAxisRange();

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;


private:
	QPoint m_lastPoint;
	bool m_isPress = false;
	bool m_ctrlPress;
	bool m_alreadySaveRange;
	double m_xMin, m_xMax, m_yMin, m_yMax;
	QGraphicsSimpleTextItem* m_coordItem;
};


