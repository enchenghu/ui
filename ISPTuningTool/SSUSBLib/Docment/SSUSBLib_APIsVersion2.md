# SSUSBLib
[TOC]
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

#### 安装驱动文件

1. 首先需要禁用系统驱动强制签名，请根据主机的操作系统来执行相应的操作。如果是`Windows10`操作系统请参考[这里](https://jingyan.baidu.com/article/cd4c2979018215756e6e60ff.html) 。

2. 右键点击`usb_driver`下的`SmartTestBench.inf`文件，选择安装即可。

#### 添加到工程文件

1. 请将`include`文件夹下所有文件添加到工程中，并在工程中配置附加包含路径。

2. 请将`lib\x64`文件下的`libusb-1.0.lib`和`SSUSBLib.lib`添加到工程中，并在工程中配置附件库文件路径。

3. 请将`lib\x64`文件下的`libusb-1.0.dll`和`SSUSBLib.dll`拷贝至工程可执行文件目录。

4. 需要调用库文件的开头添加以下代码：
    ```c++
    #include "SSUSBLib.h"
    #pragma comment (lib,"SSUSBLib.lib")
    ```

## 函数说明

#### SSUSB_EnumDevWithSerialNumber

##### 说明 {ignore=true}

枚举设备，该方法用于枚举DemoSens设备，枚举成功返回设备数量，得到设备地址和序列号信息列表。

```c++
int SSUSB_EnumDevWithSerialNumber(DevListInfo_t *DevListInfo);
```

##### 参数 {ignore=true}

- `DevListInfo`: 设备信息列表指针。参考以下结构：

    ```c++
    #define MAX_DEVICE_SUM 5    // 最大设备数
    #define CHAR_MAX_LEN 255    // 序列号最大长度
    typedef struct DevListInfo
    {
        int DevAddr[MAX_DEVICE_SUM] = { 0 }; // 地址列表
        unsigned char uSerialNumber[MAX_DEVICE_SUM][CHAR_MAX_LEN]; // 序列号列表
    }DevListInfo_t;
    ```
##### 返回值 {ignore=true}

- -1: 操作失败
- 其他: 枚举成功设备数量

#### SSUSB_OpenDevAddr

##### 说明 {ignore=true} 

开启指定数量设备，该方法用于开启指定数量的设备，开启成功得到设备句柄列表。该方法需要使用枚举设备方法获得的设备地址信息列表。

```c++
HANDLE* SSUSB_OpenDevAddr(int DevCnt, int *DevIndexaddr, int OpenDevCnt);
```

##### 参数 {ignore=true}

- `DevCnt`: 设备总数量
- `DevIndexaddr`: 设备地址列表
- `OpenDevCnt`: 开启设备数量

##### 返回值 {ignore=true}

- `0`: 操作失败
- 其他: 开启设备成功句柄列表

#### SSUSB_CloseDev

##### 说明 {ignore=true} 

关闭设备，关闭句柄列表中所有设备。

```c++
int SSUSB_CloseDev(HANDLE* hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄指针

##### 返回值 {ignore=true}

- `0`: 操作成功
- `-1`：操作失败

#### SSUSB_USB2I2CCfg

##### 说明 {ignore=true} 

USB转I2C配置，在I2C读写前需要调用此方法配置USB转I2C透明传输的设备地址，地址长度，数据长度，I2CBus和读写延时。

```c++
int SSUSB_USB2I2CCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, 
    BYTE I2cbus = I2CBUS0_SENSORBOARD, BYTE WriteDelay = 0, BYTE ReadDelay = 1);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `SlaveAddr`: I2C设备地址，7bit地址格式
- `AddrLen`: 地址数据长度，单位为Byte
- `DataLen`: 寄存器数据长度，单位为Byte
- `I2cbus`: 可设置值：
    ```c++
    enum I2CBUS
    {
        I2CBUS0_SENSORBOARD = 0,        // 板内
        I2CBUS1_BASEBOARD,              // 板外
    };
    ```
- `WriteDelay`: I2C写操作延时
- `ReadDelay`: I2C读操作延时

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CWrite

##### 说明 {ignore=true} 

单次写USB转I2C寄存器。

```c++
int SSUSB_USB2I2CWrite(HANDLE hDev, uint32_t addr, uint32_t value);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `value`: 寄存器值

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CRead

##### 说明 {ignore=true} 

单次读USB转I2C寄存器。

```c++
int SSUSB_USB2I2CRead(HANDLE hDev, uint32_t addr, uint32_t* pValue);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pValue`: 寄存器值指针

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CWriteBrust

##### 说明 {ignore=true} 

连续写USB转I2C寄存器，向给定首地址连续写指定数量寄存器。

```c++
int SSUSB_USB2I2CWriteBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pBuf`: 寄存器值指针
- `size`: 写入寄存器个数

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CReadBrust

##### 说明 {ignore=true} 

连续读USB转I2C寄存器，从给定首地址连续读指定数量寄存器。

```c++
int SSUSB_USB2I2CReadBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pBuf`: 寄存器值指针
- `size`: 读出寄存器个数

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2SPICfg

##### 说明 {ignore=true} 

USB转SPI配置，在SPI读写前需要调用该方法配置USB转SPI透明传输的设备地址，地址长度和数据长度

```c++
int SSUSB_USB2SPICfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `SlaveAddr`: 设备地址
- `AddrLen`: 地址数据长度
- `DataLen`: 寄存器数据长度

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2SPIWrite

##### 说明 {ignore=true} 

单次写USB转SPI寄存器。

```c++
int SSUSB_USB2SPIWrite(HANDLE hDev, uint32_t addr, uint32_t pValue);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pValue`: 寄存器值

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败



#### SSUSB_USB2SPIRead

##### 说明 {ignore=true} 

单次读USB转SPI寄存器。

```c++
int SSUSB_USB2SPIRead(HANDLE hDev, uint32_t addr, uint32_t * pValue);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pValue`: 寄存器值指针

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CWriteBrust

##### 说明 {ignore=true} 

连续写USB转SPI寄存器。向给定首地址连续写指定数量寄存器。

```c++
int SSUSB_USB2I2CWriteBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pBuf`: 寄存器值指针
- `size`: 写入寄存器个数

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_USB2I2CReadBrust

##### 说明 {ignore=true} 

连续读USB转SPI寄存器。从给定首地址连续读指定数量寄存器。

```c++
int SSUSB_USB2I2CReadBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: 寄存器地址
- `pBuf`: 寄存器值指针
- `size`: 读出寄存器个数

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_WriteFPGAReg

##### 说明 {ignore=true} 

写FPGA寄存器。

```c++
int SSUSB_WriteFPGAReg(HANDLE hDev, uint32_t addr, uint32_t value);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: FPGA寄存器地址
- `value`: FPGA寄存器值

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_ReadFPGAReg

##### 说明 {ignore=true} 

读FPGA寄存器

```c++
int SSUSB_ReadFPGAReg(HANDLE hDev, uint32_t addr, uint32_t* pValue);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `addr`: FPGA寄存器地址
- `pValue`: FPGA寄存器值指针

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_SetPowerEx

##### 说明 {ignore=true} 

配置供电电压。

```c++
SSUSB_API int SSUSB_SetPowerEx(HANDLE hDev, float fVolBuf[5], bool bPowerEnable = false, uint16_t *delaytime = NULL);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `fVolBuf`: 电压列表指针，电压列表按 `AFVCC` `VPP`的顺序。
- `bPowerEnable`: CMOS使能开关，`true`-使能，`false`-不使能。
- `delaytime`: 老版本硬件上电延时，顺序为`AVDD` `DOVDD` `DVDD`。

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_SetSensorReset

##### 说明 {ignore=true} 

配置Reset信号电平。

```c++
int SSUSB_SetSensorReset(HANDLE hDev, BOOL Level);
```
##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `Level`: RESET信号电平值，`TRUE`为高电平；`FALSE`为低电平

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_SetSensorPWDN

##### 说明 {ignore=true} 

配置Power Down信号电平。

```c++
int SSUSB_SetSensorPWDN(HANDLE hDev, BOOL Level);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `Level`: PWDN信号电平值，TRUE为高电平；FALSE为低电平

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_SetSensorClock

##### 说明 {ignore=true} 

配置传感器主时钟频率。

```c++
int SSUSB_SetSensorClock(HANDLE hDev, uint32_t Freq);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `Freq`: 传感器主时钟频率
    ```c++
    enum SENSORCLK
    {
        SENSORCLK_27MHZ = 0,
        SENSORCLK_24MHZ = 1,
        SENSORCLK_12MHZ = 2,
        SENSORCLK_6MHZ = 3,
        SENSORCLK_8MHZ = 4,
        SENSORCLK_25MHZ = 5,
        SENSORCLK_37_125MHZ = 6,
        SENSORCLK_7MHZ = 7,
        SENSORCLK_9MHZ = 8,
        SENSORCLK_MAX
    };
    ```

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_SetMCLKEx

##### 说明 {ignore=true} 

设置时钟。

```c++
int SSUSB_SetMCLKEx(HANDLE hDev, float fMclk, bool bEnable);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄。
- `fMclk`: 时钟频率，单位MHZ。
- `bEnable`: 使能开关，`true`-使能，`false`-不使能。

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_SetSensorInterface

##### 说明 {ignore=true} 

配置图像数据接口。

```c++
int SSUSB_SetSensorInterface(HANDLE hDev, uint32_t Int, uint32_t LaneNum);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `Int`: 图像接口，参考以下枚举变量：
    ```c++
    enum SENSORINTERFACE
    {
        SENSORINTERFACE_DVP = 0,
        SENSORINTERFACE_MIPI,
        SENSORINTERFACE_LVDS,
        SENSORINTERFACE_TEST,
        SESNORINTERFACE_MAX
    };
    ```
- `LaneNum`: 通道数量，配置为DVP接口时该参数无效。参考以下枚举变量：
    ```c++
    enum SENSORINTFLANE
    {
        SENSORINTFLANE_1LANE = 0x00,
        SENSORINTFLANE_2LANE = 0x10,
        SENSORINTFLANE_4LANE = 0x20,
        SENSORINTFLANE_8LANE = 0x30,
        SENSORINTFLANE_4LANE_1 = 0x40,//0x8002 bit[7:6] 4lane x2 mode1
        SENSORINTFLANE_4LANE_2 = 0x80,//0x8002 bit[7:6] 4lane x2 mode2
        SENSORINTFLANE_MAX
    };
    ```

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_SetSensorBitWidthEx

##### 说明 {ignore=true} 

设置数据位宽。

```c++
int SSUSB_SetSensorBitWidthEx(HANDLE hDev, uint32_t BitWidth, uint32_t SenBitWidth);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `BitWidth`: 采集数据位宽，参考以下变量：
    ```c++
    enum SENSORDATAWIDTH
    {
        SENSORDATAWIDTH_8BIT = 0,
        SENSORDATAWIDTH_10BIT,
        SENSORDATAWIDTH_12BIT,
        SENSORDATAWIDTH_14BIT = 4,
        SENSORDATAWIDTH_16BIT,
        SENSORDATAWIDTH_20BIT,
        SENSORDATAWIDTH_YUV422 = 7,
        SENSORDATAWIDTH_YUV422_10BIT = 0x0c,
        SENSORDATAWIDTH_YUV422_10BIT_NORMAL = 0x0d,
        SENSORDATAWIDTH_YUV422_10BIT_960MIPI = 0x0e,
        SENSORDATAWIDTH_YUV422_12BIT_SPECIAL = 0x0f,
        SENSORDATAWIDTH_MAX
    };
    ```

- `SenBitWidth`: 和BitWidth设置相同取值。

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_SetSensorResolutionEx

##### 说明 {ignore=true} 

配置图像数据分辨率。

```c++
int SSUSB_SetSensorResolutionEx(HANDLE hDev, uint32_t Width, uint32_t Height, 
    uint32_t SenWidth, uint32_t SenHeight);
```

##### 参数 {ignore=true} 

- `hDev`: 设备句柄
- `Width`: 采集图像宽度
- `Height`: 采集图像高度
- `SenWidth`: Sensor输出图像宽度
- `SenHeight`: Sensor输出图像高度

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他: 操作失败

#### SSUSB_OpenSensorChannel

##### 说明 {ignore=true} 

打开FPGA的图像采集通道。

```c++
int SSUSB_OpenSensorChannel(HANDLE hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

####  SSUSB_CloseSensorChannel

##### 说明 {ignore=true} 

关闭FPGA的图像采集通道。

```c++
int SSUSB_CloseSensorChannel(HANDLE hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_StartCapture

##### 说明 {ignore=true} 

启动接收图像线程。如果需要接受图像数据，必须调用该函数。

```c++
int SSUSB_StartCapture(HANDLE hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_StopCapture

##### 说明 {ignore=true} 

关闭接收图像线程。如果调用过SSUSB_StartCapture函数退出前该函数必须被调用。

```c++
int SSUSB_StopCapture(HANDLE hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_GetFrame

##### 说明 {ignore=true} 

启动USB接口芯片接收图像接口。如果需要接受图像数据，必须调用该函数。

```c++
int SSUSB_GetFrame(HANDLE hDev);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄

##### 返回值 {ignore=true}

- `0`: 操作成功
- 其他:操作失败

#### SSUSB_FetchFrame

##### 说明 {ignore=true} 

获取图像数据，该函数为阻塞函数。

```c++
int SSUSB_FetchFrame(HANDLE hDev, void* pFrame, int BufferSize, int* pFrameSize, 
    FrameInfoV0_t* pFrameInfo, uint32_t Timeout);
```

##### 参数 {ignore=true}

- `hDev`: 设备句柄
- `pFrame`: 图像数据缓存地址
- `BufferSize`: 图像数据缓存大小
- `pFrameSize`: 接收的图像数据实际大小
- `pFrameInfo`: 帧信息地址
    ```c++
    typedef struct FrameInfoV0
    {
        UINT32 version;     // 帧信息版本
        UINT32 width;       // 帧宽度
        UINT32 height;      // 帧高度
        UINT32 size;        // 帧数据量大小，单位为Byte
        UINT32 bitwidth;    // 帧数据位宽
        UINT32 timestamp;   // 帧时间戳
        UINT32 reserved[16];// 保留位
    }FrameInfoV0_t;
    ```
- `Timeout`: 超时时间，单位为ms

##### 返回值 {ignore=true}

- `0`: 操作成功
- `2`: 超时
- 其他: 获取失败