#ifndef FLASH_IMAGE_H
#define FLASH_IMAGE_H

#include "FlashImageHeader.h"
#include "FlashPartitionInterface.h"
#include "M1FlashTypeDef.h"

class FlashImage
{
public:
    FlashImage();
    int readFromImageFile(const char *path);
    int readFromDevice(int deviceIndex);
    int writeFlashImg(const char *path);
    int getFlashImgPartitionInfo(PartitionMap *info);
    int setFlashImgPartitionInfo(PartitionMap info);
    int setPartitionOrder(PartitionMap orderMap);
    int getPartitionOrder(std::vector<PartitionType> &order);
    int autoAdjustFlashImgPartition();
    int setPartitionDataFromCfgFile(PartitionType type, const char *path);
    int getPartitionDataToCfgFile(PartitionType type, const char *path);
    int getFlashData(std::vector<uint8_t> &flashData, PartitionType partitionMask = PART_ALL);
    int removePartition(PartitionType type);

private:
    FlashImageHeader                                    _header;
    std::vector<std::pair<uint32_t, FlashPartitionPtr>> _partitions;
    std::vector<PartitionType>                          _partitionOrder;

    int checkPartition();
    int checkPartition(std::vector<std::pair<uint32_t, FlashPartitionPtr>> partitions);
    int readFromImageData(std::vector<uint8_t> &data);
};

#endif // FLASH_IMAGE_H