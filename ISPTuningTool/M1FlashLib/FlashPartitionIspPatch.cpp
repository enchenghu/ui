#include "FlashPartitionIspPatch.h"
#include "ini.h"

Patch_Header             FlashPartitionPatchBase::_ispHead;
std::vector<Function_Header> FlashPartitionPatchBase::_funcHeader;
std::vector<uint8_t>     FlashPartitionPatchBase::_bootData;
std::vector<uint32_t>    FlashPartitionPatchBase::_ispTableData;
std::vector<uint8_t>     FlashPartitionPatchBase::_ispData;
PartitionInfo            FlashPartitionPatchBase::_info;
bool                     FlashPartitionPatchBase::_isInit;//是否已经初始化过
std::string              FlashPartitionPatchBase::_funcFileName;
std::string              FlashPartitionPatchBase::_bootFileName;
std::string              FlashPartitionPatchBase::_tableFileName;
std::string              FlashPartitionPatchBase::_ispFileName;
std::ifstream            FlashPartitionPatchBase::_fileIn;

FlashPartitionPatchBase::FlashPartitionPatchBase()
{
    _isInit         = false;
    _info.type       = PART_FW_PATCH;

    memset(&_ispHead, 0, sizeof(_ispHead));
    _ispHead.magic_number = MAGIC_NUM_PATCH;
    _ispHead.version      = 0;
}

FlashPartitionPatchBase::~FlashPartitionPatchBase() { }

std::vector<uint8_t> FlashPartitionPatchBase::getPartitionDataEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    //复制三段数据
    size_t               nPos = 0;
    std::vector<uint8_t> tampData(_info.dataLength);
    if (_bootData.size() > 0)
    {
        memcpy(&tampData[nPos], &_bootData[0], _bootData.size());
        nPos += _bootData.size();
    }
    if (_ispTableData.size() > 0)
    {
        memcpy(&tampData[nPos], &_ispTableData[0], _ispTableData.size() * sizeof(uint32_t));
        nPos += _ispTableData.size() * sizeof(uint32_t);
    }
    if (_ispData.size() > 0)
    {
        memcpy(&tampData[nPos], &_ispData[0], _ispData.size());
        nPos += _ispData.size();
    }

    getXor(tampData, tampData.size());

    return tampData;
}

std::vector<uint8_t> FlashPartitionPatchBase::getPartitionDataDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    //复制三段数据
    size_t               nPos = 0;
    std::vector<uint8_t> tampData(_info.dataLength);
    if (_bootData.size() > 0)
    {
        memcpy(&tampData[nPos], &_bootData[0], _bootData.size());
        nPos += _bootData.size();
    }
    if (_ispTableData.size() > 0)
    {
        memcpy(&tampData[nPos], &_ispTableData[0], _ispTableData.size() * sizeof(uint32_t));
        nPos += _ispTableData.size() * sizeof(uint32_t);
    }
    if (_ispData.size() > 0)
    {
        memcpy(&tampData[nPos], &_ispData[0], _ispData.size());
        nPos += _ispData.size();
    }

    return tampData;
}

std::vector<uint8_t> FlashPartitionPatchBase::getPartitionHeaderEncrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    //拼接头和函数数组
    std::vector<uint8_t> data(sizeof(_ispHead) + _ispHead.boot_patch_func_nums * sizeof(Function_Header));
    memcpy(&data[0], &_ispHead, sizeof(_ispHead));
    if (_ispHead.boot_patch_func_nums > 0)
    {
        memcpy(&data[sizeof(_ispHead)], &_funcHeader[0],
        _ispHead.boot_patch_func_nums * sizeof(Function_Header));
    }

	getXor(data, data.size());
    return data;
}

std::vector<uint8_t> FlashPartitionPatchBase::getPartitionHeaderDecrypted()
{
    if (!_isInit)
    {
        //若未初始化
        return std::vector<uint8_t>();
    }

    //拼接头和函数数组
    std::vector<uint8_t> data(sizeof(_ispHead) + _ispHead.boot_patch_func_nums * sizeof(Function_Header));
    memcpy(&data[0], &_ispHead, sizeof(_ispHead));
    if (_ispHead.boot_patch_func_nums > 0)
    {
        memcpy(&data[sizeof(_ispHead)], &_funcHeader[0],
        _ispHead.boot_patch_func_nums * sizeof(Function_Header));
    }
    return data;
}

PartitionInfo FlashPartitionPatchBase::getPartitionInfo()
{
    return _info;
}

int FlashPartitionPatchBase::setPartitionInfo(PartitionInfo info)
{
    _info = info;
    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::readPartitionDataFromCfgFile(const char *path)
{
    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::writePartitionDataToCfgFile(const char *path)
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

    //写入四个文件的相对路径
    std::string strPath(path);
    size_t      nPos = strPath.rfind("\\");
    strPath          = strPath.substr(0, nPos + 1);

    std::string strFuncPath = strPath + "FuncPath.ini";
    std::string strBootPath = strPath + "BootPath.bin";
    std::string strIspTablePath = strPath + "IspTablePath.ini";
    std::string strIspPath = strPath + "IspPath.bin";

    std::string strWrite = "[Patch]\nFuncPath=";
    if (_ispHead.boot_patch_func_nums > 0)
    {
        strWrite += strFuncPath;
    }
    strWrite += "\nBootPath=";
    if (_ispHead.boot_patch_len > 0)
    {
        strWrite += strBootPath;
    }
    strWrite += "\nIspTablePath=";
    if (_ispHead.isp_patch_table_len > 0)
    {
        strWrite += strIspTablePath;
    }
    strWrite += "\nIspPath=";
    if (_ispHead.isp_patch_len > 0)
    {
        strWrite += strIspPath;
    }

    file.write(strWrite.data(), strWrite.length());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    WriteFunc(strFuncPath.data());
    WriteBoot(strBootPath.data());
    WriteIspTable(strIspTablePath.data());
    WriteIsp(strIspPath.data());

    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data)
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
    Patch_Header tempHead;
    memcpy(&tempHead, &vecSourceData[0], sizeof(Patch_Header));
    if (tempHead.magic_number != MAGIC_NUM_PATCH) 
    {
        return ERROR_IMG_INVALID;
    }

    size_t nPos = sizeof(Patch_Header);
    //若头附带函数,则获取头函数
    std::vector<Function_Header> funcTemp(tempHead.boot_patch_func_nums);
    if (tempHead.boot_patch_func_nums > 0)
    {
        memcpy(&funcTemp[0], &vecSourceData[nPos], tempHead.boot_patch_func_nums * sizeof(Function_Header));
        nPos += tempHead.boot_patch_func_nums * sizeof(Function_Header);
    }

    //获取boot数据段
    std::vector<uint8_t> vecBootData(tempHead.boot_patch_len);
    if (tempHead.boot_patch_len > 0)
    {
        memcpy(&vecBootData[0], &vecSourceData[tempHead.boot_patch_off], tempHead.boot_patch_len);
        //获取数据段crc并解析
        int nCrc = getOnlyCrc((char *)(&vecBootData[0]), vecBootData.size());
        if (nCrc != tempHead.boot_patch_crc) 
        {
            return ERROR_IMG_INVALID;
        }
    }

    //获取IspTable数据段
    std::vector<uint8_t> vecTableData(tempHead.isp_patch_table_len);
    if (tempHead.isp_patch_table_len > 0)
    {
        memcpy(&vecTableData[0], &vecSourceData[tempHead.isp_patch_table_off], tempHead.isp_patch_table_len);
        //获取数据段crc并解析
        int nCrc = getOnlyCrc((char *)(&vecTableData[0]), vecTableData.size());
        if (nCrc != tempHead.isp_patch_table_crc) 
        {
            return ERROR_IMG_INVALID;
        }
    }

    //获取Isp数据段
    std::vector<uint8_t> vecIspData(tempHead.isp_patch_len);
    if (tempHead.isp_patch_len > 0)
    {
        memcpy(&vecIspData[0], &vecSourceData[tempHead.isp_patch_off], tempHead.isp_patch_len);
        //获取数据段crc并解析
        int nCrc = getOnlyCrc((char *)(&vecIspData[0]), vecIspData.size());
        if (nCrc != tempHead.isp_patch_crc) 
        {
            return ERROR_IMG_INVALID;
        }
    }

    //存入数据头
    memcpy(&_ispHead, &tempHead, sizeof(Patch_Header));
    //存入函数
    _funcHeader.clear();
    _funcHeader.swap(funcTemp);
    //存入数据段
    _bootData.clear();
    _bootData.swap(vecBootData);
    _ispTableData.clear();
    _ispTableData.resize(vecTableData.size() / 4);
    memcpy(&_ispTableData[0], &vecTableData[0], vecTableData.size());
    _ispData.clear();
    _ispData.swap(vecIspData);
    
    //分区总长度
    _info.size = (uint32_t)(sizeof(Patch_Header) + _ispData.size() + _bootData.size()
        + _ispTableData.size() * sizeof(uint32_t) + _funcHeader.size() * sizeof(Function_Header));
    //数据长度
    _info.dataLength = (uint32_t)(_ispData.size() + _bootData.size() + _ispTableData.size() * sizeof(uint32_t));

    _isInit = true;

    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::setFunction()
{
    return ERROR_NO_ERR;
}

PartitionType FlashPartitionPatchBase::getPartitionType()
{
    return _info.type;
}

uint32_t FlashPartitionPatchBase::getPartitionSize()
{
    return _info.size;
}

uint32_t FlashPartitionPatchBase::getPartitionDataSize()
{
    return _info.dataLength;
}

void FlashPartitionPatchBase::InitHead()
{
    //设置三个数据区偏移量
    _ispHead.boot_patch_len      = (uint16_t)_bootData.size() * sizeof(uint8_t);
    _ispHead.boot_patch_off      = sizeof(Patch_Header) + _ispHead.boot_patch_func_nums * sizeof(Function_Header);
    if (_ispHead.boot_patch_len > 0)
    {
        _ispHead.boot_patch_crc = getOnlyCrc((char *)(&_bootData[0]), _bootData.size());
    }

    _ispHead.isp_patch_table_len = (uint16_t)_ispTableData.size() * sizeof(uint32_t);
    _ispHead.isp_patch_table_off = _ispHead.boot_patch_off + _ispHead.boot_patch_len;
    if (_ispHead.isp_patch_table_len > 0)
    {
        _ispHead.isp_patch_table_crc = getOnlyCrc((char *)(&_ispTableData[0]), _ispTableData.size() * sizeof(uint32_t));
    }
    
    _ispHead.isp_patch_len       = (uint16_t)_ispData.size() * sizeof(uint8_t);
    _ispHead.isp_patch_off       = _ispHead.isp_patch_table_off + _ispHead.isp_patch_table_len;
    if (_ispHead.isp_patch_len > 0)
    {
        _ispHead.isp_patch_crc       = getOnlyCrc((char *)(&_ispData[0]), _ispData.size());
    }

    //分区总长度
    _info.size = (uint32_t)(sizeof(Patch_Header) + _ispData.size() + _bootData.size()
        + _ispTableData.size() * sizeof(uint32_t) + _funcHeader.size() * sizeof(Function_Header));
    //数据长度
    _info.dataLength = (uint32_t)(_ispData.size() + _bootData.size() + _ispTableData.size() * sizeof(uint32_t));
}

int FlashPartitionPatchBase::InitData()
{
    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::WriteFunc(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_funcHeader.size() < 1)
    {
        return ERROR_IMG_EMPTY;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    for (size_t i = 0; i < _funcHeader.size(); i++)
    {
        std::string line = "";
        line += std::to_string(_funcHeader[i].func_table) + " ";
        line += std::to_string(_funcHeader[i].func_idx) + " ";
        line += "0x" + decimalToHex(_funcHeader[i].func_ptr) + "\n";
        file.write(line.data(), line.size());
    }
    
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::WriteBoot(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_bootData.size() < 1)
    {
        return ERROR_IMG_EMPTY;
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

int FlashPartitionPatchBase::WriteIspTable(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_ispTableData.size() < 1)
    {
        return ERROR_IMG_EMPTY;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    //从1开始
    for (size_t i = 1; i < _ispTableData.size(); i++)
    {
        std::string line = "";
        line += decimalToHex(_ispTableData[i]) + " ";
        i += 1;
        line += decimalToHex(_ispTableData[i]) + "\n";
        file.write(line.data(), line.size());
    }
    
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();

    return ERROR_NO_ERR;
}

int FlashPartitionPatchBase::WriteIsp(const char *path)
{
    if (path == "") {
        return ERROR_PATH_INVALID;
    }
    if (!_isInit)
    {
        //若未初始化
        return ERROR_IMG_EMPTY;
    }
    if (_ispData.size() < 1)
    {
        return ERROR_IMG_EMPTY;
    }

    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return ERROR_CONFIG_FILE_READ;
    }
    file.write((char *)(&_ispData[0]), _ispData.size());
    if (!file.good()) {
        return ERROR_CONFIG_FILE_WRITE;
    }
    file.close();
    return ERROR_NO_ERR;
}

//BootPatch
FlashPartitionBootPatch::FlashPartitionBootPatch()
{
}

FlashPartitionBootPatch::~FlashPartitionBootPatch() { }

int FlashPartitionBootPatch::setFunction()
{
    if (_funcFileName=="")
    {
        return ERROR_PATH_INVALID;
    }

    _funcHeader.resize(0);
    std::ifstream fin(_funcFileName, std::ios::binary);
    if (!fin.is_open()) {
        return ERROR_CONFIG_FILE;
    }

    std::string line;
	while (!fin.eof())
	{
		getline(fin, line);
		if (line.size() < 5)
			continue;

		size_t pos;
		pos = line.find(' ');
        Function_Header funcHead;
        funcHead.func_table = stoi(line);
		line = line.substr(pos + 1);
		funcHead.func_idx = stoi(line);
		pos = line.find(' ');
		line = line.substr(pos + 1);
		funcHead.func_ptr = stoul(line, NULL, 16);
        _funcHeader.push_back(funcHead);
    }
	fin.close();
    _ispHead.boot_patch_func_nums = (uint16_t)_funcHeader.size();

    return ERROR_NO_ERR;
}

int FlashPartitionBootPatch::InitData()
{
    if (_bootFileName == "") {
        return ERROR_PATH_INVALID;
    }
    _fileIn.open(_bootFileName, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0, std::ios::end);
    size_t filelen = _fileIn.tellg();
    if (filelen < 1) {
        return ERROR_CONFIG_FILE_READ;
    }
    _bootData.resize(filelen);
    _fileIn.seekg(0);
    _fileIn.read((char *)(&_bootData[0]), filelen);

    _fileIn.close();

    InitHead();

    _isInit = true;

    return ERROR_NO_ERR;
}

//IspPatchTable
FlashPartitionIspPatchTable::FlashPartitionIspPatchTable()
{
}

FlashPartitionIspPatchTable::~FlashPartitionIspPatchTable() { }

int FlashPartitionIspPatchTable::InitData()
{
    if (_tableFileName == "") {
        return ERROR_PATH_INVALID;
    }
    _fileIn.open(_tableFileName, std::ios::in);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    int isp_table_cnt = 0;
    std::string line;
    _ispTableData.resize(0);
    _ispTableData.emplace_back(0);
    while (!_fileIn.eof())
    {
        getline(_fileIn, line);
        if (line.size() < 5)
            continue;
        size_t pos;
        pos = line.find(' ');
        _ispTableData.emplace_back(stoi(line, NULL, 16));
        line = line.substr(pos + 1);
        _ispTableData.emplace_back(stoi(line, NULL, 16));
        isp_table_cnt++;
	}
	_ispTableData[0] = isp_table_cnt;

    _fileIn.close();

    InitHead();

    _isInit = true;
    
    return ERROR_NO_ERR;
}

//IspPatch
FlashPartitionIspPatch::FlashPartitionIspPatch()
{
}

FlashPartitionIspPatch::~FlashPartitionIspPatch() { }

int FlashPartitionIspPatch::InitData()
{
    if (_ispFileName == "") {
        return ERROR_PATH_INVALID;
    }
    _fileIn.open(_ispFileName, std::ios::in | std::ios::binary);
    if (!_fileIn.is_open())
    {
        return ERROR_CONFIG_FILE;
    }
    
    _fileIn.seekg(0, std::ios::end);
    size_t filelen = _fileIn.tellg();
    if (filelen < 1) {
        return ERROR_CONFIG_FILE_READ;
    }

    _ispData.resize(filelen);
    _fileIn.seekg(0);
    _fileIn.read((char *)(&_ispData[0]), filelen);

    _fileIn.close();

    InitHead();

    _isInit = true;
    
    return ERROR_NO_ERR;
}

//patch对外模块
FlashPartitionPatch::FlashPartitionPatch() { }

FlashPartitionPatch::~FlashPartitionPatch() { }

int FlashPartitionPatch::readPartitionDataFromCfgFile(const char *path)
{
    int nRet = ReadAllConfigFile(path);
    if (nRet != ERROR_NO_ERR)
    {
        return nRet;
    }
    FlashPartitionBootPatch::setFunction();
    FlashPartitionBootPatch::InitData();
    FlashPartitionIspPatchTable::InitData();
    FlashPartitionIspPatch::InitData();
    return ERROR_NO_ERR;
}

int FlashPartitionPatch::ReadAllConfigFile(const char *path)
{
    if (path == "")
    {
        return ERROR_PATH_INVALID;
    }

    mINI::INIFile file(path);
    mINI::INIStructure ini;
    file.read(ini);

    _funcFileName = ini.get("Patch").get("FuncPath");
    _bootFileName= ini.get("Patch").get("BootPath");
    _tableFileName= ini.get("Patch").get("IspTablePath");
    _ispFileName= ini.get("Patch").get("IspPath");
    return ERROR_NO_ERR;
}