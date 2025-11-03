# SSUSBLib

当前版本信息如下：

    Major.Minor.Micro = 0.9.3
    Date = May_29_2018
    Description = SmartSens USB Demo Board Library

## 简介

SSUSBLib是Smartsens的测试设备使用的USB库文件，包含了必要的头文件，库文件及驱动文件。通过API调用的方式对Demo测试平台进行硬件配置和图像获取。

## 使用方法

下面是库文件包的文件夹说明：

|     文件夹     | 说明
| :-------------: |:-------------:|
| demo    | 库文件使用的示例代码，可以在vs studio上进行编译 |
| include     | 必要的头文件      |
| lib | 必要的库文件，当前只包含64bit的库文件      |
| usb_driver | usb驱动文件    |
| readme.md | 说明文件    |

### 安装驱动文件

1. 首先需要禁用系统驱动强制签名，请根据主机的操作系统来执行相应的操作。如果是Windows10操作系统请参考[这里](https://jingyan.baidu.com/article/cd4c2979018215756e6e60ff.html) 。

1. 右键点击usb_driver下的SmartTestBench.inf文件，选择安装即可。

### 添加到工程文件

1. 请将include文件夹下所有文件添加到工程中，并在工程中配置附加包含路径。

1. 请将lib\x64文件下的libusb-1.0.lib和SSUSBLib.lib添加到工程中，并在工程中配置附件库文件路径。

1. 请将lib\x64文件下的libusb-1.0.dll和SSUSBLib.dll拷贝至工程可执行文件目录。

1. 需要调用库文件的开头添加以下代码：

        #include "SSUSBLib.h"
        #pragma comment (lib,"SSUSBLib.lib")

## 函数说明

### HANDLE SSUSB_OpenDev()

**参数:** 无
**返回值:** 设备句柄
**说明:** 打开设备

### int SSUSB_CloseDev(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 打开设备。

### int SSUSB_GetFirmwareInfo(HANDLE hDev, char* pInfo, uint32_t Size);

**参数:**

> hDev: 设备句柄
pInfo: 下位机固件版本信息缓存地址
Size: 下位机固件版本信息缓存容量大小，至少大于128Byte

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 获取下位机固件版本信息，纯字符串形式，例如：
Name: usb3.0_firmware  Version: 1.02   Date: Jun_25_2018

### int SSUSB_StartCapture(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 启动接收图像线程。如果需要接受图像数据，必须调用该函数。

### int SSUSB_StopCapture(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 关闭接收图像线程。如果调用过SSUSB_StartCapture函数退出前该函数必须被调用。

### int SSUSB_GetFrame(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 启动USB接口芯片接收图像接口。如果需要接受图像数据，必须调用该函数。

### int SSUSB_OpenSensorChannel(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 打开FPGA的图像采集通道。

### int SSUSB_CloseSensorChannel(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 关闭FPGA的图像采集通道。

### int SSUSB_GetVerInfo(SSUSBVerInfo_t* pInfo);

**参数:**

> pInfo: SSUSBLib库文件版本信息。参考以下结构：

        typedef struct SSUSBVerInfo
        {
            uint8_t major;          //主版本号
            uint8_t minor;          //次版本号
            uint8_t micro;          //小版本号
            char date[16];          //库文件发布日期
            char description[64];   //库文件描述
        }SSUSBVerInfo_t;

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 获取SSUSBLib库文件的版本信息。

### int SSUSB_SetSensorPWDN(HANDLE hDev, BOOL Level);

**参数:**

> hDev: 设备句柄
Level: PWDN信号电平值，TRUE为高电平；FALSE为低电平

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置Power Down信号电平。

### int SSUSB_SetSensorReset(HANDLE hDev, BOOL Level);

**参数:**

> hDev: 设备句柄
Level: RESET信号电平值，TRUE为高电平；FALSE为低电平

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置Reset信号电平。

### int SSUSB_SetSensorInterface(HANDLE hDev, uint32_t Int, uint32_t LaneNum);

**参数:**

> hDev: 设备句柄
Int: 图像接口，参考以下枚举变量：

        enum SENSORINTERFACE
        {
            SENSORINTERFACE_DVP = 0,
            SENSORINTERFACE_MIPI,
            SENSORINTERFACE_LVDS,
            SENSORINTERFACE_TEST,
            SESNORINTERFACE_MAX
        };

> LaneNum: 通道数量，可以配置为1,2,4。配置为DVP接口时该参数无效。参考以下枚举变量：

        enum SENSORINTFLANE
        {
            SENSORINTFLANE_1LANE = 0x00,
            SENSORINTFLANE_2LANE = 0x10,
            SENSORINTFLANE_4LANE = 0x20,
            SENSORINTFLANE_MAX
        };

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置图像数据接口。

### int SSUSB_SetSensorBitWidth(HANDLE hDev, uint32_t BitWidth);

**参数:**

> hDev: 设备句柄
BitWidth: 数据位宽，参考以下枚举变量：

        enum SENSORDATAWIDTH
        {
            SENSORDATAWIDTH_8BIT = 8,
            SENSORDATAWIDTH_10BIT = 10,
            SENSORDATAWIDTH_12BIT = 12,
            SENSORDATAWIDTH_MAX
        };

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置图像数据位宽。

### int SSUSB_SetSensorResolution(HANDLE hDev, uint32_t Width, uint32_t Height);

**参数:**

> hDev: 设备句柄
Width: 图像宽度
Height: 图像高度

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置图像数据分辨率。

### int SSUSB_SetSensorClock(HANDLE hDev, uint32_t Freq);

**参数:**

> hDev: 设备句柄
Freq: 传感器主时钟频率

        enum SENSORCLK
        {
            SENSORCLK_27MHZ = 0,
            SENSORCLK_24MHZ,
            SENSORCLK_12MHZ,
            SENSORCLK_6MHz,
            SENSORCLK_MAX
        };

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置传感器主时钟频率。

### int SSUSB_SetSensorIntCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen);

**参数:**

> hDev: 设备句柄
SlaveAddr: Sensor的I2C设备地址，7bit地址格式
AddrLen: 地址数据长度，单位为Byte
DataLen: 寄存器数据长度，单位为Byte

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置传感器配置接口。

### int SSUSB_WriteSensorReg(HANDLE hDev, uint32_t addr, uint32_t value);

**参数:**

> hDev: 设备句柄
addr: Sensor的寄存器地址
value: Sensor的寄存器值

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 写Sensor的寄存器。

### int SSUSB_ReadSensorReg(HANDLE hDev, uint32_t addr, uint32_t* pValue);

**参数:**

> hDev: 设备句柄
addr: Sensor的寄存器地址
pValue: Sensor的寄存器值指针

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 读Sensor的寄存器。

### int SSUSB_SetPowerSupply(HANDLE hDev, uint32_t PwrRail, uint32_t ValueINmV);

**参数:**

> hDev: 设备句柄
PwrRail: 需要配置的电源轨

        enum POWERSUPPLY
        {
            POWERSUPPLY_AVDD = 0,
            POWERSUPPLY_DOVDD,
            POWERSUPPLY_DVDD,
            POWERSUPPLY_AFVCC,
            POWERSUPPLY_MAX
        };

> ValueINmV: 电压值，单位为mV。

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置sensor的供电电源电压值。

### int SSUSB_EnablePowerSupply(HANDLE hDev, uint32_t PwrRail, BOOL Enable);

**参数:**

> hDev: 设备句柄
PwrRail: 需要配置的电源轨

        enum POWERSUPPLY
        {
            POWERSUPPLY_AVDD = 0,
            POWERSUPPLY_DOVDD,
            POWERSUPPLY_DVDD,
            POWERSUPPLY_AFVCC,
            POWERSUPPLY_MAX
        };

> Enable: 电压输出使能。TRUE为输出使能，FALSE为输出关闭。

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 使能sensor的供电电源。

### int SSUSB_FetchFrame(HANDLE hDev, void* pFrame, int BufferSize, int* pFrameSize, FrameInfoV0_t* pFrameInfo, uint32_t Timeout);

**参数:**

> hDev: 设备句柄
pFrame: 图像数据缓存地址
BufferSize: 图像数据缓存大小
pFrameSize: 接收的图像数据实际大小
pFrameInfo: 帧信息地址

        typedef struct FrameInfoV0
        {
            UINT32 version;     //帧信息版本
            UINT32 width;       //帧宽度
            UINT32 height;      //帧高度
            UINT32 size;        //帧数据量大小，单位为Byte
            UINT32 bitwidth;    //帧数据位宽
            UINT32 timestamp;   //帧时间戳
            UINT32 reserved[16];//保留位
        }FrameInfoV0_t;

> Timeout: 超时时间，单位为ms

**返回值:**

> 0: 操作成功
-2: 超时
其他: 获取失败

**说明:** 获取图像数据，该函数为阻塞函数

### int SSUSB_USB2I2CCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen);

**参数:**

> hDev: 设备句柄
SlaveAddr: I2C设备地址，7bit地址格式
AddrLen: 地址数据长度，单位为Byte
DataLen: 寄存器数据长度，单位为Byte

**返回值:**

> 0: 操作成功
-2: 超时
其他: 获取失败

**说明:** 配置USB转I2C透明传输的设备地址，地址长度和数据长度。

### int SSUSB_USB2I2CWrite(HANDLE hDev, uint32_t addr, uint32_t value);

**参数:**

> hDev: 设备句柄
addr: USB转I2C的寄存器地址
value: USB转I2C的寄存器值

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 写USB转I2C的寄存器。

### int SSUSB_USB2I2CRead(HANDLE hDev, uint32_t addr, uint32_t* pValue);

**参数:**

> hDev: 设备句柄
addr: USB转I2C的寄存器地址
pValue: USB转I2C的寄存器值指针

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 读USB转I2C的寄存器。

### int SSUSB_API int SSUSB_GetBoardType(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 设备型号，

        enum BOARD_HWTYPE
        {
            BOARD_HWTYPE_RD = 1,
            BOARD_HWTYPE_CLIENT,
            BOARD_HWTYPE_UNKNOWN
        };

**说明:** 获得电路板型号。

### int SSUSB_SetSensGndConn(HANDLE hDev, BOOL bConn);

**参数:**

> hDev: 设备句柄
bConn: sensor板的GND是否和主板上的GND连接。

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 配置电路板和Sensor板的GND是否连接。

### int SSUSB_TrgOneFrame(HANDLE hDev);

**参数:**

> hDev: 设备句柄

**返回值:**

> 0: 操作成功
其他:操作失败

**说明:** 通过FPGA内部的触发IO触发Sensor抓拍一张图像。