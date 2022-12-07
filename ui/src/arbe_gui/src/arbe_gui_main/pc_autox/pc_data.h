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
#define BUFF_LEN 1024
#define MAX_BUFF_LEN 8

#define TCP_TIMES_PER_FRAME 200
#define UDP_TIMES_PER_FRAME 256
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
	FFT_ADC_READ_STOP
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
} motorCmdMsg;

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


typedef struct 
{	// 某个点的维度信息的系数
	uint16_t power1;		// 功率1
	uint16_t power2;		// 功率2
	uint16_t range;		// 距离
	uint16_t doppler;		// 速度
	uint16_t azimuth;		// 方位角
	uint16_t elevation;	// 俯仰角
}PcDataFactor;

typedef struct 
{
	uint16_t usPrefix; 		// 0xeeff
	uint16_t usType; 			// Version：0x10:1.0
	uint32_t unPayloadLength;	// 点云数据长度
	uint32_t unTimeLsb;		// 时间戳低位（ns为单位，还是UTC时间戳）
	uint32_t unTimeMsb;		// 时间戳高位
	uint16_t usFrameCounter; 	// 点云图的帧计数
	uint16_t usMessageNumber;	// rolling counter
	uint16_t ucState;			// 报文标志（比如：是否是最后一帧）
	PcDataFactor DataFactor; 	// 点云数据单位系数
	uint16_t usHeaderCrc;		// UDP头Crc校验值（整个Header）
	uint16_t usPayloadCrc;		// UDP点云数据校验值（Payload）
}PointCloud_V1_0_Header;

typedef struct  
{	
	uint16_t power1;		// 功率1
	uint16_t power2;		// 功率2
	uint16_t range;		// 距离
	uint16_t doppler;		// 速度
	uint16_t azimuth;		// 方位角
	uint16_t elevation;	// 俯仰角
	//    Unit16_t type;		// 保留（说明点的特殊情况）
}PcMetadata;

typedef enum
{
    MOTOR_CONNECT = 0x01,
    MOTOR_CONNECT_RET = 0xA1,

    MOTOR_DISCONNECT = 0xA1,
    MOTOR_DISCONNECT_RET = 0xA1,
	
    MOTOR_PID_READ,
    MOTOR_PID_READ_RET,

    MOTOR_PID_SET,
    MOTOR_PID_SET_RET,

    MOTOR_WORKMODE_READ,
    MOTOR_WORKMODE_READ_RET,

    MOTOR_WORKMODE_SET,
    MOTOR_WORKMODE_SET_RET,

    MOTOR_SHOW_ITEMS_READ,
    MOTOR_SHOW_ITEMS_READ_RET,

    MOTOR_SHOW_ITEMS_SET,
    MOTOR_SHOW_ITEMS_SET_RET,

    MOTOR_SOFT_VERSION_READ,
    MOTOR_SOFT_VERSION_READ_RET,

    MOTOR_HARD_VERSION_READ,
    MOTOR_HARD_VERSION_READ_RET,

    MOTOR_DEV_TYPE_READ,
    MOTOR_DEV_TYPE_READ_RET,

    MOTOR_SHOW_CYCLE_SET,
    MOTOR_SHOW_CYCLE_SET_RET,

	MOTOR_OPEN,
	MOTOR_OPEN_RET,

	MOTOR_CLOSE,
	MOTOR_CLOSE_RET,

	MOTOR_SHOW_ITEMS_CTRL,
	MOTOR_SHOW_ITEMS_CTRL_RET,

	MOTOR_SHOW_ITEMS_DISPLAY
}motorCmdType;
//PcMetadata PcMetadata_Arr[100];	// 每个UDP报文，包含2列激光数据
//PointCloud_V1_0_Header + PcMetadata_Arr
}

