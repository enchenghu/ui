#include "qinterfacedebug.h"
#include "Common/Others/qconfigfiletrans.h"
#include "Common/Others/qusbcommunication.h"
#include "qfiledialog.h"
#include "ui_qinterfacedebug.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>
#include <thread>
#include <vector>
#include <SStream>

struct LoadConfigRegisterStruct
{
    uint8_t              slaveId;
    uint32_t             addr;
    uint32_t             addrLen;
    std::vector<uint8_t> data;
    float                speed;
    uint32_t             spiMode;
    uint32_t             valueLen;
    bool                 isIncremental;
    int                  sleepMs;
    bool                 isRead;
    uint32_t             mask;
};

QInterfaceDebug::QInterfaceDebug(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QInterfaceDebug)
{
    ui->setupUi(this);
    this->setWindowTitle("Interface Debug");
    initI2C();
    initSPI();
    initGlobalSettings();
    initLoadConfig();

    ui->tabWidget->setCurrentIndex(1);
    bLoadIICMasterConfig();
    progressDialog.reset();
}

QInterfaceDebug::~QInterfaceDebug()
{
    delete ui;
}

void QInterfaceDebug::initI2C()
{
    QPointer<QDirectionButtonGroup> btnUpDown1 = new QDirectionButtonGroup();
    btnUpDown1->showUpDown();
    ui->btnsLayout1->addWidget(btnUpDown1);
    QPointer<QDirectionButtonGroup> btnUpDown2 = new QDirectionButtonGroup();
    btnUpDown2->showUpDown();
    ui->btnsLayout2->addWidget(btnUpDown2);
    I2CbitCheckBoxGroup = new QBitCheckBoxGroup();
    ui->bitBoxLayout->addWidget(I2CbitCheckBoxGroup);
    I2CbitCheckBoxGroup->setFixedHeight(30);
    I2CbitCheckBoxGroup->setPainterPen(0.01);
    I2CbitCheckBoxGroup->setMargins(10, 0, 0, 0);
    ui->I2CAddrLengthBox->addItems(QStringList() << "0" << "1" << "2" << "3" << "4");
    ui->I2CValueLengthBox->addItems(QStringList() << "1" << "2" << "3" << "4");
    ui->I2CAddrLengthBox->setCurrentIndex(2);
    ui->advanceBtn->setCheckable(true);
    ui->advanceBox->setVisible(false);
    connect(ui->advanceBtn, &QAbstractButton::clicked, this, [=](bool flag) { ui->advanceBox->setVisible(flag); });

    updateI2C();

    connect(ui->I2CSlaveAddrEdit, &QLineEdit::editingFinished, this, [=]() { readI2CContent(); });
    connect(ui->I2CRegAddrEdit, &QLineEdit::editingFinished, this, [=]() { readI2CContent(); });
    connect(ui->I2CRegValueEdit, &QLineEdit::editingFinished, this, [=]() { readI2CContent(); });
    connect(ui->I2CRateEdit, &QLineEdit::editingFinished, this, [=]() { readI2CContent(); });
    connect(ui->I2CAddrLengthBox, &QComboBox::currentTextChanged, this, [=]() { readI2CContent(); });
    connect(ui->I2CValueLengthBox, &QComboBox::activated, this, [=](int index) {
        if (index == 0) {
            I2CbitCheckBoxGroup->setBits(8);
            I2CbitCheckBoxGroup->setMaximumHeight(40);
        } else if (index == 1) {
            I2CbitCheckBoxGroup->setBits(16);
            I2CbitCheckBoxGroup->setMaximumHeight(40);
        } else {
            I2CbitCheckBoxGroup->setBits(32);
            I2CbitCheckBoxGroup->setMaximumHeight(80);
        }
        I2CbitCheckBoxGroup->setMargins(0, 0, 0, 0);
        // readSPIContent();
        I2CbitCheckBoxGroup->setValue(ui->I2CRegValueEdit->text().toUInt(nullptr, 16));
    });
    connect(ui->timingEdit, &QLineEdit::editingFinished, this, [=]() { readI2CContent(); });

    connect(btnUpDown1, &QDirectionButtonGroup::upSignal, this, [=]() {
        RegAddr = RegAddr + 1;
        ui->I2CRegAddrEdit->setText(QString("0x%1").arg(QString::number(RegAddr, 16)));
        ui->IC2ReadBtn->click();
    });
    connect(btnUpDown1, &QDirectionButtonGroup::downSignal, this, [=]() {
        if (RegAddr == 0) {
            return;
        }
        RegAddr = RegAddr - 1;
        ui->I2CRegAddrEdit->setText(QString("0x%1").arg(QString::number(RegAddr, 16)));
        ui->IC2ReadBtn->click();
    });
    connect(btnUpDown2, &QDirectionButtonGroup::upSignal, this, [=]() {
        RegVal = RegVal + 1;
        ui->I2CRegValueEdit->setText(QString("0x%1").arg(QString::number(RegVal, 16)));
        updatebitBox(RegVal, 0);
        ui->IC2WriteBtn->click();
    });
    connect(btnUpDown2, &QDirectionButtonGroup::downSignal, this, [=]() {
        if (RegVal == 0) {
            return;
        }
        RegVal = RegVal - 1;
        ui->I2CRegValueEdit->setText(QString("0x%1").arg(QString::number(RegVal, 16)));
        updatebitBox(RegVal, 0);
        ui->IC2WriteBtn->click();
    });
    connect(I2CbitCheckBoxGroup, &QBitCheckBoxGroup::signalValueChange, this, [=](uint32_t val) {
        RegVal = val;
        ui->I2CRegValueEdit->setText(QString("0x%1").arg(QString::number(RegVal, 16)));
        ui->IC2WriteBtn->click();
    });

    ui->i2cFileAddrLengthCombo->addItems(QStringList() << "0" << "1" << "2" << "3" << "4");
    ui->i2cFileAddrLengthCombo->setCurrentIndex(2);
    connect(ui->i2cFileDumpLengthEdit, &QLineEdit::editingFinished, this,
        [=]() { ui->i2cFileDumpLengthEdit->setText(QString::number(ui->i2cFileDumpLengthEdit->text().toInt())); });
    connect(ui->i2cFileSlaveIdEdit, &QLineEdit::editingFinished, this, [=]() {
        ui->i2cFileSlaveIdEdit->setText(
            QString("0x%1").arg(ui->i2cFileSlaveIdEdit->text().toInt(nullptr, 16), 2, 16, QChar('0')));
    });
    connect(ui->i2cFileStartAddrEdit, &QLineEdit::editingFinished, this, [=]() {
        ui->i2cFileStartAddrEdit->setText(QString("0x%1").arg(ui->i2cFileStartAddrEdit->text().toULongLong(nullptr, 16),
            ui->i2cFileAddrLengthCombo->currentText().toInt() * 2, 16, QChar('0')));
    });
    ui->i2cFileDumpLengthEdit->setText("256");
    ui->i2cFileStartAddrEdit->setText("0x0000");
    ui->i2cFileSlaveIdEdit->setText("0x22");

    connect(ui->fpgaToM1Btn, &QPushButton::clicked, this, &QInterfaceDebug::onFpgaToM1BtnClicked);
    connect(ui->fpgaToSensorBtn, &QPushButton::clicked, this, &QInterfaceDebug::onFpgaToSensorBtnClicked);

    ui->I2CBusComboBox->addItems(QStringList() << "0" << "1");
    ui->I2CBusComboBox->setCurrentIndex(0);
}

void QInterfaceDebug::initSPI()
{
    QPointer<QDirectionButtonGroup> btnUpDown1 = new QDirectionButtonGroup();
    btnUpDown1->showUpDown();
    ui->SPIBtnsLayout1->addWidget(btnUpDown1);
    QPointer<QDirectionButtonGroup> btnUpDown2 = new QDirectionButtonGroup();
    btnUpDown2->showUpDown();
    ui->SPIBtnsLayout2->addWidget(btnUpDown2);
    ui->SPIValLengthBox->addItems(QStringList() << "1" << "2" << "3" << "4");
    SPIbitCheckBoxGroup = new QBitCheckBoxGroup();
    ui->bitBoxLayout2->addWidget(SPIbitCheckBoxGroup);
    SPIbitCheckBoxGroup->setMaximumHeight(40);
    SPIbitCheckBoxGroup->setPainterPen(0.01);
    SPIbitCheckBoxGroup->setMargins(10, 0, 0, 0);
    ui->advSpiBtn->setCheckable(true);
    ui->advSpiBox->setVisible(false);
    connect(ui->advSpiBtn, &QAbstractButton::clicked, this, [=](bool flag) { ui->advSpiBox->setVisible(flag); });
    updateSPI();

    connect(ui->SPIRegAddrEdit, &QLineEdit::editingFinished, this, [=]() { readSPIContent(); });
    connect(ui->SPISpeedEdit, &QLineEdit::editingFinished, this, [=]() { readSPIContent(); });
    connect(ui->SPIRegValueEdit, &QLineEdit::editingFinished, this, [=]() { readSPIContent(); });
    connect(ui->timingEdit, &QLineEdit::editingFinished, this, [=]() { readSPIContent(); });
    connect(SPIbitCheckBoxGroup, &QBitCheckBoxGroup::signalValueChange, this, [=](uint32_t val) {
        SPIRegVal = val;
        ui->SPIRegValueEdit->setText(QString("0x%1").arg(QString::number(SPIRegVal, 16)));
        ui->SPIWriteBtn->click();
    });
    connect(ui->SPIValLengthBox, &QComboBox::activated, this, [=](int index) {
        if (index == 0) {
            SPIbitCheckBoxGroup->setBits(8);
            SPIbitCheckBoxGroup->setMaximumHeight(40);
        } else if (index == 1) {
            SPIbitCheckBoxGroup->setBits(16);
            SPIbitCheckBoxGroup->setMaximumHeight(40);
        } else {
            SPIbitCheckBoxGroup->setBits(32);
            SPIbitCheckBoxGroup->setMaximumHeight(80);
        }
        SPIbitCheckBoxGroup->setMargins(0, 0, 0, 0);
        // readSPIContent();
        SPIbitCheckBoxGroup->setValue(ui->SPIRegValueEdit->text().toUInt(nullptr, 16));
    });

    connect(btnUpDown1, &QDirectionButtonGroup::upSignal, this, [=]() {
        SPIRegAddr = SPIRegAddr + 1;
        ui->SPIRegAddrEdit->setText(QString("0x%1").arg(QString::number(SPIRegAddr, 16)));
        ui->SPIReadBtn->click();
    });
    connect(btnUpDown1, &QDirectionButtonGroup::downSignal, this, [=]() {
        if (SPIRegAddr == 0) {
            return;
        }
        SPIRegAddr = SPIRegAddr - 1;
        ui->SPIRegAddrEdit->setText(QString("0x%1").arg(QString::number(SPIRegAddr, 16)));
        ui->SPIReadBtn->click();
    });
    connect(btnUpDown2, &QDirectionButtonGroup::upSignal, this, [=]() {
        SPIRegVal = SPIRegVal + 1;
        ui->SPIRegValueEdit->setText(QString("0x%1").arg(QString::number(SPIRegVal, 16)));
        updatebitBox(SPIRegVal, 1);
        ui->SPIWriteBtn->click();
    });
    connect(btnUpDown2, &QDirectionButtonGroup::downSignal, this, [=]() {
        if (SPIRegVal == 0) {
            return;
        }
        SPIRegVal = SPIRegVal - 1;
        ui->SPIRegValueEdit->setText(QString("0x%1").arg(QString::number(SPIRegVal, 16)));
        updatebitBox(SPIRegVal, 1);
        ui->SPIWriteBtn->click();
    });

    connect(ui->spiFileDumpLengthEdit, &QLineEdit::editingFinished, this,
        [=]() { ui->spiFileDumpLengthEdit->setText(QString::number(ui->spiFileDumpLengthEdit->text().toInt())); });
    connect(ui->spiFileStartAddrEdit, &QLineEdit::editingFinished, this, [=]() {
        ui->spiFileStartAddrEdit->setText(
            QString("0x%1").arg(ui->spiFileStartAddrEdit->text().toULongLong(nullptr, 16), 8, 16, QChar('0')));
    });

    ui->spiFileDumpLengthEdit->setText("256");
    ui->spiFileStartAddrEdit->setText("0x00000000");
    ui->spiCphaCheck->setCheckState(Qt::Checked);
    ui->spiCpolCheck->setCheckState(Qt::Checked);
    ui->spiIncrementalCheck->setCheckState(Qt::Unchecked);
}

void QInterfaceDebug::initGlobalSettings()
{
    ui->comboBox->addItems(QStringList() << "SPI" << "I2C");
    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, [=](int index) {
        setUSBTypeIndex(index);
        if (index == 0) {
            ui->globalI2cSettings->setVisible(false);
            ui->globalSpiSettings->setVisible(true);
        } else {
            ui->globalI2cSettings->setVisible(true);
            ui->globalSpiSettings->setVisible(false);
        }
    });
    setUSBTypeIndex(ui->comboBox->currentIndex());
    ui->globalI2cSettings->setVisible(false);
    ui->globalSpiSettings->setVisible(true);

    // i2c
    connect(ui->globalCustomI2cSlaveEdit, &QLineEdit::editingFinished, this, [=]() {
        unsigned char slaveID = ui->globalCustomI2cSlaveEdit->text().toUInt(nullptr, 16);
        QUsbCommunication::getInstance().setI2cSlaveID(slaveID);
    });
    ui->globalI2cSlaveRadio1->setChecked(true);
    QUsbCommunication::getInstance().setI2cSlaveID(0x22);

    connect(ui->globalI2cSpeed, &QLineEdit::editingFinished, this, [=]() {
        int speed = ui->globalI2cSpeed->text().toInt();
        QUsbCommunication::getInstance().setI2cSpeed(speed);
    });
    ui->globalI2cSpeed->setText("400");
    QUsbCommunication::getInstance().setI2cSpeed(400);

    // spi
    connect(ui->globalSpiSpeedEdit, &QLineEdit::editingFinished, this, [=]() {
        float speed = ui->globalSpiSpeedEdit->text().toFloat();
        QUsbCommunication::getInstance().setSpiSpeed(speed);
    });
    ui->globalSpiSpeedEdit->setText("20");
    QUsbCommunication::getInstance().setSpiSpeed(20);
    auto spiModeFunc = [=]() {
        int mode = 0;
        if (ui->globalSpiCphaCheck->isChecked()) {
            mode |= CPHA;
        }
        if (ui->globalSpiCpolCheck->isChecked()) {
            mode |= CPOL;
        }
        QUsbCommunication::getInstance().setSpiMode(mode);
    };
    connect(ui->globalSpiCphaCheck, &QCheckBox::stateChanged, this, spiModeFunc);
    connect(ui->globalSpiCpolCheck, &QCheckBox::stateChanged, this, spiModeFunc);
    ui->globalSpiCphaCheck->setCheckState(Qt::Checked);
    ui->globalSpiCpolCheck->setCheckState(Qt::Checked);
    QUsbCommunication::getInstance().setSpiMode(CPHA | CPOL);
    connect(ui->globalSpiTimingEdit, &QLineEdit::editingFinished, this, [=]() {
        int timing = ui->globalSpiTimingEdit->text().toInt();
        QUsbCommunication::getInstance().setSpiDummyTiming(timing);
    });
    ui->globalSpiTimingEdit->setText("8");
    QUsbCommunication::getInstance().setSpiDummyTiming(8);
    connect(ui->globalSpiIncrementalCheck, &QCheckBox::stateChanged, this,
        [=](int state) { QUsbCommunication::getInstance().setSpiIncremental(state == Qt::Checked); });

    connect(this, &QInterfaceDebug::emit_proccessStart, this,[=](std::string title,int nNum) {
        progressDialog.setWindowTitle("Loading...");//对话框标题
        progressDialog.setLabelText("wait...");//对话框文本
        progressDialog.setCancelButtonText(nullptr);//禁用取消按钮
        progressDialog.setRange(0,nNum);//设置进度条范围
        progressDialog.setModal(true);//以模态方式弹出对话框
        progressDialog.autoClose();
        progressDialog.showNormal();
    });

    connect(this, &QInterfaceDebug::emit_proccessing, this,[=](std::string text,int nNum) {
        progressDialog.setLabelText(text.c_str());//对话框文本
        progressDialog.setValue(nNum);//设置进度条的值
    });

    connect(this, &QInterfaceDebug::emit_proccessEnd, this,[=]() {
        progressDialog.close();
    });

    connect(this, &QInterfaceDebug::emit_MessageBox, this,[=](std::string title,std::string text) {
        QMessageBox::warning(this, title.c_str(), text.c_str());
    });
    // connect(ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
    //             if(state > 0){
    //             ui->checkBox_2->setChecked(false);
    //             ui->checkBox_3->setChecked(false);
    //             }
    //         });
    // connect(ui->checkBox_2, &QCheckBox::stateChanged, [this](int state) {
    //             if(state > 0){
    //             ui->checkBox->setChecked(false);
    //             ui->checkBox_3->setChecked(false);
    //             }
    //         });
    // connect(ui->checkBox_3, &QCheckBox::stateChanged, [this](int state) {
    //             if(state > 0){
    //             ui->checkBox->setChecked(false);
    //             ui->checkBox_2->setChecked(false);
    //             }
    //         });

}

void QInterfaceDebug::initLoadConfig()
{
    ui->loadConfigUsbType->addItems(QStringList() << "SPI" << "I2C");
    connect(ui->loadConfigUsbType, &QComboBox::currentIndexChanged, this, [=](int index) {
        ui->loadConfigSpiWidget->setVisible(index == 0);
        ui->loadConfigI2cWidget->setVisible(index == 1);
    });
    ui->loadConfigUsbType->setCurrentIndex(1);

    ui->loadConfigI2cSpeedCombo->addItems(QStringList() << "100" << "400" << "1000");
    ui->loadConfigI2cSpeedCombo->setCurrentIndex(1);
    ui->loadConfigI2cAddrLenCombo->addItems(QStringList() << "0" << "1" << "2" << "3" << "4");
    ui->loadConfigI2cAddrLenCombo->setCurrentIndex(4);
    ui->loadConfigI2cValueLenCombo->addItems(QStringList() << "1" << "2" << "3" << "4");
    ui->loadConfigI2cValueLenCombo->setCurrentIndex(0);
    ui->loadConfigSpiValueLenCombo->addItems(QStringList() << "1" << "2" << "3" << "4");
    ui->loadConfigSpiValueLenCombo->setCurrentIndex(0);

    ui->loadConfigSlaveEdit->setText("0x22");
    connect(ui->loadConfigSlaveEdit, &QLineEdit::editingFinished, this, [=]() {
        unsigned char slaveID = ui->loadConfigSlaveEdit->text().toUInt(nullptr, 16);
        ui->loadConfigSlaveEdit->setText(QString("0x%1").arg(QString::number(slaveID, 16)));
    });
    ui->loadConfigSpiSpeedEdit->setText("10");
    connect(ui->loadConfigSpiSpeedEdit, &QLineEdit::editingFinished, this, [=]() {
        double speed = ui->loadConfigSpiSpeedEdit->text().toDouble();
        ui->loadConfigSpiSpeedEdit->setText(QString::number(speed, 'f', 2));
    });
    ui->loadConfigSpiCphaCheck->setCheckState(Qt::Checked);
    ui->loadConfigSpiCpolCheck->setCheckState(Qt::Checked);
}

void QInterfaceDebug::readI2CContent()
{
    slaveID       = ui->I2CSlaveAddrEdit->text().toUInt(nullptr, 16);
    RegAddr       = ui->I2CRegAddrEdit->text().toUInt(nullptr, 16);
    RegVal        = ui->I2CRegValueEdit->text().toUInt(nullptr, 16);
    I2CRate       = ui->I2CRateEdit->text().toInt(nullptr, 10);
    I2CAddrLength = ui->I2CAddrLengthBox->currentText().toInt();
    I2CValLength  = ui->I2CValueLengthBox->currentText().toInt();
    updateI2C();
}

void QInterfaceDebug::readSPIContent()
{
    SPIRegAddr   = ui->SPIRegAddrEdit->text().toUInt(nullptr, 16);
    SPIRegVal    = ui->SPIRegValueEdit->text().toUInt(nullptr, 16);
    SPISpeed     = ui->SPISpeedEdit->text().toInt();
    SPITiming    = ui->timingEdit->text().toInt(nullptr, 10);
    SPIValLength = ui->SPIValLengthBox->currentText().toInt();
    getSPIMode();
    updateSPI();
    ui->SPIRegAddrEdit->clearFocus();
    ui->SPIRegValueEdit->clearFocus();
    ui->SPISpeedEdit->clearFocus();
    ui->timingEdit->clearFocus();
}

void QInterfaceDebug::updateI2C()
{
    ui->I2CSlaveAddrEdit->setText(QString("0x%1").arg(QString::number(slaveID, 16)));
    ui->I2CRegAddrEdit->setText(QString("0x%1").arg(QString::number(RegAddr, 16)));
    ui->I2CRegValueEdit->setText(QString("0x%1").arg(QString::number(RegVal, 16)));
    updatebitBox(RegVal, 0);
    ui->I2CRateEdit->setText(QString::number(I2CRate));
}

void QInterfaceDebug::updateSPI()
{
    ui->SPIRegAddrEdit->setText(QString("0x%1").arg(QString::number(SPIRegAddr, 16)));
    ui->SPIRegValueEdit->setText(QString("0x%1").arg(QString::number(SPIRegVal, 16)));
    ui->SPISpeedEdit->setText(QString::number(SPISpeed));
    ui->timingEdit->setText(QString::number(SPITiming));
    updatebitBox(SPIRegVal, 1);
}

void QInterfaceDebug::setUSBTypeIndex(int index)
{
    if (index == 0) {
        QUsbCommunication::getInstance().setUsbType(USB_TYPE::USB_SPI);
    } else {
        QUsbCommunication::getInstance().setUsbType(USB_TYPE::USB_I2C);
    }
}

void QInterfaceDebug::getSPIMode()
{
    bool isCpha = ui->spiCphaCheck->isChecked();
    bool isCpol = ui->spiCpolCheck->isChecked();
    SPIMode     = 0;
    if (isCpha) {
        SPIMode |= CPHA;
    }
    if (isCpol) {
        SPIMode |= CPOL;
    }
}

void QInterfaceDebug::updatebitBox(uint32_t value, int flag)
{
    if (flag == 0) {
        // I2C
        if (RegVal <= 255) {
            // I2CbitCheckBoxGroup->setBits(8);
            // I2CbitCheckBoxGroup->setMaximumHeight(40);
        } else if (RegVal > 255 && RegVal <= 65535) {
            if (ui->I2CValueLengthBox->currentIndex() == 0) {
                I2CbitCheckBoxGroup->setBits(16);
                I2CbitCheckBoxGroup->setMaximumHeight(40);
            }
        } else if (RegVal > 65535) {
            if (ui->I2CValueLengthBox->currentIndex() < 2) {
                I2CbitCheckBoxGroup->setBits(32);
                I2CbitCheckBoxGroup->setMaximumHeight(80);
            }
        }
        I2CbitCheckBoxGroup->setValue(value);
        I2CbitCheckBoxGroup->setMargins(0, 0, 0, 0);
    }
    if (flag == 1) {
        // SPI
        if (SPIRegVal <= 255) {
            // SPIbitCheckBoxGroup->setBits(8);
            // SPIbitCheckBoxGroup->setMaximumHeight(40);
        } else if (SPIRegVal > 255 && SPIRegVal <= 65535) {
            if (ui->SPIValLengthBox->currentIndex() == 0) {
                SPIbitCheckBoxGroup->setBits(16);
                SPIbitCheckBoxGroup->setMaximumHeight(40);
            }
        } else if (SPIRegVal > 65535) {
            if (ui->SPIValLengthBox->currentIndex() < 2) {
                SPIbitCheckBoxGroup->setBits(32);
                SPIbitCheckBoxGroup->setMaximumHeight(80);
            }
        }
        SPIbitCheckBoxGroup->setValue(value);
        SPIbitCheckBoxGroup->setMargins(0, 0, 0, 0);
    }
}

void QInterfaceDebug::on_IC2ReadBtn_clicked()
{
    ui->I2CRegValueEdit->setText(QString("0x%1").arg(QString::number(0, 16)));
    readI2CContent();
    int bus=0;
    bus=ui->I2CBusComboBox->currentIndex();
    QUsbCommunication::getInstance().readI2cBurst(slaveID, RegAddr, reinterpret_cast<unsigned char *>(&RegVal),
        I2CAddrLength, I2CValLength, 0, 1, I2CRate,bus);
    updateI2C();
}

void QInterfaceDebug::on_IC2WriteBtn_clicked()
{
    readI2CContent();
    int bus=0;
    bus=ui->I2CBusComboBox->currentIndex();
    QUsbCommunication::getInstance().writeI2cBurst(slaveID, RegAddr, reinterpret_cast<unsigned char *>(&RegVal),
        I2CAddrLength, I2CValLength, 0, 1, I2CRate,bus);
}

void QInterfaceDebug::on_slaveRadio1_clicked(bool checked)
{
    if (checked) {
        ui->globalI2cSlaveRadio2->setChecked(false);
        ui->globalI2cSlaveRadio3->setChecked(false);
        ui->globalCustomI2cSlaveEdit->setEnabled(false);
        unsigned char slaveID = 0x22;
        QUsbCommunication::getInstance().setI2cSlaveID(slaveID);
        ui->globalCustomI2cSlaveEdit->setText(QString("0x%1").arg(QString::number(slaveID, 16)));
    }
}

void QInterfaceDebug::on_slaveRadio2_clicked(bool checked)
{
    if (checked) {
        ui->globalI2cSlaveRadio1->setChecked(false);
        ui->globalI2cSlaveRadio3->setChecked(false);
        ui->globalCustomI2cSlaveEdit->setEnabled(false);
        unsigned char slaveID = 0x26;
        QUsbCommunication::getInstance().setI2cSlaveID(slaveID);
        ui->globalCustomI2cSlaveEdit->setText(QString("0x%1").arg(QString::number(slaveID, 16)));
    }
}

void QInterfaceDebug::on_slaveRadio3_clicked(bool checked)
{
    if (checked) {
        ui->globalI2cSlaveRadio1->setChecked(false);
        ui->globalI2cSlaveRadio2->setChecked(false);
        ui->globalCustomI2cSlaveEdit->setEnabled(true);
        ui->globalCustomI2cSlaveEdit->clear();
    }
}
void QInterfaceDebug::on_SPIReadBtn_clicked()
{
    ui->SPIRegValueEdit->setText(QString("0x%1").arg(QString::number(0, 16)));
    readSPIContent();
    QUsbCommunication::getInstance().readSpiBurst(SPIRegAddr, reinterpret_cast<unsigned char *>(&SPIRegVal),
        SPIAddrLength, SPIValLength, SPISpeed, SPIMode, ui->spiIncrementalCheck->isChecked(), SPITiming);
    updateSPI();
}

void QInterfaceDebug::on_SPIWriteBtn_clicked()
{
    readSPIContent();
    QUsbCommunication::getInstance().writeSpiBurst(SPIRegAddr, reinterpret_cast<unsigned char *>(&SPIRegVal),
        SPIAddrLength, SPIValLength, SPISpeed, SPIMode, ui->spiIncrementalCheck->isChecked());
}

void QInterfaceDebug::on_spiCphaCheck_clicked(bool checked)
{
    getSPIMode();
}
void QInterfaceDebug::on_spiCpolCheck_clicked(bool checked)
{
    getSPIMode();
}

void QInterfaceDebug::on_loadConfigBtn_clicked()
{
    if (QUsbCommunication::getInstance().checkState() != 0) {
        QMessageBox::warning(this, "Warning", "Please open the device first!");
        return;
    }
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    QTextStream                           in(&file);
    QString                               line;
    line = in.readLine();
    file.close();
    if(line[0]=='.')
    {
        std::thread th(&QInterfaceDebug::loadBatchPartition,this,filename);
        th.detach();
        return;
    }

    // test if file is sensor config
    if (readIniString("Sensor", "SlaveID", filename, "").isEmpty()) {
        loadRegisterConfig(filename);
    } else {
        loadSensorConfig(filename);
    }
}

void QInterfaceDebug::on_i2cFileLoadBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    uint8_t         slaveId    = ui->i2cFileSlaveIdEdit->text().toUInt(nullptr, 16);
    uint32_t        addr       = ui->i2cFileStartAddrEdit->text().toUInt(nullptr, 16);
    uint8_t         addrLength = ui->i2cFileAddrLengthCombo->currentText().toInt();
    uint32_t        length     = file.size();
    int             res        = 0;
    auto            fileData   = file.readAll();
    bool            isReadBack = ui->i2cFileReadBackCheck->isChecked();
    auto            start      = std::chrono::steady_clock::now();
    QProgressDialog progress(this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progress.setWindowTitle("Loading file");
    progress.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    progress.setCancelButton(nullptr);
    progress.setMinimumDuration(0);
    progress.setRange(0, length);
    progress.setValue(0);
    progress.show();

    auto future = QtConcurrent::run([=, &res, &fileData, &progress]() {
        for (uint32_t i = 0; i < length; i += 1000) {
            res |= QUsbCommunication::getInstance().writeI2cBurst(slaveId, addr + i,
                reinterpret_cast<uint8_t *>(fileData.data() + i), addrLength, std::min(int(length - i), 1000), 0, 1,
                I2CRate);
            if (isReadBack) {
                std::vector<uint8_t> readBuffer(1000, 0);
                res |= QUsbCommunication::getInstance().readI2cBurst(slaveId, addr + i, readBuffer.data(), addrLength,
                    std::min(int(length - i), 1000), 0, 1, 100);
                if (memcmp(fileData.data() + i, readBuffer.data(), std::min(int(length - i), 1000)) != 0) {
                    QMessageBox::warning(this, "Warning", "Write i2c failed! Address: " + QString::number(addr + i));
                    return;
                }
                progress.setValue(i);
            }
        }
    });
    while (!future.isFinished()) {
        QCoreApplication::processEvents();
    }
    progress.close();

    auto                          end             = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    QMessageBox::information(this, "Info", QString("Load file success time: %1s").arg(elapsed_seconds.count()));
    ui->i2cFileLoadedSizeEdit->setText(QString::number(length));
}

void QInterfaceDebug::on_i2cFileDumpBtn_clicked()
{
    uint8_t    slaveId    = ui->i2cFileSlaveIdEdit->text().toUInt(nullptr, 16);
    uint32_t   addr       = ui->i2cFileStartAddrEdit->text().toUInt(nullptr, 16);
    uint8_t    addrLength = ui->i2cFileAddrLengthCombo->currentText().toInt();
    uint32_t   length     = ui->i2cFileDumpLengthEdit->text().toInt();
    QByteArray data;
    data.resize(length);
    int  res   = 0;
    auto start = std::chrono::steady_clock::now();
    if (slaveId != 0x40) {
        for (uint32_t i = 0; i < length; i += 1000) {
            res |= QUsbCommunication::getInstance().readI2cBurst(slaveId, addr + i,
                reinterpret_cast<uint8_t *>(data.data() + i), addrLength, std::min(int(length - i), 1000), 0, 1,
                I2CRate);
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            res |= QUsbCommunication::getInstance().readI2cBurst(slaveId, addr + i,
                reinterpret_cast<uint8_t *>(data.data() + i), addrLength, 1, 0, 1, I2CRate);
        }
    }
    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Read i2c failed!");
        return;
    }
    auto                          end             = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    QMessageBox::information(this, "Info", QString("Dump file success time: %1s").arg(elapsed_seconds.count()));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All file(*.*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    file.write(data);
    file.close();
}

void QInterfaceDebug::on_spiFileLoadBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file(*.*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    uint32_t addr       = ui->spiFileStartAddrEdit->text().toULongLong(nullptr, 16);
    uint8_t  addrLength = 4;
    uint32_t length     = file.size();
    auto res = QUsbCommunication::getInstance().writeSpiBurst(addr, reinterpret_cast<uint8_t *>(file.readAll().data()),
        addrLength, length, SPISpeed, SPIMode);
    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Write spi failed!");
    }
    ui->spiFileLoadedSizeEdit->setText(QString::number(length));
}

void QInterfaceDebug::on_spiFileDumpBtn_clicked()
{
    uint32_t   addr       = ui->spiFileStartAddrEdit->text().toULongLong(nullptr, 16);
    uint8_t    addrLength = 4;
    uint32_t   length     = ui->spiFileDumpLengthEdit->text().toInt();
    QByteArray data;
    data.resize(length);
    auto res = QUsbCommunication::getInstance().readSpiBurst(addr, reinterpret_cast<uint8_t *>(data.data()), addrLength,
        length, SPISpeed, SPIMode, SPITiming);
    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Read spi failed!");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All file(*.*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    file.write(data);
    file.close();
}

void QInterfaceDebug::onFpgaToSensorBtnClicked()
{
    uint32_t tmpValue;
    QUsbCommunication::getInstance().readFpgaRegister(0x80f2, tmpValue);
    tmpValue |= 0x01;
    QUsbCommunication::getInstance().writeFpgaRegister(0x80f2, tmpValue);
}

void QInterfaceDebug::onFpgaToM1BtnClicked()
{
    uint32_t tmpValue;
    QUsbCommunication::getInstance().readFpgaRegister(0x80f2, tmpValue);
    tmpValue &= 0xfe;
    QUsbCommunication::getInstance().writeFpgaRegister(0x80f2, tmpValue);
}

void QInterfaceDebug::loadSensorConfig(QString filename)
{
    uint8_t  slaveID = static_cast<uint8_t>(readIniData("Sensor", "SlaveID", filename, 0)) >> 1;
    uint8_t  i2cMode = static_cast<uint8_t>(readIniData("Sensor", "mode", filename, 3));
    uint32_t addrLen = 2, valueLen = 1;
    switch (i2cMode) {
    case 0:
    case 1:
        addrLen  = 1;
        valueLen = 1;
        break;
    case 2:
        addrLen  = 1;
        valueLen = 2;
        break;
    case 3:
    default:
        addrLen  = 2;
        valueLen = 1;
        break;
    case 4:
        addrLen  = 2;
        valueLen = 2;
        break;
    }
    uint16_t width        = static_cast<uint16_t>(readIniData("Sensor", "width", filename, 0));
    uint16_t height       = static_cast<uint16_t>(readIniData("Sensor", "height", filename, 0));
    uint16_t sensorWidth  = static_cast<uint16_t>(readIniData("Sensor", "senWidth", filename, width));
    uint16_t sensorHeight = static_cast<uint16_t>(readIniData("Sensor", "senHeight", filename, height));
    uint16_t outType      = static_cast<uint16_t>(readIniData("Sensor", "type", filename, 0));
    uint16_t sensorType   = static_cast<uint16_t>(readIniData("Sensor", "senType", filename, outType));
    uint16_t i2cRate      = static_cast<uint16_t>(readIniData("Sensor", "i2cRate", filename, 400));
    uint16_t hdrMode      = static_cast<uint16_t>(readIniData("Sensor", "hdrMode", filename, 0));
    uint16_t port         = static_cast<uint16_t>(readIniData("Sensor", "port", filename, 1));
    uint16_t lane         = static_cast<uint16_t>(readIniData("Sensor", "lane", filename, 4));

    // set fpga
    int res = 0;
    // hdr mode
    if ((hdrMode & 0xf) == 0x2) {
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8027, 0x02);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x802a, 0x04);
        height *= 2;
        sensorHeight *= 2;
    } else if ((hdrMode & 0xf) == 0x3) {
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8027, 0x02);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x802a, 0x24);
        height *= 3;
        sensorHeight *= 3;
    } else if ((hdrMode & 0xf) == 0x4) {
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8027, 0x04);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x802a, 0xe4);
        height *= 4;
        sensorHeight *= 4;
    } else {
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8027, 0x00);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x802a, 0x00);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // port lane
    int fpgaPort = 0;
    switch (port) {
    case 0:
        fpgaPort = 1;
        break;
    case 1:
        fpgaPort = 0;
        break;
    default:
        fpgaPort = 2;
        break;
    }
    int fpgaLane = 0;
    switch (lane) {
    case 1:
        fpgaLane = 0;
        break;
    case 2:
        fpgaLane = 16;
        break;
    case 4:
        fpgaLane = 32;
        break;
    case 5:
        fpgaLane = 64;
        break;
    case 6:
        fpgaLane = 128;
    default:
        fpgaLane = 48;
        break;
    }
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8001, fpgaPort & 0x3);
    if (fpgaPort != 0) // LVDS:0x8002 bit[5:4]; MIPI:1c4d x2 mode 1/2 0x8002 bit[7:6]
    {
        uint32_t value = 0;
        res |= QUsbCommunication::getInstance().readFpgaRegister(0x8002, value);
        value &= ~(0xf0);
        value |= (uint8_t)(fpgaLane & 0x000000f0);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8002, value);
    } else if (fpgaPort) // DVP
    {
        uint32_t value = 0;
        res |= QUsbCommunication::getInstance().readFpgaRegister(0x8001, value);
        value &= 0x9f;
        value |= (uint8_t)((fpgaLane & 0x03) << 5);
        res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8001, value);
    }

    // bit width
    auto getFpgaBitWidth = [](uint16_t bitWidth) {
        switch (bitWidth) {
        case 0x01:
        case 0x0d:
        case 0x0e:
            return 0;
        case 0x06:
        case 0x00:
            return 1;
        case 0x07:
            return 2;
        case 0x09:
            return 4;
        case 0x03:
            return 5;
        case 0x02:
        case 0x08:
            return 7;
        case 0x0c:
            return 12;
        case 0x0f:
            return 13;
        case 0x11:
            return 15;
        case 0x0a:
            return 6;
        case 0x10:
            return 14;
        default:
            return 2;
        }
    };
    uint16_t fpgaBitWidth = getFpgaBitWidth(outType), fpgaSensorBitWidth = getFpgaBitWidth(sensorType);
    uint32_t tmpValue = 0;
    res |= QUsbCommunication::getInstance().readFpgaRegister(0x8008, tmpValue);
    tmpValue &= ~(0x0f);
    tmpValue |= (uint8_t)(fpgaSensorBitWidth & 0x0000000f);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8008, tmpValue);
    res |= QUsbCommunication::getInstance().readFpgaRegister(0x8002, tmpValue);
    if (fpgaBitWidth == 13) {
        tmpValue &= ~(0x0f);
        tmpValue |= (uint8_t)(0x0c & 0x0000000f);
    } else {
        tmpValue &= ~(0x0f);
        tmpValue |= (uint8_t)(fpgaBitWidth & 0x0000000f);
    }
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8002, tmpValue);

    // resolution
    tmpValue = (uint8_t)((sensorWidth & 0x0000FF00) >> 8);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8003, tmpValue);
    tmpValue = (uint8_t)(sensorWidth & 0x000000FF);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8004, tmpValue);
    tmpValue = (uint8_t)((sensorHeight & 0x0000FF00) >> 8);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8005, tmpValue);
    tmpValue = (uint8_t)(sensorHeight & 0x000000FF);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8006, tmpValue);
    tmpValue = (uint8_t)((width & 0x0000FF00) >> 8);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8030, tmpValue);
    tmpValue = (uint8_t)(width & 0x000000FF);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8031, tmpValue);
    tmpValue = (uint8_t)((height & 0x0000FF00) >> 8);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8032, tmpValue);
    tmpValue = (uint8_t)(height & 0x000000FF);
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8033, tmpValue);

    // switch fpga reset
    res |= QUsbCommunication::getInstance().readFpgaRegister(0x8000, tmpValue);
    tmpValue &= ~0x01;
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8000, tmpValue);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    tmpValue |= 0x01;
    res |= QUsbCommunication::getInstance().writeFpgaRegister(0x8000, tmpValue);

    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Fpga configuraion failed!");
    }

    // set sensor register
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    QTextStream                           in(&file);
    bool                                  start = false;
    std::vector<LoadConfigRegisterStruct> loadRegisters;

    auto tmpSlaveId  = slaveID;
    auto tmpAddrLen  = addrLen;
    auto tmpValueLen = valueLen;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!start && line.toUpper().trimmed() == "[PARALIST]") {
            start = true;
            continue;
        }
        if (!start) {
            continue;
        }
        if (start && line.toUpper().trimmed().startsWith("[")) {
            break;
        }
        line              = line.trimmed();
        auto loadRegister = generateLoadConfigRegisterStruct(line);
        if (loadRegister.addrLen == 0 && loadRegister.valueLen == 0) {
            continue;
        }
        if (loadRegister.addr == 0xeeee) {
            if (loadRegister.data.size() >= 2 && *reinterpret_cast<uint16_t *>(loadRegister.data.data()) == 0xeeee) {
                tmpSlaveId  = slaveID;
                tmpAddrLen  = addrLen;
                tmpValueLen = valueLen;
                continue;
            }

            while (!in.atEnd()) {
                line                 = in.readLine();
                auto tmpLoadRegister = generateLoadConfigRegisterStruct(line);
                if (loadRegister.addrLen == 0 && loadRegister.valueLen == 0) {
                    continue;
                }
                if (tmpLoadRegister.addr != 0xeeef && tmpLoadRegister.addr != 0) {
                    QMessageBox::warning(this, "Warning", "Sensor configuraion failed!");
                    return;
                }
                tmpSlaveId      = loadRegister.data[0] >> 1;
                auto tmpI2cMode = tmpLoadRegister.data[0] & 0xf;
                switch (tmpI2cMode) {
                case 0:
                case 1:
                    tmpAddrLen  = 1;
                    tmpValueLen = 1;
                    break;
                case 2:
                    tmpAddrLen  = 1;
                    tmpValueLen = 2;
                    break;
                case 3:
                default:
                    tmpAddrLen  = 2;
                    tmpValueLen = 1;
                    break;
                case 4:
                    tmpAddrLen  = 2;
                    tmpValueLen = 2;
                    break;
                }
                break;
            }
            continue;
        }
        if (loadRegister.addr == 0xffff) {
            loadRegisters[loadRegisters.size() - 1].sleepMs = 0;
            for (size_t idx = 0; idx < loadRegister.data.size(); idx++) {
                loadRegisters[loadRegisters.size() - 1].sleepMs += loadRegister.data[idx] << (idx * 8);
            }
            continue;
        }
        loadRegister.slaveId  = tmpSlaveId;
        loadRegister.speed    = i2cRate;
        loadRegister.addrLen  = tmpAddrLen;
        loadRegister.valueLen = tmpValueLen;
        auto tmpData          = loadRegister.data;
        loadRegister.data.resize(valueLen);
        for (uint32_t i = 0; i < valueLen; i++) {
            loadRegister.data[i] = tmpData[i];
        }
        loadRegisters.push_back(loadRegister);
    }

    for (auto &loadRegister : loadRegisters) {
        res |= QUsbCommunication::getInstance().writeI2cBurst(loadRegister.slaveId, loadRegister.addr,
            loadRegister.data.data(), loadRegister.addrLen, static_cast<int>(loadRegister.data.size()),
            loadRegister.sleepMs, 1, loadRegister.speed);
        std::this_thread::sleep_for(std::chrono::milliseconds(loadRegister.sleepMs));
    }
    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Sensor configuraion failed!");
    } else {
        QMessageBox::information(this, "Information", "Sensor configuraion success!");
    }
}

void QInterfaceDebug::loadRegisterConfig(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    QTextStream                           in(&file);
    QString                               line;
    bool                                  isI2c = ui->loadConfigUsbType->currentIndex() == 1;
    std::vector<LoadConfigRegisterStruct> loadRegisters;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.isEmpty()) {
            continue;
        }
        auto loadRegister = generateLoadConfigRegisterStruct(line);
        if (loadRegister.addr == 0 && loadRegister.data.size() == 0) {
            continue;
        }
        loadRegister.isIncremental = ui->loadConfigSpiIncrementalCheck->isChecked();
        if (loadRegister.data.size() > loadRegister.valueLen) {
            loadRegister.data.resize(loadRegister.valueLen);
        }
        loadRegisters.push_back(loadRegister);
    }
    if (ui->preferBurstCheck->isChecked()) {
        std::vector<LoadConfigRegisterStruct> loadRegistersBurst;
        loadRegistersBurst.push_back(loadRegisters[0]);
        for (int i = 1; i < loadRegisters.size(); i++) {
            if (loadRegisters[i].slaveId == loadRegistersBurst.back().slaveId
                && loadRegisters[i].addr == loadRegistersBurst.back().addr + loadRegistersBurst.back().data.size()) {
                loadRegistersBurst.back().data.insert(loadRegistersBurst.back().data.end(),
                    loadRegisters[i].data.begin(), loadRegisters[i].data.end());
            } else {
                loadRegistersBurst.push_back(loadRegisters[i]);
            }
        }
        loadRegisters = loadRegistersBurst;
    }
    QFile outputFile("output.txt");
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    QTextStream out(&outputFile);
    int         res = 0;
    for (auto &loadRegister : loadRegisters) {
        for (int off = 0; off < loadRegister.data.size(); off += 1) {
            out << QString("%1 %2 %3\n")
                       .arg(QString::number(loadRegister.slaveId, 16))
                       .arg(QString::number(loadRegister.addr + off, 16))
                       .arg(QString::number(loadRegister.data[off], 16));
        }
        if (isI2c) {
            res |= QUsbCommunication::getInstance().writeI2cBurst(loadRegister.slaveId, loadRegister.addr,
                loadRegister.data.data(), loadRegister.addrLen, loadRegister.valueLen, loadRegister.sleepMs,
                loadRegister.sleepMs, loadRegister.speed);
        } else {
            res |= QUsbCommunication::getInstance().writeSpiBurst(loadRegister.addr, loadRegister.data.data(),
                loadRegister.addrLen, loadRegister.valueLen, loadRegister.speed, loadRegister.spiMode);
        }
    }
    if (res != 0) {
        QMessageBox::warning(this, "Warning", "Load config failed!");
    } else {
        QMessageBox::information(this, "Information", "Load config success!");
    }
}

void QInterfaceDebug::loadBatchPartition(QString filename)
{
    int nFileNum=0;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Open file failed!");
        return;
    }
    QTextStream                           in(&file);
    QString                               line;
    QString strPath=filename;
    strPath=strPath.left(strPath.lastIndexOf("/"));
    while (!in.atEnd())
    {
        line = strPath+in.readLine();
        QFile filePartition(line);
        if (!filePartition.open(QIODevice::ReadOnly)) {
            continue;
        }
        filePartition.close();
        nFileNum++;
    }
    emit emit_proccessStart("Loading...",nFileNum);
    QCoreApplication::processEvents();

    in.seek(0);
    int nCount=1;
    int nRet=0;
    while (!in.atEnd()) {
        line = strPath+in.readLine();
        QFile filePartition(line);
        if (!filePartition.open(QIODevice::ReadOnly)) {
            continue;
        }
        filePartition.close();

        QString strTips=line.right(line.length()-line.lastIndexOf('\\')-1);
        emit emit_proccessing(strTips.toStdString(),nCount++);
        QCoreApplication::processEvents();
        QString strSlaveID = readIniString("setting header", "iic_slv_id", line, "00");
        QString strImgtype = readIniString("FPGA_Config", "imgtype", line, "");
        if(strImgtype != "")
        {
            nRet=fpgaBatchPartition(line);
            if(nRet!=0)
            {
                break;
            }
        }
        else if(strSlaveID=="00")
        {
            QString strKey = readIniString("0", "setting_type", line, "general");
            QString strMoveKey = readIniString("movedata header", "movedatataraddr", line, "00");
            if(strMoveKey!="00")
            {
                nRet=movedataBatchPartition(line);
                if(nRet!=0)
                {
                    break;
                }
                continue;
            }

            if(strKey=="general")
            {
                nRet=ddrBatchPartition(line);
                if(nRet!=0)
                {
                    break;
                }
            }
            else
            {
                nRet=settingBatchPartition(line);
                if(nRet!=0)
                {
                    break;
                }
            }
        }
        else
        {
            nRet=sensorBatchPartition(line);
            if(nRet!=0)
            {
                break;
            }
        }
    }
    file.close();

    emit emit_proccessEnd();
    if(nRet==0)
    {
        emit emit_MessageBox("Success", "Load configuraion Success!");
    }
}

int QInterfaceDebug::settingBatchPartition(QString filename)
{
    int nSectionNum = readIniString("setting header", "cmdnum", filename, "0").toInt(nullptr,10);
    bool           isI2c                 = ui->loadConfigUsbType->currentIndex() == 1;
    const uint32_t defaultI2cSpeed       = ui->loadConfigI2cSpeedCombo->currentText().toInt();
    const uint32_t defaultSpiSpeed       = ui->loadConfigSpiSpeedEdit->text().toInt();
    const uint8_t  defaultSpiMode =
        (ui->loadConfigSpiCphaCheck->isChecked() ? CPHA : 0) | (ui->loadConfigSpiCpolCheck->isChecked() ? CPOL : 0);

    QFile filePartition(filename);
    if (!filePartition.open(QIODevice::ReadOnly)) {
        return -1;
    }
    std::vector<LoadConfigRegisterStruct> vecRegisterData;
    QTextStream                           in(&filePartition);
    std::string                               line;
    int nNowSectionIndex=0;
    while ((!in.atEnd()) && (nNowSectionIndex<nSectionNum)) {
        line = in.readLine().toStdString();
        if (line == "")
        {
            continue;
        }
        if (line[0] == '/' || line[0] == ';')
        {
            continue;
        }
        std::string strKey, strValue, strTemp;
        int nLeftPos, nRightPos;
        nLeftPos = line.find_first_of('[');
        nRightPos=line.find_first_of(']');
        strTemp = line.substr(nLeftPos + 1, nRightPos - nLeftPos - 1);
        qint64 nPos;
        if (strTemp == std::to_string(nNowSectionIndex))
        {
            nPos = in.pos();
            while (!in.atEnd())
            {
                line = in.readLine().toStdString();
                nPos = in.pos();
                if (line == "")
                {
                    continue;
                }
                if (line[0] == '[')
                {
                    break;
                }
                if (line[0] == '/' || line[0] == ';')
                {
                    continue;
                }
                if (line[0] <'0' || line[0]>'9')
                {
                    continue;
                }

                LoadConfigRegisterStruct registerData;
                registerData.spiMode = isI2c ? 0 : defaultSpiMode;
                registerData.speed = isI2c ? defaultI2cSpeed : defaultSpiSpeed;
                registerData.valueLen=1;
                registerData.sleepMs=0;

                std::stringstream sstrStream(line.c_str());
                getline(sstrStream, strTemp, ' ');
                registerData.slaveId=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.addr=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                uint32_t uValue=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.addrLen=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.valueLen=stoull(strTemp, 0, 16);
                registerData.data.resize(registerData.valueLen);
                memcpy(registerData.data.data(),&uValue,registerData.valueLen);

                vecRegisterData.push_back(registerData);
            }
            in.seek(nPos);
            nNowSectionIndex++;
        }
    }
    filePartition.close();

    int res=0;
    for (auto &loadRegister : vecRegisterData) {
        res |= QUsbCommunication::getInstance().writeI2cBurst(loadRegister.slaveId, loadRegister.addr,
            loadRegister.data.data(), loadRegister.addrLen, static_cast<int>(loadRegister.data.size()),
            loadRegister.sleepMs, 1, loadRegister.speed);
        std::this_thread::sleep_for(std::chrono::milliseconds(loadRegister.sleepMs));
    }
    if (res != 0) {
        emit emit_MessageBox("Warning", "Setting configuraion failed!");
    }
    return res;
}

int QInterfaceDebug::ddrBatchPartition(QString filename)
{
    int nSectionNum = readIniString("setting header", "cmdnum", filename, "0").toInt(nullptr,10);
    const uint8_t  defaultI2cSlaveId     = ui->loadConfigSlaveEdit->text().toUInt(nullptr, 16);
    bool           isI2c                 = ui->loadConfigUsbType->currentIndex() == 1;
    const uint32_t defaultI2cSpeed       = ui->loadConfigI2cSpeedCombo->currentText().toInt();
    const uint32_t defaultSpiSpeed       = ui->loadConfigSpiSpeedEdit->text().toInt();
    const uint8_t  defaultSpiMode =
        (ui->loadConfigSpiCphaCheck->isChecked() ? CPHA : 0) | (ui->loadConfigSpiCpolCheck->isChecked() ? CPOL : 0);

    QFile filePartition(filename);
    if (!filePartition.open(QIODevice::ReadOnly)) {
        return -1;
    }
    std::vector<LoadConfigRegisterStruct> vecRegisterData;
    QTextStream                           in(&filePartition);
    std::string                               line;
    int nNowSectionIndex=0;
    while ((!in.atEnd()) && (nNowSectionIndex<nSectionNum)) {
        line = in.readLine().toStdString();
        if (line == "")
        {
            continue;
        }
        if (line[0] == '/' || line[0] == ';')
        {
            continue;
        }
        std::string strKey, strValue, strTemp;
        int nLeftPos, nRightPos;
        nLeftPos = line.find_first_of('[');
        nRightPos=line.find_first_of(']');
        strTemp = line.substr(nLeftPos + 1, nRightPos - nLeftPos - 1);
        qint64 nPos;
        if (strTemp == std::to_string(nNowSectionIndex))
        {
            nPos = in.pos();
            while (!in.atEnd())
            {
                line = in.readLine().toStdString();
                nPos = in.pos();
                if (line == "")
                {
                    continue;
                }
                if (line[0] == '[')
                {
                    break;
                }
                if (line[0] == '/' || line[0] == ';')
                {
                    continue;
                }
                if (line[0] <'0' || line[0]>'9')
                {
                    continue;
                }

                LoadConfigRegisterStruct registerData;
                registerData.slaveId=defaultI2cSlaveId;
                registerData.spiMode = isI2c ? 0 : defaultSpiMode;
                registerData.speed = isI2c ? defaultI2cSpeed : defaultSpiSpeed;
                registerData.valueLen=1;
                registerData.sleepMs=0;
                registerData.addrLen=4;
                registerData.valueLen=4;

                std::stringstream sstrStream(line.c_str());
                getline(sstrStream, strTemp, ' ');
                registerData.addr=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                uint32_t uValue=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.isRead = stoull(strTemp, 0, 16)==1;
                getline(sstrStream, strTemp, ' ');
                registerData.mask = stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.sleepMs = stoull(strTemp, 0, 16);
                uValue=uValue&registerData.mask;
                registerData.data.resize(registerData.valueLen);
                memcpy(registerData.data.data(),&uValue,registerData.valueLen);
                /*if(isI2c)
                {
                    //大小端取反
                    int nLen=registerData.data.size()-1;
                    std::vector<uint8_t> reverseData;
                    while (nLen>=0) {
                        reverseData.push_back(registerData.data[nLen]);
                        nLen--;
                    }
                    registerData.data.swap(reverseData);
                }*/

                vecRegisterData.push_back(registerData);
            }
            in.seek(nPos);
            nNowSectionIndex++;
        }
    }
    filePartition.close();

    int res=0;
    for (auto &loadRegister : vecRegisterData) {
        if(loadRegister.isRead)
        {
            uint32_t uValueRight;
            memcpy(&uValueRight,loadRegister.data.data(),loadRegister.valueLen);
            uValueRight=uValueRight&loadRegister.mask;
            uint32_t uValueTemp=0;
            QDateTime startTime = QDateTime::currentDateTime();
            while (true) {
                res |= QUsbCommunication::getInstance().readI2cBurst(loadRegister.slaveId, loadRegister.addr,
                    (uchar*)&uValueTemp, loadRegister.addrLen, static_cast<int>(loadRegister.data.size()),
                    0, 1, loadRegister.speed);
                uValueTemp=uValueTemp & loadRegister.mask;
                std::this_thread::sleep_for(std::chrono::microseconds(50));
                if((startTime.msecsTo(QDateTime::currentDateTime())>loadRegister.sleepMs)||(uValueTemp==uValueRight))
                {
                    break;
                }
            }
            if((res != 0)||(uValueTemp!=uValueRight))
            {
                emit emit_MessageBox("Warning", "DDR configuraion failed!");
                return -1;
            }
        }
        else
        {
            res |= QUsbCommunication::getInstance().writeI2cBurst(loadRegister.slaveId, loadRegister.addr,
                loadRegister.data.data(), loadRegister.addrLen, static_cast<int>(loadRegister.data.size()),
                loadRegister.sleepMs, 1, loadRegister.speed);
            std::this_thread::sleep_for(std::chrono::microseconds(loadRegister.sleepMs));
        }
    }
    if (res != 0) {
        emit emit_MessageBox("Warning", "DDR configuraion failed!");
    }
    return res;
}

int QInterfaceDebug::sensorBatchPartition(QString filename)
{
    uint8_t uSlaveID = readIniString("setting header", "iic_slv_id", filename, "00").toInt(nullptr,16);
    uint8_t uAddrLen = readIniString("setting header", "iic_addr_len", filename, "0").toInt(nullptr,16);
    int nSectionNum = readIniString("setting header", "cmdnum", filename, "0").toInt(nullptr,10);
    bool           isI2c                 = ui->loadConfigUsbType->currentIndex() == 1;
    const uint32_t defaultI2cSpeed       = ui->loadConfigI2cSpeedCombo->currentText().toInt();
    const uint32_t defaultSpiSpeed       = ui->loadConfigSpiSpeedEdit->text().toInt();
    const uint8_t  defaultSpiMode =
        (ui->loadConfigSpiCphaCheck->isChecked() ? CPHA : 0) | (ui->loadConfigSpiCpolCheck->isChecked() ? CPOL : 0);

    QFile filePartition(filename);
    if (!filePartition.open(QIODevice::ReadOnly)) {
        return -1;
    }
    std::vector<LoadConfigRegisterStruct> vecRegisterData;
    QTextStream                           in(&filePartition);
    std::string                               line;
    int nNowSectionIndex=0;
    while ((!in.atEnd()) && (nNowSectionIndex<nSectionNum)) {
        line = in.readLine().toStdString();
        if (line == "")
        {
            continue;
        }
        if (line[0] == '/' || line[0] == ';')
        {
            continue;
        }
        std::string strKey, strValue, strTemp;
        int nLeftPos, nRightPos;
        nLeftPos = line.find_first_of('[');
        nRightPos=line.find_first_of(']');
        strTemp = line.substr(nLeftPos + 1, nRightPos - nLeftPos - 1);
        qint64 nPos;
        if (strTemp == std::to_string(nNowSectionIndex))
        {
            nPos = in.pos();
            while (!in.atEnd())
            {
                line = in.readLine().toStdString();
                nPos = in.pos();
                if (line == "")
                {
                    continue;
                }
                if (line[0] == '[')
                {
                    break;
                }
                if (line[0] == '/' || line[0] == ';')
                {
                    continue;
                }
                if (line[0] <'0' || line[0]>'9')
                {
                    continue;
                }

                LoadConfigRegisterStruct registerData;
                registerData.slaveId=uSlaveID;
                registerData.spiMode = isI2c ? 0 : defaultSpiMode;
                registerData.speed = isI2c ? defaultI2cSpeed : defaultSpiSpeed;
                registerData.addrLen=uAddrLen;
                registerData.valueLen=1;
                registerData.sleepMs=0;

                std::stringstream sstrStream(line.c_str());
                getline(sstrStream, strTemp, ' ');
                registerData.addr=stoull(strTemp, 0, 16);
                getline(sstrStream, strTemp, ' ');
                registerData.data.push_back(stoull(strTemp, 0, 16));

                vecRegisterData.push_back(registerData);
            }
            in.seek(nPos);
            nNowSectionIndex++;
        }
    }
    filePartition.close();

    int res=0;
    for (auto &loadRegister : vecRegisterData) {
        res |= iWriteSensorByI2CMaster(0, loadRegister.slaveId, loadRegister.addr,
            loadRegister.data[0], 0,0, loadRegister.addrLen, static_cast<int>(loadRegister.data.size()),
            0, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(loadRegister.sleepMs));
    }
    if (res != 0) {
        emit emit_MessageBox("Warning", "Sensor configuraion failed!");
    }
    return res;
}

int QInterfaceDebug::movedataBatchPartition(QString filename)
{
    const uint8_t  defaultI2cSlaveId     = ui->loadConfigSlaveEdit->text().toUInt(nullptr, 16);
    const uint32_t defaultSpiSpeed       = ui->loadConfigSpiSpeedEdit->text().toInt();
    bool           isI2c                 = ui->loadConfigUsbType->currentIndex() == 1;
    const uint8_t  defaultSpiMode =
        (ui->loadConfigSpiCphaCheck->isChecked() ? CPHA : 0) | (ui->loadConfigSpiCpolCheck->isChecked() ? CPOL : 0);

    std::string strAddr=readIniString("movedata header", "movedatataraddr", filename, "00").toStdString();
    uint32_t uAddr = stoull(strAddr, 0, 16);
    QString strPath=filename;
    strPath=strPath.left(strPath.lastIndexOf(".\\"));
    QString biName = strPath + "/"+readIniString("movedata header", "movedata", filename, "");
    QFile filePartition(biName);
    if (!filePartition.open(QIODevice::ReadOnly)) {
        return -1;
    }
    QByteArray by;
    by = filePartition.readAll();
    std::vector<uchar> vecBinData(filePartition.size());
    memcpy(vecBinData.data(),by.data(),filePartition.size());
    filePartition.close();

    int res=0;
    if(isI2c)
    {
        res    = QUsbCommunication::getInstance().writeI2cBurst(defaultI2cSlaveId, uAddr,
            vecBinData.data(), 4, static_cast<int>(vecBinData.size()),
            1, 1, defaultSpiSpeed);
    }
    else
    {
        res |= QUsbCommunication::getInstance().writeSpiBurst(uAddr, vecBinData.data(),
            4, static_cast<int>(vecBinData.size()), defaultSpiSpeed, defaultSpiMode);
    }
    if (res != 0) {
        emit emit_MessageBox("Warning", "Move Data configuraion failed!");
    }
    return res;
}

int QInterfaceDebug::fpgaBatchPartition(QString filename)
{
    uint32_t uSalveID=readIniString("FPGA_Config", "Slave_id", filename, "").toUInt(nullptr,16);
    std::string strImageType=readIniString("FPGA_Config", "imgtype", filename, "").toStdString();
    int nWidth=readIniString("FPGA_Config", "width", filename, "").toUInt(nullptr,10);
    int nHeight=readIniString("FPGA_Config", "height", filename, "").toUInt(nullptr,10);
    int nOutVCNum=readIniString("FPGA_Config", "out_vc_num", filename, "").toUInt(nullptr,10);
    int nSnr=readIniString("FPGA_Config", "online_change_snr", filename, "").toUInt(nullptr,10);

    int nRet=0;
    uchar uVal=0x2e;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8000, &uVal, 2,1);
    uVal=0x2f;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8000, &uVal, 2,1);
    uVal=0x01;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8001, &uVal, 2,1);
    if (strImageType == "raw8")
    {
        uVal=0x20;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8002, &uVal, 2,1);
        uVal=0x20;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fb, &uVal, 2,1);
    }
    else if( strImageType == "raw10")
    {
        uVal=0x21;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8002, &uVal, 2,1);
        uVal=0x21;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fb, &uVal, 2,1);
    }
    else if( strImageType == "raw12")
    {
        uVal=0x22;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8002, &uVal, 2,1);
        uVal=0x22;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fb, &uVal, 2,1);
    }
    else if( strImageType == "yuv8")
    {
        uVal=0x27;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8002, &uVal, 2,1);
        uVal=0x27;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fb, &uVal, 2,1);
    }
    else if( strImageType == "yuv10")
    {
        uVal=0x2c;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8002, &uVal, 2,1);
        uVal=0x2c;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fb, &uVal, 2,1);
    }
    uVal=(nWidth>>8)&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8003, &uVal, 2,1);
    uVal=nWidth&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8004, &uVal, 2,1);
    if( nOutVCNum == 2)
    {
        nHeight = nHeight * 2;
        uVal=0x04;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x802a, &uVal, 2,1);
        uVal=0x01;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8027, &uVal, 2,1);
        uVal=0x03;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8101, &uVal, 2,1);
    }
    uVal=(nHeight>>8)&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8005, &uVal, 2,1);
    uVal= nHeight&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8006, &uVal, 2,1);
    uVal=(nWidth>>8)&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fc, &uVal, 2,1);
    uVal=nWidth&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fd, &uVal, 2,1);
    uVal=(nHeight>>8)&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80fe, &uVal, 2,1);
    uVal=nHeight&0xff;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80ff, &uVal, 2,1);
    uVal=0x10;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8025, &uVal, 2,1);
    uVal=0x01;
    nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x8029, &uVal, 2,1);
    if( nSnr == 0)
    {
        uVal=0x01;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80f2, &uVal, 2,1);
        uVal=0x02;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80f4, &uVal, 2,1);
        uVal=0x03;
        nRet |= QUsbCommunication::getInstance().writeI2cBurst(uSalveID, 0x80f2, &uVal, 2,1);
        //iWriteRegAHB_nocheck(0x31000103, 0x01, False);
    }
    if(nRet!=0)
    {
        emit emit_MessageBox("Warning", "FPGA configuraion failed!");
    }
    return nRet;
}

bool QInterfaceDebug::bLoadIICMasterConfig()
{
    vMasterStartSlave.clear();
    vMasterStartValueLen.clear();
    vMasterStartAddrLen.clear();
    vMasterStartRegList.clear();
    vMasterStartValList.clear();
    vMasterStartBitMask.clear();
    mMasterStartSize=0;

    vMasterEndSlave.clear();
    vMasterEndValueLen.clear();
    vMasterEndAddrLen.clear();
    vMasterEndRegList.clear();
    vMasterEndValList.clear();
    vMasterEndBitMask.clear();
    mMasterEndSize = 0;
    // 解析I2C配置文件
    QString sPath = QCoreApplication::applicationDirPath();
    sPath += "/IICMaster.dll";
    QFile file(sPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QString szLine;
    bool bIsParaStart = false;
    uint32_t slave=0, reg = 0, value = 0, bitMask = 0, addrLength = 0, valueLength = 0;
    int tmp = 0;
    int state = -1; // 用于标识当前解析的段落
    while (!file.atEnd())
    {
        szLine=file.readLine();
        szLine.replace("，", ",");
        szLine.replace("；", ";");
        szLine.replace(" ", "");
        // 跳过空行和注释行
        if (szLine.isEmpty() || szLine.left(1) == "," || szLine.left(1) == ";")
            continue;
        QString Textout;
        tmp = szLine.indexOf("//");
        if (tmp == 0)
        {
            continue;
        }
        else if (tmp > 0)
        {
            szLine = szLine.left(tmp);
        }
        tmp = szLine.indexOf("]");
        if (tmp == 0)
        {
            continue;
        }
        szLine.remove('\n');
        szLine=szLine.toLower();
        // 处理 [Start] 部分
        if (szLine == "[start]")
        {
            state = 0;  // 标识开始解析 [Start] 部分
            bIsParaStart = true;
            continue;
        }
        if (szLine == "[end]")
        {
            state = 1;  // 标识开始解析 [End] 部分
            bIsParaStart = true;
            continue;
        }
        if (bIsParaStart)
        {
            szLine=szLine.trimmed();
            if(szLine=="")
            {
                continue;
            }

                   // 解析具体的数值
            QString sSlave, sReg, sVal, sBitMask, sAddrLength, sValueLength;
            QStringList strList = szLine.split(",");
            sSlave=strList.at(0);   // 获取从设备地址
            sReg=strList.at(1);   // 获取寄存器地址
            sVal=strList.at(2);  // 获取寄存器值
            sBitMask=strList.at(3);  // 获取位掩码
            sAddrLength=strList.at(4);  // 获取地址长度
            sValueLength=strList.at(5); // 获取值长度

            sSlave=sSlave.trimmed();
            sReg=sReg.trimmed();
            sVal=sVal.trimmed();
            sBitMask=sBitMask.trimmed();
            sAddrLength=sAddrLength.trimmed();
            sValueLength=sValueLength.trimmed();

            slave=sSlave.toInt(nullptr,16);
            reg=sReg.toInt(nullptr,16);
            value=sVal.toInt(nullptr,16);
            bitMask=sBitMask.toInt(nullptr,16);
            addrLength=sAddrLength.toInt(nullptr,16);
            valueLength=sValueLength.toInt(nullptr,16);

                   // 根据状态决定存入的目标向量
            if (state == 0) // [Start] 部分
            {
                vMasterStartSlave.push_back(slave);
                vMasterStartValueLen.push_back(valueLength);
                vMasterStartAddrLen.push_back(addrLength);
                vMasterStartRegList.push_back(reg);
                vMasterStartValList.push_back(value);
                vMasterStartBitMask.push_back(bitMask);
                mMasterStartSize++;
            }
            else if (state == 1) // [End] 部分
            {
                vMasterEndSlave.push_back(slave);
                vMasterEndValueLen.push_back(valueLength);
                vMasterEndAddrLen.push_back(addrLength);
                vMasterEndRegList.push_back(reg);
                vMasterEndValList.push_back(value);
                vMasterEndBitMask.push_back(bitMask);
                mMasterEndSize++;
            }
        }
    }
    file.close();
    return true;
}

int QInterfaceDebug::iReadSensorByI2CMaster(int iDeviceIndex, unsigned char uSlave, unsigned int uAddress, unsigned int *puValue, unsigned char iMode, bool bPage, int iAddrLength, int iValueLength, int iI2cBus, unsigned int iWriteDelay, unsigned int iReadDelay)
{
    int iRet = 0;

           //I2CMaster 开始流程
    for (int i = 0; i < vMasterStartRegList.size(); i++)
    {
        uint8_t slave = vMasterStartSlave[i];
        uint32_t mask = vMasterStartBitMask[i];
        uint32_t reg = vMasterStartRegList[i];
        uint8_t vallength = vMasterStartValueLen[i];
        uint8_t addrlength = vMasterStartAddrLen[i];
        if (mask == 0x00)
        {
            uint32_t val = vMasterStartValList[i];
            QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&val,addrlength, vallength);
        }
        else
        {
            uint32_t rvalue = 0;
            QUsbCommunication::getInstance().readI2cBurst(slave, reg, (uchar*)&rvalue, addrlength, vallength);
            if (vMasterStartValList[i] != 0)
            {
                rvalue=(rvalue | mask);
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue,addrlength, vallength);
            }
            else
            {
                rvalue=(rvalue | (~mask));
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue,addrlength, vallength);
            }
        }
    }

           //I2CMaster逻辑
    uint8_t uTempVal=0x90;
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //写入Sensor SlaveID
    uTempVal=((uSlave<<1)|0x01);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //写入Sensor Address
    uTempVal=((uAddress & 0xff00) >> 8);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    uTempVal=(uAddress & 0xff);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //I2CMaster 标志位
    uTempVal=0x00;
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002013, &uTempVal, 4, 1);
    _sleep(1);
    //读 Sensor数据
    iRet |= QUsbCommunication::getInstance().readI2cBurst(0x22, 0x31002015, (uchar*)puValue, 4, 1);


           //I2CMaster 结束流程
    for (int i = 0; i < vMasterEndRegList.size(); i++)
    {
        uint8_t slave = vMasterEndSlave[i];
        uint32_t mask = vMasterEndBitMask[i];
        uint32_t reg = vMasterEndRegList[i];
        uint8_t vallength = vMasterEndValueLen[i];
        uint8_t addrlength = vMasterEndAddrLen[i];
        if (mask == 0x00)
        {
            uint32_t val = vMasterEndValList[i];
            QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&val, addrlength, vallength);
        }
        else
        {
            uint32_t rvalue = 0;
            QUsbCommunication::getInstance().readI2cBurst(slave, reg, (uchar*)&rvalue, addrlength,vallength);
            if (vMasterEndValList[i] != 0)
            {
                rvalue=(rvalue | mask);
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue, -1, false, addrlength, vallength);
            }
            else
            {
                rvalue= (rvalue & (~mask));
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue, -1, false, addrlength, vallength);
            }
        }
    }

    return iRet;
}

int QInterfaceDebug::iWriteSensorByI2CMaster(int iDeviceIndex, unsigned char uSlave, unsigned int uAddress, unsigned int uValue, unsigned char iMode, bool bPage, int iAddrLength, int iValueLength, int iI2cBus, unsigned int iWriteDelay, unsigned int iReadDelay)
{
    int iRet= 0;
    //I2CMaster 开始流程
    for (int i = 0; i < vMasterStartRegList.size(); i++)
    {
        uint8_t slave = vMasterStartSlave[i];
        uint32_t mask = vMasterStartBitMask[i];
        uint32_t reg = vMasterStartRegList[i];
        uint8_t vallength = vMasterStartValueLen[i];
        uint8_t addrlength = vMasterStartAddrLen[i];
        if (mask == 0x00)
        {
            uint32_t val = vMasterStartValList[i];
            QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&val, addrlength, vallength);
        }
        else
        {
            uint32_t rvalue = 0;
            QUsbCommunication::getInstance().readI2cBurst(slave, reg, (uchar*)&rvalue, addrlength, vallength);
            if (vMasterStartValList[i] != 0)
            {
                rvalue=(rvalue | mask);
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue, addrlength, vallength);
            }
            else
            {
                rvalue=(rvalue & (~mask));
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, (uchar*)&rvalue, addrlength, vallength);
            }
        }
    }

           //I2CMaster逻辑
    uint8_t uTempVal=0x90;
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //写入Sensor SlaveID
    uTempVal=(uSlave<<1);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //写入Sensor Address
    uTempVal=((uAddress & 0xff00)>>8);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    uTempVal=(uAddress & 0xff);
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, &uTempVal, 4, 1);
    //写入Sensor Data
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002014, (uchar*)&uValue, 4, 1);
    //I2CMaster 标志位
    uTempVal=0x00;
    iRet |= QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31002013, &uTempVal, 4, 1);

           //I2CMaster 结束流程
    for (int i = 0; i < vMasterEndRegList.size(); i++)
    {
        uint8_t slave = vMasterEndSlave[i];
        uint32_t mask = vMasterEndBitMask[i];
        uint32_t reg = vMasterEndRegList[i];
        uint8_t vallength = vMasterEndValueLen[i];
        uint8_t addrlength = vMasterEndAddrLen[i];
        if (mask == 0x00)
        {
            uint8_t val = vMasterEndValList[i];
            QUsbCommunication::getInstance().writeI2cBurst(slave, reg, &val, addrlength, vallength);
        }
        else
        {
            uint8_t rvalue = 0;
            QUsbCommunication::getInstance().readI2cBurst(slave, reg, &rvalue, addrlength,vallength);
            if (vMasterEndValList[i] != 0)
            {
                rvalue= (rvalue | mask);
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, &rvalue, -1, false, addrlength, vallength);
            }
            else
            {
                rvalue= (rvalue & (~mask));
                QUsbCommunication::getInstance().writeI2cBurst(slave, reg, &rvalue, addrlength, vallength);
            }
        }
    }
    return iRet;
}

LoadConfigRegisterStruct QInterfaceDebug::generateLoadConfigRegisterStruct(QString line)
{
    // line type 1: ^\s*((0x|0X)?[0-9a-fA-F]+)\s*(,| )\s*((0x|0X)?[0-9a-fA-F]+)([^0-9a-fA-F].*)?$
    // line type 2:
    // ^\s*((0x|0X)?[0-9a-fA-F]+)\s+((0x|0X)?[0-9a-fA-F]+)\s+((0x|0X)?[0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)([^0-9a-fA-F].*)?$
    QRegularExpression reType1("^\\s*((0x|0X)?[0-9a-fA-F]+)\\s*(,| )\\s*((0x|0X)?[0-9a-fA-F]+)([^0-9a-fA-F].*)?$");
    QRegularExpression reType2(
        "^\\s*((0x|0X)?[0-9a-fA-F]+)\\s+((0x|0X)?[0-9a-fA-F]+)\\s+((0x|0X)?[0-9a-fA-F]+)\\s+([0-"
        "9a-fA-F]+)\\s+([0-9a-fA-"
        "F]+)(\\s+([0-9a-fA-F]+)\\s+([0-9a-fA-F]+)\\s+([0-9a-fA-F]+)\\s+([0-9a-fA-F]+))?([^0-9a-fA-F]*.*)?$");

    bool           isI2c                 = ui->loadConfigUsbType->currentIndex() == 1;
    const uint8_t  defaultI2cSlaveId     = ui->loadConfigSlaveEdit->text().toUInt(nullptr, 16);
    const uint32_t defaultI2cAddrLength  = ui->loadConfigI2cAddrLenCombo->currentText().toInt();
    const uint32_t defaultI2cValueLength = ui->loadConfigI2cValueLenCombo->currentText().toInt();
    const uint32_t defaultI2cSpeed       = ui->loadConfigI2cSpeedCombo->currentText().toInt();
    const uint32_t defaultSpiValueLength = ui->loadConfigSpiValueLenCombo->currentText().toInt();
    const uint32_t defaultSpiSpeed       = ui->loadConfigSpiSpeedEdit->text().toInt();
    const uint8_t  defaultSpiMode =
        (ui->loadConfigSpiCphaCheck->isChecked() ? CPHA : 0) | (ui->loadConfigSpiCpolCheck->isChecked() ? CPOL : 0);

    QRegularExpressionMatch matchType1 = reType1.match(line);
    QRegularExpressionMatch matchType2 = reType2.match(line);
    if (!matchType1.hasMatch() && !matchType2.hasMatch()) {
        return { 0 };
    }
    LoadConfigRegisterStruct loadRegister = { 0 };
    loadRegister.slaveId                  = defaultI2cSlaveId;
    loadRegister.addr                     = 0;
    loadRegister.addrLen                  = defaultI2cAddrLength;
    loadRegister.data                     = std::vector<uint8_t>(isI2c ? defaultI2cValueLength : defaultSpiValueLength);
    loadRegister.speed                    = isI2c ? defaultI2cSpeed : defaultSpiSpeed;
    loadRegister.spiMode                  = isI2c ? 0 : defaultSpiMode;
    loadRegister.valueLen                 = isI2c ? defaultI2cValueLength : defaultSpiValueLength;

    QString valueString;
    bool    isBigEndian = true;
    if (matchType2.hasMatch()) {
        emit emit_LoadTypeChange(2);
        for (int i = 0; i < matchType2.lastCapturedIndex(); i++) {
            qDebug() << "matchType2.captured(" << i << "): " << matchType2.captured(i) << "\n";
        }
        loadRegister.slaveId  = matchType2.captured(1).toUInt(nullptr, 16);
        loadRegister.addr     = matchType2.captured(3).toUInt(nullptr, 16);
        valueString           = matchType2.captured(5);
        loadRegister.addrLen  = matchType2.captured(7).toUInt();
        loadRegister.valueLen = matchType2.captured(8).toUInt();
        if (matchType2.captured(11).size() > 0) {
            isBigEndian = (matchType2.captured(11).toUInt() > 0);
        }
        
        if (matchType2.captured(12).size() > 0) {
            loadRegister.speed = matchType2.captured(12).toUInt();
        }
        if (matchType2.captured(13).size() > 0) {
            loadRegister.sleepMs = matchType2.captured(13).toUInt();
        }
        std::vector<uint8_t>(loadRegister.valueLen).swap(loadRegister.data);
    } else if (matchType1.hasMatch()) {
        emit emit_LoadTypeChange(1);
        loadRegister.addr = matchType1.captured(1).toUInt(nullptr, 16);
        valueString       = matchType1.captured(4);
    }
    if (valueString.left(2) == "0x" || valueString.left(2) == "0X") {
        valueString = valueString.mid(2);
    }
    while (static_cast<uint32_t>(valueString.size()) < loadRegister.data.size() * 2) {
        valueString = "0" + valueString;
    }
    if (static_cast<uint32_t>(valueString.size()) > loadRegister.data.size() * 2) {
        // valueString = valueString.right(loadRegister.data.size() * 2);
        loadRegister.data.resize(valueString.size() / 2);
    }
    for (int i = 0; i < loadRegister.data.size(); i++) {
        loadRegister.data[loadRegister.data.size() - 1 - i] = valueString.mid(i * 2, 2).toUInt(nullptr, 16);
    }
    loadRegister.data.resize(loadRegister.valueLen);
    if (isBigEndian) {
        std::reverse(loadRegister.data.begin(), loadRegister.data.end());
    }
    return loadRegister;
}

void QInterfaceDebug::on_I2CBusComboBox_currentIndexChanged(int index)
{

}

