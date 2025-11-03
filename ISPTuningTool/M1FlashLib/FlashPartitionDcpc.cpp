#include "FlashPartitionDcpc.h"

FlashPartitionDcpc::FlashPartitionDcpc()
{
    _isInit         = false;
    _info.type       = PART_DCPC;
    _info.size       = 0;
    _info.dataLength = 0;

    memset(&_dcpcHead, 0, sizeof(_dcpcHead));
    memset(&_lutHeadFirst, 0, sizeof(_lutHeadFirst));
    memset(&_lutHeadSecond, 0, sizeof(_lutHeadSecond));

    _dcpcHead.magic_number = MAGIC_NUM_DCPC;
    _dcpcHead.version      = 0;
 }

 FlashPartitionDcpc::~FlashPartitionDcpc() { }

 std::vector<uint8_t> FlashPartitionDcpc::getPartitionDataEncrypted()
 {
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> tempData;
    std::vector<uint8_t> tempFirst(sizeof(DCPC_Lut_Header) + _dataFirst.size() * sizeof(uint8_t));
    std::vector<uint8_t> tempSecond(sizeof(DCPC_Lut_Header) + _dataSecond.size() * sizeof(uint8_t));
    if (_dcpcHead.dcpc_num == 1)
    {
        //拼接第一帧数据和头
        memcpy(&tempFirst[0], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        memcpy(&tempFirst[sizeof(DCPC_Lut_Header)], &_dataFirst[0], _dataFirst.size() * sizeof(uint8_t));
        tempData.resize(tempFirst.size());
        memcpy(&tempData[0], &tempFirst[0], tempFirst.size());
    }
    else if (_dcpcHead.dcpc_num == 2)
    {
        //拼接第一帧数据和头
        memcpy(&tempFirst[0], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        memcpy(&tempFirst[sizeof(DCPC_Lut_Header)], &_dataFirst[0], _dataFirst.size() * sizeof(uint8_t));
        //拼接第二帧数据和头
        memcpy(&tempSecond[0], &_lutHeadSecond, sizeof(DCPC_Lut_Header));
        memcpy(&tempSecond[sizeof(DCPC_Lut_Header)], &_dataSecond[0], _dataSecond.size() * sizeof(uint8_t));
        //两帧拼接
        tempData.resize(tempFirst.size() + tempSecond.size());
        memcpy(&tempData[0], &tempFirst[0], tempFirst.size());
        memcpy(&tempData[tempFirst.size()], &tempSecond[0], tempSecond.size());
    }

    getXor((char *)(&tempData[0]), tempData.size());

    return tempData;
 }

 std::vector<uint8_t> FlashPartitionDcpc::getPartitionDataDecrypted()
 {
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }
    std::vector<uint8_t> tempData;
    std::vector<uint8_t> tempFirst(sizeof(DCPC_Lut_Header) + _dataFirst.size());
    std::vector<uint8_t> tempSecond(sizeof(DCPC_Lut_Header) + _dataSecond.size());
    if (_dcpcHead.dcpc_num == 1)
    {
        //拼接第一帧数据和头
        memcpy(&tempFirst[0], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        memcpy(&tempFirst[sizeof(DCPC_Lut_Header)], &_dataFirst[0], _dataFirst.size());
        tempData.resize(tempFirst.size());
        memcpy(&tempData[0], &tempFirst[0], tempFirst.size());
    }
    else if (_dcpcHead.dcpc_num == 2)
    {
        //拼接第一帧数据和头
        memcpy(&tempFirst[0], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        memcpy(&tempFirst[sizeof(DCPC_Lut_Header)], &_dataFirst[0], _dataFirst.size());
        //拼接第二帧数据和头
        memcpy(&tempSecond[0], &_lutHeadSecond, sizeof(DCPC_Lut_Header));
        memcpy(&tempSecond[sizeof(DCPC_Lut_Header)], &_dataSecond[0], _dataSecond.size());
        //两帧拼接
        tempData.resize(tempFirst.size() + tempSecond.size());
        memcpy(&tempData[0], &tempFirst[0], tempFirst.size());
        memcpy(&tempData[tempFirst.size()], &tempSecond[0], tempSecond.size());
    }
    
    return tempData;
 }

 std::vector<uint8_t> FlashPartitionDcpc::getPartitionHeaderEncrypted()
 {
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }
    std::vector<uint8_t> data(sizeof(_dcpcHead));
    memcpy(&data[0], &_dcpcHead, sizeof(_dcpcHead));
	getXor(data, data.size());

    return data;
 }

 std::vector<uint8_t> FlashPartitionDcpc::getPartitionHeaderDecrypted()
 {
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }
    std::vector<uint8_t> data(sizeof(_dcpcHead));
    memcpy(&data[0], &_dcpcHead, sizeof(_dcpcHead));
    
    return data;
 }

 PartitionInfo FlashPartitionDcpc::getPartitionInfo()
 {
    return _info;
 }

 int FlashPartitionDcpc::setPartitionInfo(PartitionInfo info)
 {
    _info = info;
    return ERROR_NO_ERR;
 }

 int FlashPartitionDcpc::readPartitionDataFromCfgFile(const char *path)
 {
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
 }

 int FlashPartitionDcpc::writePartitionDataToCfgFile(const char *path)
 {
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }

    std::vector<uint8_t> allData((sizeof(DCPC_Header) - 8) + _dcpcHead.dcpc_num * sizeof(DCPC_Lut_Header)
        + _dataFirst.size() + _dataSecond.size());
    //还原偏移
    DCPC_Header          tempHead = _dcpcHead;
    tempHead.dcpc_lut0_st_addr -= 8;
    tempHead.dcpc_lut1_st_addr -= 8;
    //复制头
    std::vector<uint8_t> headData(sizeof(DCPC_Header));
    memcpy(&headData[0], &tempHead, sizeof(DCPC_Header));
    //开始复制头数据
    size_t nPos = 0;
    memcpy(&allData[nPos], &headData[8], sizeof(DCPC_Header) - 8);//不复制魔数及版本
    nPos += sizeof(DCPC_Header) - 8;
    //复制子头及数据
    if (_dcpcHead.dcpc_num == 1)
    {
        memcpy(&allData[nPos], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        nPos += sizeof(DCPC_Lut_Header);
        memcpy(&allData[nPos], &_dataFirst[0], _dataFirst.size());
        nPos += _dataFirst.size();
    }
    else if (_dcpcHead.dcpc_num == 2)
    {
        memcpy(&allData[nPos], &_lutHeadFirst, sizeof(DCPC_Lut_Header));
        nPos += sizeof(DCPC_Lut_Header);
        memcpy(&allData[nPos], &_dataFirst[0], _dataFirst.size());
        nPos += _dataFirst.size();

        memcpy(&allData[nPos], &_lutHeadSecond, sizeof(DCPC_Lut_Header));
        nPos += sizeof(DCPC_Lut_Header);
        memcpy(&allData[nPos], &_dataSecond[0], _dataSecond.size());
        nPos += _dataSecond.size();
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    file.write((char *)(&allData[0]), allData.size());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
 }

 int FlashPartitionDcpc::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
 {
    if (data.size() < 1)
    {
        return ERROR_IMG_EMPTY;
    }

    std::vector<uint8_t> allData(data);
    //解码源数据
    getXor((char *)(&allData[0]), allData.size());
    //获取头
    DCPC_Header tempHead;
    memcpy(&tempHead, &allData[0], sizeof(DCPC_Header));
    if (tempHead.dcpc_num != 1 && tempHead.dcpc_num != 2)
    {
        return ERROR_IMG_INVALID;
    }
    //获取子头
    DCPC_Lut_Header          headFirst;
    DCPC_Lut_Header          headSecond;
    std::vector<uint8_t>     dataFirst;
    std::vector<uint8_t>     dataSecond;
    std::vector<uint8_t>     commbineData;//解码后的子数据和子头组合在一起
    if (tempHead.dcpc_num == 1)
    {
        memcpy(&headFirst, &allData[tempHead.dcpc_lut0_st_addr], sizeof(DCPC_Lut_Header));
        //获取第一段数据
        dataFirst.resize(headFirst.length);
        memcpy(&dataFirst[0], &allData[tempHead.dcpc_lut0_st_addr + sizeof(DCPC_Lut_Header)], headFirst.length);
        //获取第一段数据crc并解码
        int nCrc = getCRC((char *)(&dataFirst[0]), dataFirst.size());
        if (nCrc != headFirst.crc)
        {
            return ERROR_IMG_INVALID;
        }
        //验证第一段子头
        nCrc = getOnlyCrc((char *)(&headFirst), sizeof(DCPC_Lut_Header));
        if (nCrc != tempHead.lut0_crc)
        {
            return ERROR_IMG_INVALID;
        }

        //复制数据
        memcpy(&_dcpcHead, &tempHead, sizeof(tempHead));
        memcpy(&_lutHeadFirst, &headFirst, sizeof(headFirst));
        _dataFirst.swap(dataFirst);

        _info.size = (uint32_t)(sizeof(_dcpcHead) + sizeof(_lutHeadFirst) + _dataFirst.size());
        _info.dataLength = (uint32_t)(sizeof(_lutHeadFirst) + _dataFirst.size());
    }
    else if (tempHead.dcpc_num == 2)
    {
        memcpy(&headFirst, &allData[tempHead.dcpc_lut0_st_addr], sizeof(DCPC_Lut_Header));
        //获取第一段数据
        dataFirst.resize(headFirst.length);
        memcpy(&dataFirst[0], &allData[tempHead.dcpc_lut0_st_addr + sizeof(DCPC_Lut_Header)], headFirst.length);
        //获取第一段数据crc并解码
        int nCrc = getOnlyCrc((char *)(&dataFirst[0]), dataFirst.size());
        if (nCrc != headFirst.crc)
        {
            return ERROR_IMG_INVALID;
        }

        memcpy(&headSecond, &allData[tempHead.dcpc_lut1_st_addr], sizeof(DCPC_Lut_Header));
        //获取第二段数据
        dataSecond.resize(headSecond.length);
        memcpy(&dataSecond[0], &allData[tempHead.dcpc_lut1_st_addr + sizeof(DCPC_Lut_Header)], headSecond.length);
        //获取第二段数据crc并解码
        nCrc = getOnlyCrc((char *)(&dataSecond[0]), dataSecond.size());
        if (nCrc != headSecond.crc)
        {
            return ERROR_IMG_INVALID;
        }

        //验证第一段子头
        nCrc = getOnlyCrc((char *)(&headFirst), sizeof(DCPC_Lut_Header));
        if (nCrc != tempHead.lut0_crc)
        {
            return ERROR_IMG_INVALID;
        }
        //验证第二段子头
        nCrc = getOnlyCrc((char *)(&headSecond), sizeof(DCPC_Lut_Header));
        if (nCrc != tempHead.lut1_crc)
        {
            return ERROR_IMG_INVALID;
        }
        //复制数据
        memcpy(&_dcpcHead, &tempHead, sizeof(tempHead));
        memcpy(&_lutHeadFirst, &headFirst, sizeof(headFirst));
        memcpy(&_lutHeadSecond, &headSecond, sizeof(headSecond));
        _dataFirst.swap(dataFirst);
        _dataSecond.swap(dataSecond);
        
        _info.size = (uint32_t)(sizeof(_dcpcHead) + sizeof(_lutHeadFirst) + _dataFirst.size() + sizeof(_lutHeadSecond)
            + _dataSecond.size());
        _info.dataLength = (uint32_t)(sizeof(_lutHeadFirst) + _dataFirst.size() + sizeof(_lutHeadSecond) + _dataSecond.size());
    }

    _isInit = true;

    return ERROR_NO_ERR;
 }

 PartitionType FlashPartitionDcpc::getPartitionType()
 {
     return _info.type;
 }

 uint32_t FlashPartitionDcpc::getPartitionSize()
 {
    return _info.size;
 }

 uint32_t FlashPartitionDcpc::getPartitionDataSize()
 {
     return _info.dataLength;
 }

 void FlashPartitionDcpc::InitHead()
 {
    _fileIn.open(_fileName, std::ios::in | std::ios::binary);
    _fileIn.seekg(0, std::ios::end);
    _fileIn.seekg(0);

    //从配置文件读取头，文件中没有头的前8个char
    std::vector<uint8_t> tempHead(sizeof(DCPC_Header));
    _fileIn.read((char *)&tempHead[8], sizeof(DCPC_Header) - 8);
    _fileIn.close();

    //复制到头成员变量中
    memcpy(&_dcpcHead, &tempHead[0], sizeof(DCPC_Header));
    //定义前8个char
    _dcpcHead.magic_number = MAGIC_NUM_DCPC;
    _dcpcHead.version      = 0;
 }

 int FlashPartitionDcpc::InitData()
 {
    if (_fileName == "")
    {
        return ERROR_PATH_INVALID;
    }

    InitHead();

    _fileIn.open(_fileName, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0, std::ios::end);
    size_t filelen = _fileIn.tellg();
    if (filelen < sizeof(DCPC_Lut_Header)) {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0);

    std::vector<uint8_t> tempHead(filelen + 8);
    _fileIn.read((char *)&tempHead[8], filelen);
    _fileIn.close();

    if (_dcpcHead.dcpc_num == 1)
    {
        //读取第一个子头
        _dcpcHead.dcpc_lut0_st_addr += 8;
        if (_dcpcHead.dcpc_lut0_st_addr >= filelen) {
            return ERROR_CONFIG_FILE;
        }

        memcpy(&_lutHeadFirst, &tempHead[_dcpcHead.dcpc_lut0_st_addr], sizeof(DCPC_Lut_Header));
        //读取第一个子数据
        if (_lutHeadFirst.length < 1)
        {
            return ERROR_CONFIG_FILE;
        }
        _dataFirst.resize(_lutHeadFirst.length);
        memcpy(&_dataFirst[0], &tempHead[_dcpcHead.dcpc_lut0_st_addr + sizeof(DCPC_Lut_Header)], _lutHeadFirst.length);
        //第一个子数据的crc存入第一个子头
        _lutHeadFirst.crc = getOnlyCrc((char *)(&_dataFirst[0]), _dataFirst.size());
        //获取第一段子头CRC
        _dcpcHead.lut0_crc = getOnlyCrc((char *)(&_lutHeadFirst), sizeof(DCPC_Lut_Header));

        _info.size = (uint32_t)(sizeof(_dcpcHead) + sizeof(_lutHeadFirst) + _dataFirst.size());
        _info.dataLength = (uint32_t)(sizeof(_lutHeadFirst) + _dataFirst.size());
    }
    else if (_dcpcHead.dcpc_num == 2)
    {
        //读取第一个子头
        _dcpcHead.dcpc_lut0_st_addr += 8;
        memcpy(&_lutHeadFirst, &tempHead[_dcpcHead.dcpc_lut0_st_addr], sizeof(DCPC_Lut_Header));
        //读取第一个子数据
        if (_lutHeadFirst.length < 1)
        {
            return ERROR_CONFIG_FILE;
        }
        _dataFirst.resize(_lutHeadFirst.length);
        memcpy(&_dataFirst[0], &tempHead[_dcpcHead.dcpc_lut0_st_addr + sizeof(DCPC_Lut_Header)], _lutHeadFirst.length);
        //第一个子数据的crc存入第一个子头
        _lutHeadFirst.crc = getOnlyCrc((char *)(&_dataFirst[0]), _dataFirst.size());
        //获取第一段子头CRC
        _dcpcHead.lut0_crc = getOnlyCrc((char *)(&_lutHeadFirst), sizeof(DCPC_Lut_Header));

        //读取第二个子头
        _dcpcHead.dcpc_lut1_st_addr += 8;
        memcpy(&_lutHeadSecond, &tempHead[_dcpcHead.dcpc_lut1_st_addr], sizeof(DCPC_Lut_Header));
        //读取第二个子数据
        if (_lutHeadSecond.length < 1)
        {
            return ERROR_CONFIG_FILE;
        }
        _dataSecond.resize(_lutHeadSecond.length);
        memcpy(&_dataSecond[0], &tempHead[_dcpcHead.dcpc_lut1_st_addr + sizeof(DCPC_Lut_Header)], _lutHeadSecond.length);
        //第二个子数据的crc存入第二个子头
        _lutHeadSecond.crc = getOnlyCrc((char *)(&_dataSecond[0]), _dataSecond.size());
        //获取第一段子头CRC
        _dcpcHead.lut1_crc = getOnlyCrc((char *)(&_lutHeadSecond), sizeof(DCPC_Lut_Header));

        _info.size = (uint32_t)(sizeof(_dcpcHead) + sizeof(_lutHeadFirst) + _dataFirst.size() + sizeof(_lutHeadSecond)
            + _dataSecond.size());
        _info.dataLength = (uint32_t)(sizeof(_lutHeadFirst) + _dataFirst.size() + sizeof(_lutHeadSecond) + _dataSecond.size());
    }
    
    _isInit = true;

    return ERROR_NO_ERR;
 }
