#ifndef FLASH_PARTITION_SENSOR_SETTING_H
#define FLASH_PARTITION_SENSOR_SETTING_H

#include "FlashPartitionInterface.h"
#include "CommonFunction.h"
#include "CommonDefines.h"
#include <fstream>
#include "ini.h"

typedef struct sensor_Header
{
	uint32_t magic_number;
	uint32_t version;
	uint8_t num;
	uint8_t xshutdown_en;
	uint8_t mclk_en;
	uint8_t hw_rst_dly_us;
	uint16_t mclk_en_dly;
	uint16_t hw_rst_hold_time;
	uint8_t efsync_valid;
	uint8_t efsync_pwm_idx;
	uint8_t pwm_div;
	uint8_t bf_pwm_dly_ms;
	uint16_t pwm_total_cycle;
	uint16_t pwm_duty_cycle;
}Sensor_Header;

typedef struct _sensor_Setting_Header{
    uint16_t sensor_id;
    uint16_t i2c_slv_addr;
    uint16_t sw_rst_reg;
    uint8_t rst_val;
    uint8_t release_val;
    uint16_t sw_rst_dly;
    uint16_t stream_on_reg;
    uint8_t en_val;
    uint8_t dis_val;
    uint16_t data_off;
    uint16_t init_data_len;
    uint16_t data_crc; 
}Sensor_Setting_Header;

class FlashPartitionSensorSetting : public FlashPartitionIntf
{
public:
    FlashPartitionSensorSetting();
    ~FlashPartitionSensorSetting();

    virtual std::vector<uint8_t> getPartitionDataEncrypted();
    virtual std::vector<uint8_t> getPartitionDataDecrypted();

    virtual std::vector<uint8_t> getPartitionHeaderEncrypted();
    virtual std::vector<uint8_t> getPartitionHeaderDecrypted();

    virtual PartitionInfo getPartitionInfo();
    virtual int           setPartitionInfo(PartitionInfo info);

    virtual int readPartitionDataFromCfgFile(const char *path);
    virtual int writePartitionDataToCfgFile(const char *path);

    //读取数据头配置文件
    int readPartitionHeadFromCfgFile();
    //读取子头文件
    int readSubHeadFromCfgFile(const mINI::INIStructure ini,const char *num);
    //读取子数据文件
    int readDataFromCfgFile(const char *path);
    //写入子数据到字符串
    int writeSecondHeadToStr(std::string &commbineStr, const char *numStr);
    //将子数据写入配置文件
    int writeDataToCfgFile(const char *path, const std::vector<uint8_t> data,int nNow);

    virtual int setPartitionDataFromEncryptedData(const std::vector<uint8_t> &data);

    virtual PartitionType getPartitionType();
    virtual uint32_t      getPartitionSize();
    virtual uint32_t      getPartitionDataSize();

    virtual void     setVersion(uint32_t version) { _sensorHead.version = version; }
    virtual uint32_t getVersion() { return _sensorHead.version; }

private:
    //初始化数据头
    void InitHead();
    //初始化数据
    int InitData();

private:
    bool                     _isHeader;//头文件是否读过
    bool                     _isInit;//是否已经初始化过
    std::string              _fileName;
    std::ifstream            _fileIn;
    Sensor_Header            _sensorHead;//头内第一段
    std::vector<Sensor_Setting_Header>           _settingHead; // 头内第二段
    std::map<std::string, std::string>              _headFirst;//第一帧头数据的map
    std::vector<std::map<std::string, std::string>> _headSecond; // 第二帧头数据map的vector
    std::vector<std::vector<uint8_t>>            _sensorData;//数据vector
    std::vector<std::string>                     _dataPath;//数据文件的路径
    PartitionInfo                                _info;
};

#endif