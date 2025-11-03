#ifndef _SSUSBLIB_H
#define _SSUSBLIB_H
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#ifdef SSUSBLIB_EXPORTS
#define SSUSB_API __declspec(dllexport)
#else
#define SSUSB_API __declspec(dllimport)
#endif
#else
#include "stdafx.h"
#define SSUSB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PACKET_PAYLOAD_SIZE (1024-24)
#define CRC16_SENDREGMSB 0x804b
#define CRC16_SENDREGLSB 0x804c
#define CRC16_GETREGMSB 0x804d
#define CRC16_GETREGLSB 0x804e
typedef struct FrameInfoV0
{
	UINT32 version;
	UINT32 width;
	UINT32 height;
	UINT32 size;
	UINT32 bitwidth;
	UINT32 timestamp;
	UINT32 reserved[16];
}FrameInfoV0_t;

typedef struct SSUSBVerInfo
{
	uint8_t major;
	uint8_t minor;
	uint8_t micro;
	char date[16];
	char description[64];
}SSUSBVerInfo_t;

typedef struct CyU3PMipicsiCfg
{
	UINT16 dataFormat;
	UINT16 numDataLanes;
	UINT16 pllPrd;
	UINT16 pllFbd;
	UINT16 pllFrs;
	UINT16 csiRxClkDiv;
	UINT16 parClkDiv;
	UINT16 mClkCtl;
	UINT16 mClkRefDiv;
	UINT16 hResolution;
	UINT16 fifoDelay;
	UINT16 busWidth;
} CyU3PMipicsiCfg_t;

enum POWERSUPPLY
{
	POWERSUPPLY_AVDD = 0,
	POWERSUPPLY_DOVDD,
	POWERSUPPLY_DVDD,
	POWERSUPPLY_AFVCC,
	POWERSUPPLY_MAX
};

enum I2CBUS
{
	I2CBUS0_SENSORBOARD = 0,
	I2CBUS1_BASEBOARD,
};

enum SENSORINTERFACE
{
	SENSORINTERFACE_DVP = 0,
	SENSORINTERFACE_MIPI,//Mipi DPHY
	SENSORINTERFACE_LVDS,
	SENSORINTERFACE_TEST,
	SENSORINTERFACE_MIPI_CPHY,//Mipi Cphy
	SESNORINTERFACE_MAX
};

enum SENSORDATAWIDTH
{
	SENSORDATAWIDTH_8BIT = 0,
	SENSORDATAWIDTH_10BIT,
	SENSORDATAWIDTH_12BIT,
	SENSORDATAWIDTH_14BIT = 4,
	SENSORDATAWIDTH_16BIT,
	SENSORDATAWIDTH_20BIT,
	SENSORDATAWIDTH_YUV422 = 7,
	SENSORDATAWIDTH_YUV422_10BIT = 0x0c,//FPGA是特殊版本，需要设置senwidth,senheight分辨率，isp在FPGA中实现，前期debug isp模式
	SENSORDATAWIDTH_YUV422_10BIT_NORMAL = 0x0d,//FPGA是通用版本，不需要设置senwidth,senheight分辨率，isp在sensor中实现
	SENSORDATAWIDTH_YUV422_10BIT_960MIPI = 0x0e,//FPGA是通用版本，车载数据处理，MIPI 10bit数据由TI-960芯片内部自动封装完成，数据格式为U[7:0]Y[7:0]V[7:0]Y[7:0] {U[9:0]Y[9:0]V[9:0]Y[9:0]},
												//与FPGA封装的MIPI 10bit数据格式有差异
	SENSORDATAWIDTH_YUV422_12BIT_SPECIAL = 0x0f,//FPGA是特殊版本，不需要设置senwidth,senheight分辨率，isp在sensor中实现 
												//U1[11:4]Y1[11:4]{U1[3:0]Y1[3:0]} 
												//V1[11:4]Y2[11:4]{V1[3:0]Y2[3:0]}

	SENSORDATAWIDTH_MAX
};

enum SENSORINTFLANE
{
	SENSORINTFLANE_1LANE = 0x00,
	SENSORINTFLANE_2LANE = 0x10,
	SENSORINTFLANE_4LANE = 0x20,
	SENSORINTFLANE_8LANE = 0x30,
	SENSORINTFLANE_4LANE_1 = 0x40,//0x8002 bit[7:6] 4lane x2 mode1
	SENSORINTFLANE_4LANE_2 = 0x80,//0x8002 bit[7:6] 4lane x2 mode2
	SENSORINTFLANE_3LANE = 0x90,//ready to do(mipi CPHY)
	SENSORINTFLANE_MAX
};

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

enum BOARD_HWTYPE
{
	BOARD_HWTYPE_RD = 1,
	BOARD_HWTYPE_CLIENT,
	BOARD_HWTYPE_UNKNOWN
};

// SPI 模式, 使用时需要按位或，如 SPI_CPOL | SPI_CPHA
enum SPI_MODE :int
{
	SPI_CPOL = 1 << 0,
	SPI_CPHA = 1 << 1,
	SPI_RW_EDGE_DIFF = 1 << 2,
};

//Pin Count
enum POWER_MODE
{
	VER_BASIC_MODE = 4,//(AVDD DOVDD DVDD AFVCC) 
	VER_0x801_MODE = 7,//DS325TD (AVDD DOVDD DVDD AFVCC AVDD2CD DOVDD2CD DVDD2CD)
};
#define MAXD_EVICE_SUM 8 //Maximum enumerable device
#define CHAR_MAX_LEN 256

typedef struct DevListInfo
{
	int DevAddr[MAXD_EVICE_SUM] = { 0 };
	unsigned char uSerialNumber[MAXD_EVICE_SUM][CHAR_MAX_LEN];
}DevListInfo_t;


SSUSB_API HANDLE* SSUSB_OpenDev(int &DevCnt);
SSUSB_API HANDLE* SSUSB_OpenDevAddr(int DevCnt, int *DevIndexaddr, int OpenDevCnt);
SSUSB_API int SSUSB_EnumDev(int* DevListAddr);
SSUSB_API int SSUSB_EnumDevWithSerialNumber(DevListInfo_t* DevListInfo);

SSUSB_API int SSUSB_CloseDev(HANDLE* hDev);//multi device close together
SSUSB_API int SSUSB_GetFirmwareInfo(HANDLE hDev, char* pInfo, uint32_t Size);

SSUSB_API int SSUSB_StartCapture(HANDLE hDev);
SSUSB_API int SSUSB_StopCapture(HANDLE hDev);

SSUSB_API int SSUSB_GetFrame(HANDLE hDev);
SSUSB_API int SSUSB_OpenSensorChannel(HANDLE hDev);
SSUSB_API int SSUSB_CloseSensorChannel(HANDLE hDev);

SSUSB_API int SSUSB_GetVerInfo(SSUSBVerInfo_t* pInfo);

SSUSB_API int SSUSB_SetSensorPWDN(HANDLE hDev, BOOL Level);
SSUSB_API int SSUSB_SetSensorReset(HANDLE hDev, BOOL Level);
SSUSB_API int SSUSB_SetSensorInterface(HANDLE hDev, uint32_t Int, uint32_t LaneNum);
SSUSB_API int SSUSB_SetSensorBitWidth(HANDLE hDev, uint32_t BitWidth);
SSUSB_API int SSUSB_SetSensorBitWidthEx(HANDLE hDev, uint32_t BitWidth, uint32_t SenBitWidth);
SSUSB_API int SSUSB_SetSensorResolution(HANDLE hDev, uint32_t Width, uint32_t Height);
SSUSB_API int SSUSB_SetSensorResolutionEx(HANDLE hDev, uint32_t Width, uint32_t Height, uint32_t SenWidth, uint32_t SenHeight);
SSUSB_API int SSUSB_SetVCModeEx(HANDLE hDev, uint32_t VCImageSize);

SSUSB_API int SSUSB_SetSensorClock(HANDLE hDev, uint32_t Freq);
SSUSB_API int SSUSB_TrgOneFrame(HANDLE hDev);

SSUSB_API int SSUSB_SetSensorIntCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen);
SSUSB_API int SSUSB_WriteSensorReg(HANDLE hDev, uint32_t addr, uint32_t value);
SSUSB_API int SSUSB_ReadSensorReg(HANDLE hDev, uint32_t addr, uint32_t* pValue);

SSUSB_API int SSUSB_SetPowerSupply(HANDLE hDev, uint32_t PwrRail, uint32_t ValueINmV);
SSUSB_API int SSUSB_EnablePowerSupply(HANDLE hDev, uint32_t PwrRail, BOOL Enable);

SSUSB_API int SSUSB_ResetSensor(HANDLE hDev);
SSUSB_API int SSUSB_FetchFrame(HANDLE hDev, void* pFrame, int BufferSize, int* pFrameSize, FrameInfoV0_t* pFrameInfo, uint32_t Timeout);

SSUSB_API int SSUSB_USB2I2CCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus = I2CBUS0_SENSORBOARD, BYTE WriteDelay = 0, BYTE ReadDelay = 1);
SSUSB_API int SSUSB_USB2I2CWrite(HANDLE hDev, uint32_t addr, uint32_t value);
SSUSB_API int SSUSB_USB2I2CRead(HANDLE hDev, uint32_t addr, uint32_t* pValue);
SSUSB_API int SSUSB_USB2I2CWriteBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
SSUSB_API int SSUSB_USB2I2CReadBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
SSUSB_API int SSUSB_USB2I2CWriteDiscrete(HANDLE hDev, const uint32_t pAddr[], const uint32_t pValue[], const uint32_t pDelay[], uint32_t iRegCnt,
	BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus = I2CBUS0_SENSORBOARD, BYTE WriteDelay = 0, BYTE ReadDelay = 1);
SSUSB_API int SSUSB_USB2I2CReadDiscrete(HANDLE hDev, const uint32_t pAddr[], uint32_t pValue[], const uint32_t pDelay[], uint32_t iRegCnt,
	BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus, BYTE WriteDelay, BYTE ReadDelay);

SSUSB_API int SSUSB_USB2SPICfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen);
SSUSB_API int SSUSB_USB2SPIWrite(HANDLE hDev, uint32_t addr, uint32_t pValue);
SSUSB_API int SSUSB_USB2SPIRead(HANDLE hDev, uint32_t addr, uint32_t* pValue);
SSUSB_API int SSUSB_USB2SPIWriteBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);
SSUSB_API int SSUSB_USB2SPIReadBrust(HANDLE hDev, uint32_t addr, uint8_t* pBuf, uint32_t size);

SSUSB_API int SSUSB_GetBoardType(HANDLE hDev);
SSUSB_API int SSUSB_SetSensGndConn(HANDLE hDev, BOOL bConn);
SSUSB_API int SSUSB_CurrentADC(HANDLE hDev, uint8_t* pValue);
SSUSB_API int SSUSB_ResetFX3(HANDLE hDev);
SSUSB_API int SSUSB_GetVersionFX3(HANDLE hDev, BYTE* pBuf);
SSUSB_API int SSUSB_GetVersionFPGA(HANDLE hDev, BYTE* pBuf);
SSUSB_API int SSUSB_GetFPGAMAJORVersion(HANDLE hDev, uint32_t* pValue);

SSUSB_API int SSUSB_SetPower(HANDLE hDev, BYTE* pBuf, int PowerMode= VER_BASIC_MODE);
SSUSB_API int SSUSB_SetPowerDelay(HANDLE hDev, BYTE* pBuf);
SSUSB_API int SSUSB_SetLVDSFrameSync(HANDLE hDev, uint32_t SyncValue);
SSUSB_API int SSUSB_SetClockFlag(HANDLE hDev);
SSUSB_API int SSUSB_SetClockFlag(HANDLE hDev);
SSUSB_API int SSUSB_PwdnSwitch(HANDLE hDev, BOOL bPwdn);
SSUSB_API int SSUSB_CurrentTestEn(HANDLE hDev, BOOL bEnable);
SSUSB_API int SSUSB_SeletI2CBus(HANDLE hDev, BOOL I2CBus);
SSUSB_API int SSUSB_FX3FirmareUpdate(HANDLE hDev);
SSUSB_API int SSUSB_I2cConfig(HANDLE hDev, BYTE* pBuf);
SSUSB_API int SSUSB_I2CBulkTransfer(HANDLE hDev, BYTE endPoint, BYTE* pData, int Length, int* actualLen, unsigned int timeOut);

SSUSB_API void SSUSB_SetLibMode(int LibMode);

SSUSB_API int SSUSB_WriteFPGAReg(HANDLE hDev, uint32_t addr, uint32_t value);
SSUSB_API int SSUSB_ReadFPGAReg(HANDLE hDev, uint32_t addr, uint32_t* pValue);

SSUSB_API int SSUSB_SetPowerEx(HANDLE hDev, float fVolBuf[7], bool bPowerEnable = false, uint16_t *delaytime = NULL, int PowerMode = VER_BASIC_MODE);
SSUSB_API int SSUSB_SetPowerValue(HANDLE hDev, float fVolBuf[5]);
SSUSB_API int SSUSB_SetPowerOnOff(HANDLE hDev, bool bOnOff[4]);
SSUSB_API int SSUSB_SetMCLKEx(HANDLE hDev, float fMclk, bool bEnable);
SSUSB_API int SSUSB_GetHardwareVersion(HANDLE hDev);
//FX3 FLASH固件更新相关接口
SSUSB_API void SSUSB_EraseFlash(HANDLE hDev);
SSUSB_API void SSUSB_WriteFlash(HANDLE hDev, uint16_t wValue, uint16_t wIndex, BYTE *data, uint16_t wLength, unsigned int timeout);
SSUSB_API void SSUSB_FlashErasePoll(HANDLE hDev, uint16_t wValue, uint16_t wIndex, BYTE *dataa, uint16_t wLength, unsigned int timeout);
SSUSB_API void SSUSB_FlashSPIInit(HANDLE hDev);
SSUSB_API int SSUSB_CX3Config(HANDLE hDev, CyU3PMipicsiCfg tMipiCfg);

// FPGA转发SPI相关接口 Demosens盒子GPIO模拟版本
SSUSB_API int SSUSB_FPGA2SPIRead(HANDLE hDev, uint32_t uSlave, int iSpiMode, float fSpeedMhz, uint32_t uAddress, uint8_t* pVal, uint32_t uAddrLength, uint32_t uValueLength);
SSUSB_API int SSUSB_FPGA2SPIWrite(HANDLE hDev, uint32_t uSlave, int iSpiMode, float fSpeedMhz, uint32_t uAddress, const uint8_t* pVal, uint32_t uAddrLength, uint32_t uValueLength);

//PC与FPGA之间数据读取,读取数据之前需要依次调用SSUSB_OpenDev初始化设备，调用SSUSB_SetSensorResolution、SSUSB_SetSensorBitWidth、SSUSB_OpenSensorChannel、SSUSB_StartCapture设置USB
//读取数据后，需要SSUSB_CloseDev关闭设备

SSUSB_API int SSUSB_SendDataToFPGA(HANDLE hDev, uint8_t* const cpData, uint64_t uDataLen, uint32_t uDDRStartAddr,
	bool bCheckCRC, uint32_t &uRegCRC, uint32_t &uCalCRC);
SSUSB_API int SSUSB_GetDataFromFPGA(HANDLE hDev, uint8_t* cpData, uint64_t uDataLen, uint64_t uSensorDataLen, uint32_t uDDRStartAddr, 
	bool bCheckCRC, uint32_t &uRegCRC, uint32_t &uCalCRC);

// 高速FPGA转发SPI相关接口 用于M系列芯片SPI读写 DDR实现数据交换 速率支持0.1~100Mhz 步长0.1Mhz
SSUSB_API int SSUSB_HighSpeedFPGA2SPIRead(HANDLE hDev, int iSpiMode, float fSpeedMhz, uint32_t uAddress, uint8_t *pVal,
                                          uint32_t uAddrLength, uint32_t uValueLength, bool isIncremental,
                                          uint8_t uDummyTiming);
SSUSB_API int SSUSB_HighSpeedFPGA2SPIWrite(HANDLE hDev, int iSpiMode, float fSpeedMhz, uint32_t uAddress,
                                           const uint8_t *pVal, uint32_t uAddrLength, uint32_t uValueLength,
                                           bool isIncremental);

// HDMI相关接口
// 设置输出通道 1: HDMI A 2: HDMI B 4: USB3.0 , 各通道可组合
SSUSB_API int SSUSB_SetDisplayChannel(HANDLE hDev, uint32_t uChannel);
// 设置输出分辨率与帧率
SSUSB_API int SSUSB_SetHdmiResolutionAndFrameRate(HANDLE hDev, uint32_t uHdmiChannel, uint32_t uWidth, uint32_t uHeight, uint32_t uFrameRate);
// 设置HDMI时钟参数
SSUSB_API int SSUSB_SetHdmiClock(HANDLE hDev, uint32_t uHdmiChannel, float fClockMhz);
// 设置HDMI图像相关参数
SSUSB_API int SSUSB_SetHdmiImageParam(HANDLE hDev, uint32_t uHdmiChannel, uint32_t uHdmiActiveWidth,
                                      uint32_t uHdmiActiveHeight, uint32_t uHdmiTotalWidth, uint32_t uHdmiTotalHeight,
                                      uint32_t uHdmiHsync, uint32_t uHdmiVsync, uint32_t uHdmiHVPol, uint32_t uHdmiHFP,
                                      uint32_t uHdmiHBP, uint32_t uHdmiVFP, uint32_t uHdmiVBP);
#ifdef __cplusplus
}
#endif


#endif