#include "FlashImageHeader.h"
#include "CommonFunction.h"
#include "ini.h"

FlashImageHeader::FlashImageHeader()
{
    memset(&_flashSetting, 0, sizeof(FlashSettingStruct));
    memset(&_flashImageHeader, 0, sizeof(FlashImageHeaderStruct));
    _flashSetting.magic                 = MAGIC_NUM_FLASH_SETTING;
    _flashImageHeader.hdr.magic         = MAGIC_NUM_IMAGE_HEADER;
    _flashImageHeader.data.headerLength = sizeof(FlashImageHeaderStructData);
}

std::vector<uint8_t> FlashImageHeader::getPartitionDataEncrypted()
{
    std::vector<uint8_t> data(sizeof(FlashImageHeaderStruct));
    memcpy(data.data(), &_flashImageHeader, sizeof(FlashImageHeaderStruct));
    return getCopyXor(data);
}

std::vector<uint8_t> FlashImageHeader::getPartitionDataDecrypted()
{
    std::vector<uint8_t> data(sizeof(FlashImageHeaderStruct));
    memcpy(data.data(), &_flashImageHeader, sizeof(FlashImageHeaderStruct));
    return data;
}

std::vector<uint8_t> FlashImageHeader::getPartitionHeaderEncrypted()
{
    std::vector<uint8_t> data(sizeof(FlashSettingStruct));
    _flashSetting.crc =
        getOnlyCrc(reinterpret_cast<char *>(&_flashSetting), sizeof(FlashSettingStruct) - sizeof(uint32_t));
    memcpy(data.data(), &_flashSetting, sizeof(FlashSettingStruct));
    return getCopyXor(data);
}

std::vector<uint8_t> FlashImageHeader::getPartitionHeaderDecrypted()
{
    std::vector<uint8_t> data(sizeof(FlashImageHeaderStructHdr));
    _flashSetting.crc =
        getOnlyCrc(reinterpret_cast<char *>(&_flashSetting), sizeof(FlashSettingStruct) - sizeof(uint32_t));
    memcpy(data.data(), &_flashSetting, sizeof(FlashSettingStruct));
    return data;
}

PartitionInfo FlashImageHeader::getPartitionInfo()
{
    PartitionInfo info;
    info.type       = PartitionType::PART_ALL;
    info.size       = sizeof(FlashImageHeaderStruct);
    info.dataLength = sizeof(FlashImageHeaderStruct);
    return info;
}

int FlashImageHeader::setPartitionInfo(PartitionInfo info)
{
    return ERROR_NO_ERR;
}

PartitionInfo FlashImageHeader::getPartitionInfoByType(PartitionType type)
{
    PartitionInfo info = { PartitionType::PART_ALL, sizeof(FlashImageHeaderStruct), sizeof(FlashImageHeaderStruct) };
    if (PARTITION_MAP.find(type) == PARTITION_MAP.end()) {
        return info;
    }
    auto partitionSetting = _flashImageHeader.data.partitionSetting[PARTITION_MAP.at(type)];
    info.type             = type;
    info.size             = partitionSetting.size;
    info.dataLength       = 0;
    return info;
}

int FlashImageHeader::setPartitionMap(PartitionMap map)
{
    // clear partition setting
    for (int i = 0; i < PARTITION_MAP.size(); i++) {
        _flashImageHeader.data.partitionSetting[i].size   = 0;
        _flashImageHeader.data.partitionSetting[i].offset = 0;
    }
    for (int i = 0; i < map.count; i++) {
        auto &partitionSetting  = _flashImageHeader.data.partitionSetting[PARTITION_MAP.at(map.partitions[i].type)];
        partitionSetting.size   = map.partitions[i].size;
        partitionSetting.offset = map.offsets[i];
    }
    return ERROR_NO_ERR;
}

int FlashImageHeader::getPartitionMap(PartitionMap &map)
{
    map.count = 0;
    for (auto &partition : PARTITION_MAP) {
        if (_flashImageHeader.data.partitionSetting[partition.second].size != 0) {
            map.partitions[map.count].type       = partition.first;
            map.partitions[map.count].size       = _flashImageHeader.data.partitionSetting[partition.second].size;
            map.partitions[map.count].dataLength = 0;
            map.offsets[map.count]               = _flashImageHeader.data.partitionSetting[partition.second].offset;
            map.count++;
        }
    }

    return ERROR_NO_ERR;
}

int FlashImageHeader::readPartitionDataFromCfgFile(const char *path)
{
    if (path == nullptr) {
        return ERROR_PARAM_NULL;
    }
    if (!isFileExist(path)) {
        return ERROR_PATH_NOT_EXISTS;
    }
    std::string        strPath = path;
    mINI::INIFile      file(strPath);
    mINI::INIStructure ini;
    file.read(ini);

    _flashSetting.validFlag             = str2Num<uint8_t>(ini["flash"]["valid"], 10);
    _flashSetting.readCmd               = str2Num<uint8_t>(ini["flash"]["read_cmd"], 16);
    _flashSetting.writeCmd              = str2Num<uint8_t>(ini["flash"]["write_cmd"], 16);
    _flashSetting.dummyCycle            = str2Num<uint8_t>(ini["flash"]["dummy_cycle"]);
    _flashSetting.readDataWrieProto     = str2Num<uint8_t>(ini["flash"]["read_data_wire"]);
    _flashSetting.readAddressWrieProto  = str2Num<uint8_t>(ini["flash"]["read_addr_wire"]);
    _flashSetting.writeDataWrieProto    = str2Num<uint8_t>(ini["flash"]["write_data_wire"]);
    _flashSetting.writeAddressWrieProto = str2Num<uint8_t>(ini["flash"]["write_addr_wire"]);
    _flashSetting.clkDiv                = str2Num<uint8_t>(ini["flash"]["div_val"]);
    _flashSetting.crc =
        getOnlyCrc(reinterpret_cast<char *>(&_flashSetting), sizeof(FlashSettingStruct) - sizeof(uint32_t));

    _flashImageHeader.data.headerLength   = sizeof(FlashImageHeaderStructData);
    _flashImageHeader.data.errExit        = str2Num<uint64_t>(ini["reserve"]["err_bypass"], 16);
    _flashImageHeader.data.uartPrtEn      = str2Num<uint8_t>(ini["reserve"]["uart_prt_en"]);
    _flashImageHeader.data.flash2DdrCheck = str2Num<uint8_t>(ini["reserve"]["ddr_check"]);
    _flashImageHeader.data.pipelineRst    = str2Num<uint8_t>(ini["reserve"]["pipeline_rst"]);
    _flashImageHeader.data.wdtTimeEdge    = str2Num<uint8_t>(ini["reserve"]["wdt_time_edge"]);

    auto versionString = ini["reserve"]["version"];
    if (versionString.empty()) {
        _flashImageHeader.data.version = 0;
    } else {
        uint32_t version = 0;
        versionString    = Trimstring(versionString);
        while (versionString != "") {
            version = version << 8;
            version += stoi(versionString);
            size_t pos;
            if (versionString.npos != (pos = versionString.find('.'))) {
                versionString = versionString.substr(pos + 1);
                Trimstring(versionString);
            } else
                break;
        }
        _flashImageHeader.data.version = version;
    }

    _flashImageHeader.data.pllInfo.pll_sw_flg  = str2Num<uint8_t>(ini["pll"]["switch"], 16);
    _flashImageHeader.data.pllInfo.sys_clk     = str2Num<uint32_t>(ini["pll"]["clk"]) * 1000000;
    _flashImageHeader.data.pllInfo.xtal_clk    = str2Num<uint32_t>(ini["pll"]["xclk"]) * 1000000;
    _flashImageHeader.data.pllInfo.dly         = str2Num<uint8_t>(ini["pll"]["dly"]);
    _flashImageHeader.data.pllInfo.sw_dly_time = str2Num<uint8_t>(ini["pll"]["sw_dly_time"]);
    for (int i = 0; i < 6; i++) {
        _flashImageHeader.data.pllInfo.pll_cfg[i] = str2Num<uint8_t>(ini["pll"]["reg" + std::to_string(i)], 16);
    }

    _flashImageHeader.hdr.crc = getCrcOfHeader();
    return 0;
}

int FlashImageHeader::writePartitionDataToCfgFile(const char *path)
{
    if (path == nullptr) {
        return ERROR_PARAM_NULL;
    }
    if (std::ofstream(path, std::ios_base::out | std::ios_base::trunc).is_open() == false) {
        return ERROR_PATH_INVALID;
    }
    std::string        strPath = path;
    mINI::INIFile      file(strPath);
    mINI::INIStructure ini;

    ini["flash"]["valid"]           = num2Str(_flashSetting.validFlag);
    ini["flash"]["read_cmd"]        = num2Str(_flashSetting.readCmd, 16, 2, '0', "0x");
    ini["flash"]["write_cmd"]       = num2Str(_flashSetting.writeCmd, 16, 2, '0', "0x");
    ini["flash"]["dummy_cycle"]     = num2Str(_flashSetting.dummyCycle);
    ini["flash"]["read_data_wire"]  = num2Str(_flashSetting.readDataWrieProto);
    ini["flash"]["read_addr_wire"]  = num2Str(_flashSetting.readAddressWrieProto);
    ini["flash"]["write_data_wire"] = num2Str(_flashSetting.writeDataWrieProto);
    ini["flash"]["write_addr_wire"] = num2Str(_flashSetting.writeAddressWrieProto);
    ini["flash"]["div_val"]         = num2Str(_flashSetting.clkDiv);

    ini["reserve"]["err_bypass"]    = num2Str(_flashImageHeader.data.errExit, 16, 16, '0', "0x");
    ini["reserve"]["uart_prt_en"]   = num2Str(_flashImageHeader.data.uartPrtEn);
    ini["reserve"]["ddr_check"]     = num2Str(_flashImageHeader.data.flash2DdrCheck);
    ini["reserve"]["pipeline_rst"]  = num2Str(_flashImageHeader.data.pipelineRst);
    ini["reserve"]["wdt_time_edge"] = num2Str(_flashImageHeader.data.wdtTimeEdge);

    ini["reserve"]["version"] = std::to_string(_flashImageHeader.data.version >> 24) + "."
        + std::to_string((_flashImageHeader.data.version >> 16) & 0xff) + "."
        + std::to_string((_flashImageHeader.data.version >> 8) & 0xff) + "."
        + std::to_string(_flashImageHeader.data.version & 0xff);

    ini["pll"]["switch"]      = num2Str(_flashImageHeader.data.pllInfo.pll_sw_flg, 16);
    ini["pll"]["clk"]         = num2Str(_flashImageHeader.data.pllInfo.sys_clk / 1000000);
    ini["pll"]["xclk"]        = num2Str(_flashImageHeader.data.pllInfo.xtal_clk / 1000000);
    ini["pll"]["dly"]         = num2Str(_flashImageHeader.data.pllInfo.dly);
    ini["pll"]["sw_dly_time"] = num2Str(_flashImageHeader.data.pllInfo.sw_dly_time);
    for (int i = 0; i < 6; i++) {
        ini["pll"]["reg" + std::to_string(i)] = num2Str(_flashImageHeader.data.pllInfo.pll_cfg[i], 16);
    }

    file.write(ini);
    return ERROR_NO_ERR;
}

int FlashImageHeader::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() < sizeof(FlashImageHeaderStruct)) {
        return ERROR_IMG_INVALID;
    }
    auto               decryptedData = getCopyXor(data);
    FlashSettingStruct flashSetting;
    memcpy(&flashSetting, decryptedData.data(), sizeof(FlashSettingStruct));
    if (getOnlyCrc(decryptedData, sizeof(FlashSettingStruct) - sizeof(uint32_t)) != flashSetting.crc) {
        return ERROR_IMG_INVALID;
    }
    FlashImageHeaderStruct flashImageHeader;
    memcpy(&flashImageHeader, decryptedData.data() + sizeof(FlashSettingStruct), sizeof(FlashImageHeaderStruct));
    std::vector<uint8_t> tempCrcData(sizeof(FlashImageHeaderStruct) - 2 * sizeof(uint32_t));
    memcpy(tempCrcData.data(), decryptedData.data() + sizeof(FlashSettingStruct) + 2 * sizeof(uint32_t),
        sizeof(FlashImageHeaderStruct) - 2 * sizeof(uint32_t));
    if (getOnlyCrc(tempCrcData) != flashImageHeader.hdr.crc) {
        return ERROR_IMG_INVALID;
    }
    memcpy(&_flashSetting, decryptedData.data(), sizeof(FlashSettingStruct));
    memcpy(&_flashImageHeader, decryptedData.data() + sizeof(FlashSettingStruct), sizeof(FlashImageHeaderStruct));
    return ERROR_NO_ERR;
}

void FlashImageHeader::setVersion(uint32_t version)
{
    _flashImageHeader.data.version = version;
    _flashImageHeader.hdr.crc      = getCrcOfHeader();
}

uint32_t FlashImageHeader::getVersion()
{
    return _flashImageHeader.data.version;
}

PartitionType FlashImageHeader::getPartitionType()
{
    return PartitionType::PART_ALL;
}

uint32_t FlashImageHeader::getPartitionSize()
{
    return sizeof(FlashSettingStruct) + sizeof(FlashImageHeaderStruct);
}

uint32_t FlashImageHeader::getPartitionDataSize()
{
    return sizeof(FlashSettingStruct) + sizeof(FlashImageHeaderStruct);
}

int FlashImageHeader::clearPartitionSetting()
{
    for (int i = 0; i < PARTITION_MAP.size(); i++) {
        _flashImageHeader.data.partitionSetting[i].size   = 0;
        _flashImageHeader.data.partitionSetting[i].offset = 0;
    }
    return ERROR_NO_ERR;
}

int FlashImageHeader::setPartitionSetting(PartitionType type, uint32_t offset,
    const std::vector<uint8_t> encryptedPartitionHeader)
{
    if (PARTITION_MAP.find(type) == PARTITION_MAP.end()) {
        return ERROR_IMG_INVALID;
    }
    auto &partitionSetting    = _flashImageHeader.data.partitionSetting[PARTITION_MAP.at(type)];
    partitionSetting.size     = (uint32_t)encryptedPartitionHeader.size();
    partitionSetting.offset   = offset;
    partitionSetting.crc      = getOtpCrc(encryptedPartitionHeader);
    _flashImageHeader.hdr.crc = getCrcOfHeader();
    return ERROR_NO_ERR;
}

int FlashImageHeader::getCrcOfHeader()
{
    return getOnlyCrc(reinterpret_cast<char *>(&_flashImageHeader.data), sizeof(FlashImageHeaderStructData));
}
