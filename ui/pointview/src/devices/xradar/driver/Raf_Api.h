/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#pragma once

#include <cstdint>
#include <cstring>

#include "driver/ArbeMailBox.h"
#include "driver/CommonBase.h"

namespace autox {
namespace drivers {
namespace arbe {

// should be incremented in every change of RawMetadataHeader or
// RawMetadataFrame
const uint8_t RAW_METADATA_VERSION = 0x1;

/*######################### ENUMERATORS ###############################*/

typedef enum {
  Cmd_None = 0x1000,
  SetTime,
  KeepAlive,
  Status,
  Start_Tx,
  Stop_Tx,
  RecordFrameRawData,
  ConfigurePointCloud,
  SetThresholds,
  SelectActiveSeq,
  RficOperation_Input,
  MemoryOperation_Input,
  DebugOperation_Input,
  VaractorTable,
  SetHistogram,
  ConfigureInjectPC,
  ConfigureRDrecording,
  ConfigureCalibrationFrame,
  GetNxPool,
  FreeNxPool,
  DataByReference,
  LastCmdType
} EInCmdType;

typedef enum {
  etResponseTypeUnknown = -1,
  etResponseTypeRaw,
  etResponseTypeStatus,
  etResponseTypePointCloud,
  etResponseTypeRegister,
} ResponseType;

typedef enum {
  WriteMemory,
  ReadMemory,
  WriteRegister,
  ReadRegister,
  WriteAutoMemory,
  ReadAutoMemory,
} EMemoryOperation;

typedef enum {
  NONE,
  TRACE,
  TRACE_PREVIOUS,
  SMART,
  PC_INJECT,
  RD_PAIRS,
} ESpecialOpcode;

typedef enum {
  SetStaticThresholdCoarseAndFine = 1,
  SetDynamicAzimuteThresholdCoarseFine,
  SetDynamicElevationThresholdFine,
  SetStaticAndDynamicThresholds
} EThresholdOpcodes;

typedef enum {
  Arbe_Tx,
  Arbe_Rx,
  Arbe_Adc,
  Arbe_AntAdc,
  Arbe_Memory
} EArbePeripheralDevice;

typedef enum {
  Ptat,
  Pll_Ptat,
  Vcnt,
  Vtune,
  SupplyMonitor
} ETX_VoltageMeasurement;

typedef enum {
  Vdd_Synth,
  Vdd_0V9Lo,
  Vss1,
  Vss2,
  Vss3,
  Vddd2,
  Vddd,
  Vdda,
  Vdd_1V5bb,
  Vdd1V5Bgr,
  Vdd_HVco,
  Vdd_HPll,
  Vdd_3VPa,
  Vss4,
  Vss5,
  Vss6,
} ESupplyMonitorVoltage;

typedef enum {
  TxInitTest,
  RxInitTest,
  RxCommulativeCurrentTest,
  TxTransmitTest,
  PllCalTest_Freq,
  PllCalTest_Var,
  AnechoicChamberRTS,
  HistogramTest
} EDebugOperation;

typedef enum {
  ConfigureCw,
  ConfigurePhaseShifter,
  TemperatureMeasurement,
} ERficOperation;

typedef enum RadarError_e {
  OK,
  MEM_OVERFLOW,
  MEM_ALLOC,
  CRC,
} RadarError;

typedef enum {
  HistogramTypeRange = 0,
  HistogramTypeAzimuth,
  HistogramTypeLast
} EHistogramType;

typedef enum {
  HistogramDisable,
  HistogramEnableOneShot,
  HistogramEnablePeriodic
} EHistogramOpr;

/*###########################################################################*/

/*######################### BASE STRUCTURES ###############################*/
typedef struct {
  uint32_t unReserved;
} TStartTxInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TStartTxInfo tStartTxInfo;
} TStartTx;

typedef struct {
  uint32_t unReserved;
} TStopInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TStopInfo tStopTxInfo;
} TStopTx;

typedef struct {
  uint32_t unInitateTimeLsb;
  uint32_t unInitateTimeMsb;
} TSetTimeInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TSetTimeInfo tSetTimeInfo;
} TSetTime;

typedef struct {
  EHistogramOpr eHistogramOpr;
  uint32_t unFrameTypesMask;
  uint32_t unThrConfigAddress;
} TSetHistogramInfo;

typedef struct {
  TRAF_API_Header tHeader;
  TSetHistogramInfo tSetHistogramInfo[HistogramTypeLast];
} TSetHistogram;

typedef struct {
  ESeuqenceType eSequenceType;
  uint32_t unRangeOutput;
  uint32_t unDestinationBufferSize;
  uint8_t* pucDestinationAddress;
} TRecordFrameRawDataInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TRecordFrameRawDataInfo tRecordFrameRawDataInfo;
} TRecordFrameRawData;

typedef struct {
  uint32_t unDestinationBufferSize;
  uint32_t unByteAlignment;
  uint32_t unPacketFormat;
  uint8_t* pucDestinationAddress;
} TConfigurePointCloudsInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TConfigurePointCloudsInfo tConfigurePointCloudsInfo;
} TConfigurePointClouds;

typedef struct {
  EThresholdOpcodes opcode;
  uint32_t unParam1;
  uint32_t unParam2;
  uint32_t unParam3;
  uint32_t unParam4;
} TSetThresholdsInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TSetThresholdsInfo tSetThresholdsInfo;
} TSetThresholds;

typedef struct {
  EStatusOpcode opcode;
  uint32_t unParam1;
  uint32_t unParam2;
  uint32_t unParam3;
  uint32_t unParam4;
} TStatusInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TStatusInfo tStatusInfo;
} TStatus;

typedef struct {
  uint32_t unResereved;
} TVarctorTableInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TVarctorTableInfo tVarctorTableInfo;
} TVarctorTable;

typedef struct {
  ERficOperation eRficOperation;
  EArbePeripheralDevice ePeripheralDevice;
  uint32_t unChipID;
  uint32_t unParam1;
  uint32_t unParam2;
  uint32_t unParam3;
  uint32_t unParam4;
  uint32_t unParam5;
  uint32_t unParam6;
} TRficOperationInfo;

typedef struct {
  TRAF_API_Header tHeader;
  TRficOperationInfo tRficOperationInfo;
} TRficOperation;

typedef struct {
  ESeuqenceType eSequenceType;
} TSelectActiveSeqInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TSelectActiveSeqInfo tSelectActiveSeqInfo;
} TSelectActiveSeq;

typedef struct {
  EArbePeripheralDevice ePeripheralDevice;
  uint32_t unChipID;
  EMemoryOperation eMemoryOperation;
  uint32_t unAddress;
  uint32_t unValueOrTotalLength;
  ESpecialOpcode eSpecialOpcode;
} TMemoryOperationInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TMemoryOperationInfo tMemoryOperationInfo;
} TMemoryOperation;

typedef struct {
  EDebugOperation eDebugOperation;
  uint32_t unParam1;
  uint32_t unParam2;
  uint32_t unParam3;
  uint32_t unParam4;
} TDebugOperationInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TDebugOperationInfo tDebugOperationInfo;
} TDebugOperation;

typedef struct {
  uint32_t unEnable;
  uint32_t unParam1;
  uint32_t unParam2;
} TConfigureInjectPCInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TConfigureInjectPCInfo tConfigureInjectPCInfo;
} TConfigureInjectPC;

typedef struct {
  uint32_t unFrameType;
  uint32_t unRangeBin;
  uint32_t unDopplerBin;
  uint32_t unZoom;
} TConfigureRDrecordingInfo;

typedef struct {
  TRAF_API_Header tHeader;
  TConfigureRDrecordingInfo tConfigureDrecordingInfo;
} TConfigureRDrecording;


inline uint64_t LSB_TIME(uint64_t x) {
  return (x)&0x00000000ffffffff;
}
inline uint64_t MSB_TIME(uint64_t x) {
  return ((x)&0xffffffff00000000) >> 32;
}

void RAF_API_SysCfg_SetTime(TSetTimeInfo* SystemTime,
                                       ptArbeApiMailBox mailbox);
void RAF_API_RdrCtrl_StartTx(TStartTxInfo* info,
                                        ptArbeApiMailBox mailbox);

void RAF_API_RdrCtrl_StopTx(TStopInfo* info,
                                       ptArbeApiMailBox mailbox);

void RAF_API_RdrCtrl_SetThresholds(TSetThresholdsInfo* info,
                                              ptArbeApiMailBox mailbox);

void RAF_API_RdrCtrl_SetActiveSeq(TSelectActiveSeqInfo* info,
                                             ptArbeApiMailBox mailbox);

void BuildHeader(uint32_t unCmdType, uint32_t unMessageSize,
                 TRAF_API_Header* tHeader, ptArbeApiMailBox mailbox);

void SendCmdToRadar(uint8_t* buffer, unsigned int len, int radar_socket);

// User Extend
/*######################### ENUMERATION ###############################*/

typedef enum {
  External_Cmd_None = 0x2000,
  AtcMode,
  DspJob,
  Last_External_CmdType
} EExtendedCmdType;

/*######################### BASE STRUCTURES ###############################*/

typedef struct {
  uint32_t unEnableAtc;
  uint32_t unsensitivity;
} TAtcInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TAtcInfo tAtcInfo;
} TAtcMode;


void RAF_API_EXT_AtcMode(TAtcInfo* info, ptArbeApiMailBox mailbox);
}  // namespace arbe
}  // namespace drivers
}  // namespace autox
