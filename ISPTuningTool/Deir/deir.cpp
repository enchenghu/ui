#include "deir.h"
#include "ui_deir.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

QVector<QVector<int>> transpose(const QVector<QVector<double>>& matrix) {
    if (matrix.isEmpty()) return QVector<QVector<int>>();
    
    int originalRows = matrix.size();
    int originalCols = matrix[0].size();
    
    // 创建转置矩阵：3行4列
    QVector<QVector<int>> transposed(originalCols);  // 3行
    for (int i = 0; i < originalCols; ++i) {
        transposed[i].resize(originalRows);  // 每行4列
    }
    
    // 执行转置
    for (int i = 0; i < originalRows; ++i) {
        for (int j = 0; j < originalCols; ++j) {
            //transposed[j][i] = matrix[i][j] / 512.0;
            transposed[j][i] = static_cast<int>(matrix[i][j]);
        }
    }
    
    return transposed;
}

QVector<QVector<double>> divide_512(const QVector<QVector<int>>& matrix) {
    if (matrix.isEmpty()) return QVector<QVector<double>>();
    
    int originalRows = matrix.size();
    int originalCols = matrix[0].size();
    
    QVector<QVector<double>> transposed(originalRows);  
    for (int i = 0; i < originalRows; ++i) {
        transposed[i].resize(originalCols); 
    }
    
    for (int i = 0; i < originalRows; ++i) {
        for (int j = 0; j < originalCols; ++j) {
            transposed[i][j] = matrix[i][j] / 512.0;
        }
    }
    
    return transposed;
}

Deir::Deir(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Deir)
{
    wb_rgbir_processor_ = std::make_unique<ManualWBProcessor>();
    ui->setupUi(this);
    init();
    initConnect();
}

Deir::~Deir()
{
    delete ui;
}

void Deir::loadImageLablelListInit()
{
   oriStyle = 
        "QLabel {"
        "    border: 2px solid #999999;"      // 中灰色边框
        "    border-radius: 8px;"
        "    padding: 15px;"
        "    background-color: #D3D3D3;"      // 浅灰色背景
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #333333;"                 // 深灰色文字
        "    text-align: center;"
        "}";
    greenStyle = 
        "QLabel {"
        "    border: 2px solid #27ae60;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "    background-color: #2ecc71;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #ffffff;"
        "    text-align: center;"
        "}";
    loadImageLablelList.clear();
    loadImageLablelList.push_back(std::pair<int, QLabel*>(0, ui->label_9));
    loadImageLablelList.push_back(std::pair<int, QLabel*>(1, ui->label_10));
    loadImageLablelList.push_back(std::pair<int, QLabel*>(2, ui->label_11));
    loadImageLablelList.push_back(std::pair<int, QLabel*>(3, ui->label_12));
    loadImageLablelList.push_back(std::pair<int, QLabel*>(4, ui->label_18));
    loadImageLablelList.push_back(std::pair<int, QLabel*>(5, ui->label_29));
    for(auto label : loadImageLablelList){
        label.second->setMouseTracking(true);
        label.second->installEventFilter(this);
        label.second->setStyleSheet(oriStyle);
    }
}

void Deir::resetOtherLabels(QLabel* cur_label)
{
    for(auto label : loadImageLablelList) {
        if(label.second != cur_label) {
            label.second->setStyleSheet(oriStyle);
        }
    }
}

bool Deir::eventFilter(QObject *obj, QEvent *event)
{
    int modelIndex = ui->comboBox_2->currentIndex();
    if(fileList_noir_.size() < 6 && modelIndex == 0)
    {
        return QWidget::eventFilter(obj, event);
    }
    if(fileList_irp0_.size() < 6 && modelIndex == 1)
    {
        return QWidget::eventFilter(obj, event);
    }
    if(fileList_irp1_.size() < 6 && modelIndex == 2)
    {
        return QWidget::eventFilter(obj, event);
    }

    for(auto label : loadImageLablelList) {
        if (obj == label.second) {
            QString filename;
            if(modelIndex == 0) {
                filename = fileList_noir_[label.first];
            } else if(modelIndex == 1) {
                filename = fileList_irp0_[label.first];
            } else {
                filename = fileList_irp1_[label.first];
            }
            if (event->type() == QEvent::MouseButtonPress && 
                event->type() != QEvent::MouseButtonDblClick) {
                label.second->setStyleSheet(oriStyle);
                Label_name->setText(filename);
                filename  = input_folder_ + "/" + filename;
                qDebug() << "current image: " << filename;                
                pImage_->loadBmpImage(filename);
                return true; 
            } else if(event->type() == QEvent::MouseButtonDblClick) {
                label.second->setStyleSheet(greenStyle);
                resetOtherLabels(label.second);
                Label_name->setText(filename);
                viewer_->setWindowTitle(filename);
                filename  = input_folder_ + "/" + filename;
                qDebug() << "current image: " << filename;                
                viewer_->getImage()->loadBmpImage(filename);
                viewer_->show();
                viewer_->raise();
                viewer_->activateWindow();
                return true; 
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

QString Deir::getLastInputFolder()
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    qDebug() << "getLastInputFolder is " << settings.value("deir input folder").toString();
    return settings.value("deir input folder", QDir::currentPath()).toString();
}

void Deir::hideRoi() 
{
    bool currentlyVisible = viewer_->getImage()->areRectanglesVisible();
    viewer_->getImage()->setRectanglesVisible(!currentlyVisible);
}

void Deir::setInputFolder() 
{
    input_folder_ = QFileDialog::getExistingDirectory(
        this,
        "Choose folder for input folder", getLastInputFolder());

    if (!input_folder_.isNull()) {
        qDebug() << "selected input folder : " << input_folder_.toStdString();
        ui->lineEdit_6->setText(input_folder_);
        saveInputFolder(input_folder_);
    } 

    QDir dir(input_folder_);
    QStringList filters = {"*.bmp"};
    fileList_ = dir.entryList(filters, QDir::Files);
    if(fileList_.empty()){
        QMessageBox::warning(nullptr, "Deir",
                    "当前文件夹无图像数据，请确认",
                    QMessageBox::Ok);
        return;
    }
    loadImagesMap.clear();
    for (const QString &filename : fileList_) {
        QString path  = input_folder_ + "/" + filename;
        loadImagesMap[filename] = pImage_->readBmpImage(path);
        qDebug() << "======image reading:" << filename;
    }
    findIRStrings(fileList_);
    traMat_all_.clear();
    traMat_all_.resize(4, QVector<ColorTransformCalculator::TransformResult>(3));
}

void Deir::saveInputFolder(const QString& path)
{
    QSettings settings("SmartSens", "ISP_Tuning_Tools");
    settings.setValue("deir input folder", path);
    qDebug() << "deir InputFolder is" << path;
}

void Deir::init()
{
    traMat_all_.clear();
    traMat_all_.resize(4, QVector<ColorTransformCalculator::TransformResult>(3));
    ui->lineEdit->setText("8");
    ui->lineEdit_4->setText("16");
    ui->lineEdit_5->setText("225");
    ui->lineEdit_2->setText("48");
    ui->lineEdit_3->setText("48");
    ui->comboBox_2->addItem("1-NOIR");
    ui->comboBox_2->addItem("2-IRP0");
    ui->comboBox_2->addItem("3-IRP1");
    ui->comboBox_3->addItem("1-NOIR");
    ui->comboBox_3->addItem("2-IRP0");
    ui->comboBox_3->addItem("3-IRP1");
    ui->comboBox_3->addItem("mean");
    ui->lineEdit_6->setText(getLastInputFolder());
    
    loadImageLablelListInit();

    pImage_ = new ImageWidget(nullptr, 400, 300, false);
    deirMrx[0]  = std::make_shared<MicroDataGrid>();
    deirMrx[1]  = std::make_shared<MicroDataGrid>();
    deirMrx[2]  = std::make_shared<MicroDataGrid>();
    ui->horizontalLayout_deirMrx->addWidget(deirMrx[0].get());
    ui->horizontalLayout_deirMrx->addWidget(deirMrx[1].get());
    ui->horizontalLayout_deirMrx->addWidget(deirMrx[2].get());

    Label_name = new QLabel("image name");
    ui->verticalLayout_3->addWidget(Label_name, 0, Qt::AlignLeft | Qt::AlignVCenter);
    ui->verticalLayout_3->addWidget(pImage_, 1, Qt::AlignLeft |  Qt::AlignTop);
    viewer_ = new OriginalSizeViewer();
}

void Deir::initConnect()
{
    connect(ui->pushButton_8, SIGNAL(clicked(bool)), this, SLOT(deirProcess()));
    connect(ui->pushButton_9, SIGNAL(clicked(bool)), this, SLOT(deirMeanProcess()));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(setInputFolder()));
    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->pushButton_3, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->pushButton_4, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->pushButton_5, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->pushButton_6, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->pushButton_7, SIGNAL(clicked(bool)), this, SLOT(hideRoi()));
    connect(ui->comboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                qDebug() << "当前索引:" << index;
                if(traMat_all_[index][0].state == false && index != 3) {
                    QMessageBox::warning(nullptr, "Deir Process", "当前模式无校准后数据，请点击校准按钮获取", QMessageBox::Ok);
                    return;
                }
                if(traMat_all_[index][0].state == false && index == 3) {
                    QMessageBox::warning(nullptr, "Deir Process", "点击export按钮获取均值信息", QMessageBox::Ok);
                    return;
                }
                deirMrx[0]->updateData(traMat_all_[index][0].traMat_transpose);   
                ui->lineEdit_13->setText(QString::number(traMat_all_[index][0].ct));
                deirMrx[1]->updateData(traMat_all_[index][1].traMat_transpose);   
                ui->lineEdit_14->setText(QString::number(traMat_all_[index][1].ct));
                deirMrx[2]->updateData(traMat_all_[index][2].traMat_transpose);   
                ui->lineEdit_15->setText(QString::number(traMat_all_[index][2].ct));
            });

}

void Deir::processImage(const QString& filename_coIR, const QString& filename_noIR, int modeIndex)
{
    ManualWBProcessor::ROIResult res_co;
    ManualWBProcessor::WBResult res_no;
    if (loadImagesMap.find(filename_coIR) != loadImagesMap.end() 
        && loadImagesMap.find(filename_noIR) != loadImagesMap.end()) {
        if(loadImagesMap[filename_coIR].empty()) {
            qDebug() << filename_coIR << " data is empty";
            return;
        }
        if(loadImagesMap[filename_noIR].empty()) {
            qDebug() << filename_noIR << " data is empty";
            return;
        }
        qDebug() << "manualWB_RGBIR_coIR process:" << filename_coIR << ", manualWB_RGBIR_noIR process: " << filename_noIR;
        res_co = wb_rgbir_processor_->manualWB_RGBIR_coIR(loadImagesMap[filename_coIR], filename_coIR);
        res_no = wb_rgbir_processor_->manualWB_RGBIR_noIR(loadImagesMap[filename_noIR], res_co.roi_x, res_co.roi_y);
        ColorTransformCalculator::TransformResult res_traMat = ColorTransformCalculator::calTraMat_RGBIR(res_co.BMs, res_co.GMs, res_co.RMs, res_co.NMs, \
                                                                                                        res_no.BMs, res_no.GMs, res_no.RMs);
        QVector<QVector<int>> traMat_int = transpose(res_traMat.traMat);  
        QVector<QVector<double>> traMat = divide_512(traMat_int);  
        res_traMat.traMat_transpose_int = traMat_int;
        res_traMat.traMat_transpose = traMat;
        qDebug() << "traMat:" << traMat;   
        res_traMat.ct = res_no.ct;
        res_traMat.state = true;
        if(filename_coIR.contains("D65_")){
            deirMrx[0]->updateData(traMat);   
            ui->lineEdit_13->setText(QString::number(res_no.ct));
            traMat_all_[modeIndex][0] = res_traMat;
        }   
        if(filename_coIR.contains("A_")){
            deirMrx[2]->updateData(traMat);   
            ui->lineEdit_15->setText(QString::number(res_no.ct));
            traMat_all_[modeIndex][2] = res_traMat;
        } 
        if(filename_coIR.contains("TL84_")){
            deirMrx[1]->updateData(traMat);  
            ui->lineEdit_14->setText(QString::number(res_no.ct));
            traMat_all_[modeIndex][1] = res_traMat;
        }                                                                                           
    } else {
        qDebug() << ("=========can't find the image in loadImagesMap: " + filename_coIR + " or " + filename_noIR);
    }
}

void Deir::setAlgParams()
{
    wb_rgbir_processor_->setBitWidth(ui->lineEdit->text().toInt());
    wb_rgbir_processor_->setBlcTarget(ui->lineEdit_4->text().toInt());
    wb_rgbir_processor_->setOverexp(ui->lineEdit_5->text().toInt());
    wb_rgbir_processor_->setRoiWidth(ui->lineEdit_2->text().toInt());
    wb_rgbir_processor_->setRoiHeight(ui->lineEdit_3->text().toInt());
}

void Deir::deirMeanProcess()
{
    if(traMat_all_[0][0].state == false && traMat_all_[1][0].state == false && traMat_all_[2][0].state == false){
        QMessageBox::warning(nullptr, "Deir Process",
                            "无数据，请计算后再求平均",
                            QMessageBox::Ok);
        return;                       
    }
    QVector<QVector<int>> M_input;
    for(int i = 0; i < 3; i++){
        QVector<int> cur_data;
        for(const auto & it : traMat_all_[i]){
            if(it.state == false) {
                qDebug() << "!!!warining!!!M" << i <<  " empty";
                break;
            }
            cur_data.push_back(it.ct);
            for(const auto & sub_vec : it.traMat_transpose_int){
                for(const auto & val : sub_vec) cur_data.push_back(val);
            }
        }
        if(!cur_data.empty()) M_input.push_back(cur_data);
    }
    
    MatrixProcessor::ProcessResult res = MatrixProcessor::processMatrices(M_input);
    if(res.success){
        QMessageBox::information(nullptr, "success",
                            "已保存",
                            QMessageBox::Ok);
    } else {
        QMessageBox::warning(nullptr, "failed",
                            "保存失败",
                            QMessageBox::Ok);        
    }
}

void Deir::deirProcess()
{
    // processImage("D65_noir_ircut_off_expo_0x6540_gain_0x0020_00_RAW_20241112143458187.bmp", 
    //     "D65_noir_ircut_on_expo_0x6540_gain_0x0020_00_RAW_20241112143455906.bmp");
#if 1
    setAlgParams();
    std::map<QString, Coordinates> roi_map = viewer_->getImage()->getROIsMap();
    if(roi_map.empty()){
        QMessageBox::warning(nullptr, "Deir Process",
                            "未选中ROI！请先双击图像名称标签再选中ROI区域, 完成后再点击",
                            QMessageBox::Ok);
        return;    
    }
    wb_rgbir_processor_->setROIMap(roi_map);
    int modelIndex = ui->comboBox_2->currentIndex();
    QVector<std::pair<QString, QString>> filePairList;
    switch (modelIndex) {
        case 0: filePairList = filePairList_noir_; break;
        case 1: filePairList = filePairList_irp0_; break;
        case 2: filePairList = filePairList_irp1_; break;
    }
    for (auto & filename : filePairList) {
        processImage(filename.first, filename.second, modelIndex);
    }
    ui->comboBox_3->setCurrentIndex(modelIndex);
#endif
}

QVector<std::pair<QString, QString>> classifyStrings(const QStringList& stringList , int mode) {
    QVector<std::pair<QString, QString>> result;
    
    // 定义需要查找的模式对
    QVector<std::pair<QString, QString>> patterns_noir = {
        {"D65_noir_ircut_off", "D65_noir_ircut_on"},
        {"A_noir_ircut_off", "A_noir_ircut_on"},
        {"TL84_noir_ircut_off", "TL84_noir_ircut_on"}
    };

    QVector<std::pair<QString, QString>> patterns_ir1 = {
        {"D65_ir1_ircut_off", "D65_ir1_ircut_on"},
        {"A_ir1_ircut_off", "A_ir1_ircut_on"},
        {"TL84_ir1_ircut_off", "TL84_ir1_ircut_on"}
    };

    QVector<std::pair<QString, QString>> patterns_ir0 = {
        {"D65_ir0_ircut_off", "D65_ir0_ircut_on"},
        {"A_ir0_ircut_off", "A_ir0_ircut_on"},
        {"TL84_ir0_ircut_off", "TL84_ir0_ircut_on"}
    };

    QVector<std::pair<QString, QString>> patterns;
    switch (mode)
    {
        case 0: patterns = patterns_noir; break;
        case 1: patterns = patterns_ir0; break;
        case 2: patterns = patterns_ir1; break;
    }
    
    for (const auto& pattern : patterns) {
        QString offStr, onStr;
        
        // 查找 off 字符串
        for (const QString& str : stringList) {
            if (str.contains(pattern.first, Qt::CaseInsensitive)) {
                offStr = str;
                break;
            }
        }
        
        // 查找 on 字符串
        for (const QString& str : stringList) {
            if (str.contains(pattern.second, Qt::CaseInsensitive)) {
                onStr = str;
                break;
            }
        }
        
        // 如果找到了对应的字符串对，添加到结果中
        if (!offStr.isEmpty() && !onStr.isEmpty()) {
            result.append(std::make_pair(offStr, onStr));
        }
    }
    
    return result;
}

void Deir::findIRStrings(const QStringList & file_list)
{
    fileList_noir_.clear();
    fileList_irp0_.clear();
    fileList_irp1_.clear();

    filePairList_noir_ = classifyStrings(file_list, 0);
    filePairList_irp0_ = classifyStrings(file_list, 1);
    filePairList_irp1_ = classifyStrings(file_list, 2);
    
    std::copy_if(file_list.begin(), file_list.end(),
                 std::back_inserter(fileList_noir_),
                 [](const QString& str) {
                     return str.contains("_noir_", Qt::CaseInsensitive);
                 });

    std::copy_if(file_list.begin(), file_list.end(),
                 std::back_inserter(fileList_irp0_),
                 [](const QString& str) {
                     return str.contains("_ir0_", Qt::CaseInsensitive);
                 });  

    std::copy_if(file_list.begin(), file_list.end(),
                 std::back_inserter(fileList_irp1_),
                 [](const QString& str) {
                     return str.contains("_ir1_", Qt::CaseInsensitive);
                 });  
}

// OriginalSizeViewer 实现
OriginalSizeViewer::OriginalSizeViewer(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("原始尺寸视图 - 2592×1944");
    setModal(false);  // 非模态对话框，可以同时操作主窗口
    
    // 创建布局
    m_imageWidget = new ImageWidget(nullptr, 1200, 900, true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_imageWidget, 0);

    QGroupBox* box2 = new QGroupBox(this);
	QGridLayout* box2Layout = new QGridLayout(box2);
    label_width = new QLabel("宽度");
    label_height = new QLabel("高度");
    width_Slider = new QSlider(box2);
    height_Slider = new QSlider(box2);
    width_Slider->setRange(1, 100);  // 
    width_Slider->setValue(50);
    width_Slider->setOrientation(Qt::Orientation::Horizontal);
    height_Slider->setRange(1, 100);  // 高度范围15-150像素
    height_Slider->setValue(50);
    height_Slider->setOrientation(Qt::Orientation::Horizontal);
    box2Layout->addWidget(label_width, 0, 0, Qt::AlignRight);
    box2Layout->addWidget(width_Slider, 0, 1, Qt::AlignLeft);
    box2Layout->addWidget(label_height, 0, 2, Qt::AlignRight);
    box2Layout->addWidget(height_Slider, 0, 3, Qt::AlignLeft);
    box2->setLayout(box2Layout);
    layout->addWidget(box2, 1);

    connect(width_Slider, &QSlider::valueChanged, this, &OriginalSizeViewer::onWidthSliderChanged);
    connect(height_Slider, &QSlider::valueChanged, this, &OriginalSizeViewer::onHeightSliderChanged);
    // 设置窗口属性
    //setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动删除
}

void OriginalSizeViewer::onWidthSliderChanged(int value)
{
    label_width->setText(QString("宽度: %1px").arg(value));
    m_imageWidget->updateSubRectWidth(value);
}

void OriginalSizeViewer::onHeightSliderChanged(int value)
{
    label_height->setText(QString("高度: %1px").arg(value));
    m_imageWidget->updateSubRectHeight(value);
}