#ifndef FLASH_PARTITION_DDR_SETTING_H
#define FLASH_PARTITION_DDR_SETTING_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>


typedef struct _ddr_Setting{
    uint32_t addr;
    uint32_t data;
    uint8_t high_bit;
    uint8_t low_bit;
    uint8_t rd_flag;
    uint8_t reserve;
}DDR_Setting;

typedef struct _setting_Header
{
    uint32_t magic_number;
    uint32_t version;
    uint16_t data_len;
    uint16_t data_crc;
}Setting_Header;

typedef struct __tagPartationInfo
{
	std::string  part_key_name;
	unsigned int part_len;
	char* pData;
}s_Part_Info;

class FlashPartitionDdrSetting : public FlashPartitionIntf
{
public:
    FlashPartitionDdrSetting();
    ~FlashPartitionDdrSetting();

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

    virtual void     setVersion(uint32_t version) { _ddrHead.version = version; }
    virtual uint32_t getVersion() { return _ddrHead.version; }

private:
    //初始化数据头
    void InitHead();
    
private:
    bool                     _isInit;//是否已经初始化过
    Setting_Header           _ddrHead;
    std::vector<DDR_Setting> _ddrData;
    PartitionInfo            _info;
};

#endif //FLASH_PARTITION__DDR_SETTING_H