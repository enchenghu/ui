#include "FlashPartitionSensorSetting.h"
#include <filesystem>

FlashPartitionSensorSetting::FlashPartitionSensorSetting()
{
    _isHeader = false;
    _isInit   = false;

    _info.type       = PART_SENSOR_SETTING;
    _info.size       = 0;
    _info.dataLength = 0;

    // 头内第一段全部置0
    memset(&_sensorHead, 0, sizeof(Sensor_Header));
    _sensorHead.magic_number = MAGIC_NUM_SENSOR;
    _sensorHead.version      = 0;
}

FlashPartitionSensorSetting::~FlashPartitionSensorSetting() { }

std::vector<uint8_t> FlashPartitionSensorSetting::getPartitionDataEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    auto data = getPartitionDataDecrypted();
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionSensorSetting::getPartitionDataDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    size_t               nLastSize = 0;
    std::vector<uint8_t> data(_info.dataLength);
    for (int i = 0; i < _sensorData.size(); i++) {
        memcpy(&data[nLastSize], &_sensorData[i][0], _sensorData[i].size());
        nLastSize += _sensorData[i].size();
    }

    return data;
}

std::vector<uint8_t> FlashPartitionSensorSetting::getPartitionHeaderEncrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    auto data = getPartitionHeaderDecrypted();
    getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionSensorSetting::getPartitionHeaderDecrypted()
{
    if (!_isInit) {
        // 若未初始化
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> data(sizeof(Sensor_Header) + sizeof(Sensor_Setting_Header) * _sensorHead.num);
    // 复制第一段头
    memcpy(&data[0], &_sensorHead, sizeof(Sensor_Header));
    for (int i = 0; i < _sensorHead.num; i++) {
        // 复制第i个的第二段头
        memcpy(&data[sizeof(Sensor_Header) + i * sizeof(Sensor_Setting_Header)], &_settingHead[i],
            sizeof(Sensor_Setting_Header));
    }
    return data;
}

PartitionInfo FlashPartitionSensorSetting::getPartitionInfo()
{
    return _info;
}

int FlashPartitionSensorSetting::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::readPartitionDataFromCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    _fileName = path;

    int nRet = readPartitionHeadFromCfgFile();
    if (nRet != ERROR_NO_ERR) {
        return nRet;
    }

    nRet = InitData();

    return nRet;
}

int FlashPartitionSensorSetting::writePartitionDataToCfgFile(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit) {
        // 若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_sensorHead.num < 1) {
        return ERROR_IMG_EMPTY;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }

    std::string strCommbine = "[SensorHead]";
    strCommbine += "\nSensorNumber=" + std::to_string(_sensorHead.num);
    strCommbine += "\nXshutdownen=" + std::to_string(_sensorHead.xshutdown_en);
    strCommbine += "\nMclken=" + std::to_string(_sensorHead.mclk_en);
    strCommbine += "\nHwrstdlyus=" + std::to_string(_sensorHead.hw_rst_dly_us);
    strCommbine += "\nMclkdelay=" + std::to_string(_sensorHead.mclk_en_dly);
    strCommbine += "\nHwrsthold=" + std::to_string(_sensorHead.hw_rst_hold_time);
    strCommbine += "\nEfsyncvalid=" + std::to_string(_sensorHead.efsync_valid);
    strCommbine += "\nEfsyncpwmid=" + std::to_string(_sensorHead.efsync_pwm_idx);
    strCommbine += "\npwmdiv=" + std::to_string(_sensorHead.pwm_div);
    strCommbine += "\nBeforepwmdly=" + std::to_string(_sensorHead.bf_pwm_dly_ms);
    strCommbine += "\nPwmtotalcycle=" + std::to_string(_sensorHead.pwm_total_cycle);
    strCommbine += "\nPwmdutycycle=" + std::to_string(_sensorHead.pwm_duty_cycle);
    if (_sensorHead.num == 1) {
        strCommbine += "\nSensor1Path=SensorData1.ini";
        strCommbine += "\nSensor2Path=";
        writeSecondHeadToStr(strCommbine, "1");
        _dataPath.resize(0);
        _dataPath.emplace_back("SensorData1.ini");

    } else if (_sensorHead.num == 2) {
        strCommbine += "\nSensor1Path=SensorData1.ini";
        strCommbine += "\nSensor2Path=SensorData2.ini";
        writeSecondHeadToStr(strCommbine, "1");
        writeSecondHeadToStr(strCommbine, "2");
        _dataPath.resize(0);
        _dataPath.emplace_back("SensorData1.ini");
        _dataPath.emplace_back("SensorData2.ini");
    }
    file.write(strCommbine.data(), strCommbine.length());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    // get the folder path
    std::filesystem::path mainSettingPath(path);
    std::string           folderPath = mainSettingPath.parent_path().string();

    for (size_t i = 0; i < _dataPath.size(); i++) {
        _dataPath[i] = folderPath + "\\" + _dataPath[i];
        std::filesystem::path sensorDataPath(_dataPath[i]);
        if (!std::filesystem::exists(sensorDataPath.parent_path())) {
            std::filesystem::create_directories(sensorDataPath.parent_path());
        }
        writeDataToCfgFile(_dataPath[i].data(), _sensorData[i], static_cast<int>(i));
    }

    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::readPartitionHeadFromCfgFile()
{
    mINI::INIFile      file(_fileName);
    mINI::INIStructure ini;
    if (!file.read(ini)) {
        return ERROR_CONFIG_FILE_READ;
    }

    std::string str;
    str = ini.get("SensorHead").get("SensorNumber");
    _headFirst.insert(std::pair("SensorNumber", str));
    if (str != "1" && str != "2") {
        return ERROR_CONFIG_FILE_READ;
    }

    // 读入数据头
    str = ini.get("SensorHead").get("Xshutdownen");
    _headFirst.insert(std::pair("Xshutdownen", str));

    str = ini.get("SensorHead").get("Mclken");
    _headFirst.insert(std::pair("Mclken", str));

    str = ini.get("SensorHead").get("Hwrstdlyus");
    _headFirst.insert(std::pair("Hwrstdlyus", str));

    str = ini.get("SensorHead").get("Mclkdelay");
    _headFirst.insert(std::pair("Mclkdelay", str));

    str = ini.get("SensorHead").get("Hwrsthold");
    _headFirst.insert(std::pair("Hwrsthold", str));

    str = ini.get("SensorHead").get("Efsyncvalid");
    _headFirst.insert(std::pair("Efsyncvalid", str));

    str = ini.get("SensorHead").get("Efsyncpwmid");
    _headFirst.insert(std::pair("Efsyncpwmid", str));

    str = ini.get("SensorHead").get("pwmdiv");
    _headFirst.insert(std::pair("pwmdiv", str));

    str = ini.get("SensorHead").get("Beforepwmdly");
    _headFirst.insert(std::pair("Beforepwmdly", str));

    str = ini.get("SensorHead").get("Pwmtotalcycle");
    _headFirst.insert(std::pair("Pwmtotalcycle", str));

    str = ini.get("SensorHead").get("Pwmdutycycle");
    _headFirst.insert(std::pair("Pwmdutycycle", str));

    int nRet = 0;
    str      = ini.get("SensorHead").get("Sensor1Path");
    if (str != "") {
        std::string strPath(_fileName);
        size_t      nPos = strPath.rfind("\\");
        nPos             = nPos < strPath.rfind("/") ? nPos : strPath.rfind("/");
        strPath          = strPath.substr(0, nPos + 1);
        str              = strPath + str;
        _dataPath.push_back(str);
        nRet = readSubHeadFromCfgFile(ini, "1");
        if (nRet != ERROR_NO_ERR) {
            return nRet;
        }
    }

    str = ini.get("SensorHead").get("Sensor2Path");
    if (str != "") {
        std::string strPath(_fileName);
        size_t      nPos = strPath.rfind("\\");
        nPos             = nPos < strPath.rfind("/") ? nPos : strPath.rfind("/");
        strPath          = strPath.substr(0, nPos + 1);
        str              = strPath + str;
        _dataPath.push_back(str);
        nRet = readSubHeadFromCfgFile(ini, "2");
        if (nRet != ERROR_NO_ERR) {
            return nRet;
        }
    }

    _isHeader = true;

    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::readSubHeadFromCfgFile(const mINI::INIStructure ini, const char *num)
{
    int         nPos    = str2Num<int>(num) - 1;
    std::string strHead = "Sensor";
    strHead += num;

    std::string                        str;
    std::map<std::string, std::string> mapFirst;
    _headSecond.push_back(mapFirst);
    // 读入第一个sensor头
    str = ini.get(strHead).get("SensorID");
    _headSecond[nPos].insert(std::pair("SensorID", str));

    str = ini.get(strHead).get("Swrstreg");
    _headSecond[nPos].insert(std::pair("Swrstreg", str));

    str = ini.get(strHead).get("Swrstval");
    _headSecond[nPos].insert(std::pair("Swrstval", str));

    str = ini.get(strHead).get("Swrlsval");
    _headSecond[nPos].insert(std::pair("Swrlsval", str));

    str = ini.get(strHead).get("Swrstdly");
    _headSecond[nPos].insert(std::pair("Swrstdly", str));

    str = ini.get(strHead).get("Streamonreg");
    _headSecond[nPos].insert(std::pair("Streamonreg", str));

    str = ini.get(strHead).get("Enval");
    _headSecond[nPos].insert(std::pair("Enval", str));

    str = ini.get(strHead).get("Disval");
    _headSecond[nPos].insert(std::pair("Disval", str));

    mINI::INIFile      file(_dataPath[nPos]);
    mINI::INIStructure iniTemp;
    if (!file.read(iniTemp)) {
        return ERROR_CONFIG_FILE_READ;
    }

    str = iniTemp.get("Sensor").get("SlaveID");
    _headSecond[nPos].insert(std::pair("SlaveID", str));

    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::readDataFromCfgFile(const char *path)
{
    std::vector<uint8_t>   vecDataTemp;
    std::ifstream          in;
    std::string            line;
    unsigned int           data = 0;
    int                    num = 0, begin = 0;
    int                    mclk, slave_id;
    std::string::size_type space_pos = 0;
    in.open(path, std::ios::in | std::ios::binary);

    while (!in.eof()) {
        getline(in, line);
        if (!line.compare(0, 4, "mclk", 0, 4)) {
            space_pos = line.find('=', 0);
            mclk      = str2Num<int>(line.substr(space_pos + 1));
            continue;
        } else if (!line.compare(0, 7, "SlaveID", 0, 7)) {
            space_pos = line.find('=', 0);
            slave_id  = str2Num<int>(line.substr(space_pos + 1), 16);
            continue;
        }
        if (line.empty())
            continue;
        if (!line.compare(0, 8, "[DPC on]", 0, 8))
            break;
        else if (begin && line[0] != '/' && line[0] != ';' && line.npos != (space_pos = line.find(',', 0))) {
            std::string            sec_str = line.substr(space_pos + 1);
            std::string::size_type wo_pos  = sec_str.find(',', 0);
            std::string            wo_str  = sec_str.substr(wo_pos + 1);
            int                    nAddr   = str2Num<int>(line, 16) << 16;
            int                    nValue  = str2Num<int>(sec_str, 16);
            data                           = nAddr | nValue;
            wo_str                         = Trimstring(wo_str);
            if (!wo_str.compare(0, 4, "//WO", 0, 4)) {
                data |= 1 << 8;
            }
            if (!data)
                continue;
            if ((data >> 16) == 0x2100)
                continue;
            vecDataTemp.push_back((uint8_t)(data & 0xff));
            vecDataTemp.push_back((uint8_t)((data >> 8) & 0xff));
            vecDataTemp.push_back((uint8_t)((data >> 16) & 0xff));
            vecDataTemp.push_back((uint8_t)((data >> 24) & 0xff));
            num++;
            data = 0;
        } else if (!line.compare(0, 10, "[ParaList]", 0, 10))
            begin = 1;
        line.clear();
    }

    in.close();

    _sensorData.push_back(vecDataTemp);

    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::writeSecondHeadToStr(std::string &commbineStr, const char *numStr)
{
    // 索引从0开始，序号从1开始
    size_t nIndex = str2Num<int>(numStr) - 1;
    commbineStr += "\n[Sensor";
    commbineStr += +numStr;
    commbineStr += "]";
    commbineStr += "\nSensorID=" + std::to_string(_settingHead[nIndex].sensor_id);
    commbineStr += "\nSwrstreg=" + std::to_string(_settingHead[nIndex].sw_rst_reg);
    commbineStr += "\nSwrstval=" + std::to_string(_settingHead[nIndex].rst_val);
    commbineStr += "\nSwrlsval=" + std::to_string(_settingHead[nIndex].release_val);
    commbineStr += "\nSwrstdly=" + std::to_string(_settingHead[nIndex].sw_rst_dly);
    commbineStr += "\nStreamonreg=" + std::to_string(_settingHead[nIndex].stream_on_reg);
    commbineStr += "\nEnval=" + std::to_string(_settingHead[nIndex].en_val);
    commbineStr += "\nDisval=" + std::to_string(_settingHead[nIndex].dis_val);

    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::writeDataToCfgFile(const char *path, const std::vector<uint8_t> data, int nNow)
{
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_IMG_FILE_WRITE;
    }
    std::string strData = "[Sensor]";
    int         nData   = 0;
    strData += "\nSlaveID=0x";
    strData += decimalToHex(_settingHead[nNow].i2c_slv_addr);
    file.write(strData.data(), strData.length());
    strData = "\n[ParaList]";
    file.write(strData.data(), strData.length());
    for (size_t i = 0; i < data.size(); i += 4) {
        strData = "\n";
        strData += num2Str(*(uint16_t *)(&data[i + 2]), 16, 4, '0', "0x");
        strData += ",";
        strData += num2Str(*(uint8_t *)(&data[i]), 16, 2, '0', "0x");
        file.write(strData.data(), strData.length());
    }

    file.close();
    return ERROR_NO_ERR;
}

int FlashPartitionSensorSetting::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
{
    if (data.size() < sizeof(sensor_Header)) {
        return ERROR_IMG_EMPTY;
    }
    _settingHead.clear(); // 第二帧头数据map的vector
    _sensorData.clear();

    std::vector<uint8_t> AllData(data);
    // 解码源数据
    getXor((char *)(&AllData[0]), AllData.size());

    sensor_Header tmpSensorHeader;
    memcpy(&tmpSensorHeader, &AllData[0], sizeof(sensor_Header));
    if (tmpSensorHeader.magic_number != MAGIC_NUM_SENSOR) {
        return ERROR_IMG_INVALID;
    }
    _info.dataLength = 0;
    // 获取分段头和数据段
    for (int nPos = sizeof(sensor_Header), i = 0; i < tmpSensorHeader.num; i++) {
        // 获取数据头
        Sensor_Setting_Header tempHead;
        memcpy(&tempHead, &AllData[nPos], sizeof(Sensor_Setting_Header));
        if (tempHead.init_data_len < 1 || tempHead.data_crc == 0) {
            break;
        }
        nPos += sizeof(Sensor_Setting_Header);

        // 获取数据段
        std::vector<uint8_t> tempData(tempHead.init_data_len);
        memcpy(&tempData[0], &AllData[nPos], tempHead.init_data_len * sizeof(uint8_t));
        int nCrc = getOnlyCrc((char *)(&tempData[0]), tempData.size());
        if (nCrc != tempHead.data_crc) {
            return ERROR_IMG_INVALID;
        }
        nPos += tempHead.init_data_len * sizeof(uint8_t);

        _settingHead.push_back(tempHead);
        _sensorData.push_back(tempData);
        // 记录总数据长度
        _info.dataLength += static_cast<uint32_t>(tempData.size());
    }
    memcpy(&_sensorHead, &tmpSensorHeader, sizeof(sensor_Header));

    // 记录带头总长度
    _info.size =
        (uint32_t)sizeof(Sensor_Header) + _sensorHead.num * (uint32_t)sizeof(Sensor_Setting_Header) + _info.dataLength;

    _isInit = true;

    return ERROR_NO_ERR;
}

PartitionType FlashPartitionSensorSetting::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionSensorSetting::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionSensorSetting::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionSensorSetting::InitHead()
{
    // 设置第一段数据头
    _sensorHead.num              = str2Num<uint8_t>(_headFirst["SensorNumber"]);
    _sensorHead.xshutdown_en     = str2Num<uint8_t>(_headFirst["Xshutdownen"]); //
    _sensorHead.mclk_en          = str2Num<uint8_t>(_headFirst["Mclken"]);
    _sensorHead.hw_rst_dly_us    = str2Num<uint8_t>(_headFirst["Hwrstdlyus"]);
    _sensorHead.mclk_en_dly      = str2Num<uint16_t>(_headFirst["Mclkdelay"]);
    _sensorHead.hw_rst_hold_time = str2Num<uint16_t>(_headFirst["Hwrsthold"]);
    _sensorHead.efsync_valid     = str2Num<uint8_t>(_headFirst["Efsyncvalid"]);
    _sensorHead.efsync_pwm_idx   = str2Num<uint8_t>(_headFirst["Efsyncpwmid"]);
    _sensorHead.pwm_div          = str2Num<uint8_t>(_headFirst["pwmdiv"]);
    _sensorHead.bf_pwm_dly_ms    = str2Num<uint8_t>(_headFirst["Beforepwmdly"]);
    _sensorHead.pwm_total_cycle  = str2Num<uint16_t>(_headFirst["Pwmtotalcycle"]);
    _sensorHead.pwm_duty_cycle   = str2Num<uint16_t>(_headFirst["Pwmdutycycle"]);

    // 设置第二段数据头
    _settingHead.clear();
    for (int i = 0; i < _sensorHead.num; i++) {
        Sensor_Setting_Header tempHead;
        std::string           str;
        str                    = _headSecond[i]["SensorID"] == "" ? "0" : _headSecond[i]["SensorID"];
        tempHead.sensor_id     = str2Num<uint16_t>(str);
        str                    = _headSecond[i]["SlaveID"] == "" ? "0" : _headSecond[i]["SlaveID"];
        tempHead.i2c_slv_addr  = str2Num<uint16_t>(str, 16);
        str                    = _headSecond[i]["Swrstreg"] == "" ? "0" : _headSecond[i]["Swrstreg"];
        tempHead.sw_rst_reg    = str2Num<uint16_t>(str);
        str                    = _headSecond[i]["Swrstval"] == "" ? "0" : _headSecond[i]["Swrstval"];
        tempHead.rst_val       = str2Num<uint8_t>(str);
        str                    = _headSecond[i]["Swrlsval"] == "" ? "0" : _headSecond[i]["Swrlsval"];
        tempHead.release_val   = str2Num<uint8_t>(str);
        str                    = _headSecond[i]["Swrstdly"] == "" ? "0" : _headSecond[i]["Swrstdly"];
        tempHead.sw_rst_dly    = str2Num<uint16_t>(str);
        str                    = _headSecond[i]["Streamonreg"] == "" ? "0" : _headSecond[i]["Streamonreg"];
        tempHead.stream_on_reg = str2Num<uint16_t>(str);
        str                    = _headSecond[i]["Enval"] == "" ? "0" : _headSecond[i]["Enval"];
        tempHead.en_val        = str2Num<uint8_t>(str);
        str                    = _headSecond[i]["Disval"] == "" ? "0" : _headSecond[i]["Disval"];
        tempHead.dis_val       = str2Num<uint8_t>(str);

        _settingHead.push_back(tempHead);
    }
}

int FlashPartitionSensorSetting::InitData()
{
    if (!_isHeader) {
        return ERROR_CONFIG_FILE_READ;
    }
    InitHead();

    uint16_t nOff = _sensorHead.num * sizeof(Sensor_Setting_Header) + sizeof(Sensor_Header);
    // 需要循环读第二段数据头及数据
    for (int i = 0; i < _sensorHead.num; i++) {
        /*if(isRelativePath(_dataPath[i].data()))
        {
            std::string str = getAbsolutePath(_fileName.c_str());
            str = str.substr(0, str.find_last_of("\\") + 1);
            str += _dataPath[i];
            _dataPath[i] = str;
        }*/
        int nRet = readDataFromCfgFile(_dataPath[i].data());
        if (nRet != ERROR_NO_ERR) {
            return nRet;
        }

        // 第二段头记录本段数据长度
        _settingHead[i].init_data_len = (uint16_t)(_sensorData[i].size());
        _settingHead[i].data_crc      = getOnlyCrc((char *)(&_sensorData[i][0]), _settingHead[i].init_data_len);
        //_settingHead[i].i2c_slv_addr  = _sensorData[i][0] + 1;
        _settingHead[i].data_off = nOff;
        nOff += (uint16_t)(_sensorData[i].size() - 8);

        // 记录总数据长度
        _info.dataLength += static_cast<uint32_t>(_sensorData[i].size());
    }

    // 记录带头总长度
    _info.size =
        (uint32_t)sizeof(Sensor_Header) + _sensorHead.num * (uint32_t)sizeof(Sensor_Setting_Header) + _info.dataLength;

    _isInit = true;

    return ERROR_NO_ERR;
}
