#include "qfpnc.h"
#include "ui_qfpnc.h"
#include <QComboBox>
#include <QGroupBox>
#include <QPointer>
#include <QSplitter>
#include <set>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/Q3DCamera>

// 保存最后使用的路径
void QFpnc::saveInputFolder(const QString& path)
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    settings.setValue("input folder", path);
    qDebug() << "saveInputFolder is" << path;
}

void QFpnc::saveOutputFolder(const QString& path)
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    settings.setValue("output folder", path);
}

// 获取最后使用的路径
QString QFpnc::getLastInputFolder()
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    qDebug() << "getLastInputFolder is " << settings.value("input folder").toString();
    return settings.value("input folder", QDir::currentPath()).toString();
}

QString QFpnc::getLastOutputFolder()
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    return settings.value("output folder", QDir::currentPath()).toString();
}

QFpnc::QFpnc(QWidget *parent): QWidget(parent), ui(new Ui::QFpnc)
{
    ui->setupUi(this);
    processor_ = std::make_shared<CalibrationProcessor>();
    calibration_status_  =false;
    init();
    initConnect();
}

QFpnc::~QFpnc()
{
    delete ui;
}

void QFpnc::initConnect()
{
  connect(ui->pushButton_open_input_folder, SIGNAL(clicked(bool)), this, SLOT(setInputFolder()));
  connect(ui->pushButton_open_output_folder, SIGNAL(clicked(bool)), this, SLOT(setOutputFolder()));
  connect(ui->pushButton_calibrate, SIGNAL(clicked(bool)), this, SLOT(calibrate()));
  connect(ui->pushButton_reset_input, SIGNAL(clicked(bool)), this, SLOT(resetViewOri()));
  connect(ui->pushButton_reset_output, SIGNAL(clicked(bool)), this, SLOT(resetViewOut()));

  connect(ui->checkBox_databit8, &QCheckBox::stateChanged, [this](int state) {
            if(state > 0){
              data_bit_width_ = 8;  
              ui->checkBox_databit10->setChecked(false);
              ui->checkBox_databit12->setChecked(false);
            }
          });
  connect(ui->checkBox_databit10, &QCheckBox::stateChanged, [this](int state) {
            if(state > 0){
              data_bit_width_ = 10;  
              ui->checkBox_databit8->setChecked(false);
              ui->checkBox_databit12->setChecked(false);
            }
          });
  connect(ui->checkBox_databit12, &QCheckBox::stateChanged, [this](int state) {
            if(state > 0){
              data_bit_width_ = 12;  
              ui->checkBox_databit10->setChecked(false);
              ui->checkBox_databit8->setChecked(false);
            }
          });
  connect(ui->checkBox_calibbit8, &QCheckBox::stateChanged, [this](int state) {
            if(state > 0){
              calib_bit_width_ = 8;  
              ui->checkBox_calibbit6->setChecked(false);
            }
          });
  connect(ui->checkBox_calibbit6, &QCheckBox::stateChanged, [this](int state) {
            if(state > 0){
              calib_bit_width_ = 6;  
              ui->checkBox_calibbit8->setChecked(false);
            }
          });
}

void QFpnc::setInputFolder() 
{
    input_folder_ = QFileDialog::getExistingDirectory(
        this,
        "Choose folder for input folder", getLastInputFolder());

    if (!input_folder_.isNull()) {
        qDebug() << "selected input folder : " << input_folder_.toStdString();
        ui->lineEdit_input_folder->setText(input_folder_);
        saveInputFolder(input_folder_);
    } 
}

void QFpnc::setOutputFolder() 
{
    output_folder_ = QFileDialog::getExistingDirectory(
        this,
        "Choose folder for output folder", getLastOutputFolder());

    if (!output_folder_.isNull()) {
        qDebug() << "selected output folder : " << output_folder_.toStdString();
        ui->lineEdit_output_folder->setText(output_folder_);
        saveOutputFolder(output_folder_);
    } 
    // 4. 保存校准文件
    if(calibration_status_) {
        processor_->saveCalibrationFile(output_folder_, result_);
    }
}

void QFpnc::calibrate() 
{
    CalibrationResult result;
    input_folder_ = ui->lineEdit_input_folder->text();
    output_folder_ = ui->lineEdit_output_folder->text();
    result.size2D.setWidth(ui->lineEdit_size_width->text().toInt());
    result.size2D.setHeight(ui->lineEdit_size_height->text().toInt());
    result.blc = ui->lineEdit_blc->text().toDouble();
    if(result.blc <= 0.0 || result.blc >= (std::pow(2, data_bit_width_) - 1)){
      calibration_status_  = false;
      QMessageBox::critical(nullptr, "error",
                           "BLC值超过范围:(0, 2^Bit_Width - 1)",
                           QMessageBox::Ok);
      return;                   
    }
    result.sensGain = ui->lineEdit_gain->text().toDouble();
    result.dataBitWidth = data_bit_width_;
    result.calibBitWidth = calib_bit_width_;
    int ret  = processor_->getCalibFile(result, input_folder_, output_folder_);
    if(ret) {
      calibration_status_  = false;
      QMessageBox::warning(nullptr, "calibration",
                           "calibrate failed!",
                           QMessageBox::Ok);
      return;
    }                          

    ui->lineEdit_status_min->setText(QString::number(result.fpn_min));
    ui->lineEdit_status_max->setText(QString::number(result.fpn_max));
    ui->lineEdit_status_mean->setText(QString::number(result.fpn_mean));

    ui->lineEdit_status_ble->setText(QString::number(result.bl_err));
    ui->lineEdit_status_std->setText(QString::number(result.std_val));
    ui->lineEdit_status_clip->setText(QString::number(result.clip_value));

    ui->lineEdit_out_pregainshift->setText(QString::number(result.gain_pre_shift));
    ui->lineEdit_out_calibshift->setText(QString::number(result.calib_shift));
    ui->lineEdit_out_offset->setText(QString::number(result.offset));

    histogramWidget_ori_->showAccurateBarHistogram(result.histogram, -0.5, 1023.5, "fpn");
    histogramWidget_out_->showAccurateBarHistogram(result.histogram_blc, -512.5, 511.5, "fpn after BLC");
    ori_surfaceDataProxy->resetArray(convertWithSampling(result.in_img));
    out_surfaceDataProxy->resetArray(convertWithSampling(result.out_img));
    result_ = result;
    calibration_status_ = true;
    // input_proxy->setHeightMap(createHeightMap(result.in_img));
    // output_proxy->setHeightMap(createHeightMap(result.out_img));

}

QSurfaceDataArray* QFpnc::convertToSurfaceDataArray(const QVector<QVector<uint16_t>>& data2D)
{
    QSurfaceDataArray* dataArray = new QSurfaceDataArray();
    
    int rows = data2D.size();
    if (rows == 0) return dataArray;
    
    int cols = data2D[0].size();
    
    dataArray->reserve(rows);
    
    for (int i = 0; i < rows; ++i) {
        QSurfaceDataRow* newRow = new QSurfaceDataRow(cols);
        
        for (int j = 0; j < cols; ++j) {
            // 将uint16_t数据转换为float，并设置坐标
            // X坐标 = j, Y坐标 = data2D[i][j], Z坐标 = i
            (*newRow)[j].setPosition(QVector3D(j, static_cast<float>(data2D[i][j]), i));
        }
        
        dataArray->append(newRow);
    }
    
    return dataArray;
}

void QFpnc::init()
{
    data_bit_width_ = 10;  
    calib_bit_width_ = 8;  
    ui->checkBox_databit10->setChecked(true);
    ui->checkBox_calibbit8->setChecked(true);
    ui->lineEdit_size_width->setText("2592");
    ui->lineEdit_size_height->setText("1944");
    ui->lineEdit_blc->setText("64");
    ui->lineEdit_gain->setText("15.75");
    ui->lineEdit_input_folder->setText(getLastInputFolder());
    ui->lineEdit_output_folder->setText(getLastOutputFolder());

    histogramWidget_ori_ = new HistogramWidget();
    histogramWidget_out_ = new HistogramWidget();

    ori_surface = new Q3DSurface();

    out_surface = new Q3DSurface();
  
    ori_surfaceDataProxy = new QSurfaceDataProxy();
    ori_surface3DSeries = new QSurface3DSeries(ori_surfaceDataProxy);
    out_surfaceDataProxy = new QSurfaceDataProxy();
    out_surface3DSeries = new QSurface3DSeries(out_surfaceDataProxy);

    // input_proxy = new QHeightMapSurfaceDataProxy();
    // ori_surface3DSeries = new QSurface3DSeries(input_proxy);
    // output_proxy = new QHeightMapSurfaceDataProxy();
    // out_surface3DSeries = new QSurface3DSeries(output_proxy);

    ori_surface->addSeries(ori_surface3DSeries);
    out_surface->addSeries(out_surface3DSeries);

    ui->gridLayout_charts->addWidget(histogramWidget_ori_, 0, 0);
    histogramWidget_ori_->setMinimumWidth(200);
    ui->gridLayout_charts->addWidget(histogramWidget_out_, 1, 0);
    histogramWidget_out_->setMinimumWidth(200);


    ui->gridLayout_charts->addWidget(new QCurveChartGroup, 0, 1);
    ui->gridLayout_charts->addWidget(new QCurveChartGroup, 1, 1);

    ori_surface->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    ori_surface->setReflection(false);
    ori_surface->setReflectivity(0.0f);
    out_surface->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    out_surface->setReflection(false);
    out_surface->setReflectivity(0.0f);
    //setupCamera(ori_surface);
    //setupCamera(out_surface);
    QValue3DAxis* axisY = qobject_cast<QValue3DAxis*>(ori_surface->axisY());
    QValue3DAxis* axisY_out = qobject_cast<QValue3DAxis*>(out_surface->axisY());

    if (!axisY) {
      axisY = new QValue3DAxis();
      ori_surface->setAxisY(axisY);
    }

    if (!axisY_out) {
      axisY_out = new QValue3DAxis();
      out_surface->setAxisY(axisY_out);
    }
    // 设置坐标轴范围
    //axisX->setRange(0.0f, 100.0f);    // X轴范围：0 到 100
    axisY->setRange(0.0f, 150.0f);  // Y轴范围：-50 到 150
    axisY_out->setRange(0.0f, 150.0f);  // Y轴范围：-50 到 150
    //axisZ->setRange(0.0f, 200.0f);    // Z轴范围：0 到 200
    // 存储初始相机位置
    m_initialCameraPosition = ori_surface->scene()->activeCamera()->position();
    m_initialCameraTarget = ori_surface->scene()->activeCamera()->target();
    titledContainerOri = new TitledSurfaceContainer(ori_surface, "input image");
    titledContainerOut = new TitledSurfaceContainer(out_surface, "output image");
    ui->gridLayout_charts->addWidget(titledContainerOri, 0, 1);
    ui->gridLayout_charts->addWidget(titledContainerOut, 1, 1);
}


QImage QFpnc::createHeightMap(const QVector<QVector<uint16_t>>& data2D)
{
    int rows = data2D.size();
    int cols = rows > 0 ? data2D[0].size() : 0;
    
    // 创建高度图数据
    QImage heightMap(cols, rows, QImage::Format_Grayscale16);
    
    // 找到数据范围用于归一化
    uint16_t minVal = std::numeric_limits<uint16_t>::max();
    uint16_t maxVal = std::numeric_limits<uint16_t>::min();
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            minVal = std::min(minVal, data2D[i][j]);
            maxVal = std::max(maxVal, data2D[i][j]);
        }
    }
    
    // 填充高度图
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // 归一化到0-65535范围
            float normalized = static_cast<float>(data2D[i][j] - minVal) / (maxVal - minVal);
            quint16 pixelValue = static_cast<quint16>(normalized * 65535);
            heightMap.setPixel(j, i, pixelValue);
        }
    }
    return heightMap;
    
    // // 创建高度图代理
    // QHeightMapSurfaceDataProxy* proxy = new QHeightMapSurfaceDataProxy(heightMap);
    // QSurface3DSeries* series = new QSurface3DSeries(proxy);
    
    // // 设置到surface
    // surface.addSeries(series);
}

QSurfaceDataArray* QFpnc::convertWithSampling(const QVector<QVector<uint16_t>>& data2D, int rowStep, int colStep)
{
    QSurfaceDataArray* dataArray = new QSurfaceDataArray();
    
    if (data2D.isEmpty()) return dataArray;
    
    int rowCount = data2D.size();
    int colCount = data2D[0].size();
    
    int sampledRows = (rowCount + rowStep - 1) / rowStep;
    int sampledCols = (colCount + colStep - 1) / colStep;
    
    dataArray->reserve(sampledRows);
    
    for (int row = 0; row < rowCount; row += rowStep) {
        QSurfaceDataRow* newRow = new QSurfaceDataRow(sampledCols);
        
        int colIndex = 0;
        for (int col = 0; col < colCount; col += colStep) {
            // 可选：取区域平均值
            float avgValue = 0;
            int count = 0;
            for (int r = row; r < std::min(row + rowStep, rowCount); ++r) {
                for (int c = col; c < std::min(col + colStep, colCount); ++c) {
                    avgValue += data2D[r][c];
                    count++;
                }
            }
            avgValue /= count;
            
            (*newRow)[colIndex].setPosition(QVector3D(col, avgValue, row));
            colIndex++;
        }
        
        dataArray->append(newRow);
    }
    qDebug() << "Reduced from" << rowCount * colCount << "to" << sampledRows * sampledCols << "points";
    return dataArray;
}



void QFpnc::setupCamera(Q3DSurface* surface)
{
    Q3DCamera* camera = surface->scene()->activeCamera();
    
    // 设置X和Y旋转角度
    camera->setXRotation(45.0f);   // 控制上下旋转
    camera->setYRotation(45.0f);   // 控制左右旋转
    
    // 启用环绕模式
    camera->setWrapXRotation(true);
    camera->setWrapYRotation(true);
    
    // 设置缩放范围
    camera->setMinZoomLevel(50.0f);
    camera->setMaxZoomLevel(500.0f);
    camera->setZoomLevel(200.0f);
    
    // 设置目标位置
    camera->setTarget(QVector3D(0.0f, 0.0f, 0.0f));
}