#ifndef M1_FLASH_LIB_H
#define M1_FLASH_LIB_H

#if defined(_WIN32) || defined(_WIN64)
#  ifdef M1_FLASH_LIBRARY
#    define M1_FLASH_EXPORT __declspec(dllexport)
#  else
#    define M1_FLASH_EXPORT __declspec(dllimport)
#  endif
#endif

#include "M1FlashTypeDef.h"

#ifdef __cplusplus
extern "C" {
#endif

M1_FLASH_EXPORT int M1Flash_CreateFlashImg(FlashImg *img);
M1_FLASH_EXPORT int M1Flash_CreateFlashImgFromImgFile(FlashImg *img, const char *path);
M1_FLASH_EXPORT int M1Flash_CreateFlashImgFromDevice(FlashImg *img, int deviceIndex);
M1_FLASH_EXPORT int M1Flash_ReleaseFlashImg(FlashImg img);
M1_FLASH_EXPORT int M1Flash_ReadFlashImg(FlashImg img, const char *path);
M1_FLASH_EXPORT int M1Flash_WriteFlashImg(FlashImg img, const char *path);
M1_FLASH_EXPORT int M1Flash_GetFlashImgPartitionInfo(FlashImg img, PartitionMap *info);
M1_FLASH_EXPORT int M1Flash_SetFlashImgPartitionInfo(FlashImg img, PartitionMap info);
M1_FLASH_EXPORT int M1Flash_AutoAdjustFlashImgPartition(FlashImg img);
M1_FLASH_EXPORT int M1Flash_SetFlashImgPartitionDataFromCfgFile(FlashImg img, PartitionType type, const char *path);
M1_FLASH_EXPORT int M1Flash_GetFlashImgPartitionDataToCfgFile(FlashImg img, PartitionType type, const char *path);
M1_FLASH_EXPORT int M1Flash_RemoveFlashImgPartition(FlashImg img, PartitionType type);
M1_FLASH_EXPORT int M1Flash_SetSpiReadWriteFunc(SpiReadFunc readFunc, SpiWriteFunc writeFunc);
M1_FLASH_EXPORT int M1Flash_BurnFlash(FlashImg img, PartitionType partitionMask, int deviceIndex);
M1_FLASH_EXPORT int M1Flash_EraseFlash(int deviceIndex);

#ifdef __cplusplus
}
#endif

#endif // M1_FLASH_LIB_H