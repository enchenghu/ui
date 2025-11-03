#include "CommonFunction.h"
#include <filesystem>
#include <fstream>
#include <regex>

uint16_t CRCLut[256] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318,
    0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
    0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7,
    0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4,
    0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b,
    0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc,
    0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
    0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f,
    0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d,
    0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
    0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45,
    0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36,
    0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };

int getCRC(char *pBuf, size_t nLen)
{
    uint16_t crc = 0xffff;
    for (int i = 0; i < nLen; i++) {
        pBuf[i] ^= MAGIC_ENCODER;
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (pBuf[i] & 0xff)];
    }
    return crc;
}

int getCRC(std::vector<uint8_t> &buffer)
{
    uint16_t crc = 0xffff;
    for (int i = 0; i < buffer.size(); i++) {
        buffer[i] ^= MAGIC_ENCODER;
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (buffer[i] & 0xff)];
    }
    return crc;
}

void getXor(char *pBuf, size_t nLen)
{
    for (int i = 0; i < nLen; i++) {
        pBuf[i] ^= MAGIC_ENCODER;
    }
    return;
}

void getXor(std::vector<uint8_t> &pBuf, size_t nLen)
{
    for (int i = 0; i < nLen; i++) {
        pBuf[i] ^= MAGIC_ENCODER;
    }
    return;
}

void getXor(std::vector<uint8_t> &buffer)
{
    for (int i = 0; i < buffer.size(); i++) {
        buffer[i] ^= MAGIC_ENCODER;
    }
    return;
}

std::vector<uint8_t> getCopyXor(char *pBuf, int nLen)
{
    std::vector<uint8_t> buf(nLen);
    for (int i = 0; i < nLen; i++) {
        buf[i] = pBuf[i] ^ MAGIC_ENCODER;
    }
    return buf;
}

std::vector<uint8_t> getCopyXor(const std::vector<uint8_t> &buffer)
{
    std::vector<uint8_t> buf(buffer.size());
    for (int i = 0; i < buffer.size(); i++) {
        buf[i] = buffer[i] ^ MAGIC_ENCODER;
    }
    return buf;
}

int getOnlyCrc(char *pBuf, size_t nLen)
{
    uint16_t crc = 0xffff;
    char     cTemp;
    for (int i = 0; i < nLen; i++) {
        cTemp = pBuf[i];
        cTemp ^= MAGIC_ENCODER;
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (cTemp & 0xff)];
    }
    return crc;
}

int getOnlyCrc(const std::vector<uint8_t> &buffer, size_t len)
{
    uint16_t crc = 0xffff;
    char     cTemp;
    for (int i = 0; i < len; i++) {
        cTemp = buffer[i];
        cTemp ^= MAGIC_ENCODER;
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (cTemp & 0xff)];
    }
    return crc;
}

int getOnlyCrc(const std::vector<uint8_t> &buffer)
{
    uint16_t crc = 0xffff;
    char     cTemp;
    for (int i = 0; i < buffer.size(); i++) {
        cTemp = buffer[i];
        cTemp ^= MAGIC_ENCODER;
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (cTemp & 0xff)];
    }
    return crc;
}

int getOtpCrc(char *pBuf, int nLen)
{
    uint16_t crc = 0xffff;
    for (int i = 0; i < nLen; i++) {
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (pBuf[i] & 0xff)];
    }
    return crc;
}

int getOtpCrc(const std::vector<uint8_t> &buffer)
{
    uint16_t crc = 0xffff;
    for (int i = 0; i < buffer.size(); i++) {
        crc = ((crc << 8) & 0xFFFF) ^ CRCLut[(crc >> 8) ^ (buffer[i] & 0xff)];
    }
    return crc;
}

std::string &Trimstring(std::string &str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
}

bool isFileExist(const char *path)
{
    try {
        std::filesystem::path p(path);
        if (std::filesystem::exists(p) && std::filesystem::is_regular_file(p)) {
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool isDirExist(const char *path)
{
    try {
        std::filesystem::path p(path);
        if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool canCreateFile(const char *path)
{
    try {
        std::filesystem::path p(path);
        if (std::filesystem::exists(p)) {
            return false;
        }
        std::ofstream file(p, std::ios::out | std::ios::binary | std::ios::trunc);
        if (file.is_open()) {
            file.close();
            std::filesystem::remove(p);
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool isRelativePath(const char *path)
{
    try {
        std::filesystem::path p(path);
        if (p.is_relative()) {
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

std::string getAbsolutePath(const char *path)
{
    try {
        std::filesystem::path p(path);
        return p.is_absolute() ? p.string() : std::filesystem::absolute(p).string();
    } catch (...) {
        return "";
    }
    return "";
}

std::string getDirPath(const char *path)
{
    try {
        std::filesystem::path p(path);
        return p.parent_path().string();
    } catch (...) {
        return "";
    }
    return "";
}

// 函数：将十进制数字转换为16进制的字符串
std::string decimalToHex(size_t num, int size)
{
    std::string s;
    while (num) {
        int remainder = num % 16; // 拿到余数，对余数进行处理
        num           = num / 16; // 这两步可以对比以十进制获取一个整数的每一位
        if (remainder < 10) {
            s += remainder + '0'; // 将整数转为字符
        } else {
            s += remainder - 10 + 'A';
        }
    }
    std::reverse(s.begin(), s.end()); // 最后要进行翻转
    int nLast = size - (int)s.length();
    if (nLast > 0) {
        for (size_t i = 0; i < nLast; i++) {
            s = "0" + s;
        }
    }

    return s;
}

int setting_to_grp(std::string in_file, std::vector<uint8_t> &buffer, size_t &fileLen)
{
    std::ifstream in;
    in.open(in_file, std::ios::in | std::ios::binary);
    buffer.resize(0);
    in.seekg(0, std::ios::end);
    in.seekg(0);
    std::string          line;
    std::string          strTemp;
    std::vector<uint8_t> data_list;
    uint32_t             address, address0;
    int                  cnt = 0;
    int                  num = 0;

    while (!in.eof()) {
        address  = 0;
        address0 = 0;
        data_list.emplace_back(0);
        data_list.emplace_back(0);
        size_t s_pos = 0;
        while (data_list.size() < 4096 && !in.eof()) {
            getline(in, line);
            line = Trimstring(line);
            std::regex parser("^\\s*((0x|0X)?[0-9a-fA-F]+)\\s+((0x|0X)?[0-9a-fA-F]+)\\s+((0x|0X)?[0-9a-fA-F]+)\\s+([0-"
                              "9a-fA-F]+)\\s+([0-9a-fA-F]+)\\s+([0-9a-fA-F]+)\\s+([0-9a-fA-F]+)([^0-9a-fA-F].*)?$");

            std::smatch match;
            if (!std::regex_match(line, match, parser))
                continue;
            std::string addressString = match[3];
            std::string dataString    = match[5];
            std::string dataNumString = match[8];

            address = stoul(addressString, NULL, 16);

            if (address != address0) {

                if (data_list[s_pos] == 0x5f) {
                    data_list[s_pos] = (uint8_t)(0x50 + (data_list.size() - s_pos - 2));
                } else if (data_list[s_pos] == 0xaf) {
                    data_list[s_pos] = (uint8_t)(0xa0 + (data_list.size() - s_pos - 4));
                }
                if (address0 >> 16 != address >> 16) {
                    data_list.emplace_back(0xb0);
                    data_list.emplace_back((address >> 16) & 0xff);
                    data_list.emplace_back(address >> 24);
                }
                s_pos = data_list.size();
                data_list.emplace_back(0xaf);
                data_list.emplace_back(address & 0xff);
                data_list.emplace_back((address >> 8) & 0xff);
                address0 = address;
                cnt      = 0;
            }

            uint32_t data     = stoul(dataString, NULL, 16);
            uint32_t data_num = stoul(dataNumString, NULL, 16);

            for (size_t i = 0; i < data_num; i++) {
                if (cnt == 16) {
                    s_pos = data_list.size();
                    data_list.emplace_back(0x5f);
                    cnt = 0;
                }
                data_list.emplace_back((data >> (8 * i) & 0xff));
                cnt++;
                address0++;
            }
        }

        if (data_list[s_pos] == 0x5f) {
            data_list[s_pos] = (uint8_t)(0x50 + (data_list.size() - s_pos - 2));
        } else if (data_list[s_pos] == 0xaf) {
            data_list[s_pos] = (uint8_t)(0xa0 + (data_list.size() - s_pos - 4));
        }

        int size     = (int)(data_list.size());
        data_list[0] = (uint32_t)(size & 0xff);
        data_list[1] = size >> 8;
        buffer.insert(buffer.end(), data_list.begin(), data_list.end());
        num++;
        data_list.clear();
    }
    in.close();
    fileLen = buffer.size();
    return num;
}

uint32_t getVersion(std::string version_str)
{
    uint32_t version = 0;
    version_str      = Trimstring(version_str);
    while (version_str != "") {
        version = version << 8;
        version += stoi(version_str);
        // printf("%d\n",stoi(version_str));
        size_t pos;
        if (version_str.npos != (pos = version_str.find('.'))) {
            version_str = version_str.substr(pos + 1);
            Trimstring(version_str);
        } else
            break;
    }
    return version;
}