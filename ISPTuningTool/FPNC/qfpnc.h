#ifndef QFPNC_H
#define QFPNC_H

#include "Common/UI/qcurvechartgroup.h"
#include "Common/UI/qdirectionbuttongroup.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qtabwndcomm.h"
#include <QGroupBox>
#include <QPushButton>
#include <QWidget>
#include <QtDataVisualization>
#include "HistogramWidget.h"
#include "CalibrationProcessor.h"
#include <QMessageBox>


namespace Ui {
class QFpnc;
}

class TitledSurfaceContainer : public QWidget
{
    Q_OBJECT
public:
    TitledSurfaceContainer(Q3DSurface *surface, const QString &title, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        // 创建容器
        QWidget *container = QWidget::createWindowContainer(surface);
        
        // 创建标题
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("QLabel {"
                                 "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                 "stop:0 #2980b9, stop:1 #3498db);"
                                 "color: white;"
                                 "font-size: 14px;"
                                 "font-weight: bold;"
                                 //"padding: 8px;"
                                 "border-radius: 4px 4px 0 0;"
                                 "}");
        
        // 设置布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(2, 2, 2, 2);
        
        mainLayout->addWidget(titleLabel);
        mainLayout->addWidget(container);
        
        // 设置容器大小
        container->setMinimumSize(400, 300);
    }
};

class QFpnc : public QWidget
{
    Q_OBJECT

public:
    explicit QFpnc(QWidget *parent = nullptr);
    ~QFpnc();

private slots:
    void setInputFolder();
    void setOutputFolder();
    void calibrate();
    void resetViewOri()
    {
        // 获取相机对象
        Q3DCamera *camera = ori_surface->scene()->activeCamera();
        
        // 重置相机位置和目标
        camera->setPosition(m_initialCameraPosition);
        camera->setTarget(m_initialCameraTarget);
        
        // 可选：重置其他相机参数
        camera->setXRotation(0.0f);
        camera->setYRotation(0.0f);
        camera->setZoomLevel(100.0f); // 默认缩放级别
    }
    void resetViewOut()
    {
        // 获取相机对象
        Q3DCamera *camera = out_surface->scene()->activeCamera();
        
        // 重置相机位置和目标
        camera->setPosition(m_initialCameraPosition);
        camera->setTarget(m_initialCameraTarget);
        
        // 可选：重置其他相机参数
        camera->setXRotation(0.0f);
        camera->setYRotation(0.0f);
        camera->setZoomLevel(100.0f); // 默认缩放级别
    }

private:
    Ui::QFpnc *ui;
    QPointer<QCurveChartGroup> ori_FPN_chart      = nullptr;
    Q3DSurface* ori_surface      = nullptr;
    QSurfaceDataProxy* ori_surfaceDataProxy      = nullptr;
    QSurface3DSeries* ori_surface3DSeries      = nullptr;
    QSurfaceDataArray* ori_dataArray      = nullptr;

    QHeightMapSurfaceDataProxy* input_proxy  = nullptr;
    QHeightMapSurfaceDataProxy* output_proxy  = nullptr;


    QPointer<QCurveChartGroup> BLC_FPN_chart      = nullptr;
    Q3DSurface* out_surface      = nullptr;
    QSurfaceDataProxy* out_surfaceDataProxy      = nullptr;
    QSurface3DSeries* out_surface3DSeries      = nullptr;
    QSurfaceDataArray* out_dataArray      = nullptr;
    TitledSurfaceContainer *titledContainerOri = nullptr;
    TitledSurfaceContainer *titledContainerOut = nullptr;
    HistogramWidget *histogramWidget_ori_;
    HistogramWidget *histogramWidget_out_;
    void init();
    void initConnect();
    QString getLastInputFolder();
    QString getLastOutputFolder();
    void saveOutputFolder(const QString& path);
    void saveInputFolder(const QString& path);
    QSurfaceDataArray* convertToSurfaceDataArray(const QVector<QVector<uint16_t>>& data2D);
    QSurfaceDataArray* convertWithSampling(const QVector<QVector<uint16_t>>& data2D, 
                                        int rowStep = 4, int colStep = 4);
    QImage createHeightMap(const QVector<QVector<uint16_t>>& data2D);
    void setupCamera(Q3DSurface* surface);
    QStringList curvePointNameX;
    QStringList curvePointNameY;
    std::shared_ptr<CalibrationProcessor> processor_;
    QString input_folder_;
    QString output_folder_;
    int data_bit_width_;
    int calib_bit_width_;
    QVector3D m_initialCameraPosition;
    QVector3D m_initialCameraTarget;
    CalibrationResult result_;
    QSize size2D_;
    double sensGain_;
    double blc_;
    bool calibration_status_;

};

#endif // QFPNC_H
