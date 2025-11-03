#include "Group.h"

Group::Group(int Id, GroupInfo info)
{
    _groupId = Id;
    _info = info;
    kernel << 1, 2, 3, 4, 3, 2, 1,
        2, 5, 9, 10, 9, 5, 2,
        3, 8, 12, 15, 12, 8, 3,
        2, 5, 9, 10, 9, 5, 2,
        1, 2, 3, 4, 3, 2, 1;
}

Group::~Group()
{
}

int Group::getLogPointNum(int *num)
{
    *num = static_cast<int>(logPointVector.size());
    return 0;
}

int Group::getLogPoint(double *x, double *y, int count)
{
    int index = 0;
    if (logPointVector.size() > count)
        return ERROR_PARAM_SIZE;

    double *pBufx = new double[count]();
    double *pBufy = new double[count]();
    memset(pBufx, 0, count);
    memset(pBufy, 0, count);

    for (auto &it : logPointVector)
    {
        pBufx[index] = it.first;
        pBufy[index] = it.second;
        index += 1;
    }
    memcpy(x, pBufx, sizeof(double) * count);
    memcpy(y, pBufy, sizeof(double) * count);
    delete[] pBufx;
    pBufx = 0;
    delete[] pBufy;
    pBufy = 0;
    return 0;
}

int Group::getWeightMapSize(int *row, int *col)
{
    if (weightMap.size() == 0)
        return ERROR_IMG_EMPTY;
    *row = (int)weightMap.rows();
    *col = (int)weightMap.cols();
    return 0;
}

int Group::getWeightMap(double *weight, int weightSize)
{
    int index = 0;
    if (weightMap.size() > weightSize)
        return ERROR_PARAM_SIZE;

    double *pBuf = new double[weightSize]();
    memset(pBuf, 0, weightSize);
    for (int i = 0; i < weightMap.rows(); ++i)
    {
        for (int j = 0; j < weightMap.cols(); ++j)
        {
            pBuf[index] = weightMap(i, j);
            index = index + 1;
        }
    }
    memcpy(weight, pBuf, sizeof(double) * weightSize);
    delete[] pBuf;
    pBuf = 0;
    return 0;
}

int Group::getCalibResult(AWBCalibRes *res)
{
    if(greenFixedVector.size()<=4)
    {
        int index=0;
        for(int i=0;i<greenFixedVector.size();++i)
        {
            calibrationRes.green_fixed_BG[index]=greenFixedVector.at(i).first;
            calibrationRes.green_fixed_RG[index]=greenFixedVector.at(i).second;
            index+=1;
        }

    }
    *res = calibrationRes;
    return 0;
}

int Group::setlogPoint(VectorXd x, VectorXd y)
{
    for (int i = 0; i < x.size(); ++i)
    {
        double _xVal = x(i);
        double _yVal = y(i);
        logPointVector.push_back(std::make_pair(_xVal, _yVal));
    }
    return 0;
}

int Group::setGreenFixPara(double BG, double RG)
{
    greenFixedVector.push_back(std::make_pair(BG,RG));
    return 0;
}

int Group::saveGreenMeanPoint(double xMean, double yMean)
{
    Green_xMean=xMean;
    Green_yMean=yMean;
    return 0;

}

int Group::saveGreenLogPoint(Matrix<double, Dynamic, Dynamic> xVal, Matrix<double, Dynamic, Dynamic> yVal)
{
    for(int i=0;i<xVal.rows();++i)
    {
        for(int j=0;j<xVal.cols();++j)
        {
            Green_xVal.push_back(xVal(i,j));
            Green_yVal.push_back(yVal(i,j));
        }
    }
    return 0;
}

int Group::saveGreenMeanGain(float MeanB, float MeanG, float MeanR, uint32_t sumBGR)
{
    _MeanB+=MeanB;
    _MeanG+=MeanG;
    _MeanR+=MeanR;
    _sumBGR+=sumBGR;

    //每次迭代更新
    float logMeanBG=std::log2((float)(_MeanB/_sumBGR)/(float)(_MeanG/_sumBGR));
    float logMeanRG=std::log2((float)(_MeanR/_sumBGR)/(float)(_MeanG/_sumBGR));
    Green_xMean=logMeanBG-logMeanRG;
    Green_yMean=logMeanBG+logMeanRG;
    return 0;
}

int Group::calculateWeightMap()
{
    int TableSizeX = 1;
    int TableSizeY = 1;
    switch (_info.type)
    {
    case IMAGE_NORMAL:
        TableSizeX = 32;
        TableSizeY = 8;
        break;
    case IMAGE_CWF:
        TableSizeX = 8;
        TableSizeY = 6;
        break;
    case IMAGE_GRASS:
        break;
    default:
        break;
    }

    xVal.resize(logPointVector.size());
    yVal.resize(logPointVector.size());
    for(int i=0;i<logPointVector.size();++i)
    {
        xVal(i) = logPointVector.at(i).first;
        yVal(i) = logPointVector.at(i).second;
    }

    double borderUp = yVal.maxCoeff();
    double borderRight = xVal.maxCoeff();
    double borderLeft = xVal.minCoeff();
    double borderBottom = yVal.minCoeff();

    double TableL = borderLeft - _info.Margin;
    double TableR = borderRight + _info.Margin;
    double TableU = borderUp + _info.Margin;
    double TableD = borderBottom - _info.Margin;
    double Xstep = (TableR - TableL) / (double)(TableSizeX - 1.0);
    double Ystep = (TableU - TableD) / (double)(TableSizeY - 1.0);
    int Nums = (int)xVal.size();

    Matrix<double, 1, Dynamic> X;
    Matrix<double, 1, Dynamic> Y;
    X.resize(1, TableSizeX);
    double xtemp = TableL;
    for (int i = 0; i < TableSizeX; i++)
    {
        X(0, i) = xtemp;
        xtemp += Xstep;
    }
    double ytemp = TableU;
    Y.resize(1, TableSizeY);
    for (int i = 0; i < TableSizeY; i++)
    {
        Y(0, i) = ytemp;
        ytemp -= Ystep;
    }

    // setTable
    table.resize(Y.size() + 4, X.size() + 6);
    table.setZero();
    int BlockNumX = 3;
    int BlockNumY = 2;
    for (int k = 0; k < Nums; k++)
    {
        Matrix<double, 1, Dynamic> vx;
        vx.resize(1, X.cols());
        vx.fill(xVal(k));
        auto retx = (X - vx).cwiseAbs();
        MatrixXd::Index minRowx, minColx;
        retx.minCoeff(&minRowx, &minColx);
        int addressX = (int)minColx + 3;

        Matrix<double, 1, Dynamic> vy;
        vy.resize(1, Y.cols());
        vy.fill(yVal(k));
        auto rety = (Y - vy).cwiseAbs();
        MatrixXd::Index minRowy, minColy;
        rety.minCoeff(&minRowy, &minColy);
        int addressY = (int)minColy + 2;

        int Top = addressY - BlockNumY;
        int Bottom = addressY + BlockNumY;
        int Left = addressX - BlockNumX;
        int Right = addressX + BlockNumX;
        Matrix<double, Dynamic, Dynamic> temp;
        if(!blockValid(table,Top, Left, (Bottom - Top + 1), (Right - Left + 1)))
            return ERROR_PARAM_SIZE;
        temp = table.block(Top, Left, (Bottom - Top + 1), (Right - Left + 1));
        Matrix<double, Dynamic, Dynamic> midTemp = (temp.array() > kernel.array()).cast<double>();
        midTemp=midTemp.cwiseProduct(temp);
        Matrix<double, Dynamic, Dynamic> midKernel = (kernel.array()>=temp.array()).cast<double>();
        midKernel = midKernel.cwiseProduct(kernel);
        temp= midTemp + midKernel;
        table.block(Top, Left, (Bottom - Top + 1), (Right - Left + 1)) = temp;
    }
    //_tab_bor
    double TableBorL = (double)std::round(TableL * 2048);
    double TableBorT = (double)std::round(TableU * 2048);
    // r_div_step
    double DivStepX =(double)std::round(pow(2, 16) / (double)(2048.0 * Xstep));
    double DivStepY =(double) std::round(pow(2, 16) / (double)(2048.0 * Ystep));
    //r_table_[][]
    weightMap=table.block(2, 3, table.rows()-4, table.cols()-6);

    //赋值
    if (_info.type == IMAGE_CWF)
    {
        // r_cwf_line_x
        double linex0 =(double) round(TableL * 2048);
        double linex1 =(double) round(TableR * 2048);
        double liney0 = (double)round(TableU * 2048);
        double liney1 =(double) round(TableD * 2048);
        calibrationRes.cwf_tab_bor[0]=TableBorL;
        calibrationRes.cwf_tab_bor[1]=TableBorT;
        calibrationRes.cwf_div_step[0]=DivStepX;
        calibrationRes.cwf_div_step[1]=DivStepY;
        calibrationRes.cwf_line_x[0]=linex0;
        calibrationRes.cwf_line_x[1]=linex1;
        calibrationRes.cwf_line_y[0]=liney0;
        calibrationRes.cwf_line_y[1]=liney1;
        if(weightMap.size()<=48)
        {
            int index=0;
            for (int i = 0; i < weightMap.rows(); ++i)
            {
                for (int j = 0; j < weightMap.cols(); ++j)
                {
                    calibrationRes.cwf_table[index] = weightMap(i, j);
                    index = index + 1;
                }
            }
        }
    }
    if(_info.type == IMAGE_NORMAL)
    {
        calibrationRes.nor_tab_bor[0]=TableBorL;
        calibrationRes.nor_tab_bor[1]=TableBorT;
        calibrationRes.nor_div_step[0]=DivStepX;
        calibrationRes.nor_div_step[1]=DivStepY;
        if(weightMap.size()<=256)
        {
            int index=0;
            for (int i = 0; i < weightMap.rows(); ++i)
            {
                for (int j = 0; j < weightMap.cols(); ++j)
                {
                    calibrationRes.nor_table[index] = weightMap(i, j);
                    index = index + 1;
                }
            }
        }
    }

    return 0;
}

int Group::getGreenFixPara(double *BG, double *RG,int size)
{
    int size_t=(int)(greenFixedVector.size());
    if(size!=size_t)
        return ERROR_PARAM_SIZE;
    std::vector<double> _BG;
    std::vector<double> _RG;
    for(int i=0;i<size_t;++i)
    {
        _BG.push_back(greenFixedVector.at(i).first);
        _RG.push_back(greenFixedVector.at(i).second);
    }
    memcpy(BG, _BG.data(), sizeof(double) * size_t);
    memcpy(RG, _RG.data(), sizeof(double) * size_t);
    return 0;
}

int Group::getGreenLogPointNum(int *num)
{
    *num=(int)Green_xVal.size();
    return 0;
}

int Group::getGreenLogPoint(double *logx, double *logy, int num)
{

    int size=(int)(Green_xVal.size());
    if(num!=size)
        return ERROR_PARAM_SIZE;
    std::vector<double> _logx;
    std::vector<double> _logy;

    for(int i=0;i<size;++i)
    {
        _logx.push_back(Green_xVal.at(i));
        _logy.push_back(Green_yVal.at(i));
    }
    memcpy(logx, _logx.data(), sizeof(double) * size);
    memcpy(logy, _logy.data(), sizeof(double) * size);

    return 0;
}

int Group::getGreenLogMeanPoint(double *meanx, double*meany)
{
    *meanx=Green_xMean;
    *meany=Green_yMean;
    return 0;
}
int Group::getGreenFixParaNum(int* fixedNum)
{
    *fixedNum=(int)greenFixedVector.size();
    return 0;
}

void Group::clear()
{
    logPointVector.clear();
    weightMap.setZero();
    table.setZero();
    Green_xVal.clear();
    Green_yVal.clear();
    greenFixedVector.clear();
    calibrationRes=AWBCalibRes();
}

bool Group::blockValid(Matrix<double, Dynamic, Dynamic> matrix, int startx, int starty, int x_zise, int y_size)
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



int Group::clearHighLumaVal()
{
    Green_xVal.clear();
    Green_yVal.clear();
    _MeanB=0;
    _MeanG=0;
    _MeanR=0;
    _sumBGR=0;
    return 0;
}

ImageType Group::groupType()
{
    return _info.type;
}

