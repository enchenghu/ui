#include "DeviceCommunication.h"
#include <chrono>
#include <map>
#include <thread>
#include <vector>

#define SPI_M_BASE                            0x31002180
#define SPI_M_FLASH_BASE                      0x34000000

#define REG_SPI_M_EN                          0x0
#define REG_SPI_DIV                           0x2
#define REG_SPI_M_MODE                        0x4
#define REG_TX_INTER_CONF                     0x08
#define REG_RX_INTER_CONF                     0x09
#define REG_TX_FREE                           0x0C
#define REG_RX_REMAIN                         0x10
#define REG_INTERRUPT                         0x14
#define REG_CLR_INTER_FIFO                    0x18
#define REG_RX_FIFO                           0x1c
#define REG_TX_FIFO                           0x20

#define REG_XIP_EN_DUMMY                      0x24
#define REG_XIP_TRANS_CMD                     0x25
#define REG_XIP_WIRE_MODE                     0x26
#define REG_XIP_ADDRESS_MODE                  0x27

#define REG_RX_LEN                            0x28
#define REG_SERIAL_LEN                        0x2c
#define REG_TX_LEN                            0x2d
#define REG_SUSPEND_DONE                      0x30
#define REG_SUSPEND_EN                        0x31
#define REG_SUSPEND_RESTART                   0x32

/* default data */
#define DEFAULT_XIP_ADDRESS_MODE              0x3
#define DEFAULT_XIP_WIRE_MODE                 0x4

#define DEFAULT_FIFO_DEPTH                    32
#define DEFAULT_TX_THRESHOLD                  31
#define DEFAULT_RX_THRESHOLD                  31
#define DEFAULT_FLASH_PRGM_LEN                256

#define CMD_WR_EN_LATCH                       0x06
#define CMD_BULK_ERASE                        0xc7
#define CMD_READ_FLASH_NONVOLATILE_REG        0x05

// address 4 bytes
#define CMD_4BYTE_QUAD_OUTPUT_FAST_READ       0xec
#define CMD_4BYTE_EXTEND_QUAD_INPUT_FAST_PRGM 0x3e
// address 3 bytes
#define CMD_3BYTE_QUAD_OUTPUT_FAST_READ       0xeb
#define CMD_3BYTE_EXTEND_QUAD_INPUT_FAST_PRGM 0x38

#define CMD_READ_ID                           0x9e

#define MODE_QUAD                             0x4
#define MODE_TX_SINGLE                        0x08
#define MODE_TX_QUAD                          0x0c
#define MODE_RX_SINGLE                        0x10

#define COMMAND_SIZE                          1
#define ADDRESS_SIZE                          4
#define WRITE_SINGLE_COMMAND                  0xfc
#define WRITE_BURST_COMMAND                   0xfe
#define READ_SINGLE_COMMAND                   0xfd
#define READ_BURST_COMMAND                    0xff

#define SPI_MODE                              0x3
#define SPI_SPEED                             5
#define DUMMY_TIMING                          8

#define RETRANSFER_TIMES                      5

#define R_VALUE_TO_BUFFER(initialize_list)    std::vector<uint8_t>(initialize_list).data()

class FlashCommand
{
public:
    static void setFlashAddressMode(int deviceIndex, uint8_t mode)
    {
        getInstance()._flashAddressMode[deviceIndex] = mode;
    }
    static uint8_t getFlashAddressMode(int deviceIndex)
    {
        if (getInstance()._flashAddressMode.find(deviceIndex) == getInstance()._flashAddressMode.end()) {
            return 0;
        }
        return getInstance()._flashAddressMode[deviceIndex];
    }

    static uint8_t getFlashReadCommand(int deviceIndex)
    {
        if (getInstance()._flashAddressMode.find(deviceIndex) == getInstance()._flashAddressMode.end()) {
            return 0;
        }
        if (getInstance()._flashAddressMode[deviceIndex] == 3) {
            return CMD_3BYTE_QUAD_OUTPUT_FAST_READ;
        } else {
            return CMD_4BYTE_QUAD_OUTPUT_FAST_READ;
        }
    }

    static uint8_t getFlashWriteCommand(int deviceIndex)
    {
        if (getInstance()._flashAddressMode.find(deviceIndex) == getInstance()._flashAddressMode.end()) {
            return 0;
        }
        if (getInstance()._flashAddressMode[deviceIndex] == 3) {
            return CMD_3BYTE_EXTEND_QUAD_INPUT_FAST_PRGM;
        } else {
            return CMD_4BYTE_EXTEND_QUAD_INPUT_FAST_PRGM;
        }
    }

private:
    static FlashCommand &getInstance()
    {
        static FlashCommand instance;
        return instance;
    }
    FlashCommand() { }
    ~FlashCommand() { }
    std::map<int, uint8_t> _flashAddressMode;
};

int DeviceCommunication::burnFlash(int deviceIndex, std::vector<uint8_t> &flashData)
{
    uint8_t addressMode = 0;
    auto    res         = spiGetAddressMode(deviceIndex, addressMode);
    if (res != ERROR_NO_ERR || (addressMode != 3 && addressMode != 4)) {
        return ERROR_COMM_UNKNOWN;
    }
    FlashCommand::setFlashAddressMode(deviceIndex, addressMode);

    return spiWriteFlash(deviceIndex, 0, flashData);
}

int DeviceCommunication::burnFlash(int deviceIndex, uint32_t offset, std::vector<uint8_t> &flashData)
{
    uint8_t addressMode = 0;
    auto    res         = spiGetAddressMode(deviceIndex, addressMode);
    if (res != ERROR_NO_ERR || (addressMode != 3 && addressMode != 4)) {
        return ERROR_COMM_UNKNOWN;
    }
    FlashCommand::setFlashAddressMode(deviceIndex, addressMode);
    return spiWriteFlash(deviceIndex, SPI_M_FLASH_BASE + offset, flashData);
}

int DeviceCommunication::eraseFlash(int deviceIndex)
{
    return spiBulkEraseFlash(deviceIndex);
}

int DeviceCommunication::readFlash(int deviceIndex, uint32_t offset, uint32_t size, std::vector<uint8_t> &flashData)
{
    uint8_t addressMode = 0;
    auto    res         = spiGetAddressMode(deviceIndex, addressMode);
    if (res != ERROR_NO_ERR || (addressMode != 3 && addressMode != 4)) {
        return ERROR_COMM_UNKNOWN;
    }
    FlashCommand::setFlashAddressMode(deviceIndex, addressMode);

    return spiReadFlash(deviceIndex, offset, flashData, size);
}

int DeviceCommunication::spiRead(int deviceIndex, uint32_t addr, std::vector<uint8_t> &data, uint32_t len)
{
    if (len == 0) {
        return ERROR_NO_ERR;
    }
    if (!DeviceCommunication::getInstance()._readFunc) {
        return ERROR_COMM_FUNC;
    }
    std::vector<uint8_t>(len, 0).swap(data);
    uint32_t remainLen   = len;
    uint32_t currentAddr = addr;
    // read part can't divide by 4
    if (currentAddr % 4 != 0) {
        uint32_t readLen = 4 - currentAddr % 4;
        if (readLen > remainLen) {
            readLen = remainLen;
        }
        std::vector<uint8_t> sendData(ADDRESS_SIZE + COMMAND_SIZE);
        std::vector<uint8_t> recvData(readLen);
        sendData[0] = READ_SINGLE_COMMAND;
        memcpy(sendData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        if (0
            != DeviceCommunication::getInstance()._readFunc(deviceIndex, sendData.data(),
                static_cast<int>(sendData.size()), recvData.data(), static_cast<int>(recvData.size()), SPI_MODE,
                SPI_SPEED, DUMMY_TIMING)) {
            return ERROR_COMM_READ;
        }
        auto offset = currentAddr - addr;
        memcpy(data.data() + offset, recvData.data(), readLen);
        currentAddr += readLen;
        remainLen -= readLen;
    }
    // read part can divide by 4
    while (remainLen >= 4) {
        uint32_t readLen = remainLen - remainLen % 4;
        if (readLen > DEFAULT_FLASH_PRGM_LEN) {
            readLen = DEFAULT_FLASH_PRGM_LEN;
        }
        std::vector<uint8_t> sendData(ADDRESS_SIZE + COMMAND_SIZE);
        std::vector<uint8_t> recvData(readLen);
        sendData[0] = READ_BURST_COMMAND;
        memcpy(sendData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        if (0
            != DeviceCommunication::getInstance()._readFunc(deviceIndex, sendData.data(),
                static_cast<int>(sendData.size()), recvData.data(), static_cast<int>(recvData.size()), SPI_MODE,
                SPI_SPEED, DUMMY_TIMING)) {
            return ERROR_COMM_READ;
        }
        auto offset = currentAddr - addr;
        memcpy(data.data() + offset, recvData.data(), readLen);
        currentAddr += readLen;
        remainLen -= readLen;
    }
    // read part can't divide by 4
    if (remainLen > 0) {
        std::vector<uint8_t> sendData(ADDRESS_SIZE + COMMAND_SIZE);
        std::vector<uint8_t> recvData(remainLen);
        sendData[0] = READ_SINGLE_COMMAND;
        memcpy(sendData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        if (0
            != DeviceCommunication::getInstance()._readFunc(deviceIndex, sendData.data(),
                static_cast<int>(sendData.size()), recvData.data(), static_cast<int>(recvData.size()), SPI_MODE,
                SPI_SPEED, DUMMY_TIMING)) {
            return ERROR_COMM_READ;
        }
        auto offset = currentAddr - addr;
        memcpy(data.data() + offset, recvData.data(), remainLen);
    }
    return ERROR_NO_ERR;
}

int DeviceCommunication::spiWrite(int deviceIndex, uint32_t addr, std::vector<uint8_t> data)
{
    if (data.size() == 0) {
        return ERROR_NO_ERR;
    }
    if (!DeviceCommunication::getInstance()._writeFunc) {
        return ERROR_COMM_FUNC;
    }
    uint32_t sentLen     = 0;
    uint32_t remainLen   = static_cast<uint32_t>(data.size());
    uint32_t currentAddr = addr;
    // send part can't divide by 4
    if (currentAddr % 4 != 0) {
        uint32_t sendLen = 4 - currentAddr % 4;
        if (sendLen > remainLen) {
            sendLen = remainLen;
        }
        std::vector<uint8_t> tempData(sendLen + ADDRESS_SIZE + COMMAND_SIZE);
        tempData[0] = WRITE_SINGLE_COMMAND;
        memcpy(tempData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        memcpy(tempData.data() + COMMAND_SIZE + ADDRESS_SIZE, data.data(), sendLen);
        if (0
            != DeviceCommunication::getInstance()._writeFunc(deviceIndex, tempData.data(),
                static_cast<int>(tempData.size()), SPI_MODE, SPI_SPEED)) {
            return ERROR_COMM_WRITE;
        }
        sentLen += sendLen;
        remainLen -= sendLen;
        currentAddr += sendLen;
    }
    // send part can divide by 4
    while (remainLen >= 4) {
        uint32_t sendLen = remainLen - remainLen % 4;
        if (sendLen > DEFAULT_FLASH_PRGM_LEN) {
            sendLen = DEFAULT_FLASH_PRGM_LEN;
        }
        std::vector<uint8_t> tempData(sendLen + ADDRESS_SIZE + COMMAND_SIZE);
        tempData[0] = WRITE_BURST_COMMAND;
        memcpy(tempData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        memcpy(tempData.data() + COMMAND_SIZE + ADDRESS_SIZE, data.data() + sentLen, sendLen);
        if (0
            != DeviceCommunication::getInstance()._writeFunc(deviceIndex, tempData.data(),
                static_cast<int>(tempData.size()), SPI_MODE, SPI_SPEED)) {
            return ERROR_COMM_WRITE;
        }
        sentLen += sendLen;
        remainLen -= sendLen;
        currentAddr += sendLen;
    }
    // send part can't divide by 4
    if (remainLen > 0) {
        std::vector<uint8_t> tempData(remainLen + ADDRESS_SIZE + COMMAND_SIZE);
        tempData[0] = WRITE_SINGLE_COMMAND;
        memcpy(tempData.data() + COMMAND_SIZE, &currentAddr, ADDRESS_SIZE);
        memcpy(tempData.data() + COMMAND_SIZE + ADDRESS_SIZE, data.data() + sentLen, remainLen);
        if (0
            != DeviceCommunication::getInstance()._writeFunc(deviceIndex, tempData.data(),
                static_cast<int>(tempData.size()), SPI_MODE, SPI_SPEED)) {
            return ERROR_COMM_WRITE;
        }
    }
    return ERROR_NO_ERR;
}

int DeviceCommunication::spiWriteWithCheck(int deviceIndex, uint32_t addr, std::vector<uint8_t> data)
{
    int res   = ERROR_NO_ERR;
    int count = RETRANSFER_TIMES;
    do {
        res = spiWrite(deviceIndex, addr, data);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        std::vector<uint8_t> readData;
        res = spiRead(deviceIndex, addr, readData, static_cast<uint32_t>(data.size()));
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (memcmp(data.data(), readData.data(), data.size()) == 0) {
            return ERROR_NO_ERR;
        }
    } while (--count > 0);
    return ERROR_COMM_WRITE;
}

int DeviceCommunication::setSpiClockDiv(int deviceIndex, uint8_t div)
{
    auto res = spiWrite(deviceIndex, SPI_M_BASE + REG_SPI_DIV, { div });
    return res;
}

int DeviceCommunication::spiMasterDisable(int deviceIndex)
{
    auto res = spiWrite(deviceIndex, SPI_M_BASE + REG_SPI_M_EN, { 0 });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_TX_LEN, { 0 });
    return res;
}
int DeviceCommunication::spiMasterEnable(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SPI_M_EN, { 0x23 });
}
int DeviceCommunication::spiMasterSuspendEnable(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SUSPEND_EN, { 0x1 });
}
int DeviceCommunication::spiMasterSuspendDisable(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SUSPEND_EN, { 0 });
}
int DeviceCommunication::spiMasterSuspendRestart(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SUSPEND_RESTART, { 0x1 });
}
int DeviceCommunication::spiMasterSuspendDone(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SUSPEND_DONE, { 0x1 });
}
int DeviceCommunication::writeTxFifo(int deviceIndex, uint8_t data)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_TX_FIFO, { data });
}
int DeviceCommunication::writeTxFifo4byte(int deviceIndex, uint32_t data)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_TX_FIFO, { (uint8_t *)&data, (uint8_t *)&data + 4 });
}
int DeviceCommunication::readRxFifo(int deviceIndex, uint8_t &data)
{
    std::vector<uint8_t> readData;
    auto                 res = spiRead(deviceIndex, SPI_M_BASE + REG_RX_FIFO, readData, 1);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    data = readData[0];
    return ERROR_NO_ERR;
}
int DeviceCommunication::spiMasterSetSerialTxRxLen(int deviceIndex, uint8_t serial_len, uint8_t tx_len, uint8_t rx_len)
{
    auto res = spiWrite(deviceIndex, SPI_M_BASE + REG_RX_LEN, { static_cast<uint8_t>(rx_len) });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_SERIAL_LEN, { static_cast<uint8_t>(serial_len) });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_TX_LEN, { static_cast<uint8_t>(tx_len) });
    return res;
}
int DeviceCommunication::spiMasterSetWireProtocalDtr(int deviceIndex, uint8_t mode)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_SPI_M_MODE, { mode });
}
int DeviceCommunication::waitForTxFifoEmpty(int deviceIndex)
{
    int                  res;
    std::vector<uint8_t> readData;
    auto                 now = std::chrono::system_clock::now();
    do {
        res = spiRead(deviceIndex, SPI_M_BASE + REG_TX_FREE, readData, 1);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
    } while ((0x3f & readData[0]) != DEFAULT_FIFO_DEPTH);
    return ERROR_NO_ERR;
}
int DeviceCommunication::waitForRxFifoRemain(int deviceIndex, uint8_t remain)
{
    int                  res;
    std::vector<uint8_t> readData;
    auto                 now = std::chrono::system_clock::now();
    do {
        res = spiRead(deviceIndex, SPI_M_BASE + REG_RX_REMAIN, readData, 1);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
    } while ((0x3f & readData[0]) != remain);
    return ERROR_NO_ERR;
}
int DeviceCommunication::clearFifoInterSts(int deviceIndex)
{
    return spiWrite(deviceIndex, SPI_M_BASE + REG_CLR_INTER_FIFO, { 0x3 });
}
int DeviceCommunication::spiFlashWrEnable(int deviceIndex)
{
    auto res = spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_TX_SINGLE);
    res |= writeTxFifo(deviceIndex, CMD_WR_EN_LATCH);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 0);
    res |= waitForTxFifoEmpty(deviceIndex);
    return res;
}

int DeviceCommunication::setSpiMasterXipMode(int deviceIndex)
{
    auto res = spiMasterEnable(deviceIndex);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_QUAD);
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_XIP_EN_DUMMY, { ((8 << 2) + 1) });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_XIP_TRANS_CMD, { CMD_4BYTE_QUAD_OUTPUT_FAST_READ });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_XIP_WIRE_MODE, { DEFAULT_XIP_WIRE_MODE });
    res |= spiWrite(deviceIndex, SPI_M_BASE + REG_XIP_ADDRESS_MODE, { DEFAULT_XIP_ADDRESS_MODE });
    return res;
}
int DeviceCommunication::exitSpiMasterXipMode(int deviceIndex)
{
    auto res = spiWrite(deviceIndex, SPI_M_BASE + REG_XIP_EN_DUMMY, { 0 });
    res |= spiMasterDisable(deviceIndex);
    return res;
}
int DeviceCommunication::spiMasterSetRxInter(int deviceIndex, int threshold)
{
    if (threshold >= 0 && threshold <= 32) {
        if (threshold == 0) {
            return spiWrite(deviceIndex, SPI_M_BASE + REG_RX_INTER_CONF, { 0 });
        } else {
            return spiWrite(deviceIndex, SPI_M_BASE + REG_RX_INTER_CONF,
                { static_cast<uint8_t>((1 + (threshold << 1)) & 0xff) });
        }
    }
    return ERROR_COMM_UNKNOWN;
}
int DeviceCommunication::spiMasterSetTxInter(int deviceIndex, int threshold)
{
    if (threshold >= 0 && threshold <= 32) {
        if (threshold == 0) {
            return spiWrite(deviceIndex, SPI_M_BASE + REG_TX_INTER_CONF, { 0 });
        } else {
            return spiWrite(deviceIndex, SPI_M_BASE + REG_TX_INTER_CONF,
                { static_cast<uint8_t>((1 + (threshold << 1)) & 0xff) });
        }
    }
    return ERROR_COMM_UNKNOWN;
}
int DeviceCommunication::waitRxInterrupt(int deviceIndex)
{
    int                  res;
    std::vector<uint8_t> readData;
    auto                 now = std::chrono::system_clock::now();
    do {
        res = spiRead(deviceIndex, SPI_M_BASE + REG_INTERRUPT, readData, 1);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
    } while ((0x4 & readData[0]) == 0);
    return ERROR_NO_ERR;
}
int DeviceCommunication::waitTxInterrupt(int deviceIndex)
{
    int                  res;
    std::vector<uint8_t> readData;
    auto                 now = std::chrono::system_clock::now();
    do {
        res = spiRead(deviceIndex, SPI_M_BASE + REG_INTERRUPT, readData, 1);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
    } while ((0x1 & readData[0]) == 0);
    return ERROR_NO_ERR;
}

int DeviceCommunication::spiQuadWrOnce(int deviceIndex, uint32_t addr, std::vector<uint8_t> data)
{
    if (data.size() > 256) {
        return ERROR_COMM_UNKNOWN;
    }

    int res = 0;
    res |= spiMasterEnable(deviceIndex);
    setSpiClockDiv(deviceIndex, 2);
    spiFlashQuadEnable(deviceIndex);

    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);

    uint8_t flag = 0xff;
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSetRxInter(deviceIndex, 1);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 1);
    auto now = std::chrono::system_clock::now();
    while (true) {
        res |= writeTxFifo(deviceIndex, CMD_READ_FLASH_NONVOLATILE_REG);
        res |= waitRxInterrupt(deviceIndex);
        res |= readRxFifo(deviceIndex, flag);
        res |= clearFifoInterSts(deviceIndex);
        if ((flag & 0x1) == 0) {
            break;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    res |= spiFlashWrEnable(deviceIndex);

    flag = 0xff;
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSetRxInter(deviceIndex, 1);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 1);
    now = std::chrono::system_clock::now();
    while (true) {
        res |= writeTxFifo(deviceIndex, CMD_READ_FLASH_NONVOLATILE_REG);
        res |= waitRxInterrupt(deviceIndex);
        res |= readRxFifo(deviceIndex, flag);
        res |= clearFifoInterSts(deviceIndex);
        if ((flag & 0x2) != 0) {
            break;
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    res |= spiMasterSuspendEnable(deviceIndex);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= spiMasterSetTxInter(deviceIndex, DEFAULT_FIFO_DEPTH);

    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_TX_QUAD);
    res |= writeTxFifo(deviceIndex, FlashCommand::getFlashWriteCommand(deviceIndex));
    if (FlashCommand::getFlashAddressMode(deviceIndex) == 4)
        res |= writeTxFifo(deviceIndex, (addr >> 24) & 0xff);
    res |= writeTxFifo(deviceIndex, (addr >> 16) & 0xff);
    res |= writeTxFifo(deviceIndex, (addr >> 8) & 0xff);
    res |= writeTxFifo(deviceIndex, (addr >> 0) & 0xff);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1 + FlashCommand::getFlashAddressMode(deviceIndex),
        1 + FlashCommand::getFlashAddressMode(deviceIndex), 0);
    res |= waitTxInterrupt(deviceIndex);
    res |= clearFifoInterSts(deviceIndex);
    uint32_t remainLen = static_cast<uint32_t>(data.size());
    uint32_t cnt       = 0;
    while (remainLen > 0) {
        cnt = remainLen >= DEFAULT_FIFO_DEPTH ? DEFAULT_FIFO_DEPTH : remainLen;
        std::vector<uint8_t> transBuf(cnt);
        memcpy(transBuf.data(), data.data() + (data.size() - remainLen), cnt);
        remainLen -= cnt;
        if (cnt == DEFAULT_FIFO_DEPTH) {
            for (int i = 0; i < DEFAULT_FIFO_DEPTH / 4; i++) {
                res |= writeTxFifo4byte(deviceIndex, *(uint32_t *)(transBuf.data() + 4 * i));
            }
        } else {
            for (uint32_t i = 0; i < cnt; i++) {
                res |= writeTxFifo(deviceIndex, transBuf[i]);
            }
        }
        res |= spiMasterSetSerialTxRxLen(deviceIndex, 1 + FlashCommand::getFlashAddressMode(deviceIndex), cnt, 0);
        res |= spiMasterSuspendRestart(deviceIndex);
        res |= waitTxInterrupt(deviceIndex);
        res |= clearFifoInterSts(deviceIndex);
    }
    res |= spiMasterSetTxInter(deviceIndex, 0);
    res |= spiMasterSuspendDone(deviceIndex);
    res |= spiMasterSuspendDisable(deviceIndex);
    flag = 0xff;
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSetRxInter(deviceIndex, 1);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 1);
    now = std::chrono::system_clock::now();
    while (true) {
        res |= writeTxFifo(deviceIndex, CMD_READ_FLASH_NONVOLATILE_REG);
        res |= waitRxInterrupt(deviceIndex);
        res |= readRxFifo(deviceIndex, flag);
        res |= clearFifoInterSts(deviceIndex);
        if ((flag & 0x1) == 0) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
            > 1000) {
            return ERROR_COMM_TIMEOUT;
        }
    }
    res |= spiMasterDisable(deviceIndex);

    return res;
}
int DeviceCommunication::spiReadFlash(int deviceIndex, uint32_t uFlashAddr, std::vector<uint8_t> &data, int nLen)
{
    std::vector<uint8_t>().swap(data);
    setSpiClockDiv(deviceIndex, 2);

    spiMasterEnable(deviceIndex);
    spiFlashQuadEnable(deviceIndex);

    clearFifoInterSts(deviceIndex);
    spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);

    spiMasterSetWireProtocalDtr(deviceIndex, 0x14);

    writeTxFifo(deviceIndex, FlashCommand::getFlashReadCommand(deviceIndex));
    if (FlashCommand::getFlashAddressMode(deviceIndex) == 4)
        writeTxFifo(deviceIndex, (uFlashAddr >> 24) & 0xff);
    writeTxFifo(deviceIndex, (uFlashAddr >> 16) & 0xff);
    writeTxFifo(deviceIndex, (uFlashAddr >> 8) & 0xff);
    writeTxFifo(deviceIndex, (uFlashAddr >> 0) & 0xff);
    const int dummyLen = 3;
    for (int i = 0; i < dummyLen; i++) {
        writeTxFifo(deviceIndex, 0x00);
    }
    spiMasterSuspendEnable(deviceIndex);

    int singleLen = nLen > DEFAULT_FIFO_DEPTH ? DEFAULT_FIFO_DEPTH : nLen;
    spiMasterSetRxInter(deviceIndex, singleLen);
    spiMasterSetSerialTxRxLen(deviceIndex, 1, 1 + FlashCommand::getFlashAddressMode(deviceIndex) + dummyLen, singleLen);
    while (true) {
        waitRxInterrupt(deviceIndex);
        // std::vector<uint8_t> rxCnt;
        // spiRead(deviceIndex, SPI_M_BASE + REG_RX_REMAIN, rxCnt, 1);
        // spiRead(deviceIndex, SPI_M_BASE + REG_RX_REMAIN, rxCnt, 1);
        for (int i = 0; i < singleLen; i++) {
            uint8_t singleData;
            readRxFifo(deviceIndex, singleData);
            data.push_back(singleData);
        }
        nLen -= singleLen;
        if (nLen <= 0)
            break;
        singleLen = nLen > DEFAULT_FIFO_DEPTH ? DEFAULT_FIFO_DEPTH : nLen;
        clearFifoInterSts(deviceIndex);

        spiMasterSetRxInter(deviceIndex, singleLen);
        spiMasterSetSerialTxRxLen(deviceIndex, 1, 1 + FlashCommand::getFlashAddressMode(deviceIndex) + dummyLen,
            singleLen);
        spiMasterSuspendRestart(deviceIndex);
    }
    spiMasterSetRxInter(deviceIndex, 0);
    spiMasterSuspendDone(deviceIndex);
    spiMasterSuspendDisable(deviceIndex);
    return ERROR_NO_ERR;

    // int  q   = nLen / 4;
    // int  r   = nLen % 4;
    // auto res = clearFifoInterSts(deviceIndex);
    // res |= setSpiMasterXipMode(deviceIndex);
    // for (int i = 0; i < q; i++) {
    //     std::vector<uint8_t> tmpData(4);
    //     res |= spiRead(deviceIndex, uFlashAddr + 4 * i, tmpData, 4);
    //     memcpy(data.data() + 4 * i, tmpData.data(), 4);
    // }
    // for (int i = 0; i < r; i++) {
    //     std::vector<uint8_t> tmpData(1);
    //     res |= spiRead(deviceIndex, uFlashAddr + 4 * q + i, tmpData, 1);
    //     memcpy(data.data() + 4 * q + i, tmpData.data(), 1);
    // }
    // res |= exitSpiMasterXipMode(deviceIndex);
    // return res;
}
int DeviceCommunication::spiWriteFlash(int deviceIndex, uint32_t uFlashAddr, std::vector<uint8_t> data)
{
    if (uFlashAddr % 256 != 0) {
        return ERROR_COMM_UNKNOWN;
    }
    uint32_t q = static_cast<uint32_t>(data.size() / DEFAULT_FLASH_PRGM_LEN);
    uint32_t r = data.size() % DEFAULT_FLASH_PRGM_LEN;
    if (q) {
        for (uint32_t i = 0; i < q; i++) {
            std::vector<uint8_t> tempData(DEFAULT_FLASH_PRGM_LEN);
            memcpy(tempData.data(), data.data() + i * DEFAULT_FLASH_PRGM_LEN, DEFAULT_FLASH_PRGM_LEN);
            auto res = spiQuadWrOnce(deviceIndex, uFlashAddr + i * DEFAULT_FLASH_PRGM_LEN, tempData);
            if (res != ERROR_NO_ERR) {
                return res;
            }
        }
    }
    if (r) {
        std::vector<uint8_t> tempData(r);
        memcpy(tempData.data(), data.data() + q * DEFAULT_FLASH_PRGM_LEN, r);
        auto res = spiQuadWrOnce(deviceIndex, uFlashAddr + q * DEFAULT_FLASH_PRGM_LEN, tempData);
        if (res != ERROR_NO_ERR) {
            return res;
        }
    }
    return ERROR_NO_ERR;
}
int DeviceCommunication::spiBulkEraseFlash(int deviceIndex)
{
    uint8_t flag = 0xff;
    auto    res  = spiMasterEnable(deviceIndex);
    setSpiClockDiv(deviceIndex, 2);

    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= clearFifoInterSts(deviceIndex);

    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_TX_SINGLE);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 0);

    res |= writeTxFifo(deviceIndex, CMD_WR_EN_LATCH);
    res |= waitForTxFifoEmpty(deviceIndex);
    res |= writeTxFifo(deviceIndex, CMD_BULK_ERASE);
    res |= waitForTxFifoEmpty(deviceIndex);

    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSetRxInter(deviceIndex, 1);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 1);
    auto now = std::chrono::system_clock::now();
    while (true) {
        res |= writeTxFifo(deviceIndex, CMD_READ_FLASH_NONVOLATILE_REG);
        res |= waitRxInterrupt(deviceIndex);
        res |= readRxFifo(deviceIndex, flag);
        res |= clearFifoInterSts(deviceIndex);
        if ((flag & 0x1) == 0) {
            break;
        }
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - now).count() > 120)
            return ERROR_COMM_UNKNOWN;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetRxInter(deviceIndex, 0);
    res |= spiMasterDisable(deviceIndex);
    return res;
}
int DeviceCommunication::spiReadFlashId(int deviceIndex, std::vector<uint8_t> &data)
{
    std::vector<uint8_t> id;
    auto                 res = spiMasterEnable(deviceIndex);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSetRxInter(deviceIndex, 20);
    res |= writeTxFifo(deviceIndex, CMD_READ_ID);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 1, 1, 10);
    res |= waitRxInterrupt(deviceIndex);
    for (int i = 0; i < 32; i++) {
        uint8_t temp;
        res |= readRxFifo(deviceIndex, temp);
        id.push_back(temp);
    }
    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetRxInter(deviceIndex, 0);
    data.swap(id);
    return res;
}

int DeviceCommunication::spiReadFlashSfdp(int deviceIndex, uint32_t address, std::vector<uint8_t> &sfdp, int length)
{
    std::vector<uint8_t>().swap(sfdp);
    auto res = spiMasterEnable(deviceIndex);
    setSpiClockDiv(deviceIndex, 2);

    res |= spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= clearFifoInterSts(deviceIndex);

    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_RX_SINGLE);
    res |= spiMasterSuspendEnable(deviceIndex);
    res |= spiMasterSetRxInter(deviceIndex, length);

    res |= writeTxFifo(deviceIndex, 0x5a);
    res |= writeTxFifo(deviceIndex, (address >> 16) & 0xff);
    res |= writeTxFifo(deviceIndex, (address >> 8) & 0xff);
    res |= writeTxFifo(deviceIndex, (address >> 0) & 0xff);
    res |= writeTxFifo(deviceIndex, 0x00);

    res |= spiMasterSetSerialTxRxLen(deviceIndex, 5, 5, length);
    res |= waitForRxFifoRemain(deviceIndex, length);

    if (res != ERROR_NO_ERR) {
        return res;
    }

    for (int i = 0; i < length; i++) {
        uint8_t temp;
        res |= readRxFifo(deviceIndex, temp);
        sfdp.push_back(temp);
    }

    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSuspendDone(deviceIndex);
    res |= spiMasterSetRxInter(deviceIndex, 0);
    res |= spiMasterSuspendDisable(deviceIndex);
    return 0;
}

int DeviceCommunication::spiGetAddressMode(int deviceIndex, uint8_t &addressMode)
{
    std::vector<uint8_t> sfdpInfo;
    uint32_t             size = 0;

    auto res = spiReadFlashSfdp(deviceIndex, 0, sfdpInfo, 16);
    if (res != ERROR_NO_ERR || sfdpInfo[0] != 'S' || sfdpInfo[1] != 'F' || sfdpInfo[2] != 'D' || sfdpInfo[3] != 'P'
        || sfdpInfo[8] != 0x00) {
        return res;
    }

    res = spiReadFlashSfdp(deviceIndex, (sfdpInfo[14] << 8) | (sfdpInfo[13] << 4) | (sfdpInfo[12] << 0), sfdpInfo, 8);
    if (res != ERROR_NO_ERR) {
        return res;
    }

    if ((sfdpInfo[7] & 0x80) == 0x80)
        size = sfdpInfo[7] & 0x7f;
    else {
        uint32_t tmpSize = (sfdpInfo[7] << 24) | (sfdpInfo[6] << 16) | (sfdpInfo[5] << 8) | (sfdpInfo[4] << 0);
        if (tmpSize) {
            do {
                size++;
            } while (tmpSize >>= 1);
        } else {
            size = 0;
            return ERROR_COMM_UNKNOWN;
        }
    }
    size -= 3;
    if (size > 24)
        addressMode = 4;
    else
        addressMode = 3;
    return ERROR_NO_ERR;
}

int DeviceCommunication::spiFlashQuadEnable(int deviceIndex)
{
    auto res = spiMasterSetSerialTxRxLen(deviceIndex, 0, 0, 0);
    res |= clearFifoInterSts(deviceIndex);
    res |= spiMasterSetWireProtocalDtr(deviceIndex, MODE_TX_SINGLE);
    res |= writeTxFifo(deviceIndex, 0x01);
    res |= writeTxFifo(deviceIndex, 0x40);
    res |= spiMasterSetSerialTxRxLen(deviceIndex, 2, 2, 0);
    res |= waitForTxFifoEmpty(deviceIndex);

    return res;
}
