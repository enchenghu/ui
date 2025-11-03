#include "FlashPartitionOsd.h"

FlashPartitionOsd::FlashPartitionOsd()
{
    _info.type = PART_OSD;
    _info.size = 0;
    _info.dataLength = 0;

    memset(&_head, 0, sizeof(_head));
    _head.magic_number = MAGIC_NUM_OSD;
    _head.version      = 0;
}

FlashPartitionOsd::~FlashPartitionOsd() { }

std::vector<uint8_t> FlashPartitionOsd::getPartitionDataEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    int nDataSize = 0;
    for (size_t i = 0; i < _dataHead.size(); i++)
    {
        nDataSize += sizeof(Frame_Data_Header);
        for (size_t p = 0; p < 8; p++)
        {
            nDataSize += _dataHead[i].LyrLen[p];
        }
    }

    //拼接子头子数据
    std::vector<uint8_t> data(nDataSize);
    size_t nPos = 0;
    for (size_t i = 0; i < _dataHead.size(); i++)
    {
        memcpy(&data[nPos], &_dataHead[i], sizeof(Frame_Data_Header));//每段先写入自己的头数据段
        nPos += sizeof(Frame_Data_Header);
        std::vector<std::vector<uint8_t>> vecTempData = _onlyData[i];
        for (size_t p = 0; p < vecTempData.size(); p++)
        {
            memcpy(&data[nPos], &vecTempData[p][0], _dataHead[i].LyrLen[p]);
            nPos += _dataHead[i].LyrLen[p];
        }
    }
    getXor(data, data.size());

    return data;
}

std::vector<uint8_t> FlashPartitionOsd::getPartitionDataDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    int nDataSize = 0;
    for (size_t i = 0; i < _dataHead.size(); i++)
    {
        nDataSize += sizeof(Frame_Data_Header);
        for (size_t p = 0; p < 8; p++)
        {
            nDataSize += _dataHead[i].LyrLen[p];
        }
    }

    //拼接子头子数据
    std::vector<uint8_t> data(nDataSize);
    size_t nPos = 0;
    for (size_t i = 0; i < _dataHead.size(); i++)
    {
        memcpy(&data[nPos], &_dataHead[i], sizeof(Frame_Data_Header));//每段先写入自己的头数据段
        nPos += sizeof(Frame_Data_Header);
        std::vector<std::vector<uint8_t>> vecTempData = _onlyData[i];
        for (size_t p = 0; p < vecTempData.size(); p++)
        {
            memcpy(&data[nPos], &vecTempData[p][0], _dataHead[i].LyrLen[p]);
            nPos += _dataHead[i].LyrLen[p];
        }
    }

    return data;
}

std::vector<uint8_t> FlashPartitionOsd::getPartitionHeaderEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_head));
    memcpy(&data[0], &_head, sizeof(_head));
	getXor(data, data.size());

    return data;
}

std::vector<uint8_t> FlashPartitionOsd::getPartitionHeaderDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(_head));
    memcpy(&data[0], &_head, sizeof(_head));

    return data;
}

PartitionInfo FlashPartitionOsd::getPartitionInfo()
{
    return _info;
}

int FlashPartitionOsd::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionOsd::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = InitData();

    return nRet;
}

int FlashPartitionOsd::writePartitionDataToCfgFile(const char *path)
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

    std::string strCommbine = "[OSD]";
    strCommbine += "\nwork_mode=" + std::to_string(_head.work_mode);
    strCommbine += "\nsnr0_cfg_id=" + std::to_string(_head.snr0_cfg_id);
    strCommbine += "\nsnr1_cfg_id=" + std::to_string(_head.snr1_cfg_id);

    std::string strPath(path);
    size_t      nPos = strPath.rfind("\\");
    strPath          = strPath.substr(0, nPos + 1);
    strPath += "osdData.bin";
    strCommbine += "\ndataPath=" + strPath;

    file.write(strCommbine.data(), strCommbine.length());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    int nRet = writeDataToCfgFile(strPath.data());

    return nRet;
}

int FlashPartitionOsd::writeDataToCfgFile(const char *path)
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

    std::vector<uint8_t> vecAllData(_info.dataLength - 160);

    Frame_Data_Header tempDataHead;
    memcpy(&tempDataHead, &_onlyData[0][0], sizeof(Frame_Data_Header));
    //memcpy(&vecAllData[0], &tempDataHead.LyrAddr[0], sizeof(uint32_t));
    vecAllData[0] = 0x00;
    vecAllData[1] = 0x00;
    vecAllData[2] = 0x00;
    vecAllData[3] = 0x87;
    vecAllData[4] = _head.frame_num;
    vecAllData[5] = 0x00;
    vecAllData[6] = 0x00;
    vecAllData[7] = 0x00;

    uint32_t nHeadPos = 0;
    size_t   nDataPos = (uint32_t)(8 + _onlyData.size() * sizeof(uint32_t) );
    for (size_t i = 0; i < _onlyData.size(); i++)
    {
        //存入头的地址
        uint32_t  frame_off_in = (uint32_t)(8 + 4 * i);
        nHeadPos               = (uint32_t)nDataPos;
        memcpy(&vecAllData[frame_off_in], (char*)&nHeadPos, sizeof(uint32_t));
        Frame_Header_In   tempHeaderIn      = {};
        Frame_Data_Header frame_data_header = _dataHead[i];
        int               lyr_num           = 0;
        nDataPos          = nHeadPos + sizeof(Frame_Header_In) - sizeof(uint32_t);
        int nLyrNum                         = 0;
        for (lyr_num = 0; lyr_num < 8; lyr_num++)
		{
            if (frame_data_header.LyrLen[lyr_num] == 0)
                break;
            tempHeaderIn.length[lyr_num] = frame_data_header.LyrLen[lyr_num];
            tempHeaderIn.coord[lyr_num]        = frame_data_header.LyrCoord[lyr_num];
            tempHeaderIn.sram_length[lyr_num]  = frame_data_header.SramLen[lyr_num];
            //存入数据
            memcpy(&vecAllData[nDataPos], &_onlyData[i][lyr_num][0], tempHeaderIn.length[lyr_num]);
            nDataPos += tempHeaderIn.length[lyr_num];
            nDataPos = (size_t)std::ceil(((double)nDataPos / 4)) * 4;
            nLyrNum++;
        }
        tempHeaderIn.reserve        = nLyrNum;
        tempHeaderIn.default_switch = frame_data_header.LyrSwitch;
        for (int j = 0; j < 30; j++)
        {
            for (int k = 0; k < 3; k++)
                tempHeaderIn.color_lut[3 * j + k] = (frame_data_header.ColorLut[j] >> (k * 8)) & (0xff);
        }
        //存入头
        memcpy(&vecAllData[nHeadPos], (char *)&tempHeaderIn, sizeof(Frame_Header_In) - sizeof(uint32_t));
    }

    file.write((char*)&vecAllData[0], vecAllData.size());

    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionOsd::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() < 1) {
        return ERROR_IMG_EMPTY;
    }

    std::vector<uint8_t> vecSourceData(data);
    //解码源数据
    getXor((char *)(&vecSourceData[0]), vecSourceData.size());

    OSD_Header tempHead;
    memcpy(&tempHead, &vecSourceData[0], sizeof(OSD_Header));
    if (tempHead.frame_num == 0 || tempHead.magic_number == 0) 
    {
        return ERROR_IMG_INVALID;
    }
    
    size_t nPos  = sizeof(OSD_Header);
    std::vector<Frame_Data_Header> DataHead;
    std::vector<std::vector<std::vector<uint8_t>>> OnlyData;
    for (size_t i = 0; i < tempHead.frame_num; i++)
    {
        std::vector<std::vector<uint8_t>> tempFrameData;
        Frame_Data_Header tempSubHead;
        memcpy(&tempSubHead, &vecSourceData[nPos], sizeof(Frame_Data_Header));//获取子头
        DataHead.push_back(tempSubHead);
        nPos += sizeof(Frame_Data_Header);
        for (size_t p = 0; p < 8; p++)
        {
            if (DataHead[i].LyrLen[p] < 1)
            {
                break;
            }
            size_t nTempSize = DataHead[i].LyrLen[p];
            std::vector<uint8_t> tempData(nTempSize);
            memcpy(&tempData[0], &vecSourceData[nPos], DataHead[i].LyrLen[p]);
            nPos += DataHead[i].LyrLen[p];
            int nCrc = getOnlyCrc((char*)(&tempData[0]), DataHead[i].LyrLen[p]);//数据子头解析数据并获取crc
            if (nCrc != DataHead[i].crc[p])
            {
                return ERROR_IMG_INVALID;
            }
            tempFrameData.push_back(tempData);
        }
        OnlyData.push_back(tempFrameData);
    }

    //存入新头
    memcpy(&_head, &tempHead, sizeof(OSD_Header));
    //清除旧数据
    _onlyData.clear();
    _onlyData.swap(OnlyData);
    _dataHead.clear();
    _dataHead.swap(DataHead);

    _info.dataLength = (uint32_t)nPos - 10;//偏移10
    _info.size       = (uint32_t)(sizeof(OSD_Header) + _info.dataLength);

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionOsd::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionOsd::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionOsd::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionOsd::InitHead(){
    if (_fileName == "")
    {
        return;
    }

    mINI::INIFile file(_fileName);
    mINI::INIStructure ini;
    file.read(ini);

    std::string str;
    str = ini.get("OSD").get("work_mode");
    str             = str == "" ? "0" : str;
    _head.work_mode = std::stoi(Trimstring(str));

    str = ini.get("OSD").get("snr0_cfg_id");
    str             = str == "" ? "0" : str;
    _head.snr0_cfg_id = std::stoi(Trimstring(str));

    str = ini.get("OSD").get("snr1_cfg_id");
    str             = str == "" ? "0" : str;
    _head.snr1_cfg_id = std::stoi(Trimstring(str));

    _fileName = ini.get("OSD").get("dataPath");

    _head.magic_number = MAGIC_NUM_OSD;
    _head.version      = 0;
}

int FlashPartitionOsd::InitData()
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
    uint32_t nFilelen = (uint32_t)_fileIn.tellg();
    if (nFilelen < 1) {
        return ERROR_CONFIG_FILE_READ;
    }
    std::vector<uint8_t> tempData(nFilelen);
    _fileIn.seekg(0);
    _fileIn.read((char *)(&tempData[0]), nFilelen);
    _fileIn.close();

    size_t nDataSize       = 0;
    _head.frame_num       = tempData[4];
    uint32_t lyraddr       = 0;
    memcpy(&lyraddr, (char*)&tempData[0], sizeof(uint32_t));
    uint32_t frame_off = sizeof(OSD_Header) + _head.frame_num * 8;
    for (size_t i = 0; i < _head.frame_num; i++)
    {
        uint32_t frame_off_in = *(uint32_t*)(&tempData[0] + 8 + 4 * i);
        Frame_Header_In  *frame_header_in   = (Frame_Header_In *)(&tempData[frame_off_in]);
        Frame_Data_Header frame_data_header = {};
        int               lyr_num;

        //frame的数据数组
        std::vector<std::vector<uint8_t>>    tempSubData;
		uint32_t out_length = 0;

		for (lyr_num = 0; lyr_num < 8; lyr_num++)
		{
			frame_data_header.LyrLen[lyr_num] = frame_header_in->length[lyr_num];
			if (frame_data_header.LyrLen[lyr_num] == 0)
				break;
			frame_data_header.LyrAddr[lyr_num] = lyraddr;
			lyraddr += (uint32_t)std::ceil(((double)frame_data_header.LyrLen[lyr_num] / 4)) * 4;

			frame_data_header.LyrCoord[lyr_num] = frame_header_in->coord[lyr_num];
			frame_data_header.SramLen[lyr_num] = frame_header_in->sram_length[lyr_num];

            //Frame最小的数据组
            std::vector<uint8_t> tempMinData(frame_data_header.LyrLen[lyr_num]);
            memcpy(&tempMinData[0], ((char*)(&frame_header_in->data)) + out_length, frame_data_header.LyrLen[lyr_num]);
            tempSubData.push_back(tempMinData);

            frame_data_header.crc[lyr_num] = getOnlyCrc((char *)&tempMinData[0], frame_data_header.LyrLen[lyr_num]);
            out_length += (uint32_t)std::ceil(((double)frame_data_header.LyrLen[lyr_num] / 4)) * 4;
		}
		frame_data_header.LyrSwitch = frame_header_in->default_switch;
		for (int j = 0; j < 30; j++)
		{
			for (int k = 0; k < 3; k++)
				frame_data_header.ColorLut[j] |= ((uint32_t)(frame_header_in->color_lut[3 * j + k]) & 0xff) << (8 * k);
		}

		uint32_t offset = sizeof(Frame_Data_Header);
        //存入纯数据
        _onlyData.push_back(tempSubData);
        //存入子头
        _dataHead.emplace_back(frame_data_header);
        nDataSize += out_length + offset;
    }

    _info.dataLength = (uint32_t)nDataSize;
    _info.size       = sizeof(OSD_Header) + _info.dataLength;

    _isInit = true;
    return ERROR_NO_ERR;
}
