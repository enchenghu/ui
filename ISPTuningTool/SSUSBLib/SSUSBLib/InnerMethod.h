#pragma once
#include"USBComm.h"
#include <chrono>

enum COMMUNICATION_MODE : int
{
	MODE_I2C = 0,
	MODE_I3C,
	MODE_SPI
};

// 上电引脚数量
enum PIN_COUNT
{
	PIN_BASIC = 4,//AVDD DOVDD DVDD AFVCC 
	PIN_0x801 = 7,//AVDD DOVDD DVDD AFVCC AVDD2CD DOVDD2CD DVDD2CD
};

static uint32_t CMDWrite(HANDLE hDev, uint32_t Command, uint32_t Addr, void *pData, uint32_t DataSize);

static uint32_t CMDRead(HANDLE hDev, uint32_t Command, uint32_t Addr, void *pBuff, uint32_t BuffSize, void *pSend = nullptr, uint32_t SendSize = 0);

static int FPGAWriteReg(HANDLE hDev, uint32_t addr, uint8_t value);

static int FPGAReadReg(HANDLE hDev, uint32_t addr, uint8_t* pValue);

static int GetBoardType(HANDLE hDev);

static int FPGAWriteReg(HANDLE hDev, uint32_t addr, uint8_t value);

static int FPGAReadReg(HANDLE hDev, uint32_t addr, uint8_t* pValue);

int SSUSB_SetPowerVerV20(HANDLE hDev, float fVolBuf[5], bool bPowerUp);

int SSUSB_SetPowerVerV20Pro(HANDLE hDev, float fVolBuf[5], bool bPowerUp);

int SSUSB_SetPowerVerV20Base(HANDLE hDev, float fVolBuf[5], bool bPowerUp);

int SSUSB_SetPowerVerV30(HANDLE hDev, float fVolBuf[7], bool bPowerUp, bool bPowerEnable, int pinCount= PIN_BASIC);

int SSUSB_SetPowerValueVerV30(HANDLE hDev, float fVolBuf[5]);

int SSUSB_SetPowerOnOffVerV30(HANDLE hDev, bool bOnOff[4]);

int SSUSB_SetPowerVerPE(HANDLE hDev, float fVolBuf[5], bool bPowerUp);

int SSUSB_SetPowerVerV10(HANDLE hDev, float fVolBuf[5], bool bPowerUp, uint16_t * delaytime);

void SSUSB_GetFPGAClock(int iFrequence, int & iSSUSBFrequence);

int SSUSB_SetMCLKVerV30(HANDLE hDev, int iFrequence);

int SSUSB_SetMCLKVerV20(HANDLE hDev, int iFrequence);

int SSUSB_SwitchCommunicationMode(HANDLE hDev, int iMode);

static int GetBoardType(HANDLE hDev)
{
	uint8_t value = 0;
	int nRet = 0;

	nRet = FPGAReadReg(hDev, 0x8000, &value);
	if (nRet != 0)
	{
		return nRet;
	}

	value = (value & 0x30) >> 4;

	return value;
}

static int FPGAWriteReg(HANDLE hDev, uint32_t addr, uint8_t value)
{
	UINT8 pBuf[4];

	if (!hDev)
	{
		return -1;
	}

	*pBuf = value;

	if (CMDWrite(hDev, CMD_WRITEFPGAREG, addr, pBuf, sizeof(uint8_t)) != sizeof(uint8_t))
	{
		return -2;
	}

	return 0;
}

static int FPGAReadReg(HANDLE hDev, uint32_t addr, uint8_t* pValue)
{
	if ((!hDev) || (!pValue))
	{
		return -1;
	}

	if (CMDRead(hDev, CMD_READFPGAREG, addr, pValue, sizeof(uint8_t)) != sizeof(uint8_t))
	{
		return -2;
	}

	return 0;
}

template <typename T> void busySleep(T duration)
{
    auto end = std::chrono::high_resolution_clock::now() + duration;
    while (std::chrono::high_resolution_clock::now() < end)
        ;
    return;
}

static uint32_t CMDWrite(HANDLE hDev, uint32_t Command, uint32_t Addr, void *pData, uint32_t DataSize)
{
	int  nRet;
	uint32_t BytesSend, Size, ErrCount;

	PacketCmd_t cmdSend = { 0 }, cmdRecv = { 0 };
	cmdSend.Version = 1;
	cmdSend.Command = Command;

	BytesSend = 0;
	ErrCount = 0;

	while (BytesSend < DataSize && ErrCount < MAX_ATTEMPT_COUNT)
	{
		cmdSend.Address = Addr + BytesSend;
		cmdSend.DataSize = (uint16_t)min(static_cast<uint32_t>(MAX_PACKET_PAYLOAD_SIZE), (DataSize - BytesSend));
		//memcpy(cmdSend.pData, (char*)pData + BytesSend, MAX_PACKET_PAYLOAD_SIZE);
		memcpy_s(cmdSend.pData, cmdSend.DataSize, (char*)pData + BytesSend, cmdSend.DataSize);
		Size = sizeof(cmdRecv);
		nRet = usb_send_cmd(hDev, &cmdSend, sizeof(cmdSend), &cmdRecv, &Size);

		if ((nRet != 0) || (cmdRecv.DataSize == 0))
		{
			++ErrCount;
			return ErrCount;
		}
		BytesSend += cmdRecv.DataSize;
	}
    busySleep(std::chrono::microseconds(25));

    return BytesSend;
}

static uint32_t CMDRead(HANDLE hDev, uint32_t Command, uint32_t Addr, void *pBuff, uint32_t BuffSize, void *pSend, uint32_t SendSize)
{
	int nRet;
	uint32_t BytesRecv, Size, ErrCount;

	PacketCmd_t cmdSend = { 0 }, cmdRecv = { 0 };
	cmdSend.Version = 1;
	cmdSend.Command = Command;

	BytesRecv = 0;
	ErrCount = 0;
	while (BytesRecv < BuffSize && ErrCount < MAX_ATTEMPT_COUNT)
	{
		cmdSend.Address = Addr + BytesRecv;
		cmdSend.DataSize = (UINT16)min(static_cast<uint32_t>(MAX_PACKET_PAYLOAD_SIZE), (BuffSize - BytesRecv));
		if (pSend != nullptr)
		{
			memcpy_s(cmdSend.pData, SendSize, pSend, SendSize);
		}
		Size = sizeof(cmdRecv);
		nRet = usb_send_cmd(hDev, &cmdSend, sizeof(cmdSend), &cmdRecv, &Size);
		if (nRet != 0)
		{
			++ErrCount;
			continue;
		}
		//memcpy((char*)pBuff + BytesRecv, cmdRecv.pData, cmdRecv.DataSize);
		memcpy_s((char*)pBuff + BytesRecv, cmdRecv.DataSize, cmdRecv.pData, cmdRecv.DataSize);
		BytesRecv += cmdRecv.DataSize;
		if (cmdRecv.DataSize != cmdSend.DataSize)
		{
			break;
		}
	}
    busySleep(std::chrono::microseconds(25));

    return BytesRecv;
}

static void InvertUint16(unsigned short *DesBuf, unsigned short *SrcBuf)
{
	int i;
	unsigned short temp = 0;

	for (i = 0; i < 16; i++)
	{
		if (SrcBuf[0] & (1 << i))
		{
			temp |= 1 << (15 - i);
		}
	}
	DesBuf[0] = temp;
}

static void InvertUint8(unsigned char *DesBuf, unsigned char *SrcBuf)
{
	int i;
	unsigned char temp = 0;

	for (i = 0; i < 8; i++)
	{
		if (SrcBuf[0] & (1 << i))
		{
			temp |= 1 << (7 - i);
		}
	}
	DesBuf[0] = temp;
}

static unsigned short CRC16_CCITT(unsigned char const *puchMsg, unsigned int usDataLen)
{
	unsigned short wCRCin = 0xffff;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);

		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
			{
				wCRCin = (wCRCin << 1) ^ wCPoly;
			}
			else
			{
				wCRCin = wCRCin << 1;
			}
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin);
}
int SSUSB_CalcDataCRC(HANDLE hDev, uint8_t* const cpData, uint64_t uDataLen, bool bIsSendFlag, uint32_t &uRegCRC, uint32_t &uCalCRC);
int SSUSB_InitSignalSend(HANDLE hDev, uint64_t uDataLen, uint32_t uDDRStartAddr);
int SSUSB_InitSignalGet(HANDLE hDev, uint64_t uDataLen, uint32_t uDDRStartAddr);

// type 0: SPI, 1: HDMI A, 2: HDMI B 
int SSUSB_SetM1VersionFpgaClock(HANDLE hDev, int iType, float fFrequence);