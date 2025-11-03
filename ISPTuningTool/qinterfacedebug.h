#ifndef QINTERFACEDEBUG_H
#define QINTERFACEDEBUG_H

#include <Common/UI/qbitcheckboxgroup.h>
#include <Common/UI/qdirectionbuttongroup.h>
#include <QPointer>
#include <QVector>
#include <QWidget>
#include <QProgressDialog>
namespace Ui {
class QInterfaceDebug;
}
struct LoadConfigRegisterStruct;
class QInterfaceDebug : public QWidget
{
    Q_OBJECT

public:
    explicit QInterfaceDebug(QWidget *parent = nullptr);
    ~QInterfaceDebug();

private slots:
    void on_IC2ReadBtn_clicked();

    void on_IC2WriteBtn_clicked();

    void on_slaveRadio1_clicked(bool checked);

    void on_slaveRadio2_clicked(bool checked);

    void on_SPIReadBtn_clicked();

    void on_SPIWriteBtn_clicked();

    void on_slaveRadio3_clicked(bool checked);

    void on_spiCphaCheck_clicked(bool checked);

    void on_spiCpolCheck_clicked(bool checked);

    void on_loadConfigBtn_clicked();

    void on_i2cFileLoadBtn_clicked();

    void on_i2cFileDumpBtn_clicked();

    void on_spiFileLoadBtn_clicked();

    void on_spiFileDumpBtn_clicked();

    void onFpgaToSensorBtnClicked();
    void onFpgaToM1BtnClicked();

    void on_I2CBusComboBox_currentIndexChanged(int index);

signals:
    void emit_proccessStart(std::string title,int nNum);
    void emit_proccessing(std::string text,int nNum);
    void emit_proccessEnd();

    void emit_MessageBox(std::string title,std::string text);
    void emit_LoadTypeChange(int iType);

private:
    Ui::QInterfaceDebug *ui;
    uint8_t              slaveID       = 0x22;
    uint16_t             I2CRate       = 400;
    uint32_t             RegAddr       = 0;
    uint32_t             RegVal        = 0;
    int                  I2CAddrLength = 2;
    int                  I2CValLength  = 1;

    uint32_t                    SPIRegAddr    = 0;
    uint32_t                    SPIRegVal     = 0;
    uint32_t                    SPISpeed      = 10;
    int                         SPIAddrLength = 2;
    int                         SPIValLength  = 1;
    uint16_t                    SPITiming     = 8;
    int                         SPIDumpLength = 1;
    int                         SPIMode       = 3;
    QString                     I2CFileName;
    QString                     SPIFileName;
    QPointer<QBitCheckBoxGroup> I2CbitCheckBoxGroup = nullptr;
    QPointer<QBitCheckBoxGroup> SPIbitCheckBoxGroup = nullptr;

    void loadSensorConfig(QString filename);
    void loadRegisterConfig(QString filename);
    void loadBatchPartition(QString filename);

    int settingBatchPartition(QString filename);
    int ddrBatchPartition(QString filename);
    int sensorBatchPartition(QString filename);
    int movedataBatchPartition(QString filename);
    int fpgaBatchPartition(QString filename);
    std::vector<uint8_t> vMasterStartSlave, vMasterStartValueLen, vMasterStartAddrLen;
    std::vector<uint32_t> vMasterStartRegList, vMasterStartValList, vMasterStartBitMask;
    unsigned short mMasterStartSize;
    std::vector<uint8_t> vMasterEndSlave, vMasterEndValueLen, vMasterEndAddrLen;
    std::vector<uint32_t> vMasterEndRegList, vMasterEndValList, vMasterEndBitMask;
    unsigned short mMasterEndSize;
    bool bLoadIICMasterConfig();

    int iReadSensorByI2CMaster(int iDeviceIndex, unsigned char uSlave, unsigned int uAddress, unsigned int * puValue, unsigned char iMode, bool bPage, int iAddrLength, int iValueLength, int iI2cBus, unsigned int iWriteDelay, unsigned int iReadDelay);
    int iWriteSensorByI2CMaster(int iDeviceIndex, unsigned char uSlave, unsigned int uAddress, unsigned int uValue, unsigned char iMode, bool bPage, int iAddrLength, int iValueLength, int iI2cBus, unsigned int iWriteDelay, unsigned int iReadDelay);

    void initI2C();
    void initSPI();
    void initGlobalSettings();
    void initLoadConfig();
    void readI2CContent();
    void readSPIContent();
    void updateI2C();
    void updateSPI();
    void setUSBTypeIndex(int index);
    void getSPIMode();

    void                     updatebitBox(uint32_t value, int flag);
    LoadConfigRegisterStruct generateLoadConfigRegisterStruct(QString line);
    QProgressDialog progressDialog;
};

#endif // QINTERFACEDEBUG_H
