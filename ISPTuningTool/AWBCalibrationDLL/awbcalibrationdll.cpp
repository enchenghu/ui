#include "awbcalibrationdll.h"
#include "mainwindow.h"
#include "awbburn.h"

AWBCalibrationDLL::AWBCalibrationDLL() {}

int AWB_CreateWindow()
{
    MainWindow wind;
    wind.setWindowTitle("AWBCalibration");
    return wind.exec();
}

void AWB_SetWriteFunc(AWBWriteFunc writeFunc)
{
    AWBBurn::setWriteFunc(writeFunc);
}

void AWB_SetRegisterGetFunc(AWBGetAWBRegisterFunc registerFunc)
{
    AWBBurn::setAWBRegisterFunc(registerFunc);
}

void AWB_SetRegisterValueGetFunc(AWBGetRegisterValue getRegisterValueFunc)
{
    AWBBurn::setAWBGetRegisterValueFunc(getRegisterValueFunc);
}
