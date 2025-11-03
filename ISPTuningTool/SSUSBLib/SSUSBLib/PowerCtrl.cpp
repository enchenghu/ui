#include "stdafx.h"
#include "PowerCtrl.h"
#include <stdio.h>
#include "USBOpt.h"

static sVoltageSetting voltageList[53] = { 0 };
static sPowerRail AVDD;
static sPowerRail DOVDD;
static sPowerRail DVDD;
static sPowerRail AFVCC;

static int SetExportVoltage(HANDLE hDev, sPowerRail* port, sVoltageSetting* pSetting);
static int EnableExportVoltage(HANDLE hDev, sPowerRail* port, BOOL enable);

int checkVoltageList()
{
	int i;
	unsigned int test = 500;
	for (i = 0; i < VOLTAGESETTINGNUM; i++)
	{
		test = 500;

		if (voltageList[i].out50mV == 0)
		{
			test += 50;
		}

		if (voltageList[i].out100mV == 0)
		{
			test += 100;
		}

		if (voltageList[i].out200mV == 0)
		{
			test += 200;
		}

		if (voltageList[i].out400mV == 0)
		{
			test += 400;
		}

		if (voltageList[i].out800mV == 0)
		{
			test += 800;
		}

		if (voltageList[i].out1600mV == 0)
		{
			test += 1600;
		}

		if (test != voltageList[i].targetVoltage)
		{
// 			printf("Check error[%d],test = %d, targetVoltage = %d\r\n", i, test, voltageList[i].targetVoltage);
		}
	}

	return 0;
}

void PowerCtrlInitVoltageList()
{
	int i;
	int gap50mV = 1;
	int gap100mV = 2;
	int gap200mV = 4;
	int gap400mV = 8;

	unsigned char flag_out50mV = PIN_GROUNGING;
	unsigned char flag_out100mV = PIN_GROUNGING;
	unsigned char flag_out200mV = PIN_GROUNGING;
	unsigned char flag_out400mV = PIN_FLOATING;
	unsigned char flag_out800mV = PIN_FLOATING;
	unsigned char flag_out1600mV = PIN_FLOATING;

	for (i = 0; i < VOLTAGESETTINGNUM; i++)
	{

		if (flag_out50mV == PIN_FLOATING)
		{
			flag_out50mV = PIN_GROUNGING;
		}
		else
		{
			flag_out50mV = PIN_FLOATING;
		}

		if (i % 2 == 0)
		{
			if (flag_out100mV == PIN_FLOATING)
			{
				flag_out100mV = PIN_GROUNGING;
			}
			else
			{
				flag_out100mV = PIN_FLOATING;
			}
		}

		if (i % 4 == 0)
		{
			if (flag_out200mV == PIN_FLOATING)
			{
				flag_out200mV = PIN_GROUNGING;
			}
			else
			{
				flag_out200mV = PIN_FLOATING;
			}
		}

		if (i % 8 == 0)
		{
			if (flag_out400mV == PIN_FLOATING)
			{
				flag_out400mV = PIN_GROUNGING;
			}
			else
			{
				flag_out400mV = PIN_FLOATING;
			}
		}


		if (i > 39)
		{
			flag_out800mV = PIN_GROUNGING;
			flag_out1600mV = PIN_GROUNGING;
		}
		else if (i > 23)
		{
			flag_out800mV = PIN_FLOATING;
			flag_out1600mV = PIN_GROUNGING;
		}
		else if (i > 7)
		{
			flag_out800mV = PIN_GROUNGING;
			flag_out1600mV = PIN_FLOATING;
		}
		else
		{
			flag_out800mV = PIN_FLOATING;
			flag_out1600mV = PIN_FLOATING;
		}

		voltageList[i].targetVoltage = 900 + 50 * i;
		voltageList[i].out50mV = flag_out50mV;
		voltageList[i].out100mV = flag_out100mV;
		voltageList[i].out200mV = flag_out200mV;
		voltageList[i].out400mV = flag_out400mV;
		voltageList[i].out800mV = flag_out800mV;
		voltageList[i].out1600mV = flag_out1600mV;
	}
	//checkVoltageList();
}

void PowerCtrlInitPowerPort(int board)
{
	if (board == 2)
	{
		AVDD.Pin50mV = PORT1_0;
		AVDD.Pin100mV = PORT1_1;
		AVDD.Pin200mV = PORT1_2;
		AVDD.Pin400mV = PORT1_3;
		AVDD.Pin800mV = PORT1_4;
		AVDD.Pin1600mV = PORT1_5;
		AVDD.PinEnable = PORT1_6;

		DOVDD.Pin50mV = PORT2_0;
		DOVDD.Pin100mV = PORT2_1;
		DOVDD.Pin200mV = PORT2_2;
		DOVDD.Pin400mV = PORT2_3;
		DOVDD.Pin800mV = PORT2_4;
		DOVDD.Pin1600mV = PORT2_5;
		DOVDD.PinEnable = PORT2_6;

		DVDD.Pin50mV = PORT3_0;
		DVDD.Pin100mV = PORT3_1;
		DVDD.Pin200mV = PORT3_2;
		DVDD.Pin400mV = PORT3_3;
		DVDD.Pin800mV = PORT3_4;
		DVDD.Pin1600mV = PORT3_5;
		DVDD.PinEnable = PORT3_6;

		AFVCC.Pin50mV = PORT0_0;
		AFVCC.Pin100mV = PORT0_1;
		AFVCC.Pin200mV = PORT0_2;
		AFVCC.Pin400mV = PORT0_3;
		AFVCC.Pin800mV = PORT0_4;
		AFVCC.Pin1600mV = PORT0_5;
		AFVCC.PinEnable = PORT0_6;
	}
	else
	{
		AVDD.Pin50mV = PORT2_6;
		AVDD.Pin100mV = PORT2_7;
		AVDD.Pin200mV = PORT3_0;
		AVDD.Pin400mV = PORT2_0;
		AVDD.Pin800mV = PORT2_1;
		AVDD.Pin1600mV = PORT2_2;
		AVDD.PinEnable = PORT2_3;

		DOVDD.Pin50mV = PORT3_6;
		DOVDD.Pin100mV = PORT3_5;
		DOVDD.Pin200mV = PORT3_4;
		DOVDD.Pin400mV = PORT3_3;
		DOVDD.Pin800mV = PORT3_2;
		DOVDD.Pin1600mV = PORT3_1;
		DOVDD.PinEnable = PORT2_4;

		DVDD.Pin50mV = PORT4_6;
		DVDD.Pin100mV = PORT4_5;
		DVDD.Pin200mV = PORT4_4;
		DVDD.Pin400mV = PORT4_3;
		DVDD.Pin800mV = PORT4_2;
		DVDD.Pin1600mV = PORT4_1;
		DVDD.PinEnable = PORT4_0;

		AFVCC.Pin50mV = PORT1_6;
		AFVCC.Pin100mV = PORT1_5;
		AFVCC.Pin200mV = PORT1_4;
		AFVCC.Pin400mV = PORT1_3;
		AFVCC.Pin800mV = PORT1_2;
		AFVCC.Pin1600mV = PORT1_1;
		AFVCC.PinEnable = PORT1_0;
	}
	
}

int PowerCtrlEnablePower(HANDLE hDev, uint32_t powerRail, BOOL enable)
{

	if (powerRail >= IOEXPAND_VOLTAGE_MAX)
	{
		return -1;
	}

	switch (powerRail)
	{
	case IOEXPAND_VOLTAGE_AVDD:
	{
		EnableExportVoltage(hDev, &AVDD, enable);
		break;
	}
	case IOEXPAND_VOLTAGE_DOVDD:
	{
		EnableExportVoltage(hDev, &DOVDD, enable);
		break;
	}
	case IOEXPAND_VOLTAGE_DVDD:
	{
		EnableExportVoltage(hDev, &DVDD, enable);
		break;
	}
	case IOEXPAND_VOLTAGE_AFVCC:
	{
		EnableExportVoltage(hDev, &AFVCC, enable);
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int PowerCtrlSetVoltage(HANDLE hDev, uint32_t powerRail, uint32_t voltage)
{
	unsigned int voltageValue = 0;
	int i;

	if (powerRail >= IOEXPAND_VOLTAGE_MAX)
	{
		return -1;
	}

	if ((voltage < 900) || (voltage > 3500))
	{
		return -2;
	}

	for (i = 0; i < VOLTAGESETTINGNUM; i++)
	{
		if (voltage <= voltageList[i].targetVoltage)
		{
			break;
		}
	}

	switch (powerRail)
	{
	case IOEXPAND_VOLTAGE_AVDD:
	{
		SetExportVoltage(hDev, &AVDD, &voltageList[i]);
		break;
	}
	case IOEXPAND_VOLTAGE_DOVDD:
	{
		SetExportVoltage(hDev, &DOVDD, &voltageList[i]);
		break;
	}
	case IOEXPAND_VOLTAGE_DVDD:
	{
		SetExportVoltage(hDev, &DVDD, &voltageList[i]);
		break;
	}
	case IOEXPAND_VOLTAGE_AFVCC:
	{
		SetExportVoltage(hDev, &AFVCC, &voltageList[i]);
		break;
	}
	default:
		break;
	}
	return 0;
}


static int SetExportVoltage(HANDLE hDev, sPowerRail* port, sVoltageSetting* pSetting)
{
	if ((port == NULL) || (pSetting == NULL) || (hDev == NULL))
	{
		return -1;
	}

	if (pSetting->out50mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin50mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin50mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin50mV, IOEXPAND_CFG_INPUT);
	}

	if (pSetting->out100mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin100mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin100mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin100mV, IOEXPAND_CFG_INPUT);
	}

	if (pSetting->out200mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin200mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin200mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin200mV, IOEXPAND_CFG_INPUT);
	}

	if (pSetting->out400mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin400mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin400mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin400mV, IOEXPAND_CFG_INPUT);
	}

	if (pSetting->out800mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin800mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin800mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin800mV, IOEXPAND_CFG_INPUT);
	}

	if (pSetting->out1600mV == PIN_GROUNGING)
	{
		USBSetIOExpCfg(hDev, port->Pin1600mV, IOEXPAND_CFG_OUTPUT);
		USBSetIOExpValue(hDev, port->Pin1600mV, IOEXPAND_OUTPUT_LOW);
	}
	else
	{
		USBSetIOExpCfg(hDev, port->Pin1600mV, IOEXPAND_CFG_INPUT);
	}

	USBSetIOExpCfg(hDev, port->PinEnable, IOEXPAND_CFG_OUTPUT);

	return 0;
}

static int EnableExportVoltage(HANDLE hDev, sPowerRail* port, BOOL enable)
{
	if (port == NULL)
	{
		return -1;
	}

	if (enable)
	{
		USBSetIOExpValue(hDev, port->PinEnable, IOEXPAND_OUTPUT_HIGH);
	}
	else
	{
		USBSetIOExpValue(hDev, port->PinEnable, IOEXPAND_OUTPUT_LOW);
	}

	return 0;
}

