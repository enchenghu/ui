#pragma once
#include "SSUSBLib.h"
#include <string>
#include <vector>


#define D_RAW10 0x00
#define D_RAW8 0x01
#define D_MIPI_RAW8 0x01
#define D_YUV 0x02
#define D_RAW16 0x03
#define D_MIPI_RAW16 0x03
#define D_RGB565 0x04
#define D_YUV_SPI 0x05
#define D_MIPI_RAW10 0x06 // 5bytes = 4 pixel...
#define D_MIPI_RAW12 0x07 // 3bytes = 2 pixel...
#define D_RAW12 0x07
#define D_YUV_MTK_S 0x08  // MTK output...
#define D_MIPI_RAW14 0x09 // 7bytes = 4 pixel...
#define D_MIPI_RAW20 0x0a // 5bytes = 2 pixel...
#define DEMO_RAW10 0x0b
#define D_YUV422_RAW10 0x0c // YUV422 10bit
#define D_RAW4_DDR 0x0d     // DVP RAW 4bit DDR
#define D_RAW4_SDR 0x0e     // DVP RAW 4bit SDR
#define D_YUV422_RAW10_NORMAL 0x0f // YUV422 10bit,FPGA是通用版本，不需要设置senwidth,senheight分辨率, isp在sensor中实现
#define D_YUV422_RAW10_960 0x10     // 车载类型，MIPI 10bit数据由TI-960芯片自动封装完成
#define D_YUV422_RAW12_SPECIAL 0x11 // YUV422 12bit
#define D_BGR24 0x20                ///< 排列顺序为B，G，R，各8bit
#define D_BGR32 0x21                ///< 排列顺序为B，G，R，0各8bit

#define D_P10 0x24 ///< 一个像素占两个字节，LSB，0～1023，一般用于MIPI_RAW10转换
#define D_P12 0x25 ///< 一个像素占两个字节，LSB，0～4095，一般用于MIPI_RAW12转换

#define D_G8 0x28
#define D_G10 0x29
#define D_GRAY8 0x2a
#define DEMO_RAW10 0x2b

typedef struct _tagRegNode
{
    USHORT uReg;
    USHORT uVal;
} STRegNode;

typedef struct _SensorTab
{
    /// @brief SENSOR宽度
    USHORT width; ///< SENSOR宽度
    /// @brief SENSOR高度
    USHORT height; ///< SENSOR高度
    /// @brief SENSOR数据类型
    BYTE type; ///< SENSOR数据类型
    /// @brief SENSOR的RESET和PWDN引脚设置
    BYTE pin; ///< SENSOR的RESET和PWDN引脚设置
    /// @brief SENSOR的器件地址
    BYTE SlaveID; ///< SENSOR的器件地址
    /// @brief SENSOR的I2C模式
    BYTE mode; ///< SENSOR的I2C模式
    /// @brief SENSOR标志寄存器1.
    USHORT FlagReg; ///< SENSOR标志寄存器1.
    /// @brief SENSOR标志寄存器1的值
    USHORT FlagData; ///< SENSOR标志寄存器1的值
    /// @brief SENSOR标志寄存器1的掩码值
    USHORT FlagMask; ///< SENSOR标志寄存器1的掩码值
    /// @brief SENSOR标志寄存器2.
    USHORT FlagReg1; ///< SENSOR标志寄存器2.
    /// @brief SENSOR标志寄存器2的值
    USHORT FlagData1; ///< SENSOR标志寄存器2的值
    /// @brief SENSOR标志寄存器2的掩码值
    USHORT FlagMask1; ///< SENSOR标志寄存器2的掩码值
    /// @brief SENSOR的名称
    char name[64]; ///< SENSOR的名称
    /// @brief SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
    BYTE outformat; ///< SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
    /// @brief SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
    float mclk; ///< SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
    int mclken;

    /// @brief SENSOR的AVDD电压值
    float avdd;
    /// @brief SENSOR的DOVDD电压值
    float dovdd;
    /// @brief SENSOR的DVDD电压值
    float dvdd;
    float afvdd;

    int dovdddelay; // dovdd 上电后到下一个电压上电的时间间隔,单位ms
    int dvdddelay;  // dvdd 上电后到下一个电压上电的时间间隔,单位ms
    int avdddelay;  // avdd 上电后到下一个电压上电的时间间隔,单位ms
    /// @brief SENSOR的数据接口类型
    BYTE port; ///< SENSOR的数据接口类型
    USHORT Ext0;
    USHORT Ext1;
    USHORT Ext2;

    BYTE lane;
    USHORT avtime;
    USHORT dvtime;
    USHORT dovtime;
    int I2CDelay; // I2C 操作延时时间
    _SensorTab()
    {
        width = 0;
        height = 0;
        type = 0;
        pin = 0;
        SlaveID = 0;
        mode = 0;
        FlagReg = 0;
        FlagData = 0;
        FlagMask = 0;
        FlagReg1 = 0;
        FlagData1 = 0;
        FlagMask1 = 0;
        memset(name, 0, sizeof(name));
        outformat = 0;
        mclk = 0; // 0:12M; 1:24M; 2:48M.
        mclken = 1;
        avdd = 0.0;  //
        dovdd = 0.0; //
        dvdd = 0.0;
        afvdd = 0.0;

        dovdddelay = 0;
        dvdddelay = 0;
        avdddelay = 0;

        port = 0;
        Ext0 = 0;
        Ext1 = 0;
        Ext2 = 0;

        lane = 4;
        avtime = 0;
        dvtime = 0;
        dovtime = 0;
        I2CDelay = 0;
    }
} SensorTab, *pSensorTab;

enum POWER
{
    AVDD = 0,
    DOVDD,
    DVDD,
    AFVCC,
    VPP,
};

class SenPara
{
  public:
    USHORT width;  //<SENSOR宽度
    USHORT height; //<SENSOR高度
    BYTE pin;      //<SENSOR的RESET和PWDN引脚设置
    BYTE SlaveID;  //<SENSOR的器件地址
    float mclk;    //<SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
    int mclken;
    int senLane;
    float fVoltage[5];
    USHORT delaytime[5];
    SENSORINTERFACE senPort;
    SENSORDATAWIDTH senDataType;

  public:
    SenPara();
    ~SenPara();
    void vGetSSUSBPortLane(int iPort, int iLane);
    void vGetSSUSBDataType(int iType);
    void vInitSenPara();
    void vSetSensorCfg(HANDLE hDev);
};
