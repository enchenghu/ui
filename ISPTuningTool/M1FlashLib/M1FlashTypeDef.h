#ifndef M1_FLASH_TYPE_DEF_H
#define M1_FLASH_TYPE_DEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *FlashImg;

enum ErrorType : int32_t {
    ERROR_NO_ERR                 = 0,
    ERROR_PARAM                  = (int32_t)0x80010000,
    ERROR_PARAM_NULL             = (int32_t)0x80010001,
    ERROR_PARAM_OUT_OF_RANGE     = (int32_t)0x80010002,
    ERROR_IMG                    = (int32_t)0x80020000,
    ERROR_IMG_EMPTY              = (int32_t)0x80020001,
    ERROR_IMG_INVALID            = (int32_t)0x80020002,
    ERROR_PATH                   = (int32_t)0x80030000,
    ERROR_PATH_INVALID           = (int32_t)0x80030001,
    ERROR_PATH_NOT_FILE          = (int32_t)0x80030002,
    ERROR_PATH_NOT_EXISTS        = (int32_t)0x80030003,
    ERROR_CFG                    = (int32_t)0x80040000,
    ERROR_CFG_SIZE               = (int32_t)0x80040001,
    ERROR_IMG_FILE               = (int32_t)0x80050000,
    ERROR_IMG_FILE_READ          = (int32_t)0x80050001,
    ERROR_IMG_FILE_WRITE         = (int32_t)0x80050002,
    ERROR_PARTITION              = (int32_t)0x80060000,
    ERROR_PARTITION_OVERLAP      = (int32_t)0x80060001,
    ERROR_PARTITION_TYPE_INVALID = (int32_t)0x80060002,
    ERROR_PARTITION_NOT_FOUND    = (int32_t)0x80060003,
    ERROR_COMM                   = (int32_t)0x80070000,
    ERROR_COMM_FUNC              = (int32_t)0x80070001,
    ERROR_COMM_WRITE             = (int32_t)0x80070002,
    ERROR_COMM_READ              = (int32_t)0x80070003,
    ERROR_COMM_TIMEOUT           = (int32_t)0x80070004,
    ERROR_COMM_UNKNOWN           = (int32_t)0x80070005,
    ERROR_CONFIG_FILE            = (int32_t)0x80080000,
    ERROR_CONFIG_FILE_READ       = (int32_t)0x80080001,
    ERROR_CONFIG_FILE_WRITE      = (int32_t)0x80080002,
};

enum PartitionType : int32_t {
    PART_NONE               = 0,
    PART_DDR_SETTING        = 1 << 0,
    PART_GENERAL_HW_SETTING = 1 << 1,
    PART_FW_SETTING         = 1 << 2,
    PART_FW_RECOVER         = 1 << 3,
    PART_SENSOR_SETTING     = 1 << 4,
    PART_FW_PATCH           = 1 << 5,
    PART_DCPC               = 1 << 6,
    PART_OSD                = 1 << 7,
    PART_ARM_BOOT           = 1 << 8,
    PART_LINUX_KERNEL       = 1 << 9,
    PART_LINUX_ROOTFS       = 1 << 10,
    PART_MOVE_DATA          = 1 << 11,
    PART_ALL                = (1 << 12) - 1,
};

struct PartitionInfo
{
    PartitionType type;       // 分区类型
    uint32_t      size;       // 分区大小
    uint32_t      dataLength; // 分区内数据量
};

struct PartitionMap
{
    int           count;          // 分区数量
    PartitionInfo partitions[16]; // 分区信息
    uint32_t      offsets[16];    // 分区偏移
};

// SPI读写回调函数
typedef int (*SpiReadFunc)(int deviceIndex, uint8_t *writeData, int writeLen, uint8_t *readData, int readLen,
    uint8_t spiMode, float spiSpeedMhz, uint8_t dummyTiming);
typedef int (*SpiWriteFunc)(int deviceIndex, uint8_t *writeData, int writeLen, uint8_t spiMode, float spiSpeedMhz);

#ifdef __cplusplus
}
#endif

#endif // M1_FLASH_TYPE_DEF_H