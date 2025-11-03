#ifndef AWBBURN_H
#define AWBBURN_H
#include "stdint.h"
#include "AWBCalibrationDLL_global.h"

class AWBBurn
{
public:
    AWBBurn();

    //设置写寄存器回调函数
    static void setWriteFunc(AWBWriteFunc writeFunc);
    //设置获取寄存器地址回调函数
    static void setAWBRegisterFunc(AWBGetAWBRegisterFunc registernFunc);
    //设置获取寄存器值回调函数
    static void setAWBGetRegisterValueFunc(AWBGetRegisterValue getRegisternValueFunc);
    //写入寄存器
    static void writing(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t writeData0);
    //获取地址
    static void getting(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t* registe, uint32_t* mask,uint8_t* registeNum);
    //获取寄存器值
    static void getRegisterValue(uint32_t address, uint8_t* pBuf, uint32_t addrLength, uint32_t valueLength);

private:
    static AWBBurn &getInstance()
    {
        static AWBBurn instance;
        return instance;
    }

    AWBWriteFunc  _writeFunc;
    AWBGetAWBRegisterFunc _awbRegister;
    AWBGetRegisterValue _getRegisterValue;
};

#endif // AWBBURN_H
