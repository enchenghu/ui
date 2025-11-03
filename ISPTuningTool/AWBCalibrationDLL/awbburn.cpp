#include "awbburn.h"

AWBBurn::AWBBurn() { }

void AWBBurn::setWriteFunc(AWBWriteFunc writeFunc)
{
    getInstance()._writeFunc=writeFunc;
}

void AWBBurn::setAWBRegisterFunc(AWBGetAWBRegisterFunc registernFunc)
{
    getInstance()._awbRegister=registernFunc;
}

void AWBBurn::setAWBGetRegisterValueFunc(AWBGetRegisterValue getRegisternValueFunc)
{
    getInstance()._getRegisterValue = getRegisternValueFunc;
}

void AWBBurn::writing(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t writeData)
{
    getInstance()._writeFunc(awbRegisterCode,cameraId,writeData);
}

void AWBBurn::getting(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t *registe, uint32_t *mask, uint8_t *registeNum)
{
    getInstance()._awbRegister( awbRegisterCode,cameraId,registe, mask,registeNum);
}

void AWBBurn::getRegisterValue(uint32_t address, uint8_t *pBuf, uint32_t addrLength, uint32_t valueLength)
{
    getInstance()._getRegisterValue(address,pBuf,addrLength,valueLength);
}
