#ifndef QUSBCOMMUNICATION_H
#define QUSBCOMMUNICATION_H

const int MAX_DEVICE_NUM     = 8;
const int DEVICE_NAME_LENGTH = 256;

enum USB_TYPE {
    USB_I2C = 0,
    USB_SPI,
};

enum Q_SPI_MODE {
    CPOL         = 1 << 0,
    CPHA         = 1 << 1,
    RW_EDGE_DIFF = 1 << 2,
};

class QUsbCommunication
{
public:
    static QUsbCommunication &getInstance();

    int enumDevice(char (&name)[MAX_DEVICE_NUM][DEVICE_NAME_LENGTH], int (&deviceAddr)[MAX_DEVICE_NUM], int &count);
    int openDevice(int deviceAddr);
    int closeDevice();
    int checkState();

    int readBurst(unsigned int address, unsigned char *pBuf, int addrLength = 4, int valueLength = 1);
    int writeBurst(unsigned int address, unsigned char *pBuf, int addrLength = 4, int valueLength = 1);

    void          setUsbType(enum USB_TYPE type);
    void          setI2cSlaveID(unsigned char slaveId);
    unsigned char getI2cSlaveID();
    void          setI2cSpeed(int speedKhz);
    int           getI2cSpeed();
    void          setSpiMode(int spiMode);
    int           getSpiMode();
    void          setSpiSpeed(float speedMhz);
    float         getSpiSpeed();
    void          setSpiDummyTiming(int dummyTiming);
    int           getSpiDummyTiming();
    void          setSpiIncremental(bool isIncremental);
    bool          getSpiIncremental();
    void         *getDeviceHandle();

    int readI2cBurst(unsigned char slaveID, unsigned int address, unsigned char *pBuf, int addrLength = 2,
        int valueLength = 1, unsigned int writeDelay = 0, unsigned int readDelay = 1, unsigned int speedKhz = 400,int bus=0);
    int writeI2cBurst(unsigned char slaveID, unsigned int address, unsigned char *pBuf, int addrLength = 2,
                      int valueLength = 1, unsigned int writeDelay = 0, unsigned int readDelay = 1, unsigned int speedKhz = 400,int bus=0);

    int writeFpgaRegister(unsigned int address, unsigned int value);
    int readFpgaRegister(unsigned int address, unsigned int &value);

    int readSpiBurst(unsigned int address, unsigned char *pBuf, int addrLength = 4, int valueLength = 1,
        float speedMhz = 5, int spiMode = 3, bool isIncremental = false, int dummyTiming = 8);
    int writeSpiBurst(unsigned int address, unsigned char *pBuf, int addrLength = 4, int valueLength = 1,
        float speedMhz = 5, int spiMode = 3, bool isIncremental = false);

private:
    bool  isOpen;
    void *handle;
    // global variables
    enum USB_TYPE globalUsbType;

    // I2C
    unsigned char globalSlaveId;
    int           globalI2cSpeedKhz;
    unsigned int  I2CSpeed = 0;
    // SPI
    int   globalSpiMode;
    float globalSpiSpeedMhz;
    int   globalSpiDummyTiming;
    bool  globalSpiIncremental;

    QUsbCommunication();
    ~QUsbCommunication();
    int checkI2cState();
    int checkSpiState();
    int setI2cRate(int speedKhz);
};

#endif // QUSBCOMMUNICATION_H
