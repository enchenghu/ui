#ifndef FLASH_PARTITION_MOVE_DATA_H
#define FLASH_PARTITION_MOVE_DATA_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>

typedef struct _move_Data_Header{
    uint32_t magic_number;
    uint32_t version;
    uint32_t num;
    uint32_t offset[16];
    uint32_t length[16];
    uint16_t crc[16];
}Move_Data_Header;

class FlashPartitionMoveData : public FlashPartitionIntf
{
public:
    FlashPartitionMoveData();
    ~FlashPartitionMoveData();

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

    virtual void     setVersion(uint32_t version) { _head.version = version; }
    virtual uint32_t getVersion() { return _head.version; }

private:
    //初始化数据头
    void InitHead();
    //初始化数据
    int InitData();

private:
    bool                     _isInit;//是否已经初始化过
    std::string              _fileName;
    std::vector<std::string> _fileOffset;
    std::vector<std::string> _filePath;
    std::ifstream            _fileIn;
    Move_Data_Header         _head;
    std::vector<std::vector<uint8_t>> _data;
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION_MOVE_DATA_H