#ifndef AWBCALIBRATIONDLL_GLOBAL_H
#define AWBCALIBRATIONDLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AWBCALIBRATIONDLL_LIBRARY)
#define AWBCALIBRATIONDLL_EXPORT Q_DECL_EXPORT
#else
#define AWBCALIBRATIONDLL_EXPORT Q_DECL_IMPORT
#endif

typedef int (*AWBWriteFunc)(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t writeData);

typedef int (*AWBGetAWBRegisterFunc)(uint32_t awbRegisterCode, uint32_t cameraId, uint32_t* registe, uint32_t* mask,uint8_t* registeNum);

typedef int (*AWBGetRegisterValue)(uint32_t address, uint8_t* pBuf, uint32_t addrLength, uint32_t valueLength);

#endif // AWBCALIBRATIONDLL_GLOBAL_H
