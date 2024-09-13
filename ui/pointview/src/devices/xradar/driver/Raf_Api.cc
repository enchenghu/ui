// Copyright (C) 2019 Arbe - http://www.arberobotics.com/

#include "driver/Raf_Api.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cmath>
#include <cstdarg>
#include <cstdio>

#include "driver/Utils.h"
namespace autox {
namespace drivers {
namespace arbe {

void RAF_API_SysCfg_SetTime(TSetTimeInfo* info, ptArbeApiMailBox mailbox) {
  TSetTime tSetTime;
  int _size = sizeof(TSetTime);
  memset(&tSetTime, 0, _size);
  tSetTime.tSetTimeInfo = *info;
  BuildHeader(SetTime, _size, &tSetTime.tHeader, mailbox);
  SendCmdToRadar(reinterpret_cast<uint8_t*>(&tSetTime), _size,
                 mailbox->radar_socket);
}

void RAF_API_RdrCtrl_StartTx(TStartTxInfo* /*info*/, ptArbeApiMailBox mailbox) {
  TStartTx startCmd;

  int _size = sizeof(TStartTx);

  memset(&startCmd, 0, _size);

  BuildHeader(Start_Tx, _size, &startCmd.tHeader, mailbox);
  SendCmdToRadar(reinterpret_cast<uint8_t*>(&startCmd), _size,
                 mailbox->radar_socket);
}

void RAF_API_RdrCtrl_StopTx(TStopInfo* info, ptArbeApiMailBox mailbox) {
  TStopTx stopCmd;

  int _size = sizeof(TStopTx);

  memset(&stopCmd, 0, _size);

  BuildHeader(Stop_Tx, _size, &stopCmd.tHeader, mailbox);

  stopCmd.tStopTxInfo.unReserved = info->unReserved;
  SendCmdToRadar(reinterpret_cast<uint8_t*>(&stopCmd), _size,
                 mailbox->radar_socket);
}

void RAF_API_RdrCtrl_SetThresholds(TSetThresholdsInfo* info,
                                   ptArbeApiMailBox mailbox) {
  TSetThresholds thresholdsCmd;

  int _size = sizeof(TSetThresholds);

  memset(&thresholdsCmd, 0, _size);

  BuildHeader(SetThresholds, _size, &thresholdsCmd.tHeader, mailbox);

  thresholdsCmd.tSetThresholdsInfo.opcode = info->opcode;
  thresholdsCmd.tSetThresholdsInfo.unParam1 = info->unParam1;
  thresholdsCmd.tSetThresholdsInfo.unParam2 = info->unParam2;
  thresholdsCmd.tSetThresholdsInfo.unParam3 = info->unParam3;
  thresholdsCmd.tSetThresholdsInfo.unParam4 = info->unParam4;
  SendCmdToRadar(reinterpret_cast<uint8_t*>(&thresholdsCmd), _size,
                 mailbox->radar_socket);
}

void RAF_API_RdrCtrl_SetActiveSeq(TSelectActiveSeqInfo* info,
                                  ptArbeApiMailBox mailbox) {
  TSelectActiveSeq selectActiveSeqCmd;

  int _size = sizeof(selectActiveSeqCmd);

  memset(&selectActiveSeqCmd, 0, _size);

  BuildHeader(SelectActiveSeq, _size, &selectActiveSeqCmd.tHeader, mailbox);

  selectActiveSeqCmd.tSelectActiveSeqInfo.eSequenceType = info->eSequenceType;
  SendCmdToRadar(reinterpret_cast<uint8_t*>(&selectActiveSeqCmd), _size,
                 mailbox->radar_socket);
}

void BuildHeader(uint32_t unCmdType, uint32_t unMessageSize,
                 TRAF_API_Header* tHeader, ptArbeApiMailBox mailbox) {
  tHeader->usPrefix = PACKET_PREFIX;
  tHeader->unLength = unMessageSize;
  tHeader->usType = unCmdType;
  tHeader->unMessageNumber = mailbox->unMessageNumber++;
  uint64_t ulSystemTime = GetSystemTime();
  tHeader->unTimeLsb = (uint32_t)LSB_TIME(ulSystemTime);
  tHeader->unTimeMsb = (uint32_t)MSB_TIME(ulSystemTime);
}

void SendCmdToRadar(uint8_t* buffer, unsigned int len, int radar_socket) {
  if (write(radar_socket, buffer, len) < 0) {
    printf("Write fail!");
  }
}
// user extend
void RAF_API_EXT_AtcMode(TAtcInfo* info, ptArbeApiMailBox mailbox) {
  TAtcMode tAtcMode;

  int _size = sizeof(TAtcMode);

  memset(&tAtcMode, 0, _size);

  BuildHeader(AtcMode, _size, &tAtcMode.tHeader, mailbox);

  tAtcMode.tAtcInfo.unEnableAtc = info->unEnableAtc;
  tAtcMode.tAtcInfo.unsensitivity = info->unsensitivity;

  SendCmdToRadar(reinterpret_cast<uint8_t*>(&tAtcMode), _size,
                 mailbox->radar_socket);
}
}  // namespace arbe
}  // namespace drivers
}  // namespace autox
