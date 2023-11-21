#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<errno.h> 
#include <vector>
#include<sys/types.h> 
#include<netinet/in.h> 
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QMultiMap>
#include <QtCore/QFlags>
#include <QtCore/QDebug>
#include <QtCore/QStack>
#include <QtCore/QCache>
#include <QtCore/QMargins>
#define MAX_RADARS 10
#define TCP_PC_SIZE_SINGLE 128000
#define UDP_PC_SIZE_SINGLE 1024

#define UDP_PC_SIZE_SINGLE_V01 100

#define BUFF_LEN 1024
#define ITEMS_NUM 3

#define MAX_BUFF_LEN 8

#define TCP_TIMES_PER_FRAME 200
#define UDP_TIMES_PER_FRAME 192
#define MOTOR_ITEMS_NUM 10

#define UDP_PC_TIMES_PER_FRAME 200
#define BST_MAX_TASK_NUM		(16)
#define PI_FMCW 3.14159265
#define NONE_COLOR                  "\e[0m"			//清除颜色，即之后的打印为正常输出，之前的不受影响
#define BLACK                 "\e[0;30m"		//深黑
#define L_BLACK             "\e[1;30m"			//亮黑，偏灰褐
#define RED                     "\e[0;31m"		//深红，暗红
#define L_RED                 "\e[1;31m"		//鲜红
#define GREEN                "\e[0;32m"		//深绿，暗绿
#define L_GREEN            "\e[1;32m"		//鲜绿
#define BROWN               "\e[0;33m"		//深黄，暗黄
#define YELLOW              "\e[1;33m"		//鲜黄
#define BLUE                    "\e[0;34m"		//深蓝，暗蓝
#define L_BLUE                "\e[1;34m"		//亮蓝，偏白灰
#define PURPLE               "\e[0;35m"		//深粉，暗粉，偏暗紫
#define L_PURPLE           "\e[1;35m"		//亮粉，偏白灰
#define CYAN                   "\e[0;36m"		//暗青色
#define L_CYAN               "\e[1;36m"		//鲜亮青色
#define GRAY                   "\e[0;37m"		//灰色
#define WHITE                  "\e[1;37m"		//白色，字体粗一点，比正常大，比bold小
#define BOLD                    "\e[1m"		//白色，粗体
#define UNDERLINE         "\e[4m"		//下划线，白色，正常大小
#define BLINK                   "\e[5m"		//闪烁，白色，正常大小
#define REVERSE            "\e[7m"		//反转，即字体背景为白色，字体为黑色
#define HIDE                     "\e[8m"		//隐藏
#define CLEAR                  "\e[2J"		//清除
#define CLRLINE               "\r\e[K"		//清除行

namespace fmcw_types
{
typedef struct 
{
	uint32_t indensity_0 = 0;
	uint32_t indensity_1 = 0;
	double distance = 0;
	double speed = 0;
	uint32_t reserved = 0;

} fmcwPoint;

typedef std::vector<fmcw_types::fmcwPoint> fmcwPoints;

typedef enum {
    POWER_WRITE                 = 1 ,   // 设置激光器输出功率
    CFAR_WRITE                  = 2 ,   // 设置CFAR
    DFT3_WRITE                  = 3 ,   // 设置DFT3
    DIFF_WRITE                  = 4 ,   // 设置DIFF
    REG_WRITE                   = 5 ,   // 写REG
    POWER_READ                  = 6 ,   // 读取激光器输出功率
    CFAR_READ                   = 7 ,   // 读取CFAR
    DFT3_READ                   = 8 ,   // 读取DFT3
    DIFF_READ                   = 9 ,   // 读取DIFF
    REG_READ                    = 10,   // 读REG
    FFT_ADC_READ_START          = 11,   // 读FFT_ADC值（开始）
    FFT_ADC_READ_STOP           = 12,   // 读FFT_ADC值（停止）
    POINTCLOUD_TCP_READ         = 13,   // TCP读点云数据（400000个点）
    POINTCLOUD_UDP_START        = 14,   // UDP读点云数据（开始）
    POINTCLOUD_UDP_STOP         = 15,   // UDP读点云数据（停止）
	ADC_OV_FLAG = 17
} commandType;

typedef enum{
    FFT_ORI = 0,
    FFT_DB,
    ADC_ORI,
    MOTOR_ORI
} showModel;

typedef struct API_Header
{
	uint16_t 	usPrefix; // 0xeeff
	uint16_t 	usType; // 0x10 version 1.0
	uint16_t 	usCommand; // command enum
	uint16_t 	usPayloadCrc;
	uint32_t 	unLength;
}API_Header;

typedef struct 
{
	uint16_t 	usPrefix; // 0xeeff
	uint16_t 	usType; // 0x10 version 1.0
	uint16_t 	usRollingCounter; 
	uint16_t 	usPayloadCrc;
	uint32_t 	usFrameCounter;
	uint32_t 	unLength;
}UDP_Header;

typedef struct 
{
	API_Header 	mHead; 
	uint32_t 	mCommandVal[2];
} commandMsg;

#pragma pack(1)    // pack(1): pack之间的数据类型，1字节对齐

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     motor_index;
	uint8_t     cmd;
	uint8_t     dataLen;
} MotorMsgHeader; // 5 bytes

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
} MotorMsgHeaderOld; // 5 bytes

typedef struct 
{
	uint8_t		count;
	uint16_t 	crc; 
} MotorMsgTailer; // 3 bytes

typedef struct 
{
	MotorMsgHeader	header;
	uint8_t data[50];
	MotorMsgTailer 	tailer; 
} motorItemsMsg;

typedef struct 
{
	MotorMsgHeader header;
	MotorMsgTailer tailer; 
} motorCmdMsg;

typedef struct 
{
	MotorMsgHeader header;
	uint8_t     data;
	MotorMsgTailer 	tailer; 
} motorCmdMsg1;


typedef struct {
	uint8_t     item_id;
	float     data;
} ItemData;

typedef struct 
{
	MotorMsgHeader header;
	ItemData data[ITEMS_NUM];
	MotorMsgTailer 	tailer; 
} motorItemsShowMsg;

typedef struct 
{
	MotorMsgHeaderOld header;
	ItemData data[ITEMS_NUM];
	MotorMsgTailer 	tailer; 
} motorItemsShowMsgOld;


typedef struct 
{
	MotorMsgHeader header;
	uint8_t     data[16];
	MotorMsgTailer 	tailer; 
} motorPidMsg;

typedef struct 
{
	MotorMsgHeader header;
	float       cycle;
	float       p;
	float       i;
	float       d;
	MotorMsgTailer 	tailer; 
} motorPidSetMsg;

typedef struct 
{
	MotorMsgHeader header;
	uint16_t     data;
	MotorMsgTailer 	tailer; 
} motorShowCycleMsg;

typedef struct 
{
	MotorMsgHeader header;
	float     speed;
	float     angle;
	float     location;
	uint8_t		mode;
	MotorMsgTailer 	tailer; 
} motorWorkModeMsg;

typedef struct 
{
	MotorMsgHeader header;
	uint8_t     data[5];
	MotorMsgTailer 	tailer; 
} motorShowMsg;

typedef struct 
{
	uint8_t     data[128];
} motorMaxBuff;

typedef struct 
{
	uint8_t     data[128];
} stateMaxBuff;

#pragma pack()     // pack() 结束

typedef struct 
{
	UDP_Header 	mHead; 
	uint8_t 	pcUdpData[UDP_PC_SIZE_SINGLE];
} udpMsg;

typedef struct 
{
	QVector<double> dataFFT_0;
	QVector<double> dataFFT_1;
	QVector<double> dataFFT_2;
	QVector<double> dataFFTdB_0;
	QVector<double> dataFFTdB_1;
	QVector<double> dataFFTdB_2;
}fftMsg;

typedef struct 
{
	QVector<double> dataADC0;
	QVector<double> dataADC1;
}adcMsg;

typedef struct 
{
	std::vector<uint8_t> fftDataV;
	std::vector<uint8_t> adcDataV;
}udp_ADC_FFT_Msg;


typedef struct 
{
	commandMsg 	cmdmsg;  //head 20byte
	uint8_t 	pcTcpData[TCP_PC_SIZE_SINGLE];
} pcData_t;

/* pointcloud info*/
#pragma pack(1)    // pack(1): pack之间的数据类型，1字节对齐
typedef struct {
    uint16_t uphPrefix;         // 0xEEFF
    uint16_t uphVersion;        // 0x0102: Version V1.2
    uint32_t uphTimeLsb;        // 时间戳低位（ns为单位，还是UTC时间戳）
    uint32_t uphTimeMsb;        // 时间戳高位
    uint16_t uphPayloadLength;  // 点云数据长度  // 目前固定：1400（包含100个点云元数据）
    uint16_t uphFrameCounter;   // 点云图的帧计数
    uint16_t uphRollingCounter; // rolling counter
    uint16_t uphState;          // 报文标志（比如：是否是最后一帧）
    uint16_t uphHeaderCrc;      // UDP头数据Crc校验值（Header）
    uint16_t uphPayloadCrc;     // UDP载荷数据校验值（Payload）
}UDP_PC_head_st;    // 24Bytes


typedef struct {	
    uint32_t pcmIndensity;  // 功率
    uint32_t pcmDistance;   // 距离（单位：米）   // 实际值：÷65536
    int16_t  pcmSpeed;      // 速度（单位：米/s） // 实际值：÷128(1bit-signed, 8bit-integer, 7bit-decimal-fraction)  // 正数:远离 负数:靠近
    uint16_t pcmVertical;   // 俯仰角（单位：°）  // 实际值：÷256  // 数据0实际上是-2.5°
    uint16_t pcmHorizontal; // 水平角（单位：°）  // 实际值：*360/32000*2  // 0° ~ 359°
}PC_pointMeta_st;   // 14Bytes


typedef struct {
    UDP_PC_head_st  UDP_PC_head;                        // head：24字节
    PC_pointMeta_st UDP_PC_payload[100];   // Payload：1400字节 = 14 * 100
} UDP_PC_package_st;    // 1424字节


typedef struct 
{
	std::vector<UDP_PC_package_st> pcDataOneFrame;
}udpPcMsgOneFrame;

typedef struct 
{
	std::vector<PC_pointMeta_st> pcDataOneFrame;
	std::vector<uint16_t> frameCounter;
	uint16_t frameCounterLast;   // 点云图的帧计数
	uint16_t frameCounterCur;   // 点云图的帧计数
}udpPcMsgOneFrame360;

#pragma pack()     // pack() 结束
typedef enum
{
    MOTOR_CONNECT = 0x01,
    MOTOR_CONNECT_RET = 0xA1,

    MOTOR_DISCONNECT = 0x02,
    MOTOR_DISCONNECT_RET = 0xA2,
	
    MOTOR_PID_READ = 0x03,
    MOTOR_PID_READ_RET = 0xA3,

    MOTOR_PID_SET = 0x0C,
    MOTOR_PID_SET_RET = 0xAC,

    MOTOR_WORKMODE_READ = 0x06,
    MOTOR_WORKMODE_READ_RET = 0xA6,

    MOTOR_WORKMODE_SET = 0x0D,
    MOTOR_WORKMODE_SET_RET = 0xAD,

    MOTOR_SHOW_ITEMS_READ = 0x07,
    MOTOR_SHOW_ITEMS_READ_RET = 0xA7,

    MOTOR_SHOW_ITEMS_SET,
    MOTOR_SHOW_ITEMS_SET_RET,

    MOTOR_SOFT_VERSION_READ = 0x08,
    MOTOR_SOFT_VERSION_READ_RET = 0xA8,

    MOTOR_HARD_VERSION_READ = 0x09,
    MOTOR_HARD_VERSION_READ_RET = 0xA9,

    MOTOR_DEV_TYPE_READ = 0x0A,
    MOTOR_DEV_TYPE_READ_RET = 0xAA,

    MOTOR_SHOW_CYCLE_SET = 0x0B,
    MOTOR_SHOW_CYCLE_SET_RET = 0xAB,

	MOTOR_OPEN = 0x0E,
	MOTOR_OPEN_RET = 0xAE,

	MOTOR_SHOW_ITEMS_CTRL = 0x0F,
	MOTOR_SHOW_ITEMS_CTRL_RET = 0xAF,

	MOTOR_ITEMS_INFO = 0xC0,
	//MOTOR_SHOW_ITEMS_CTRL_RET = 0xAF,

	MOTOR_SHOW_ITEMS_DISPLAY
}motorCmdType;
//PcMetadata PcMetadata_Arr[100];	// 每个UDP报文，包含2列激光数据
//PointCloud_V1_0_Header + PcMetadata_Arr
}

