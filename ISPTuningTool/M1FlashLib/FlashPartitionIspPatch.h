#ifndef FLASH_PARTITION_ISP_PATCH_H
#define FLASH_PARTITION_ISP_PATCH_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include "PatchStructHead.h"
#include <fstream>

class FlashPartitionPatchBase : public FlashPartitionIntf
{
public:
    FlashPartitionPatchBase();
    ~FlashPartitionPatchBase();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data);

    virtual int setFunction();

    virtual PartitionType getPartitionType();
    virtual uint32_t      getPartitionSize();
    virtual uint32_t      getPartitionDataSize();

    virtual void     setVersion(uint32_t version) { _ispHead.version = version; }
    virtual uint32_t getVersion() { return _ispHead.version; }

protected:
    //初始化数据头
    static void InitHead();
    //初始化数据
    virtual int InitData();
    //反写回函数数据
    virtual int WriteFunc(const char *path);
    //反写回boot数据
    virtual int WriteBoot(const char *path);
    //反写回IspTable数据
    virtual int WriteIspTable(const char *path);
    //反写回Isp数据
    virtual int WriteIsp(const char *path);
protected:
    static Patch_Header             _ispHead;
    static std::vector<Function_Header> _funcHeader;
    static std::vector<uint8_t>     _bootData;
    static std::vector<uint32_t>    _ispTableData;
    static std::vector<uint8_t>     _ispData;
    static PartitionInfo            _info;
    static bool                     _isInit;//是否已经初始化过
    static std::string              _funcFileName;
    static std::string              _bootFileName;
    static std::string              _tableFileName;
    static std::string              _ispFileName;
    static std::ifstream            _fileIn;
};

class FlashPartitionBootPatch : public FlashPartitionPatchBase
{
public:
    FlashPartitionBootPatch();
    ~FlashPartitionBootPatch();

    //设置函数数组
    static int setFunction();
    //初始化数据
    static int InitData();

};

class FlashPartitionIspPatchTable : public FlashPartitionPatchBase
{
public:
    FlashPartitionIspPatchTable();
    ~FlashPartitionIspPatchTable();

    //初始化数据
    static int InitData();
};

class FlashPartitionIspPatch : public FlashPartitionPatchBase
{
public:
    FlashPartitionIspPatch();
    ~FlashPartitionIspPatch();

    //初始化数据
    static int InitData();
};

//patch对外模块
class FlashPartitionPatch : public FlashPartitionPatchBase
{
public:
    FlashPartitionPatch();
    ~FlashPartitionPatch();

protected:
    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int ReadAllConfigFile(const char *path);
};

#endif //FLASH_PARTITION_ISP_PATCH_H