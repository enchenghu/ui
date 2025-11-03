#ifndef IMAGE_H
#define IMAGE_H
#include "M1AWBTypeDef.h"
#include "Eigen/Dense"
#include "Windows.h"
using namespace Eigen;
class Image
{

public:
    Image();
    ~Image();
    int initImage(int groupId, uint16_t *data, int dLength, ImageInfo info);
    int initImagebyFile(const char *path, ImageInfo info);
    int addImageROI(int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type);
    int removeImageROI(int index, RoiType type);
    int updateImageROI(int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type, int index);
    int deleteImageROI();
    int getImageROICount(int *count);
    int getImageROI(std::vector<int64_t> &left, std::vector<int64_t> &top, std::vector<int64_t> &right, std::vector<int64_t> &bottom, int size);
    int getImageInfo(ImageInfo *info);
    int getImageAfterAWBCalib(uint16_t *data, uint32_t dLength);
    int autoCalibration();
    int calGreenHighLumaROI();
    int groupId();
private:
    int Fix(double val);
    int calLogPoint(MatrixXd RoiWBGain);
    int calibNormalCWF();
    int calibGrass(RoiType roiType);
    int calGreenBorder();
    int calGreenMeanPoint();
    bool blockValid(Matrix<double, Dynamic, Dynamic> matrix, int startx, int starty, int x_zise, int y_size);


private:
    ImageInfo _info;
    Matrix<int, Dynamic, Dynamic> im;
    Matrix<double, Dynamic, Dynamic> imdst;
    Matrix<uint16_t, 2, 2> CFAPattern;
    Matrix<double, Dynamic, Dynamic> MeanGain;
    Matrix<double, Dynamic, Dynamic> RoiWbGain;
    Matrix<double, Dynamic, Dynamic> MeanGain_Green;
    Matrix<double, Dynamic, Dynamic> RoiWBGain_Green;
    std::vector<std::pair<int64_t, int64_t>> P_Gray;
    std::vector<std::pair<int64_t, int64_t>> P_Green;
    Matrix<double, Dynamic, Dynamic> im_1;
    Matrix<double, Dynamic, Dynamic> im_2;
    Matrix<double, Dynamic, Dynamic> im_3;//image三维矩阵第i维度

    Matrix<double, Dynamic, Dynamic> im_roi1;
    Matrix<double, Dynamic, Dynamic> im_roi2;
    Matrix<double, Dynamic, Dynamic> im_roi3;//image三维矩阵第i维度的roi

    int green_x1=0;
    int green_x2=0;
    int green_y1=0;
    int green_y2=0;

    float MeanB=0;
    float MeanG=0;
    float MeanR=0;
    uint32_t sumBGR=0;
    double XMeanval=0;
    double YMeanval=0;

    int _groupId=0;
    bool use_high_border=false;
    ImageType _type;

};

#endif
