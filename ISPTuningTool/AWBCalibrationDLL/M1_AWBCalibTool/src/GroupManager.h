#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H
#include <iostream>
#include "M1AWBTypeDef.h"
#include "Group.h"
#include <map>
#include "Eigen/Dense"
using namespace Eigen;
class Group;
class GroupManager
{
public:
    static GroupManager *instance();
    int createGroup(int groupId, GroupInfo groupInfo);
    int getLogPointNum(int groupId,int* num);
    int getGroupLogPoint(int groupId, double *x, double *y, int count);
    int calculateWeightMap(int groupId);
    int getWeightMapSize(int groupId, int *row, int *col);
    int getWeightMap(int groupId, double *weight, int weightSize);
    int getGroupCalibRes(int groupId, AWBCalibRes *result);

    int setGreenCalibMode(int groupId,bool isHighLumaBorder);
    int getGreenLogPointNum(int groupId, int *num);
    int getGreenLogPoint(int groupId, double *xVal, double *yVal, int num);
    int getGreenLogMeanPoint(int groupId, double *xMean, double *yMean);
    int getGreenFixedParaNum(int groupId, int *num);
    int getGreenFixedPara(int groupId, double *fixedBG, double *fixedRG, int size);

    int saveLogPoint(int groupId, VectorXd xVal,VectorXd yVal);
    int saveGreenFixedPara(int groupId, double BG,double RG);
    int saveGreenLogPoint(int groupId, Matrix<double, Dynamic, Dynamic> xVal,Matrix<double, Dynamic, Dynamic> yVal);
    int saveGreenMeanPoint(int groupId,double x,double y);
    int saveGreenMeanGain(int groupId, float MeanB, float MeanG, float MeanR, uint32_t sumBGR);
    int getTotalGroup(int* count);
    int releseGroup(int groupId);
    int releaseAll();

    int clearHighLumaVal(int groupId);
    ImageType imageType(int groupId);
    bool getGreenCalibMode(int groupId);
private:
    explicit GroupManager();
    ~GroupManager();
    bool valid(int groupId);
private:
    static GroupManager *_instance;
    int id;
    std::map<int, Group> _GroupMap;
    std::map<int,bool> GreenCalibModeMap;

};
#endif

