#ifndef FLASH_PARTITION_INTERFACE_H
#define FLASH_PARTITION_INTERFACE_H

#include "CommonFunction.h"
#include "M1FlashTypeDef.h"
#include <map>
#include <memory>
#include <vector>


class FlashPartitionIntf;
class FlashPartitionCreatorBase;

using FlashPartitionPtr            = std::shared_ptr<FlashPartitionIntf>;
using FlashPartitionCreatorBasePtr = std::shared_ptr<FlashPartitionCreatorBase>;

class FlashPartitionCreatorBase
{
public:
    virtual FlashPartitionPtr create() = 0;
};

template <typename T>
class FlashPartitionCreator : public FlashPartitionCreatorBase
{
public:
    FlashPartitionPtr create() override { return std::make_shared<T>(); }
};

class FlashPartitionFactory
{
public:
    static FlashPartitionPtr create(PartitionType type)
    {
        static FlashPartitionFactory factory;
        auto                         it = factory._partitionMap.find(type);
        if (it != factory._partitionMap.end()) {
            return it->second->create();
        }
        return nullptr;
    }

private:
    FlashPartitionFactory();
    std::map<PartitionType, FlashPartitionCreatorBasePtr> _partitionMap;
};

class FlashPartitionIntf
{
public:
    FlashPartitionPtr copy()
    {
        auto                 ptr    = FlashPartitionFactory::create(getPartitionType());
        auto                 header = getPartitionHeaderEncrypted();
        auto                 data   = getPartitionDataEncrypted();
        std::vector<uint8_t> allData;
        allData.insert(allData.end(), header.begin(), header.end());
        allData.insert(allData.end(), data.begin(), data.end());
        ptr->setPartitionDataFromEncryptedData(allData);
        ptr->setPartitionInfo(getPartitionInfo());
        return ptr;
    }
    virtual std::vector<uint8_t> getPartitionDataEncrypted() = 0;
    virtual std::vector<uint8_t> getPartitionDataDecrypted() = 0;

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted() = 0;
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted() = 0;

    virtual PartitionInfo getPartitionInfo()                   = 0;
    virtual int           setPartitionInfo(PartitionInfo info) = 0;

    virtual int readPartitionDataFromCfgFile(const char *path) = 0;
    virtual int writePartitionDataToCfgFile(const char *path)  = 0;

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data) = 0;

    virtual void setVersion(uint32_t version) { ; }
    virtual uint32_t getVersion() { return 0; }

    virtual PartitionType getPartitionType()     = 0;
    virtual uint32_t      getPartitionSize()      = 0;
    virtual uint32_t      getPartitionDataSize() = 0;
    virtual uint32_t      getPartitionHeaderCrc() { return getOnlyCrc(getPartitionHeaderEncrypted()); }
    virtual ~FlashPartitionIntf() { }

protected:
    virtual uint32_t getPartitionDataCrc() { return getOnlyCrc(getPartitionDataEncrypted()); }
};

#endif // FLASH_PARTITION_INTERFACE_H