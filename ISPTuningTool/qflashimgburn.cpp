#include "qflashimgburn.h"
#include "Common/Others/qusbcommunication.h"
#include "M1FlashLib.h"
#include "ui_qflashimgburn.h"
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>

QFlashImgBurn *QFlashImgBurn::instance()
{
    static QFlashImgBurn *_instance = new QFlashImgBurn();
    return _instance;
}

QFlashImgBurn::QFlashImgBurn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QFlashImgBurn)
{
    ui->setupUi(this);
    this->setWindowTitle("Flash Setting");
    ui->configWidget->setVisible(false);
    initPartitionTypeName();
    partitionMap = PartitionMap();

    M1Flash_SetSpiReadWriteFunc( // 设置spi读写函数
        [](int deviceIndex, uint8_t *writeData, int writeLen, uint8_t *readData, int readLen, uint8_t spiMode,
            float spiSpeedMhz, uint8_t dummyTiming) -> int {
            return QUsbCommunication::getInstance().readBurst(*(uint32_t *)(writeData + 1), readData, 4, readLen);
        },
        [](int deviceIndex, uint8_t *writeData, int writeLen, uint8_t spiMode, float spiSpeedMhz) -> int {
            return QUsbCommunication::getInstance().writeBurst(*(uint32_t *)(writeData + 1), writeData + 5, 4,
                writeLen - 5);
        });
}

void QFlashImgBurn::loadFlashImage()
{
    M1Flash_GetFlashImgPartitionInfo(_flashImg, &partitionMap);
    // M1Flash_AutoAdjustFlashImgPartition(_flashImg);
    for (int i = 0; i < partitionMap.count; i++) {
        PartitionInfo                 partitionInfo = partitionMap.partitions[i];
        QPointer<PartitionInfoWidget> infoWidget =
            new PartitionInfoWidget(partitionTypeName.value(partitionInfo.type), partitionInfo);
        partitionInfoMap.insert(partitionInfo.type, infoWidget);
        ui->partitionDataLayout->addWidget(infoWidget);
        partitionDataRangeMap.insert(partitionInfo.type,
            std::make_pair(partitionMap.offsets[i], (partitionMap.offsets[i] + partitionInfo.size))); // 更新值
    }
}

QFlashImgBurn::~QFlashImgBurn()
{
    delete ui;
}

void QFlashImgBurn::initPartitionTypeName()
{
    partitionTypeName.insert(PART_DDR_SETTING, "DDR Setting");
    partitionTypeName.insert(PART_GENERAL_HW_SETTING, "General HW Setting");
    partitionTypeName.insert(PART_FW_SETTING, "Firmware Setting");
    partitionTypeName.insert(PART_FW_RECOVER, "Firmware Recover");
    partitionTypeName.insert(PART_SENSOR_SETTING, "Sensor Setting");
    partitionTypeName.insert(PART_FW_PATCH, "FW Patch code");
    partitionTypeName.insert(PART_DCPC, "DCPC");
    partitionTypeName.insert(PART_OSD, "OSD");
    partitionTypeName.insert(PART_ARM_BOOT, "ARM Boot");
    partitionTypeName.insert(PART_LINUX_KERNEL, "ARM Linux Kernel");
    partitionTypeName.insert(PART_LINUX_ROOTFS, "ARM Linux RootFS");
    partitionTypeName.insert(PART_MOVE_DATA, "Move Data");

    for (auto iter = partitionTypeName.begin(); iter != partitionTypeName.end(); iter++)
        ui->partitionTypeBox->addItem(iter.value());

    defaultPartitionOrder.push_back(PART_DDR_SETTING);
    defaultPartitionOrder.push_back(PART_GENERAL_HW_SETTING);
    defaultPartitionOrder.push_back(PART_FW_SETTING);
    defaultPartitionOrder.push_back(PART_FW_RECOVER);
    defaultPartitionOrder.push_back(PART_SENSOR_SETTING);
    defaultPartitionOrder.push_back(PART_FW_PATCH);
    defaultPartitionOrder.push_back(PART_DCPC);
    defaultPartitionOrder.push_back(PART_OSD);
    defaultPartitionOrder.push_back(PART_ARM_BOOT);
    defaultPartitionOrder.push_back(PART_LINUX_KERNEL);
    defaultPartitionOrder.push_back(PART_LINUX_ROOTFS);
    defaultPartitionOrder.push_back(PART_MOVE_DATA);
}

uint32_t QFlashImgBurn::getPartitionOffset(PartitionType type)
{
    for (int i = 0; i < partitionMap.count; i++) {
        if (type == partitionMap.partitions[i].type) {
            return partitionMap.offsets[i];
        }
    }
    return 0;
}

void QFlashImgBurn::updatePartitionMap()
{
    // 读取当前界面所有partitioninfowidget，更新到partitionMap(offset正确)
    if (!partitionInfoMap.empty()) {
        for (int i = 0; i < partitionMap.count; i++) {
            if (partitionInfoMap.contains(partitionMap.partitions[i].type)) {
                auto part = partitionInfoMap.find(partitionMap.partitions[i].type).value();
                part->getInfo();
                PartitionInfo currentPartition = part->partitionInfo();
                partitionMap.partitions[i]     = currentPartition;
                partitionMap.offsets[i]        = part->partitionOffSet();
            }
        }
        M1Flash_SetFlashImgPartitionInfo(_flashImg, partitionMap);
    }
}

void QFlashImgBurn::updateUIDataRangeMap()
{
    // 缓存ui信息预处理，判断offset是否合理
    partitionDataRangeMap.clear();
    for (auto it = partitionInfoMap.begin(); it != partitionInfoMap.end(); it++) {
        auto part = it.value();
        part->getInfo();
        PartitionInfo cPartition = part->partitionInfo();
        uint32_t      valbegin   = part->partitionOffSet();
        uint32_t      valEnd     = valbegin + cPartition.size;
        partitionDataRangeMap.insert(cPartition.type, std::make_pair(valbegin, valEnd)); // 更新值
    }
}

bool QFlashImgBurn::allCheckWell()
{
    // 检测所有partitionInfo widget
    updateUIDataRangeMap(); // 生成partitionDataRangeMap
    QString overlapName;
    QString currentName;
    bool    isOverlap = false;
    for (auto it = partitionInfoMap.begin(); it != partitionInfoMap.end(); it++) {
        QPointer<PartitionInfoWidget> infoWidget = it.value();

        infoWidget->getInfo();
        uint32_t      dataBegin        = 0;
        uint32_t      dataEnd          = 0;
        PartitionInfo currentPartition = infoWidget->partitionInfo();
        dataBegin                      = infoWidget->partitionOffSet();
        dataEnd                        = dataBegin + currentPartition.size;
        for (auto iter = partitionDataRangeMap.begin(); iter != partitionDataRangeMap.end(); iter++) {
            if (currentPartition.type == iter.key()) {
                continue;
            }
            // qDebug() << partitionTypeName.value(iter.key()) << dataBegin << dataEnd;
            // qDebug() << iter.value().first << iter.value().second;
            if ((dataBegin > iter.value().first && dataBegin < iter.value().second)
                || (dataEnd > iter.value().first && dataEnd < iter.value().second)) {
                // 重叠判断
                isOverlap   = true;
                overlapName = partitionTypeName.value(iter.key());
                currentName = partitionTypeName.value(currentPartition.type);
                break;
            }
        }
        if (isOverlap) {
            QString str = " There is a conflict between " + currentName + " and " + overlapName;
            QMessageBox::warning(this, QString::fromUtf8(""), str, QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

bool QFlashImgBurn::checkCurrentPart(QPointer<PartitionInfoWidget> partWidget)
{
    updateUIDataRangeMap(); // 生成partitionDataRangeMap
    QString overlapName;
    QString currentName;
    bool    isOverlap = false;
    partWidget->getInfo();
    uint32_t      dataBegin        = 0;
    uint32_t      dataEnd          = 0;
    PartitionInfo currentPartition = partWidget->partitionInfo();
    dataBegin                      = partWidget->partitionOffSet();
    dataEnd                        = dataBegin + currentPartition.size;

    for (auto iter = partitionDataRangeMap.begin(); iter != partitionDataRangeMap.end(); iter++) {
        if (iter.key() == currentPartition.type) {
            continue;
        }
        if ((dataBegin > iter.value().first && dataBegin < iter.value().second)
            || (dataEnd > iter.value().first && dataEnd < iter.value().second)) {
            // 重叠判断
            isOverlap   = true;
            overlapName = partitionTypeName.value(iter.key());
            currentName = partitionTypeName.value(currentPartition.type);
            break;
        }
    }
    if (isOverlap) {
        QString str = " There is a conflict between " + currentName + " and " + overlapName;
        QMessageBox::warning(this, QString::fromUtf8(""), str, QMessageBox::Ok);
        return false;
    } else {
        return true;
    }
}

int QFlashImgBurn::savePartitionDataToCfgFile(PartitionType type, QString path)
{
    // updatePartitionMap();
    return M1Flash_GetFlashImgPartitionDataToCfgFile(_flashImg, type, path.toStdString().c_str());
}

int QFlashImgBurn::loadPartitionDataFromCfgFile(PartitionType type, QString path)
{
    return M1Flash_SetFlashImgPartitionDataFromCfgFile(_flashImg, type, path.toStdString().c_str());
    // updatePartitionMap();
}

int QFlashImgBurn::removePartition(PartitionType type)
{
    partitionInfoMap.remove(type);
    return M1Flash_RemoveFlashImgPartition(_flashImg, type);
}

void QFlashImgBurn::on_loadHeaderBtn_clicked() // header相关
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
    if (fileName == "")
        return;

    configLoadPath = fileName;
    ui->configEdit->setText(configLoadPath);
    M1Flash_SetFlashImgPartitionDataFromCfgFile(_flashImg, PART_ALL, configLoadPath.toStdString().c_str());
}

void QFlashImgBurn::on_exportHeaderButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All file(*.*)"));
    if (fileName == "")
        return;
    if (ERROR_NO_ERR
        != M1Flash_GetFlashImgPartitionDataToCfgFile(_flashImg, PART_ALL, fileName.toStdString().c_str())) {
        QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Export Header Fail"), QMessageBox::Ok);
        return;
    }
}

void QFlashImgBurn::on_releaseFlashBtn_clicked()
{
    ui->configEdit->clear();
    partitionMap = PartitionMap();
    for (auto iter = partitionInfoMap.begin(); iter != partitionInfoMap.end(); iter++) {
        iter.value()->setVisible(false);
    }
    partitionInfoMap.clear();
    partitionDataRangeMap.clear();
    configLoadPath = "";
    M1Flash_ReleaseFlashImg(_flashImg);
    _flashImg = nullptr;
    updateFlashBtnStatus();
}

void QFlashImgBurn::on_createFlashBtn_clicked()
{
    createFlash(CreateFlashType::New);
    updateFlashBtnStatus();
}

void QFlashImgBurn::on_createFlashFromFileBtn_clicked()
{
    createFlash(CreateFlashType::File);
    updateFlashBtnStatus();
}

void QFlashImgBurn::on_createFlashFromDeviceBtn_clicked()
{
    createFlash(CreateFlashType::Device);
    updateFlashBtnStatus();
}

void QFlashImgBurn::on_saveFlashBtn_clicked()
{
    if (!QFlashImgBurn::instance()->allCheckWell()) {
        int ret = QMessageBox::question(this, QString::fromUtf8(""), QString::fromUtf8("Is AutoAdjust Offset？"),
            QMessageBox::Ok, QMessageBox::Cancel);
        if (ret == QMessageBox::Ok)
            on_autoAdjustBtn_clicked(); // 自动计算offset
        else
            return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All file(*.*)"));
    if (fileName == "")
        return;
    updatePartitionMap();
    M1Flash_WriteFlashImg(_flashImg, fileName.toStdString().c_str());
    updateFlashBtnStatus();
}

void progressWindow(QString msg, QWidget *parent, QFuture<qint64> &future)
{
    QProgressDialog progress(parent);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progress.setWindowTitle(msg);
    progress.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    progress.setCancelButton(nullptr);
    progress.setMinimumDuration(0);
    progress.setRange(0, 0);
    progress.setValue(0);
    progress.show();
    while (!future.isFinished()) {
        QCoreApplication::processEvents();
    }
    progress.close();
}

void QFlashImgBurn::on_burnFlashBtn_clicked()
{
    if (!QFlashImgBurn::instance()->allCheckWell()) {
        int ret = QMessageBox::question(this, QString::fromUtf8(""), QString::fromUtf8("Is AutoAdjust Offset？"),
            QMessageBox::Ok, QMessageBox::Cancel);
        if (ret == QMessageBox::Ok)
            on_autoAdjustBtn_clicked(); // 自动计算offset
        else
            return;
    }
    if (0 != QUsbCommunication::getInstance().checkState()) {
        QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Device is not connected"),
            QMessageBox::Ok);
        return;
    }

    int ret = QMessageBox::question(this, QString::fromUtf8(""), QString::fromUtf8("Burn Flash"), QMessageBox::Ok,
        QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    int partMask = PART_NONE;
    for (auto iter = partitionInfoMap.begin(); iter != partitionInfoMap.end(); iter++) {
        QPointer<PartitionInfoWidget> part = iter.value();
        partMask |= part->partitionInfo().type;
    }
    if (partMask == PART_NONE) {
        QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Please select partition to burn"),
            QMessageBox::Ok);
        return;
    }
    const QVector<QString> supportFlashModel = { "IS25WP128F", "IS25WP256D", "IS25WP512M" };
    if (QMessageBox::Cancel
        == QMessageBox::question(this, QString::fromUtf8(""),
            QString::fromUtf8("Support Flash Model: \n%1\nUnsupported Flash Model will cause burn fail\n\nContinue?")
                .arg(supportFlashModel.join("\n")),
            QMessageBox::Ok, QMessageBox::Cancel))
        return;

    // set offsets
    updatePartitionMap();
    int    isErasing = QMessageBox::question(this, QString::fromUtf8(""), QString::fromUtf8("Is Erasing?"),
           QMessageBox::Ok, QMessageBox::Cancel);
    qint64 eraseTime = -1, flashTime = -1;
    if (isErasing == QMessageBox::Ok) {
        auto eraseFuture = QtConcurrent::run([=]() {
            auto start = QDateTime::currentDateTime();
            if (ERROR_NO_ERR != M1Flash_EraseFlash(deviceIndex)) {
                return qint64(-1);
            }
            return start.msecsTo(QDateTime::currentDateTime());
        });
        // erasing window
        progressWindow("Erasing...", this, eraseFuture);
        eraseTime = eraseFuture.result();
        if (eraseTime == -1) {
            QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Erase Flash Fail"), QMessageBox::Ok);
            return;
        }
    }
    auto flashFuture = QtConcurrent::run([=]() {
        auto start = QDateTime::currentDateTime();
        if (ERROR_NO_ERR != M1Flash_BurnFlash(_flashImg, static_cast<PartitionType>(partMask), deviceIndex)) {
            return qint64(-1);
        }
        return start.msecsTo(QDateTime::currentDateTime());
    });
    // burning window
    progressWindow("Burning...", this, flashFuture);
    flashTime = flashFuture.result();
    if (flashTime == -1) {
        QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Burn Flash Fail"), QMessageBox::Ok);
        return;
    }
    QString msg = QString::fromUtf8("Burn Success\n");
    if (isErasing == QMessageBox::Ok)
        msg += QString::fromUtf8("Erase Time: %1ms\n").arg(eraseTime);
    msg += QString::fromUtf8("Burn Time: %1ms").arg(flashTime);
    QMessageBox::information(this, QString::fromUtf8(""), msg, QMessageBox::Ok);
    updateFlashBtnStatus();
}

void QFlashImgBurn::on_autoAdjustBtn_clicked()
{
    // 自动排序，更新offset[]
    M1Flash_AutoAdjustFlashImgPartition(_flashImg);
    M1Flash_GetFlashImgPartitionInfo(_flashImg, &partitionMap);
    for (int i = 0; i < partitionMap.count; i++) {
        if (partitionInfoMap.contains(partitionMap.partitions[i].type)) {
            QPointer<PartitionInfoWidget> infoWidget = partitionInfoMap.find(partitionMap.partitions[i].type).value();
            // qDebug() << partitionMap.offsets[i];
            infoWidget->setPartitionOffSet(partitionMap.offsets[i]);
            infoWidget->setPartitionInfo(partitionMap.partitions[i]);
            infoWidget->updateData();
        }
    }
}

void QFlashImgBurn::on_addPartitionBtn_clicked()
{
    PartitionType addPartitionType;
    for (PartitionType type : defaultPartitionOrder) {
        if (partitionTypeName.value(type) == ui->partitionTypeBox->currentText()) {
            if (partitionInfoMap.contains(type))
                return;
            addPartitionType = type;
            break;
        }
    } // 获取当前添加partitionType
    PartitionInfo partitionInfo = PartitionInfo();
    partitionInfo.type          = addPartitionType;
    QPointer<PartitionInfoWidget> infoWidget =
        new PartitionInfoWidget(partitionTypeName.value(addPartitionType), partitionInfo);
    partitionInfoMap.insert(addPartitionType, infoWidget);
    ui->partitionDataLayout->addWidget(infoWidget);
    // 新增对应partitionInfoWidget
}

void QFlashImgBurn::createFlash(CreateFlashType type)
{
    int res;
    switch (type) {
    case CreateFlashType::New:
        res = M1Flash_CreateFlashImg(&_flashImg);
        break;
    case CreateFlashType::File: {
        QString flashFileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
        res                   = M1Flash_CreateFlashImgFromImgFile(&_flashImg, flashFileName.toStdString().c_str());
        break;
    }
    case CreateFlashType::Device:
        res = M1Flash_CreateFlashImgFromDevice(&_flashImg, deviceIndex);
        break;
    }
    if (res != ERROR_NO_ERR) {
        QMessageBox::warning(this, QString::fromUtf8(""), QString::fromUtf8("Create Flash Image Fail"),
            QMessageBox::Ok);
        return;
    }
    loadFlashImage(); // 解析flashImage
}

void QFlashImgBurn::updateFlashBtnStatus()
{
    ui->createFlashBtn->setEnabled(_flashImg == nullptr);
    ui->createFlashFromFileBtn->setEnabled(_flashImg == nullptr);
    ui->createFlashFromDeviceBtn->setEnabled(_flashImg == nullptr);
    ui->saveFlashBtn->setEnabled(_flashImg != nullptr);
    ui->burnFlashBtn->setEnabled(_flashImg != nullptr);
    ui->releaseFlashBtn->setEnabled(_flashImg != nullptr);
    ui->autoAdjustBtn->setEnabled(_flashImg != nullptr);
    ui->addPartitionBtn->setEnabled(_flashImg != nullptr);
    ui->configWidget->setVisible(_flashImg != nullptr);
}

//////PartitionInfoWidget//////////////////////////////
PartitionInfoWidget::PartitionInfoWidget(QString partitionName, PartitionInfo info, QWidget *parent)
    : QWidget(parent)
{
    this->setMaximumHeight(200);
    groupBox                     = new QGroupBox(partitionName);
    QPointer<QGridLayout> layout = new QGridLayout();

    layout->setContentsMargins(1, 1, 1, 1);
    layout->setVerticalSpacing(0);
    layout->addWidget(groupBox, 0, 1);

    this->setLayout(layout);

    c_partitionInfo = info;
    initGroupBox(c_partitionInfo);
}
PartitionInfoWidget::~PartitionInfoWidget() { }

void PartitionInfoWidget::initGroupBox(PartitionInfo info)
{
    QPointer<QHBoxLayout> hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(1, 1, 1, 1);
    groupBox->setLayout(hlayout);
    offsetlabel = new QLabel("offset");
    lengthlabel = new QLabel("length");
    sizelabel   = new QLabel("size");
    offsetEdit  = new QLineEdit;
    lengthEdit  = new QLineEdit;
    sizeEdit    = new QLineEdit;

    configLabel     = new QLabel("Config: ");
    configPathEdit  = new QLineEdit;
    exportConfigBtn = new QPushButton;
    exportConfigBtn->setText("Export");
    loadConfigBtn = new QPushButton;
    loadConfigBtn->setText("Load");

    connect(exportConfigBtn, &QAbstractButton::clicked, this, [=]() {
        // QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All file(*.*)"));
        if (fileName == "")
            return;
        auto res = QFlashImgBurn::instance()->savePartitionDataToCfgFile(c_partitionInfo.type, fileName);
        if (ERROR_NO_ERR != res) {
            QMessageBox::warning(this, QString::fromUtf8(""),
                QString::fromUtf8("Export Config Fail. Error Code: %1").arg(static_cast<uint32_t>(res), 0, 16),
                QMessageBox::Ok);
            return;
        }
    });

    connect(loadConfigBtn, &QAbstractButton::clicked, this, [=]() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
        if (fileName == "")
            return;
        auto res = QFlashImgBurn::instance()->loadPartitionDataFromCfgFile(c_partitionInfo.type, fileName);
        if (ERROR_NO_ERR != res) {
            QMessageBox::warning(this, QString::fromUtf8(""),
                QString::fromUtf8("Load Config Fail. Error Code: %1").arg(static_cast<uint32_t>(res), 0, 16),
                QMessageBox::Ok);
            return;
        }
        configPathEdit->setText(fileName);
        QFlashImgBurn::instance()->updatePartitionInfo(c_partitionInfo.type);
    });

    removeBtn = new QPushButton;
    removeBtn->setText("Remove");

    connect(removeBtn, &QAbstractButton::clicked, this, [=]() {
        int ret = QMessageBox::question(this, QString::fromUtf8(""), QString::fromUtf8("Remove Partition?"),
            QMessageBox::Ok, QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            QFlashImgBurn::instance()->removePartition(c_partitionInfo.type);
            this->setVisible(false);
        }
    });

    // 显示Partition信息
    offsetEdit->setText(QString::number(QFlashImgBurn::instance()->getPartitionOffset(info.type)));
    lengthEdit->setText(QString::number(info.dataLength));
    sizeEdit->setText(QString::number(info.size));
    lengthEdit->setEnabled(false);
    sizeEdit->setEnabled(false);

    connect(offsetEdit, &QLineEdit::editingFinished, this, [=]() {
        offSet = offsetEdit->text().toUInt(nullptr, 10);
        if (QFlashImgBurn::instance()->checkCurrentPart(this)) {
            QFlashImgBurn::instance()->updateUIDataRangeMap();
        }
    });

    hlayout->addWidget(configLabel, 1);
    hlayout->addWidget(configPathEdit, 10);
    hlayout->addWidget(exportConfigBtn, 1);
    hlayout->addWidget(loadConfigBtn, 1);
    hlayout->addWidget(removeBtn, 1);
    hlayout->addWidget(offsetlabel, 1);
    hlayout->addWidget(offsetEdit, 1);
    hlayout->addWidget(lengthlabel, 1);
    hlayout->addWidget(lengthEdit, 1);
    hlayout->addWidget(sizelabel, 1);
    hlayout->addWidget(sizeEdit, 1);

    getInfo();
}

PartitionInfo PartitionInfoWidget::partitionInfo()
{
    return c_partitionInfo;
}

void PartitionInfoWidget::setPartitionInfo(PartitionInfo newPartition)
{
    c_partitionInfo = newPartition;
    size            = c_partitionInfo.size;
    dataLength      = c_partitionInfo.dataLength;
}

uint32_t PartitionInfoWidget::partitionOffSet()
{
    return offSet;
}

void PartitionInfoWidget::setPartitionOffSet(uint32_t val)
{
    offSet = val;
    offsetEdit->setText(QString::number(val));
}

void PartitionInfoWidget::getInfo()
{
    offSet                     = offsetEdit->text().toUInt(nullptr, 10);
    c_partitionInfo.size       = sizeEdit->text().toUInt(nullptr, 10);
    c_partitionInfo.dataLength = lengthEdit->text().toUInt(nullptr, 10);

    offsetEdit->setText(QString::number(offSet));
    sizeEdit->setText(QString::number(c_partitionInfo.size));
    lengthEdit->setText(QString::number(c_partitionInfo.dataLength));
}

void PartitionInfoWidget::updateData()
{
    offsetEdit->setText(QString::number(offSet));
    sizeEdit->setText(QString::number(c_partitionInfo.size));
    lengthEdit->setText(QString::number(c_partitionInfo.dataLength));
}

void QFlashImgBurn::updatePartitionInfo(PartitionType type)
{
    PartitionMap partitionMap;
    M1Flash_GetFlashImgPartitionInfo(_flashImg, &partitionMap);
    for (int i = 0; i < partitionMap.count; i++) {
        if (partitionMap.partitions[i].type == type) {
            if (partitionInfoMap.contains(type)) {
                QPointer<PartitionInfoWidget> infoWidget = partitionInfoMap.find(type).value();
                infoWidget->setPartitionInfo(partitionMap.partitions[i]);
                infoWidget->setPartitionOffSet(partitionMap.offsets[i]);
                infoWidget->updateData();
            }
        }
    }
}
