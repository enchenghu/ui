#ifndef GROUP_H
#define GROUP_H
//#include "GroupManager.h"
#include "M1AWBTypeDef.h"
#include "Eigen/Dense"
#include <map>
using namespace Eigen;
class Group
{
public:
    Group(int Id, GroupInfo info);
    ~Group();
    int getLogPointNum(int *num);
    int getLogPoint(double *x, double *y, int count);
    int getWeightMapSize(int *row, int *col);
    int getWeightMap(double *weight, int weightSize);
    int getCalibResult(AWBCalibRes *res);
    int calculateWeightMap();

    int getGreenFixParaNum(int* fixedNum);
    int getGreenFixPara(double* BG, double*RG, int size);
    int getGreenLogPointNum(int* num);
    int getGreenLogPoint(double* logx, double* logy, int num);
    int getGreenLogMeanPoint(double *meanx,double* meany);

    int setlogPoint(VectorXd x, VectorXd y);
    int setGreenFixPara(double BG,double RG);
    int saveGreenMeanPoint(double xMean,double yMean);
    int saveGreenLogPoint(Matrix<double, Dynamic, Dynamic> xVal,Matrix<double, Dynamic, Dynamic> yVal);
    int saveGreenMeanGain(float MeanB, float MeanG, float MeanR, uint32_t sumBGR);

    void clear();
    bool blockValid(Matrix<double, Dynamic, Dynamic> matrix,int startx,int starty,int x_zise,int y_size);
    int clearHighLumaVal();
    ImageType groupType();

private:
    int _groupId;
    GroupInfo _info;
    std::vector<std::pair<double, double>> logPointVector;
    std::vector<std::pair<double, double>> greenFixedVector;
    VectorXd xVal;
    VectorXd yVal;
    Matrix<double, Dynamic, Dynamic> weightMap;
    Matrix<double, Dynamic, Dynamic> table;
    Matrix<double, 5, 7> kernel;
    AWBCalibRes calibrationRes;
    std::vector<double> Green_xVal;
    std::vector<double> Green_yVal;
    double Green_xMean=0;
    double Green_yMean=0;

    float _MeanB=0;
    float _MeanG=0;
    float _MeanR=0;
    uint64_t _sumBGR=0;

};
#endif
