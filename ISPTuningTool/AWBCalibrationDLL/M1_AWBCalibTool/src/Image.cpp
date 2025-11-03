#include "Image.h"
#include "GroupManager.h"

Image::Image()
{
}

Image::~Image()
{
}

int Image::initImage(int groupId, uint16_t *data, int dLength, ImageInfo info)
{
    _info = info;
    _groupId = groupId;
    _type = GroupManager::instance()->imageType(_groupId);
    int pix_size = info.Width * info.Height;
    if (dLength < pix_size)
        return ERROR_PARAM_SIZE;
    uint16_t *pBuf = new uint16_t[pix_size]();
    memset(pBuf, 0, sizeof(uint16_t) * pix_size);
    memcpy(pBuf, data, sizeof(uint16_t) * pix_size);
    im.resize(info.Height, info.Width);
    im.setZero();
    im = Map<Matrix<uint16_t, Dynamic, Dynamic,RowMajor>>(pBuf, info.Height, info.Width).cast<int>(); // 默认按列，逐列存储数据
    delete[] pBuf;
    pBuf = 0;
    CFAPattern.setZero();
    CFAPattern(0, 0) = 1;
    CFAPattern(0, 1) = 2;
    CFAPattern(1, 0) = 2;
    CFAPattern(1, 1) = 3;
    P_Gray.clear();
    P_Green.clear();
    return 0;
}

int Image::initImagebyFile(const char *path, ImageInfo info)
{
    return 0;
}

int Image::removeImageROI(int index, RoiType type)
{
    if (type == Gray)
    {
        P_Gray.erase(P_Gray.begin() + index);
        P_Gray.erase(P_Gray.begin() + index + 1);
    }
    else
    {
        P_Green.erase(P_Green.begin() + index);
        P_Green.erase(P_Green.begin() + index + 1);
    }
    return ERROR_NO_ERR;
}

int Image::updateImageROI(int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type, int index)
{
    if (type == Gray)
    {
        auto it1 = P_Gray.begin() + index;
        auto it2 = P_Gray.begin() + index + 1;
        *it1 = (std::make_pair(left, top));     // 左上点
        *it2 = (std::make_pair(right, bottom)); // 右下点
    }
    else if (type == Green)
    {
        auto it1 = P_Green.begin() + index;
        auto it2 = P_Green.begin() + index + 1;
        *it1 = (std::make_pair(left, top));     // 左上点
        *it2 = (std::make_pair(right, bottom)); // 右下点
    }
    else
    {
        return ERROR_ROI_TYPE;
    }
    return ERROR_NO_ERR;
}

int Image::deleteImageROI()
{
    P_Gray.clear();
    P_Green.clear();
    return ERROR_NO_ERR;
}

int Image::getImageROICount(int *count)
{
    if (count == nullptr)
        return ERROR_PARAM_NULL;
    (*count) = (int)(P_Gray.size() + P_Green.size()) / 2;
    return ERROR_NO_ERR;
}

int Image::getImageROI(std::vector<int64_t> &left, std::vector<int64_t> &top, std::vector<int64_t> &right, std::vector<int64_t> &bottom, int size)
{
    if (size > (int)(P_Gray.size() + P_Green.size()) / 2)
        return ERROR_ROI_SIZE;
    for (int i = 0; i < (P_Gray.size() / 2); ++i)
    {
        left.push_back(P_Gray.at(i).first);
        top.push_back(P_Gray.at(i).second);
        right.push_back(P_Gray.at(i + 1).first);
        bottom.push_back(P_Gray.at(i + 1).second);
    }
    for (int j = 0; j < (P_Green.size() / 2); ++j)
    {
        left.push_back(P_Green.at(j).first);
        top.push_back(P_Green.at(j).second);
        right.push_back(P_Green.at(j + 1).first);
        bottom.push_back(P_Green.at(j + 1).second);
    }
    return ERROR_NO_ERR;
}

int Image::getImageInfo(ImageInfo *info)
{
    if (info == nullptr)
        return ERROR_PARAM_NULL;
    *info = _info;
    return 0;
}

int Image::getImageAfterAWBCalib(uint16_t *data, uint32_t dLength)
{
    int size = (int)(imdst.rows() * imdst.cols());
    if(dLength<size)
        return ERROR_PARAM_SIZE;

    std::vector<uint16_t> pBuf;
    for (int j = 0; j < imdst.rows(); j++)
    {
        for (int i = 0; i < imdst.cols(); i++)
        {
            uint16_t _val=(uint16_t)(imdst(j, i));
            pBuf.push_back(_val);
        }
    }
    memcpy(data, pBuf.data(), sizeof(uint16_t) * size);
    return 0;
}

int Image::autoCalibration()
{
    int res;
    if (_type == IMAGE_GRASS)
    {
        if(P_Green.empty()||P_Gray.empty())
            return ERROR_PARAM_SIZE;
        res = calibGrass(Green);
        if (res != ERROR_NO_ERR)
            return res;
        res = calibGrass(Gray);
        if (res != ERROR_NO_ERR)
            return res;

        double fixedBG = std::round((MeanGain(0, 0) - MeanGain_Green(0, 0)) * 256);
        double fixedRG = std::round((MeanGain(0, 2) - MeanGain_Green(0, 2)) * 256);
        // 保存当前img的pGreenFixedBG
        GroupManager::instance()->saveGreenFixedPara(_groupId, fixedBG, fixedRG);
        use_high_border=GroupManager::instance()->getGreenCalibMode(_groupId);
        if(!use_high_border)
        {        // 计算边界
            res= calGreenBorder();
        }
    }
    else
    {
        res = calibNormalCWF();
    }
    return res;
}

int Image::calGreenHighLumaROI()
{
    GroupManager::instance()->clearHighLumaVal(_groupId);
    auto res=calGreenBorder();
    return res;

}

int Image::groupId()
{
    return _groupId;
}

int Image::Fix(double val)
{
    if (val >= 0)
    {
        return (int)(std::floor(val));
    }
    else
    {
        return (int)(std::ceil(val));
    }
}

int Image::calLogPoint(MatrixXd RoiWBGain)
{
    VectorXd GaryR;
    GaryR = RoiWBGain.cwiseInverse().col(2);
    VectorXd LogRG;
    LogRG.resize(GaryR.size());
    LogRG.array()=GaryR.array().log2();

    VectorXd GaryBVal;
    GaryBVal = RoiWBGain.cwiseInverse().col(0);
    VectorXd LogBG;
    LogBG.resize(GaryBVal.size());
    LogBG.array()=GaryBVal.array().log2();

    // printXYVal
    VectorXd Xval = LogBG - LogRG;
    VectorXd Yval = LogBG + LogRG;
    return GroupManager::instance()->saveLogPoint(_groupId, Xval, Yval);
}

int Image::calibNormalCWF()
{
    int num = Fix((P_Gray.size() / 2.0));
    if (num == 0)
        return ERROR_ROI_INVALID;
    if (_info.Height <= 0 || _info.Width <= 0)
        return ERROR_IMG_INFO;
    double maxVal = pow(2, _info.Bits) - 1;
    if (maxVal <= 0)
        return ERROR_IMG_BITS;

    Matrix<int, 1, 2> ptnSZ;
    ptnSZ(0, 0) = (int)CFAPattern.rows();
    ptnSZ(0, 1) = (int)CFAPattern.cols();
    // MatrixXd RoiWbGain;
    RoiWbGain.resize(num, 3);
    RoiWbGain.setZero();
    VectorXd wt;
    wt.resize(num);
    wt.setZero();

    Matrix<int, Dynamic, Dynamic> _BLC;
    _BLC.resize(im.rows(), im.cols());
    _BLC.fill(_info.BLC);
    im -= _BLC;
    for (int i = 0; i < num; ++i)
    {
        int x1 = (Fix((double)(P_Gray[i * 2].first) / (double)ptnSZ(0, 1)) * ptnSZ(0, 1) + 1);
        int x2 = (Fix((double)(P_Gray[i * 2 + 1].first) / (double)ptnSZ(0, 1)) * ptnSZ(0, 1));
        int y1 = (Fix((double)(P_Gray[i * 2].second) / (double)ptnSZ(0, 0)) * ptnSZ(0, 0) + 1);
        int y2 = (Fix((double)(P_Gray[i * 2 + 1].second) / (double)ptnSZ(0, 0)) * ptnSZ(0, 0));


        int roi_row = abs(y2 - y1) + 1;
        int roi_col = abs(x2 - x1) + 1;
        Matrix<int, Dynamic, Dynamic> roi;
        auto imtemp=im.cast<double>();
        if(!blockValid(imtemp,y1-1, x1-1, roi_row, roi_col))
            return ERROR_PARAM_SIZE;
        roi = im.block(y1-1, x1-1, roi_row, roi_col);
        int hk = Fix((int)roi.rows() / (int)ptnSZ(0, 0));
        int wk = Fix((int)roi.cols() / (int)ptnSZ(0, 1));
        // dim=1
        Matrix<int, Dynamic, Dynamic> maskB = (CFAPattern.array() == 1).cast<int>().replicate(hk, wk);
        Matrix<int, Dynamic, Dynamic> maskG = (CFAPattern.array() == 2).cast<int>().replicate(hk, wk);
        Matrix<int, Dynamic, Dynamic> maskR = (CFAPattern.array() == 3).cast<int>().replicate(hk, wk);
        double mB = ((roi.cwiseProduct(maskB)).sum()) * (double)(1.0 / maskB.sum());
        double mG = ((roi.cwiseProduct(maskG)).sum()) * (double)(1.0 / maskG.sum());
        double mR = ((roi.cwiseProduct(maskR)).sum()) * (double)(1.0 / maskR.sum());
        RoiWbGain(i, 0) = (mG / mB);
        RoiWbGain(i, 1) = 1.0;
        RoiWbGain(i, 2) = (mG / mR);
        wt(i) = static_cast<double>(roi.size());
        if (i == (num - 1))
        {
            wt /= wt.sum();
        }
    }
    Matrix<double, Dynamic, Dynamic> ret = RoiWbGain.cwiseProduct(wt.replicate(1, 3));
    MeanGain = ret.colwise().sum();
    for (int i = 0; i < MeanGain.cols(); i++) // MeanGain保留4位精度
    {
        for (int j = 0; j < MeanGain.rows(); j++)
        {
            MeanGain(j, i) = round(1000 * MeanGain(j, i)) / 1000;
        }
    }
    // apply WBGain
    Matrix<double, 2, 2> ratio;
    ratio.setZero();
    Matrix<double, 2, 2> _cfa1 = (CFAPattern.array() == 1).cast<double>();
    Matrix<double, 2, 2> _cfa2 = (CFAPattern.array() == 2).cast<double>();
    Matrix<double, 2, 2> _cfa3 = (CFAPattern.array() == 3).cast<double>();
    _cfa1 *= MeanGain(0, 0);
    _cfa2 *= MeanGain(0, 1);
    _cfa3 *= MeanGain(0, 2);
    ratio = _cfa1 + _cfa2 + _cfa3;

    int dsth = (int)im.rows() / ptnSZ(0, 0);
    int dstw = (int)im.cols() / ptnSZ(0, 1);
    imdst.resize(dsth, dstw);
    imdst.setZero();
    imdst = im.cast<double>().cwiseProduct(ratio.replicate(dsth, dstw));

    Matrix<double, Dynamic, Dynamic> BLC;
    BLC.resize(imdst.rows(), imdst.cols());
    BLC.fill(_info.BLC);
    imdst = imdst + BLC;

    Matrix<double, Dynamic, Dynamic> zeroThreshold = (imdst.array() > 0).cast<double>();
    imdst = imdst.cwiseProduct(zeroThreshold);
    Matrix<double, Dynamic, Dynamic> maxThreshold1 = (imdst.array() <= maxVal).cast<double>();
    Matrix<double, Dynamic, Dynamic> maxThreshold2 = ((imdst.array() > maxVal).cast<double>()) * maxVal;
    imdst = (imdst.cwiseProduct(maxThreshold1)) + maxThreshold2;
    auto res = calLogPoint(RoiWbGain);
    return res;
}

int Image::calibGrass(RoiType roiType)
{
    std::vector<std::pair<int64_t, int64_t>> P_Temp;
    Matrix<double, Dynamic, Dynamic> MeanGain_Temp;
    Matrix<double, Dynamic, Dynamic> RoiWbGain_Temp;

    if (roiType == Green)
    {
        P_Temp= P_Green;
    }
    if (roiType == Gray)
    {
        P_Temp = P_Gray;
    }

    int num = Fix((P_Temp.size() / 2.0));
    if (num == 0)
        return ERROR_ROI_INVALID;
    if (_info.Height <= 0 || _info.Width <= 0)
        return ERROR_IMG_INFO;
    double maxVal = pow(2, _info.Bits) - 1;
    if (maxVal <= 0)
        return ERROR_IMG_BITS;

    Matrix<int, 1, 2> ptnSZ;
    ptnSZ(0, 0) = (int)CFAPattern.rows();
    ptnSZ(0, 1) = (int)CFAPattern.cols();

    RoiWbGain_Temp.resize(num, 3);
    RoiWbGain_Temp.setZero();
    VectorXd wt;
    wt.resize(num);
    wt.setZero();

    Matrix<int, Dynamic, Dynamic> im_temp;
    im_temp.resize(im.rows(),im.cols());
    im_temp.array()=im.array();
    Matrix<int, Dynamic, Dynamic> _BLC;
    _BLC.resize(im_temp.rows(), im_temp.cols());
    _BLC.fill(_info.BLC);

    im_temp =im_temp- _BLC;
    for (int i = 0; i < num; ++i)
    {
        int x1 = (Fix((double)(P_Temp[i * 2].first) / (double)ptnSZ(0, 1)) * ptnSZ(0, 1) + 1);
        int x2 = (Fix((double)(P_Temp[i * 2 + 1].first) / (double)ptnSZ(0, 1)) * ptnSZ(0, 1));
        int y1 = (Fix((double)(P_Temp[i * 2].second) / (double)ptnSZ(0, 0)) * ptnSZ(0, 0) + 1);
        int y2 = (Fix((double)(P_Temp[i * 2 + 1].second) / (double)ptnSZ(0, 0)) * ptnSZ(0, 0));
        int roi_row = abs(y2 - y1) + 1;
        int roi_col = abs(x2 - x1) + 1;
        Matrix<int, Dynamic, Dynamic> roi;
        auto imtemp=im_temp.cast<double>();
        if(!blockValid(imtemp,y1-1, x1-1, roi_row, roi_col))
            return ERROR_PARAM_SIZE;
        roi = im_temp.block(y1-1, x1-1, roi_row, roi_col);
        int hk = Fix((int)roi.rows() / (int)ptnSZ(0, 0));
        int wk = Fix((int)roi.cols() / (int)ptnSZ(0, 1));

        Matrix<int, Dynamic, Dynamic> maskB = (CFAPattern.array() == 1).cast<int>().replicate(hk, wk);
        Matrix<int, Dynamic, Dynamic> maskG = (CFAPattern.array() == 2).cast<int>().replicate(hk, wk);
        Matrix<int, Dynamic, Dynamic> maskR = (CFAPattern.array() == 3).cast<int>().replicate(hk, wk);
        double mB = ((roi.cwiseProduct(maskB)).sum())/ (double)(maskB.sum());
        double mG = ((roi.cwiseProduct(maskG)).sum())/ (double)(maskG.sum());
        double mR = ((roi.cwiseProduct(maskR)).sum())/ (double)(maskR.sum());
        RoiWbGain_Temp(i, 0) = (mG / mB);
        RoiWbGain_Temp(i, 1) = 1.0;
        RoiWbGain_Temp(i, 2) = (mG / mR);
        wt(i) = static_cast<double>(roi.size());
        if (i == (num - 1))
        {
            wt /= (double)(wt.sum());
        }
    }
    Matrix<double, Dynamic, Dynamic> ret = RoiWbGain_Temp.cwiseProduct(wt.replicate(1, 3));
    MeanGain_Temp = ret.colwise().sum();
    for (int i = 0; i < MeanGain_Temp.cols(); i++) // MeanGain保留4位精度
    {
        for (int j = 0; j < MeanGain_Temp.rows(); j++)
        {
            MeanGain_Temp(j, i) = round(10000 * MeanGain_Temp(j, i)) / 10000;
        }
    }
    if (roiType == Green)
    {
        MeanGain_Green.resize(MeanGain_Temp.rows(),MeanGain_Temp.cols());
        RoiWBGain_Green.resize(RoiWbGain_Temp.rows(),RoiWbGain_Temp.cols());
        MeanGain_Green.array() =MeanGain_Temp.array();
        RoiWBGain_Green.array()=RoiWbGain_Temp.array();
    }
    if (roiType == Gray)
    {
        MeanGain.resize(MeanGain_Temp.rows(),MeanGain_Temp.cols());
        RoiWbGain.resize(RoiWbGain_Temp.rows(),RoiWbGain_Temp.cols());
        MeanGain.array()=MeanGain_Temp.array();
        RoiWbGain.array()=RoiWbGain_Temp.array();
    }
    return 0;
}

int Image::calGreenBorder()
{
    im_1 = im(seqN(0, im.rows() / 2, 2), seqN(0, im.cols() / 2, 2)).cast<double>();
    Matrix<double, Dynamic, Dynamic> im_2temp;
    im_2 = im(seqN(0, im.rows() / 2, 2), seqN(1, im.cols() / 2, 2)).cast<double>() +
           im(seqN(1, im.rows() / 2, 2), seqN(0, im.cols() / 2, 2)).cast<double>();
    im_2temp.resize(im_2.rows(),im_2.cols());
    im_2temp.fill(0.5);
    im_2=im_2.cwiseProduct(im_2temp);
    im_3 = im(seqN(1, im.rows() / 2, 2), seqN(1, im.cols() / 2, 2)).cast<double>();

    Matrix<double, Dynamic, Dynamic> BLC;
    BLC.resize(im_1.rows(), im_1.cols());
    BLC.fill(_info.BLC);
    im_1 = im_1-BLC;
    im_2 = im_2-BLC;
    im_3 = im_3-BLC;
    Matrix<double, Dynamic, Dynamic> zeroThreshold = (im_1.array() > 0).cast<double>();
    im_1 = im_1.cwiseProduct(zeroThreshold);
    Matrix<double, Dynamic, Dynamic> zeroThreshold2 = (im_2.array() > 0).cast<double>();
    im_2 = im_2.cwiseProduct(zeroThreshold2);
    Matrix<double, Dynamic, Dynamic> zeroThreshold3 = (im_3.array() > 0).cast<double>();
    im_3 = im_3.cwiseProduct(zeroThreshold3);

    // //区域选择与绿区保持一致
    int x1 = (int)((P_Green[0].first)/2.0);
    int x2 = (int)((P_Green[1].first)/2.0);
    int y1 = (int)((P_Green[0].second)/2.0);
    int y2 = (int)((P_Green[1].second)/2.0);

    int roi_row = abs(y2 - y1) + 1;
    int roi_col = abs(x2 - x1) + 1;
    if(!blockValid(im_1,y1, x1, roi_row, roi_col)||!blockValid(im_2,y1, x1, roi_row, roi_col)||!blockValid(im_3,y1, x1, roi_row, roi_col))
        return ERROR_PARAM_SIZE;
    im_roi1 = im_1.block(y1, x1, roi_row, roi_col).cast<double>();
    im_roi2 = im_2.block(y1, x1, roi_row, roi_col).cast<double>();
    im_roi3 = im_3.block(y1, x1, roi_row, roi_col).cast<double>();

    Matrix<double, Dynamic, Dynamic> Bval;
    Bval=im_roi1.cwiseQuotient(im_roi2);
    Matrix<double, Dynamic, Dynamic> Rval;
    Rval=im_roi3.cwiseQuotient(im_roi2);
    Matrix<double, Dynamic, Dynamic> Xval;
    Matrix<double, Dynamic, Dynamic> Yval;
    Xval=Bval.array().log2()-Rval.array().log2();
    Yval=Bval.array().log2()+Rval.array().log2();
    GroupManager::instance()->saveGreenLogPoint(_groupId,Xval,Yval);   //保存绿区点列表

    int roi_height=(int)im_roi1.rows();
    int roi_weight=(int)im_roi1.cols();

    MeanB=(float)(im_roi1.mean()*roi_height*roi_weight);
    MeanG=(float)(im_roi2.mean()*roi_height*roi_weight);
    MeanR=(float)(im_roi3.mean()*roi_height*roi_weight);
    sumBGR=(uint32_t)roi_height*roi_weight;//roi单通道像素总量
    auto res=GroupManager::instance()->saveGreenMeanGain(_groupId,MeanB,MeanG,MeanR,sumBGR);   //保存绿区点列表
    return res;
}

int Image::calGreenMeanPoint()
{
    double logMeanBG=std::log2((double)(MeanB/sumBGR)/(double)(MeanG/sumBGR));
    double logMeanRG=std::log2((double)(MeanR/sumBGR)/(double)(MeanG/sumBGR));

    XMeanval=logMeanBG-logMeanRG;
    YMeanval=logMeanBG+logMeanRG;
    auto res=GroupManager::instance()->saveGreenMeanPoint(_groupId,XMeanval,YMeanval);
    return res;
}

int Image::addImageROI(int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type)
{
    switch (type)
    {
    case Gray:
        P_Gray.push_back(std::make_pair(left, top));     // 左上点
        P_Gray.push_back(std::make_pair(right, bottom)); // 右下点
        break;
    case Green:
        P_Green.push_back(std::make_pair(left, top));     // 左上点
        P_Green.push_back(std::make_pair(right, bottom)); // 右下点
        break;
    default:
        return ERROR_ROI_TYPE;
    }
    return ERROR_NO_ERR;
}

bool Image::blockValid(Matrix<double, Dynamic, Dynamic> matrix, int startx, int starty, int x_zise, int y_size)
{
    if(startx>=0&&starty>=0)
    {
        if((startx+x_zise)<=matrix.rows()&&(starty+y_size)<=matrix.cols())
        {
            return true;
        }
    }
    return false;

}
