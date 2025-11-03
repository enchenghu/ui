#include "SenPara.h"
#include "CMethod.h"
extern SensorTab gstCurrentSensor;
extern string gstrFileName;
extern float garyfVolBuf[5];
extern vector<STRegNode> gvecRegList;
SenPara::SenPara()
{
	
}

SenPara::~SenPara()
{
}

void SenPara::vGetSSUSBPortLane(int iPort, int iLane)
{
	switch (iPort)
	{
	case 0:
		senPort = SENSORINTERFACE_MIPI;
		break;
	case 1:
		senPort = SENSORINTERFACE_DVP;
		break;
	default:
		senPort = SENSORINTERFACE_LVDS;
		break;
	}
	if (senPort == SENSORINTERFACE_DVP)
	{
		switch (iLane)
		{
		case 0x0d:
			senLane = 1;
			break;
		case 0x0e:
			senLane = 2;
			break;
		default:
			senLane = 0;
			break;
		}
	}
	else
	{
		switch (iLane)
		{
		case 1:
			senLane = SENSORINTFLANE_1LANE;
			break;
		case 2:
			senLane = SENSORINTFLANE_2LANE;
			break;
		case 4:
			senLane = SENSORINTFLANE_4LANE;
			break;
		case 5:
			senLane = SENSORINTFLANE_4LANE_1;
			break;
		case 6:
			senLane = SENSORINTFLANE_4LANE_2;
			break;
		default:
			senLane = SENSORINTFLANE_8LANE;
			break;
		}
	}
}

void SenPara::vGetSSUSBDataType(int iType)
{
	switch (iType)
	{
	case 1:
	case D_RAW4_DDR:
	case D_RAW4_SDR:
		senDataType = SENSORDATAWIDTH_8BIT;
		break;
	case 6:
	case D_RAW10:
		senDataType = SENSORDATAWIDTH_10BIT;
		break;
	case D_RAW12:
		senDataType = SENSORDATAWIDTH_12BIT;
		break;
	case 9:
		senDataType = SENSORDATAWIDTH_14BIT;
		break;
	case 3:
		senDataType = SENSORDATAWIDTH_16BIT;
		break;
	case 2:
	case D_YUV_MTK_S:
		senDataType = SENSORDATAWIDTH_YUV422;
		break;
	case D_YUV422_RAW10:
		senDataType = SENSORDATAWIDTH_YUV422_10BIT;
		break;
	case D_YUV422_RAW10_NORMAL:
		senDataType = SENSORDATAWIDTH_YUV422_10BIT_NORMAL;
		break;
	case D_YUV422_RAW12_SPECIAL:
		senDataType = SENSORDATAWIDTH_YUV422_12BIT_SPECIAL;
		break;
	case 10:
		senDataType = SENSORDATAWIDTH_20BIT;
		break;
	case D_YUV422_RAW10_960:
		senDataType = SENSORDATAWIDTH_YUV422_10BIT_960MIPI;
		break;
	default:
		senDataType = SENSORDATAWIDTH_12BIT;
		break;
	}
}

void SenPara::vInitSenPara()
{
	OpenFileName();
	vGetSSUSBPortLane(gstCurrentSensor.port, gstCurrentSensor.lane);
	vGetSSUSBDataType(gstCurrentSensor.type);
	fVoltage[0] = gstCurrentSensor.avdd;
	fVoltage[1] = gstCurrentSensor.dovdd;
	fVoltage[2] = gstCurrentSensor.dvdd;
	fVoltage[3] = gstCurrentSensor.afvdd;
	fVoltage[4] = 0;
	delaytime[0] = gstCurrentSensor.avdddelay;
	delaytime[1] = gstCurrentSensor.dovdddelay;
	delaytime[3] = gstCurrentSensor.dvdddelay;
	delaytime[4] = 0;
	delaytime[5] = 0;
	mclk = gstCurrentSensor.mclk;
	mclken = gstCurrentSensor.mclken;
	width = gstCurrentSensor.width;
	height = gstCurrentSensor.height;
	pin = gstCurrentSensor.pin;
	SlaveID = gstCurrentSensor.SlaveID;
}

void SenPara::vSetSensorCfg(HANDLE hDev)
{
	int nRet = 0;
	nRet = SSUSB_SetSensorInterface(hDev, senPort, senLane);
	nRet = SSUSB_SetSensorBitWidthEx(hDev, senDataType, senDataType);
	nRet = SSUSB_SetSensorResolutionEx(hDev, width, height, width, height);
}
