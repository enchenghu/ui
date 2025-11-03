#include "qcameratype.h"

CameraType *CameraType::_instance = 0;

CameraType *CameraType::instance()
{
	if (_instance == 0)
	{
		_instance = new CameraType();
	}
	return _instance;
}

int CameraType::getCurrentCamera()
{
	return cameraID;
}

void CameraType::setCurrentCamera(int type)
{
	cameraID = type;
        emit updateCamera();
}

void CameraType::setCameraAddress(int cam0Addr, int cam1Addr)
{
	camera0Addr = cam0Addr;
	camera1Addr = cam1Addr;
}

CameraType::CameraType(QObject *parent)
	: QObject(parent)
{
	camera0Addr = QString("0x30080eb0").toInt(nullptr, 16);
	camera1Addr = QString("0x300822b0").toInt(nullptr, 16);
}
