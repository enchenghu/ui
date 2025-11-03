#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "QFileDialog"
#include "stdio.h"
#include "QRegularExpressionValidator"
#include "QRegularExpression"
#include "QMessageBox"
#include "imagewidget.h"
#include "QButtonGroup"
#include "AWBCalibrationDef.h"
#include <fstream>
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "calibration.h"
#include "M1_AWBCalibTool/src/M1_AWBTool.h"
#include "M1_AWBCalibTool/src/M1AWBTypeDef.h"
#include <shlobj.h>
#include <windows.h>
#include "awbburn.h"

namespace Ui {
class MainWindow;
}

enum RawFormat{RGGB,GRBG,GBRG,BGGR,RGBIR};

//像素结构体
struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#define NORMAL_START_L 0
#define NORMAL_START_M 260
#define CWF_START_L 1000
#define CWF_START_M 1056
#define GREEN_START_L 2000
#define GREEN_START_M 2004
#define GREEN_FW 2008

struct rawInfo{
    QString rawFilePath;
    std::vector<QRectF> roiRect;
};

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //raw图转Pixmap
    int rawToPixmap(QPixmap &pixMap,const char* path,size_t width,size_t height);
    //raw图字节流转RGB888
    int rawToRGB888(const std::vector<uint16_t> vecSource,std::vector<uint8_t>& vecDest,size_t width,size_t height);
    //接收并显示图片的函数
    void recvShowPicSignal(QPixmap pix);
    //设置主界面显示图及图名
    int setGraphicName(size_t nIndex);
    //字符串过长自动换行
    QString autoWrap(QString str,size_t nLength=16);
    //打开输入文件夹
    void openSourcePicPath(QString srcDirPath);
    //打开raw文件
    int openRawData(const char* srcDirPath,std::vector<uint16_t>& rawDoubleData,size_t width,size_t height);
    //输出TXT
    void outputTxt(QString srcDirPath, QString strTime);
    //输出excel
    void outputExcel(QString srcDirPath, QString strTime);
    //各场景下写入寄存器
    void normal2Reg();
    void cwf2Reg();
    void green2Reg();
    //各场景下获取寄存器地址并将“地址，值”保存至txt同路径文件夹
    void normalGet2Txt(QString srcDirPath, QString strTime);
    void cwfGet2Txt(QString srcDirPath, QString strTime);
    void greenGet2Txt(QString srcDirPath, QString strTime);
    //保存图片
    void outputImage(QString srcDirPath);

    //将单个寄存器键值对保存至txt
    void saveOneRegisterValue2Txt(std::ofstream *outFile, uint32_t nStart, double value, uint32_t cameraId);
    //calibration散点图
    void calibrationOpen();
    //检测重复Rect，若不重复，则放入数组内
    bool checkRepeatRect(std::vector<QRectF>& vecRect,QRectF rect);

    //RGBIR转BGGR
    void rgbir2BGGR(std::vector<std::vector<uint16_t>>& rawDoubleData,size_t width,size_t height);
    //此函数功能同上，只是输入数组为一维
    void rgbir2BGGR(std::vector<uint16_t>& rawDoubleData,size_t width,size_t height);

private slots:
    void modelChanged();
    //点击输入文件夹按钮
    void inputPathFolder();
    //完成输入文件夹路径框
    void inputPathFolderFromEdit();
    //是否显示缩略图
    void isShowThumb();
    //缩略图索引点击
    void thumbItemChange();
    //缩略图索引改变
    void thumbCurrentItemChange();
    //切换上/下一张
    void previousPic();
    void nextPic();
    //长宽输入改变
    void widthEditChange();
    void heightEditChange();
    //raw图bit改变
    void bitRadioChange();
    //raw图format改变
    void rawFormatChange();
    //output按钮
    void txtOutputClick();
    //outputTxt文件路径框改变
    void txtPathChanged();
    //获取标定结果
    void getROIResult();
    //luma设定按钮
    void setHighLuma();
    //写入寄存器按钮
    void write2Reg();
    //当前ROI应用到全图
    void applyAllBtn();
    //ROI导入
    void roiImport();
    //ROI导出
    void roiExport();

signals:
    void addMean();

private:
    Ui::MainWindow *ui;
    ImageWidget    *_Image;
    std::vector<rawInfo> _vecRawInfo;   //图片文件信息
    int _picWidth;
    int _picHeight;
    QButtonGroup * _rawBitGroup;          //raw图bit按钮组
    int _nRawBit;                         //当前raw图bit
    int _nRawFormat;
    // Img _img;
    AWBCalibRes   _AWBData;              //计算后的结构体数据
    int _highLumaIndex;                 //最高luma索引
    bool _isCalculated;                 //当前组是否被计算过
    bool _isHighLuma;                   //是否HighLuma模式
    std::vector<PointInfo> _calibrationVecResult;   //散点图框选范围
    std::vector<std::pair<QString,Img*>> vecImgName;
    std::vector<Img> vecImg;
    ImageType currentType;
};

#endif // MAINWINDOW_H
