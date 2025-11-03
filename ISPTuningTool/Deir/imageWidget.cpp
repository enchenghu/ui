#include "imageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QDebug>
#include <cmath>
#include <iostream>

QString extractAfterLastSlash(const QString& path) {
    int lastSlashIndex = path.lastIndexOf('/');
    if (lastSlashIndex == -1) {
        return path; // 没有斜杠，返回原字符串
    }
    return path.mid(lastSlashIndex + 1);
}

ImageWidget::ImageWidget(QWidget *parent, int width_, int height_, bool show)
    : QWidget(parent)
    , m_dragging(false)
    , m_scaling(false)
    , m_scale(1.0)
    , hide_(false)
    , m_subRectSpacing(30.0)
    , m_scalingCorner(-1)
    , m_subRectWidth(50)
    , m_subRectHeight(50)
    , m_rectanglesVisible(show)
    , current_image_name_("")
{
    setMouseTracking(true);
    //setMinimumSize(800, 600);
    setFixedSize(width_, height_);  // 设置固定尺寸
    // 初始化显示区域（居中显示）
    m_displayRect = QRect(0, 0, width(), height());    
    // 初始化子矩形框布局
    updateSubRectsLayout();
}

// void ImageWidget::loadBmpImage(const QString& filePath)
// {
//     if (m_backgroundImage.load(filePath)) {
//         update();
//     } else {
//         qDebug() << "Failed to load image:" << filePath;
//     }
// }
QVector<QVector<double>> ImageWidget::readBmpImage(const QString& filePath)
{
    QImage image(filePath);
    if (image.isNull()) {
        qWarning() << "Cannot open BMP file:" << filePath;
        return QVector<QVector<double>>();
    }
    
    // 转换为灰度（类似 MATLAB 的灰度处理）
    QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);
    
    QVector<QVector<double>> im(grayImage.height());
    for (int y = 0; y < grayImage.height(); ++y) {
        im[y].resize(grayImage.width());
        const uchar* scanLine = grayImage.constScanLine(y);
        for (int x = 0; x < grayImage.width(); ++x) {
            im[y][x] = static_cast<double>(scanLine[x]);  // 相当于 MATLAB 的 double()
        }
    }
    
    return im;
}

void ImageWidget::loadBmpImage(const QString& filePath)
{
    QPixmap originalImage;
    if (originalImage.load(filePath)) {
        current_image_name_ = extractAfterLastSlash(filePath);
        m_originalImageSize = originalImage.size();
        
        // 计算图像在widget中的显示区域（保持宽高比）
        qreal scaleX = static_cast<qreal>(width()) / m_originalImageSize.width();
        qreal scaleY = static_cast<qreal>(height()) / m_originalImageSize.height();
        qreal scale = qMin(scaleX, scaleY);
        
        int displayWidth = qRound(m_originalImageSize.width() * scale);
        int displayHeight = qRound(m_originalImageSize.height() * scale);
        int displayX = (width() - displayWidth) / 2;
        int displayY = (height() - displayHeight) / 2;
        
        m_displayRect = QRect(displayX, displayY, displayWidth, displayHeight);
        
        // 缩放图像以适应显示
        m_backgroundImage = originalImage.scaled(displayWidth, displayHeight, 
                                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
        update();
    } else {
        qDebug() << "Failed to load image:" << filePath;
    }
}

// 坐标映射函数：从widget坐标映射到图像原始坐标
QPointF ImageWidget::widgetToImageCoordinates(const QPointF& widgetPoint)
{
    if (m_displayRect.isEmpty() || m_originalImageSize.isEmpty()) {
        return widgetPoint;
    }
    
    qreal scaleX = static_cast<qreal>(m_originalImageSize.width()) / m_displayRect.width();
    qreal scaleY = static_cast<qreal>(m_originalImageSize.height()) / m_displayRect.height();
    
    qreal imageX = (widgetPoint.x() - m_displayRect.left()) * scaleX;
    qreal imageY = (widgetPoint.y() - m_displayRect.top()) * scaleY;
    
    return QPointF(imageX, imageY);
}

// 坐标映射函数：从图像原始坐标映射到widget坐标
QPointF ImageWidget::imageToWidgetCoordinates(const QPointF& imagePoint)
{
    if (m_displayRect.isEmpty() || m_originalImageSize.isEmpty()) {
        return imagePoint;
    }
    
    qreal scaleX = static_cast<qreal>(m_displayRect.width()) / m_originalImageSize.width();
    qreal scaleY = static_cast<qreal>(m_displayRect.height()) / m_originalImageSize.height();
    
    qreal widgetX = imagePoint.x() * scaleX + m_displayRect.left();
    qreal widgetY = imagePoint.y() * scaleY + m_displayRect.top();
    
    return QPointF(widgetX, widgetY);
}

// 矩形映射函数：从widget矩形映射到图像原始矩形
QRectF ImageWidget::widgetToImageRect(const QRectF& widgetRect)
{
    QPointF topLeft = widgetToImageCoordinates(widgetRect.topLeft());
    QPointF bottomRight = widgetToImageCoordinates(widgetRect.bottomRight());
    return QRectF(topLeft, bottomRight);
}

// 矩形映射函数：从图像原始矩形映射到widget矩形
QRectF ImageWidget::imageToWidgetRect(const QRectF& imageRect)
{
    QPointF topLeft = imageToWidgetCoordinates(imageRect.topLeft());
    QPointF bottomRight = imageToWidgetCoordinates(imageRect.bottomRight());
    return QRectF(topLeft, bottomRight);
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景（如果没有加载图片，使用默认背景）
    if (!m_backgroundImage.isNull()) {
        // 在计算好的显示区域绘制图像
        painter.drawPixmap(m_displayRect, m_backgroundImage);
    } else {
        painter.fillRect(rect(), QColor(240, 240, 240));
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "请加载BMP图片");
        
        // 在没有图像时，显示区域为整个widget
        m_displayRect = rect();
    }
    
    // 绘制子矩形框（离散排列）
    if (m_rectanglesVisible) {
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(QColor(255, 0, 0, 30));
        
        for (int i = 0; i < m_subRects.size(); ++i) {
            painter.drawRect(m_subRects[i]);
            
            // 为四个角落的矩形框绘制特殊标记
            if (i == getCornerRectIndex(TopLeft) || i == getCornerRectIndex(TopRight) ||
                i == getCornerRectIndex(BottomLeft) || i == getCornerRectIndex(BottomRight)) {
                painter.setBrush(Qt::yellow);
                painter.drawEllipse(m_subRects[i].center(), 4, 4);
                painter.setBrush(QColor(255, 0, 0, 30));
            }
        }
    }

    
    // 显示信息
    // painter.setPen(Qt::blue);
    // painter.drawText(10, 20, QString("子矩形尺寸: %1×%2").arg(m_subRectWidth).arg(m_subRectHeight));
    // painter.drawText(10, 40, QString("子矩形间距: %1px").arg(m_subRectSpacing));
    // painter.drawText(10, 60, QString("图像尺寸: %1×%2").arg(m_originalImageSize.width()).arg(m_originalImageSize.height()));
    // painter.drawText(10, 80, "操作提示: 拖拽移动 | 角落缩放 | Ctrl+滚轮调整间距");
    
    // 绘制边框，明确显示固定区域
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
    
    // 绘制图像显示区域边框（调试用）
    painter.setPen(QPen(Qt::green, 1, Qt::DashLine));
    painter.drawRect(m_displayRect);
}

// void ImageWidget::paintEvent(QPaintEvent *event)
// {
//     Q_UNUSED(event)
    
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);
    
//     // 绘制背景（如果没有加载图片，使用默认背景）
//     if (!m_backgroundImage.isNull()) {
//         painter.drawPixmap(rect(), m_backgroundImage);
//     } else {
//         painter.fillRect(rect(), QColor(240, 240, 240));
//         painter.setPen(Qt::gray);
//         painter.drawText(rect(), Qt::AlignCenter, "请加载BMP图片");
//     }
    
//     // 绘制子矩形框（离散排列）
//     painter.setPen(QPen(Qt::red, 2));
//     painter.setBrush(QColor(255, 0, 0, 30));
    
//     for (int i = 0; i < m_subRects.size(); ++i) {
//         painter.drawRect(m_subRects[i]);
        
//         // 为四个角落的矩形框绘制特殊标记
//         if (i == getCornerRectIndex(TopLeft) || i == getCornerRectIndex(TopRight) ||
//             i == getCornerRectIndex(BottomLeft) || i == getCornerRectIndex(BottomRight)) {
//             painter.setBrush(Qt::yellow);
//             painter.drawEllipse(m_subRects[i].center(), 4, 4);
//             painter.setBrush(QColor(255, 0, 0, 30));
//         }
//     }
    
//     // 显示信息
//     painter.setPen(Qt::blue);
//     painter.drawText(10, 20, QString("子矩形间距: %1px").arg(m_subRectSpacing));
//     painter.drawText(10, 40, QString("操作提示: 拖拽移动 | 角落缩放 | Ctrl+滚轮调整间距"));
// }

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
        
        // 检查是否点击在角落矩形上
        if (isNearCorner(event->pos(), TopLeft)) {
            m_scaling = true;
            m_scalingCorner = TopLeft;
        } else if (isNearCorner(event->pos(), TopRight)) {
            m_scaling = true;
            m_scalingCorner = TopRight;
        } else if (isNearCorner(event->pos(), BottomLeft)) {
            m_scaling = true;
            m_scalingCorner = BottomLeft;
        } else if (isNearCorner(event->pos(), BottomRight)) {
            m_scaling = true;
            m_scalingCorner = BottomRight;
        } 
        // 检查是否点击在任意子矩形上（用于移动）
        else {
            for (const QRectF& rect : m_subRects) {
                if (rect.contains(event->pos())) {
                    m_dragging = true;
                    break;
                }
            }
        }
    }
}

void ImageWidget::setRectanglesVisible(bool visible)
{
    if (m_rectanglesVisible != visible) {
        m_rectanglesVisible = visible;
        update();  // 触发重绘
    }
}

void ImageWidget::saveCoordinates()
{
    if(current_image_name_ == "") return;
    QVector<qreal> xCoordinates_temp;  // 存储所有矩形的x1和x2坐标
    QVector<qreal> yCoordinates_temp;  // 存储所有矩形的y1和y2坐标
    
    for (const QRectF& widgetRect : m_subRects) {
        // 将widget坐标映射到图像原始坐标
        QRectF imageRect = widgetToImageRect(widgetRect);
        
        // 保存左上角坐标 (x1, y1) - 转换为整数
        xCoordinates_temp.append(qRound(imageRect.left()));
        yCoordinates_temp.append(qRound(imageRect.top()));
        
        // 保存右下角坐标 (x2, y2) - 转换为整数
        xCoordinates_temp.append(qRound(imageRect.right()));
        yCoordinates_temp.append(qRound(imageRect.bottom()));
    }
    coordinatesMap_[current_image_name_].xCoordinates = xCoordinates_temp;
    coordinatesMap_[current_image_name_].yCoordinates = yCoordinates_temp;
    qDebug() << "=== saveCoordinates ===";
    // qDebug() << "=== 坐标已保存（映射到原始图像尺寸）===";
    // qDebug() << "原始图像尺寸:" << m_originalImageSize;
    // qDebug() << "X坐标向量 (" << m_xCoordinates.size() << "个值):" << m_xCoordinates;
    // qDebug() << "Y坐标向量 (" << m_yCoordinates.size() << "个值):" << m_yCoordinates;
    
    // // 输出每个矩形的详细坐标
    // for (int i = 0; i < m_xCoordinates.size(); i += 2) {
    //     int rectIndex = i / 2;
    //     qDebug() << "矩形" << rectIndex + 1 << ": (" 
    //              << m_xCoordinates[i] << "," << m_yCoordinates[i] << ") -> ("
    //              << m_xCoordinates[i+1] << "," << m_yCoordinates[i+1] << ")";
    // }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF delta = event->pos() - m_lastMousePos;
    
    if (m_dragging) {
        // 移动所有子矩形框
        for (QRectF& rect : m_subRects) {
            rect.translate(delta);
        }
        updateBoundingRect();
        update();
    } else if (m_scaling && m_scalingCorner != -1) {
        // 根据角落进行缩放
        scaleFromCorner(static_cast<Corner>(m_scalingCorner), delta);
        update();
    }
    
    m_lastMousePos = event->pos();
    
    // 更新光标形状
    Qt::CursorShape cursorShape = Qt::ArrowCursor;
    if (m_dragging) {
        cursorShape = Qt::SizeAllCursor;
    } else if (m_scaling) {
        switch (m_scalingCorner) {
        case TopLeft: cursorShape = Qt::SizeFDiagCursor; break;
        case TopRight: cursorShape = Qt::SizeBDiagCursor; break;
        case BottomLeft: cursorShape = Qt::SizeBDiagCursor; break;
        case BottomRight: cursorShape = Qt::SizeFDiagCursor; break;
        default: break;
        }
    } else {
        if (isNearCorner(event->pos(), TopLeft) || isNearCorner(event->pos(), BottomRight)) {
            cursorShape = Qt::SizeFDiagCursor;
        } else if (isNearCorner(event->pos(), TopRight) || isNearCorner(event->pos(), BottomLeft)) {
            cursorShape = Qt::SizeBDiagCursor;
        } else {
            for (const QRectF& rect : m_subRects) {
                if (rect.contains(event->pos())) {
                    cursorShape = Qt::SizeAllCursor;
                    break;
                }
            }
        }
    }
    
    setCursor(cursorShape);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(m_dragging || m_scaling) saveCoordinates();
    m_dragging = false;
    m_scaling = false;
    m_scalingCorner = -1;
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    // 按住Ctrl键时调整间距
    if (event->modifiers() & Qt::ControlModifier) {
        m_subRectSpacing *= (event->angleDelta().y() > 0 ? 1.1 : 0.9);
        m_subRectSpacing = qMax(1.0, qMin(50.0, m_subRectSpacing));
        
        // 重新布局子矩形（保持当前整体位置和大小）
        QPointF oldCenter = m_boundingRect.center();
        updateSubRectsLayout();
        
        QPointF delta = oldCenter - m_boundingRect.center();
        for (QRectF& rect : m_subRects) {
            rect.translate(delta);
        }
        updateBoundingRect();
        update();
    }
}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_subRects.size() == 24) {
        updateSubRectsLayout();
        update();
    }
}

void ImageWidget::updateSubRectsLayout()
{
    m_subRects.clear();
    
    const int rows = 4;
    const int cols = 6;
    
    // 使用当前设置的子矩形尺寸
    qreal subRectWidth = m_subRectWidth;
    qreal subRectHeight = m_subRectHeight;
    
    qreal totalWidth = cols * subRectWidth + (cols - 1) * m_subRectSpacing;
    qreal totalHeight = rows * subRectHeight + (rows - 1) * m_subRectSpacing;
    
    qreal startX = (width() - totalWidth) / 2.0;
    qreal startY = (height() - totalHeight) / 2.0;
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            qreal x = startX + col * (subRectWidth + m_subRectSpacing);
            qreal y = startY + row * (subRectHeight + m_subRectSpacing);
            
            QRectF subRect(x, y, subRectWidth, subRectHeight);
            m_subRects.append(subRect);
        }
    }
    
    updateBoundingRect();
}

void ImageWidget::updateBoundingRect()
{
    if (m_subRects.isEmpty()) {
        m_boundingRect = QRectF();
        return;
    }
    
    m_boundingRect = m_subRects.first();
    for (const QRectF& rect : m_subRects) {
        m_boundingRect = m_boundingRect.united(rect);
    }
}

int ImageWidget::getCornerRectIndex(Corner corner)
{
    const int rows = 4;
    const int cols = 6;
    
    switch (corner) {
    case TopLeft: return 0;                    // 第1行第1列
    case TopRight: return cols - 1;           // 第1行第6列
    case BottomLeft: return (rows - 1) * cols; // 第4行第1列
    case BottomRight: return rows * cols - 1;  // 第4行第6列
    default: return -1;
    }
}

bool ImageWidget::isNearCorner(const QPointF& point, Corner corner)
{
    int rectIndex = getCornerRectIndex(corner);
    if (rectIndex < 0 || rectIndex >= m_subRects.size()) {
        return false;
    }
    
    const QRectF& cornerRect = m_subRects[rectIndex];
    
    // 检查是否在角落矩形的角落区域（15像素范围内）
    QRectF topLeftArea(cornerRect.topLeft(), QSizeF(15, 15));
    QRectF topRightArea(cornerRect.topRight() - QPointF(15, 0), QSizeF(15, 15));
    QRectF bottomLeftArea(cornerRect.bottomLeft() - QPointF(0, 15), QSizeF(15, 15));
    QRectF bottomRightArea(cornerRect.bottomRight() - QPointF(15, 15), QSizeF(15, 15));
    
    switch (corner) {
    case TopLeft: return topLeftArea.contains(point);
    case TopRight: return topRightArea.contains(point);
    case BottomLeft: return bottomLeftArea.contains(point);
    case BottomRight: return bottomRightArea.contains(point);
    default: return false;
    }
}

void ImageWidget::scaleFromCorner(Corner corner, const QPointF& delta)
{
    if (m_subRects.isEmpty()) return;
    
    // 获取对角落的矩形索引（作为缩放锚点）
    int anchorCornerIndex = -1;
    switch (corner) {
    case TopLeft: anchorCornerIndex = getCornerRectIndex(BottomRight); break;
    case TopRight: anchorCornerIndex = getCornerRectIndex(BottomLeft); break;
    case BottomLeft: anchorCornerIndex = getCornerRectIndex(TopRight); break;
    case BottomRight: anchorCornerIndex = getCornerRectIndex(TopLeft); break;
    default: return;
    }
    
    if (anchorCornerIndex == -1) return;
    
    QPointF anchorPoint = m_subRects[anchorCornerIndex].center();
    int scalingRectIndex = getCornerRectIndex(corner);
    
    // 计算缩放比例
    QPointF scalingRectCenter = m_subRects[scalingRectIndex].center();
    QPointF vectorToAnchor = anchorPoint - scalingRectCenter;
    
    if (vectorToAnchor.manhattanLength() < 1.0) return;
    
    // 根据拖动方向和角落计算缩放因子
    qreal scaleX = 1.0, scaleY = 1.0;
    
    switch (corner) {
    case TopLeft:
        // 左上角：向右下拖动放大，向左上拖动缩小（不变）
        scaleX = 1.0 - delta.x() / vectorToAnchor.x();
        scaleY = 1.0 - delta.y() / vectorToAnchor.y();
        break;
    case TopRight:
        // 右上角：向左下拖动放大，向右上拖动缩小（不变）
        scaleX = 1.0 - delta.x() / vectorToAnchor.x();
        scaleY = 1.0 - delta.y() / vectorToAnchor.y();
        break;
    case BottomLeft:
        // 左下角：向左下拖动放大，向右上拖动缩小（修改）
        scaleX = 1.0 - delta.x() / vectorToAnchor.x();  // 改为+
        scaleY = 1.0 - delta.y() / vectorToAnchor.y();  // 改为+
        break;
    case BottomRight:
        // 右下角：向右下拖动放大，向左上拖动缩小（修改）
        scaleX = 1.0 - delta.x() / vectorToAnchor.x();  // 改为-
        scaleY = 1.0 - delta.y() / vectorToAnchor.y();  // 改为-
        break;
    default:
        return;
    }
    
    // 限制最小缩放比例
    scaleX = qMax(0.3, qMin(3.0, scaleX));
    scaleY = qMax(0.3, qMin(3.0, scaleY));
    
    // 应用缩放变换
    for (int i = 0; i < m_subRects.size(); ++i) {
        QRectF& rect = m_subRects[i];
        QPointF relativePos = rect.center() - anchorPoint;
        
        // 缩放矩形尺寸
        qreal newWidth = qMax(10.0, rect.width() * scaleX);
        qreal newHeight = qMax(10.0, rect.height() * scaleY);
        rect.setWidth(newWidth);
        rect.setHeight(newHeight);
        
        // 调整位置
        QPointF newRelativePos(relativePos.x() * scaleX, relativePos.y() * scaleY);
        QPointF newCenter = anchorPoint + newRelativePos;
        rect.moveCenter(newCenter);
    }
    
    updateBoundingRect();
}

void ImageWidget::updateSubRectWidth(int width)
{
    // 保存当前所有矩形的中心位置
    QVector<QPointF> centers;
    for (const QRectF& rect : m_subRects) {
        centers.append(rect.center());
    }
    
    m_subRectWidth = width;
    
    // 更新布局但保持中心位置
    if (!m_subRects.isEmpty()) {
        for (int i = 0; i < m_subRects.size(); ++i) {
            QRectF& rect = m_subRects[i];
            QPointF center = centers[i];
            rect.setWidth(m_subRectWidth);
            rect.moveCenter(center);
        }
        updateBoundingRect();
        update();
        saveCoordinates();
    }
}

void ImageWidget::updateSubRectHeight(int height)
{
    // 保存当前所有矩形的中心位置
    QVector<QPointF> centers;
    for (const QRectF& rect : m_subRects) {
        centers.append(rect.center());
    }
    
    m_subRectHeight = height;
    
    // 更新布局但保持中心位置
    if (!m_subRects.isEmpty()) {
        for (int i = 0; i < m_subRects.size(); ++i) {
            QRectF& rect = m_subRects[i];
            QPointF center = centers[i];
            rect.setHeight(m_subRectHeight);
            rect.moveCenter(center);
        }
        updateBoundingRect();
        update();
        saveCoordinates();
    }
}



