#ifndef AWBCALIBRATIONDLL_H
#define AWBCALIBRATIONDLL_H

#include "AWBCalibrationDLL_global.h"

#ifdef __cplusplus
extern "C" {
#endif

class AWBCALIBRATIONDLL_EXPORT AWBCalibrationDLL
{
public:
    AWBCalibrationDLL();
};

AWBCALIBRATIONDLL_EXPORT int AWB_CreateWindow();

AWBCALIBRATIONDLL_EXPORT void AWB_SetWriteFunc(AWBWriteFunc writeFunc);

AWBCALIBRATIONDLL_EXPORT void AWB_SetRegisterGetFunc(AWBGetAWBRegisterFunc registerFunc);

AWBCALIBRATIONDLL_EXPORT void AWB_SetRegisterValueGetFunc(AWBGetRegisterValue getRegisterValueFunc);

#ifdef __cplusplus
}
#endif

#endif // AWBCALIBRATIONDLL_H
