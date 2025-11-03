// SSUSBLibTest.cpp :
//

#include "SSUSBLib.h"
#include "CMethod.h"
#include "SenPara.h"
#include <iostream>
#include <map>
#include <string>

#pragma comment(lib, "SSUSBLib.lib")
using namespace std;

SensorTab gstCurrentSensor;
string gstrFileName;
float garyfVolBuf[5];
bool bPowerUp = true;
unsigned int giHardwareVer = 4;
HANDLE ghDev = NULL;
SenPara gSenPara;
vector<STRegNode> gvecRegList; // 初始化寄存器列表

static int SensorInit(HANDLE ghDev);
int iOpenCamera();
int iCloseCamera();

const uint32_t i2c_regs[][2] = {
0x302c,0x00,
0x3105,0x12,
0x3215,0x11,
0x3214,0x11,
0x32e0,0x00,
0x3208,0x0c,
0x3209,0x80,
0x3211,0x04,
0x320a,0x07,
0x320b,0x08,
0x3213,0x04,
0x320c,0x03,
0x320d,0xc0,
0x320e,0x07,
0x320f,0x53,
0x3204,0x0c,
0x3205,0x87,
0x3206,0x07,
0x3207,0x0f,
0x5000,0x26,
0x30b0,0x01,
0x3018,0x7b,
0x480f,0x03,
0x301e,0xf0,
0x33b0,0x0f,
0x302c,0x00,
0x3e03,0x0b,
0x327f,0x3f,//pll1 pd all0
0x3250,0x43,//[6]mipi clk pd all0; [1:0] ana pd mipi when pd ramp
0x4800,0x24,
0x3250,0x40,
0x3e00,0x00,
0x3e01,0x74,
0x3e02,0xb0,
0x3722,0xfb,
0x3729,0xf4,
0x3727,0x24,
0x338f,0x80,
0x3333,0x10,
0x3334,0x40,
0x363b,0x57,//LCG used 8k
0x363a,0x80,
0x3304,0x50,
0x331e,0x39,
0x3309,0xb0,
0x331f,0x99,
0x33ae,0x30,
0x33af,0x90,
0x3308,0x18,
0x33ad,0x2c,
0x33ac,0x0c,
0x3633,0x4d,
0x3632,0x6d,
0x33b3,0x10,
0x3e08,0x00,
0x3632,0x6d,
0x33b3,0x10,
0x3722,0x8b,
0x3729,0xb4,
0x3306,0x90,
0x3301,0x12,
0x3223,0xc0,//[3]//自动降帧
0x3721,0x6c,
0x3364,0x5e,//0x3364[4] comprst 0x3301 auto enable, 0x33f2/0x33f3 readout
0x23b0,0x00, //comprst gain0 when dcg off
0x23b1,0x08, //comprst gain0 when dcg off    2x
0x23b2,0x00, //comprst gain1 when dcg off
0x23b3,0x18, //comprst gain1 when dcg off    4x
0x23b4,0x00, //comprst gain2 when dcg off
0x23b5,0x38, //comprst gain2 when dcg off    8x
0x3301,0x12, //comprst sel0 when dcg off
0x3393,0x18, //comprst sel1 when dcg off
0x3394,0x2c, //comprst sel2 when dcg off
0x3395,0x3c, //comprst sel3 when dcg off
0x23b6,0x04,//dcg enable gain0
0x23b7,0x08,//dcg enable gain0	2*hcg
0x23b8,0x04,//dcg enable gain1
0x23b9,0x18,//dcg enable gain1	4*hcg
0x23ba,0x04,//dcg enable gain2
0x23bb,0x38,//dcg enable gain2	8*hcg
0x3399,0x12,//sel0
0x339a,0x16,//sel1
0x339b,0x1e,//sel2
0x339c,0x2e,//sel3

//ramp offset
0x33b2,0x24,
//0x33b3 auto logic read 0x34f5 for auto value
0x349f,0x03,//[1] 0x33b3 auto en
0x23c0,0x04,//gain0
0x23c1,0x00,//gain0	1*hcg
0x23c2,0x04,//gain1
0x23c3,0x18,//gain1	4*hcg
0x23c4,0x04,//gain2
0x23c5,0x78,//gain2 	16*hcg
0x33b3,0x10,//sel0
0x34a8,0x10,//sel1
0x34a9,0x10,//sel2
0x34f9,0x12,//sel3

//0x3306 auto logic read 0x33f4[1:0],0x33f5 for auto value
0x3364,0x5e,//[3] 0x3306 auto en
0x23bc,0x04,//gain0
0x23bd,0x08,//gain0	2*hcg
0x23be,0x04,//gain1
0x23bf,0x78,//gain1	16*hcg
0x3305,0x00,
0x3306,0x70,//sel0
0x33f8,0x00,
0x33f9,0x70,//sel1
0x33fa,0x00,
0x33fb,0x70,//sel2

//0x330b auto logic read 0x34f6,0x34f7 for auto value
0x349f,0x03,//[0] 0x330b auto en
0x23c6,0x04,//gain0
0x23c7,0x08,//gain0	2*hcg
0x23c8,0x04,//gain1
0x23c9,0x78,//gain1               16*hcg
0x330a,0x01,//sel0
0x330b,0x20,
0x34aa,0x01,//sel1
0x34ab,0x20,
0x34ac,0x01,//sel2
0x34ad,0x20,

//0x3722 auto logic read 0x37d1 for auto value
0x370f,0x13,//[0] 0x3722 auto en
0x37b3,0x04,//gain0
0x37b4,0x08,//gain0               2*dcg
0x37b5,0x04,//gain1
0x37b6,0x38,//gain1               8*dcg
0x37b0,0x77,//sel0
0x37b1,0x77,//sel1
0x37b2,0x73,//sel2

//0x3728 auto logic read 0x37d2 for auto value
0x370f,0x13,//[1] 0x3728 auto en
0x37bd,0x04,//gain0
0x37be,0x08,//gain0               2*dcg
0x37bf,0x04,//gain1
0x37c0,0x38,//gain1               8*dcg
0x37b7,0x13,//sel0
0x37b8,0x00,//sel1
0x37b9,0x00,//sel2

//0x3729 auto logic read 0x37d3 for auto value
0x370f,0x13,//[4] 0x3729 auto en
0x37c1,0x04,//gain0
0x37c2,0x08,//gain0               2*dcg
0x37c3,0x04,//gain1
0x37c4,0x38,//gain1               8*dcg
0x37ba,0xc4,//sel0
0x37bb,0xc4,//sel1
0x37bc,0xc4,//sel2

//clamp logic
//0x3630 auto logic read 0x36c0 for auto value
0x36d0,0x0d,//[0] 0x3630 auto en
0x3673,0x04,//gain0
0x3674,0x08,//gain0	2*hcg
0x3675,0x04,//gain1
0x3676,0x18,//gain1	4*hcg
0x3670,0x42,//sel0
0x3671,0x33,//sel1
0x3672,0x34,//sel2

//current logic
//0x3632 auto logic read 0x36c2 for auto value
0x36d0,0x0d,//[2] 0x3632 auto en
0x3681,0x04,//gain0
0x3682,0x08,//gain0	2*hcg
0x3683,0x04,//gain1
0x3684,0x78,//gain1	16*hcg
0x367e,0x69,//sel0
0x367f,0x6d,//sel1
0x3680,0x8d,//sel2

//psrr logic
//0x363a auto logic read 0x36ca for auto value
0x36d0,0x0d,//[3] 0x363a auto en
0x368d,0x00,//gain0
0x368e,0x08,//gain0	2*lcg
0x368f,0x00,//gain1
0x3690,0x18,//gain1	4*lcg
0x3691,0x04,//gain2
0x3692,0x00,//gain2	1*hcg
0x3693,0x04,//gain3
0x3694,0x08,//gain3	2*hcg
0x3695,0x04,//gain4
0x3696,0x18,//gain4	4*hcg
0x3697,0x04,//gain5
0x3698,0x38,//gain5	8*hcg
0x3699,0x04,//gain6
0x369a,0x78,//gain6	16*hcg
0x3685,0x81,//sel0  1*lcg
0x3686,0x81,//sel1  2*lcg
0x3687,0x83,//sel2  4*lcg
0x3688,0x80,//sel3  1*hcg
0x3689,0x83,//sel4  2*hcg
0x368a,0x84,//sel5  4*hcg
0x368b,0x8d,//sel6  8*hcg
0x368c,0x9a,//sel7  16*hcg

//pll
0x36e9,0x24,
0x36ea,0x14,
0x36eb,0x45,
0x36ec,0x4b,
0x36ed,0x18,//vco = 1080M, mipi = 1080M, mipip = 67.5M, sysclk = 108M, cntclk = 540M

0x37f9,0x24,
0x37fa,0x18,
0x37fb,0x55,
0x37fc,0x19,
0x37fd,0x1a,

0x450a,0x80,//r_col_swap_en

0x3724,0xc1,
0x30b8,0x44,

//DPC 20240704
0x578d,0x41, //DPC AWB功能关闭

0x57aa,0x2a, // vbuf开关，提升清晰度Vbuf功能低增益开启，中高增益关闭
0x57ab,0x7f, //vbuf低增益打开
0x57ac,0x00,//vbuf 中增益关闭
0x57ad,0x00,// vbuf高增益关闭

0x5780,0x76, //DPC smth功能打开
0x57a1,0x04, //DPC smth强度

0x5784,0x10, //中/高增益切换点16xtotalgain
0x5785,0x08, //低/中增益切换点8xtotalgain

0x5799,0x46, //高增益打开双点白点，中高增益打开双点黑点
0x579a,0x77, //全档位开启单点黑/白点

0x57a8,0xd2, //SHDR模式下长短帧使用不同sram空间存储DPC数据

0x3e16,0x01, //0x00 0x80表示dcg系数为1x,SC635AI dcg系数2.66
0x3e17,0x54,
0x3e18,0x01,
0x3e19,0x54,

//DPC15
0x5787,0x0a, //低增益白点阈值   
0x5790,0x08, //低增益白点斜率
0x5788,0x0a, //中增益白点阈值   
0x5791,0x04, //中增益白点斜率
0x5789,0x08, //高增益白点阈值   
0x5792,0x04, //高增益白点斜率
0x578a,0x0a, //低增益黑点阈值   
0x5793,0x08, //低增益黑点斜率
0x578b,0x0a, //中增益黑点阈值   
0x5794,0x04, //中增益黑点斜率
0x578c,0x08, //高增益黑点阈值   
0x5795,0x04, //高增益黑点斜率

//adv DPC
0x58c0,0x30, //低增益白点阈值   
0x58c6,0x3c, //低增益白点斜率
0x58c1,0x28, //中增益白点阈值   
0x58c7,0x30, //中增益白点斜率
0x58c2,0x20, //高增益白点阈值   
0x58c8,0x28, //高增益白点斜率
0x58c3,0x30, //低增益黑点阈值   
0x58c9,0x3c, //低增益黑点斜率
0x58c4,0x28, //中增益黑点阈值   
0x58ca,0x30, //中增益黑点斜率
0x58c5,0x20, //高增益黑点阈值   
0x58cb,0x28, //高增益黑点斜率

//pixel 20240709
0x363c,0xd8,
0x363d,0x40,

//digit 20240722
0x3271,0x10,//Pdummy row
0x3905,0x2d,//1 line
0x450d,0x0a,//adc sync margin
0x3250,0x00,//mipi noncontinue mode

//Frame Avarage
0x3926,0xe0,//r_frmavg_en_ro
0x3900,0x05,
0x391d,0x00,//stbl_rang[0]=0
0x391c,0x26,//stbl_rang[1]=1.2
0x391b,0x40,//stbl_rang[2]=2
0x391a,0x60,//stbl_rang[3]=3
0x39dd,0x00,//alpha[0]=0
0x39de,0x08,//alpha[1]=1/16
0x39e7,0x04,//alpha[2]=1/32
0x39e8,0x04,//alpha[3]=1/32
0x39e9,0x80,//alpha[4]=1
//BLC max
0x3903,0x40,
0x3933,0x80,
0x3934,0x06,
0x3935,0x00,
0x3936,0xbc,
0x3937,0x71,
0x3938,0x73,
0x3939,0x0f,
0x393a,0xf7,
0x393b,0x0f,
0x393c,0xb3,

//20240904
//KBLC
0x3905,0x0d,
0x3903,0x60,
0x393f,0x80,//stpt
0x3940,0x0b,
0x3941,0x00,//ofst
0x3942,0x0b,
0x3943,0x7e,//B
0x3944,0x7f,//Gb
0x3945,0x7f,//Gr
0x3946,0x7e,//R、
//BLC max
0x3933,0x80,
0x3934,0x06,
0x3935,0x00,
0x3936,0xf0,
0x3937,0x7b,
0x3938,0x7c,
0x3939,0x0f,
0x393a,0xf3,
0x393b,0x0f,
0x393c,0xf9,


0x338f,0xa0,

0x301f,0x03,

//0x330d,0x10,

0x4837,0x1d,

//30fps
0x320e,0x0e,
0x320f,0xa6,
0x3e00,0x00,
0x3e01,0xe9,
0x3e02,0xe0,

0x36e9,0x24,
0x37f9,0x24,

0x0100,0x01,
};

void vCommandExecutor()
{
    int commandId = 0;
    uint8_t *pImgBuf = NULL;
    std::map<int, std::string> mapCommand = {
        {0, "Write High Speed Spi"},
        {1, "Read High Speed Spi"},
        {2, "Set Display Channel"},
        {3, "Set Hdmi Resolution And Frame Rate"},
        {4, "Set Hdmi Clock"},
        {5, "Set Hdmi Config"},
        {6, "Open Camera"},
        {7, "Write I2C"},
        {8, "Read I2C"},
        {9, "Fetch Frame"},
        {10, "M1 EVB SPI test 808b"},
		{11, "Discrete Write Sensor"},
		{12, "Discrete Read Sensor"},
        {-1, "Quit"},
    };
    int imageCnt = 0;
    pImgBuf = new uint8_t[4 * gstCurrentSensor.width * gstCurrentSensor.height];
    if (pImgBuf == NULL)
    {
        cout << "Allocate buffer failed" << endl;
    }
    while (1)
    {
        cout << "Please input command" << endl;
        for (auto iter = mapCommand.begin(); iter != mapCommand.end(); ++iter)
        {
            cout << iter->first << " : " << iter->second << endl;
        }
        cin >> commandId;
        switch (commandId)
        {
        case 0: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the SPI mode" << std::endl;
            int iSpiMode;
            std::cin >> iSpiMode;
            std::cout << "Please input the SPI speed" << std::endl;
            float fSpeedMhz;
            std::cin >> fSpeedMhz;
            std::cout << "Please input the SPI address" << std::endl;
            uint32_t uAddress;
            std::cin >> std::hex >> uAddress;
            std::cout << "Please input the SPI address length" << std::endl;
            uint32_t uAddrLength;
            std::cin >> uAddrLength;
            std::cout << "Please input the SPI value length" << std::endl;
            uint32_t uValueLength;
            std::cin >> uValueLength;
            std::vector<uint8_t> vecVal;
            for (int i = 0; i < uValueLength; i++)
            {
                std::cout << "Please input the value[" << i << "]" << std::endl;
                uint16_t uVal;
                std::cin >> std::hex >> uVal;
                vecVal.push_back(uVal & 0xff);
            }
            int iRet = SSUSB_HighSpeedFPGA2SPIWrite(ghDev, iSpiMode, fSpeedMhz, uAddress, vecVal.data(), uAddrLength,
                                                    uValueLength,FALSE);
            std::cout << "Write SPI " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 1: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the SPI mode" << std::endl;
            int iSpiMode;
            std::cin >> iSpiMode;
            std::cout << "Please input the SPI speed" << std::endl;
            float fSpeedMhz;
            std::cin >> fSpeedMhz;
            std::cout << "Please input the SPI address" << std::endl;
            uint32_t uAddress;
            std::cin >> std::hex >> uAddress;
            std::cout << "Please input the SPI address length" << std::endl;
            uint32_t uAddrLength;
            std::cin >> uAddrLength;
            std::cout << "Please input the SPI value length" << std::endl;
            uint32_t uValueLength;
            std::cin >> uValueLength;
            std::cout << "Please input the SPI dummy timing" << std::endl;
            uint8_t uDummyTiming;
            std::cin >> uDummyTiming;
            std::vector<uint8_t> vecVal(uValueLength);
            int iRet = SSUSB_HighSpeedFPGA2SPIRead(ghDev, iSpiMode, fSpeedMhz, uAddress, vecVal.data(), uAddrLength,
                                                   uValueLength, uDummyTiming, FALSE);
            std::cout << "Read SPI " << (iRet == 0 ? "successfully" : "failed") << std::endl;
            if (iRet == 0)
            {
                std::cout << "The value is: " << std::endl;
                for (int i = 0; i < uValueLength; i++)
                {
                    std::cout << std::hex << (int)vecVal[i] << " ";
                }
            }
        }
        break;
        case 2: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the display channel" << std::endl;
            int iChannel;
            std::cin >> iChannel;
            int iRet = SSUSB_SetDisplayChannel(ghDev, iChannel);
            std::cout << "Set display channel " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 3: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the HDMI channel" << std::endl;
            int iChannel;
            std::cin >> iChannel;
            std::cout << "Please input the HDMI width" << std::endl;
            int iWidth;
            std::cin >> iWidth;
            std::cout << "Please input the HDMI height" << std::endl;
            int iHeight;
            std::cin >> iHeight;
            std::cout << "Please input the HDMI frame rate" << std::endl;
            int iFrameRate;
            std::cin >> iFrameRate;
            int iRet = SSUSB_SetHdmiResolutionAndFrameRate(ghDev, iChannel, iWidth, iHeight, iFrameRate);
            std::cout << "Set HDMI resolution and frame rate " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 4: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the HDMI channel" << std::endl;
            int iChannel;
            std::cin >> iChannel;
            std::cout << "Please input the HDMI clock" << std::endl;
            float iClock;
            std::cin >> iClock;
            int iRet = SSUSB_SetHdmiClock(ghDev, iChannel, iClock);
            std::cout << "Set HDMI clock " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 5: {
            // SSUSB_API int SSUSB_SetHdmiImageParam(
            //     HANDLE hDev, uint32_t uHdmiChannel, uint32_t uHdmiActiveWidth, uint32_t uHdmiActiveHeight,
            //     uint32_t uHdmiTotalWidth, uint32_t uHdmiTotalHeight, uint32_t uHdmiHsync, uint32_t uHdmiVsync,
            //     uint32_t uHdmiHVPol, uint32_t uHdmiHFP, uint32_t uHdmiHBP, uint32_t uHdmiVFP, uint32_t uHdmiVBP);
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the HDMI channel" << std::endl;
            int iChannel;
            std::cin >> iChannel;
            std::cout << "Please input the HDMI active width" << std::endl;
            int iActiveWidth;
            std::cin >> iActiveWidth;
            std::cout << "Please input the HDMI active height" << std::endl;
            int iActiveHeight;
            std::cin >> iActiveHeight;
            std::cout << "Please input the HDMI total width" << std::endl;
            int iTotalWidth;
            std::cin >> iTotalWidth;
            std::cout << "Please input the HDMI total height" << std::endl;
            int iTotalHeight;
            std::cin >> iTotalHeight;
            std::cout << "Please input the HDMI Hsync" << std::endl;
            int iHsync;
            std::cin >> iHsync;
            std::cout << "Please input the HDMI Vsync" << std::endl;
            int iVsync;
            std::cin >> iVsync;
            std::cout << "Please input the HDMI HVPol" << std::endl;
            int iHVPol;
            std::cin >> iHVPol;
            std::cout << "Please input the HDMI HFP" << std::endl;
            int iHFP;
            std::cin >> iHFP;
            std::cout << "Please input the HDMI HBP" << std::endl;
            int iHBP;
            std::cin >> iHBP;
            std::cout << "Please input the HDMI VFP" << std::endl;
            int iVFP;
            std::cin >> iVFP;
            std::cout << "Please input the HDMI VBP" << std::endl;
            int iVBP;
            std::cin >> iVBP;
            int iRet = SSUSB_SetHdmiImageParam(ghDev, iChannel, iActiveWidth, iActiveHeight, iTotalWidth, iTotalHeight,
                                               iHsync, iVsync, iHVPol, iHFP, iHBP, iVFP, iVBP);
            std::cout << "Set HDMI config " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 6: {
            std::cout << mapCommand[commandId] << std::endl;
            // 加载配置
            gSenPara.vInitSenPara();
            int iRet = iOpenCamera();
            std::cout << "Open camera " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 7: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the I2C address" << std::endl;
            uint32_t uAddress;
            std::cin >> uAddress;
            std::cout << "Please input the I2C value" << std::endl;
            uint32_t uValue;
            std::cin >> uValue;
            int iRet = SSUSB_WriteSensorReg(ghDev, uAddress, uValue);
            std::cout << "Write I2C " << (iRet == 0 ? "successfully" : "failed") << std::endl;
        }
        break;
        case 8: {
            std::cout << mapCommand[commandId] << std::endl;
            std::cout << "Please input the I2C address" << std::endl;
            uint32_t uAddress;
            std::cin >> uAddress;
            uint32_t uValue;
            int iRet = SSUSB_ReadSensorReg(ghDev, uAddress, &uValue);
            std::cout << "Read I2C " << (iRet == 0 ? "successfully" : "failed") << std::endl;
            if (iRet == 0)
            {
                std::cout << "The value is: " << std::hex << uValue << std::endl;
            }
        }
        break;
        case 9: {
            std::cout << mapCommand[commandId] << std::endl;
            int size;
            FrameInfoV0_t info;
            for (int cnt = 0; cnt < 10; cnt++)
            {
                if (SSUSB_FetchFrame(ghDev, pImgBuf, 4 * gstCurrentSensor.width * gstCurrentSensor.height, &size, &info,
                                     3000) != 0)
                {
                    cout << "No Image!" << endl;
                    continue;
                }

                cout << "Get Image" << endl;
                uint8_t *pTempBuf = new uint8_t[4 * gstCurrentSensor.width * gstCurrentSensor.height];
                int j = 0;
                // 以Mipi 10bit为例
                for (int i = 0; i < size; i += 5)
                { // 10bit to 8bit
                    pTempBuf[(j + 0) * 3 + 0] = pImgBuf[i + 0];
                    pTempBuf[(j + 0) * 3 + 1] = pImgBuf[i + 0];
                    pTempBuf[(j + 0) * 3 + 2] = pImgBuf[i + 0];

                    pTempBuf[(j + 1) * 3 + 0] = pImgBuf[i + 1];
                    pTempBuf[(j + 1) * 3 + 1] = pImgBuf[i + 1];
                    pTempBuf[(j + 1) * 3 + 2] = pImgBuf[i + 1];

                    pTempBuf[(j + 2) * 3 + 0] = pImgBuf[i + 2];
                    pTempBuf[(j + 2) * 3 + 1] = pImgBuf[i + 2];
                    pTempBuf[(j + 2) * 3 + 2] = pImgBuf[i + 2];

                    pTempBuf[(j + 3) * 3 + 0] = pImgBuf[i + 3];
                    pTempBuf[(j + 3) * 3 + 1] = pImgBuf[i + 3];
                    pTempBuf[(j + 3) * 3 + 2] = pImgBuf[i + 3];

                    j += 4;
                }

                char fileNm[100];
                _snprintf_s(fileNm, sizeof(fileNm), "D:\\%d.bmp", ++imageCnt);

                if (WriteRgb24ToBmpFile(pTempBuf, info.width, info.height, fileNm) == 0)
                {
                    cout << "Save Image Successfully" << endl;
                }
                else
                {
                    cout << "Save Image failed" << endl;
                }
                if (pTempBuf)
                {
                    delete[] pTempBuf;
                }
            }
        }
        break;
        case 10: {
            std::map<int, std::pair<double, double>> mapSpiDelayRange = {
                {0, {-1, -1}}, {1, {-1, -1}}, {2, {-1, -1}}, {3, {-1, -1}}, {4, {-1, -1}}, {5, {-1, -1}},
            };
            std::vector<std::pair<uint32_t, uint32_t>> vectorSpiInit = {
                {0x31000103, 0x01}, {0x31003238, 0x00}, {0x31003239, 0x11}, {0x31002410, 0x22}, {0x31002410, 0x21},
                {0x3100c00f, 0x01}, {0x3100c00f, 0x00}, {0x31003239, 0x00}, {0x31003072, 0x00}, {0x31003073, 0x00},
                {0x310094e0, 0x01}, {0x310094e0, 0x00}, {0x3100a806, 0x01}, {0x3100a806, 0x00}, {0x3100a809, 0x01},
                {0x3100a809, 0x00}, {0x31003073, 0x07}, {0x31004001, 0xf0}, {0x31004001, 0xd0}, {0x31004002, 0xf0},
                {0x31004002, 0xe0}, {0x31004011, 0x80}, {0x31004011, 0x20}, {0x31004003, 0xf0}, {0x31004003, 0xc0},
                {0x31004004, 0xd0}, {0x31004004, 0x90}, {0x31004007, 0x10}, {0x31004007, 0x00}, {0x31004009, 0x0c},
                {0x31004009, 0x00}, {0x31003072, 0xe0}, {0x31009201, 0x03}, {0x31009201, 0x00}, {0x31000103, 0x01},
                {0x31003072, 0x00}, {0x31003073, 0x00}};

            BYTE buf[2];
            buf[0] = (400 & 0xff00) >> 8;
            buf[1] = 400 & 0x00ff;

            SSUSB_I2cConfig(ghDev, buf);

            std::vector<uint8_t> vecWriteValue(0);
            for (uint32_t i = 0; i < 70000; i++)
            {
                vecWriteValue.push_back(i % 255);
            }
            for (auto regPair : vectorSpiInit)
            {
                SSUSB_USB2I2CCfg(ghDev, 0x22, 4, 1);
                SSUSB_USB2I2CWriteBrust(ghDev, regPair.first, (uint8_t *)&regPair.second, 1);
                Sleep(1);
            }
            Sleep(100);
            SSUSB_USB2I2CCfg(ghDev, 0x40, 2, 1, 0, 0, 1);
            uint8_t tmp = 0;
            SSUSB_USB2I2CWriteBrust(ghDev, 0x808b, &tmp, 1);
            SSUSB_HighSpeedFPGA2SPIWrite(ghDev, 3, 10, 0x90000000, vecWriteValue.data(), 4, vecWriteValue.size(), FALSE);

            int tryCnt = -1;
            while (tryCnt++ < 10)
            {
                for (double speed = 10; speed <= 110; speed += 1)
                {
                    for (auto &delaySpeedPair : mapSpiDelayRange)
                    {
                        if (tryCnt != 0 &&
                            (delaySpeedPair.second.first < speed || delaySpeedPair.second.second > speed))
                        {
                            continue;
                        }
                        SSUSB_USB2I2CCfg(ghDev, 0x40, 2, 1, 0, 0, 1);
                        SSUSB_USB2I2CWriteBrust(ghDev, 0x808b, (uint8_t *)&delaySpeedPair.first, 1);
                        bool bSuccess = true;
                        std::vector<uint8_t> vecReadValue(vecWriteValue.size());
                        auto ret = SSUSB_HighSpeedFPGA2SPIRead(ghDev, 3, speed, 0x90000000, vecReadValue.data(), 4,
                                                               vecReadValue.size(), 8, FALSE);
                        if (memcmp(vecWriteValue.data(), vecReadValue.data(), vecWriteValue.size()) == 0)
                        {
                            delaySpeedPair.second.first =
                                delaySpeedPair.second.first == -1 ? speed : delaySpeedPair.second.first;
                            delaySpeedPair.second.second = speed;
                        }
                    }
                }
            }
            for (auto delaySpeedPair : mapSpiDelayRange)
            {
                std::cout << "The delay range for speed " << delaySpeedPair.second.first << " to "
                          << delaySpeedPair.second.second << " is " << delaySpeedPair.first << std::endl;
            }
            break;
        }
		case 11:
		{
			int iRegCnt = sizeof(i2c_regs) / 8;
			std::vector<uint32_t> vecAddr(iRegCnt, 0);
			std::vector<uint32_t> vecValue(iRegCnt, 0);
			std::vector<uint32_t> vecDelay(iRegCnt, 100);

			for (int i = 0; i < iRegCnt; i++)
			{
				vecAddr[i] = i2c_regs[i][0];
				//vecValue[i] = i2c_regs[i][1];
			}
			gSenPara.vInitSenPara();
			int iRet = iOpenCamera();

			iRet = SSUSB_USB2I2CWriteDiscrete(ghDev, vecAddr.data(), vecValue.data(), vecDelay.data(), iRegCnt, gSenPara.SlaveID >> 1, 2, 1, 0, 0, 1);
			std::cout << "Retvalue: " << iRet << std::endl;
		}
		break;
		case 12:
		{
			int iRegCnt = sizeof(i2c_regs) / 8;
			std::vector<uint32_t> vecAddr(iRegCnt, 0);
			std::vector<uint32_t> vecValue(iRegCnt, 0);
			std::vector<uint32_t> vecDelay(iRegCnt, 100);

			for (int i = 0; i < iRegCnt; i++)
			{
				vecAddr[i] = i2c_regs[i][0];
				//vecValue[i] = i2c_regs[i][1];
			}
			gSenPara.vInitSenPara();
			int iRet = iOpenCamera();

			iRet = SSUSB_USB2I2CReadDiscrete(ghDev, vecAddr.data(), vecValue.data(), vecDelay.data(), iRegCnt, gSenPara.SlaveID >> 1, 2, 1, 0, 1, 1);
			std::cout << "Retvalue: " << iRet << std::endl;
			if (!iRet)
			{
				for (int i = 0; i < iRegCnt; i++)
				{
					std::cout << std::hex << vecValue[i] << std::endl;
				}
			}
			std::cout << std::dec;
		}
		break;
        case -1: {
            std::cout << mapCommand[commandId] << std::endl;
            return;
        }
        break;
        }
    }
}

int main()
{
    string cmd;
    HANDLE *hDevList = NULL;
    int nRet = 0;

    // 打开设备
    int iDevCnt = 0;
    DevListInfo_t gstDevListInfo = {0}; // 设备信息列表
    iDevCnt = SSUSB_EnumDevWithSerialNumber(&gstDevListInfo);
    if (iDevCnt < 1)
    {
        cout << "Enum device failed!" << endl;
        return -1;
    }

    // 打开DemoSens设备，以第一个设备为例
    hDevList = SSUSB_OpenDevAddr(1, gstDevListInfo.DevAddr, 1);

    if (hDevList)
    {
        cout << "Open device successfully!" << endl;
        ghDev = hDevList[0];
    }
    else
    {
        cout << "Open device failed!" << endl;
        return -1;
    }
    vCommandExecutor();

    iCloseCamera();
    SSUSB_CloseDev(hDevList);
    return 0;
}

static int SensorInit(HANDLE ghDev)
{
    int ret = -1;
    ret = SSUSB_USB2I2CCfg(ghDev, gSenPara.SlaveID >> 1, 2, 1);
    for (auto iter = gvecRegList.begin(); iter != gvecRegList.end(); ++iter)
    {
        ret = SSUSB_USB2I2CWrite(ghDev, iter->uReg, iter->uVal);
        if (ret != 0)
        {
            break;
        }
    }
    return ret;
}
int iOpenCamera()
{
    int nRet = 0;

    // 获取硬件版本号
    giHardwareVer = SSUSB_GetHardwareVersion(ghDev);
    SSUSB_SetLibMode(giHardwareVer);
    nRet = SSUSB_SetSensorIntCfg(ghDev, gSenPara.SlaveID >> 1, 2, 1);

    // 设置MCLK
    SSUSB_SetMCLKEx(ghDev, gSenPara.mclk, gSenPara.mclken);

    gSenPara.vSetSensorCfg(ghDev);
    SSUSB_CloseSensorChannel(ghDev);
    Sleep(100);
    SSUSB_OpenSensorChannel(ghDev);
    Sleep(5);

    // 电压设置
    bPowerUp = true;
    SSUSB_SetPowerEx(ghDev, gSenPara.fVoltage, bPowerUp);
    ////V30 上电时序测试
    // bool bOnOff[4];
    // int Order[4];
    // Order[0] = 2;
    // Order[1] = 0;
    // Order[2] = 3;
    // Order[3] = 1;
    // bOnOff[0] = FALSE;
    // bOnOff[1] = FALSE;
    // bOnOff[2] = FALSE;
    // bOnOff[3] = FALSE;
    // SSUSB_SetPowerValue(ghDev, gSenPara.fVoltage);
    // for (int i = 0; i < 4; i++)
    //{
    //	bOnOff[Order[i]] = TRUE;
    //	SSUSB_SetPowerOnOff(ghDev, bOnOff);
    // }

    bPowerUp = false;
    BOOL bReset = ((gSenPara.pin & 0x02) == 0) ? FALSE : TRUE;
    BOOL bPwdn = ((gSenPara.pin & 0x01) == 0) ? FALSE : TRUE;
    nRet = SSUSB_SetSensorReset(ghDev, bReset);
    nRet = SSUSB_SetSensorPWDN(ghDev, bPwdn);
    Sleep(100);

    if (SensorInit(ghDev) == 0)
    {
        cout << "Sensor Initialization Success!!" << endl;
    }
    else
    {
        cout << "Sensor Initialization failed!!" << endl;
    }

    Sleep(100);
    nRet = SSUSB_StartCapture(ghDev);
    SSUSB_GetFrame(ghDev);
    return nRet;
}
int iCloseCamera()
{
    float vol[5] = {0, 0, 0, 0, 0};
    SSUSB_SetPowerEx(ghDev, vol);
    SSUSB_StopCapture(ghDev);
    return 0;
}