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

}

