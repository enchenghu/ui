#ifndef FLASH_PARTITION_DCPC_H
#define FLASH_PARTITION_DCPC_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>

typedef struct __dcpc_Header{
    uint32_t magic_number;
    uint32_t version;
    uint32_t total_len;
    uint8_t dcpc_num;
    uint8_t dcpc_bino;
    uint8_t reserved[2];
    uint32_t dcpc_lut0_st_addr;
    uint32_t dcpc_lut1_st_addr;
    uint16_t lut0_crc;
    uint16_t lut1_crc;
}DCPC_Header;

typedef struct _dcpc_Lut_header{
    uint8_t index;
    uint8_t reserve;
    uint16_t length;
    uint16_t img_width;
    uint16_t img_height;
    uint16_t hts;
    uint16_t vts;
    uint8_t lut_width;
    uint8_t lut_height;
    uint8_t data_type;
    uint8_t sv_sel;
    uint16_t line_cnt;
    uint16_t x_off;
    uint16_t y_off;
    uint16_t crc;
    uint32_t addr_even;
    uint32_t addr_odd;
    uint32_t lut_addr;
    uint8_t ymink[120];
}DCPC_Lut_Header;

class FlashPartitionDcpc : public FlashPartitionIntf
{
public:
    FlashPartitionDcpc();
    ~FlashPartitionDcpc();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data);

    virtual void     setVersion(uint32_t version) { _dcpcHead.version = version; }
    virtual uint32_t getVersion() { return _dcpcHead.version; }

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
    DCPC_Header              _dcpcHead;
    DCPC_Lut_Header          _lutHeadFirst;
    DCPC_Lut_Header          _lutHeadSecond;
    std::vector<uint8_t>     _dataFirst;
    std::vector<uint8_t>     _dataSecond;
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION_DCPC_H