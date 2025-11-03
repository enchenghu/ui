#ifndef M1_AWB_TYPE_DEF_H
#define M1_AWB_TYPE_DEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *Img;

enum ErrorType : int32_t {
    ERROR_NO_ERR                 = 0,
    ERROR_PARAM                  = (int32_t)0x80010000,
    ERROR_PARAM_NULL             = (int32_t)0x80010001,
    ERROR_PARAM_OUT_OF_RANGE     = (int32_t)0x80010002,
    ERROR_PARAM_SIZE             = (int32_t)0x80010003,

    ERROR_IMG                    = (int32_t)0x80020000,
    ERROR_IMG_EMPTY              = (int32_t)0x80020001,
    ERROR_IMG_INVALID            = (int32_t)0x80020002,
    ERROR_PATH                   = (int32_t)0x80030000,
    ERROR_PATH_INVALID           = (int32_t)0x80030001,
    ERROR_PATH_NOT_FILE          = (int32_t)0x80030002,
    ERROR_PATH_NOT_EXISTS        = (int32_t)0x80030003,
    ERROR_CFG                    = (int32_t)0x80040000,
    ERROR_CFG_SIZE               = (int32_t)0x80040001,
    ERROR_IMG_FILE               = (int32_t)0x80050000,
    ERROR_IMG_FILE_READ          = (int32_t)0x80050001,
    ERROR_IMG_FILE_WRITE         = (int32_t)0x80050002,
    ERROR_IMG_BITS               = (int32_t)0x80050003,
    ERROR_IMG_INFO               = (int32_t)0x80050004,

    ERROR_GROUPID                = (int32_t)0x80060000,
    ERROR_ROI_INVALID            = (int32_t)0x80060001,
    ERROR_ROI_TYPE               = (int32_t)0x80060002,
    ERROR_ROI_SIZE               = (int32_t)0x80060003,

    ERROR_CONFIG_FILE            = (int32_t)0x80080000,
    ERROR_CONFIG_FILE_READ       = (int32_t)0x80080001,
    ERROR_CONFIG_FILE_WRITE      = (int32_t)0x80080002,
};


enum ImageType : int32_t {
    IMAGE_NORMAL        = 1 << 0,
    IMAGE_CWF           = 1 << 1,
    IMAGE_GRASS         = 1 << 2,
};

enum RoiType : int32_t {
    Gray           = 1 << 0,
    Green          = 1 << 1,
};


struct RegInfo
{

};

struct AWBCalibRes
{
    //normal
    double nor_tab_bor[2]={};//灰点分布区域大小
    double nor_div_step[2]={};
    double nor_table[256]={};//8*32标定参数矩阵
    //CWF
    double cwf_tab_bor[2]={};//灰点分布区域大小
    double cwf_div_step[2]={};
    double cwf_line_x[2]={};//限定CWF区域
    double cwf_line_y[2]={};
    double cwf_table[48]={};//6*8标定参数矩阵
    //Grass
    double green_fixed_BG[4]={};//绿区分布区域
    double green_fixed_RG[4]={};
};

struct ImageInfo//图像信息
{
     int Width=0;  
     int Height=0;  
     int Bits=0;  
     int BLC=0;
};

struct GroupInfo{//一组标定信息
     ImageType type;//一组标定对应一个场景类型
     double Margin=0.1;//0~0.2
};



#ifdef __cplusplus
}
#endif

#endif // M1_AWB_TYPE_DEF_H
