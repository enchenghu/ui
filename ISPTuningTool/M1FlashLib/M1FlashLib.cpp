#include "M1FlashLib.h"
#include "CommonFunction.h"
#include "DeviceCommunication.h"
#include "FlashImgManager.h"

int M1Flash_CreateFlashImg(FlashImg *img)
{
    return FlashImgManager::create(img);
}

int M1Flash_CreateFlashImgFromImgFile(FlashImg *img, const char *path)
{
    return FlashImgManager::create(img, path);
}

int M1Flash_CreateFlashImgFromDevice(FlashImg *img, int deviceIndex)
{
    return FlashImgManager::create(img, deviceIndex);
}

int M1Flash_ReleaseFlashImg(FlashImg img)
{
    return FlashImgManager::release(img);
}

int M1Flash_ReadFlashImg(FlashImg img, const char *path)
{
    return FlashImgManager::execute(img, &FlashImage::readFromImageFile, path);
}

int M1Flash_WriteFlashImg(FlashImg img, const char *path)
{
    return FlashImgManager::execute(img, &FlashImage::writeFlashImg, path);
}

int M1Flash_GetFlashImgPartitionInfo(FlashImg img, PartitionMap *info)
{
    return FlashImgManager::execute(img, &FlashImage::getFlashImgPartitionInfo, info);
}

int M1Flash_SetFlashImgPartitionInfo(FlashImg img, PartitionMap info)
{
    return FlashImgManager::execute(img, &FlashImage::setFlashImgPartitionInfo, info);
}

int M1Flash_AutoAdjustFlashImgPartition(FlashImg img)
{
    return FlashImgManager::execute(img, &FlashImage::autoAdjustFlashImgPartition);
}

int M1Flash_SetFlashImgPartitionOrder(FlashImg img, PartitionMap orderMap)
{
    return FlashImgManager::execute(img, &FlashImage::setPartitionOrder, orderMap);
}

int M1Flash_SetFlashImgPartitionDataFromCfgFile(FlashImg img, PartitionType type, const char *path)
{
    return FlashImgManager::execute(img, &FlashImage::setPartitionDataFromCfgFile, type, path);
}

int M1Flash_GetFlashImgPartitionDataToCfgFile(FlashImg img, PartitionType type, const char *path)
{
    return FlashImgManager::execute(img, &FlashImage::getPartitionDataToCfgFile, type, path);
}

int M1Flash_RemoveFlashImgPartition(FlashImg img, PartitionType type)
{
    return FlashImgManager::execute(img, &FlashImage::removePartition, type);
}

int M1Flash_SetSpiReadWriteFunc(SpiReadFunc readFunc, SpiWriteFunc writeFunc)
{
    return DeviceCommunication::setSpiReadWriteFunc(readFunc, writeFunc);
}

int M1Flash_BurnFlash(FlashImg img, PartitionType partitionMask, int deviceIndex)
{
    if (img == nullptr)
        return ERROR_PARAM_NULL;
    std::vector<uint8_t> flashData;
    auto res = FlashImgManager::execute(img, &FlashImage::getFlashData, std::ref(flashData), partitionMask);
    if (res != ERROR_NO_ERR)
        return res;
    return DeviceCommunication::burnFlash(deviceIndex, flashData);
}

M1_FLASH_EXPORT int M1Flash_EraseFlash(int deviceIndex)
{
    return DeviceCommunication::eraseFlash(deviceIndex);
}
