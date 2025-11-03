#include "FlashImage.h"
#include "CommonFunction.h"
#include "DeviceCommunication.h"
#include <algorithm>
#include <fstream>

FlashImage::FlashImage()
    : _partitionOrder({ PART_DDR_SETTING, PART_GENERAL_HW_SETTING, PART_FW_SETTING, PART_FW_RECOVER,
        PART_SENSOR_SETTING, PART_FW_PATCH, PART_DCPC, PART_OSD, PART_ARM_BOOT, PART_LINUX_KERNEL, PART_LINUX_ROOTFS,
        PART_NONE, PART_MOVE_DATA, PART_NONE })
{
}

int FlashImage::readFromImageFile(const char *path)
{
    if (path == nullptr) {
        return ERROR_PARAM_NULL;
    }
    if (!isFileExist(path)) {
        return ERROR_PATH_NOT_EXISTS;
    }
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        return ERROR_IMG_FILE_READ;
    }
    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (fileSize < _header.getPartitionSize()) {
        return ERROR_IMG_FILE_READ;
    }
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
    return readFromImageData(buffer);
}

int FlashImage::readFromDevice(int deviceIndex)
{
    const uint32_t       PAGE_SIZE      = 256;
    const uint32_t       MAX_FLASH_SIZE = 0x1000000;
    std::vector<uint8_t> data;
    std::vector<uint8_t> zeroPage(PAGE_SIZE, 0);
    std::vector<uint8_t> erasedPage(PAGE_SIZE, 0xff);
    for (uint32_t offset = 0; offset < MAX_FLASH_SIZE; offset += PAGE_SIZE) {
        std::vector<uint8_t> page(0);
        auto                 res = DeviceCommunication::readFlash(deviceIndex, offset, PAGE_SIZE, page);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (page == zeroPage || page == erasedPage) {
            break;
        }
        data.insert(data.end(), page.begin(), page.end());
    }
    return readFromImageData(data);
}

int FlashImage::writeFlashImg(const char *path)
{
    if (path == nullptr) {
        return ERROR_PARAM_NULL;
    }
    std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_IMG_FILE_WRITE;
    }
    std::vector<uint8_t> flashData;
    auto                 res = getFlashData(flashData, PART_ALL);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    file.write(reinterpret_cast<char *>(flashData.data()), flashData.size());
    if (!file.good()) {
        return ERROR_IMG_FILE_WRITE;
    }

    return ERROR_NO_ERR;
}

int FlashImage::getFlashImgPartitionInfo(PartitionMap *info)
{
    if (info == nullptr)
        return ERROR_PARAM_NULL;
    info->count = 0;
    for (auto &partition : _partitions) {
        info->partitions[info->count].type       = partition.second->getPartitionType();
        info->partitions[info->count].size       = partition.second->getPartitionSize();
        info->partitions[info->count].dataLength = partition.second->getPartitionDataSize();
        info->offsets[info->count]               = partition.first;
        info->count++;
    }
    return ERROR_NO_ERR;
}

int FlashImage::setFlashImgPartitionInfo(PartitionMap info)
{
    std::vector<std::pair<uint32_t, FlashPartitionPtr>> tmpPartitions;
    for (auto &partition : _partitions) {
        tmpPartitions.push_back({ partition.first, partition.second->copy() });
    }
    for (int i = 0; i < info.count; i++) {
        auto &partitionInfo = info.partitions[i];
        auto  offset        = info.offsets[i];
        auto  type          = partitionInfo.type;
        int   index         = -1;
        for (int i = 0; i < tmpPartitions.size(); i++) {
            if (tmpPartitions[i].second->getPartitionType() == type) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            auto partition = FlashPartitionFactory::create(type);
            if (partition == nullptr) {
                return ERROR_PARTITION_TYPE_INVALID;
            }
            tmpPartitions.push_back(std::make_pair(offset, partition));
            index = static_cast<int>(tmpPartitions.size() - 1);
        }
        auto &partition = tmpPartitions[index];
        partition.first = offset;
        partition.second->setPartitionInfo(partitionInfo);
    }
    auto res = checkPartition(tmpPartitions);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    _partitions.swap(tmpPartitions);
    return ERROR_NO_ERR;
}

int FlashImage::setPartitionOrder(PartitionMap orderMap)
{
    _partitionOrder.clear();
    for (int i = 0; i < orderMap.count; i++) {
        _partitionOrder.push_back(orderMap.partitions[i].type);
    }
    return ERROR_NO_ERR;
}

int FlashImage::autoAdjustFlashImgPartition()
{
    // sort partitions by _partitionOrder
    std::sort(_partitions.begin(), _partitions.end(), [this](auto &a, auto &b) {
        auto aType = a.second->getPartitionType();
        auto bType = b.second->getPartitionType();
        auto aPos  = std::find(_partitionOrder.begin(), _partitionOrder.end(), aType);
        auto bPos  = std::find(_partitionOrder.begin(), _partitionOrder.end(), bType);
        return aPos < bPos;
    });
    uint32_t offset = 0;
    for (auto &partition : _partitions) {
        partition.first = offset;
        offset += partition.second->getPartitionSize();
        if(offset % 4 != 0)
            offset += 4 - offset % 4;
    }
    return ERROR_NO_ERR;
}

int FlashImage::setPartitionDataFromCfgFile(PartitionType type, const char *path)
{
    if (type == PART_ALL) {
        auto res = _header.readPartitionDataFromCfgFile(path);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        for (auto &partition : _partitions) {
            partition.second->setVersion(_header.getVersion());
        }
        return ERROR_NO_ERR;
    }
    for (auto &partition : _partitions) {
        if (partition.second->getPartitionType() == type) {
            auto res = partition.second->readPartitionDataFromCfgFile(path);
            if (res != ERROR_NO_ERR) {
                return res;
            }
            partition.second->setVersion(_header.getVersion());
            return res;
        }
    }
    auto partition = FlashPartitionFactory::create(type);
    if (partition == nullptr) {
        return ERROR_PARTITION_TYPE_INVALID;
    }
    auto res = partition->readPartitionDataFromCfgFile(path);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    _partitions.push_back(std::make_pair(0, partition));
    return res;
}

int FlashImage::getPartitionDataToCfgFile(PartitionType type, const char *path)
{
    if (type == PART_ALL) {
        return _header.writePartitionDataToCfgFile(path);
    }

    for (auto &partition : _partitions) {
        if (partition.second->getPartitionType() == type) {
            return partition.second->writePartitionDataToCfgFile(path);
        }
    }
    return ERROR_PARTITION_NOT_FOUND;
}

int FlashImage::getFlashData(std::vector<uint8_t> &flashData, PartitionType partitionMask)
{
    if (partitionMask > PART_ALL) {
        return ERROR_PARTITION_TYPE_INVALID;
    }
    // get partitions by mask
    std::vector<std::pair<uint32_t, FlashPartitionPtr>> partitions;
    for (auto &partition : _partitions) {
        if (partition.second->getPartitionType() & partitionMask) {
            partitions.push_back(partition);
        }
    }
    auto res = checkPartition(partitions);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    flashData.clear();
    // calculate total size
    uint32_t totalSize = 0;
    for (auto &partition : partitions) {
        totalSize = std::max(totalSize, partition.first + partition.second->getPartitionSize());
    }
    totalSize += _header.getPartitionSize();
    std::vector<uint8_t>(totalSize, MAGIC_ENCODER).swap(flashData);
    _header.clearPartitionSetting();
    // copy partition data
    for (auto &partition : partitions) {
        auto partHeader = partition.second->getPartitionHeaderEncrypted();
        auto partData   = partition.second->getPartitionDataEncrypted();
        memcpy(flashData.data() + partition.first + _header.getPartitionSize(), partHeader.data(), partHeader.size());
        memcpy(flashData.data() + partition.first + _header.getPartitionSize() + partHeader.size(), partData.data(),
            partData.size());
        _header.setPartitionSetting(partition.second->getPartitionType(), partition.first + _header.getPartitionSize(),
            partHeader);
    }
    // copy header
    auto flashSetting     = _header.getPartitionHeaderEncrypted();
    auto flashImageHeader = _header.getPartitionDataEncrypted();
    memcpy(flashData.data(), flashSetting.data(), flashSetting.size());
    memcpy(flashData.data() + flashSetting.size(), flashImageHeader.data(), flashImageHeader.size());
    return ERROR_NO_ERR;
}

int FlashImage::removePartition(PartitionType type)
{
    for (auto it = _partitions.begin(); it != _partitions.end(); it++) {
        if (it->second->getPartitionType() == type) {
            _partitions.erase(it);
            return ERROR_NO_ERR;
        }
    }
    return ERROR_PARTITION_NOT_FOUND;
}

int FlashImage::checkPartition()
{
    return checkPartition(_partitions);
}

int FlashImage::checkPartition(std::vector<std::pair<uint32_t, FlashPartitionPtr>> partitions)
{
    for (int i = 0; i < partitions.size(); i++) {
        for (int j = i + 1; j < partitions.size(); j++) {
            auto &partition = partitions[i];
            auto &other     = partitions[j];
            if (partition.first < other.first) {
                if (partition.first + partition.second->getPartitionSize() > other.first) {
                    return ERROR_PARTITION_OVERLAP;
                }
            } else {
                if (other.first + other.second->getPartitionSize() > partition.first) {
                    return ERROR_PARTITION_OVERLAP;
                }
            }
        }
    }
    return ERROR_NO_ERR;
}

int FlashImage::readFromImageData(std::vector<uint8_t> &data)
{
    auto res = _header.setPartitionDataFromEncryptedData(data);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    PartitionMap map;
    res = _header.getPartitionMap(map);
    if (res != ERROR_NO_ERR) {
        return res;
    }
    for (int i = 0; i < map.count; i++) {
        auto &info   = map.partitions[i];
        auto  offset = map.offsets[i] - _header.getPartitionSize();
        auto  type   = info.type;
        int   index  = -1;
        for (int i = 0; i < _partitions.size(); i++) {
            if (_partitions[i].second->getPartitionType() == type) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            auto partition = FlashPartitionFactory::create(type);
            if (partition == nullptr) {
                return ERROR_PARTITION_TYPE_INVALID;
            }
            partition->setPartitionInfo(info);
            _partitions.push_back(std::make_pair(offset, partition));
            index = static_cast<int>(_partitions.size() - 1);
        }
        auto                &partition = _partitions[index];
        std::vector<uint8_t> partData(data.size() - offset - _header.getPartitionSize());
        memcpy(partData.data(), data.data() + offset + _header.getPartitionSize(), partData.size());
        res = partition.second->setPartitionDataFromEncryptedData(partData);
        if (res != ERROR_NO_ERR) {
            return res;
        }
        if (_header.getVersion() != partition.second->getVersion()) {
            return ERROR_IMG_FILE_READ;
        }
    }
    return ERROR_NO_ERR;
}