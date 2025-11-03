#ifndef TRANSFERMANAGER_H
#define TRANSFERMANAGER_H

#include <QObject>

class TransferManager : public QObject
{
    Q_OBJECT
public:
    static TransferManager& getInstance()
    {
        static TransferManager instance;
        return instance;
    }

    void changeRgbIrMode(int newMode)
    {
        if (iRgbIrMode != newMode) {
            iRgbIrMode = newMode;
            emit rgbIrModeChanged(newMode);
        }
    }

    void changeLscValue(bool bNewValue)
    {
        if (bLscValue != bNewValue)
        {
            bLscValue = bNewValue;
            emit lscValueChanged(bNewValue);
        }
    }

    int getRgbIrMode() const { return iRgbIrMode; }
    bool getLscValue() const { return bLscValue; }
signals:
    void rgbIrModeChanged(int &newMode);
    void lscValueChanged(bool &bNewValue);
private:
    TransferManager() {};
    ~TransferManager() {};
    TransferManager(const TransferManager&) = delete;
    TransferManager& operator=(const TransferManager&) = delete;

    int iRgbIrMode = 0;
    bool bLscValue = false;
};

#endif