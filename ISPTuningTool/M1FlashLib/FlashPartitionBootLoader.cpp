#include "FlashPartitionBootLoader.h"

FlashPartitionBootLoader::FlashPartitionBootLoader()
{
    _isInit         = false;
    _info.type       = PART_ARM_BOOT;
    _info.size       = 0;
    _info.dataLength = 0;

    memset(&_bootHead,0,sizeof(_bootHead));
    _bootHead.magic_number = MAGIC_NUM_BOOTLOADER;
    _bootHead.version      = 0;
    _bootHead.data_crc     = 0;
    _bootHead.data_len     = 0;
}

FlashPartitionBootLoader::~FlashPartitionBootLoader() { }

std::vector<uint8_t> FlashPartitionBootLoader::getPartitionDataEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(_bootData.size() * sizeof(uint8_t));
    memcpy(&data[0], &_bootData[0], _bootData.size() * sizeof(uint8_t));
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionBootLoader::getPartitionDataDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(_bootData.size() * sizeof(uint8_t));
    memcpy(&data[0], &_bootData[0], _bootData.size() * sizeof(uint8_t));
    return data;
}

std::vector<uint8_t> FlashPartitionBootLoader::getPartitionHeaderEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_bootHead));
    memcpy(&data[0], &_bootHead, sizeof(_bootHead));
	getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionBootLoader::getPartitionHeaderDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_bootHead));
    memcpy(&data[0], &_bootHead, sizeof(_bootHead));
    return data;
}

PartitionInfo FlashPartitionBootLoader::getPartitionInfo()
{
    return _info;
}

int FlashPartitionBootLoader::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionBootLoader::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
}

int FlashPartitionBootLoader::writePartitionDataToCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_FILE_READ;
    }
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    file.write((char *)(&_bootData[0]), _bootData.size());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionBootLoader::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() == 0) 
    {
        return ERROR_IMG_EMPTY;
    }

    //getXor(data, data.size());
    std::vector<uint8_t> vecSourceData(data);
    //解码源数据
    getXor((char *)(&vecSourceData[0]), vecSourceData.size());

    //获取数据头
    BOOT_LOADER_Header tempHead;
    memcpy(&tempHead, &vecSourceData[0], sizeof(BOOT_LOADER_Header));
    if (tempHead.data_len == 0 || tempHead.magic_number == 0) 
    {
        return ERROR_IMG_INVALID;
    }

    //获取数据段
    std::vector<uint8_t> vecData(tempHead.data_len * sizeof(uint8_t));
    memcpy(&vecData[0], &vecSourceData[sizeof(BOOT_LOADER_Header)], tempHead.data_len * sizeof(uint8_t));
    //获取数据段crc并解析
    int nCrc = getOnlyCrc((char *)(&vecData[0]), vecData.size());
    if (nCrc != tempHead.data_crc) 
    {
        return ERROR_IMG_INVALID;
    }

    //存入数据头
    memcpy(&_bootHead, &tempHead, sizeof(tempHead));
    //存入数据段
    _bootData.clear();
    _bootData.swap(vecData);

    //分区总长度
    _info.size = (uint32_t)(sizeof(_bootHead) + _bootData.size() * sizeof(uint8_t));
    //数据长度
    _info.dataLength = (uint32_t)_bootData.size() * sizeof(uint8_t);

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionBootLoader::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionBootLoader::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionBootLoader::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionBootLoader::InitHead()
{
    _bootHead.magic_number   = MAGIC_NUM_BOOTLOADER;
    _bootHead.version        = 0;
    _bootHead.data_len       = (uint32_t)_bootData.size() * sizeof(uint8_t);
    _bootHead.data_crc       = getOnlyCrc((char *)(&_bootData[0]), _bootHead.data_len);
    
    //分区总长度
    _info.size = (uint32_t)(sizeof(_bootHead) + _bootData.size() * sizeof(uint8_t));
    //数据长度
    _info.dataLength = (uint32_t)_bootData.size() * sizeof(uint8_t);
}

int FlashPartitionBootLoader::InitData()
{
    if (_fileName == "") {
        return ERROR_PATH_INVALID;
    }
    _fileIn.open(_fileName, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0, std::ios::end);
    size_t filelen = _fileIn.tellg();
    if (filelen < 1) {
        return ERROR_CONFIG_FILE;
    }
    _fileIn.seekg(0);
    _bootData.resize(filelen);
    _fileIn.read((char *)(&_bootData[0]), filelen);
    _fileIn.close();

    InitHead();

    _isInit = true;

    return ERROR_NO_ERR;
}
