#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QPointF>
#include <QDragEnterEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>
#include <QLabel>
#include <QStyleOptionGraphicsItem>
#include <QFont>

enum Enu_zoomState{
    NO_STATE,
    RESET,
    ZOOM_IN,
    ZOOM_OUT
};
// class ImageWidget :public QObject, QGraphicsItem
class ImageWidget :public QGraphicsItem
{
    //Q_OBJECT
public:
    ImageWidget(QPixmap *pixmap,std::vector<QRectF>& vecRect,bool green=false);
    ~ImageWidget();
    QRectF  boundingRect() const;
    void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void    wheelEvent(QGraphicsSceneWheelEvent *event);
    void    ResetItemPos();
    void    mousePressEvent(QGraphicsSceneMouseEvent *event);
    void    mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void    mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    qreal   getScaleValue() const;
    void    setQGraphicsViewWH(int nwidth,int nheight);
    void    setXYValue(QLabel* leftValue,QLabel* rightValue);
private:
    //清除选中ROI
    void    clearRect(QPointF start,QPointF end,std::vector<QRectF>* vecRect);
private:
    qreal       _scaleValue;
    qreal       _scaleDafault;
    QPixmap     _pix;
    int         _zoomState;
    bool        _isMove;
    QPointF     _startPos;
    QPointF     _startRect;
    QPointF     _endRect;
    bool        _isRect;
    QLabel*     _xValue;
    QLabel*     _yValue;
    std::vector<QRectF>* _vecRect;
    bool        _isClear;
    QPointF     _startClear;
    QPointF     _endClear;
    bool        _isGreen;
};

#endif // IMAGEWIDGET_H
