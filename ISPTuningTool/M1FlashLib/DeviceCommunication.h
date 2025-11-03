#ifndef M1_FLASH_TYPE_H
#define M1_FLASH_TYPE_H

#include "M1FlashTypeDef.h"
#include <vector>

class DeviceCommunication
{
public:
    static int setSpiReadWriteFunc(SpiReadFunc readFunc, SpiWriteFunc writeFunc)
    {
        getInstance()._readFunc  = readFunc;
        getInstance()._writeFunc = writeFunc;
        return ERROR_NO_ERR;
    }

    static int burnFlash(int deviceIndex, std::vector<uint8_t> &flashData);
    static int burnFlash(int deviceIndex, uint32_t offset, std::vector<uint8_t> &flashData);
    static int eraseFlash(int deviceIndex);
    
    static int readFlash(int deviceIndex, uint32_t offset, uint32_t size, std::vector<uint8_t> &flashData);

    static int spiRead(int deviceIndex, uint32_t addr, std::vector<uint8_t> &data, uint32_t len);
    static int spiWrite(int deviceIndex, uint32_t addr, std::vector<uint8_t> data);
    static int spiWriteWithCheck(int deviceIndex, uint32_t addr, std::vector<uint8_t> data);

private:
    static DeviceCommunication &getInstance()
    {
        static DeviceCommunication instance;
        return instance;
    }
    DeviceCommunication() { }
    SpiReadFunc  _readFunc;
    SpiWriteFunc _writeFunc;

    static int setSpiClockDiv(int deviceIndex, uint8_t div);
    static int spiMasterDisable(int deviceIndex);
    static int spiMasterEnable(int deviceIndex);
    static int spiMasterSuspendEnable(int deviceIndex);
    static int spiMasterSuspendDisable(int deviceIndex);
    static int spiMasterSuspendRestart(int deviceIndex);
    static int spiMasterSuspendDone(int deviceIndex);
    static int writeTxFifo(int deviceIndex, uint8_t data);
    static int writeTxFifo4byte(int deviceIndex, uint32_t data);
    static int readRxFifo(int deviceIndex, uint8_t &data);
    static int spiMasterSetSerialTxRxLen(int deviceIndex, uint8_t serial_len, uint8_t tx_len, uint8_t rx_len);
    static int spiMasterSetWireProtocalDtr(int deviceIndex, uint8_t mode);
    static int waitForTxFifoEmpty(int deviceIndex);
    static int waitForRxFifoRemain(int deviceIndex, uint8_t remain);
    static int clearFifoInterSts(int deviceIndex);
    static int spiFlashWrEnable(int deviceIndex);
    static int setSpiMasterXipMode(int deviceIndex);
    static int exitSpiMasterXipMode(int deviceIndex);
    static int spiMasterSetRxInter(int deviceIndex, int threshold);
    static int spiMasterSetTxInter(int deviceIndex, int threshold);
    static int waitRxInterrupt(int deviceIndex);
    static int waitTxInterrupt(int deviceIndex);
    static int spiQuadWrOnce(int deviceIndex, uint32_t addr, std::vector<uint8_t> data);
    static int spiReadFlash(int deviceIndex, uint32_t uFlashAddr, std::vector<uint8_t> &data, int nLen);
    static int spiWriteFlash(int deviceIndex, uint32_t uFlashAddr, std::vector<uint8_t> data);
    static int spiBulkEraseFlash(int deviceIndex);
    static int spiReadFlashId(int deviceIndex, std::vector<uint8_t> &data);
    static int spiReadFlashSfdp(int deviceIndex, uint32_t address, std::vector<uint8_t> &sfdp, int length);
    static int spiGetAddressMode(int deviceIndex, uint8_t &addressMode);
    static int spiFlashQuadEnable(int deviceIndex);
};

#endif // M1_FLASH_TYPE_H