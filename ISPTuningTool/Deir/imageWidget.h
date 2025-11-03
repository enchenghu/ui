#ifndef ImageWidget_H
#define ImageWidget_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QRectF>
#include "deir_common.h"

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr, int width = 800, int height = 600, bool show = false);
    void loadBmpImage(const QString& filePath);
    QVector<QVector<double>> readBmpImage(const QString& filePath);
        // 获取坐标数据
    QVector<qreal> getXCoordinates() const { return m_xCoordinates; }
    QVector<qreal> getYCoordinates() const { return m_yCoordinates; }

    void saveCoordinates();  // 保存坐标到向量
    // 显示/隐藏矩形框
    void setRectanglesVisible(bool visible);
    bool areRectanglesVisible() const { return m_rectanglesVisible; }

    std::map<QString, Coordinates> getROIsMap() const { return coordinatesMap_; }
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void updateSubRectWidth(int width);
    void updateSubRectHeight(int height);

private:
    std::map<QString, Coordinates> coordinatesMap_;
    QPixmap m_backgroundImage;
    QVector<QRectF> m_subRects;  // 子矩形框
    bool m_dragging;
    bool m_scaling;
    QPointF m_lastMousePos;
    qreal m_scale;
    qreal m_subRectSpacing;  // 子矩形间距
    QRectF m_boundingRect;   // 子矩形组的边界框
    int m_scalingCorner;     // 正在缩放的角落：0=左上，1=右上，2=左下，3=右下，-1=无
    // 子矩形尺寸
    qreal m_subRectWidth;
    qreal m_subRectHeight;    
    // 图像原始尺寸和显示尺寸
    QSize m_originalImageSize;    // 图像原始尺寸（2592×1944）
    QRect m_displayRect;          // 图像在widget中的显示区域

    enum Corner {
        TopLeft = 0,
        TopRight = 1,
        BottomLeft = 2,
        BottomRight = 3,
        None = -1
    };
    
    void updateSubRectsLayout();
    void updateBoundingRect();
    int getCornerRectIndex(Corner corner);
    bool isNearCorner(const QPointF& point, Corner corner);
    void scaleFromCorner(Corner corner, const QPointF& delta);
    // 坐标映射函数
    QPointF widgetToImageCoordinates(const QPointF& widgetPoint);
    QPointF imageToWidgetCoordinates(const QPointF& imagePoint);
    QRectF widgetToImageRect(const QRectF& widgetRect);
    QRectF imageToWidgetRect(const QRectF& imageRect);
        // 坐标存储
    QVector<qreal> m_xCoordinates;  // 存储所有矩形的x1和x2坐标
    QVector<qreal> m_yCoordinates;  // 存储所有矩形的y1和y2坐标
    bool hide_;
    bool m_rectanglesVisible;
    QString current_image_name_;
};

#endif // MAINWINDOW_H