#ifndef _POWERCTRL_H
#define _POWERCTRL_H

#include "stdafx.h"

#include <stdint.h>

#define PIN_FLOATING (1)
#define PIN_GROUNGING (0)

#define VOLTAGESETTINGNUM (53)

typedef struct
{
	unsigned int targetVoltage;

	unsigned char out50mV;
	unsigned char out100mV;
	unsigned char out200mV;
	unsigned char out400mV;
	unsigned char out800mV;
	unsigned char out1600mV;
}sVoltageSetting;

enum IOEXPAND_VOLTAGE_RAIL
{
	IOEXPAND_VOLTAGE_AVDD,
	IOEXPAND_VOLTAGE_DOVDD,
	IOEXPAND_VOLTAGE_DVDD,
	IOEXPAND_VOLTAGE_AFVCC,
	IOEXPAND_VOLTAGE_MAX
};

typedef struct
{
	uint32_t PinEnable;

	uint32_t Pin50mV;
	uint32_t Pin100mV;
	uint32_t Pin200mV;
	uint32_t Pin400mV;
	uint32_t Pin800mV;
	uint32_t Pin1600mV;
}sPowerRail;

void PowerCtrlInitVoltageList();
void PowerCtrlInitPowerPort(int board);
int PowerCtrlSetVoltage(HANDLE hDev, uint32_t rail, uint32_t voltage);
int PowerCtrlEnablePower(HANDLE hDev, uint32_t powerRail, BOOL enable);

#endif // _POWERCTRL_H