#include "imagewidget.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>

ImageWidget::ImageWidget(QPixmap *pixmap,std::vector<QRectF>& vecRect,bool green)
{
    _pix = *pixmap;
    _vecRect=&vecRect;
    if(green)
    {
        if(_vecRect->size()>2)
        {
            _vecRect->resize(2);
        }
    }
    setAcceptDrops(true);//If enabled is true, this item will accept hover events; otherwise, it will ignore them. By default, items do not accept hover events.
    _scaleValue = 0;
    _scaleDafault = 0;
    _isMove = false;
    _isRect=false;
    _isClear=false;
    _isGreen=green;
}

ImageWidget::~ImageWidget()
{
    _vecRect=NULL; //单纯指空，不释放，上层还用得到
}

QRectF ImageWidget::boundingRect() const
{
    return QRectF(-_pix.width() / 2, -_pix.height() / 2,
                  _pix.width(), _pix.height());
}

void ImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *)
{
    painter->drawPixmap(-_pix.width() / 2, -_pix.height() / 2, _pix);

    //绘制ROI
    painter->save();
    painter->setPen(QPen(Qt::red,2));
    if(_isGreen)
    {
        if(_vecRect->size() > 0)
        {
            painter->setFont(QFont("Arial", _vecRect->at(0).width()/5));
            painter->drawText(_vecRect->at(0),Qt::AlignCenter,"Green");
            painter->drawRect(_vecRect->at(0));
        }
        if(_vecRect->size() > 1)
        {
            painter->setFont(QFont("Arial", _vecRect->at(1).width()/5));
            painter->drawText(_vecRect->at(1),Qt::AlignCenter,"Gray");
            painter->drawRect(_vecRect->at(1));
        }
    }
    else
    {
        for (int var = 0; var < _vecRect->size(); ++var) {
            painter->setFont(QFont("Arial", _vecRect->at(var).width()/5));
            painter->drawText(_vecRect->at(var),Qt::AlignCenter,QString::number(var));
            painter->drawRect(_vecRect->at(var));
        }
    }
    if(_isRect)
    {
        painter->setFont(QFont("Arial", QRectF(_startRect,_endRect).width()/5));
        if(_isGreen)
        {
            if(_vecRect->size() == 0)
            {
                painter->drawText(QRectF(_startRect,_endRect),Qt::AlignCenter,"Green");
            }
            if(_vecRect->size() == 1)
            {
                painter->drawText(QRectF(_startRect,_endRect),Qt::AlignCenter,"Gray");
            }
        }
        else
        {
            painter->drawText(QRectF(_startRect,_endRect),Qt::AlignCenter,QString::number(_vecRect->size()));
        }
        painter->drawRect(QRectF(_startRect,_endRect));
    }
    /*if(_isClear)
    {
        painter->setPen(QPen(Qt::blue,2));
        painter->drawRect(QRectF(_startClear,_endClear));
    }*/
    painter->restore();
}

void ImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
    {
        //画ROI
        _isRect=true;
        _startRect=event->pos();
        if(_isGreen)
        {
            if(_vecRect->size()>=2)
            {
                _vecRect->clear();
            }
        }
    }
    else if(event->button() == Qt::MiddleButton)
    {
        //鼠标按下滚轮时，清除选中ROI
        _isClear=true;
        _startClear=event->pos();
    }

}

void ImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(_isMove)
    {
        QPointF point = (event->pos() - _startPos)*_scaleValue;
        moveBy(point.x(), point.y());
    }
    if(_isRect)//画ROI
    {
        _endRect = event->pos();
        //限制范围
        _endRect.setX(_endRect.x()<(-_pix.width()/2)?(-_pix.width()/2):_endRect.x());
        _endRect.setX(_endRect.x()>(_pix.width()/2)?(_pix.width()/2):_endRect.x());
        _endRect.setY(_endRect.y()<(-_pix.height()/2)?(-_pix.height()/2):_endRect.y());
        _endRect.setY(_endRect.y()>(_pix.height()/2)?(_pix.height()/2):_endRect.y());
    }
    if(_isClear)//清除选中ROI
    {
        _endClear=event->pos();
    }
    //更新坐标显示
    int x=event->pos().x()+_pix.width()/2;
    x=x<0?0:x;
    x=x>_pix.width()?_pix.width():x;
    int y=event->pos().y()+_pix.height()/2;
    y=y<0?0:y;
    y=y>_pix.height()?_pix.height():y;
    _xValue->setText("x:"+QString::number(x));
    _yValue->setText("y:"+QString::number(y));

    update();
}

void ImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    if(_isRect)//画ROI
    {
        QRectF rect(_startRect,_endRect);
        //限制ROI最小范围
        if(rect.width()>=5&&rect.height()>=5)
        {
            _vecRect->push_back(QRectF(_startRect,_endRect));
        }
    }
    if(_isClear)//清除选中ROI
    {
        clearRect(_startClear, _endClear, _vecRect);
    }
    _isMove = false;//标记鼠标滚轮已经抬起
    _isRect=false;
    _isClear=false;

    update();
}


void ImageWidget::wheelEvent(QGraphicsSceneWheelEvent *event)//鼠标滚轮事件
{
    if((event->delta() > 0)&&(_scaleValue >= 50))//最大放大到原始图像的50倍
    {
        return;
    }
    else if((event->delta() < 0)&&(_scaleValue <= _scaleDafault))//图像缩小到自适应大小之后就不继续缩小
    {
        ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
    }
    else
    {
        qreal qrealOriginScale = _scaleValue;
        if(event->delta() > 0)//鼠标滚轮向前滚动
        {
            _scaleValue*=1.1;//每次放大10%
        }
        else
        {
            _scaleValue*=0.9;//每次缩小10%
        }
        setScale(_scaleValue);
        if(event->delta() > 0)
        {
            moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
        else
        {
            moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
    }
}

void ImageWidget::setQGraphicsViewWH(int nwidth, int nheight)//将主界面的控件QGraphicsView的width和height传进本类中，并根据图像的长宽和控件的长宽的比例来使图片缩放到适合控件的大小
{
    int nImgWidth = _pix.width();
    int nImgHeight = _pix.height();
    qreal temp1 = nwidth*1.0/nImgWidth;
    qreal temp2 = nheight*1.0/nImgHeight;
    if(temp1>temp2)
    {
        _scaleDafault = temp2;
    }
    else
    {
        _scaleDafault = temp1;
    }
    setScale(_scaleDafault);
    _scaleValue = _scaleDafault;
}

void ImageWidget::setXYValue(QLabel* leftValue,QLabel* rightValue)
{
    _xValue=leftValue;
    _yValue=rightValue;
}

void ImageWidget::clearRect(QPointF start, QPointF end, std::vector<QRectF> *vecRect)
{
    if(vecRect->size()<1)
    {
        return;
    }
    QPointF recStart;
    QPointF recEnd;
    QPointF ptLT;
    QPointF ptRB;
    for(std::vector<QRectF>::iterator i= vecRect->begin(); i!= vecRect->end(); )
    {
        //画的方式不一样导致其位置不一定
        recStart=i->topLeft();    //不一定在左上
        recEnd=i->bottomRight();  //不一定在右下
        //因为Y轴是向下生长的，所以Y轴逻辑要倒过来
        ptLT.setX(recStart.x()<recEnd.x()?recStart.x():recEnd.x());
        ptLT.setY(recStart.y()<recEnd.y()?recStart.y():recEnd.y());
        ptRB.setX(recStart.x()>recEnd.x()?recStart.x():recEnd.x());
        ptRB.setY(recStart.y()>recEnd.y()?recStart.y():recEnd.y());
        if(ptLT.x()<start.x()&&ptLT.y()<start.y()&&
            ptRB.x()>start.x()&&ptRB.y()>start.y())
        {
                i= vecRect->erase(i);
                continue;
        }
        i++;
    }
    return;
}

void ImageWidget::ResetItemPos()//重置图片位置
{
    _scaleValue = _scaleDafault;//缩放比例回到一开始的自适应比例
    setScale(_scaleDafault);//缩放到一开始的自适应大小
    setPos(0,0);
}

qreal ImageWidget::getScaleValue() const
{
    return _scaleValue;
}
