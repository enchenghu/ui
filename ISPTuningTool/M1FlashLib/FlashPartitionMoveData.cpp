#include "FlashPartitionMoveData.h"
#include "CommonFunction.h"
#include <filesystem>

FlashPartitionMoveData::FlashPartitionMoveData()
{
    _isInit          = false;
    _info.type       = PART_MOVE_DATA;
    _info.size       = 0;
    _info.dataLength = 0;

    memset(&_head, 0, sizeof(_head));
    _head.magic_number = MAGIC_NUM_MOVE_DATA;
    _head.version      = 0;
    _head.num          = 0;
}

FlashPartitionMoveData::~FlashPartitionMoveData() { }

std::vector<uint8_t> FlashPartitionMoveData::getPartitionDataEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(0);
    for (uint32_t i = 0; i < _head.num; i++) {
        data.insert(data.end(), _data[i].begin(), _data[i].end());
    }
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionMoveData::getPartitionDataDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(0);
    for (uint32_t i = 0; i < _head.num; i++) {
        data.insert(data.end(), _data[i].begin(), _data[i].end());
    }
    return data;
}

std::vector<uint8_t> FlashPartitionMoveData::getPartitionHeaderEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_head));
    memcpy(&data[0], &_head, sizeof(_head));
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionMoveData::getPartitionHeaderDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_head));
    memcpy(&data[0], &_head, sizeof(_head));
    return data;
}

PartitionInfo FlashPartitionMoveData::getPartitionInfo()
{
    return _info;
}

int FlashPartitionMoveData::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionMoveData::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
}

int FlashPartitionMoveData::writePartitionDataToCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit) {
        // 若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_head.num < 1) {
        return ERROR_IMG_EMPTY;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }

    std::string strPath(path);
    size_t      nPos = strPath.rfind("\\");
    strPath          = strPath.substr(0, nPos + 1);

    std::string strCommbine = "";
    for (size_t i = 0; i < _head.num; i++) {
        std::ofstream tempFile;
        tempFile.open(strPath + "moveData" + std::to_string(i), std::ios::out | std::ios::binary | std::ios::trunc);
        if (!tempFile.is_open()) {
            continue;
        }
        tempFile.write((char *)(&_data[i][0]), _head.length[i]);
        tempFile.close();

        strCommbine += decimalToHex(_head.offset[i]) + ",";
        strCommbine += strPath + "moveData" + std::to_string(i) + "\n";
    }

    file.write(strCommbine.data(), strCommbine.length());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionMoveData::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() == 0) {
        return ERROR_IMG_EMPTY;
    }

    // getXor(data, data.size());
    std::vector<uint8_t> vecSourceData(data);
    // 解码源数据
    getXor((char *)(&vecSourceData[0]), vecSourceData.size());

    // 获取数据头
    Move_Data_Header tempHead;
    memcpy(&tempHead, &vecSourceData[0], sizeof(Move_Data_Header));
    if (tempHead.num == 0 || tempHead.magic_number == 0) {
        return ERROR_IMG_INVALID;
    }

    size_t                            nSize = 0;
    size_t                            nPos  = sizeof(Move_Data_Header);
    std::vector<std::vector<uint8_t>> vecData;
    for (uint32_t i = 0; i < tempHead.num; i++) {
        // 获取数据段
        std::vector<uint8_t> tempData(tempHead.length[i] * sizeof(uint8_t));
        memcpy(&tempData[0], &vecSourceData[nPos], tempHead.length[i]);
        // 获取数据段crc并解析
        int nCrc = getOnlyCrc((char *)(&tempData[0]), tempData.size());
        if (nCrc != tempHead.crc[i]) {
            return ERROR_IMG_INVALID;
        }
        vecData.push_back(tempData);
        nSize += tempData.size();
        nPos += tempHead.length[i];
    }

    // 存入数据头
    memcpy(&_head, &tempHead, sizeof(tempHead));
    // 存入数据段
    _data.clear();
    _data.swap(vecData);

    // 分区总长度
    _info.size = sizeof(Move_Data_Header) + (uint32_t)nSize;
    // 数据长度
    _info.dataLength = (uint32_t)nSize;

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionMoveData::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionMoveData::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionMoveData::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionMoveData::InitHead()
{
    _head.magic_number = MAGIC_NUM_MOVE_DATA;
    _head.version      = 0;
}

int FlashPartitionMoveData::InitData()
{
    if (_fileName == "") {
        return ERROR_PATH_INVALID;
    }

    // 获取配置文件中的文件名串
    _fileIn.open(_fileName, std::ios::in);
    if (!_fileIn.is_open()) {
        return ERROR_CONFIG_FILE;
    }

    std::string line;
    _fileOffset.resize(0);
    _filePath.resize(0);
    while (!_fileIn.eof()) {
        getline(_fileIn, line);
        if (line.size() < 3)
            continue;
        size_t nPos = line.find(",");
        if (nPos == 0) {
            continue;
        }
        std::string strOff  = line.substr(0, nPos);
        std::string strPath = line.substr(nPos + 1);
        if (strOff == "" || strPath == "") {
            continue;
        }
        _fileOffset.push_back(strOff);
        _filePath.push_back(strPath);
    }
    _fileIn.close();

    if (_filePath.size() < 1) {
        return ERROR_PATH_NOT_FILE;
    }

    std::size_t nDataSize = 0;
    _data.resize(0);
    for (size_t i = 0; i < _filePath.size(); i++) {
        if (isRelativePath(_filePath[i].c_str())) {
            std::filesystem::path p = getDirPath(getAbsolutePath(_fileName.c_str()).c_str());
            p.append(_filePath[i]);
            _filePath[i] = p.string();
        }
        _fileIn.open(_filePath[i], std::ios::in | std::ios::binary);
        if (!_fileIn.is_open()) {
            return ERROR_CONFIG_FILE;
        }
        _fileIn.seekg(0, std::ios::end);
        size_t filelen = _fileIn.tellg();
        if (filelen < 1) {
            _fileIn.close();
            continue;
        }

        _fileIn.seekg(0);

        std::vector<uint8_t> data(filelen);
        _fileIn.read((char *)(&data[0]), filelen);
        _data.push_back(data);
        nDataSize += filelen;

        _head.length[_head.num] = (uint32_t)filelen;
        _head.offset[_head.num] = (uint32_t)stoul(_fileOffset[i], NULL, 16);
        _head.crc[_head.num]    = getOnlyCrc((char *)(&data[0]), filelen);
        _head.num++;

        _fileIn.close();
    }

    // 分区总长度
    _info.size = sizeof(Move_Data_Header) + (uint32_t)nDataSize;
    // 数据长度
    _info.dataLength = (uint32_t)nDataSize;

    InitHead();

    _isInit = true;

    return ERROR_NO_ERR;
}
