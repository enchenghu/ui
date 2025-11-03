#include "FlashPartitionDdrSetting.h"
#include <regex>

FlashPartitionDdrSetting::FlashPartitionDdrSetting()
{
    _isInit          = false;
    _info.type       = PART_DDR_SETTING;
    _info.size       = 0;
    _info.dataLength = 0;

    memset(&_ddrHead, 0, sizeof(_ddrHead));
    _ddrHead.magic_number = MAGIC_NUM_DDR;
    _ddrHead.version      = 0;
    _ddrHead.data_crc     = 0;
    _ddrHead.data_len     = 0;
}

FlashPartitionDdrSetting::~FlashPartitionDdrSetting() { }

std::vector<uint8_t> FlashPartitionDdrSetting::getPartitionDataEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(_ddrData.size() * sizeof(DDR_Setting));
    memcpy(&data[0], &_ddrData[0], _ddrData.size() * sizeof(DDR_Setting));
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionDdrSetting::getPartitionDataDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(_ddrData.size() * sizeof(DDR_Setting));
    memcpy(&data[0], &_ddrData[0], _ddrData.size() * sizeof(DDR_Setting));
    return data;
}

std::vector<uint8_t> FlashPartitionDdrSetting::getPartitionHeaderEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_ddrHead));
    memcpy(&data[0], &_ddrHead, sizeof(Setting_Header));
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionDdrSetting::getPartitionHeaderDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_ddrHead));
    memcpy(&data[0], &_ddrHead, sizeof(Setting_Header));
    return data;
}

PartitionInfo FlashPartitionDdrSetting::getPartitionInfo()
{
    return _info;
}

int FlashPartitionDdrSetting::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionDdrSetting::readPartitionDataFromCfgFile(const char *path)
{
    if (!isFileExist(path)) {
        return ERROR_PATH_INVALID;
    }
    std::ifstream _fileIn;

    _fileIn.open(path, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open()) {
        return ERROR_CONFIG_FILE;
    }

    _fileIn.seekg(0, std::ios::end);
    _fileIn.seekg(0);

    std::vector<DDR_Setting> vTemp;
    _ddrData.swap(vTemp);
    _ddrData.clear();

    std::string line;
    while (!_fileIn.eof()) {
        getline(_fileIn, line);
        if (line.size() < 5)
            continue;
        DDR_Setting ddr_setting = {};
        line                    = Trimstring(line);
        if (!line.compare(0, 2, "rd", 0, 2)) {
            ddr_setting.rd_flag = 1;
        } else if (!line.compare(0, 2, "wr", 0, 2)) {
            ddr_setting.rd_flag = 0;
        } else
            continue;
        line = line.substr(2);
        // (0x310011e0,31,31,0x80000000)
        // (0x31001020, 0x6908280a)
        std::regex pattern("\\((0x[0-9a-f]+),((\\d+),(\\d+),)?(0x[0-9a-f]+)\\)", std::regex::icase);
        auto       match = std::smatch();
        std::regex_search(line, match, pattern);
        if (match.size() == 0)
            continue;
        try {
            ddr_setting.addr = std::stoul(match[1].str(), nullptr, 16);
            ddr_setting.data = std::stoul(match[5].str(), nullptr, 16);
            if (match[2].matched) {
                ddr_setting.high_bit = std::stoi(match[3].str());
                ddr_setting.low_bit  = std::stoi(match[4].str());
            } else {
                ddr_setting.high_bit = 31;
                ddr_setting.low_bit  = 0;
            }
        } catch (...) {
            continue;
        }
        _ddrData.emplace_back(ddr_setting);
    }
    _fileIn.close();

    if (_ddrData.size() == 0) {
        return ERROR_CONFIG_FILE;
    }

    InitHead();

    _isInit = true;

    return ERROR_NO_ERR;
}

int FlashPartitionDdrSetting::writePartitionDataToCfgFile(const char *path)
{
    // todo
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit) {
        // 若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_ddrData.size() < 1) {
        return ERROR_IMG_INVALID;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }

    std::string strWrite = "";
    for (size_t i = 0; i < _ddrData.size(); i++) {
        if (_ddrData[i].rd_flag == 1) {
            strWrite += "rd(";
        } else if (_ddrData[i].rd_flag == 0) {
            strWrite += "wr(";
        }
        strWrite += num2Str(_ddrData[i].addr, 16, 8, '0', "0x") + ",";
        if (_ddrData[i].high_bit != 31 || _ddrData[i].low_bit != 0)
            strWrite += num2Str(_ddrData[i].high_bit, 10) + "," + num2Str(_ddrData[i].low_bit, 10) + ",";
        strWrite += num2Str(_ddrData[i].data, 16, 8, '0', "0x") + ")\n";
    }
    file.write(strWrite.data(), strWrite.length());

    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();
    return ERROR_NO_ERR;
}

int FlashPartitionDdrSetting::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() == 0) {
        return ERROR_IMG_EMPTY;
    }

    // getXor(data, data.size());
    std::vector<uint8_t> vecSourceData(data);
    // 解码源数据
    getXor((char *)(&vecSourceData[0]), vecSourceData.size());

    // 获取数据头
    Setting_Header tempDdrHead;
    memcpy(&tempDdrHead, &vecSourceData[0], sizeof(Setting_Header));
    if (tempDdrHead.data_len == 0 || tempDdrHead.magic_number == 0) {
        return ERROR_IMG_INVALID;
    }

    // 获取数据段
    std::vector<DDR_Setting> vecDdrData(tempDdrHead.data_len / sizeof(DDR_Setting));
    memcpy(&vecDdrData[0], &vecSourceData[sizeof(Setting_Header)], tempDdrHead.data_len);
    // 获取数据段crc并解析
    int nCrc = getOnlyCrc((char *)(&vecDdrData[0]), vecDdrData.size() * sizeof(DDR_Setting));
    if (nCrc != tempDdrHead.data_crc) {
        return ERROR_IMG_INVALID;
    }

    // 存入数据头
    memcpy(&_ddrHead, &tempDdrHead, sizeof(Setting_Header));
    // 存入数据段
    _ddrData.clear();
    _ddrData.swap(vecDdrData);

    // 分区总长度
    _info.size = (uint32_t)(sizeof(_ddrHead) + _ddrData.size() * sizeof(DDR_Setting));
    // 数据长度
    _info.dataLength = (uint32_t)(_ddrData.size() * sizeof(DDR_Setting));

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionDdrSetting::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionDdrSetting::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionDdrSetting::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionDdrSetting::InitHead()
{
    _ddrHead.magic_number = MAGIC_NUM_DDR;
    _ddrHead.version      = 0;
    _ddrHead.data_len     = (uint16_t)_ddrData.size() * sizeof(DDR_Setting);
    _ddrHead.data_crc     = getOnlyCrc((char *)(&_ddrData[0]), _ddrHead.data_len);

    // 分区总长度
    _info.size = (uint32_t)(sizeof(_ddrHead) + _ddrData.size() * sizeof(DDR_Setting));
    // 数据长度
    _info.dataLength = (uint32_t)(_ddrData.size() * sizeof(DDR_Setting));
}
