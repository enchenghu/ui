#ifndef QCAMERATYPE_H
#define QCAMERATYPE_H

#include <QObject>

class CameraType : public QObject
{
	Q_OBJECT
public:
	static CameraType *instance();
	int                getCurrentCamera();
	void               setCurrentCamera(int type);
	void               setCameraAddress(int cam0Addr, int cam1Addr);
signals:
        void updateCamera();

private:
	explicit CameraType(QObject *parent = 0);

private:
	static CameraType *_instance;
	int                cameraID = 0;
	uint32_t           camera0Addr;
	uint32_t           camera1Addr;
};

#endif // QCAMERATYPE_H
