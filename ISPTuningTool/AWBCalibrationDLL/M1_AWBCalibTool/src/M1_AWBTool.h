#ifndef M1_AWBTool_LIB_H
#define M1_AWBTool_LIB_H

#if defined(_WIN32) || defined(_WIN64)
#  ifdef M1_AWBTool_LIBRARY
#    define M1_AWBTool_EXPORT __declspec(dllexport)
#  else
#    define M1_AWBTool_EXPORT __declspec(dllimport)
#  endif
#endif
#include "M1AWBTypeDef.h"
#ifdef __cplusplus
extern "C" {
#endif

M1_AWBTool_EXPORT int CreateGroup(int groupId, GroupInfo group);
M1_AWBTool_EXPORT int SetImageInfo(int groupId, Img* img, uint16_t* data, int dLength, ImageInfo info);
M1_AWBTool_EXPORT int SetImageInfoFile(int groupId, Img* img, const char* path, ImageInfo info);
M1_AWBTool_EXPORT int SetRegInfofromCfg(int groupId, const char* path);///暂时未空
M1_AWBTool_EXPORT int RemoveImage(Img img);
M1_AWBTool_EXPORT int RemoveGroup(int groupId);
M1_AWBTool_EXPORT int ClearAll();

M1_AWBTool_EXPORT int AddROI(Img img, int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type);
M1_AWBTool_EXPORT int RemoveROI(Img img, int index,RoiType type);
M1_AWBTool_EXPORT int UpdateROI(Img img, int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type, int index);
M1_AWBTool_EXPORT int DeleteROI(Img img);
M1_AWBTool_EXPORT int GetImageROICount(Img img, int *count);
M1_AWBTool_EXPORT int GetImageROI(Img img, int64_t* left, int64_t* top, int64_t* right, int64_t* bottom, int size);
M1_AWBTool_EXPORT int CalculateWBGain(Img img);//所有类型（Normal，CWF，Grass）场景都需在添加ROI操作后调用。

M1_AWBTool_EXPORT int CalculateWeightMap(int groupId);///（Normal，CWF）场景需在CalculateWBGain后调用。
M1_AWBTool_EXPORT int GetImageInfo(Img img, ImageInfo* info);
M1_AWBTool_EXPORT int GetGroupsNum(int* count);
M1_AWBTool_EXPORT int GetImageAfterAWB(Img img, uint16_t* data, uint32_t dLength);///获取（Normal，CWF）白平衡后的raw图
M1_AWBTool_EXPORT int GetRegInfo(int groupId, RegInfo* regInfo);///暂时为空
M1_AWBTool_EXPORT int GetLogPointNum(int groupId, int *num);
M1_AWBTool_EXPORT int GetLogPoint(int groupId, double* x, double* y, int count);///（Normal，CWF）打印的excel数据。
M1_AWBTool_EXPORT int GetWeightMapSize(int groupId, int *row, int *col);
M1_AWBTool_EXPORT int GetWeightMap(int groupId, double* weight, int weightSize);///（Normal，CWF）打印的txt数据。

M1_AWBTool_EXPORT int SetGreenCalibMode(int groupId, bool isUseHighLumaBoder);//设置Grass标定模式
M1_AWBTool_EXPORT int GetGreenLogPointNum(int groupId, int*num);
M1_AWBTool_EXPORT int GetGreenLogPoint(int groupId, double* xVal, double *yVal, int num);//获取Grass标定后的散点坐标
M1_AWBTool_EXPORT int CalGreenHighLumaROI(int groupId, Img img);//（isUseHighLumaBoder=true）模式下，需要手动指定最高luma的Grass图
M1_AWBTool_EXPORT int GetGreenLogMeanPoint(int groupId, double* xMean, double* yMean);//获取Grass标定后的散点中心点坐标
M1_AWBTool_EXPORT int GetGreenFixedParaNum(int groupId, int*num);
M1_AWBTool_EXPORT int GetGreenFixedPara(int groupId, double* fixedBG, double* fixedRG, int size);//Grass打印的txt数据
M1_AWBTool_EXPORT int GetCalibRes(int groupId, AWBCalibRes* res);///所有类型（Normal，CWF，Grass）获取标定结果AWBCalibRes（寄存器读写相关的所有值）

#ifdef __cplusplus
}
#endif

#endif // M1_FLASH_LIB_H
