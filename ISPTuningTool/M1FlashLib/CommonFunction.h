#ifndef COMMON_FUNCTION_H
#define COMMON_FUNCTION_H

#include <iomanip>
#include <map>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>


#define MAGIC_ENCODER 0X45

// 获取CRC并异或源数组
int getCRC(char *pBuf, size_t nLen);
int getCRC(std::vector<uint8_t> &buffer);

// 获取CRC不异或源数组
int getOnlyCrc(char *pBuf, size_t nLen);
int getOnlyCrc(const std::vector<uint8_t> &buffer, size_t len);
int getOnlyCrc(const std::vector<uint8_t> &buffer);

// 不异或获取CRC
int getOtpCrc(char *pBuf, int nLen);
int getOtpCrc(const std::vector<uint8_t> &buffer);

// 只异或源数组
void getXor(char *pBuf, size_t nLen);
void getXor(std::vector<uint8_t> &buffer, size_t nLen);
void getXor(std::vector<uint8_t> &buffer);

// 获取源数组的异或结果，不改变源数组
std::vector<uint8_t> getCopyXor(char *pBuf, int nLen);
std::vector<uint8_t> getCopyXor(const std::vector<uint8_t> &buffer);

std::string &Trimstring(std::string &str);

bool        isFileExist(const char *path);
bool        isDirExist(const char *path);
bool        canCreateFile(const char *path);
bool        isRelativePath(const char *path);
std::string getAbsolutePath(const char *path);
std::string getDirPath(const char *path);

// 函数：将十进制数字转换为16进制的字符串
std::string decimalToHex(size_t num, int size = 0);

// setting文件转码为bin
int setting_to_grp(std::string in_file, std::vector<uint8_t> &buffer, size_t &fileLen);

uint32_t getVersion(std::string version_str);

template <typename T>
inline std::string num2Str(T num, int base = 10, int width = 0, char fill = '0', std::string prefix = "")
{
    std::stringstream ss;
    auto              baseFlag = std::dec;
    if (base == 16)
        baseFlag = std::hex;
    else if (base == 8)
        baseFlag = std::oct;
    if (sizeof(T) == 1)
        ss << std::setfill(fill) << std::setw(width) << std::setiosflags(std::ios::uppercase) << baseFlag
           << (uint16_t)num;
    else
        ss << std::setfill(fill) << std::setw(width) << std::setiosflags(std::ios::uppercase) << baseFlag << num;
    return prefix + ss.str();
}

template <typename T>
inline T str2Num(std::string str, int base = 10, T default = 0)
{
    uint64_t          num;
    std::stringstream ss;
    ss << str;
    if (base == 16)
        ss >> std::hex >> num;
    else if (base == 8)
        ss >> std::oct >> num;
    else
        ss >> num;
    if (ss.fail())
        return default;
    return static_cast<T>(num);
}

#endif // COMMON_FUNCTION_H