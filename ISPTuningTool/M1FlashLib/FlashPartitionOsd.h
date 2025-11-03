#ifndef FLASH_PARTITION_OSD_H
#define FLASH_PARTITION_OSD_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>
#include "ini.h"

typedef struct _osd_Header{
    uint32_t magic_number;
    uint32_t version;
    uint8_t frame_num;
    uint8_t work_mode;
    uint8_t snr0_cfg_id;
    uint8_t snr1_cfg_id;
}OSD_Header;

typedef struct _frame_Header_In{
    uint32_t reserve;
    uint32_t length[8];
    uint32_t coord[8];
    uint8_t sram_length[8];
    uint32_t default_switch;
    uint8_t color_lut[92];
    uint32_t data;
}Frame_Header_In;

typedef struct _frame_Data_Header{
    uint32_t LyrSwitch;
    uint32_t LyrAddr[8];
    uint32_t LyrLen[8];
    uint32_t LyrCoord[8];
    uint8_t SramLen[8];
    uint32_t ColorLut[30];
    uint32_t crc[8];
}Frame_Data_Header;

typedef struct _frame_Data{
    uint32_t offset;
    uint32_t length;
    uint32_t crc;
    uint8_t* data;
}Frame_Data;

typedef struct _lyr_Data{
    uint32_t length;
    uint8_t* data;
}Layer_Data;

class FlashPartitionOsd : public FlashPartitionIntf
{
public:
    FlashPartitionOsd();
    ~FlashPartitionOsd();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);
    //写入数据
    virtual int writeDataToCfgFile(const char *path);

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
    std::ifstream            _fileIn;
    OSD_Header               _head;//整个数据头
    std::vector<Frame_Data_Header>  _dataHead; // 包含子头
    std::vector<std::vector<std::vector<uint8_t>>> _onlyData;  //纯子数据
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION_OSD_H