#include "qusbcommunication.h"
#include "SSUSBLib/SSUSBLib/SSUSBLib.h"
#include "qcommondef.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "qpipiclient.h"
extern BOOL bDemoSensUDP;
extern DWORD processId;

QUsbCommunication &QUsbCommunication::getInstance()
{
    L_TRACE_FUNC;
    static QUsbCommunication instance;
    return instance;
}

int QUsbCommunication::enumDevice(char (&name)[MAX_DEVICE_NUM][DEVICE_NAME_LENGTH], int (&deviceAddr)[MAX_DEVICE_NUM],
    int &count)
{
    L_TRACE_FUNC;

    DevListInfo deviceList = { {0} };
    int         devNum     = SSUSB_EnumDevWithSerialNumber(&deviceList);
    count                  = 0;
    for (int i = 0; i < devNum; i++) {
        if (deviceList.DevAddr[i] != 0) {
            // check serial number is start with "M1"
            QString serialNum(reinterpret_cast<char *>(deviceList.uSerialNumber[i]));
            if (serialNum.startsWith("M1")) {
            memset(name[count], 0, DEVICE_NAME_LENGTH);
            memcpy(name[count], serialNum.toStdString().c_str(), serialNum.size());
            deviceAddr[count] = deviceList.DevAddr[i];
            count++;
        }
    }
    }
    return 0;
}

int QUsbCommunication::openDevice(int deviceAddr)
{
    L_TRACE_FUNC;
    if (isOpen)
        return 0;
    int     devAddrList[1] = { deviceAddr };
    HANDLE *hDev           = SSUSB_OpenDevAddr(1, devAddrList, 1);
    if (hDev == NULL)
        return -1;
    handle = hDev[0];
    isOpen = true;
    setI2cSpeed(400);
    return 0;
}

int QUsbCommunication::closeDevice()
{
    L_TRACE_FUNC;
    if (!isOpen)
        return 0;
    SSUSB_CloseDev(&handle);
    handle = NULL;
    isOpen = false;
    return 0;
}

int QUsbCommunication::checkState()
{
    L_TRACE_FUNC;
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;

        int ret = 0xFFFFFFFF;
        ret &= checkSpiState();
        ret &= checkI2cState();
        return ret;
    }else
        return 0;
}

int QUsbCommunication::readBurst(unsigned int address, unsigned char *pBuf, int addrLength, int valueLength)
{
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;

        switch (globalUsbType) {
        case USB_I2C:
            return readI2cBurst(globalSlaveId, address, pBuf, addrLength, valueLength, 0, 1, globalI2cSpeedKhz);
        case USB_SPI:
            return readSpiBurst(address, pBuf, addrLength, valueLength, globalSpiSpeedMhz, globalSpiMode,
                globalSpiIncremental, globalSpiDummyTiming);
        }
    }else{
        return readI2cBurst(globalSlaveId, address, pBuf, addrLength, valueLength, 0, 1, globalI2cSpeedKhz);
    }
    return -1;
}

int QUsbCommunication::writeBurst(unsigned int address, unsigned char *pBuf, int addrLength, int valueLength)
{
    if(!bDemoSensUDP)
    {
        if (!isOpen)
            return -1;

        switch (globalUsbType) {
        case USB_I2C:
            return writeI2cBurst(globalSlaveId, address, pBuf, addrLength, valueLength, 0, 1, globalI2cSpeedKhz);
        case USB_SPI:
            return writeSpiBurst(address, pBuf, addrLength, valueLength, globalSpiSpeedMhz, globalSpiMode,
                globalSpiIncremental);
        }
    }
    else
    {
        return writeI2cBurst(globalSlaveId, address, pBuf, addrLength, valueLength, 0, 1, globalI2cSpeedKhz);
    }
    return -1;
}

void QUsbCommunication::setUsbType(enum USB_TYPE type)
{
    globalUsbType = type;
}

void QUsbCommunication::setI2cSlaveID(unsigned char slaveId)
{
    globalSlaveId = slaveId;
}

unsigned char QUsbCommunication::getI2cSlaveID()
{
    return globalSlaveId;
}

void QUsbCommunication::setI2cSpeed(int speedKhz)
{
    globalI2cSpeedKhz = speedKhz;
}

int QUsbCommunication::getI2cSpeed()
{
    return globalI2cSpeedKhz;
}

void QUsbCommunication::setSpiMode(int spiMode)
{
    globalSpiMode = spiMode;
}

int QUsbCommunication::getSpiMode()
{
    return globalSpiMode;
}

void QUsbCommunication::setSpiSpeed(float speedMhz)
{
    globalSpiSpeedMhz = speedMhz;
}

float QUsbCommunication::getSpiSpeed()
{
    return globalSpiSpeedMhz;
}

void QUsbCommunication::setSpiDummyTiming(int dummyTiming)
{
    globalSpiDummyTiming = dummyTiming;
}

int QUsbCommunication::getSpiDummyTiming()
{
    return globalSpiDummyTiming;
}

void QUsbCommunication::setSpiIncremental(bool isIncremental)
{
    globalSpiIncremental = isIncremental;
}

bool QUsbCommunication::getSpiIncremental()
{
    return globalSpiIncremental;
}

void *QUsbCommunication::getDeviceHandle()
{
    return handle;
}

/**
 * @brief 读取Sensor寄存器, 支持连续读取
 *
 * @param slaveID SlaveID, 7-bit
 * @param address 寄存器地址
 * @param pBuf 读取buffer: 需准备 iValueLength * uSize 大小的空间
 * @param addrLength 地址长度
 * @param valueLength 值长度
 * @param count 读取个数
 * @param busI2c I2C总线
 * @param writeDelay 写延迟
 * @param readDelay 读延迟
 * @retval CAMERACTRL_OK: 读取寄存器成功
 * @retval CAMERACTRL_ERROR: 读取寄存器失败
 */
int QUsbCommunication::readI2cBurst(unsigned char slaveID, unsigned int address, unsigned char *pBuf, int addrLength,
    int valueLength, unsigned int writeDelay, unsigned int readDelay, unsigned int speedKhz,int bus)
{
    L_TRACE_FUNC;
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;
        int ret = setI2cRate(speedKhz);
        ret |= SSUSB_USB2I2CCfg(handle, slaveID, addrLength, valueLength > 4 ? 1 : valueLength, 0, writeDelay, readDelay);
        if(valueLength > 4) {
            ret |= SSUSB_USB2I2CReadBrust(handle, address, pBuf, valueLength);
        } else {
            uint32_t value = 0;
            ret |= SSUSB_USB2I2CRead(handle, address, &value);
            for (int i = 0; i < valueLength; i++) {
                pBuf[i] = (value >> (8 * (valueLength-1-i))) & 0xff;
            }
        }
        // ret |= SSUSB_USB2I2CReadBrust(handle, address, pBuf, valueLength);
        Sleep(readDelay);
        return ret;
    }
    else
    {
        // qint64 startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        int ret=0;
        QJsonObject requestJson;
        requestJson["verifyCode"] = 1;        // 指令包序号
        requestJson["command"] = "ISPReadI2C"; // 指令代码
        requestJson["mode"] = "r";            // 读写模式
        requestJson["processID"] = static_cast<int>(processId);
        requestJson["dataSize"] = valueLength;
        requestJson["bus"] = bus;

        QJsonObject commandData;
        commandData["slave"] = slaveID;
        commandData["addrs"] = static_cast<int>(address);
        commandData["addr_len"] = addrLength;
        commandData["val_len"] = valueLength;
        commandData["I2CSpeed"] = static_cast<int>(speedKhz);
        requestJson["command_data"] = commandData;
        //qDebug() << "I2CRead JSON:" << QJsonDocument(requestJson).toJson(QJsonDocument::Indented);
        ret= QISPClient::getInstance().sendI2CRead(requestJson,pBuf);

        if(valueLength>1 && valueLength<= 4)
        {
            unsigned char *tempBuf = new unsigned char[valueLength];
            for (int i = 0; i < valueLength; i++) {
                tempBuf[i] = pBuf[valueLength - 1 - i];
            }
            memcpy(pBuf, tempBuf, valueLength);
            delete[] tempBuf;
        }

        if (ret != 0)
            return ret;

        // // 记录结束时间
        // qint64 endTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        // qint64 elapsedMs = endTime - startTime;  // 计算耗时
        // qDebug() << "耗时:" << elapsedMs << "ms";
        return ret;
    }
    return 0;
}
/**
 * @brief 写Sensor寄存器, 支持连续写入
 *
 * @param slaveID SlaveID, 7-bit
 * @param address 寄存器地址
 * @param pBuf 写入buffer
 * @param addrLength 地址长度
 * @param valueLength 值长度
 * @param count 写入个数
 * @param busI2c I2C总线
 * @param writeDelay 写延迟
 * @param readDelay 读延迟
 * @retval CAMERACTRL_OK: 写入寄存器成功
 * @retval CAMERACTRL_ERROR: 写入寄存器失败
 */
int QUsbCommunication::writeI2cBurst(unsigned char slaveID, unsigned int address, unsigned char *pBuf, int addrLength,
    int valueLength, unsigned int writeDelay, unsigned int readDelay, unsigned int speedKhz,int bus)
{
    L_TRACE_FUNC;
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;
        int ret = setI2cRate(speedKhz);
        ret |= SSUSB_USB2I2CCfg(handle, slaveID, addrLength, valueLength > 4 ? 1 : valueLength, 0, writeDelay, readDelay);
        if (valueLength > 4) {
            ret |= SSUSB_USB2I2CWriteBrust(handle, address, pBuf, valueLength);
        } else {
            uint32_t value = 0;
            for (int i = 0; i < valueLength; i++) {
                value |= pBuf[i] << (8 * (valueLength-1-i));
            }
            ret |= SSUSB_USB2I2CWrite(handle, address, value);
        }
        // ret |= SSUSB_USB2I2CWriteBrust(handle, address, pBuf, valueLength);
        Sleep(writeDelay);
        return ret;
    }else{
        int ret=0;
        QJsonObject requestJson;
        requestJson["verifyCode"] = 1;        // 指令包序号
        requestJson["command"] = "ISPWriteI2C"; // 指令代码
        requestJson["mode"] = "w";            // 读写模式
        requestJson["processID"] = static_cast<int>(processId); // 读写的数据量
        requestJson["dataSize"] = valueLength;          // 读写的数据量
        requestJson["bus"] = bus;

        QJsonObject commandData;
        commandData["slave"] = slaveID;          // slave地址
        commandData["addrs"] = static_cast<int>(address);     // 寄存器列表
        if(static_cast<int>(address)<0)
            return 0;
        QJsonArray vals;

        if(valueLength>1 && valueLength<= 4){
            for (int i = 0; i < valueLength; i++)
                vals.append(static_cast<uint8_t>(pBuf[(valueLength-1)-i]));
        }
        else{
            for (int i = 0; i < valueLength; ++i)
                vals.append(static_cast<uint8_t>(pBuf[i]));
        }
        commandData["vals"] = vals;           // 值列表（写模式）
        commandData["addr_len"] = addrLength;          // 地址长度
        commandData["val_len"] = valueLength;           // 值长度
        commandData["I2CSpeed"] = static_cast<int>(speedKhz);        // 速率
        requestJson["command_data"] = commandData;
        // qDebug() << "I2CWrite JSON:" << QJsonDocument(requestJson).toJson(QJsonDocument::Indented);
        ret=QISPClient::getInstance().sendI2CWrite(requestJson);
        // int index=0;
        // while(index<valueLength){//按最长512进行拆包
        //     int Length=((valueLength-index)>512) ? (512):(valueLength-index);
        //     QJsonObject commandData;
        //     commandData["slave"] = slaveID;
        //     commandData["addrs"] = static_cast<int>(address+index);
        //     QJsonArray vals;
        //     for (int i = index; i <(index + Length); ++i)
        //         vals.append(static_cast<uint8_t>(pBuf[i]));

        //     commandData["vals"] = vals;
        //     commandData["addr_len"] = addrLength;
        //     commandData["val_len"] = Length;
        //     commandData["I2CSpeed"] = static_cast<int>(speedKhz);
        //     requestJson["command_data"] = commandData;
        //     index+=Length;
        //     ret= QISPClient::getInstance().sendI2CWrite(requestJson);
        //     if (ret != 0)
        //         return ret;
        // }
        return ret;

    }

    return 0;
}

int QUsbCommunication::writeFpgaRegister(unsigned int address, unsigned int value)
{
    L_TRACE_FUNC;
    return SSUSB_WriteFPGAReg(handle, address, value);
}

int QUsbCommunication::readFpgaRegister(unsigned int address, unsigned int &value)
{
    L_TRACE_FUNC;
    return SSUSB_ReadFPGAReg(handle, address, &value);
}

/**
 * @brief 读取Sensor寄存器, 支持连续读取
 *
 * @param slaveID SlaveID, 7-bit
 * @param address 寄存器地址
 * @param pBuf 读取buffer: 需准备 iValueLength * uSize 大小的空间
 * @param addrLength 地址长度
 * @param valueLength 值长度
 * @param count 读取个数
 * @retval CAMERACTRL_OK: 读取寄存器成功
 * @retval CAMERACTRL_ERROR: 读取寄存器失败
 */
int QUsbCommunication::readSpiBurst(unsigned int address, unsigned char *pBuf, int addrLength, int valueLength,
    float speedMhz, int spiMode, bool isIncremental, int dummyTiming)
{
    L_TRACE_FUNC;
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;
        int ret = SSUSB_HighSpeedFPGA2SPIRead(handle, spiMode, speedMhz, address, pBuf, addrLength, valueLength,
            isIncremental, dummyTiming);
        return ret;
    }else{
        QJsonObject requestJson;
        requestJson["verifyCode"] = 1;        // 指令包序号
        requestJson["command"] = "ISPReadSPI"; // 指令代码
        requestJson["mode"] = "r";            // 读写模式
        requestJson["processID"] = static_cast<int>(processId); // 读写的数据量
        requestJson["dataSize"] = valueLength;          // 读写的数据量

        QJsonObject commandData;
        commandData["addrs"] = static_cast<int>(address);     // 寄存器列表
        commandData["addr_len"] = addrLength;          // 地址长度
        commandData["val_len"] = valueLength;           // 值长度
        commandData["SPISpeed"] = static_cast<int>(speedMhz);        // 速率
        commandData["SPIMode"] = spiMode;
        commandData["incremental"] = isIncremental;
        commandData["dummyTime"] = dummyTiming;
        requestJson["command_data"] = commandData;
        int ret=QISPClient::getInstance().sendSPIRead(requestJson,pBuf);
        return ret;

    }
    return -1;

}

/**
 * @brief 写Sensor寄存器, 支持连续写入
 *
 * @param slaveID SlaveID, 7-bit
 * @param address 寄存器地址
 * @param pBuf 写入buffer
 * @param addrLength 地址长度
 * @param valueLength 值长度
 * @param count 写入个数
 * @retval CAMERACTRL_OK: 写入寄存器成功
 * @retval CAMERACTRL_ERROR: 写入寄存器失败
 */
int QUsbCommunication::writeSpiBurst(unsigned int address, unsigned char *pBuf, int addrLength, int valueLength,
    float speedMhz, int spiMode, bool isIncremental)
{
    L_TRACE_FUNC;
    if(!bDemoSensUDP){
        if (!isOpen)
            return -1;
        int ret =
            SSUSB_HighSpeedFPGA2SPIWrite(handle, spiMode, speedMhz, address, pBuf, addrLength, valueLength, isIncremental);
        return ret;
    }else{
        QJsonObject requestJson;
        requestJson["verifyCode"] = 1;        // 指令包序号
        requestJson["command"] = "ISPWriteSPI"; // 指令代码
        requestJson["mode"] = "w";            // 读写模式
        requestJson["processID"] = static_cast<int>(processId); // 读写的数据量
        requestJson["dataSize"] = valueLength;          // 读写的数据量

        QJsonObject commandData;
        commandData["addrs"] = static_cast<int>(address);     // 寄存器列表
        QJsonArray vals;
        for (int i = 0; i < valueLength; ++i)
            vals.append(static_cast<uint8_t>(pBuf[i]));
        commandData["vals"] = vals;           // 值列表（写模式）
        commandData["addr_len"] = addrLength;          // 地址长度
        commandData["val_len"] = valueLength;           // 值长度
        commandData["SPISpeed"] = static_cast<int>(speedMhz);        // 速率
        commandData["SPIMode"] = spiMode;
        commandData["incremental"] = isIncremental;
        requestJson["command_data"] = commandData;
        int ret=QISPClient::getInstance().sendSPIWrite(requestJson);
        return ret;
    }
    return -1;
}

QUsbCommunication::QUsbCommunication()
    : globalSlaveId(0x22)
    , globalUsbType(USB_SPI)
    , isOpen(false)
    , handle(NULL)
    , globalI2cSpeedKhz(400)
    , globalSpiMode(3)
    , globalSpiSpeedMhz(5)
    , globalSpiDummyTiming(8)
    , globalSpiIncremental(false)
{
    L_TRACE_FUNC;
}

QUsbCommunication::~QUsbCommunication()
{
    L_TRACE_FUNC;
    if (isOpen)
        closeDevice();
}

int QUsbCommunication::checkI2cState()
{
    L_TRACE_FUNC;
    for (uint8_t slave = 0; slave < 0x80; slave++) {
        unsigned char buf[1];
        if (readI2cBurst(slave, 0, buf) == 0)
            return 0;
    }
    return -1;
}

int QUsbCommunication::checkSpiState()
{
    L_TRACE_FUNC;
    unsigned char buf[1];
    if (readSpiBurst(0, buf) == 0)
        return 0;
    return -1;
}

int QUsbCommunication::setI2cRate(int speedKhz)
{
    static int lastRate = 0;
    if (lastRate == speedKhz)
        return 0;
    BYTE buf[2];
    buf[0] = (speedKhz & 0xff00) >> 8;
    buf[1] = speedKhz & 0x00ff;

    SSUSB_I2cConfig(handle, buf);
    lastRate = speedKhz;
    return 0;
}
