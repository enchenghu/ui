#ifndef DEIR_COMMON_H
#define DEIR_COMMON_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QRectF>

struct Coordinates {
    QVector<qreal> xCoordinates;  // 存储所有矩形的x1和x2坐标
    QVector<qreal> yCoordinates;  // 存储所有矩形的y1和y2坐标
};


#endif