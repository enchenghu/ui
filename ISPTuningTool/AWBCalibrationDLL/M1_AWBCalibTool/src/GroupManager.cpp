#include "GroupManager.h"
#include "GroupManager.h"
#include "ImageManager.h"

GroupManager *GroupManager::_instance = 0;
GroupManager *GroupManager::instance()
{
    if (_instance == 0)
    {
        _instance = new GroupManager();
    }
    return _instance;
}
GroupManager::GroupManager() {}
GroupManager::~GroupManager() {}

int GroupManager::createGroup(int groupId, GroupInfo groupInfo)
{
    if (valid(groupId))
        return ERROR_GROUPID;
    Group _group(groupId, groupInfo);
    _GroupMap.emplace(groupId, _group);
    return ERROR_NO_ERR;
}

int GroupManager::getLogPointNum(int groupId, int *num)
{
    if (num == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getLogPointNum(num);
    return res;
}

int GroupManager::getGroupLogPoint(int groupId, double *x, double *y, int count)
{
    if (x == nullptr || y == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;

    auto it = _GroupMap.find(groupId);
    auto res = it->second.getLogPoint(x, y, count);
    return res;
}

int GroupManager::getWeightMapSize(int groupId, int *row, int *col)
{
    if (row == nullptr || col == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getWeightMapSize(row, col);
    return res;
}

int GroupManager::getWeightMap(int groupId, double *weight, int weightSize)
{
    if (weight == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getWeightMap(weight, weightSize);
    return res;
}

int GroupManager::getGroupCalibRes(int groupId, AWBCalibRes *result)
{
    if (result == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getCalibResult(result);
    return res;
}

int GroupManager::setGreenCalibMode(int groupId, bool isHighLumaBorder)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    GreenCalibModeMap[groupId]=isHighLumaBorder;
    return ERROR_NO_ERR;
}

int GroupManager::getGreenLogPointNum(int groupId, int *num)
{
    if (num == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getGreenLogPointNum(num);
    return res;
}

int GroupManager::getGreenLogPoint(int groupId,double *xVal, double*yVal,int num)
{
    if (xVal == nullptr||yVal == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getGreenLogPoint(xVal,yVal,num);
    return res;
}

int GroupManager::getGreenLogMeanPoint(int groupId, double *xMean, double* yMean)
{
    if (xMean == nullptr||yMean == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getGreenLogMeanPoint(xMean,yMean);
    return res;
}

int GroupManager::getGreenFixedParaNum(int groupId, int *num)
{
    if (num == nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getGreenFixParaNum(num);
    return res;
}

int GroupManager::getGreenFixedPara(int groupId, double *fixedBG, double *fixedRG,int size)
{
    if (fixedBG == nullptr||fixedRG==nullptr)
        return ERROR_PARAM_NULL;
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.getGreenFixPara(fixedBG,fixedRG,size);
    return res;
}

int GroupManager::getTotalGroup(int *count)
{
    if (count == nullptr)
        return ERROR_PARAM_NULL;
    *count = static_cast<int>(_GroupMap.size());
    return ERROR_NO_ERR;
}

int GroupManager::releseGroup(int groupId)
{
    auto it = _GroupMap.find(groupId);
    if(it==_GroupMap.end())
        return 0;
    it->second.clear();
    _GroupMap.erase(it);
    return ImageManager::releaseGroup(groupId);
}

int GroupManager::releaseAll()
{
    for(auto iter=_GroupMap.begin();iter!=_GroupMap.end();iter++)
        iter->second.clear();
    _GroupMap.clear();
    GreenCalibModeMap.clear();
    return ImageManager::clearAll();
}

int GroupManager::clearHighLumaVal(int groupId)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.clearHighLumaVal();
    return res;
}

bool GroupManager::valid(int groupId)
{
    if (_GroupMap.find(groupId) != _GroupMap.end())
        return true;
    else
        return false;
}

int GroupManager::saveLogPoint(int groupId, VectorXd xVal, VectorXd yVal)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    it->second.setlogPoint(xVal, yVal);
    return ERROR_NO_ERR;
}

int GroupManager::saveGreenFixedPara(int groupId, double BG, double RG)
{
   if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
   it->second.setGreenFixPara(BG, RG);
   return ERROR_NO_ERR;
}

int GroupManager::saveGreenLogPoint(int groupId, Matrix<double, Dynamic, Dynamic> xVal, Matrix<double, Dynamic, Dynamic> yVal)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    it->second.saveGreenLogPoint(xVal, yVal);
    return ERROR_NO_ERR;
}
int GroupManager::saveGreenMeanPoint(int groupId,double x,double y)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    it->second.saveGreenMeanPoint(x, y);
    return ERROR_NO_ERR;

}

int GroupManager::saveGreenMeanGain(int groupId, float MeanB, float MeanG, float MeanR, uint32_t sumBGR)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    it->second.saveGreenMeanGain(MeanB, MeanG,MeanR,sumBGR);
    return ERROR_NO_ERR;
}

ImageType GroupManager::imageType(int groupId)
{
    if (valid(groupId))
    {
        auto it = _GroupMap.find(groupId);
        return it->second.groupType();
    }

    return ImageType();
}

bool GroupManager::getGreenCalibMode(int groupId)
{
    if (_GroupMap.find(groupId) != _GroupMap.end())
    {
        auto it = GreenCalibModeMap.find(groupId);
        bool flag= (it->second);
        return flag;
    }
    return false;
}

int GroupManager::calculateWeightMap(int groupId)
{
    if (!valid(groupId))
        return ERROR_GROUPID;
    auto it = _GroupMap.find(groupId);
    auto res = it->second.calculateWeightMap();
    return res;
}
