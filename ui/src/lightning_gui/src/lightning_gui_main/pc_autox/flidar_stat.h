#ifndef _FLIDAR_STAT_H_
#define _FLIDAR_STAT_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <vector>
#include <string>
/* #include "debug.h"
#include "flidar_queue.h"
#include "flidar_socket.h" */

static std::vector<std::string> edfaDevName = {"硬件版本号", "软件版本号"};
static std::vector<std::string> edfaStateName = {"光源状态", "输入功率/uW", "输出功率/mW", "模块温度/℃", "第一个泵浦温度/℃", "第一个泵浦电流/mA", "第二个泵浦电流/mA"};
static std::vector<std::string> edfaWarnName = {"设备在线", "模块温度", "种子激光器温度", "输入功率", "泵浦温度","泵浦功率", "种子激光器功率", "保存数据"};

#pragma pack(1)     // 1字节对齐
typedef enum {
    sm_opera_r,     // 读
    sm_opera_w,     // 写
} flidar_sm_opera_rw;

typedef struct {
    uint16_t frameHead;     // 起始头：0xAA 0x55
    uint32_t timeStamp;     // 时间戳：高22位:秒，低10位:ms
    uint8_t  validPeriod;   // 消息有效期（单位：100ms）
    uint8_t  countToken;    // 消息帧计数（每次+1）
    uint8_t  moduleCode;    // 模块代码
    uint8_t  moduleCmd;     // 模块指令
    uint8_t  dataLen;       // dataBody 长度
    uint8_t *dataBody;      // 消息帧数据体
    uint8_t  crcSum;        // 除头外所有数据的累加和
} flidar_sm_msgFrame_st;

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

typedef struct {
    uint8_t hardVer[2];     // 硬件版本号：0x01 0x02：V1.2
    uint8_t softVer[3];     // 软件版本号：0x01 0x02 0x03：V1.23
} flidar_sm_EDFA_base_st;

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

#define LEN_SM_SEND_ATOM    12  // 状态监测消息长度：12+N bytes
#define LEN_SM_EDFA_BASE    (sizeof(flidar_sm_EDFA_base_st))    // 5   // EDFA 基本信息
#define LEN_SM_EDFA_STAT    (sizeof(flidar_sm_EDFA_stat_st))    // 13  // EDFA 状态信息
#define LEN_SM_EDFA_WARN    (sizeof(flidar_sm_EDFA_warn_st))    // 7   // EDFA 状态信息


#pragma pack()

#endif  /* _FLIDAR_STAT_H_ */