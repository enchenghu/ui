#ifndef FLASH_PARTITION_HW_FW_H
#define FLASH_PARTITION_HW_FW_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>

typedef struct _hw_fw_Header{
    uint32_t magic_number;
    uint32_t version;
    uint32_t num;
}HW_FW_Header;

typedef struct _hw_fw_Pair{
    uint16_t data_len;
    uint16_t data_crc;
}HW_FW_Pair;

class FlashPartitionHwFwBase : public FlashPartitionIntf
{
public:
    ~FlashPartitionHwFwBase();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data);

    virtual PartitionType getPartitionType();
    virtual uint32_t      getPartitionSize();
    virtual uint32_t      getPartitionDataSize();

    virtual void     setVersion(uint32_t version) { _hwHead.version = version; }
    virtual uint32_t getVersion() { return _hwHead.version; }

protected:
    FlashPartitionHwFwBase();
    PartitionInfo _info;
    HW_FW_Header  _hwHead;

    //解析数据
    virtual int analysisData(const std::vector<uint8_t> vecData, std::string &allStr);

private:
    //初始化数据头
    void InitHead();
    //初始化数据
    int InitData();

private:
    bool                     _isInit;//是否已经初始化过
    std::string              _fileName;
    std::ifstream            _fileIn;
    std::vector<HW_FW_Pair>           _subHead;
    std::vector<std::vector<uint8_t>> _hwData;
};

class FlashPartitionHwSetting : public FlashPartitionHwFwBase
{
public:
    FlashPartitionHwSetting()
        : FlashPartitionHwFwBase()
    {
        _info.type           = PART_GENERAL_HW_SETTING;
        _hwHead.magic_number = MAGIC_NUM_HARDWARE_SETTING;
    }
};

class FlashPartitionFwSetting : public FlashPartitionHwFwBase
{
public:
    FlashPartitionFwSetting()
        : FlashPartitionHwFwBase()
    {
        _info.type           = PART_FW_SETTING;
        _hwHead.magic_number = MAGIC_NUM_FIRMWARE_SETTING;
    }
};

class FlashPartitionFirmwareRecover : public FlashPartitionHwFwBase
{
public:
    FlashPartitionFirmwareRecover()
        : FlashPartitionHwFwBase()
    {
        _info.type           = PART_FW_RECOVER;
        _hwHead.magic_number = MAGIC_NUM_FIRMWARE_RECOVER;
    }
};

#endif // FLASH_PARTITION_HW_FW_H