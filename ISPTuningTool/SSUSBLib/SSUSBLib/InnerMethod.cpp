#include "stdafx.h"

#include "InnerMethod.h"
#include "COscillatorMap.h"
#include "FpgaMmcmFreqGenerator.h"
#include "SSUSBLib.h"
#include "USBComm.h"
#include <cmath>
#include <map>
#include <stdio.h>
#include <stdlib.h>


enum POWER
{
    AVDD = 0,
    DOVDD,
    DVDD,
    AFVCC,
	AVDD_2CD,//DS325引脚-对应Sensor配置avdd2
	DOVDD_2CD,//DS325引脚-对应Sensor配置vpp
	DVDD_2CD,
    VPP
};

// 设置MCLK用地址列表
const unsigned int m_iaryAddressList[34] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x10, 0x11, 0x15, 0x16, 0x17, 0x19, 0x1A,
                                            0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
                                            0x27, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x38, 0x48};
COscillatorMap m_cOsciMap;

int SSUSB_SetPowerVerV20(HANDLE hDev, float fVolBuf[5], bool bPowerUp)
{
    // 2.0旧板上电/下电
    int iRet = -1;
    if (bPowerUp)
    {
        for (int i = 0; i < 4; i++)
        {
            float fR1 = 60 * fVolBuf[i] - 30;
            float fR2 = 0;
            int iValue1 = 0x7ff;
            int iValue2 = 0x400;
            if ((int)fR1 >= 100)
            {
                fR2 = fR1 - 100;
                iValue2 = (int)(1024 * fR2 / 100) | 0x400;
                iValue1 = 0x7ff;
            }
            else
            {
                iValue1 = (int)(1024 * fR1 / 100) | 0x400;
                iValue2 = 0x400;
            }
            // enable
            iRet = SSUSB_USB2I2CCfg(hDev, 0x77 >> 1, 1, 1);
            iRet = SSUSB_USB2I2CWrite(hDev, 1 << i, 1 << i);
            // R1
            iRet = SSUSB_USB2I2CCfg(hDev, 0x2C >> 1, 1, 1);
            iRet = SSUSB_USB2I2CWrite(hDev, 0x1C, 0x02);
            int temH = (iValue1 >> 8) & 0xff;
            int temL = iValue1 & 0xff;
            iRet = SSUSB_USB2I2CWrite(hDev, temH, temL);
            // R2
            iRet = SSUSB_USB2I2CCfg(hDev, 0x2F >> 1, 1, 1);
            iRet = SSUSB_USB2I2CWrite(hDev, 0x1C, 0x02);
            temH = (iValue2 >> 8) & 0xff;
            temL = iValue2 & 0xff;
            iRet = SSUSB_USB2I2CWrite(hDev, temH, temL);
            if (iRet)
                return iRet;
        }
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, 0x0f);
    }
    else
    {
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, 0);
    }
    return iRet;
}
int SSUSB_SetPowerVerV20Pro(HANDLE hDev, float fVolBuf[5], bool bPowerUp)
{
    // 2.0新板Pro版本上电/下电
    int iRet = -1;
    if (bPowerUp)
    {
        unsigned char byBuf[28] = {0};
        memset(byBuf, 0, 28);
        unsigned char byH = 0, byL = 0;
        for (int i = 0; i < 4; i++)
        {
            byH = 0;
            byL = 0;
            if (fVolBuf[i] < 0.6)
            {
                fVolBuf[i] = 0;
                if (i < 3)
                {
                    printf("Power out of range(0.6V ~ 3.9V)");
                    return -1000; // 区分错误码
                }
            }
            else if (fVolBuf[i] > 3.9)
            {
                fVolBuf[i] = 3.9;
                if (i < 3)
                {
                    printf("Power out of range(0.6V ~ 3.9V)");
                    return -1000;
                }
            }
            byBuf[i * 2 + 0] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff00) >> 8;
            byBuf[i * 2 + 1] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff);
        }
        iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
    }
    else
    {
        unsigned char byBuf[8] = {0};
        iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
    }
    return iRet;
}
int SSUSB_SetPowerVerV20Base(HANDLE hDev, float fVolBuf[5], bool bPowerUp)
{
    // 2.0新板Base版本上电/下电
    int iRet = -1;
    if (bPowerUp)
    {
        unsigned char byBuf[8] = {0};
        memset(byBuf, 0, 8);
        unsigned char byH = 0, byL = 0;
        for (int i = 0; i < 4; i++)
        {
            byH = 0;
            byL = 0;
            byBuf[i * 2 + 0] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff00) >> 8;
            byBuf[i * 2 + 1] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff);
        }
        iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
    }
    else
    {
        unsigned char byBuf[8] = {0};
        iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
    }
    return iRet;
}
int SSUSB_SetPowerVerV30(HANDLE hDev, float fVolBuf[7], bool bPowerUp, bool bPowerEnable, int pinCount)
{
    // v30
    int iRet = -1;
    uint32_t val = 0;

	if (pinCount == PIN_BASIC)
	{
		if (bPowerUp)
		{
			unsigned char byBuf[28] = { 0 };
			memset(byBuf, 0, 28);
			unsigned char byH = 0, byL = 0;
			for (int i = 0; i < 4; i++)
			{
				byH = 0;
				byL = 0;

				// DVDD 0.6V ~ 2.0V
				if (i == DVDD)
				{
					if (fVolBuf[i] < 0.6)
					{
						fVolBuf[i] = 0;
						printf("Power out of range(0.6V ~ 2.0V)");
						return -1000;
					}
					else if (fVolBuf[i] > 2.0)
					{
						fVolBuf[i] = 2.0;
						printf("Power out of range(0.6V ~ 2.0V)");
						return -1000;
					}
				}
				else
				{
					if (fVolBuf[i] < 0.6)
					{
						fVolBuf[i] = 0;
						if (i < 2)
						{
							printf("Power out of range(0.6V ~ 3.9V)");
							return -1000;
						}
					}
					else if (fVolBuf[i] > 3.9)
					{
						fVolBuf[i] = 3.9;
						if (i < 2)
						{
							printf("Power out of range(0.6V ~ 3.9V)");
							return -1000;
						}
					}
				}
				byBuf[i * 2 + 0] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff00) >> 8;
				byBuf[i * 2 + 1] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff);
			}
			iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
			if (iRet)
				return iRet;
			if (bPowerEnable) // 使能
			{
				iRet = SSUSB_ReadFPGAReg(hDev, 0x8052, &val);
				val = val | 0x1f;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // 上电
				val &= 0xfe;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dovdd mos on
				Sleep(10);
				val &= 0xfa;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dvdd mos on
				Sleep(10);
				val &= 0xf2;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // avdd mos on
				Sleep(10);
				val &= 0xf0;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // afvcc mos on
				Sleep(20);
			}
		}
		else // 下电
		{
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8051, 0xDF);
			iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &val);
			val &= (~0x04);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // afvcc LDO OFF
			val &= (~0x08);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // avdd LDO OFF
			Sleep(10);
			val &= (~0x02);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // dvdd LDO OFF
			Sleep(10);
			val &= (~0x01);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // dovdd LDO OFF

			// 关闭8052 MOS
			iRet = SSUSB_ReadFPGAReg(hDev, 0x8052, &val);
			val |= 0x0f;
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dovdd MOS OFF+
			if (iRet)
				return iRet;

			// FX3 电压配置源码 并Reset DAC
			unsigned char byBuf[8] = { 0 };
			iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);
		}
	}
	else if (pinCount == PIN_0x801)
	{//DS325TD
		if (bPowerUp)
		{
			unsigned char byBuf[28] = { 0 };
			memset(byBuf, 0, 28);
			unsigned char byH = 0, byL = 0;
			for (int i = 0; i < 7; i++)
			{
				byH = 0;
				byL = 0;

				// DVDD 0.6V ~ 2.0V
				if (i == DVDD|| i == DVDD_2CD)
				{
					if (fVolBuf[i] < 0.6)
					{
						fVolBuf[i] = 0;
					}
					else if (fVolBuf[i] > 2.0)
					{
						fVolBuf[i] = 2.0;
						printf("Power out of range(0.6V ~ 2.0V)");
						return -1000;
					}
				}
				else
				{
					if (fVolBuf[i] < 0.6)
					{
						fVolBuf[i] = 0;
					}
					else if (fVolBuf[i] > 3.9)
					{
						fVolBuf[i] = 3.9;
						if (i < 2)
						{
							printf("Power out of range(0.6V ~ 3.9V)");
							return -1000;
						}
					}
				}
				byBuf[i * 2 + 0] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff00) >> 8;
				byBuf[i * 2 + 1] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff);
			}
			iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf, VER_0x801_MODE);
			if (iRet)
				return iRet;
			if (bPowerEnable) // 使能
			{
				iRet = SSUSB_ReadFPGAReg(hDev, 0x8052, &val);
				val = val | 0xff;
				iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // 上电
			
				if (fVolBuf[0] > 0)
				{
					val &= 0xf2;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // avdd mos on

				}
				Sleep(10);
				if (fVolBuf[1] > 0)
				{
					val &= 0xfe;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dovdd mos on
				}
				Sleep(10);
				if (fVolBuf[2] > 0)
				{
					val &= 0xfa;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dvdd mos on
				}
				Sleep(10);
				if (fVolBuf[3] > 0)
				{
					val &= 0xf0;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // afvcc mos on
				}
				Sleep(20);
				if (fVolBuf[4] > 0)
				{
					val &= 0x10;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // avdd2cd mos on
				}
				Sleep(20);
				if (fVolBuf[5] > 0)
				{
					val &= 0x90;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dovdd2cd mos on
				}
				Sleep(20);
				if (fVolBuf[6] > 0)
				{
					val &= 0xd0;
					iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dvdd2cd mos on
				}
				Sleep(20);
			}
		}
		else // 下电
		{
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8051, 0xDF);
			iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &val);
			val &= (~0x04);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // afvcc LDO OFF
			val &= (~0x08);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // avdd LDO OFF
			Sleep(10);
			val &= (~0x02);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // dvdd LDO OFF
			Sleep(10);
			val &= (~0x01);
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, val); // dovdd LDO OFF

			// 关闭8052 MOS
			iRet = SSUSB_ReadFPGAReg(hDev, 0x8052, &val);
			val |= 0xef;
			iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); //  MOS OFF+
			if (iRet)
				return iRet;

			// FX3 电压配置源码 并Reset DAC
			unsigned char byBuf[14] = { 0 };
			iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf, VER_0x801_MODE);
		}


	}


    
    return iRet;
}

int SSUSB_SetPowerValueVerV30(HANDLE hDev, float fVolBuf[5])
{
    // v30
    int iRet = -1;
    unsigned char byBuf[28] = {0};
    memset(byBuf, 0, 28);
    unsigned char byH = 0, byL = 0;
    for (int i = 0; i < 4; i++)
    {
        byH = 0;
        byL = 0;

        // DVDD 0.6V ~ 2.0V
        if (i == DVDD)
        {
            if (fVolBuf[i] < 0.6)
            {
                fVolBuf[i] = 0;
                printf("Power out of range(0.6V ~ 2.0V)");
                return -1000;
            }
            else if (fVolBuf[i] > 2.0)
            {
                fVolBuf[i] = 2.0;
                printf("Power out of range(0.6V ~ 2.0V)");
                return -1000;
            }
        }
        else
        {
            if (fVolBuf[i] < 0.6)
            {
                fVolBuf[i] = 0;
                if (i < 2)
                {
                    printf("Power out of range(0.6V ~ 3.9V)");
                    return -1000;
                }
            }
            else if (fVolBuf[i] > 3.9)
            {
                fVolBuf[i] = 3.9;
                if (i < 2)
                {
                    printf("Power out of range(0.6V ~ 3.9V)");
                    return -1000;
                }
            }
        }
        byBuf[i * 2 + 0] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff00) >> 8;
        byBuf[i * 2 + 1] = (((uint32_t)(fVolBuf[i] * 1000)) & 0xff);
    }
    iRet = SSUSB_SetPower(hDev, (unsigned char *)byBuf);

    return iRet;
}

int SSUSB_SetPowerOnOffVerV30(HANDLE hDev, bool bOnOff[4])
{
    // v30
    int iRet = -1;
    uint32_t val = 0;

    iRet = SSUSB_ReadFPGAReg(hDev, 0x8052, &val);
    val = val | 0x1f;
    // iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // 上电

    if (bOnOff[1]) // dovdd
    {
        val &= 0xfe;
        // iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dovdd mos on
        // Sleep(10);
    }

    if (bOnOff[2]) // dvdd
    {
        val &= 0xfb;
        // iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // dvdd mos on
        // Sleep(10);
    }

    if (bOnOff[0]) // avdd
    {
        val &= 0xf7;
        // iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // avdd mos on
        // Sleep(10);
    }

    if (bOnOff[3]) // afvcc
    {
        val &= 0xfd;
        // iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // afvcc mos on
        // Sleep(20);
    }
    iRet = SSUSB_WriteFPGAReg(hDev, 0x8052, val); // 上电

    return iRet;
}

int SSUSB_SetPowerVerPE(HANDLE hDev, float fVolBuf[5], bool bPowerUp)
{
    int iRet = -1;
    for (int i = 0; i < 4; i++)
    {
        float fR1 = 60 * fVolBuf[i] - 30;
        float fR2 = 0;
        int iValue1 = 0x7ff;
        int iValue2 = 0x400;
        if ((int)fR1 >= 100)
        {
            fR2 = fR1 - 100;
            iValue2 = (int)(1024 * fR2 / 100) | 0x400;
            iValue1 = 0x7ff;
        }
        else
        {
            iValue1 = (int)(1024 * fR1 / 100) | 0x400;
            iValue2 = 0x400;
        }
        // enable
        unsigned char bTmp = 0;
        iRet = SSUSB_USB2I2CCfg(hDev, 0x77 >> 1, 1, 1);
        switch (i)
        {
        case 0: // AVDD
            iRet = SSUSB_USB2I2CWrite(hDev, 1 << 3, 1 << 3);
            break;
        case 1: // D0VDD
            iRet = SSUSB_USB2I2CWrite(hDev, 1 << 2, 1 << 2);
            break;
        case 2: // DVDD
            iRet = SSUSB_USB2I2CWrite(hDev, 1 << 1, 1 << 1);
            break;
        case 3: // AFVCC
            iRet = SSUSB_USB2I2CWrite(hDev, 1 << 0, 1 << 0);
            break;
        default:
            break;
        }
        // R1
        SSUSB_USB2I2CCfg(hDev, 0x2C >> 1, 1, 1);
        iRet = SSUSB_USB2I2CWrite(hDev, 0x1C, 0x02);
        int temH = (iValue1 >> 8) & 0xff;
        int temL = iValue1 & 0xff;
        iRet = SSUSB_USB2I2CWrite(hDev, temH, temL);

        // R2
        iRet = SSUSB_USB2I2CCfg(hDev, 0x2F, 1, 1);
        iRet = SSUSB_USB2I2CWrite(hDev, 0x1C, 0x02);
        temH = (iValue2 >> 8) & 0xff;
        temL = iValue2 & 0xff;
        iRet = SSUSB_USB2I2CWrite(hDev, temH, temL);
        if (iRet)
            return iRet;
    }
    iRet = SSUSB_USB2I2CCfg(hDev, 0x40, 2, 1);
    iRet =
        SSUSB_USB2I2CWrite(hDev, 0x8025, 0x0f); // iWriteFPGAReg(iDeviceIndex, 0x8025, 0x0f, m_pstCurrentSensor->mode);
    return iRet;
}
int SSUSB_SetPowerVerV10(HANDLE hDev, float fVolBuf[5], bool bPowerUp, uint16_t *delaytime = NULL)
{
    int iRet = -1;
    if (delaytime == NULL)
        return iRet;
    if (bPowerUp)
    {
        SSUSB_SetPowerSupply(hDev, POWERSUPPLY_DOVDD, (uint32_t)(fVolBuf[1] * 1000));
        SSUSB_SetPowerSupply(hDev, POWERSUPPLY_DVDD, (uint32_t)(fVolBuf[2] * 1000));
        SSUSB_SetPowerSupply(hDev, POWERSUPPLY_AVDD, (uint32_t)(fVolBuf[0] * 1000));
        SSUSB_SetPowerSupply(hDev, POWERSUPPLY_AFVCC, (uint32_t)(fVolBuf[3] * 1000));
        SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_AFVCC, true);
        Sleep(100);
        uint16_t dovtime = *(delaytime + 1);
        uint16_t dvtime = *(delaytime + 2);
        uint16_t avtime = *(delaytime + 0);
        int dovOk = 0, dvOk = 0, avOk = 0;
        for (int i = 0; i < 1000; i++)
        {
            if (i >= dovtime)
            {
                iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_DOVDD, true);
                dovOk = 1;
            }

            if (i >= dvtime)
            {
                iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_DVDD, true);
                dvOk = 1;
            }

            if (i >= avtime)
            {
                iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_AVDD, true);
                avOk = 1;
            }
            if ((dovOk == 1) && (avOk == 1) && (dvOk == 1))
            {
                break;
            }
            else
            {
                Sleep(1);
            }
        }
    }
    else
    {
        iRet = SSUSB_SetPowerSupply(hDev, POWERSUPPLY_DOVDD, 0);
        iRet = SSUSB_SetPowerSupply(hDev, POWERSUPPLY_DVDD, 0);
        iRet = SSUSB_SetPowerSupply(hDev, POWERSUPPLY_AVDD, 0);

        iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_DOVDD, false);
        iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_DVDD, false);
        iRet = SSUSB_EnablePowerSupply(hDev, POWERSUPPLY_AVDD, false);
    }
    return iRet;
}

void SSUSB_GetFPGAClock(int iFrequence, int &iSSUSBFrequence)
{
    switch (iFrequence)
    {
    case 0:
        iSSUSBFrequence = 9;
        break;
    case 6000:
        iSSUSBFrequence = SENSORCLK_6MHZ;
        break;
    case 7000:
        iSSUSBFrequence = SENSORCLK_7MHZ;
        break;
    case 8000:
        iSSUSBFrequence = SENSORCLK_8MHZ;
        break;
    case 9000:
        iSSUSBFrequence = SENSORCLK_9MHZ;
        break;
    case 12000:
        iSSUSBFrequence = SENSORCLK_12MHZ;
        break;
    case 24000:
        iSSUSBFrequence = SENSORCLK_24MHZ;
        break;
    case 25000:
        iSSUSBFrequence = SENSORCLK_25MHZ;
        break;
    case 27000:
        iSSUSBFrequence = SENSORCLK_27MHZ;
        break;
    case 37125:
        iSSUSBFrequence = SENSORCLK_37_125MHZ;
        break;
    }
}
int SSUSB_SetMCLKVerV30(HANDLE hDev, int iFrequence)
{
    int iRet = -1;
    int iMClk = iFrequence; // kHz
    float fClkInput = iFrequence;
    int iOCT = 0;
    int iDAC = 0;
    unsigned int uValue = 0;
    iOCT = 3.322 * log10((double)(((iMClk * 1000) / 1039.0f) + 0.5));
    iDAC = 2048 - 2078 * pow(2, (10 + iOCT)) / (iMClk * 1000);
    uValue = ((iOCT & 0x0f) << 12) + (iDAC << 2);
    // iRet = SSUSB_USB2I2CCfg(hDev, 0x17, 0, 2); // 0x17:Oscillator Slave address
    // iRet = SSUSB_USB2I2CWrite(hDev, 0, uValue);
    // 配置0x8051 [3:2]=10 LS_CTRL_OE CTRL信号电平   转换使能/方向
    iRet = SSUSB_ReadFPGAReg(hDev, 0x8051, &uValue);
    uValue &= (~0x04);
    uValue |= (0x08);
    iRet = SSUSB_WriteFPGAReg(hDev, 0x8051, uValue); //[3.2]=10
    if ((iFrequence > 75000))
    {
        return -1000;
    }
    if ((iFrequence == 0) || (iFrequence == 6000) || (iFrequence == 7000) || (iFrequence == 8000) ||
        (iFrequence == 9000) || (iFrequence == 12000) || (iFrequence == 24000) || (iFrequence == 25000) ||
        (iFrequence == 27000) || (iFrequence == 37125))
    {
        // FPGA产生时钟
        int iSSUSBFrequence = 9; // 0HZ 0x8007
        SSUSB_GetFPGAClock(iFrequence, iSSUSBFrequence);
        iRet = SSUSB_SetSensorClock(hDev, iSSUSBFrequence);
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8029, 0x02); // 选择MCLK 来源
        unsigned int uValue = 0;
        iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue); // 关闭时钟芯片使能 CLK IC EN
        uValue &= (~0x10);
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue); // PFGA 0x8025 bit[4]: MClk enable signal
        m_cOsciMap.m_iOldHz = 0;
    }
    else // IC
    {
        float fKHz = iFrequence / 100.0f + 0.5;
        int iMapColIndex = floor(fKHz) - 100; // base:10MHz
        if ((m_cOsciMap.m_iOldHz == iMapColIndex) & (iMapColIndex != 0))
        {
            return 0;
        }
        unsigned int uValue = 0;
        iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue); // 时钟芯片使能
        uValue |= 0x10;
        // signal
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
        iRet = SSUSB_USB2I2CCfg(hDev, 0x58, 1, 1, I2CBUS1_BASEBOARD);
        for (int i = 0; i <= 33; i++)
        {
            iRet = SSUSB_USB2I2CWrite(hDev, m_iaryAddressList[i], m_cOsciMap.m_byarrOsciMap[i][iMapColIndex]);
        }
        // 给FX3一个信号，输出一个脉冲，便于示波器捕捉信号
        //  SSUSB_SetClockFlag(m_pstDeviceIdentity->hHandleList[iDeviceIndex]);
        m_cOsciMap.m_iOldHz = iMapColIndex;
        // 使能晶振输出
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8029, 0x01);
    }
    return iRet;
}
int SSUSB_SetMCLKVerV20(HANDLE hDev, int iFrequence)
{
    int iRet = -1;
    int iMClk = iFrequence; // kHz
    float fClkInput = iFrequence;
    int iOCT = 0;
    int iDAC = 0;
    unsigned int uValue = 0;
    iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue);
    uValue |= 0x10;
    iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
    iOCT = 3.322 * log10((double)(((iMClk * 1000) / 1039.0f) + 0.5));
    iDAC = 2048 - 2078 * pow(2, (10 + iOCT)) / (iMClk * 1000);
    uValue = ((iOCT & 0x0f) << 12) + (iDAC << 2);
    iRet = SSUSB_USB2I2CCfg(hDev, 0x17, 0, 2); // 0x17:Oscillator Slave address
    iRet = SSUSB_USB2I2CWrite(hDev, 0, uValue);
    if ((iFrequence > 75000))
    {
        return -1000;
    }
    if (iFrequence < 10000)
    {
        int iSSUSBFrequence = 9; // 0HZ 0x8007
        SSUSB_GetFPGAClock(iFrequence, iSSUSBFrequence);
        iRet = SSUSB_SetSensorClock(hDev, iSSUSBFrequence);
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8029, 2);
        unsigned int uValue = 0;
        iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue);
        uValue &= (~0x10);
        // signal
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
        m_cOsciMap.m_iOldHz = 0;
    }
    else
    {
        float fKHz = iFrequence / 100.0f + 0.5;
        int iMapColIndex = floor(fKHz) - 100; // base:10MHz
        if (iFrequence == 37125)              // 37.125MHz
        {
            iMapColIndex = 901;
        }
        if (m_cOsciMap.m_iOldHz == iMapColIndex)
        {
            return 0;
        }
        unsigned int uValue = 0;
        iRet = SSUSB_ReadFPGAReg(hDev, 0x8025, &uValue);
        uValue |= 0x10;
        // signal
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8025, uValue);
        iRet = SSUSB_USB2I2CCfg(hDev, 0x58, 1, 1, I2CBUS1_BASEBOARD);
        for (int i = 0; i <= 33; i++)
        {
            iRet = SSUSB_USB2I2CWrite(hDev, m_iaryAddressList[i], m_cOsciMap.m_byarrOsciMap[i][iMapColIndex]);
        }
        // 给FX3一个信号，输出一个脉冲，便于示波器捕捉信号
        // 使能晶振输出
        iRet = SSUSB_WriteFPGAReg(hDev, 0x8029, 1);
        m_cOsciMap.m_iOldHz = iMapColIndex;
    }
    return iRet;
}

int SSUSB_SwitchCommunicationMode(HANDLE hDev, int iMode)
{
    static std::map<HANDLE, int> mapDeviceLastMode;
    if (mapDeviceLastMode.find(hDev) == mapDeviceLastMode.end())
    {
        mapDeviceLastMode[hDev] = -1;
    }
    if (mapDeviceLastMode[hDev] == iMode)
    {
        return 0;
    }
    unsigned int uTmpVal = 0;
    SSUSB_ReadFPGAReg(hDev, 0x8051, &uTmpVal);
    switch (iMode)
    {
    case MODE_I2C:
        uTmpVal |= 1 << 6;
        break;
    case MODE_SPI:
        uTmpVal &= ~(1 << 6);
        break;
    }
    SSUSB_WriteFPGAReg(hDev, 0x8051, uTmpVal);
    mapDeviceLastMode[hDev] = iMode;
    Sleep(10);
    return 0;
}

int USBSetIOExpCfg(HANDLE hDev, uint32_t port, BYTE cfg)
{
    UINT8 pBuf[4];

    if (!hDev)
    {
        return -1;
    }

    *pBuf = cfg;

    if (CMDWrite(hDev, CMD_IOEXPIOCFG, port, pBuf, sizeof(BYTE)) != sizeof(BYTE))
    {
        return -2;
    }

    return 0;
}

int USBSetIOExpValue(HANDLE hDev, uint32_t port, BYTE value)
{
    UINT8 pBuf[4];

    if (!hDev)
    {
        return -1;
    }

    *pBuf = value;

    if (CMDWrite(hDev, CMD_IOEXPIOVALUE, port, pBuf, sizeof(BYTE)) != sizeof(BYTE))
    {
        return -2;
    }

    return 0;
}

// 计算发送（或读取）数据的crc，并与FPGA内部计算的crc进行对比，若相同，则返回1，否则，返回0
int SSUSB_CalcDataCRC(HANDLE hDev, uint8_t *const cpData, uint64_t uDataLen, bool bIsSendFlag, uint32_t &uRegCRC, uint32_t &uCalCRC)
{
    int nRet = 0;
    unsigned char i[8] = {0x00, 0x00, 0x00, 0x00, 0x06, 0x0d, 0xd2, 0xe3};
    uint32_t unCRC_16 = 0, unRegCRC_16 = 0;
    uint32_t unRegCRC_16MSB = 0, unRegCRC_16LSB = 0;

    unCRC_16 = CRC16_CCITT(cpData, uDataLen);

    if (bIsSendFlag)
    {
        // Send模式CRC
        SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
        nRet = SSUSB_USB2I2CRead(hDev, CRC16_SENDREGMSB, &unRegCRC_16MSB);
        SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
        nRet = SSUSB_USB2I2CRead(hDev, CRC16_SENDREGLSB, &unRegCRC_16LSB);
    }
    else
    {
        // Get模式CRC
        SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
        nRet = SSUSB_USB2I2CRead(hDev, CRC16_GETREGMSB, &unRegCRC_16MSB);
        SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
        nRet = SSUSB_USB2I2CRead(hDev, CRC16_GETREGLSB, &unRegCRC_16LSB);
    }

    unRegCRC_16 = (unRegCRC_16MSB << 8) + unRegCRC_16LSB;
    // cout << hex << "Buffer_CRC  & Reg_CRC: 0x" << unCRC_16 << " &0x" << unRegCRC_16 << endl;
	uRegCRC = unRegCRC_16;
	uCalCRC = unCRC_16;
    if (unRegCRC_16 == unCRC_16)
        return 1;
    else
        return 0; // 接受到的数据与CRC不匹配，需要重新get一次
}

// 向FPGA DDR发送数据前设置相关寄存器
int SSUSB_InitSignalSend(HANDLE hDev, uint64_t uDataLen, uint32_t uDDRStartAddr)
{
    if (!hDev)
        return -1;
    uint32_t unVal = 0;

    int nRet = 0;
    // 控制GPIF方向，PC-->FPGA
    nRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 0);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 1);

    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8035, 1); // 设置FX3数据流传输方向：FX3->FPGA

    // 复位信号，上升沿
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x00);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x01);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x00);

    // 写入地址和数据长度
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8057, (uDDRStartAddr >> 24) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8058, (uDDRStartAddr >> 16) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8059, (uDDRStartAddr >> 8) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805a, (uDDRStartAddr >> 0) & 0xff);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805b, (uDataLen >> 24) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805c, (uDataLen >> 16) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805d, (uDataLen >> 8) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805e, (uDataLen >> 0) & 0xff);

    // 有效信号，上升沿
    nRet |= SSUSB_USB2I2CRead(hDev, 0x805f, &unVal);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805f, unVal & 0xfe); // 0x805f bit0写0
    Sleep(10);
    nRet |= SSUSB_USB2I2CRead(hDev, 0x805f, &unVal);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x805f, unVal | 0x01); // 0x805f bit0写1

    return nRet;
}

// 从FPGA的DDR中读取数据前需要设置FPGA相关寄存器
int SSUSB_InitSignalGet(HANDLE hDev, uint64_t uDataLen, uint32_t uDDRStartAddr)
{
    int nRet = 0;
    uint32_t ival = -1;
    // 控制GPIF方向，FPGA—>PC
    nRet |= SSUSB_USB2I2CCfg(hDev, 0x40, 0x02, 0x01, I2CBUS1_BASEBOARD);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 0);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x804f, 1);

    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8035, 0);

    ////复位信号，上升沿
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x00);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x01);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8034, 0x00);

    // 写入地址和数据长度
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8040, (uDDRStartAddr >> 24) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8041, (uDDRStartAddr >> 16) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8042, (uDDRStartAddr >> 8) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8043, (uDDRStartAddr >> 0) & 0xff);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8044, (uDataLen >> 24) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8045, (uDataLen >> 16) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8046, (uDataLen >> 8) & 0xff);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8047, (uDataLen >> 0) & 0xff);

    // 有效信号，上升沿
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8048, 0x00);
    Sleep(10);
    nRet |= SSUSB_USB2I2CWrite(hDev, 0x8048, 0x01);
    Sleep(10);

    return nRet;
}

int SSUSB_SetM1VersionFpgaClock(HANDLE hDev, int iType, float fFrequence)
{
    struct SSUSB_FPGA_M1_CLOCK_REG
    {
        uint32_t uParamD;
        uint32_t uParamM1;
        uint32_t uParamM2;
        uint32_t uParamM3;
        uint32_t uParamO1;
        uint32_t uParamO2;
        uint32_t uParamO3;
        uint32_t uParamLoad;
    };
    const std::map<int, SSUSB_FPGA_M1_CLOCK_REG> MAP_FPGA_M1_CLOCK_REG = {
        {0, {0x80a5, 0x80a6, 0x80a7, 0x80a8, 0x80a9, 0x80aa, 0x80ab, 0x80ac}},
        {1, {0x80b1, 0x80b2, 0x80b3, 0x80b4, 0x80b5, 0x80b6, 0x80b7, 0x80b8}},
        {2, {0x80ba, 0x80bb, 0x80bc, 0x80bd, 0x80be, 0x80bf, 0x80c0, 0x80c1}},
    };
    if (MAP_FPGA_M1_CLOCK_REG.find(iType) == MAP_FPGA_M1_CLOCK_REG.end())
    {
        return -1;
    }
    auto stClockReg   = MAP_FPGA_M1_CLOCK_REG.at(iType);
    auto stClockParam = CFpgaMmcmFreqGenerator::stGetMmcmFreqParameter(fFrequence);
    int  nRet         = 0;
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamLoad, 0x00);
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamD, stClockParam.fDivider);
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamM1, floor(stClockParam.fMultiplier));
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamM2, (uint32_t(stClockParam.fMultiplier * 1000) % 1000) & 0xff);
    nRet |=
        SSUSB_WriteFPGAReg(hDev, stClockReg.uParamM3, ((uint32_t(stClockParam.fMultiplier * 1000) % 1000) >> 8) & 0x3);
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamO1, floor(stClockParam.fVcoDivider));
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamO2, (uint32_t(stClockParam.fVcoDivider * 1000) % 1000) & 0xff);
    nRet |=
        SSUSB_WriteFPGAReg(hDev, stClockReg.uParamO3, ((uint32_t(stClockParam.fVcoDivider * 1000) % 1000) >> 8) & 0x3);
    nRet |= SSUSB_WriteFPGAReg(hDev, stClockReg.uParamLoad, 0x01);
    return nRet;
}
