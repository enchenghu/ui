#ifndef FLASH_PARTITION_KERNEL_H
#define FLASH_PARTITION_KERNEL_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>

typedef struct _kernel_Header
{
    uint32_t magic_number;
    uint32_t version;
    uint32_t data_len;
    uint32_t data_crc;
}KERNEL_Header;

class FlashPartitionKernel : public FlashPartitionIntf
{
public:
    FlashPartitionKernel();
    ~FlashPartitionKernel();

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

    virtual void     setVersion(uint32_t version) { _kernelHead.version = version; }
    virtual uint32_t getVersion() { return _kernelHead.version; }

private:
    //初始化数据头
    void InitHead();
    //初始化数据
    int InitData();

private:
    bool                     _isInit;//是否已经初始化过
    std::string              _fileName;
    std::ifstream            _fileIn;
    KERNEL_Header            _kernelHead;
    std::vector<uint8_t>     _kernelData;
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION_KERNEL_H