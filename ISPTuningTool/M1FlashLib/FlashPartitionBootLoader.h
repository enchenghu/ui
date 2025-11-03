#ifndef FLASH_PARTITION_BOOT_LOADER_H
#define FLASH_PARTITION_BOOT_LOADER_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>

typedef struct _boot_Loader_Header
{
    uint32_t magic_number;
    uint32_t version;
    uint8_t  mbist_en;
    uint8_t  rsv[3];
    uint32_t data_len;
    uint32_t data_crc;
}BOOT_LOADER_Header;

class FlashPartitionBootLoader : public FlashPartitionIntf
{
public:
    FlashPartitionBootLoader();
    ~FlashPartitionBootLoader();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data);

    virtual void     setVersion(uint32_t version) { _bootHead.version = version; }
    virtual uint32_t getVersion() { return _bootHead.version; }

    virtual PartitionType getPartitionType();
    virtual uint32_t      getPartitionSize();
    virtual uint32_t      getPartitionDataSize();

private:
    //初始化数据头
    void InitHead();
    //初始化数据
    int InitData();

private:
    bool                     _isInit;//是否已经初始化过
    std::string              _fileName;
    std::ifstream            _fileIn;
    BOOT_LOADER_Header       _bootHead;
    std::vector<uint8_t>     _bootData;
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION_BOOT_LOADER_H