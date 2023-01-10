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
#define TCP_PC_SIZE_SINGLE 32000
#define UDP_PC_SIZE_SINGLE 1024

#define UDP_PC_SIZE_SINGLE_V01 100

#define BUFF_LEN 1024
#define MAX_BUFF_LEN 8

#define TCP_TIMES_PER_FRAME 200
#define UDP_TIMES_PER_FRAME 256
#define UDP_PC_TIMES_PER_FRAME 200
#define BST_MAX_TASK_NUM		(16)
#define PI_FMCW 3.14159265
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
} commandType;

typedef enum{
    FFT_ORI = 0,
    FFT_DB,
    ADC_ORI
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

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint8_t		count;
	uint16_t 	crc; 
} motorCmdMsg;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint8_t     data;
	uint8_t		count;
	uint16_t 	crc; 
} motorCmdMsg1;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint8_t     data[16];
	uint8_t		count;
	uint16_t 	crc; 
} motorPidMsg;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	float       cycle;
	float       p;
	float       i;
	float       d;
	uint8_t		count;
	uint16_t 	crc; 
} motorPidSetMsg;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint16_t     data;
	uint8_t		count;
	uint16_t 	crc; 
} motorShowCycleMsg;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint16_t     speed;
	uint16_t     angle;
	uint16_t     location;
	uint8_t		mode;
	uint8_t		count;
	uint16_t 	crc; 
} motorWorkModeMsg;

typedef struct 
{
	uint16_t 	mHead; 
	uint8_t     cmd;
	uint8_t     dataLen;
	uint8_t     data[5];
	uint8_t		count;
	uint16_t 	crc; 
} motorShowMsg;

typedef struct 
{
	UDP_Header 	mHead; 
	uint8_t 	pcUdpData[UDP_PC_SIZE_SINGLE];
} udpMsg;

typedef struct 
{
	QVector<double> dataFFT_0;
	QVector<double> dataFFT_1;
	QVector<double> dataFFTdB_0;
	QVector<double> dataFFTdB_1;

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

	MOTOR_SHOW_ITEMS_DISPLAY
}motorCmdType;
//PcMetadata PcMetadata_Arr[100];	// 每个UDP报文，包含2列激光数据
//PointCloud_V1_0_Header + PcMetadata_Arr
}

