
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<errno.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<netinet/in.h> 
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
#define MAXLINE 4096 
#define INFO "info"
#define WARNING "warning"
#define ERROR "error"
#define LOG(n) std::cout << n << "|" << __FILE__ << ":" << __LINE__ << "->|"
#define LOG_END << std::endl;
#define TCP_PC_SIZE 32000
#define UDP_IP "127.0.0.1"
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

typedef enum {
    /* 硬件系统 */
    sm_mCode_SYS   = 0xA1,     // SYS（版本/时钟）
    sm_mCode_POW   = 0xA2,     // POW（电源）
    sm_mCode_CPU   = 0xA3,     // CPU
    sm_mCode_FPGA  = 0xA4,     // FPGA
    sm_mCode_DDR   = 0xA5,     // DDR
    sm_mCode_eMMC  = 0xA6,     // eMMC
    
    /* 接口外设 */
    sm_mCode_SD    = 0xB1,     // SD卡
    sm_mCode_ETH   = 0xB2,     // ETH（以太网）
    sm_mCode_UART  = 0xB3,     // UART（串口）
    sm_mCode_ADDA  = 0xB4,     // ADDA（采样系统）

    /* 光电系统 */
    sm_mCode_NLL   = 0xC1,     // NLL（窄带宽激光器）
    sm_mCode_PHA   = 0xC2,     // PHA（相位调制器）
    sm_mCode_FILT  = 0xC3,     // FILT（光纤滤波器）
    sm_mCode_EDFA  = 0xC4,     // EDFA 激光放大器

    /* 电机系统 */
    sm_mCode_POLY  = 0xD1,     // Polygon电机
    sm_mCode_GALV  = 0xD2,     // Galvo电机

    /* 软件系统 */
    sm_mCode_PROG  = 0xE1,     // PROG（进程）
    sm_mCode_THRD  = 0xE2,     // PROG（进程）
} flidar_sm_moduleCode;

typedef enum {
    sm_mCmd_base    = 0xF1,     // 基本信息
    sm_mCmd_stat    = 0xF2,     // 状态信息
    sm_mCmd_warn    = 0xF3,     // 报警信息
} flidar_sm_moduleCmd;
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

    MOTOR_SYSTEM_RESET = 0x10,

	MOTOR_ITEMS_INFO = 0xC0,
	//MOTOR_SHOW_ITEMS_CTRL_RET = 0xAF,

	MOTOR_SHOW_ITEMS_DISPLAY
}motorCmdType;
uint8_t *encode_cali_data = nullptr;
long file_size_g = 0;
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
	API_Header 	mHead; // 0xeeff
	uint32_t 	mCommandVal[2];
	//float 	mCommandVal_f;
} commandMsg;

typedef struct 
{
	commandMsg 	cmdmsg; 
	uint8_t 	pcTcpData[TCP_PC_SIZE];
} pcData_t;

typedef struct 
{
	UDP_Header 	mHead; 
	uint8_t 	pcUdpData[1024];
} udpMsg;

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

typedef struct {
    uint16_t frameHead;     // 起始头：0xAA 0x55
    uint32_t timeStamp;     // 时间戳：高22位:秒，低10位:ms
    uint8_t  validPeriod;   // 消息有效期（单位：100ms）
    uint8_t  countToken;    // 消息帧计数（每次+1）
    uint8_t  moduleCode;    // 模块代码
    uint8_t  moduleCmd;     // 模块指令
    uint8_t  dataLen;       // dataBody 长度
} flidar_sm_msgFrame_st;

typedef struct {
    uint8_t  lhtSrc;        // 光源状态：0:光源关 1:光源开
    uint16_t powerIn;       // 输入功率（uW）
    uint16_t powerOut;      // 输出功率（mW）
    uint16_t mduTemp;       // 模块温度：(val ÷ 100) ℃
    uint16_t pump1thTemp;   // 第一个泵浦温度：(val ÷ 100) ℃
    uint16_t pump1thCurr;   // 第一个泵浦电流（mA）
    uint16_t pump2thCurr;   // 第二个泵浦电流（mA）
} flidar_sm_EDFA_stat_st;

typedef struct {
    uint8_t w_offLine;
    uint8_t w_mduTemp;      // 模块温度异常         // Bit[7]：1:异常 0:正常；Bit[6~0]: 异常次数
    uint8_t w_seedTemp;     // 种子激光器温度异常
    uint8_t w_powerIn;      // 输入功率异常
    uint8_t w_pumpTemp;     // 泵浦温度异常
    uint8_t w_pumpPower;    // 泵浦功率异常
    uint8_t w_seedPower;    // 种子激光器功率异常
    uint8_t w_saveData;     // 保存数据出错
} flidar_sm_EDFA_warn_st;

typedef struct {
    flidar_sm_msgFrame_st  header;                        // head：24字节
    flidar_sm_EDFA_stat_st load;   // Payload：1400字节 = 14 * 100
    uint8_t crc;
} stateMsg; 

typedef struct {
    flidar_sm_msgFrame_st  header;                        // head：24字节
    flidar_sm_EDFA_warn_st load;   // Payload：1400字节 = 14 * 100
    uint8_t crc;
} warnMsg; 

#pragma pack()

pcData_t g_msg;
UDP_PC_package_st g_msg_pc;

bool ifstop = false;
bool ifPCstop = false;

bool check_file_exist(const std::string &path) {
#ifdef _MSC_VER  
	bool ret = 0 == _access(path.c_str(), 0);
#else
	bool ret = 0 == access(path.c_str(), F_OK);
#endif
	if (!ret) {
		LOG(INFO) << path << " not exist";
	}
	return ret;
}

size_t GetFileSize(const string &filepath) {
	FILE *fd = fopen(filepath.c_str(), "rb");
	if (fd == NULL) {
		LOG(ERROR) << "Failed to open file " << filepath;
		return 0;
	}
	// Determine size of the file
	fseek(fd, 0, SEEK_END);
	size_t file_length = static_cast<size_t>(ftell(fd));
	fseek(fd, 0, SEEK_SET);
	fclose(fd);

	return file_length;
}


int  LoadDat(const char *cali_file_path)
{
	if (NULL == cali_file_path) {
		LOG(ERROR) << "Bad cali_file_path " LOG_END;
		return -1;
	}
	if (!check_file_exist(cali_file_path)) {
		LOG(ERROR) << "Cali file " << cali_file_path << " not exist " LOG_END;
		return -2;
	}
	auto file_size = GetFileSize(cali_file_path);
    cout << "file_size is " << file_size << endl;
	if (file_size < 1000) {
		LOG(ERROR) << "Bad cali file size " << file_size LOG_END;
		return -3;
	}

	FILE *file = fopen(cali_file_path, "rb");
	encode_cali_data = (uint8_t *)malloc(file_size);
    file_size_g = file_size;
	fseek(file, 0, SEEK_SET);
	fread(encode_cali_data, file_size, 1, file);
    return file_size;
}



int loopsend(int socket, int size, uint8_t* data){
    int index = 0;
    //pcData_t msg;
    while(index < 200){
        usleep(1000);
        if(!data){
            cout << "data size error" << endl;
            return -1;
        }
        memset(&g_msg, 0, sizeof(g_msg));
        g_msg.cmdmsg.mHead.usCommand = PC_READ;
        g_msg.cmdmsg.mCommandVal[1] = index++;
        //cout << " count is " << index++ << endl;; 
        memcpy(g_msg.pcTcpData, data, TCP_PC_SIZE);
        int ret = send(socket, &g_msg, sizeof(g_msg), 0 );
        cout << "send byte is " << ret << endl;

        if( ret < 0 ){
            cout << "write msg failed" << endl;
            return -2;          
        }else if(ret == 0){
            cout << "write full" << endl;
            return -3;
        }
        data += TCP_PC_SIZE;
    }

    return 0;
}

int udpRecvSocketFd_  = 0;
int udpPcRecvSocketFd_  = 0;
int motorSocketFd_  = 0;
int motorSocketFdl_  = 0;
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
	uint8_t		count;
	uint16_t 	crc; 
} MotorMsgTailer; // 3 bytes
typedef struct {
	uint8_t     item_id;
	float     data;
} ItemData;
#define ITEMS_NUM 5
typedef struct 
{
	MotorMsgHeader header;
	ItemData data[ITEMS_NUM];
	MotorMsgTailer 	tailer; 
} motorItemsShowMsg;

typedef struct 
{
	MotorMsgHeader header;
	MotorMsgTailer 	tailer; 
} motorMsgRecv;

#pragma pack()     // pack() 结束

#define state 0
void motorSockerInit();

void *motor_msg_sender(void *)
{
    motorSockerInit();

#if state
    stateMsg stateMsg_;
    warnMsg warnMsg_;
    int counter = 0;
    stateMsg_.header.frameHead = 0x55aa;
    stateMsg_.header.moduleCode = sm_mCode_EDFA;
    stateMsg_.header.moduleCmd = sm_mCmd_stat;
    stateMsg_.header.dataLen = sizeof(flidar_sm_EDFA_stat_st);


    warnMsg_.header.frameHead = 0x55aa;
    warnMsg_.header.moduleCode = sm_mCode_EDFA;
    warnMsg_.header.moduleCmd = sm_mCmd_warn;
    warnMsg_.header.dataLen = sizeof(flidar_sm_EDFA_stat_st);

    int sum = 0;

    while(1){
        //memset(motorMsg.data, 0, motorMsg.header.dataLen);
        //int n = recv(motorSocketFd_, &sum, sizeof(int), MSG_WAITALL); 
        //printf("recv motorMsg , ret is %d!\n", n);
/*         stateMsg_.load.lhtSrc = sum;
        stateMsg_.load.powerIn = sum + 1;
        stateMsg_.load.powerOut = sum + 2;
        stateMsg_.load.mduTemp = sum + 3;
        stateMsg_.load.pump1thTemp = sum + 4;
        stateMsg_.load.pump1thCurr = sum + 5;
        stateMsg_.load.pump2thCurr = sum + 6; */
    

        warnMsg_.load.w_offLine = 128;
        warnMsg_.load.w_mduTemp = 127;
        warnMsg_.load.w_seedTemp = 128;
        warnMsg_.load.w_powerIn = 127;
        warnMsg_.load.w_pumpTemp = 128;
        warnMsg_.load.w_pumpPower = 127;
        warnMsg_.load.w_seedPower = 128;
        warnMsg_.load.w_saveData = 127;

        sum++;
        int ret = write(motorSocketFd_, &warnMsg_, sizeof(warnMsg_));
        printf("send warnMsg_ , ret is %d!\n", ret);
        if(ret < 0)
        {
            counter++;
            if(counter > 3) break;
        }
        usleep(1000*1000);
    }
#else
    motorMsgRecv motorMsg;
/*     int counter = 0;
    motorMsg.header.mHead = 0xaa55;
    motorMsg.header.motor_index = 0;
    motorMsg.header.cmd = 0xc0;
    motorMsg.header.dataLen = sizeof(ItemData) * ITEMS_NUM;
    motorMsg.tailer.count = 0; */
    while(1){
        memset(&motorMsg, 0, sizeof(motorMsg));
        int n = recv(motorSocketFdl_, &motorMsg, sizeof(motorMsg), MSG_WAITALL); 
        printf("motor recv len is %d , real is %d \n", n,  sizeof(motorMsgRecv)); 
        if(n == 0){
            close(motorSocketFdl_); 
            motorSocketFdl_ = accept(motorSocketFd_, (struct sockaddr*)NULL, NULL);
            printf("recv MOTOR continue !\n");
            continue;
        }
        switch (motorMsg.header.cmd)
        {
        case MOTOR_CONNECT:
            /* code */
            printf("recv MOTOR_CONNECT !\n");
            break;
        case MOTOR_PID_READ:
            /* code */
            printf("recv MOTOR_PID_READ !\n");
            break;
        case MOTOR_HARD_VERSION_READ:
            /* code */
            printf("recv MOTOR_HARD_VERSION_READ !\n");
            break;
        case MOTOR_SOFT_VERSION_READ:
            /* code */
            printf("recv MOTOR_SOFT_VERSION_READ !\n");
            break;            
        
        default:
            break;
        }
/*         printf("recv motorMsg , ret is %d!\n", n);
        for(int i = 0; i < ITEMS_NUM; i++){
            motorMsg.data[i].item_id = i;
            motorMsg.data[i].data = rand() % 100;
        }
        //motorMsg.tailer.crc =  motorMsg.header.motor_index +  motorMsg.header.cmd +  motorMsg.header.dataLen + \
        ;
        int ret = write(motorSocketFd_, &motorMsg, sizeof(motorMsg));
        printf("send motorMsg , ret is %d!\n", ret);
        if(ret < 0)
        {
            counter++;
            if(counter > 8) break;
        }
        usleep(500*1000); */
    }
#endif

}

void *udp_msg_sender(void *)
 {
     int ret;
    struct sockaddr_in ser_addr; 

    udpRecvSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(udpRecvSocketFd_ < 0)
    {
        printf("create udpRecvSocketFd fail!");
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(UDP_IP);
    //ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    ser_addr.sin_port = htons(8889);  //端口号，需要网络序转换

#if 0

    ret = bind(udpRecvSocketFd_, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        printf("socket bind fail!\n");
    }
#endif

    socklen_t len;
    struct sockaddr_in src;
    uint8_t buf[1024] = "client send: TEST UDP MSG!\n";
    uint8_t buff[] = "00010203040506070809\n";
    string mbuf = "00010203040506070809";

	printf("ready recv udp msg!\n");
    len = sizeof(sockaddr);
    string header;
    header.resize(sizeof(UDP_Header), '0');
    std::cout << "header is " << header << std::endl;
    string msg = header + mbuf;
    std::cout << "msg size is " << msg.size() << std::endl;
    std::cout << "msg sizeof is " << sizeof(msg) << std::endl;
    std::cout << "header size is " << header.size() << std::endl;
    std::cout << "mbuf size is " << mbuf.size() << std::endl;
    udpMsg sendMsg;
    memset(&sendMsg, 0, sizeof(udpMsg));
    int index = 0;
    while(!ifstop)
    {
        //memset(buf, 0, 1024);
        //recvfrom(udpRecvSocketFd_, buf, 1024, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        //printf("client send is :%s\n",msg.c_str());
        //std::cout << "msg is " << msg << std::endl;
        sendMsg.mHead.usFrameCounter = index++;
        memset(&sendMsg, 0, sizeof(sendMsg));
        for(int i = 0; i < 256; i++){
            //sendMsg = i;
            int nnn = sendto(udpRecvSocketFd_, &sendMsg, sizeof(udpMsg), 0, (struct sockaddr*)&ser_addr, len);
            usleep(500);  //一秒发送一次消息
        }
#if 1
#endif
    }
 }



void pcSockerInit()
{
    udpPcRecvSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(udpPcRecvSocketFd_ < 0)
    {
        printf("create udpRecvSocketFd fail!");
    }
}

void motorSockerInit(void)
{
    motorSocketFd_ = socket(AF_INET, SOCK_STREAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(motorSocketFd_ < 0)
    {
        printf("create udpRecvSocketFd fail!");
    }
    struct sockaddr_in servaddr; 
    int one = 1;
	setsockopt(motorSocketFd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    //fcntl(motorSocketFd_, F_SETFL, O_NONBLOCK);
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);             
    servaddr.sin_port = htons(5002); 
    long long index_0 = 0;
    if( bind(motorSocketFd_, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    { 
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0); 
    } 

    if( listen(motorSocketFd_, 10) == -1)
    { 
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0); 
    } 
    printf("======waiting for Motor client's request======\n"); 
    int connfd = 0;
    while(1)
    { 
        if( (motorSocketFdl_ = accept(motorSocketFd_, (struct sockaddr*)NULL, NULL)) == -1)
        { 
            printf("accept socket error: %s(errno: %d)\n",strerror(errno),errno); 
            usleep(1000*500);
            continue; 
        } else {
            break;
        }
    }
    printf("======ready receive Motor data======\n"); 
	int nRecvBuf= 320 * 1024;//设置为32K
	setsockopt(motorSocketFdl_, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int nSendBuf= 320 * 1024;//设置为32K
	setsockopt(motorSocketFdl_, SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));


}

static vector<PC_pointMeta_st> mv_g;
void Save2filecsv()
{
	//memset(&curPcData, 0, sizeof(curPcData));
	static long findex = 0;
    string save_folder_ = ".";
    uint8_t* data = encode_cali_data;
#if 0
	std::string datPath;
	datPath = save_folder_ + "/data_test_raw_index" + std::to_string(findex++) +".bin";
	ROS_INFO("datPath is %s \n", datPath.c_str());
	std::ofstream datfile; 
	datfile.open(datPath, std::ios::out | std::ios::binary); 
	for(int i = 0; i < data.size(); i++) {
		datfile << data[i];
	}
	datfile.close();
#endif
#if 0
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
	ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
	ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
	std::string csvPath;
	csvPath = save_folder_ + "/data_convert_" + 
	std::to_string(ptminfo->tm_year + 1900) + 
	"-" + std::to_string(ptminfo->tm_mon + 1) +
	"-" + std::to_string(ptminfo->tm_mday) +
	"-" + std::to_string(ptminfo->tm_hour) +
	"-" + std::to_string(ptminfo->tm_min) +
	"-" + std::to_string(ptminfo->tm_sec) +
	+".csv";
	ROS_INFO("csvPath is %s \n", csvPath.c_str());
	std::ofstream csvfile; 
	csvfile.open(csvPath, std::ios::out); 
	csvfile << "intensity" << "," << "distance(m)" << "," 
	<< "speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)" << "\n";	
#endif

    PC_pointMeta_st temp;
    memset(&temp, 0, sizeof(PC_pointMeta_st));
	int32_t cur_data = 0;
	int index = 0;
	double distance;
	double speed;
	double vAngle;
	double hAngle;
	for(int i = 0; i < file_size_g; i++) {
		index += 1;
		if(index < 5)
			cur_data += data[i] << (8 * (index - 1));
		else if (index < 9)
			cur_data += data[i] << (8 * (index - 5));
		else if (index < 11)
			cur_data += data[i] << (8 * (index - 9));
		else if (index < 13)
			cur_data += data[i] << (8 * (index - 11));
		else if (index < 15)
			cur_data += data[i] << (8 * (index - 13));

		if(index == 4 || index == 8){
			if(index == 4) {
				//csvfile << cur_data << ",";	//intensity
                temp.pcmIndensity = cur_data;
			}
			else{
				distance = cur_data / 65536.0; //distance
                temp.pcmDistance = cur_data;
				//csvfile << distance << ",";	
			}
			cur_data = 0;
		}

		if(index == 10){
			if(cur_data > 32767)
				cur_data -= 65536;

            temp.pcmSpeed = cur_data;
            
			speed = cur_data / 128.0;
			//csvfile << speed << ",";	 // speed
			cur_data = 0;
		}

		if(index == 12){
			vAngle = cur_data / 256.0 - 2.5; //vertical
            temp.pcmVertical = cur_data;
			//csvfile << vAngle << ",";	
			cur_data = 0;
		}

		if(index == 14){
			hAngle = cur_data * 720.0 / 32000.0; //horizontal
            temp.pcmHorizontal = cur_data;
			if(hAngle > 360.0) hAngle -= 360.0;
			//csvfile << hAngle << "\n";	
			cur_data = 0;
		}

		if(index == 16){
			index = 0;
            mv_g.push_back(temp);
            memset(&temp, 0, sizeof(PC_pointMeta_st));
		}
	}
	//csvfile.close();
}



void* udp_pc_msg_send_once(void* )
 {
     int ret;
    struct sockaddr_in ser_addr; 
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(UDP_IP);
    //ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    ser_addr.sin_port = htons(8002);  //端口号，需要网络序转换

    socklen_t len;
    struct sockaddr_in src;
    len = sizeof(sockaddr);

    UDP_PC_package_st sendMsg;
    memset(&sendMsg, 0, sizeof(sendMsg));
    static long long index = 0;
    Save2filecsv();
    long long index_frame = 0;
    while(!ifPCstop)
    {
        //memset(buf, 0, 1024);
        //recvfrom(udpRecvSocketFd_, buf, 1024, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        //printf("client send is :%s\n",msg.c_str());
        //std::cout << "msg is " << msg << std::endl
        sendMsg.UDP_PC_head.uphPrefix  = 0xEEFF;
        sendMsg.UDP_PC_head.uphVersion = 0x0102;
        sendMsg.UDP_PC_head.uphTimeLsb = 0;
        sendMsg.UDP_PC_head.uphTimeMsb = 0;
        sendMsg.UDP_PC_head.uphPayloadLength = 1400; // 每个UDP报文，点云元数据大小（1400=14*100）
        sendMsg.UDP_PC_head.uphFrameCounter = index_frame++;
        sendMsg.UDP_PC_head.uphRollingCounter = 0;
        sendMsg.UDP_PC_head.uphState = 0;
        sendMsg.UDP_PC_head.uphHeaderCrc = 0;
        sendMsg.UDP_PC_head.uphPayloadCrc = 0;
        std::cout << "mv_g size is " << mv_g.size() << std::endl;
        //memset(&sendMsg, 0, sizeof(sendMsg));
        if(index > 3900) index = 0;
        for(int i = index; i < index + 200; i++){
            //memset(&sendMsg, 0, sizeof(sendMsg));
#if 1
            for(int j = 0; j < 100; j++){
                sendMsg.UDP_PC_payload[j].pcmIndensity = mv_g[i * 100 + j].pcmIndensity;
                sendMsg.UDP_PC_payload[j].pcmDistance = mv_g[i * 100 + j].pcmDistance;
                sendMsg.UDP_PC_payload[j].pcmSpeed = mv_g[i * 100 + j].pcmSpeed;
                sendMsg.UDP_PC_payload[j].pcmVertical = mv_g[i * 100 + j].pcmVertical;
                sendMsg.UDP_PC_payload[j].pcmHorizontal = mv_g[i * 100 + j].pcmHorizontal;

            }
#endif
            int nnn = sendto(udpPcRecvSocketFd_, &sendMsg, sizeof(sendMsg), 0, (struct sockaddr*)&ser_addr, len);
            //int nnn = sendto(udpPcRecvSocketFd_, encode_cali_data + i * 1424, 1424, 0, (struct sockaddr*)&ser_addr, len);
            usleep(100);  //一秒发送一次消息
        }
        index += 200;
    }
 }


void *udppc_msg_sender(void *)
 {
     int ret;
    struct sockaddr_in ser_addr; 

    udpPcRecvSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(udpPcRecvSocketFd_ < 0)
    {
        printf("create udpRecvSocketFd fail!");
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(UDP_IP);
    //ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    ser_addr.sin_port = htons(8001);  //端口号，需要网络序转换

#if 0

    ret = bind(udpRecvSocketFd_, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        printf("socket bind fail!\n");
    }
#endif

    socklen_t len;
    struct sockaddr_in src;
    len = sizeof(sockaddr);

    UDP_PC_package_st sendMsg;
    memset(&sendMsg, 0, sizeof(sendMsg));
    long long index = 0;
    while(!ifPCstop)
    {
        //memset(buf, 0, 1024);
        //recvfrom(udpRecvSocketFd_, buf, 1024, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        //printf("client send is :%s\n",msg.c_str());
        //std::cout << "msg is " << msg << std::endl;
        sendMsg.UDP_PC_head.uphFrameCounter = index++;
        memset(&sendMsg, 0, sizeof(sendMsg));
        for(int i = 0; i < 200; i++){
            for(int j = 0; j < 100; j++){
                sendMsg.UDP_PC_payload[j].pcmDistance = (j + i + index % 100) * 65536;
                sendMsg.UDP_PC_payload[j].pcmIndensity = 32768 + j + i;
                if(j % 2){
                    sendMsg.UDP_PC_payload[j].pcmHorizontal = 30 * 32000 / 720;
                    sendMsg.UDP_PC_payload[j].pcmVertical = 0;
                    sendMsg.UDP_PC_payload[j].pcmSpeed = (10.2) * 128;
                }
                else{
                    sendMsg.UDP_PC_payload[j].pcmHorizontal = 330 * 32000 / 720;
                    sendMsg.UDP_PC_payload[j].pcmVertical = 5 * 256;
                    sendMsg.UDP_PC_payload[j].pcmSpeed = (-10.2) * 128;

                }

            }
            int nnn = sendto(udpPcRecvSocketFd_, &sendMsg, sizeof(sendMsg), 0, (struct sockaddr*)&ser_addr, len);
            usleep(500);  //一秒发送一次消息
        }
#if 1
#endif
    }
 }

double fft2dBm(double x){
	double inputV = (x / pow(2, 4.5) + 22) / 7.048;
	//double res = 10 * log10(20 * pow((inputV / 4000 / sqrt(2)), 2));
	double res = 10 * log10(20 * (inputV / 4000 / sqrt(2)) * (inputV / 4000 / sqrt(2)));
	return res; 
}
using namespace std;

void FloatToChar(float fNum, unsigned char *strBuf, int nLen) 
{
  if (nLen < 4)
    return;
  int i = 0;
  unsigned char *p = (unsigned char *)&fNum;
  for (i = 0; i < 4; i++) {
    strBuf[i] = *p;
    p++;
  }
}

float CharToFloat(unsigned char *strBuf, int nLen) 
{
  if (nLen < 4)
    return 0;
  int i = 0;
  float fNum;
  unsigned char *p = (unsigned char *)&fNum;
  for (i = 0; i < 4; i++) {
    *p = strBuf[i];
    p++;
  }
  return fNum;
}

void func(int a, int b , int c ,int d){
    printf("a is %d, b is %d, c is %d, d is %d\n ", a, b, c, d);
}

void testFile()
{
	std::ofstream csvfile; 
	csvfile.open("./testFile.csv", std::ios::out); 
	csvfile << "intensity" << "," << "distance(m)" << "," << \
				"speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)"; 
    for(int i = 0; i < 3; i++){
        csvfile << "," << "," << "intensity" << "," << "distance(m)" << \ 
                ","  << "speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)"; 
    }
	csvfile << "\n";    
}
int main(int argc, char** argv) 
{ 
    pthread_t udp_send;
    pthread_t udp_PC_send;

    const char *cali_file_path = "/home/encheng/data/data_test_raw_index0.bin";
    int  filesize = LoadDat(cali_file_path);
    std::chrono::duration<double> elapsed;
    auto start = std::chrono::steady_clock::now();
    float data_float = -8.888;
    float data_float_test;
    float data_float_out = 6.00;
    uint8_t data_u8_test[4];
    auto newfun = std::bind(func,  placeholders::_2, 2, 3,  placeholders::_1);
    newfun(1, 4);
    testFile();
/*     data_u8_test[0] = data_float & 0xff;
    data_u8_test[1] = (data_float & 0xff00) >> 8;
    data_u8_test[2] = (data_float & 0xff0000) >> 16;
    data_u8_test[3] = (data_float & 0xff000000) >> 24; */
    memcpy(data_u8_test, &data_float, 4);
    memcpy(&data_float_test, data_u8_test, 4);
    int flag = 7 / 4;
    std::cout << "test data is " <<  flag << std::endl;


    uint8_t data_u8[4];
    FloatToChar(data_float, data_u8, 4);
    std::cout << "data_float_out is " <<  CharToFloat(data_u8, 4) << std::endl;

    auto end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "time for fft2dBm: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
    string test_str = "123456\t\n";
    cout << "test_str size is " << test_str.size() << endl;
    cout << test_str;
    cout << "test_str" << endl;
    double angle = 389.2;
    angle -= 360.0;
    cout << "angle is " << angle << endl;
    int listenfd, connfd; 
    commandMsg msg;
    struct sockaddr_in servaddr; 
    char buff[4096]; int n; 
#if 1
    pthread_t motor_send;
    //pthread_t motor_init;
    //pthread_create(&motor_init, NULL, motorSockerInit, NULL);
    pthread_create(&motor_send, NULL, motor_msg_sender, NULL);

    pcSockerInit();
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    { 
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0); 
    } 
    int one = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    //fcntl(listenfd, F_SETFL, O_NONBLOCK);
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);             
    servaddr.sin_port = htons(6666); 
    long long index_0 = 0;
    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    { 
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0); 
    } 

    if( listen(listenfd, 10) == -1)
    { 
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0); 
    } 
    int para;
    //std::string node_name = std::string("/my_record_node");
    printf("======waiting for client's request======\n"); 
    while(1)
    { 
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1)
        { 
            printf("accept socket error: %s(errno: %d)\n",strerror(errno),errno); 
            usleep(100);
            continue; 
        } else {
            break;
        }
    }
    printf("======receive data======\n"); 
	int nRecvBuf= 320 * 1024;//设置为32K
	setsockopt(connfd, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	int nSendBuf= 320 * 1024;//设置为32K
	setsockopt(connfd, SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
#endif
   // motorSockerInit();
    ros::init(argc, argv, "talker");
    ros::NodeHandle roshandle;
    ros::V_string v_nodes;
    ros::master::getNodes(v_nodes);
    std::string myNode = "/record";
    for(int i  = 0; i < v_nodes.size(); i++)
    {
        std::cout << "v_nodes :" << v_nodes[i] << std::endl;
    }
    auto it = std::find(v_nodes.begin(), v_nodes.end(), myNode.c_str());

    std::cout << "myNode :" << myNode << std::endl;

    if(it != v_nodes.end())
    {
        std::cout << "myNode :" << myNode << std::endl;
    }


    //pthread_join(motor_send, NULL);

#if 1
    while(1)
    {
        memset(&msg, 0,  sizeof(msg)); 
        n = recv(connfd, &msg, sizeof(msg), MSG_WAITALL); 
        printf("buffer len is %d\n", n); 
        if(n == 0){
            close(connfd); 
            connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
            continue;
        }
        switch (msg.mHead.usCommand)
        {
        case POWER_READ:
        case CFAR_READ:
        case DFT3_READ:
        case DIFF_READ:
        case REG_READ:
            write(connfd, &msg, sizeof(msg));
            break;   
        case PC_READ:
            while(1){
                //cout << "send data times: " << index_0++ << endl;
                if(loopsend(connfd, filesize, encode_cali_data))
                    return -1;
            }
            break;
        case FFT_ADC_READ_START:
        	pthread_create(&udp_send, NULL, udp_msg_sender, NULL);
            break;       
        case FFT_ADC_READ_STOP:
            close(udpRecvSocketFd_);
            ifstop = true;
            break;
        case POINTCLOUD_DISPLAY_START:
        	pthread_create(&udp_PC_send, NULL, udp_pc_msg_send_once, NULL);
            //udp_pc_msg_send_once();
            break;       
        case POINTCLOUD_DISPLAY_STOP:
            close(udpPcRecvSocketFd_);
            ifPCstop = true;
            break;
        default:
            break;
        }

        if(ifPCstop) break;
        //buff[n] = '\0'; 
        printf("msg.mCommandVal from client: %d\n", msg.mCommandVal[1]); 
    } 
    close(connfd); 
    close(listenfd); 
#endif
    return 0;
}