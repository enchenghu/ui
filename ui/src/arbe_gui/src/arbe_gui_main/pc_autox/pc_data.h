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

typedef enum
{
    POWER_WRITE = 1, 
    CFAR_WRITE,
    DFT3_WRITE,
    DIFF_WRITE,
	REG_WRITE,
    POWER_READ,
    CFAR_READ,
    DFT3_READ,
    DIFF_READ,
	REG_READ,
	PC_READ,
	FFT_ADC_READ_START,
	FFT_ADC_READ_STOP,
	POINTCLOUD_DISPLAY_START,
	POINTCLOUD_DISPLAY_STOP
}commandType;

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
	commandMsg 	cmdmsg; 
	uint8_t 	pcTcpData[TCP_PC_SIZE_SINGLE];
} pcData_t;


typedef struct {	// 某个点的维度信息的系数
	uint16_t dfPower1;		// 功率1
	uint16_t dfPower2;		// 功率2
	uint16_t dfRange;			// 距离
	uint16_t dfDoppler;		// 速度
	uint16_t dfAzimuth;		// 方位角
	uint16_t dfElevation;		// 俯仰角
}DataFactor_st;	// 12Bytes

typedef struct {
	uint16_t pcPrefix; 		// 0xeeff
	uint16_t pcVersion;		// Version：0x0102 = V1.2
	uint32_t pcTimeLsb;		// 时间戳低位（ns为单位，还是UTC时间戳）
	uint32_t pcTimeMsb;		// 时间戳高位
	uint32_t pcPayloadLength;	// 点云数据长度  // 目前固定：1200
	uint16_t pcFrameCounter; 	// 点云图的帧计数
	uint16_t pcMessageNumber;	// rolling counter
	uint16_t pcState;			// 报文标志（比如：是否是最后一帧）
	DataFactor_st DataFactor; 	// 点云数据单位系数
	uint16_t pcHeaderCrc;		// UDP头Crc校验值（整个Header）
	uint16_t pcPayloadCrc;		// UDP点云数据校验值（Payload）
}PointCloud_st;	// 38Bytes


typedef struct {	
    uint32_t indensity;  // 功率    // (indensity_0 + indensity_1)/2
    uint32_t distance;   // 距离    // 单位：
    int16_t  speed;      // 速度    // 单位：    // 正数:远离 负数:靠近
    uint16_t vertical;   // 俯仰角  // Units : 1/256 degree  // 数据0实际上是-2.5°
    uint16_t  horizontal; // 水平角  // units : 360/32000*2 
}PC_PointMeta_st;   // 14Bytes



typedef struct 
{
	PointCloud_st 	pcHeader; 
	PC_PointMeta_st 	pcUdpData[UDP_PC_SIZE_SINGLE_V01];
} pcData_v01;

typedef struct 
{
	std::vector<pcData_v01> pcDataOneFrame;
}udpPcMsgOneFrame;

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

