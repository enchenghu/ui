#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    mID_d2h_state       = 0,    // dev->hui：设备状态上报（定时）
    mID_d2h_linearity   = 1,    // dev->hui：上传线性度
    mID_d2h_waveform    = 2,    // dev->hui：上传波形文件
    mID_h2d_DACen       = 100,  // hui->dev：控制DAC使能
    mID_h2d_waveform    = 101,  // hui->dev：下发波形文件
    mID_h2d_setTemp     = 102,  // hui->dev：下发虚拟温度
} mID_em;

#pragma pack(1)
typedef struct {
    uint32_t tv_sec;    // s(1970+)
    uint32_t tv_nsec;   // ns
} mTimespec_st;

typedef struct {
    uint8_t  ocDirection;     // 消息方向 // 0:dev->hui
    uint8_t  ocMsgType;     // 消息类型 // 0:Command
    uint16_t ocMsgFlag;    // 消息标志
    mID_em  ocMsgID;     // 消息ID
} brst_mOpCode;

typedef struct {
    uint16_t     mFrameHead;    // 0xAA 0x55
    uint16_t     mModuleID;     // 0:fmcw-lidar  1:illuminant
    mTimespec_st mTimespec;     // s(4Byte) + ms(4Byte)
    uint16_t     mTimeout;      // ms
    uint32_t     mFrameCnt;     //
    brst_mOpCode mOpCode;       // 
    uint32_t     mPaySize;      // 载荷大小
} msg_frame_header;

typedef struct {
    msg_frame_header mHeader;
    uint8_t     *mPayload;      // 载荷数据
    uint16_t     mCheckSum;     // 校验和：除帧头的累加和
} msg_frame_st;
#pragma pack()


