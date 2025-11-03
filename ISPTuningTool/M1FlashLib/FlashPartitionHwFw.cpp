#include "FlashPartitionHwFw.h"

FlashPartitionHwFwBase::FlashPartitionHwFwBase()
{
    _isInit = false;

    _info.size       = 0;
    _info.dataLength = 0;

    _hwHead.version = 0;
}

FlashPartitionHwFwBase::~FlashPartitionHwFwBase() { }

std::vector<uint8_t> FlashPartitionHwFwBase::getPartitionDataEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }
    // 拼接子头子段
    std::vector<uint8_t> data(_info.dataLength);
    size_t               nPos = 0;
    for (size_t i = 0; i < _hwHead.num; i++) {
        memcpy(&data[nPos], &_hwData[i][0], _hwData[i].size() * sizeof(uint8_t));
        nPos += _hwData[i].size();
    }
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionHwFwBase::getPartitionDataDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }
    // 拼接子头子段
    std::vector<uint8_t> data(_info.dataLength);
    size_t               nPos = 0;
    for (size_t i = 0; i < _hwHead.num; i++) {
        memcpy(&data[nPos], &_hwData[i][0], _hwData[i].size() * sizeof(uint8_t));
        nPos += _hwData[i].size();
    }

    return data;
}

std::vector<uint8_t> FlashPartitionHwFwBase::getPartitionHeaderEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_hwHead) + _hwHead.num * sizeof(_hw_fw_Pair));
    memcpy(&data[0], &_hwHead, sizeof(_hwHead));
    for (size_t i = 0; i < _hwHead.num; i++) {
        memcpy(&data[sizeof(_hwHead) + i * sizeof(_hw_fw_Pair)], &_subHead[i], sizeof(_hw_fw_Pair));
    }
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionHwFwBase::getPartitionHeaderDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_hwHead) + _hwHead.num * sizeof(_hw_fw_Pair));
    memcpy(&data[0], &_hwHead, sizeof(_hwHead));
    for (size_t i = 0; i < _hwHead.num; i++) {
        memcpy(&data[sizeof(_hwHead) + i * sizeof(_hw_fw_Pair)], &_subHead[i], sizeof(_hw_fw_Pair));
    }

    return data;
}

PartitionInfo FlashPartitionHwFwBase::getPartitionInfo()
{
    return _info;
}

int FlashPartitionHwFwBase::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionHwFwBase::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
}

int FlashPartitionHwFwBase::writePartitionDataToCfgFile(const char *path)
{
    // todo
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit) {
        // 若未初始化
        return ERROR_IMG_EMPTY;
    }
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }

    std::string AllStr = "";
    for (size_t i = 0; i < _hwHead.num; i++) {
        analysisData(_hwData[i], AllStr);
    }
    file.write(AllStr.data(), AllStr.length());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();
    return ERROR_NO_ERR;
}

int FlashPartitionHwFwBase::analysisData(const std::vector<uint8_t> vecData, std::string &allStr)
{
    if (vecData.size() < 8) {
        return ERROR_IMG_EMPTY;
    }

    std::vector<std::tuple<uint32_t, uint32_t, int>> vecRegData;
    uint32_t                                         addrPrefix = (*(uint16_t *)(vecData.data() + 3)) << 16;
    uint32_t                                         nNum       = 0;
    uint32_t                                         subAddr    = 0;
    for (size_t i = 5; i < vecData.size();) {
        if (vecData[i] >= 0xa0 && vecData[i] <= 0xaf) {
            nNum    = 1 + vecData[i] - 0xa0;
            subAddr = *(uint16_t *)(vecData.data() + i + 1);
            i += 3;
        } else if (vecData[i] >= 0x50 && vecData[i] <= 0x5f && nNum == 16) {
            nNum = 1 + vecData[i] - 0x50;
            subAddr += 16;
            i++;
        } else if (vecData[i] == 0xb0) {
            addrPrefix = (*(uint16_t *)(vecData.data() + i + 1)) << 16;
            i += 3;
            continue;
        }

        for (uint32_t p = 0; p < nNum;) {
            if (nNum - p >= 4) {
                uint32_t value = *(uint32_t *)(vecData.data() + i);
                vecRegData.push_back(std::make_tuple(addrPrefix + subAddr + p, value, 4));
                p += 4;
                i += 4;
            } else {
                uint8_t value = vecData[i];
                vecRegData.push_back(std::make_tuple(addrPrefix + subAddr + p, static_cast<uint32_t>(value), 1));
                i++;
                p++;
            }
        }
    }

    for (size_t i = 0; i < vecRegData.size(); i++) {
        allStr += "22 ";
        allStr += num2Str(std::get<0>(vecRegData[i]), 16, 8, '0') + " ";
        allStr += num2Str(std::get<1>(vecRegData[i]), 16, std::get<2>(vecRegData[i]) * 2, '0') + " ";
        allStr += "4 ";
        allStr += num2Str(std::get<2>(vecRegData[i]), 10, 1, '0') + " 1 1\n";
    }

    return ERROR_NO_ERR;
}

int FlashPartitionHwFwBase::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() == 0) {
        return ERROR_IMG_EMPTY;
    }

    // getXor(data, data.size());
    std::vector<uint8_t> vecSourceData(data);
    // 解码源数据
    getXor((char *)(&vecSourceData[0]), vecSourceData.size());

    // 获取数据头
    HW_FW_Header tempDdrHead;
    memcpy(&tempDdrHead, &vecSourceData[0], sizeof(HW_FW_Header));
    if (tempDdrHead.num == 0 || tempDdrHead.magic_number == 0) {
        return ERROR_IMG_INVALID;
    }

    size_t                            nPos = sizeof(HW_FW_Header);
    std::vector<HW_FW_Pair>           tempSubHead;
    std::vector<std::vector<uint8_t>> tempSubData;
    for (size_t i = 0; i < tempDdrHead.num; i++) {
        HW_FW_Pair tempHead;
        memcpy((char *)&tempHead, &vecSourceData[nPos], sizeof(HW_FW_Pair));
        if (tempHead.data_len < 1) {
            return ERROR_IMG_INVALID;
        }
        nPos += sizeof(HW_FW_Pair);
        std::vector<uint8_t> tempData(tempHead.data_len);
        memcpy((char *)&tempData[0], &vecSourceData[nPos], tempHead.data_len);
        // 解码数据
        if (tempHead.data_crc != getOnlyCrc((char *)&tempData[0], tempHead.data_len)) {
            return ERROR_IMG_INVALID;
        }
        tempSubData.push_back(tempData);
        tempSubHead.push_back(tempHead);
        nPos += tempHead.data_len;
    }

    // 存入数据头
    memcpy(&_hwHead, &tempDdrHead, sizeof(HW_FW_Header));
    // 存入数据段
    _hwData.clear();
    _hwData.swap(tempSubData);
    _subHead.swap(tempSubHead);

    _info.dataLength = 0;
    for (size_t i = 0; i < _hwHead.num; i++) {
        _info.dataLength += _subHead[i].data_len;
    }
    // 设置总长度
    _info.size = (uint32_t)sizeof(_hwHead) + _hwHead.num * (uint32_t)sizeof(_hw_fw_Pair) + _info.dataLength;

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionHwFwBase::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionHwFwBase::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionHwFwBase::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionHwFwBase::InitHead()
{
    switch (_info.type) {
    case PartitionType::PART_GENERAL_HW_SETTING:
    default:
        _hwHead.magic_number = MAGIC_NUM_HARDWARE_SETTING;
        break;
    case PartitionType::PART_FW_SETTING:
        _hwHead.magic_number = MAGIC_NUM_FIRMWARE_SETTING;
        break;
    case PartitionType::PART_FW_RECOVER:
        _hwHead.magic_number = MAGIC_NUM_FIRMWARE_RECOVER;
        break;
    }
    _hwHead.version = 0;

    // 设置总长度
    _info.size = (uint32_t)sizeof(_hwHead) + _hwHead.num * (uint32_t)sizeof(_hw_fw_Pair) + _info.dataLength;
}

int FlashPartitionHwFwBase::InitData()
{
    if (_fileName == "") {
        return ERROR_PATH_INVALID;
    }

    std::vector<uint8_t> tempData;
    size_t               nFilelen = 0;
    _hwHead.num                   = setting_to_grp(_fileName, tempData, nFilelen);
    if (nFilelen < 1) {
        return ERROR_PATH_INVALID;
    }

    // 设置子段数量
    _subHead.resize(_hwHead.num);
    _hwData.resize(_hwHead.num);

    size_t nPos = 0;
    for (size_t i = 0; i < _hwHead.num; i++) {
        if (tempData.size() < nPos) {
            break;
        }
        // 获取长度
        _subHead[i].data_len = tempData[nPos] + (tempData[nPos + 1] << 8);
        // 复制数据到子段
        _hwData[i].resize(_subHead[i].data_len);
        memcpy(&_hwData[i][0], &tempData[nPos], _subHead[i].data_len);
        // 获取CRC
        _subHead[i].data_crc = getOnlyCrc((char *)&_hwData[i][0], _subHead[i].data_len);

        nPos += _subHead[i].data_len;
    }
    _info.dataLength = (uint32_t)(nFilelen);

    InitHead();

    _isInit = true;

    return ERROR_NO_ERR;
}
