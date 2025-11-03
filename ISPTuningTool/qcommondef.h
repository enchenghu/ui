#ifndef QCOMMONDEF_H
#define QCOMMONDEF_H

#include <QString>
#include <QCoreApplication>
#include "Common/Others/qmylogger.h"
#include "Common/Others/qconfigfiletrans.h"

const int ADDRESS_LENGTH = 4; // 寄存器地址长度，4 Byte
const int VALUE_LENGTH   = 1; // 寄存器值长度，1 Byte
const int ADDRESS_HEX_INFO = 8; //寄存器值16进制占8个字符
const int VALUE_HEX_INFO = 2; //寄存器值16进制占2个字符
const int GAMMAPOINT = 33;	   //Gamma实际点数
const int MAXPOINT = 100*10000;	   //Gamma曲线拟合的最大点数
const int GAMMA_MAX = 1023;
const int GAMMAGAIN_MAX = 4095;
const int COMBINE_VALUE_BIT_MAX = 32; //组合寄存器最大bit个数


//Gamma拟合点 数量
enum POINTTYPE
{
    POINT_3 = 0,
    POINT_5,
    POINT_8,
    POINT_12,
    POINT_16,
    POINT_20,
    POINT_33,
    POINT_33_Single,
};

enum GAMMAMETHOD
{
    GAMMA_CUBIC_SPLINE = 0, //三次样条插值拟合
    GAMMA_BEZIER = 1,       //贝塞尔曲线拟合
};

enum REFERENCE_SET {
    SET_1 = 0,
    SET_2,
    SET_3,
    SET_4,
    SET_5,
    REFERENCE_SET_MAX,
};

enum GAMMA_TYPE {
    GAMMA_A = 0,
    GAMMA_B,
    GAMMA_TYPE_MAX,
};

enum CAMERA_ID {
    CAMERA_0 = 0,
    CAMERA_1,
    CAMERA_ID_MAX,
};

enum MODULE_GAMMA_GAMMAGAIN {
    MODULE_GAMMA = 0,
    MODULE_GAMMAGAIN,
    MODULE_GAMMA_GAMMAGAIN_MAX,
};

enum USERLEVER
{
    LEVEL_ADMINISTRATOR = 1000,
    LEVEL_HIGHUSER = 2000,
    LEVEL_MIDDLEUSER = 3000,
    LEVEL_USER = 4000,
    LEVEL_TEMPUSER = 5000,
};

enum FloatIntType
{
    Type_1_0x100 = 0,
    Type_1_0x200 = 1,
};

#endif // QCOMMONDEF_H
