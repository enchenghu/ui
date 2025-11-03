#ifndef FLASH_IMAGE_HEADR_H
#define FLASH_IMAGE_HEADR_H

#include "CommonDefines.h"
#include "FlashPartitionInterface.h"
#include "M1FlashTypeDef.h"
#include <map>
#include <vector>

struct FlashSettingStruct
{
    uint32_t magic = MAGIC_NUM_FLASH_SETTING;
    uint8_t  validFlag;
    uint8_t  readCmd;
    uint8_t  writeCmd;
    uint8_t  dummyCycle;
    uint8_t  readDataWrieProto : 4;
    uint8_t  readAddressWrieProto : 4;
    uint8_t  writeDataWrieProto : 4; // 因使用位域，顺序与定义相反
    uint8_t  writeAddressWrieProto : 4;
    uint8_t  clkDiv;
    uint8_t  reserved;
    uint32_t crc;
};

struct FlashImageHeaderStructHdr
{
    uint32_t magic = MAGIC_NUM_IMAGE_HEADER;
    uint32_t crc;
};
struct PartitionSetting
{
    uint32_t offset;
    uint32_t size;
    uint32_t crc;
};

struct PLLInfoStruct
{
    uint8_t  pll_sw_flg;
    uint8_t  sw_dly_time; // us
    uint8_t  pll_cfg[6];
    uint8_t  dly;
    uint8_t  reserve[3];
    uint32_t sys_clk;
    uint32_t xtal_clk;
};

struct FlashImageHeaderStructData
{
    uint32_t         headerLength;
    uint32_t         version;
    uint64_t         errExit;
    uint8_t          uartPrtEn;
    uint8_t          flash2DdrCheck;
    uint8_t          pipelineRst;
    uint8_t          wdtTimeEdge;
    uint8_t          reserve[8];
    PartitionSetting partitionSetting[14];
    PLLInfoStruct    pllInfo;
};

struct FlashImageHeaderStruct
{
    FlashImageHeaderStructHdr  hdr;
    FlashImageHeaderStructData data;
};

class FlashImageHeader : public FlashPartitionIntf
{
public:
    FlashImageHeader();
    virtual std::vector<uint8_t> getPartitionDataEncrypted() override;
    virtual std::vector<uint8_t> getPartitionDataDecrypted() override;

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted() override;
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted() override;

    virtual PartitionInfo getPartitionInfo() override;
    virtual int           setPartitionInfo(PartitionInfo info) override;
    PartitionInfo         getPartitionInfoByType(PartitionType type);
    int                   setPartitionMap(PartitionMap map);
    int                   getPartitionMap(PartitionMap &map);

    virtual int readPartitionDataFromCfgFile(const char *path) override;
    virtual int writePartitionDataToCfgFile(const char *path) override;

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data) override;

    virtual void     setVersion(uint32_t version) override;
    virtual uint32_t getVersion() override;

    virtual PartitionType getPartitionType() override;
    virtual uint32_t      getPartitionSize() override;
    virtual uint32_t      getPartitionDataSize() override;

    int clearPartitionSetting();
    int setPartitionSetting(PartitionType type, uint32_t offset, const std::vector<uint8_t> encryptedPartitionHeader);

private:
    FlashSettingStruct                 _flashSetting;
    FlashImageHeaderStruct             _flashImageHeader;
    const std::map<PartitionType, int> PARTITION_MAP = {
        { PartitionType::PART_DDR_SETTING, 0 },
        { PartitionType::PART_GENERAL_HW_SETTING, 1 },
        { PartitionType::PART_FW_SETTING, 2 },
        { PartitionType::PART_FW_RECOVER, 3 },
        { PartitionType::PART_SENSOR_SETTING, 4 },
        { PartitionType::PART_FW_PATCH, 5 },
        { PartitionType::PART_DCPC, 6 },
        { PartitionType::PART_OSD, 7 },
        { PartitionType::PART_ARM_BOOT, 8 },
        { PartitionType::PART_LINUX_KERNEL, 9 },
        { PartitionType::PART_LINUX_ROOTFS, 10 },
        { PartitionType::PART_MOVE_DATA, 12 },
    };

    int getCrcOfHeader();
};

#endif // FLASH_IMAGE_HEADR_H