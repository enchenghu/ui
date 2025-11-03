#include "FlashPartitionKernel.h"

FlashPartitionKernel::FlashPartitionKernel()
{
    _isInit         = false;
    _info.type       = PART_LINUX_KERNEL;
    _info.size       = 0;
    _info.dataLength = 0;

    memset(&_kernelHead, 0, sizeof(_kernelHead));
    _kernelHead.magic_number = MAGIC_NUM_KERNEL;
    _kernelHead.version      = 0;
    _kernelHead.data_crc     = 0;
    _kernelHead.data_len     = 0;
}

FlashPartitionKernel::~FlashPartitionKernel() { }

std::vector<uint8_t> FlashPartitionKernel::getPartitionDataEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> data(_kernelData.size() * sizeof(uint8_t));
    memcpy(&data[0], &_kernelData[0], _kernelData.size() * sizeof(uint8_t));
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionKernel::getPartitionDataDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(_kernelData.size() * sizeof(uint8_t));
    memcpy(&data[0], &_kernelData[0], _kernelData.size() * sizeof(uint8_t));
    return data;
}

std::vector<uint8_t> FlashPartitionKernel::getPartitionHeaderEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_kernelHead));
    memcpy(&data[0], &_kernelHead, sizeof(KERNEL_Header));
	getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionKernel::getPartitionHeaderDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_kernelHead));
    memcpy(&data[0], &_kernelHead, sizeof(KERNEL_Header));
    return data;
}

PartitionInfo FlashPartitionKernel::getPartitionInfo()
{
    return _info;
}

int FlashPartitionKernel::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionKernel::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
}

int FlashPartitionKernel::writePartitionDataToCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    file.write((char *)(&_kernelData[0]), _kernelData.size());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionKernel::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
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
    KERNEL_Header tempDdrHead;
    memcpy(&tempDdrHead, &vecSourceData[0], sizeof(KERNEL_Header));
    if (tempDdrHead.magic_number != MAGIC_NUM_KERNEL) 
    {
        return ERROR_IMG_INVALID;
    }

    //获取数据段
    std::vector<uint8_t> vecDdrData(tempDdrHead.data_len * sizeof(uint8_t));
    memcpy(&vecDdrData[0], &vecSourceData[sizeof(KERNEL_Header)], tempDdrHead.data_len * sizeof(uint8_t));
    //获取数据段crc并解析
    int nCrc = getOnlyCrc((char *)(&vecDdrData[0]), vecDdrData.size());
    if (nCrc != tempDdrHead.data_crc) 
    {
        return ERROR_IMG_INVALID;
    }

    //存入数据头
    memcpy(&_kernelHead, &tempDdrHead, sizeof(KERNEL_Header));
    //存入数据段
    _kernelData.clear();
    _kernelData.swap(vecDdrData);

    //分区总长度
    _info.size = (uint32_t)(sizeof(_kernelHead) + _kernelData.size() * sizeof(uint8_t));
    //数据长度
    _info.dataLength = (uint32_t)(_kernelData.size() * sizeof(uint8_t));

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionKernel::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionKernel::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionKernel::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionKernel::InitHead()
{
    _kernelHead.magic_number   = MAGIC_NUM_KERNEL;
    _kernelHead.version        = 0;
    _kernelHead.data_len       = (uint32_t)_kernelData.size() * sizeof(uint8_t);
    _kernelHead.data_crc       = getOnlyCrc((char *)(&_kernelData[0]), _kernelHead.data_len);
    
    //分区总长度
    _info.size = (uint32_t)(sizeof(_kernelHead) + _kernelData.size() * sizeof(uint8_t));
    //数据长度
    _info.dataLength = (uint32_t)(_kernelData.size() * sizeof(uint8_t));
}

int FlashPartitionKernel::InitData()
{
    if (_fileName == "") {
        ERROR_PATH_INVALID;
    }

    _fileIn.open(_fileName, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0, std::ios::end);
    uint32_t nFilelen = (uint32_t)_fileIn.tellg();
    if (nFilelen < 1) {
        return ERROR_CONFIG_FILE_READ;
    }
    _fileIn.seekg(0);

    //设置数据长度
    _kernelData.resize(nFilelen);
    //读取数据
    _fileIn.read((char *)(&_kernelData[0]), nFilelen);
    _fileIn.close();

    InitHead();

    _isInit = true;
    
    return ERROR_NO_ERR;
}
