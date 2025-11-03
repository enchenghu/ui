#include "stdafx.h"

#include "BufferPool.h"
#include "InnerMethod.h"
#include "PowerCtrl.h"
#include "SSUSBLib.h"
#include "USBComm.h"
#include "USBOpt.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>

#ifdef _WIN32
#  include <process.h>
#endif

typedef struct ImgSurInfo
{
    UINT32 ImgWidth;
    UINT32 ImgHeight;
    UINT32 BitWidth;
    UINT32 IntF;
	BOOL isVCEnable = FALSE;
	UINT32 VCImageSize;
} ImgSurInfo_t;

static ImgSurInfo_t s_ImgInfo = { 0 };

extern int ImgLen[DEVICE_SUM];
extern int ImgbufferLen[DEVICE_SUM];
extern int actSize[DEVICE_SUM];
extern int gLibMode;
#if LOG
extern int gSaveCnt;
#endif

static SSUSBVerInfo_t s_LibInfo = { 3, 0, 1, "2022_05_23", "SmartSens USB Demo Board Library" };

#define SPI_EN          0x8051

#define SPI_CHIP_ID     0x8060
#define SPI_ADDR_L      0x8061
#define SPI_WDATA       0x8062
#define SPI_RDATA       0x8063
#define SPI_CTRL        0x8064
#define SPI_STAT        0x8065
#define SPI_ADDR_H      0x8066
#define SPI_BURST_LEN   0x8067
#define SPI_BURST_WDATA 0x8070
#define SPI_BURST_RDATA 0x8078

#define DISCRETE_REG_MAX 80

SSUSB_API int SSUSB_EnumDev(int *DevListAddr)
{
    SmartsensDev Sdev;
    memset(&Sdev, 0, sizeof(SmartsensDev));
    Sdev.EnumAddr = DevListAddr;
    if (usb_dev_enum(&Sdev) < 0) {
        return -1;
    }
    printf(" 1 DevCnt = %d \n", Sdev.DevCnt);
    return Sdev.DevCnt;
}

SSUSB_API int SSUSB_EnumDevWithSerialNumber(DevListInfo_t *DevListInfo)
{
    SmartsensDev Sdev;
    memset(&Sdev, 0, sizeof(SmartsensDev));
    Sdev.EnumAddr = DevListInfo->DevAddr;

    if (usb_dev_enum(&Sdev) < 0) {
        return -1;
    }
    memcpy(DevListInfo->uSerialNumber, Sdev.uSerialNumber, sizeof(Sdev.uSerialNumber));
    printf(" 1 DevCnt = %d \n", Sdev.DevCnt);
    return Sdev.DevCnt;
}

SSUSB_API HANDLE *SSUSB_OpenDevAddr(int DevCnt, int *DevIndexaddr, int OpenDevCnt)
{
    // int nDevCnt = 0;
    SmartsensDev Sdev;
    int          nBoardType[DEVICE_SUM] = { 0 };
    HANDLE      *hList                  = usb_dev_open_addr(&Sdev, DevCnt, DevIndexaddr, OpenDevCnt);

    if (hList == NULL) {
        return 0;
    }

    for (int i = 0; i < DEVICE_SUM; i++) {
        if (hList[i]) {
            nBoardType[i] = SSUSB_GetBoardType(hList[i]);
            PowerCtrlInitVoltageList();
            PowerCtrlInitPowerPort(nBoardType[i]);
        }
    }

    s_ImgInfo.BitWidth  = SENSORDATAWIDTH_10BIT;
    s_ImgInfo.IntF      = SENSORINTERFACE_DVP;
    s_ImgInfo.ImgWidth  = 1280;
    s_ImgInfo.ImgHeight = 1024;
	s_ImgInfo.isVCEnable = FALSE;
	s_ImgInfo.VCImageSize = 0;

    return hList;
}

SSUSB_API HANDLE *SSUSB_OpenDev(int &DevCnt)
{
    // 	int nDevCnt = 0;
    int     nBoardType[DEVICE_SUM] = { 0 };
    HANDLE *hList                  = usb_dev_open(DevCnt);

    if (hList == NULL) {
        return 0;
    }

    for (int i = 0; i < DEVICE_SUM; i++) {
        if (hList[i]) {
            nBoardType[i] = SSUSB_GetBoardType(hList[i]);
            PowerCtrlInitVoltageList();
            PowerCtrlInitPowerPort(nBoardType[i]);
        }
    }

    s_ImgInfo.BitWidth  = SENSORDATAWIDTH_10BIT;
    s_ImgInfo.IntF      = SENSORINTERFACE_DVP;
    s_ImgInfo.ImgWidth  = 1280;
    s_ImgInfo.ImgHeight = 1024;

    return hList;
}

// SSUSB_API int SSUSB_OpenDev(HANDLE* hList)
// {
// 	int nDevCnt = 0;
// 	int nBoardType[DEVICE_SUM] = {0};
// 	hList = usb_dev_open(nDevCnt);
// 	HANDLE hDev0 = hList[0];
// 	HANDLE hDev1 = hList[1];
//
// 	PowerCtrlInitVoltageList();
//
// 	if (hDev0)
// 	{
// 		nBoardType[0] = SSUSB_GetBoardType(hDev0);
// 		PowerCtrlInitPowerPort(nBoardType[0]);
// 	}
//
// 	if (hDev1)
// 	{
// 		nBoardType[1] = SSUSB_GetBoardType(hDev1);
// 		PowerCtrlInitPowerPort(nBoardType[1]);
// 	}
//
// 	s_ImgInfo.BitWidth = SENSORDATAWIDTH_10BIT;
// 	s_ImgInfo.IntF = SENSORINTERFACE_DVP;
// 	s_ImgInfo.ImgWidth = 1280;
// 	s_ImgInfo.ImgHeight = 1024;
//
// 	hList[0] = hDev0;
// 	hList[1] = hDev1;
//
// 	return nDevCnt;
// }

SSUSB_API int SSUSB_CloseDev(HANDLE *hDev)
{
    if (!hDev) {
        return -1;
    }

    usb_dev_close(hDev);
    return 0;
}

SSUSB_API int SSUSB_SetSensorIntCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen)
{
    SSUSB_SwitchCommunicationMode(hDev, MODE_I2C);
    BYTE buf[4];
    if (!hDev) {
        return -1;
    }

    buf[0] = SlaveAddr;
    buf[1] = AddrLen;
    buf[2] = DataLen;

    if (CMDWrite(hDev, CMD_SENSORINTCFG, 0, buf, 3) != 3) {
        return -2;
    }

    return 0;

    // 	int ret = CMDWrite(hDev, CMD_SENSORINTCFG, 0, buf, 3);
    // 	return ret;
}

SSUSB_API int SSUSB_WriteSensorReg(HANDLE hDev, uint32_t addr, uint32_t value)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }

    *(uint32_t *)pBuf = value;

    if (CMDWrite(hDev, CMD_WRITESENSORREG, addr, pBuf, sizeof(uint32_t)) != sizeof(uint32_t)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_ReadSensorReg(HANDLE hDev, uint32_t addr, uint32_t *pValue)
{
    if ((!hDev) || (!pValue)) {
        return -1;
    }

    if (CMDRead(hDev, CMD_READSENSORREG, addr, pValue, sizeof(uint32_t)) != sizeof(uint32_t)) {
        *pValue = 0xffff;
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_GetFirmwareInfo(HANDLE hDev, char *pInfo, uint32_t Size)
{
    uint32_t ret;
    BYTE     cmdbuf[MAX_PACKET_PAYLOAD_SIZE];

    if ((!hDev) || (!pInfo) || (Size == 0)) {
        return -1;
    }

    ret = CMDRead(hDev, CMD_READVERINFO, 0, cmdbuf, MAX_PACKET_PAYLOAD_SIZE);
    if (ret == 0) {
        return -2;
    }

    if (strlen((char *)cmdbuf) > Size) {
        return -3;
    }

#ifdef _WIN32
    strcpy_s(pInfo, Size, (char *)cmdbuf);
#else
    strcpy(pInfo, (char *)cmdbuf);
#endif

    return 0;
}

SSUSB_API int SSUSB_GetFrame(HANDLE hDev)
{
    uint32_t ret;
    BYTE     cmdbuf[4];

    if (!hDev) {
        return -1;
    }

    ret = CMDRead(hDev, CMD_GETFRAME, 0, cmdbuf, 4);
    if (ret == 0) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_SetPowerSupply(HANDLE hDev, uint32_t PwrRail, uint32_t ValueINmV)
{
    return PowerCtrlSetVoltage(hDev, PwrRail, ValueINmV);
}

SSUSB_API int SSUSB_EnablePowerSupply(HANDLE hDev, uint32_t PwrRail, BOOL Enable)
{
    return PowerCtrlEnablePower(hDev, PwrRail, Enable);
}

SSUSB_API int SSUSB_OpenSensorChannel(HANDLE hDev)
{
    uint8_t value = 0;
    int     nRet  = 0;

    if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_10BIT) {
        ImgLen[0] = s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 5 / 4;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_12BIT) {
        ImgLen[0] = s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 3 / 2;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_8BIT) {
        ImgLen[0] = s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight;

        //  		ImgLen[1] = s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_14BIT) {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 4) * 7;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_16BIT) {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight) * 2;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422) {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2);
    } else if ((s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_10BIT)
        || (s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_10BIT_NORMAL)) {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2) * 5 / 4;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_12BIT_SPECIAL) // YUV422 12BIT
    {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2 / 2) * 3;
    } else {
        ImgLen[0] = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 2) * 5;
    }
	if (s_ImgInfo.isVCEnable)
	{
		ImgLen[0] = s_ImgInfo.VCImageSize;
	}
    ImgLen[1]       = ImgLen[0]; // device 0 image length = device 1 image length(default)
    ImgbufferLen[0] = ImgLen[0] + USB_IMGRCV_BUFF_SIZE + 10240; // 大于一包+4B包头
    ImgbufferLen[1] = ImgbufferLen[0];
    nRet            = FPGAReadReg(hDev, 0x8000, &value);
    if (nRet != 0) {
        return nRet;
    }

    value |= 0x01;

    nRet = FPGAWriteReg(hDev, 0x8000, value);
    if (nRet != 0) {
        return nRet;
    }

    return 0;
}

SSUSB_API int SSUSB_CloseSensorChannel(HANDLE hDev)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8000, &value);
    if (nRet != 0) {
        return nRet;
    }

    value &= ~0x01;

    return FPGAWriteReg(hDev, 0x8000, value);
}

SSUSB_API int SSUSB_GetVerInfo(SSUSBVerInfo_t *pInfo)
{
    if (!pInfo) {
        return -1;
    }

    pInfo->major = s_LibInfo.major;
    pInfo->minor = s_LibInfo.minor;
    pInfo->micro = s_LibInfo.micro;
#ifdef _WIN32
    strcpy_s(pInfo->date, s_LibInfo.date);
#else
    strcpy(pInfo->date, s_LibInfo.date);
#endif
#ifdef _WIN32
    strcpy_s(pInfo->description, s_LibInfo.description);
#else
    strcpy(pInfo->description, s_LibInfo.description);
#endif

    return 0;
}

SSUSB_API int SSUSB_SetSensorPWDN(HANDLE hDev, BOOL Level)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8000, &value);
    if (nRet != 0) {
        return nRet;
    }

    if (Level) {
        value |= 0x02;
    } else {
        value &= ~(0x02);
    }

    return FPGAWriteReg(hDev, 0x8000, value);
}

SSUSB_API int SSUSB_SetSensorReset(HANDLE hDev, BOOL Level)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8000, &value);
    if (nRet != 0) {
        return nRet;
    }

    if (Level) {
        value |= 0x04;
    } else {
        value &= ~(0x04);
    }

    return FPGAWriteReg(hDev, 0x8000, value);
}

SSUSB_API int SSUSB_SetSensorInterface(HANDLE hDev, uint32_t Int, uint32_t LaneNum)
{
    uint8_t value = 0;
    int     nRet  = 0;

	if (Int == SENSORINTERFACE_MIPI_CPHY)
	{
		nRet = FPGAReadReg(hDev, 0x80a6, &value);
		nRet = FPGAWriteReg(hDev, 0x80a6, (value|0x01));
		if (nRet != 0) {
			return nRet;
		}
	}
	else
	{
		value = (uint8_t)(Int & 0x00000003);

		nRet = FPGAWriteReg(hDev, 0x8001, value);
		if (nRet != 0) {
			return nRet;
		}

		if (Int != SENSORINTERFACE_DVP) // LVDS:0x8002 bit[5:4]; MIPI:1c4d x2 mode 1/2 0x8002 bit[7:6]
		{
			nRet = FPGAReadReg(hDev, 0x8002, &value);
			if (nRet != 0) {
				return nRet;
			}

			value &= ~(0xf0);
			value |= (uint8_t)(LaneNum & 0x000000f0);
			nRet = FPGAWriteReg(hDev, 0x8002, value);
			if (nRet == 0) {
				s_ImgInfo.IntF = Int;
			}
		}
		else // DVP
		{
			nRet = FPGAReadReg(hDev, 0x8001, &value);
			if (nRet != 0) {
				return nRet;
			}

			value &= 0x9f;
			value |= (uint8_t)((LaneNum & 0x03) << 5);
			nRet = FPGAWriteReg(hDev, 0x8001, value);
			if (nRet != 0) {
				return nRet;
			}
		}
	}
    return nRet;
}

SSUSB_API int SSUSB_SetSensorBitWidth(HANDLE hDev, uint32_t BitWidth)
{
    uint8_t value = 0;
    int     nRet  = 0;

    // 	s_ImgInfo.BitWidth = BitWidth;

    nRet = FPGAReadReg(hDev, 0x8002, &value);
    if (nRet != 0) {
        return nRet;
    }

    value &= ~(0x0f);
    value |= (uint8_t)(BitWidth & 0x0000000f);
    nRet = FPGAWriteReg(hDev, 0x8002, value);
    if (nRet == 0) {
        s_ImgInfo.BitWidth = BitWidth;
    }

    return nRet;
}

SSUSB_API int SSUSB_SetSensorBitWidthEx(HANDLE hDev, uint32_t BitWidth, uint32_t SenBitWidth)
{
    uint8_t value = 0;
    int     nRet  = 0;
    if (SenBitWidth == -1) {
        SenBitWidth = BitWidth;
    }
    s_ImgInfo.BitWidth = BitWidth;

    // senbitwidth
    nRet = FPGAReadReg(hDev, 0x8008, &value);
    if (nRet != 0) {
        return nRet;
    }

    value &= ~(0x0f);
    value |= (uint8_t)(SenBitWidth & 0x0000000f);
    nRet = FPGAWriteReg(hDev, 0x8008, value);

    // bitwidth
    value = 0;
    nRet  = FPGAReadReg(hDev, 0x8002, &value);
    if (nRet != 0) {
        return nRet;
    }

    if (BitWidth == SENSORDATAWIDTH_YUV422_10BIT_NORMAL) {
        value &= ~(0x0f);
        value |= (uint8_t)(0x0c & 0x0000000f);
        nRet = FPGAWriteReg(hDev, 0x8002, value);
    } else {
        value &= ~(0x0f);
        value |= (uint8_t)(BitWidth & 0x0000000f);
        nRet = FPGAWriteReg(hDev, 0x8002, value);
    }

    // if (nRet == 0)
    //{
    //     s_ImgInfo.BitWidth = BitWidth;
    // }
    return nRet;
}

SSUSB_API int SSUSB_SetSensorResolution(HANDLE hDev, uint32_t Width, uint32_t Height)
{
    uint8_t value = 0;
    int     nRet  = 0;

    //	s_ImgInfo.ImgWidth = Width;
    //	s_ImgInfo.ImgHeight = Height;

    value = (uint8_t)((Width & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8003, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(Width & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8004, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)((Height & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8005, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(Height & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8006, value);
    if (nRet != 0) {
        return nRet;
    }

    s_ImgInfo.ImgWidth  = Width;
    s_ImgInfo.ImgHeight = Height;

    return 0;
}

SSUSB_API int SSUSB_WriteFPGAReg(HANDLE hDev, uint32_t addr, uint32_t value)
{
    if (!hDev) {
        return -1;
    }

    int nRet = FPGAWriteReg(hDev, addr, value);
    if (nRet != 0) {
        return nRet;
    }

    return 0;
}

SSUSB_API int SSUSB_ReadFPGAReg(HANDLE hDev, uint32_t addr, uint32_t *pValue)
{
    if (!hDev) {
        return -1;
    }

    uint8_t value = 0;
    int     nRet  = 0;
    nRet          = FPGAReadReg(hDev, addr, &value);
    if (nRet != 0) {
        return nRet;
    }
    *pValue = value; // B section

    return 0;
}

SSUSB_API int SSUSB_SetSensorResolutionEx(HANDLE hDev, uint32_t Width, uint32_t Height, uint32_t SenWidth,
    uint32_t SenHeight)
{
    uint8_t value = 0;
    int     nRet  = 0;

    s_ImgInfo.ImgWidth  = Width;
    s_ImgInfo.ImgHeight = Height;

    int isenwidth  = SenWidth;
    int isenheight = SenHeight;
    if ((isenwidth == 0) || (isenheight == 0)) {
        isenwidth  = Width;
        isenheight = Height;
    }

    value = (uint8_t)((isenwidth & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8003, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(isenwidth & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8004, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)((isenheight & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8005, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(isenheight & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8006, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)((Width & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8030, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(Width & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8031, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)((Height & 0x0000FF00) >> 8);
    nRet  = FPGAWriteReg(hDev, 0x8032, value);
    if (nRet != 0) {
        return nRet;
    }

    value = (uint8_t)(Height & 0x000000FF);
    nRet  = FPGAWriteReg(hDev, 0x8033, value);
    if (nRet != 0) {
        return nRet;
    }

    return 0;
}

SSUSB_API int SSUSB_SetVCModeEx(HANDLE hDev, uint32_t VCImageSize)
{

	if (!hDev) {
		return -1;
	}
	s_ImgInfo.isVCEnable = TRUE;
	s_ImgInfo.VCImageSize = VCImageSize;
	return 0;
}

SSUSB_API int SSUSB_SetSensorClock(HANDLE hDev, uint32_t Freq)
{
    uint8_t value = 0;
    int     nRet  = 0;

    value = Freq;

    return FPGAWriteReg(hDev, 0x8007, value);
}

SSUSB_API int SSUSB_TrgOneFrame(HANDLE hDev)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8024, &value);
    if (nRet != 0) {
        return nRet;
    }

    value |= 0x01;
    nRet = FPGAWriteReg(hDev, 0x8024, value);
    if (nRet != 0) // error
    {
        return nRet;
    }

    value &= ~(0x01);
    nRet = FPGAWriteReg(hDev, 0x8024, value);
    if (nRet != 0) {
        return nRet;
    }

    return 0;
}

SSUSB_API int SSUSB_GetBoardType(HANDLE hDev)
{
    return GetBoardType(hDev);
}

SSUSB_API int SSUSB_SetSensGndConn(HANDLE hDev, BOOL bConn)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8000, &value);
    if (nRet != 0) {
        return nRet;
    }

    if (bConn) {
        value |= 0x04;
    } else {
        value &= ~(0x04);
    }

    return FPGAWriteReg(hDev, 0x8000, value);
}

SSUSB_API int SSUSB_ResetSensor(HANDLE hDev)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }

    if (CMDWrite(hDev, CMD_RESETSESNOR, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_StartCapture(HANDLE hDev)
{
    if (!hDev) {
        return -1;
    }
    return usb_capture_start(hDev);
}

SSUSB_API int SSUSB_StopCapture(HANDLE hDev)
{
    if (!hDev) {
        return -1;
    }

    return usb_capture_stop(hDev);
}

SSUSB_API int SSUSB_FetchFrame(HANDLE hDev, void *pFrame, int BufferSize, int *pFrameSize, FrameInfoV0_t *pFrameInfo,
    uint32_t Timeout)
{
    int        nRet = 0;
    UINT32     size = 0;
    static int frameSize[256];
    static int cnt;
    if ((!hDev) || (!pFrame) || (BufferSize == 0) || (!pFrameInfo) || (!pFrameSize)) {
        return -1;
    }

    if (s_ImgInfo.BitWidth == 0) // 8bit
    {
        size = s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight;
    } else if (s_ImgInfo.BitWidth == 1) // 10bit
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 4) * 5;
    } else if (s_ImgInfo.BitWidth == 2) // 12bit
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 2) * 3;
    } else if (s_ImgInfo.BitWidth == 4) // 14bit 7B->4pixel
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 4) * 7;
    } else if (s_ImgInfo.BitWidth == 5) // 16bit
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight) * 2;
    } else if (s_ImgInfo.BitWidth == 7) // YUV422
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2);
    } else if ((s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_10BIT)
        || (s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_10BIT_NORMAL)) // YUV422 10BIT
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2) * 5 / 4;
    } else if (s_ImgInfo.BitWidth == SENSORDATAWIDTH_YUV422_12BIT_SPECIAL) // YUV422 12BIT
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight * 2 / 2) * 3;
    }

    else // 20bit 5B->2pixel
    {
        size = (s_ImgInfo.ImgWidth * s_ImgInfo.ImgHeight / 2) * 5;
    }

	if (s_ImgInfo.isVCEnable)
	{
		size = s_ImgInfo.VCImageSize;
	}
    nRet = usb_get_frame(hDev, pFrame, BufferSize, pFrameSize, Timeout);
    if (nRet == 0) {
        if (cnt == 256) {
            cnt = 0;
        }

        frameSize[cnt] = *pFrameSize;
        cnt++;

        if (*pFrameSize != size) {
            return -2;
        }

        pFrameInfo->bitwidth = s_ImgInfo.BitWidth;
        pFrameInfo->width    = s_ImgInfo.ImgWidth;
        pFrameInfo->height   = s_ImgInfo.ImgHeight;
#ifdef _WIN32
        pFrameInfo->timestamp = GetTickCount();
#else
        struct timeval tp;
        gettimeofday(&tp, NULL);
        pFrameInfo->timestamp = tp.tv_sec * 1000 + tp.tv_usec / 1000;
#endif
        pFrameInfo->size = *pFrameSize;
    }

    return nRet;
}

SSUSB_API int SSUSB_USB2I2CCfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus, BYTE WriteDelay,
    BYTE ReadDelay)
{
    if (I2cbus == 0) {
        SSUSB_SwitchCommunicationMode(hDev, MODE_I2C);
    }
    BYTE buf[6];
    if (!hDev) {
        return -1;
    }
    buf[0] = SlaveAddr;
    buf[1] = AddrLen;
    buf[2] = DataLen;
    buf[3] = I2cbus;
    buf[4] = WriteDelay;
    buf[5] = ReadDelay;

    if (CMDWrite(hDev, CMD_USB2I2CCFG, 0, buf, 6) != 6) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2I2CWrite(HANDLE hDev, uint32_t addr, uint32_t value)
{
    UINT8 pBuf[4];
    if (!hDev) {
        return -1;
    }
    *(uint32_t *)pBuf = value;
    if (CMDWrite(hDev, CMD_USB2I2CWRITE, addr, pBuf, sizeof(uint32_t)) != sizeof(uint32_t)) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2I2CWriteBrust(HANDLE hDev, uint32_t addr, uint8_t *pBuf, uint32_t size)
{
    // UINT8 pBuf[4];
    if (!hDev || (!pBuf)) {
        return -1;
    }
    //*(uint32_t*)pBuf = value;
    if (CMDWrite(hDev, CMD_USB2I2CWRITEBURST, addr, pBuf, size) != size) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2I2CRead(HANDLE hDev, uint32_t addr, uint32_t *pValue)
{
    if ((!hDev) || (!pValue)) {
        return -1;
    }
    if (CMDRead(hDev, CMD_USB2I2CREAD, addr, pValue, sizeof(uint32_t)) != sizeof(uint32_t)) {
        *pValue = 0xffff;
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2I2CReadBrust(HANDLE hDev, uint32_t addr, uint8_t *pBuf, uint32_t size)
{
    if ((!hDev) || (!pBuf)) {
        return -1;
    }
    if (CMDRead(hDev, CMD_USB2I2CREADBURST, addr, pBuf, size) != size) {
        *pBuf = 0xffff;
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2I2CWriteDiscrete(HANDLE hDev, const uint32_t pAddr[], const uint32_t pValue[], const uint32_t pDelay[], uint32_t iRegCnt, 
	BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus, BYTE WriteDelay, BYTE ReadDelay)
{
	if ((!hDev) || (!pAddr) || (!pValue) || (!pDelay))
	{
		return -1;
	}

	

	struct StDiscreteData
	{
		uint32_t addr;
		uint32_t data;
		uint32_t delay;
	};

	std::vector<StDiscreteData> vecSendData(iRegCnt);
	for (int i = 0; i < iRegCnt; i++)
	{
		vecSendData[i].addr = pAddr[i];
		vecSendData[i].data = pValue[i];
		vecSendData[i].delay = pDelay[i];
	}

	int iCountSend = 0;
	while (iCountSend < iRegCnt)
	{
		int iRet = SSUSB_USB2I2CCfg(hDev, SlaveAddr, AddrLen, DataLen, I2cbus, WriteDelay, ReadDelay);
		if (iRet)
		{
			return iRet;
		}

		UINT8 *pBuf = (UINT8*)(vecSendData.data());
		uint32_t iRealSend = (iRegCnt - iCountSend) >= DISCRETE_REG_MAX ? DISCRETE_REG_MAX : (iRegCnt - iCountSend);
		//int iRet = SSUSB_USB2I2CCfg(hDev, 0x30, 2, 1, 0, 0, 1);
		//if (iRet)
		//{
		//	return iRet;
		//}\
		//buffer偏移
		int iBufOffset = iCountSend * 3 * sizeof(uint32_t);
		int iSendBytes = iRealSend * 3 * sizeof(uint32_t);
		if (CMDWrite(hDev, CMD_USB2I2CWRITEDISCRETE, 0, pBuf + iBufOffset, iSendBytes) != iSendBytes) {
			return -2;
		}

		iCountSend += iRealSend;
	}

	return 0;
}

SSUSB_API int SSUSB_USB2I2CReadDiscrete(HANDLE hDev, const uint32_t pAddr[], uint32_t pValue[], const uint32_t pDelay[], uint32_t iRegCnt, 
	BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen, BYTE I2cbus, BYTE WriteDelay, BYTE ReadDelay)
{
	if ((!hDev) || (!pAddr) || (!pValue) || (!pDelay))
	{
		return -1;
	}

	struct StDiscreteData
	{
		uint32_t addr;
		uint32_t value;
		uint32_t delay;
	};

	std::vector<StDiscreteData> vecSendData(iRegCnt);
	for (int i = 0; i < iRegCnt; i++)
	{
		vecSendData[i].addr = pAddr[i];
		vecSendData[i].value = pValue[i];
		vecSendData[i].delay = pDelay[i];
	}

	std::vector<StDiscreteData> vecRecvData(iRegCnt);

	int iCountSend = 0;
	while (iCountSend < iRegCnt)
	{
		int iRet = SSUSB_USB2I2CCfg(hDev, SlaveAddr, AddrLen, DataLen, I2cbus, WriteDelay, ReadDelay);
		if (iRet)
		{
			return iRet;
		}

		UINT8 *pSendBuf = (UINT8*)(vecSendData.data());
		UINT8 *pRecvBuf = (UINT8*)(vecRecvData.data());
		uint32_t iRealSend = (iRegCnt - iCountSend) >= DISCRETE_REG_MAX ? DISCRETE_REG_MAX : (iRegCnt - iCountSend);
		int iBufOffset = iCountSend * 3 * sizeof(uint32_t);
		int iSendBytes = iRealSend * 3 * sizeof(uint32_t);

		if (CMDRead(hDev, CMD_USB2I2CREADDISCRETE, 0, pRecvBuf + iBufOffset, iSendBytes, pSendBuf + iBufOffset, iSendBytes) != iSendBytes) 
		{
			return -2;
		}

		iCountSend += iRealSend;
	}

	for (int i = 0; i < iRegCnt; i++)
	{
		pValue[i] = vecRecvData[i].value;
	}

	return 0;
}

SSUSB_API int SSUSB_USB2SPICfg(HANDLE hDev, BYTE SlaveAddr, BYTE AddrLen, BYTE DataLen)
{
    BYTE buf[6];
    if (!hDev) {
        return -1;
    }
    buf[0] = SlaveAddr;
    buf[1] = AddrLen;
    buf[2] = DataLen;

    if (CMDWrite(hDev, CMD_USB2SPICONFIG, 0, buf, 3) != 3) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2SPIWrite(HANDLE hDev, uint32_t addr, uint32_t pValue)
{
    UINT8 pBuf[4];
    if (!hDev) {
        return -1;
    }
    *(uint32_t *)pBuf = pValue;
    if (CMDWrite(hDev, CMD_USB2SPIWRITE, addr, pBuf, sizeof(uint32_t)) != sizeof(uint32_t)) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2SPIWriteBrust(HANDLE hDev, uint32_t addr, uint8_t *pBuf, uint32_t size)
{
    if (!hDev || (!pBuf)) {
        return -1;
    }
    if (CMDWrite(hDev, CMD_USB2SPIWRITEBURST, addr, pBuf, size) != size) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2SPIRead(HANDLE hDev, uint32_t addr, uint32_t *pValue)
{
    if ((!hDev) || (!pValue)) {
        return -1;
    }
    if (CMDRead(hDev, CMD_USB2SPIREAD, addr, pValue, sizeof(uint32_t)) != sizeof(uint32_t)) {
        *pValue = 0xffff;
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_USB2SPIReadBrust(HANDLE hDev, uint32_t addr, uint8_t *pBuf, uint32_t size)
{
    if ((!hDev) || (!pBuf)) {
        return -1;
    }
    if (CMDRead(hDev, CMD_USB2SPIREADBURST, addr, pBuf, size) != size) {
        *pBuf = 0xffff;
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_CurrentADC(HANDLE hDev, uint8_t *pValue)
{
    if ((!hDev) || (!pValue)) {
        return -1;
    }
    if (CMDRead(hDev, CMD_READCURRENT, 0, pValue, CURRENT_SIZE * sizeof(uint8_t)) != (CURRENT_SIZE * sizeof(uint8_t))) {
        return -2;
    }
    return 0;
}

SSUSB_API int SSUSB_ResetFX3(HANDLE hDev)
{

    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }

    if (CMDWrite(hDev, CMD_RESETFX3, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_GetVersionFX3(HANDLE hDev, BYTE *pBuf)
{
    if (!hDev || !pBuf) {
        return -1;
    }
    if (CMDRead(hDev, CMD_GETVERSIONFX3, 0, pBuf, 6 * sizeof(BYTE)) != (6 * sizeof(BYTE))) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_GetVersionFPGA(HANDLE hDev, BYTE *pBuf)
{
    if (!hDev) {
        return -1;
    }

    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8011, &value);
    if (nRet != 0) {
        return nRet;
    }
    pBuf[0] = value; // A section

    if (value >= 3) {
        nRet = FPGAReadReg(hDev, 0x8010, &value);
        if (nRet != 0) {
            return nRet;
        }
        pBuf[1] = value; // B section, high byte

        nRet = FPGAReadReg(hDev, 0x8019, &value);
        if (nRet != 0) {
            return nRet;
        }
        pBuf[2] = value; // B section, low byte

        nRet = FPGAReadReg(hDev, 0x8012, &value);
        if (nRet != 0) {
            return nRet;
        }
        pBuf[3] = value; // C section

        nRet = FPGAReadReg(hDev, 0x8017, &value); // D section High Byte
        if (nRet != 0) {
            return nRet;
        }
        pBuf[4] = value;

        nRet = FPGAReadReg(hDev, 0x8018, &value); // D section Low Byte
        if (nRet != 0) {
            return nRet;
        }
        pBuf[5] = value;
    } else {
        nRet = FPGAReadReg(hDev, 0x8010, &value);
        if (nRet != 0) {
            return nRet;
        }
        pBuf[1] = value; // B section, high byte

        nRet = FPGAReadReg(hDev, 0x8012, &value);
        if (nRet != 0) {
            return nRet;
        }
        pBuf[2] = value; // C section

        nRet = FPGAReadReg(hDev, 0x8017, &value); // D section High Byte
        if (nRet != 0) {
            return nRet;
        }
        pBuf[3] = value;

        nRet = FPGAReadReg(hDev, 0x8018, &value); // D section Low Byte
        if (nRet != 0) {
            return nRet;
        }
        pBuf[4] = value;
    }

    return 0;
}

// FPGA B section 部分，2022/2/14由1 Byte改为2 Bytes
SSUSB_API int SSUSB_GetFPGAMAJORVersion(HANDLE hDev, uint32_t *pValue)
{
    if (!hDev) {
        return -1;
    }

    uint8_t value = 0;
    int     nRet  = 0;
    nRet          = FPGAReadReg(hDev, 0x8010, &value);
    if (nRet != 0) {
        return nRet;
    }
    *pValue = value << 8; // B section high byte

    nRet = FPGAReadReg(hDev, 0x8019, &value);
    if (nRet != 0) {
        return nRet;
    }
    *pValue += value; // B section low byte
}

// pBuf:(2n)Byte data,(n=PowerMode) AVDD(2B) DOVDD(2B) DVDD(2B) AFVCC(2B) AVDD2CD(2B) DOVDD2CD(2B) DVDD2CD(2B)
SSUSB_API int SSUSB_SetPower(HANDLE hDev, BYTE *pBuf, int PowerMode)
{
    if ((!hDev) || (!pBuf)) {
        return -1;
    }
    if (CMDWrite(hDev, CMD_POWERCFG, 0, pBuf, (2* PowerMode) * sizeof(BYTE)) != ((2 * PowerMode) * sizeof(BYTE))) {
        return -2;
    }

    return 0;
}
// pBuf:8bytes power value + 1byte pwdn value + 1byte rst value + (1byte power index + 2bytes delay)*6 = 26 bytes
// avdd dovdd dvdd afvcc pwdn rst
SSUSB_API int SSUSB_SetPowerDelay(HANDLE hDev, BYTE *pBuf)
{
    if ((!hDev) || (!pBuf)) {
        return -1;
    }
    if (CMDWrite(hDev, CMD_POWERTEST, 0, pBuf, 28 * sizeof(BYTE)) != (28 * sizeof(BYTE))) {
        return -2;
    }

    return 0;
}

// LVDS 帧头与帧尾同步机制设置，SyncValue：0，兼容之前；1：新模式
SSUSB_API int SSUSB_SetLVDSFrameSync(HANDLE hDev, uint32_t SyncValue)
{
    uint8_t value = 0;
    int     nRet  = 0;

    nRet = FPGAReadReg(hDev, 0x8001, &value);
    if (nRet != 0) {
        return nRet;
    }

    value |= (SyncValue << 7);

    return FPGAWriteReg(hDev, 0x8001, value);
}

// 时钟调试标记位
SSUSB_API int SSUSB_SetClockFlag(HANDLE hDev)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }

    if (CMDWrite(hDev, CMD_CLOCKFLAG, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_PwdnSwitch(HANDLE hDev, BOOL bPwdn)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }
    pBuf[0] = bPwdn;
    if (CMDWrite(hDev, CMD_PWDNADC, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_CurrentTestEn(HANDLE hDev, BOOL bEnable)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }
    pBuf[0] = bEnable;
    if (CMDWrite(hDev, CMD_CURRENTTEST, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE)) {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_SeletI2CBus(HANDLE hDev, BOOL I2CBus)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }
    pBuf[0] = I2CBus;
    // if (CMDWrite(hDev, CMD_CHANGEIICBUS, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE))
    {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_FX3FirmareUpdate(HANDLE hDev)
{
    UINT8 pBuf[4];

    if (!hDev) {
        return -1;
    }

    // if (CMDWrite(hDev, CMD_FX3FWUPDATE, 0, pBuf, sizeof(BYTE)) != sizeof(BYTE))
    {
        return -2;
    }

    return 0;
}

SSUSB_API int SSUSB_I2cConfig(HANDLE hDev, BYTE *pBuf)
{
    if ((!hDev) || (!pBuf)) {
        return -1;
    }
    if (CMDWrite(hDev, CMD_I2CCONFIG, 0, pBuf, 2 * sizeof(BYTE)) != (2 * sizeof(BYTE))) {
        return -2;
    }
}

SSUSB_API int SSUSB_I2CBulkTransfer(HANDLE hDev, BYTE endPoint, BYTE *pData, int Length, int *actualLen,
    unsigned int timeOut)
{
    if ((!hDev) || (!pData)) {
        return -1;
    }

    int ret = libusb_bulk_transfer((libusb_device_handle *)hDev, endPoint, pData, Length, actualLen, timeOut);
    if (ret != 0) {
        return -2;
    }

    return 0;
}

SSUSB_API void SSUSB_SetLibMode(int LibMode)
{
    gLibMode = LibMode;
}

SSUSB_API void SSUSB_EraseFlash(HANDLE hDev)
{
    libusb_control_transfer((libusb_device_handle *)hDev, 0x40, 0xC4, 1, 1, NULL, 0, 10);
}

SSUSB_API void SSUSB_WriteFlash(HANDLE hDev, uint16_t wValue, uint16_t wIndex, BYTE *data, uint16_t wLength,
    unsigned int timeout)
{
    libusb_control_transfer((libusb_device_handle *)hDev, CY_U3P_USB_VENDOR_RQT, CY_FX_RQT_SPI_FLASH_WRITE, wValue,
        wIndex, data, wLength, timeout);
}

SSUSB_API void SSUSB_FlashErasePoll(HANDLE hDev, uint16_t wValue, uint16_t wIndex, BYTE *data, uint16_t wLength,
    unsigned int timeout)
{
    // BYTE* pstate = (BYTE *)malloc(1);
    libusb_control_transfer((libusb_device_handle *)hDev, CY_U3P_USB_VENDOR_RQT, CY_FX_RQT_SPI_FLASH_ERASE_POLL, wValue,
        wIndex, data, wLength, timeout);
}

SSUSB_API void SSUSB_FlashSPIInit(HANDLE hDev)
{
    libusb_control_transfer((libusb_device_handle *)hDev, CY_U3P_USB_VENDOR_RQT, CY_FX_RQT_SYS_MEM_READ, 0, 0, NULL, 0,
        1000);
}

SSUSB_API int SSUSB_CX3Config(HANDLE hDev, CyU3PMipicsiCfg tMipiCfg)
{

    if (CMDWrite(hDev, CMD_CX3MIPICONFIG, 0, &tMipiCfg, sizeof(CyU3PMipicsiCfg)) != 3) {
        return -2;
    }
    return 0;
}

const std::vector<float> VEC_SPI_SPEED{
    8,
    6,
    3,
    1.5,
    2,
    6.25,
    12.5,
    20,
    25,
    15,
};

const std::string SPI_DEBUG_OUT_FILE = "spi_debug.txt";
SSUSB_API int     SSUSB_FPGA2SPIRead(HANDLE hDev, uint32_t uSlave, int iSpiMode, float fSpeedMhz, uint32_t uAddress,
        uint8_t *pVal, uint32_t uAddrLength, uint32_t uValueLength)
{
    SSUSB_SwitchCommunicationMode(hDev, MODE_SPI);
    bool bValidSpeed = false;
    for (int i = 0; i < VEC_SPI_SPEED.size(); i++) {
        if (fSpeedMhz == VEC_SPI_SPEED[i]) {
            SSUSB_WriteFPGAReg(hDev, 0x8068, i);
            bValidSpeed = true;
        }
    }
    if (!bValidSpeed) {
        return -2;
    }
    unsigned int uTmpValue = 0 |                                    // spi enable
        (((iSpiMode & SPI_CPOL) == SPI_CPOL) << 1) |                // spi cpol
        (((iSpiMode & SPI_CPHA) == SPI_CPHA) << 2) |                // spi cpha
        (((iSpiMode & SPI_RW_EDGE_DIFF) != SPI_RW_EDGE_DIFF) << 3); // spi rw edge 0:diff 1:same
    SSUSB_WriteFPGAReg(hDev, SPI_CTRL, uTmpValue);
    uTmpValue = uValueLength - 1;
    SSUSB_WriteFPGAReg(hDev, SPI_BURST_LEN, uTmpValue);
    if (uSlave != 0x7f) {
        uTmpValue = (((((uAddress >> 8) <= 0x01) ? (uAddress >> 8) : ((uAddress >> 8) - 0x2e)) << 1)) | 0x1;
        SSUSB_WriteFPGAReg(hDev, SPI_CHIP_ID, uTmpValue);
        uTmpValue = uAddress & 0xff;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_L, uTmpValue);
    } else {
        uTmpValue = 0xff;
        SSUSB_WriteFPGAReg(hDev, SPI_CHIP_ID, uTmpValue);
        uTmpValue = uAddress & 0xff;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_L, uTmpValue);
        uTmpValue = uAddress >> 8;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_H, uTmpValue);
    }

    // clear write buffer
    for (int i = 0; i < 8; i++) {
        SSUSB_WriteFPGAReg(hDev, SPI_BURST_WDATA, 0);
    }

    uTmpValue = 1 |                                                 // spi enable
        (((iSpiMode & SPI_CPOL) == SPI_CPOL) << 1) |                // spi cpol
        (((iSpiMode & SPI_CPHA) == SPI_CPHA) << 2) |                // spi cpha
        (((iSpiMode & SPI_RW_EDGE_DIFF) != SPI_RW_EDGE_DIFF) << 3); // spi rw edge 0:diff 1:same
    SSUSB_WriteFPGAReg(hDev, SPI_CTRL, uTmpValue);
    clock_t start;
    start = clock();
    while (true) {
        uTmpValue = 0;
        SSUSB_ReadFPGAReg(hDev, SPI_STAT, &uTmpValue);
        if (uTmpValue == 1 || clock() - start > 1000) {
            break;
        }
    }
    int iRet = uTmpValue == 1 ? 0 : -1;
    for (int i = 0; i < uValueLength; i++) {
        uTmpValue = 0;
        SSUSB_ReadFPGAReg(hDev, SPI_BURST_RDATA + i, &uTmpValue);
        *(pVal + i) = uTmpValue & 0xff;
    }
    // if spi debug out file exists, record spi debug info
    std::ofstream spiDebugOutFile(SPI_DEBUG_OUT_FILE, std::ios::app);
    if (spiDebugOutFile.is_open()) {
        spiDebugOutFile << "SPI Read, "
                        << "slave id: " << std::hex << uSlave << ", address: " << std::setw(4) << std::setfill('0')
                        << uAddress << ", value length: " << uValueLength << ", value: ";
        for (int i = 0; i < uValueLength; i++) {
            spiDebugOutFile << std::hex << std::setw(2) << std::setfill('0') << (int)(*(pVal + i)) << " ";
        }
        spiDebugOutFile << std::endl;
        spiDebugOutFile.close();
    }
    return iRet;
}

SSUSB_API int SSUSB_FPGA2SPIWrite(HANDLE hDev, uint32_t uSlave, int iSpiMode, float fSpeedMhz, uint32_t uAddress,
    const uint8_t *pVal, uint32_t uAddrLength, uint32_t uValueLength)
{
    SSUSB_SwitchCommunicationMode(hDev, MODE_SPI);
    bool bValidSpeed = false;
    for (int i = 0; i < VEC_SPI_SPEED.size(); i++) {
        if (fSpeedMhz == VEC_SPI_SPEED[i]) {
            SSUSB_WriteFPGAReg(hDev, 0x8068, i);
            bValidSpeed = true;
        }
    }
    if (!bValidSpeed) {
        return -2;
    }
    // if spi debug out file exists, record spi debug info
    std::ofstream spiDebugOutFile(SPI_DEBUG_OUT_FILE, std::ios::app);
    if (spiDebugOutFile.is_open()) {
        spiDebugOutFile << "SPI Write, "
                        << "slave id: " << std::hex << uSlave << ", address: " << std::setw(4) << std::setfill('0')
                        << uAddress << ", value length: " << uValueLength << ", value: ";
        for (int i = 0; i < uValueLength; i++) {
            spiDebugOutFile << std::hex << std::setw(2) << std::setfill('0') << (int)(*(pVal + i)) << " ";
        }
        spiDebugOutFile << std::endl;
        spiDebugOutFile.close();
    }
    unsigned int uTmpValue = 0 |                                    // spi enable
        (((iSpiMode & SPI_CPOL) == SPI_CPOL) << 1) |                // spi cpol
        (((iSpiMode & SPI_CPHA) == SPI_CPHA) << 2) |                // spi cpha
        (((iSpiMode & SPI_RW_EDGE_DIFF) != SPI_RW_EDGE_DIFF) << 3); // spi rw edge 0:diff 1:same
    SSUSB_WriteFPGAReg(hDev, SPI_CTRL, uTmpValue);
    uTmpValue = uValueLength - 1;
    SSUSB_WriteFPGAReg(hDev, SPI_BURST_LEN, uTmpValue);
    if (uSlave != 0x7f) {
        uTmpValue = ((((uAddress >> 8) <= 0x01) ? (uAddress >> 8) : ((uAddress >> 8) - 0x2e)) << 1);
        SSUSB_WriteFPGAReg(hDev, SPI_CHIP_ID, uTmpValue);
        uTmpValue = uAddress & 0xff;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_L, uTmpValue);
    } else {
        uTmpValue = 0xfe;
        SSUSB_WriteFPGAReg(hDev, SPI_CHIP_ID, uTmpValue);
        uTmpValue = uAddress & 0xff;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_L, uTmpValue);
        uTmpValue = uAddress >> 8;
        SSUSB_WriteFPGAReg(hDev, SPI_ADDR_H, uTmpValue);
    }
    for (int i = 0; i < uValueLength; i++) {
        uTmpValue = *(pVal + i);
        SSUSB_WriteFPGAReg(hDev, SPI_BURST_WDATA + i, uTmpValue);
    }
    uTmpValue = 1 |                                                 // spi enable
        (((iSpiMode & SPI_CPOL) == SPI_CPOL) << 1) |                // spi cpol
        (((iSpiMode & SPI_CPHA) == SPI_CPHA) << 2) |                // spi cpha
        (((iSpiMode & SPI_RW_EDGE_DIFF) != SPI_RW_EDGE_DIFF) << 3); // spi rw edge 0:diff 1:same
    SSUSB_WriteFPGAReg(hDev, SPI_CTRL, uTmpValue);
    clock_t start;
    start = clock();
    while (true) {
        uTmpValue = 0;
        SSUSB_ReadFPGAReg(hDev, SPI_STAT, &uTmpValue);
        if (uTmpValue == 1 || clock() - start > 1000) {
            break;
        }
    }
    int iRet = uTmpValue == 1 ? 0 : -1;
    return iRet;
}

SSUSB_API int SSUSB_GetHardwareVersion(HANDLE hDev)
{
    if (!hDev)
        return -1;
    unsigned int  iHardwareVer = 0;
    unsigned char pbuf[10]     = { 0 };
    SSUSB_GetVersionFPGA(hDev, pbuf);
    iHardwareVer = 0;
    if (pbuf[0] >= 3) // 2000T 以后硬件版本
    {
        iHardwareVer = (pbuf[1] << 8) + pbuf[2];
    } else {
        iHardwareVer = pbuf[1];
    }
    return iHardwareVer;
}

SSUSB_API int SSUSB_SetPowerEx(HANDLE hDev, float fVolBuf[7], bool bPowerEnable, uint16_t *delaytime, int PowerMode)
{
    int iRet = -1;
    if (!hDev)
        return iRet;
    unsigned int iHardwareVer = 0;
    bool         bPowerUp     = false;
	if (PowerMode == VER_BASIC_MODE)
	{
		for (int i = 0; i < 5; i++) {
			if (fVolBuf[i] != 0) {
				bPowerUp = true;
				break;
			}
		}
		iHardwareVer = SSUSB_GetHardwareVersion(hDev);
		if (iHardwareVer == 2) {
			iRet = SSUSB_SetPowerVerV20(hDev, fVolBuf, bPowerUp); // v20
		}
		else if (iHardwareVer == 4) {
			iRet = SSUSB_SetPowerVerV20Pro(hDev, fVolBuf, bPowerUp); // V20 pro
		}
		else if (iHardwareVer == 3 || iHardwareVer == 0x0e) {
			iRet = SSUSB_SetPowerVerV20Base(hDev, fVolBuf, bPowerUp); // V20 base
		}
		else if (iHardwareVer == 0x0f) {
			iRet = SSUSB_SetPowerVerPE(hDev, fVolBuf, bPowerUp); // PE Test
		}
		else if (iHardwareVer >= 0x501) {
			// bPowerEnable:true-上电，false-修改电压
			//  check if file ssusbliboption.txt exists and half_avdd is true
			bool          bHalfAvdd = false;
			std::ifstream infile("ssusbliboption.txt");
			if (infile.is_open()) {
				std::string line;
				while (std::getline(infile, line)) {
					// check if line contains half_avdd = 1 (not zero)
					std::regex  re("half_avdd\\s*=\\s*([1-9][0-9]*)");
					std::smatch match;
					if (std::regex_search(line, match, re)) {
						// check if match contains 1 (not zero)
						if (match[1].str() != "0") {
							bHalfAvdd = true;
							break;
						}
					}
				}
				infile.close();
			}

			if (!bHalfAvdd || !bPowerEnable) {
				iRet = SSUSB_SetPowerVerV30(hDev, fVolBuf, bPowerUp, bPowerEnable); // V30
			}
			else {
				fVolBuf[0] /= 2;
				iRet = SSUSB_SetPowerVerV30(hDev, fVolBuf, bPowerUp, bPowerEnable); // V30
				fVolBuf[0] *= 2;
				iRet = SSUSB_SetPowerVerV30(hDev, fVolBuf, bPowerUp, false); // V30
			}
		}
		else {
			iRet = SSUSB_SetPowerVerV10(hDev, fVolBuf, bPowerUp, delaytime); // v10
		}
	}
	else if(PowerMode == VER_0x801_MODE)
	{
		for (int i = 0; i < 7; i++) {
			if (fVolBuf[i] != 0) {
				bPowerUp = true;
				break;
			}
		}
		iRet = SSUSB_SetPowerVerV30(hDev, fVolBuf, bPowerUp, bPowerEnable, PIN_0x801);
	}



   
    return iRet;
}

SSUSB_API int SSUSB_SetPowerValue(HANDLE hDev, float fVolBuf[5])
{
    int iRet = -1;
    if (!hDev)
        return iRet;
    unsigned int iHardwareVer = 0;
    iHardwareVer              = SSUSB_GetHardwareVersion(hDev);

    if (iHardwareVer >= 0x501) {
        iRet = SSUSB_SetPowerValueVerV30(hDev, fVolBuf); // V30
    }

    return iRet;
}

SSUSB_API int SSUSB_SetPowerOnOff(HANDLE hDev, bool bOnOff[4])
{
    int iRet = -1;
    if (!hDev)
        return iRet;
    unsigned int iHardwareVer = 0;
    iHardwareVer              = SSUSB_GetHardwareVersion(hDev);

    if (iHardwareVer >= 0x501) {
        iRet = SSUSB_SetPowerOnOffVerV30(hDev, bOnOff); // V30
    }

    return iRet;
}

SSUSB_API int SSUSB_SetMCLKEx(HANDLE hDev, float fMclk, bool bEnable)
{
    if (!hDev)
        return -1;
    int iRet = -1;
    if (!bEnable) {
        unsigned int uValue = 0;
        SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue);
        uValue &= (~0x10);
        // signal
        SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
        return 0;
    } else {
        unsigned int iHardwareVer = 0;
        iHardwareVer              = SSUSB_GetHardwareVersion(hDev);
        if (iHardwareVer >= 0x501) {
            iRet = SSUSB_SetMCLKVerV30(hDev, fMclk * 1000);
        } else if (iHardwareVer > 3 && iHardwareVer != 0x0e) {
            iRet = SSUSB_SetMCLKVerV20(hDev, fMclk * 1000);
        }
    }
    return iRet;
}

// 发送数据到FPGA的DDR,cpData: 待传输数据。 uDataLen: 传输数据长度（需要是USB_IMGRCV_BUFF_SIZE的整数倍），单位Byte。
// uDDRStartAddr: DDR数据存放起始地址。
SSUSB_API int SSUSB_SendDataToFPGA(HANDLE hDev, uint8_t *const cpData, uint64_t uDataLen, uint32_t uDDRStartAddr, bool bCheckCRC, uint32_t &uRegCRC, uint32_t &uCalCRC)
{
    if ((!hDev) || (!cpData))
        return -1;
    // 发送数据长度需要是USB_IMGRCV_BUFF_SIZE的整数倍
    int      iSendPackageNum = ceil((float)uDataLen / USB_IMGRCV_BUFF_SIZE);
    uint64_t iSendLen        = iSendPackageNum * USB_IMGRCV_BUFF_SIZE;

    uint64_t iSendOffset        = 0;
    int      iData              = 0;
    uint32_t iDDRStartAddr_Real = 0x80000000;
    int      iRet = SSUSB_InitSignalSend(hDev, iSendLen, iDDRStartAddr_Real + uDDRStartAddr); // 初始化FPGA寄存器
    if (iRet < 0)
        return iRet;
    // 传输数据
    for (int i = 0; i < iSendPackageNum; i++) {
        iRet = libusb_bulk_transfer((libusb_device_handle *)hDev, 0x01, cpData + iSendOffset, USB_IMGRCV_BUFF_SIZE,
            &iData, 20000); // 传输时间最多20秒
        if (iRet != 0) {
            // printf("OUT Transfer USB ERROR(%s)!\n", libusb_strerror((libusb_error)nRet)); //返回具体错误类型
            return -1;
        }
        iSendOffset = iSendOffset + USB_IMGRCV_BUFF_SIZE;
    }
	Sleep(100);

    // 判断FPGA是否成功接收数据
    uint32_t val = 0;
    iRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    iRet |= SSUSB_USB2I2CRead(hDev, 0x8056, &val);
    if (iRet != 0 || val != 1) {
        return -1; // 失败
    }
	
	if (bCheckCRC)
	{   
		// 计算CRC是否一致
		int iRetCRC = SSUSB_CalcDataCRC(hDev, cpData, iSendLen, 1, uRegCRC, uCalCRC);
		if (!iRetCRC) {
			return -1;
		}
	}


    iRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    iRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 0);
    return iRet;
}

// 从fpga中读取数据，uDataLen为一帧数据的长度，uDDRStartAddr每次读取数据时，必须更新起始地址
SSUSB_API int SSUSB_GetDataFromFPGA(HANDLE hDev, uint8_t *cpData, uint64_t uDataLen, uint64_t uSensorDataLen,
    uint32_t uDDRStartAddr, bool bCheckCRC, uint32_t &uRegCRC, uint32_t &uCalCRC)
{
    if ((!hDev) || (!cpData))
        return -1;
    // int iSendPackageNum = ceil((float)uSensorDataLen / USB_IMGRCV_BUFF_SIZE);
    // uint64_t iGetLen = iSendPackageNum * USB_IMGRCV_BUFF_SIZE;
    int           iImgSize = 0;
    FrameInfoV0_t info;
    int           iRet               = 0;
    uint32_t      iDDRStartAddr_Real = 0x80000000;
    iRet |= SSUSB_InitSignalGet(hDev, uSensorDataLen, iDDRStartAddr_Real + uDDRStartAddr);
    iRet |= SSUSB_FetchFrame(hDev, cpData, uDataLen, &iImgSize, &info, 20000); // 获取数据

	Sleep(100);
    uint32_t val = 0;
    iRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    iRet |= SSUSB_USB2I2CRead(hDev, 0x8049, &val); // FPGA发送数据成功后，拉高0x8049
    if (iRet != 0 || val != 1) {
        return -1; // 失败
    }
	if (bCheckCRC)
	{	
		int iRetCRC = SSUSB_CalcDataCRC(hDev, cpData, uDataLen, 0, uRegCRC, uCalCRC);
		if (!iRetCRC) {
			return -1;
		}
	}


    iRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    iRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 0);

    return iRet;
}

#define HIGHSPEED_SPI_RESERVED         0b11110100
#define HIGHSPEED_SPI_INC_MODE         0b1000
#define HIGHSPEED_SPI_SIZE_O           0b10
#define HIGHSPEED_SPI_READ             0b1
#define HIGHSPEED_SPI_DUMMY_TIMING_REG 0x80ad
#define HIGHSPEED_SPI_MODE_REG         0x80ae
#define HIGHSPEED_SPI_DELAY_REG        0x808b

void SetHighSpeedSpiModeAndSpeed(HANDLE hDev, int iSpiMode, float fSpeedMhz)
{
    static HANDLE lastHandle  = hDev;
    static int    lastSpiMode = iSpiMode;
    static float  lastSpeed   = fSpeedMhz;
    static bool   firstTime   = true;
    if (lastHandle == hDev && lastSpiMode == iSpiMode && lastSpeed == fSpeedMhz && !firstTime) {
        return;
    }
    lastHandle         = hDev;
    lastSpiMode        = iSpiMode;
    lastSpeed          = fSpeedMhz;
    firstTime          = false;
    uint32_t uTmpValue = (((iSpiMode & SPI_CPOL) == SPI_CPOL) << 0) | // spi cpol
        (((iSpiMode & SPI_CPHA) == SPI_CPHA) << 1);                   // spi cpha
    SSUSB_WriteFPGAReg(hDev, HIGHSPEED_SPI_MODE_REG, uTmpValue);
    SSUSB_SetM1VersionFpgaClock(hDev, 0, fSpeedMhz * 2);
}

SSUSB_API int SSUSB_HighSpeedFPGA2SPIRead(HANDLE hDev, int iSpiMode, float fSpeedMhz, uint32_t uAddress, uint8_t *pVal,
                                          uint32_t uAddrLength, uint32_t uValueLength, bool isIncremental,
                                          uint8_t uDummyTiming)
{
    // set clock
    SetHighSpeedSpiModeAndSpeed(hDev, iSpiMode, fSpeedMhz);
    static bool    firstTime       = true;
    static uint8_t lastDummyTiming = 8;
    static float   lastSpeed       = fSpeedMhz;
    const std::vector<float> VEC_MAX_SPEED{ 25, 45, 60, 80, 100, 110 };
    if (firstTime || lastDummyTiming != uDummyTiming || lastSpeed != fSpeedMhz) {
        firstTime       = false;
        lastDummyTiming = uDummyTiming;
        lastSpeed       = fSpeedMhz;
        SSUSB_WriteFPGAReg(hDev, HIGHSPEED_SPI_DUMMY_TIMING_REG, uDummyTiming);
        int iDelay = 0;
        for (int i = 0; i < VEC_MAX_SPEED.size(); i++) {
            if (fSpeedMhz <= VEC_MAX_SPEED[i]) {
                iDelay = i;
                break;
            }
        }
        SSUSB_WriteFPGAReg(hDev, HIGHSPEED_SPI_DELAY_REG, iDelay);
    }

    const uint32_t MAX_READ_SIZE = MAX_PACKET_PAYLOAD_SIZE - MAX_PACKET_PAYLOAD_SIZE % 4;
    // config fx3 read delay time
    uint32_t delayTimeUs = std::ceil((min(uValueLength, MAX_READ_SIZE) + 20 + uDummyTiming) * 8 / fSpeedMhz) + 50;
    if (CMDWrite(hDev, CMD_USB2SPICONFIG, 0, reinterpret_cast<char *>(&delayTimeUs), 4) != 4) {
        return -2;
    }

    uint32_t uCommand      = 0;
    uint32_t uHeaderLength = min(uValueLength, 4 - uAddress % 4);
    if (uHeaderLength == 4) {
        uHeaderLength = 0;
    }
    uint32_t             uMiddleLength = (uValueLength - uHeaderLength) - (uValueLength - uHeaderLength) % 4;
    uint32_t             uTailLength   = uValueLength - uHeaderLength - uMiddleLength;
    std::vector<uint8_t> vData(uValueLength, 0);
    if (uHeaderLength != 0) {
        if (isIncremental)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE | HIGHSPEED_SPI_READ;
        }
        else
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_READ;
        }
        if (uHeaderLength != CMDRead(hDev, uCommand, uAddress, reinterpret_cast<void *>(vData.data()), uHeaderLength))
            return -1;
    }
    if (uMiddleLength != 0) {
        uint32_t uSentLength  = 0;
        uint32_t uSentAddress = uAddress + uHeaderLength;
        uint32_t uReadLength;
        uReadLength = min(uMiddleLength, MAX_READ_SIZE);
        if (isIncremental) {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE | HIGHSPEED_SPI_SIZE_O |
                       HIGHSPEED_SPI_READ;
        } else {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_SIZE_O | HIGHSPEED_SPI_READ;
        }
        while (uSentLength < uMiddleLength)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE | HIGHSPEED_SPI_SIZE_O |
                       HIGHSPEED_SPI_READ;
            if (uReadLength != CMDRead(hDev, uCommand, uSentAddress,
                                       reinterpret_cast<void *>(vData.data() + uHeaderLength + uSentLength),
                                       uReadLength))
                return -1;
            uSentLength += uReadLength;
            uSentAddress += uReadLength;
            uReadLength = min(uMiddleLength - uSentLength, MAX_READ_SIZE);
        }
    }
    if (uTailLength != 0) {
        if (isIncremental)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE | HIGHSPEED_SPI_READ;
        }
        else
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_READ;
        }
        if (uTailLength
            != CMDRead(hDev, uCommand, uAddress + uHeaderLength + uMiddleLength,
                reinterpret_cast<void *>(vData.data() + uHeaderLength + uMiddleLength), uTailLength))
            return -1;
    }
    std::copy(vData.begin(), vData.end(), pVal);
    return 0;
}

SSUSB_API int SSUSB_HighSpeedFPGA2SPIWrite(HANDLE hDev, int iSpiMode, float fSpeedMhz, uint32_t uAddress,
                                           const uint8_t *pVal, uint32_t uAddrLength, uint32_t uValueLength,
                                           bool isIncremental)
{
    // set clock
    SetHighSpeedSpiModeAndSpeed(hDev, iSpiMode, fSpeedMhz);

    // config fx3 write delay time
    const uint32_t MAX_SEND_SIZE = MAX_PACKET_PAYLOAD_SIZE - MAX_PACKET_PAYLOAD_SIZE % 4;
    uint32_t       delayTimeUs   = std::ceil((min(uValueLength, MAX_SEND_SIZE) + 20) * 8 / fSpeedMhz) + 50;
    if (CMDWrite(hDev, CMD_USB2SPICONFIG, 0, reinterpret_cast<char *>(&delayTimeUs), 4) != 4) {
        return -2;
    }

    uint32_t uCommand      = 0;
    uint32_t uHeaderLength = min(uValueLength, 4 - uAddress % 4);
    if (uHeaderLength == 4) {
        uHeaderLength = 0;
    }
    uint32_t             uMiddleLength = (uValueLength - uHeaderLength) - (uValueLength - uHeaderLength) % 4;
    uint32_t             uTailLength   = uValueLength - uHeaderLength - uMiddleLength;
    std::vector<uint8_t> vData(pVal, pVal + uValueLength);
    if (uHeaderLength != 0)
    {
        if (isIncremental)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE;
        }
        else
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED;
        }
        if (uHeaderLength != CMDWrite(hDev, uCommand, uAddress, reinterpret_cast<void *>(vData.data()), uHeaderLength))
            return -1;
    }
    if (uMiddleLength != 0)
    {
        uint32_t uSentLength = 0;
        uint32_t uSentAddress = uAddress + uHeaderLength;
        uint32_t uSendLength;
        uSendLength = min(uMiddleLength, MAX_SEND_SIZE);
        if (isIncremental)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE | HIGHSPEED_SPI_SIZE_O;
        }
        else
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_SIZE_O;
        }
        while (uSentLength < uMiddleLength)
        {
            if (uSendLength != CMDWrite(hDev, uCommand, uSentAddress,
                                        reinterpret_cast<void *>(vData.data() + uHeaderLength + uSentLength),
                                        uSendLength))
                return -1;
            uSentLength += uSendLength;
            uSentAddress += uSendLength;
            uSendLength = min(uMiddleLength - uSentLength, MAX_SEND_SIZE);
        }
    }
    if (uTailLength != 0) {
        if (isIncremental)
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED | HIGHSPEED_SPI_INC_MODE;
        }
        else
        {
            uCommand = CMD_HISPEEDSPI | HIGHSPEED_SPI_RESERVED;
        }
        if (uTailLength
            != CMDWrite(hDev, uCommand, uAddress + uHeaderLength + uMiddleLength,
                reinterpret_cast<void *>(vData.data() + uHeaderLength + uMiddleLength), uTailLength))
            return -1;
    }
    return 0;
}

SSUSB_API int SSUSB_SetDisplayChannel(HANDLE hDev, uint32_t uChannel)
{
    SSUSB_WriteFPGAReg(hDev, 0x80b0, uChannel);
    return 0;
}

struct StHdmiResolutionAndFrameRate
{
    uint32_t uWidth;
    uint32_t uHeight;
    uint32_t uFrameRate;
};

struct StHdmiConfigReg
{
    uint32_t uHdmiActiveWidthLow;
    uint32_t uHdmiActiveWidthHigh;
    uint32_t uHdmiActiveHeightLow;
    uint32_t uHdmiActiveHeightHigh;
    uint32_t uHdmiTotalWidthLow;
    uint32_t uHdmiTotalWidthHigh;
    uint32_t uHdmiTotalHeightLow;
    uint32_t uHdmiTotalHeightHigh;
    uint32_t uHdmiHsyncLow;
    uint32_t uHdmiHsyncHigh;
    uint32_t uHdmiVsyncLow;
    uint32_t uHdmiVsyncHigh;
    uint32_t uHdmiHVPol;
    uint32_t uHdmiHFPLow;
    uint32_t uHdmiHFPHigh;
    uint32_t uHdmiHBPLow;
    uint32_t uHdmiHBPHigh;
    uint32_t uHdmiVFPLow;
    uint32_t uHdmiVFPHigh;
    uint32_t uHdmiVBPLow;
    uint32_t uHdmiVBPHigh;
};

struct StHdmiConfig
{
    uint32_t uHdmiActiveWidth;
    uint32_t uHdmiActiveHeight;
    uint32_t uHdmiTotalWidth;
    uint32_t uHdmiTotalHeight;
    uint32_t uHdmiHsync;
    uint32_t uHdmiVsync;
    uint32_t uHdmiHVPol;
    uint32_t uHdmiHFP;
    uint32_t uHdmiHBP;
    uint32_t uHdmiVFP;
    uint32_t uHdmiVBP;
};

const std::map<uint32_t, StHdmiConfigReg> g_mapHdmiConfigReg = {
    { 1,
        { 0x80c4, 0x80c5, 0x80c6, 0x80c7, 0x80c8, 0x80c9, 0x80ca, 0x80cb, 0x80cc, 0x80cd, 0x80ce, 0x80cf, 0x80d0,
            0x80d1, 0x80d2, 0x80d3, 0x80d4, 0x80d5, 0x80d6, 0x80d7, 0x80d8 } },
    { 2,
        { 0x80d9, 0x80da, 0x80db, 0x80dc, 0x80dd, 0x80de, 0x80df, 0x80e0, 0x80e1, 0x80e2, 0x80e3, 0x80e4, 0x80e5,
            0x80e6, 0x80e7, 0x80e8, 0x80e9, 0x80ea, 0x80eb, 0x80ec, 0x80ed } },
};

SSUSB_API int SSUSB_SetHdmiResolutionAndFrameRate(HANDLE hDev, uint32_t uHdmiChannel, uint32_t uWidth, uint32_t uHeight,
    uint32_t uFrameRate)
{
    static const std::vector<std::pair<StHdmiResolutionAndFrameRate, StHdmiConfig>> vecDefaultConfig = {
        { { 3840, 2160, 60 }, { 3840, 2160, 4400, 2250, 88, 10, 3, 176, 296, 72, 8 } },
        { { 3840, 2160, 30 }, { 3840, 2160, 4400, 2250, 88, 10, 3, 176, 296, 72, 8 } },
        { { 1920, 1080, 60 }, { 1920, 1080, 2200, 1125, 44, 5, 3, 88, 148, 36, 4 } },
        { { 1920, 1080, 30 }, { 1920, 1080, 2200, 1125, 44, 5, 3, 88, 148, 36, 4 } },
    };
    if (g_mapHdmiConfigReg.find(uHdmiChannel) == g_mapHdmiConfigReg.end()) {
        return -1;
    }
    auto                         stHdmiConfigReg              = g_mapHdmiConfigReg.at(uHdmiChannel);
    StHdmiResolutionAndFrameRate stHdmiResolutionAndFrameRate = { uWidth, uHeight, uFrameRate };
    bool                         bFound                       = false;
    for (int i = 0; i < vecDefaultConfig.size(); i++) {
        if (vecDefaultConfig[i].first.uWidth == uWidth && vecDefaultConfig[i].first.uHeight == uHeight
            && vecDefaultConfig[i].first.uFrameRate == uFrameRate) {
            auto &stHdmiConfig = vecDefaultConfig[i].second;
            SSUSB_SetHdmiImageParam(hDev, uHdmiChannel, stHdmiConfig.uHdmiActiveWidth, stHdmiConfig.uHdmiActiveHeight,
                stHdmiConfig.uHdmiTotalWidth, stHdmiConfig.uHdmiTotalHeight, stHdmiConfig.uHdmiHsync,
                stHdmiConfig.uHdmiVsync, stHdmiConfig.uHdmiHVPol, stHdmiConfig.uHdmiHFP, stHdmiConfig.uHdmiHBP,
                stHdmiConfig.uHdmiVFP, stHdmiConfig.uHdmiVBP);
            float fClock = stHdmiConfig.uHdmiTotalHeight * stHdmiConfig.uHdmiTotalWidth * uFrameRate / 1000000.0 / 8;
            SSUSB_SetHdmiClock(hDev, uHdmiChannel, fClock);
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        uint32_t uHdmiActiveWidth  = uWidth;
        uint32_t uHdmiActiveHeight = uHeight;
        SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveWidthLow, uHdmiActiveWidth & 0xff);
        SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveWidthHigh, (uHdmiActiveWidth >> 8) & 0xff);
        SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveHeightLow, uHdmiActiveHeight & 0xff);
        SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveHeightHigh, (uHdmiActiveHeight >> 8) & 0xff);
    }
    uint32_t uTmpValue = 0;
    SSUSB_ReadFPGAReg(hDev, 0x8000, &uTmpValue);
    uTmpValue &= ~(0x1);
    SSUSB_WriteFPGAReg(hDev, 0x8000, uTmpValue);
    Sleep(1);
    uTmpValue |= 0x1;
    SSUSB_WriteFPGAReg(hDev, 0x8000, uTmpValue);
    return 0;
}

SSUSB_API int SSUSB_SetHdmiClock(HANDLE hDev, uint32_t uHdmiChannel, float fClockMhz)
{
    return SSUSB_SetM1VersionFpgaClock(hDev, uHdmiChannel, fClockMhz);
}

SSUSB_API int SSUSB_SetHdmiImageParam(HANDLE hDev, uint32_t uHdmiChannel, uint32_t uHdmiActiveWidth,
    uint32_t uHdmiActiveHeight, uint32_t uHdmiTotalWidth, uint32_t uHdmiTotalHeight, uint32_t uHdmiHsync,
    uint32_t uHdmiVsync, uint32_t uHdmiHVPol, uint32_t uHdmiHFP, uint32_t uHdmiHBP, uint32_t uHdmiVFP,
    uint32_t uHdmiVBP)
{
    if (g_mapHdmiConfigReg.find(uHdmiChannel) == g_mapHdmiConfigReg.end()) {
        return -1;
    }
    auto stHdmiConfigReg = g_mapHdmiConfigReg.at(uHdmiChannel);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveWidthLow, uHdmiActiveWidth & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveWidthHigh, (uHdmiActiveWidth >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveHeightLow, uHdmiActiveHeight & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiActiveHeightHigh, (uHdmiActiveHeight >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiTotalWidthLow, uHdmiTotalWidth & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiTotalWidthHigh, (uHdmiTotalWidth >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiTotalHeightLow, uHdmiTotalHeight & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiTotalHeightHigh, (uHdmiTotalHeight >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHsyncLow, uHdmiHsync & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHsyncHigh, (uHdmiHsync >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVsyncLow, uHdmiVsync & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVsyncHigh, (uHdmiVsync >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHVPol, uHdmiHVPol);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHFPLow, uHdmiHFP & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHFPHigh, (uHdmiHFP >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHBPLow, uHdmiHBP & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiHBPHigh, (uHdmiHBP >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVFPLow, uHdmiVFP & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVFPHigh, (uHdmiVFP >> 8) & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVBPLow, uHdmiVBP & 0xff);
    SSUSB_WriteFPGAReg(hDev, stHdmiConfigReg.uHdmiVBPHigh, (uHdmiVBP >> 8) & 0xff);
    return 0;
}
