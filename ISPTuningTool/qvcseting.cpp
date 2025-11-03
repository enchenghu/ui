#include "qvcseting.h"
#include "ui_qvcseting.h"
#include <Common/Others/qusbcommunication.h>
#include <QMessageBox>
#include "SSUSBLib/SSUSBLib/SSUSBLib.h"

QVCSeting::QVCSeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QVCSeting)
{
    ui->setupUi(this);
    ui->VC0Box->setChecked(true);
    bitWidthList<<"YUV_10"<<"YUV_8"<<"MIPI_RAW10"<<"MIPI_RAW8"<<"MIPI_RAW12"<<"MIPI_RAW14"<<"MIPI_RAW16"<<"MIPI_RAW20";
    ui->VC0comboBox->addItems(bitWidthList);
    ui->VC1comboBox->addItems(bitWidthList);
}

QVCSeting::~QVCSeting()
{
    delete ui;
}

int QVCSeting::GetBitWidthbyIndex(int index)
{
    int bitWidth=1;
    switch (index) {
    case 0:
        bitWidth=12;//yuv10
        break;
    case 1:
        bitWidth=7;
        break;
    case 2:
        bitWidth=1;  //raw10
        break;
    case 3:
        bitWidth=0;
        break;
    case 4:
        bitWidth=2;
        break;
    case 5:
        bitWidth=4;
        break;
    case 6:
        bitWidth=5;
        break;
    case 7:
        bitWidth=6;
        break;
    }
    return bitWidth;
}

void QVCSeting::on_SetButton_clicked()
{
    // int index0=ui->VC0comboBox->currentIndex();
    // int index1=ui->VC1comboBox->currentIndex();
    // VC0_isRaw=index0>1?true:false;
    // VC1_isRaw=index1>1?true:false;
    // int     nRet = 0;
    // if(ui->VC0Box->isChecked()&&ui->VC1Box->isChecked()){
    //     VCcount=2;
    // }else{
    //     VCcount=1;
    // }
    // if(VCcount>1){
    //     SSUSB_SetVCCount(QUsbCommunication::getInstance().getDeviceHandle(),VCcount);
    //     nRet=QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003230, QVector<uint8_t>{ 0x02 }.data(), 4, 1, 0, 1,400);
    //     if(nRet!=0)
    //         return;
    // }
    // else{
    //     SSUSB_SetVCCount(QUsbCommunication::getInstance().getDeviceHandle(),VCcount);
    //     nRet=QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003230, QVector<uint8_t>{ 0x00 }.data(), 4, 1, 0, 1,400);
    //     if(nRet!=0)
    //         return;
    // }

    // if(VC0_isRaw)
    //     QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003c0c, QVector<uint8_t>{ 0x01 }.data(), 4, 1, 0, 1,400);
    // else
    //     QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003c0c, QVector<uint8_t>{ 0x03 }.data(), 4, 1, 0, 1,400);

    // if(VC1_isRaw)
    //     QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003c2c, QVector<uint8_t>{ 0x01 }.data(), 4, 1, 0, 1,400);
    // else
    //     QUsbCommunication::getInstance().writeI2cBurst(0x22, 0x31003c2c, QVector<uint8_t>{ 0x03 }.data(), 4, 1, 0, 1,400);


    // if(ui->VC0Box->isChecked()){
    //     nRet=SSUSB_SetSensorBitWidthExbyVC(QUsbCommunication::getInstance().getDeviceHandle(),0,GetBitWidthbyIndex(ui->VC0comboBox->currentIndex()));
    //     if(nRet!=0)
    //         return;
    //     nRet=SSUSB_SetSensorResolutionExbyVC(QUsbCommunication::getInstance().getDeviceHandle(),0,(uint32_t)ui->VC0WidthSpin->value(),(uint32_t)(ui->VC0HeightSpin->value()*VCcount));
    //     if(nRet!=0)
    //         return;
    // }
    // if(ui->VC1Box->isChecked()){
    //     nRet=SSUSB_SetSensorBitWidthExbyVC(QUsbCommunication::getInstance().getDeviceHandle(),1,GetBitWidthbyIndex(ui->VC1comboBox->currentIndex()));
    //     if(nRet!=0)
    //         return;
    //     nRet=SSUSB_SetSensorResolutionExbyVC(QUsbCommunication::getInstance().getDeviceHandle(),1,(uint32_t)ui->VC1WidthSpin->value(),(uint32_t)(ui->VC1HeightSpin->value()*VCcount));
    //     if(nRet!=0)
    //         return;
    // }
    // QMessageBox::information(this, "Success", "VCMode Set Complete!", QMessageBox::Ok);
}


