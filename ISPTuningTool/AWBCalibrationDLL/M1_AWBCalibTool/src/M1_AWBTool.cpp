#include "M1_AWBTool.h"
#include "M1_AWBTool.h"
#include "GroupManager.h"
#include "ImageManager.h"
int CreateGroup(int groupId, GroupInfo group)
{
    return GroupManager::instance()->createGroup(groupId, group);
}

int SetImageInfo(int groupId, Img *img, uint16_t *data, int dLength, ImageInfo info)
{
    auto res = ImageManager::create(img);
    if (res == ERROR_NO_ERR)
    {
        res = ImageManager::setImageInfo(groupId,*img, data, dLength, info);
    }
    return res;
}

int SetImageInfoFile(int groupId, Img *img, const char *path, ImageInfo info)
{
    auto res = ImageManager::create(img);
    if (res == ERROR_NO_ERR)
    {
    }
    return res;
}

int SetRegInfofromCfg(int groupId, const char *path)
{
    return ERROR_NO_ERR;
}

int RemoveImage(Img img)
{
    return ImageManager::release(img);
}

int AddROI(Img img, int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type)
{
    return ImageManager::addROI(img, left, top, right, bottom, type);
}

int RemoveROI(Img img, int index,RoiType type)
{
    return ImageManager::execute(img, &Image::removeImageROI, index, type);
}

int UpdateROI(Img img, int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type, int index)
{
    return ImageManager::execute(img, &Image::updateImageROI, left, top, right,bottom, type,index);
}

int DeleteROI(Img img)
{
    return ImageManager::execute(img, &Image::deleteImageROI);
}

int GetImageROICount(Img img, int *count)
{
    return ImageManager::execute(img, &Image::getImageROICount,count);
}

int GetImageROI(Img img, int64_t *left, int64_t *top, int64_t *right, int64_t *bottom, int size)
{

   return ImageManager::getImageROI(img,left,top,right,bottom,size);
}

int CalculateWBGain(Img img)
{
    return ImageManager::calculateWBGain(img);
}

int CalculateWeightMap(int groupId)
{
    return GroupManager::instance()->calculateWeightMap(groupId);
}

int GetImageInfo(Img img, ImageInfo *info)
{
    return ImageManager::execute(img, &Image::getImageInfo,info);
}

int GetImageAfterAWB(Img img, uint16_t *data, uint32_t dLength)
{
    return ImageManager::getImageAfterAWBCalib(img, data, dLength);

}

int GetRegInfo(int groupId, RegInfo *regInfo)
{
    return 0;
}
int GetLogPointNum(int groupId, int *num)
{
    return GroupManager::instance()->getLogPointNum(groupId, num);
}

int GetLogPoint(int groupId, double *x, double *y, int count)
{
    return GroupManager::instance()->getGroupLogPoint(groupId, x,y,count);
}

int GetWeightMapSize(int groupId, int *row, int *col)
{
    return GroupManager::instance()->getWeightMapSize(groupId, row,col);
}

int GetWeightMap(int groupId, double *weight, int weightSize)
{
    return GroupManager::instance()->getWeightMap(groupId, weight,weightSize);

}

int GetCalibRes(int groupId, AWBCalibRes *res)
{
    return GroupManager::instance()->getGroupCalibRes(groupId, res);
}

int GetGroupsNum(int *count)
{
    return GroupManager::instance()->getTotalGroup(count);
}

int RemoveGroup(int groupId)
{
    return GroupManager::instance()->releseGroup(groupId);

}

int ClearAll()
{
    return GroupManager::instance()->releaseAll();
}
int GetGreenLogPointNum(int groupId, int *num)
{
    return GroupManager::instance()->getGreenLogPointNum(groupId,num);
}

int GetGreenLogPoint(int groupId, double *xVal, double *yVal,int num)
{
    return GroupManager::instance()->getGreenLogPoint(groupId,xVal,yVal,num);
}

int GetGreenLogMeanPoint(int groupId, double *xMean, double* yMean)
{
    return GroupManager::instance()->getGreenLogMeanPoint(groupId,xMean,yMean);
}

int CalGreenHighLumaROI(int groupId, Img img)
{
    return ImageManager::calGreenHighLumaROI(groupId, img);

}

int GetGreenFixedPara(int groupId, double *fixedBG, double *fixedRG,int size)
{
    return GroupManager::instance()->getGreenFixedPara(groupId,fixedBG,fixedRG,size);
}

int GetGreenFixedParaNum(int groupId, int *num)
{
    return GroupManager::instance()->getGreenFixedParaNum(groupId,num);
}

int SetGreenCalibMode(int groupId, bool isUseHighLumaBoder)
{
    return GroupManager::instance()->setGreenCalibMode(groupId,isUseHighLumaBoder);
}
