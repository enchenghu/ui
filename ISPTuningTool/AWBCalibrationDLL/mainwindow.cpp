#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //最大/小化按钮
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;
    setWindowFlags(windowFlag);

    //Luma设置
    ui->setLumaBtn->setVisible(false);
    _isHighLuma=false;
    _highLumaIndex=-1;
    _isCalculated=false;

    _Image=NULL;
    //绑定raw图bit的radio按钮
    _rawBitGroup=new QButtonGroup();
    _rawBitGroup->addButton(ui->bit8Radio);
    _rawBitGroup->addButton(ui->bit10Radio);
    _rawBitGroup->addButton(ui->bit12Radio);
    _rawBitGroup->addButton(ui->bit14Radio);
    _rawBitGroup->addButton(ui->bit16Radio);
    _nRawBit=10;
    ui->bit10Radio->setChecked(true);
    //raw图format初始化
    _nRawFormat=0;
    //size只能输入数字
    ui->sizeWidth->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));//输入0到9
    ui->sizeHeight->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));//输入0到9
    //BLC只能输入数字
    QIntValidator *validator = new QIntValidator(0, 999999, this);
    ui->blcEdit->setValidator(validator);
    //Margin只能输入小数
     QDoubleValidator *validator1 = new QDoubleValidator(0.0, 999.99, 2, this);
    ui->marginEdit->setValidator(validator1);
    //初始化size
    _picWidth=ui->sizeWidth->text().toInt();
    _picHeight=ui->sizeHeight->text().toInt();
    //设置缩略图列表
    ui->thumbList->setIconSize(QSize(130,100));
    ui->thumbList->setViewMode(QListView::IconMode);
    ui->thumbList->setResizeMode(QListWidget::Adjust);   //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    ui->thumbList->setMovement(QListWidget::Static);    //设置不能移动
    ui->thumbList->setWrapping(true);						//自动换行
    ui->thumbList->setFlow(QListView::LeftToRight);		//从左到右布局
    ui->outFormatCombo->setCurrentIndex(3);
    ui->blcEdit->setText("64");
    //槽绑定
    connect(ui->modelCombo,&QComboBox::currentIndexChanged,this,&MainWindow::modelChanged);
    connect(ui->inputFolderBtn,&QPushButton::clicked,this,&MainWindow::inputPathFolder);
    connect(ui->inputFolderEdit,&QLineEdit::editingFinished,this,&MainWindow::inputPathFolderFromEdit);
    connect(ui->thumbCheck,&QCheckBox::stateChanged,this,&MainWindow::isShowThumb);
    connect(ui->thumbList, &QListWidget::itemClicked, this, &MainWindow::thumbItemChange);
    connect(ui->previousBtn,&QPushButton::clicked,this,&MainWindow::previousPic);
    connect(ui->nextBtn,&QPushButton::clicked,this,&MainWindow::nextPic);
    connect(ui->sizeWidth,&QLineEdit::editingFinished,this,&MainWindow::widthEditChange);
    connect(ui->sizeHeight,&QLineEdit::editingFinished,this,&MainWindow::heightEditChange);
    connect(_rawBitGroup,&QButtonGroup::buttonClicked,this,&MainWindow::bitRadioChange);
    connect(ui->outFormatCombo,&QComboBox::currentIndexChanged,this,&MainWindow::rawFormatChange);
    connect(ui->calibrationPathBtn,&QPushButton::clicked,this,&MainWindow::txtOutputClick);
    connect(ui->finishROIBtn,&QPushButton::clicked,this,&MainWindow::getROIResult);
    connect(ui->setLumaBtn,&QPushButton::clicked,this,&MainWindow::setHighLuma);
    connect(ui->WriteBtn,&QPushButton::clicked,this,&MainWindow::write2Reg);
    connect(ui->calibrationPathEdit,&QLineEdit::editingFinished,this,&MainWindow::txtPathChanged);
    connect(ui->thumbList, &QListWidget::currentRowChanged, this, &MainWindow::thumbCurrentItemChange);
    connect(ui->applyAllButton,&QPushButton::clicked,this,&MainWindow::applyAllBtn);
    connect(ui->roiImportBtn,&QPushButton::clicked,this,&MainWindow::roiImport);
    connect(ui->roiExportBtn,&QPushButton::clicked,this,&MainWindow::roiExport);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::rawToPixmap(QPixmap &pixMap,const char* path,size_t width,size_t height)
{
    std::vector<uint16_t> double_data;
    int nRet=openRawData(path,double_data,width,height);
    if(nRet!=ERROR_NO_ERR)
    {
        return nRet;
    }

    //目标数据，将原始raw数据转为rgb数据
    std::vector<uint8_t> rgb_data;
    nRet=rawToRGB888(double_data,rgb_data,width,height);
    if(nRet != ERROR_NO_ERR)
    {
        return nRet;
    }

    QImage img((uint8_t*)&rgb_data[0],(int)width,(int)height,QImage::Format_BGR888);//转换图像
    pixMap = QPixmap::fromImage(img);
    return ERROR_NO_ERR;
}

int MainWindow::rawToRGB888(const std::vector<uint16_t> vecSource, std::vector<uint8_t> &vecDest, size_t width, size_t height)
{
    int nBitMove=0;
    switch (_nRawBit) {
    case 8:
        nBitMove=0;
        break;
    case 10:
        nBitMove=2;
        break;
    case 12:
        nBitMove=4;
        break;
    case 14:
        nBitMove=6;
        break;
    case 16:
        nBitMove=8;
        break;
    default:
        nBitMove=0;
        break;
    }
    vecDest.resize(0);
    //临时源数组，转为二维矩阵模式
    std::vector<std::vector<uint16_t>> image(height,std::vector<uint16_t>(width));
    size_t nRow=vecSource.size()/width;
    for (int var = 0; var < nRow; ++var)
    {
        memcpy((uint16_t*)&image[var][0],(uint16_t*)&vecSource[var*width],width*2);
    }

    //临时目标数组，存放RGB值
    std::vector<std::vector<Pixel>> rgbImage(height, std::vector<Pixel>(width));

    //计算
    int indexModel=ui->outFormatCombo->currentIndex();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int Gcount = 0, Rcount = 0, Bcount = 0;
            bool Green=false;
            //GBRG格式
            bool Red=false;
            bool Blue=false;
            uint16_t temporary_R = 0, temporary_G = 0, temporary_B = 0; // 防止溢出16位来让八位累加！！
            switch (indexModel) {
            case RawFormat::RGGB:
                Green = ((i % 2 == 0) && (j % 2 == 1)) || ((i % 2 == 1) && (j % 2 == 0));
                Red = (i % 2 == 0) && (j % 2 == 0);
                Blue = (i % 2 == 1) && (j % 2 == 1);
                if (Green)
                {
                    for (int x = i - 1; x <= i + 1; x++)
                    {
                        for (int y = j - 1; y <= j + 1; y++)
                        {
                            // 判断边界内有效数据
                            if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                            {
                                if ((i % 2 == 0)) // 偶数行红蓝读取
                                {
                                    // 红色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                                if ((i % 2 == 1))
                                {
                                    // 红色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                            }
                        }
                    }
                    rgbImage[i][j].g = (image[i][j] / (uint16_t)1) >> nBitMove;
                    rgbImage[i][j].r = (temporary_R / (uint16_t)Rcount) >> nBitMove;
                    rgbImage[i][j].b = (temporary_B / (uint16_t)Bcount) >> nBitMove;
                }
                break;
            case RawFormat::GRBG:
                Green = ((i % 2 == 0) && (j % 2 == 0)) || ((i % 2 == 1) && (j % 2 == 1));
                Red = (i % 2 == 0) && (j % 2 == 1);
                Blue = (i % 2 == 1) && (j % 2 == 0);
                if (Green)
                {
                    for (int x = i - 1; x <= i + 1; x++)
                    {
                        for (int y = j - 1; y <= j + 1; y++)
                        {
                            // 判断边界内有效数据
                            if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                            {
                                if ((i % 2 == 0)) // 偶数行红蓝读取
                                {
                                    // 红色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                                if ((i % 2 == 1))
                                {
                                    // 红色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                            }
                        }
                    }
                    rgbImage[i][j].g = (image[i][j] / (uint16_t)1) >> nBitMove;
                    rgbImage[i][j].r = (temporary_R / (uint16_t)Rcount) >> nBitMove;
                    rgbImage[i][j].b = (temporary_B / (uint16_t)Bcount) >> nBitMove;
                }
                break;
            case RawFormat::GBRG:
                Green = ((i % 2 == 0) && (j % 2 == 0)) || ((i % 2 == 1) && (j % 2 == 1));
                Red = (i % 2 == 1) && (j % 2 == 0);
                Blue = (i % 2 == 0) && (j % 2 == 1);
                if (Green)
                {
                    for (int x = i - 1; x <= i + 1; x++)
                    {
                        for (int y = j - 1; y <= j + 1; y++)
                        {
                            // 判断边界内有效数据
                            if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                            {
                                if ((i % 2 == 0)) // 偶数行红蓝读取
                                {
                                    // 红色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                                if ((i % 2 == 1))
                                {
                                    // 红色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                            }
                        }
                    }
                    rgbImage[i][j].g = (image[i][j] / (uint16_t)1) >> nBitMove;
                    rgbImage[i][j].r = (temporary_R / (uint16_t)Rcount) >> nBitMove;
                    rgbImage[i][j].b = (temporary_B / (uint16_t)Bcount) >> nBitMove;
                }
                break;
            case RawFormat::RGBIR:
            case RawFormat::BGGR:
                Green = ((i % 2 == 0) && (j % 2 == 1)) || ((i % 2 == 1) && (j % 2 == 0));
                Red = (i % 2 == 1) && (j % 2 == 1);
                Blue = (i % 2 == 0) && (j % 2 == 0);
                if (Green)
                {
                    for (int x = i - 1; x <= i + 1; x++)
                    {
                        for (int y = j - 1; y <= j + 1; y++)
                        {
                            // 判断边界内有效数据
                            if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                            {
                                if ((i % 2 == 0)) // 偶数行红蓝读取
                                {
                                    // 红色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                                if ((i % 2 == 1))
                                {
                                    // 红色
                                    if ((x == i && y == j - 1) || (x == i && y == j + 1))
                                    {
                                        temporary_R += image[x][y];
                                        Rcount++;
                                    }
                                    // 蓝色
                                    if ((x == i - 1 && y == j) || (x == i + 1 && y == j))
                                    {
                                        temporary_B += image[x][y];
                                        Bcount++;
                                    }
                                }
                            }
                        }
                    }
                    rgbImage[i][j].g = (image[i][j] / (uint16_t)1) >> nBitMove;
                    rgbImage[i][j].r = (temporary_R / (uint16_t)Rcount) >> nBitMove;
                    rgbImage[i][j].b = (temporary_B / (uint16_t)Bcount) >> nBitMove;
                }
                break;
            default:
                break;
            }

            if (Blue)
            {
                for (int x = i - 1; x <= i + 1; x++)
                {
                    for (int y = j - 1; y <= j + 1; y++)
                    {
                        if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                        {
                            // 绿色插值上下左右
                            if ((x == i && y == j - 1) || (x == i && y == j + 1) || (x == i - 1 && y == j) || (x == i + 1 && y == j))
                            {
                                temporary_G += image[x][y];
                                Gcount++;
                            }
                            // 对红色四个角插值
                            if ((x == i - 1 && y == j - 1) || (x == i + 1 && y == j - 1) || (x == i - 1 && y == j + 1) || (x == i + 1 && y == j + 1))
                            {
                                temporary_R += image[x][y];
                                Rcount++;
                            }
                        }
                    }
                }
                rgbImage[i][j].g = (temporary_G / (uint16_t)Gcount) >> nBitMove;
                rgbImage[i][j].r = (temporary_R / (uint16_t)Rcount) >> nBitMove;
                rgbImage[i][j].b = (image[i][j] / (uint16_t)1) >> nBitMove;
            }
            if (Red)
            {
                for (int x = i - 1; x <= i + 1; x++)
                {
                    for (int y = j - 1; y <= j + 1; y++)
                    {
                        if (x >= 0 && x < height && y >= 0 && y < width && !(x == i && y == j))
                        {
                            // 绿色插值上下左右
                            if ((x == i && y == j - 1) || (x == i && y == j + 1) || (x == i - 1 && y == j) || (x == i + 1 && y == j))
                            {
                                temporary_G += image[x][y];
                                Gcount++;
                            }
                            // 对四角蓝色插值
                            if ((x == i - 1 && y == j - 1) || (x == i + 1 && y == j - 1) || (x == i - 1 && y == j + 1) || (x == i + 1 && y == j + 1))
                            {
                                temporary_B += image[x][y];
                                Bcount++;
                            }
                        }
                    }
                }
                rgbImage[i][j].g = (temporary_G / (uint16_t)Gcount) >> nBitMove;
                rgbImage[i][j].r = (image[i][j] / (uint16_t)1) >> nBitMove;
                rgbImage[i][j].b = (temporary_B / (uint16_t)Bcount) >> nBitMove;
            }
        }
    }

    //存入RGB
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            vecDest.push_back(rgbImage[i][j].b);
            vecDest.push_back(rgbImage[i][j].g);
            vecDest.push_back(rgbImage[i][j].r);
        }
    }

    return ERROR_NO_ERR;
}

void MainWindow::recvShowPicSignal(QPixmap pix)
{
    QGraphicsScene* tempScene=ui->ImageGraphic->scene();
    if(tempScene!=nullptr)
    {
        if(_Image!=nullptr)
        {
            delete _Image;
            _Image=nullptr;
        }
        ui->ImageGraphic->setScene(nullptr);
        delete tempScene;
        tempScene=nullptr;
    }
    bool isGreen=ui->modelCombo->currentIndex()==2;

    QGraphicsScene  *qgraphicsScene = new QGraphicsScene;//要用QGraphicsView就必须要有QGraphicsScene搭配着用
    int nIndex=ui->thumbList->currentRow();
    _Image = new ImageWidget(&pix,_vecRawInfo.at(nIndex).roiRect,isGreen);//实例化类ImageWidget的对象_Image，该类继承自QGraphicsItem，是自己写的类
    int nwith = ui->ImageGraphic->width();//获取界面控件Graphics View的宽度
    int nheight = ui->ImageGraphic->height();//获取界面控件Graphics View的高度
    _Image->setQGraphicsViewWH(nwith,nheight);//将界面控件Graphics View的width和height传进类_Image中
    _Image->setXYValue(ui->xValueLabel,ui->yValueLabel);
    qgraphicsScene->addItem(_Image);//将QGraphicsItem类对象放进QGraphicsScene中
    ui->ImageGraphic->setSceneRect(QRectF(-(nwith/2),-(nheight/2),nwith,nheight));//使视窗的大小固定在原始大小，不会随图片的放大而放大（默认状态下图片放大的时候视窗两边会自动出现滚动条，并且视窗内的视野会变大），防止图片放大后重新缩小的时候视窗太大而不方便观察图片
    ui->ImageGraphic->setScene(qgraphicsScene);//Sets the current scene to scene. If scene is already being viewed, this function does nothing.
    ui->ImageGraphic->setFocus();//将界面的焦点设置到当前Graphics View控件
    ui->ImageGraphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
    ui->ImageGraphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
}

int MainWindow::setGraphicName(size_t nIndex)
{
    if(_picWidth==0 || _picHeight==0)
    {
        QMessageBox::information(this, "Warning", "Width or Height can't be empty or 0!", QMessageBox::Ok);
        return ERROR_NUM;
    }
    QPixmap pix;
    int nRet=rawToPixmap(pix,_vecRawInfo[nIndex].rawFilePath.toLatin1().data(),_picWidth,_picHeight);
    if(nRet!=ERROR_NO_ERR)
    {
        return nRet;
    }
    recvShowPicSignal(pix);
    QString str=_vecRawInfo[nIndex].rawFilePath;
    str=str.right(str.size() - (str.lastIndexOf("/")+1));
    ui->photoNameLabel->setText(str);

    return ERROR_NO_ERR;
}

QString MainWindow::autoWrap(QString str, size_t nLength)
{
    if(str.length()<(int)nLength)
    {
        return str;
    }
    str=str.insert(nLength,"\n");
    return str;
}

void MainWindow::openSourcePicPath(QString srcDirPath)
{
    QDir directory(srcDirPath);
    // 过滤器，只保留图片文件
    QStringList filters;
    filters << "*.raw";
    directory.setNameFilters(filters);

    //清除旧图片
    ui->thumbList->clear();
    QGraphicsScene* tempScene=ui->ImageGraphic->scene();
    if(tempScene!=nullptr)
    {
        ui->ImageGraphic->setScene(nullptr);
        delete tempScene;
        tempScene=nullptr;
    }
    _vecRawInfo.resize(0);
    ui->photoNameLabel->setText("");

    bool isHaveFile=false;
    bool isShowPic=ui->thumbCheck->isChecked();
    std::vector<rawInfo> tempInfo;
    foreach (const QFileInfo &fileInfo, directory.entryInfoList()) {
        if (fileInfo.isFile()) {
            rawInfo info;
            info.rawFilePath=fileInfo.absoluteFilePath();
            tempInfo.push_back(info);
            QListWidgetItem *pItem = new QListWidgetItem;
            if(isShowPic)
            {
                QPixmap pix;
                int nRet=rawToPixmap(pix,fileInfo.absoluteFilePath().toLatin1().data(),_picWidth,_picHeight);
                if(nRet!=ERROR_NO_ERR)
                {
                    ui->thumbList->clear();
                    return ;
                }
                QIcon icon(pix);
                pItem->setIcon(icon);
            }
            pItem->setText(autoWrap(fileInfo.fileName()));
            ui->thumbList->addItem(pItem);
            isHaveFile=true;
        }
    }
    if(!isHaveFile)
    {
        QMessageBox::information(this, "Warning", "Not found RAW!", QMessageBox::Ok);
        return;
    }

    _vecRawInfo.swap(tempInfo);
    ui->thumbList->setCurrentRow(0);
    setGraphicName(0);
}

int MainWindow::openRawData(const char* srcDirPath, std::vector<uint16_t> &rawDoubleData,size_t width,size_t height)
{
    std::ifstream in;
    in.open(srcDirPath,std::ios::in | std::ios::binary);
    if(!in.is_open())
    {
        return ERROR_FILE;
    }
    in.seekg(0,std::ios::end);
    int nFileLen=in.tellg();

    int nBitNum=1;
    if(_nRawBit > 8)
    {
        nBitNum=2;
    }
    size_t size = width * height * nBitNum;
    if(size > nFileLen)
    {
        QMessageBox::information(this, "Warning", "Width or Height or RawBit is wrong!", QMessageBox::Ok);
        in.close();
        return ERROR_FILE;
    }

    //raw原始数据，需要将读到的像素映射到uint16的数组中，以便计算
    if(_nRawBit==8)
    {
        rawDoubleData.resize(0);
        std::vector<uint8_t> raw_data;
        raw_data.resize(width * height);
        in.seekg(0);
        in.read((char*)&raw_data[0],width * height);
        //转为uint16数组
        for (int var = 0; var < width * height; var++)
        {
            rawDoubleData.push_back((uint16_t)raw_data[var]);
        }
    }
    else if(_nRawBit > 8)
    {
        //raw10及更大像素处理
        rawDoubleData.resize(width * height);
        //2个字节为一像素读取
        size_t nTempSize=width * height * nBitNum;
        std::vector<uint8_t> doubleData(nTempSize);
        in.seekg(0);
        in.read((char*)&doubleData[0],nTempSize);
        memcpy((char*)&rawDoubleData[0],(char*)&doubleData[0],width * height * nBitNum);
    }
    in.close();

    //若是RGBIR还需搬运一下像素
    if(ui->outFormatCombo->currentIndex()==RawFormat::RGBIR)
    {
        rgbir2BGGR(rawDoubleData,width,height);
    }

    return ERROR_NO_ERR;
}

void MainWindow::normal2Reg()
{
    uint32_t nStart=0;
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=NORMAL_START_M;
    }
    else
    {
        nStart=NORMAL_START_L;
    }
    AWBBurn::writing(nStart++,-1,_AWBData.nor_tab_bor[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.nor_tab_bor[1]);
    AWBBurn::writing(nStart++,-1,_AWBData.nor_div_step[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.nor_div_step[1]);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 32; ++j) {
            AWBBurn::writing(nStart++,-1,_AWBData.nor_table[i*32+j]);
        }
    }
    QMessageBox::information(this, "Success", "Write Finished!", QMessageBox::Ok);
    return;
}

void MainWindow::cwf2Reg()
{
    uint32_t nStart=0;
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=CWF_START_M;
    }
    else
    {
        nStart=CWF_START_L;
    }
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_tab_bor[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_tab_bor[1]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_div_step[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_div_step[1]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_line_x[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_line_x[1]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_line_y[0]);
    AWBBurn::writing(nStart++,-1,_AWBData.cwf_line_y[1]);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 8; ++j) {
            AWBBurn::writing(nStart++,-1,_AWBData.cwf_table[i*8+j]);
        }
    }
    QMessageBox::information(this, "Success", "Write Finished!", QMessageBox::Ok);
    return;
}

void MainWindow::green2Reg()
{
    if(_calibrationVecResult.size()<2)
    {
        QMessageBox::information(this, "Warning", "Please get Calibration ROI!", QMessageBox::Ok);
        return;
    }
    uint32_t nStart=0;
    //是否双目
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=GREEN_START_M;
    }
    else
    {
        nStart=GREEN_START_L;
    }
    AWBBurn::writing(nStart++,-1,_calibrationVecResult[0].x*2048);
    AWBBurn::writing(nStart++,-1,_calibrationVecResult[1].x*2048);
    AWBBurn::writing(nStart++,-1,_calibrationVecResult[0].y*2048);
    AWBBurn::writing(nStart++,-1,_calibrationVecResult[1].y*2048);
    uint32_t nCount=GREEN_FW;
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_BG[0]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_BG[1]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_BG[2]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_BG[3]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_RG[0]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_RG[1]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_RG[2]);
    AWBBurn::writing(nCount++,0,_AWBData.green_fixed_RG[3]);
    QMessageBox::information(this, "Success", "Write Finished!", QMessageBox::Ok);
    return;
}

void MainWindow::normalGet2Txt(QString srcDirPath, QString strTime)
{
    std::ofstream outFile;
    srcDirPath=srcDirPath.left((srcDirPath.lastIndexOf(".")));
    srcDirPath+="_addr-value"+strTime+".txt";
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::trunc);
    uint32_t nStart=0;
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=NORMAL_START_M;
    }
    else
    {
        nStart=NORMAL_START_L;
    }
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.nor_tab_bor[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.nor_tab_bor[1],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.nor_div_step[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.nor_div_step[1],-1);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 32; ++j) {
            saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.nor_table[i*32+j],-1);
        }
    }

    outFile.close();
}

void MainWindow::cwfGet2Txt(QString srcDirPath, QString strTime)
{
    std::ofstream outFile;
    srcDirPath=srcDirPath.left((srcDirPath.lastIndexOf(".")));
    srcDirPath+="_addr-value"+strTime+".txt";
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::trunc);
    uint32_t nStart=0;
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=CWF_START_M;
    }
    else
    {
        nStart=CWF_START_L;
    }
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_tab_bor[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_tab_bor[1],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_div_step[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_div_step[1],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_line_x[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_line_x[1],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_line_y[0],-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_line_y[1],-1);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 8; ++j) {
            saveOneRegisterValue2Txt(&outFile,nStart++,_AWBData.cwf_table[i*8+j],-1);
        }
    }

    outFile.close();
}

void MainWindow::greenGet2Txt(QString srcDirPath, QString strTime)
{
    if(_calibrationVecResult.size()<2)
    {
        QMessageBox::information(this, "Warning", "Please get Calibration ROI!", QMessageBox::Ok);
        return;
    }
    std::ofstream outFile;
    srcDirPath=srcDirPath.left((srcDirPath.lastIndexOf(".")));
    srcDirPath+="_addr-value"+strTime+".txt";
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::trunc);
    uint32_t nStart=0;
    if(ui->isBinocularCheckBox->isChecked())
    {
        nStart=GREEN_START_M;
    }
    else
    {
        nStart=GREEN_START_L;
    }
    saveOneRegisterValue2Txt(&outFile,nStart++,_calibrationVecResult[0].x*2048,-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_calibrationVecResult[1].x*2048,-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_calibrationVecResult[0].y*2048,-1);
    saveOneRegisterValue2Txt(&outFile,nStart++,_calibrationVecResult[1].y*2048,-1);
    uint32_t nCount=GREEN_FW;
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_BG[0],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_BG[1],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_BG[2],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_BG[3],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_RG[0],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_RG[1],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_RG[2],0);
    saveOneRegisterValue2Txt(&outFile,nCount++,_AWBData.green_fixed_RG[3],0);

    outFile.close();
}

void MainWindow::saveOneRegisterValue2Txt(std::ofstream *outFile, uint32_t nStart, double value, uint32_t cameraId)
{
    uint32_t registe[8];
    uint32_t mask[8];
    uint8_t registeNum;
    uint32_t tempValue;
    AWBBurn::getting(nStart++,cameraId,registe,mask,&registeNum);
    QString strWrite="";
    for (int var = 0; var < registeNum; ++var) {
        strWrite="0x"+QString::number(registe[var],16)+",";
        tempValue=value;
        tempValue=(tempValue>>(var*8))&(mask[var]);
        strWrite+="0x"+QString::number(tempValue,16).rightJustified(2, '0')+"\n";
        outFile->write(strWrite.toStdString().c_str(),strWrite.length());
    }
}

void MainWindow::thumbItemChange()
{
    size_t nRow=ui->thumbList->currentRow();
    setGraphicName(nRow);
}

void MainWindow::thumbCurrentItemChange()
{
    if(ui->modelCombo->currentIndex()==2)
    {
        if(ui->thumbList->currentRow()==_highLumaIndex)
        {
            ui->setLumaBtn->setText("CancelHighLuma");
        }
        else
        {
            ui->setLumaBtn->setText("SetHighLuma");
        }
    }
}

void MainWindow::previousPic()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    //上一个
    int nRow=ui->thumbList->currentRow();
    nRow-=1;
    if(nRow<0)
    {
        nRow=0;
    }
    ui->thumbList->setCurrentRow(nRow);
    setGraphicName(nRow);
}

void MainWindow::nextPic()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    //下一个
    int Count=ui->thumbList->count();
    int nRow=ui->thumbList->currentRow();
    nRow+=1;
    if(nRow>=Count)
    {
        nRow=Count-1;
    }
    ui->thumbList->setCurrentRow(nRow);
    setGraphicName(nRow);
}

void MainWindow::widthEditChange()
{
    QString str=ui->sizeWidth->text();
    if(str.toInt()==0||str=="")
    {
        QMessageBox::information(this, "Warning", "Width can't be empty or 0!", QMessageBox::Ok);
        ui->sizeWidth->setText(QString::number(_picWidth));
        return;
    }
    _picWidth=ui->sizeWidth->text().toInt();

    if(_vecRawInfo.size()<1)
    {
        return;
    }
    isShowThumb();
    int nIndex=ui->thumbList->currentRow();
    if(nIndex<0)
    {
        QGraphicsScene* tempScene=ui->ImageGraphic->scene();
        if(tempScene!=nullptr)
        {
            ui->ImageGraphic->setScene(nullptr);
            delete tempScene;
            tempScene=nullptr;
        }
        _vecRawInfo.resize(0);
        ui->photoNameLabel->setText("");
        return;
    }
    setGraphicName(nIndex);
}

void MainWindow::heightEditChange()
{
    QString str=ui->sizeHeight->text();
    if(str.toInt()==0||str=="")
    {
        QMessageBox::information(this, "Warning", "Height can't be empty or 0!", QMessageBox::Ok);
        ui->sizeHeight->setText(QString::number(_picHeight));
        return;
    }
    _picHeight=ui->sizeHeight->text().toInt();

    if(_vecRawInfo.size()<1)
    {
        QGraphicsScene* tempScene=ui->ImageGraphic->scene();
        if(tempScene!=nullptr)
        {
            ui->ImageGraphic->setScene(nullptr);
            delete tempScene;
            tempScene=nullptr;
        }
        _vecRawInfo.resize(0);
        ui->photoNameLabel->setText("");
        return;
    }
    isShowThumb();
    int nIndex=ui->thumbList->currentRow();
    if(nIndex<0)
    {
        return;
    }
    setGraphicName(nIndex);
}

void MainWindow::bitRadioChange()
{
    QRadioButton *selectedButton = qobject_cast<QRadioButton*>(_rawBitGroup->checkedButton());
    if(selectedButton)
    {
        QString selectedValue = selectedButton->text();
        _nRawBit=selectedValue.toInt();
    }
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    isShowThumb();
    int nIndex=ui->thumbList->currentRow();
    setGraphicName(nIndex);
}

void MainWindow::rawFormatChange()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    _nRawFormat=ui->outFormatCombo->currentIndex();
    isShowThumb();
    int nIndex=ui->thumbList->currentRow();
    setGraphicName(nIndex);
}

void MainWindow::calibrationOpen()
{
    if(!_isCalculated)
    {
        QMessageBox::information(this, "Warning", "Please Calculate ROI!", QMessageBox::Ok);
        return;
    }
    _calibrationVecResult.resize(0);

    Calibration dialogCalibration(this);
    int groupId=ui->modelCombo->currentIndex();
    int nNum=0;
    //获取散点
    GetGreenLogPointNum(groupId, &nNum);
    if(nNum<1)
    {
        QMessageBox::information(this, "Warning", "Please Set ROI!", QMessageBox::Ok);
        return;
    }
    std::vector<double> vecX(nNum);
    std::vector<double> vecY(nNum);
    GetGreenLogPoint(groupId, &vecX[0], &vecY[0],nNum);
    std::vector<PointInfo> vecTemp;
    for (int var = 0; var <nNum; ++var) {
        PointInfo info;
        info.x=vecX[var];
        info.y=vecY[var];
        vecTemp.push_back(info);
    }
    //获取中心点
    double xMiddle;
    double yMiddle;
    GetGreenLogMeanPoint(groupId, &xMiddle, &yMiddle);//获取Grass标定后的散点中心点坐标
    PointInfo middlePt;
    middlePt.x=xMiddle;
    middlePt.y=yMiddle;

    dialogCalibration.setSourcePointList(vecTemp,middlePt);
    dialogCalibration.setWindowTitle("Calibration");
    dialogCalibration.exec();
    _calibrationVecResult=dialogCalibration.getDestPointList();
    return;
}

bool MainWindow::checkRepeatRect(std::vector<QRectF> &vecRect, QRectF rect)
{
    for (int var = 0; var < vecRect.size(); ++var) {
        if(vecRect[var]==rect)
        {
            return false;
        }
    }
    vecRect.push_back(rect);
    return true;
}

void MainWindow::rgbir2BGGR(std::vector<std::vector<uint16_t>>& rawDoubleData,size_t width,size_t height)
{
    for (int i = 0; i < height; ) {
        for (int j = 0; j < width; ) {
            rawDoubleData[i+1][j+1]=rawDoubleData[i][j+2];//R
            rawDoubleData[i+1][j+3]=rawDoubleData[i][j+2];//R
            rawDoubleData[i][j+2]=rawDoubleData[i][j];//B
            rawDoubleData[i+3][j+1]=rawDoubleData[i+2][j];//R
            rawDoubleData[i+3][j+3]=rawDoubleData[i+2][j];//R
            rawDoubleData[i+2][j]=rawDoubleData[i+2][j+2];//B
            j+=4;
        }
        i+=4;
    }
}

void MainWindow::rgbir2BGGR(std::vector<uint16_t> &rawDoubleData, size_t width, size_t height)
{
    for (int i = 0; i < height; ) {
        for (int j = 0; j < width; ) {
            rawDoubleData[(i+1)*width+j+1]=rawDoubleData[i*width+j+2];//R
            rawDoubleData[(i+1)*width+j+3]=rawDoubleData[i*width+j+2];//R
            rawDoubleData[i*width+j+2]=rawDoubleData[i*width+j];//B
            rawDoubleData[(i+3)*width+j+1]=rawDoubleData[(i+2)*width+j];//R
            rawDoubleData[(i+3)*width+j+3]=rawDoubleData[(i+2)*width+j];//R
            rawDoubleData[(i+2)*width+j]=rawDoubleData[(i+2)*width+j+2];//B
            j+=4;
        }
        i+=4;
    }
}

void MainWindow::txtOutputClick()
{
    QString srcDirPath=ui->calibrationPathEdit->text()+"/";
    /*BROWSEINFO bi;
    bi.hwndOwner = NULL;
    bi.pidlRoot = CSIDL_DESKTOP;//文件夹的根目录，此处为桌面
    bi.pszDisplayName = NULL;
    bi.lpszTitle = NULL;//显示位于对话框左上部的提示信息
    bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;//有新建文件夹按钮
    bi.lpfn = NULL;
    if(srcDirPath!="")
    {
        bi.lParam   =   (LPARAM)srcDirPath.toStdString().c_str();
    }
    bi.iImage = 0;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);//调用选择对话框
    if (pidl == NULL)
    {
        return;
    }
    TCHAR strFolder[MAX_PATH];
    SHGetPathFromIDList(pidl, strFolder);
    srcDirPath=QString::fromWCharArray(strFolder);*/
    srcDirPath=QFileDialog::getExistingDirectory(this, "选择目录",srcDirPath, QFileDialog::ShowDirsOnly);
    if (srcDirPath.isEmpty())
    {
        return;
    }

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString strTime =current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");

    ui->calibrationPathEdit->setText(srcDirPath);
    outputTxt(srcDirPath,strTime);
    if(ui->modelCombo->currentIndex()!=2)
    {
        outputExcel(srcDirPath,strTime);
    }
    outputImage(srcDirPath);
}

void MainWindow::outputTxt(QString srcDirPath, QString strTime)
{
    if(!_isCalculated)
    {
        QMessageBox::information(this, "Warning", "Please Calculate ROI!", QMessageBox::Ok);
        return;
    }

    QString originPath=srcDirPath+"/awbCalibration.txt";
    srcDirPath+="/awbCalibration"+strTime+".txt";
    std::ofstream outFile;
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::trunc);
    if(!outFile.is_open())
    {
        QMessageBox::information(this, "Warning", "File Path is wrong!", QMessageBox::Ok);
        return;
    }
    QString strContent="";
    int groupId=ui->modelCombo->currentIndex();
    switch (groupId) {
    case 0:
        strContent+="r_nor_tab_bor[0]=";
        strContent+=QString::number(_AWBData.nor_tab_bor[0]);
        strContent+="\nr_nor_tab_bor[1]=";
        strContent+=QString::number(_AWBData.nor_tab_bor[1]);
        strContent+="\nr_div_norm_step[0]=";
        strContent+=QString::number(_AWBData.nor_div_step[0]);
        strContent+="\nr_div_norm_step[1]=";
        strContent+=QString::number(_AWBData.nor_div_step[1]);
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 32; ++j) {
                strContent+="\nr_table_normal["+QString::number(i)+"]["+QString::number(j)+"]=";
                strContent+=QString::number(_AWBData.nor_table[i*32+j]);
            }
        }
        outFile.write(strContent.toStdString().c_str(),strContent.length());
        normalGet2Txt(originPath,strTime);
        break;
    case 1:
        strContent+="r_cwf_tab_bor[0]=";
        strContent+=QString::number(_AWBData.cwf_tab_bor[0]);
        strContent+="\nr_cwf_tab_bor[1]=";
        strContent+=QString::number(_AWBData.cwf_tab_bor[1]);
        strContent+="\nr_div_cwf_step[0]=";
        strContent+=QString::number(_AWBData.cwf_div_step[0]);
        strContent+="\nr_div_cwf_step[1]=";
        strContent+=QString::number(_AWBData.cwf_div_step[1]);
        strContent+="\nr_cwf_line_x[0]=";
        strContent+=QString::number(_AWBData.cwf_line_x[0]);
        strContent+="\nr_cwf_line_x[1]=";
        strContent+=QString::number(_AWBData.cwf_line_x[1]);
        strContent+="\nr_cwf_line_y[0]=";
        strContent+=QString::number(_AWBData.cwf_line_y[0]);
        strContent+="\nr_cwf_line_y[1]=";
        strContent+=QString::number(_AWBData.cwf_line_y[1]);
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 8; ++j) {
                strContent+="\nr_table_cwf["+QString::number(i)+"]["+QString::number(j)+"]=";
                strContent+=QString::number(_AWBData.cwf_table[i*8+j]);
            }
        }
        outFile.write(strContent.toStdString().c_str(),strContent.length());
        cwfGet2Txt(originPath,strTime);
        break;
    case 2:
        strContent+="pGreenFixedBG[0]=";
        strContent+=QString::number(_AWBData.green_fixed_BG[0]);
        strContent+="\npGreenFixedRG[0]=";
        strContent+=QString::number(_AWBData.green_fixed_RG[0]);
        strContent+="\npGreenFixedBG[1]=";
        strContent+=QString::number(_AWBData.green_fixed_BG[1]);
        strContent+="\npGreenFixedRG[1]=";
        strContent+=QString::number(_AWBData.green_fixed_RG[1]);
        strContent+="\npGreenFixedBG[2]=";
        strContent+=QString::number(_AWBData.green_fixed_BG[2]);
        strContent+="\npGreenFixedRG[2]=";
        strContent+=QString::number(_AWBData.green_fixed_RG[2]);
        strContent+="\npGreenFixedBG[3]=";
        strContent+=QString::number(_AWBData.green_fixed_BG[3]);
        strContent+="\npGreenFixedRG[3]=";
        strContent+=QString::number(_AWBData.green_fixed_RG[3]);
        outFile.write(strContent.toStdString().c_str(),strContent.length());
        greenGet2Txt(originPath,strTime);
        break;
    default:
        break;
    }
    outFile.close();
}

void MainWindow::outputExcel(QString srcDirPath, QString strTime)
{
    if(!_isCalculated)
    {
        QMessageBox::information(this, "Warning", "Please Calculate ROI!", QMessageBox::Ok);
        return;
    }

    int groupId=ui->modelCombo->currentIndex();
    int num;
    GetLogPointNum(groupId, &num);
    std::vector<double> vecX(num);
    std::vector<double> vecY(num);
    GetLogPoint(groupId,  &vecX[0],  &vecY[0], num);

    srcDirPath+="/awbCalibration"+strTime+".csv";
    std::ofstream outFile;
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::binary|std::ios::trunc);
    QString strTemp="ImageName,XVal,YVal,x,y,width,height\n";
    QString str;
    int nCount=0;
    for (int i = 0; i < _vecRawInfo.size(); ++i) {
        for (int j = 0; j < _vecRawInfo[i].roiRect.size(); ++j) {
            str=_vecRawInfo[i].rawFilePath;
            str=str.right(str.size() - (str.lastIndexOf("/")+1));
            strTemp+=str+"-ROI_"+QString ::number(j+1)+",";          //文本内序号从1开始
            strTemp+=QString::number(vecX[nCount],'f',6);
            strTemp+=",";
            strTemp+=QString::number(vecY[nCount],'f',6);
            strTemp+=",";
            strTemp+=QString::number(_vecRawInfo[i].roiRect[j].x()+_picWidth/2);
            strTemp+=",";
            strTemp+=QString::number(_vecRawInfo[i].roiRect[j].y()+_picHeight/2);
            strTemp+=",";
            strTemp+=QString::number(_vecRawInfo[i].roiRect[j].width());
            strTemp+=",";
            strTemp+=QString::number(_vecRawInfo[i].roiRect[j].height());
            strTemp+="\n";
            outFile.write(strTemp.toStdString().c_str(),strTemp.size());
            strTemp="";
            nCount++;
        }
    }
    outFile.close();
}

void MainWindow::getROIResult()
{
    if(_isHighLuma)
    {
        if(_highLumaIndex==-1)
        {
            QMessageBox::information(this, "Warning", "Please check High Luma!", QMessageBox::Ok);
            return;
        }
    }

    if(_vecRawInfo.size()<1)
    {
        QMessageBox::information(this, "Warning", "Please load Raw!", QMessageBox::Ok);
        return;
    }

    int groupId=ui->modelCombo->currentIndex();
    RemoveGroup(groupId);
    vecImg.clear();
    vecImgName.clear();


    if(ui->marginEdit->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "Please Enter Margin!", QMessageBox::Ok);
        return;
    }
    if(ui->blcEdit->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "Please Enter BLC!", QMessageBox::Ok);
        return;
    }
    if(ui->sizeWidth->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "Please Enter Width!", QMessageBox::Ok);
        return;
    }
    if(ui->sizeHeight->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "Please Enter Height!", QMessageBox::Ok);
        return;
    }

    GroupInfo info;
    info.Margin = ui->marginEdit->text().toDouble();
    switch (ui->modelCombo->currentIndex()) {
    case 0:
        info.type = IMAGE_NORMAL;
        break;
    case 1:
        info.type = IMAGE_CWF;
        break;
    case 2:
        info.type = IMAGE_GRASS;
        break;
    default:
        break;
    }
    currentType=info.type ;

    CreateGroup(groupId, info);
    SetGreenCalibMode(groupId,_isHighLuma);
    ImageInfo iminfo;
    iminfo.Width = _picWidth;
    iminfo.Height = _picHeight;
    iminfo.Bits = _nRawBit;
    iminfo.BLC = ui->blcEdit->text().toInt();//BLC默认64
    //iminfo.BLC=64;
    bool isCalculated=false;   //整组是否有rect被计算过
    bool isHaveRcet=false;    //当前图是否有rect
    bool isGreen=ui->modelCombo->currentIndex()==2;
    int k=10;//测试偏移
    vecImg.resize(_vecRawInfo.size());

    for (int var = 0; var < _vecRawInfo.size(); ++var) {
        std::vector<uint16_t> rawDoubleData;
        openRawData(_vecRawInfo[var].rawFilePath.toStdString().c_str(),rawDoubleData,_picWidth,_picHeight);
        vecImgName.push_back(std::make_pair(QFileInfo(_vecRawInfo[var].rawFilePath).baseName(),&vecImg[var]));

        SetImageInfo(groupId, &vecImg[var], &rawDoubleData[0], (int)rawDoubleData.size(), iminfo);
        isHaveRcet=false;
        if(isGreen)
        {
            if(_vecRawInfo[var].roiRect.size()>0)
            {
                double x=_vecRawInfo[var].roiRect[0].left()+_picWidth/2;
                x=x<0?0:x;
                x=x>=_picWidth?_picWidth-1:x;
                double y=_vecRawInfo[var].roiRect[0].top()+_picHeight/2;
                y=y<0?0:y;
                y=y>=_picHeight?_picHeight-1:y;
                double x1=_vecRawInfo[var].roiRect[0].right()+_picWidth/2;
                x1=x1<0?0:x1;
                x1=x1>=_picWidth?_picWidth-1:x1;
                double y1=_vecRawInfo[var].roiRect[0].bottom()+_picHeight/2;
                y1=y1<0?0:y1;
                y1=y1>=_picHeight?_picHeight-1:y1;
                AddROI(vecImg[var],x,y,x1,y1, Green);
                // AddROI(vecImg[var],220+k,290-k,740+k,512-k, Green);
                isCalculated=true;
                isHaveRcet=true;
            }
            if(_vecRawInfo[var].roiRect.size()>1)
            {
                double x=_vecRawInfo[var].roiRect[1].left()+_picWidth/2;
                x=x<0?0:x;
                x=x>=_picWidth?_picWidth-1:x;
                double y=_vecRawInfo[var].roiRect[1].top()+_picHeight/2;
                y=y<0?0:y;
                y=y>=_picHeight?_picHeight-1:y;
                double x1=_vecRawInfo[var].roiRect[1].right()+_picWidth/2;
                x1=x1<0?0:x1;
                x1=x1>=_picWidth?_picWidth-1:x1;
                double y1=_vecRawInfo[var].roiRect[1].bottom()+_picHeight/2;
                y1=y1<0?0:y1;
                y1=y1>=_picHeight?_picHeight-1:y1;
                AddROI(vecImg[var],x,y,x1,y1, Gray);
                // AddROI(vecImg[var],882+k,421+k,968+k,545+k, Gray);
            }
        }
        else
        {
            for (int roi = 0; roi < _vecRawInfo[var].roiRect.size(); ++roi) {
                double x=_vecRawInfo[var].roiRect[roi].left()+_picWidth/2;
                x=x<0?0:x;
                x=x>=_picWidth?_picWidth-1:x;
                double y=_vecRawInfo[var].roiRect[roi].top()+_picHeight/2;
                y=y<0?0:y;
                y=y>=_picHeight?_picHeight-1:y;
                double x1=_vecRawInfo[var].roiRect[roi].right()+_picWidth/2;
                x1=x1<0?0:x1;
                x1=x1>=_picWidth?_picWidth-1:x1;
                double y1=_vecRawInfo[var].roiRect[roi].bottom()+_picHeight/2;
                y1=y1<0?0:y1;
                y1=y1>=_picHeight?_picHeight-1:y1;
                AddROI(vecImg[var],x,y,x1,y1, Gray);

                isCalculated=true;
                isHaveRcet=true;
            }
            // AddROI(vecImg[var],431+k,530+k,453+k,557+k, Gray);
            // AddROI(vecImg[var],492+k,533+k,515+k,557+k, Gray);
            // AddROI(vecImg[var],543+k,539+k,566+k,559+k, Gray);
            // AddROI(vecImg[var],606+k,536+k,626+k,557+k, Gray);
            // isCalculated=true;
            // isHaveRcet=true;
        }

        if(isHaveRcet)
        {
            CalculateWBGain(vecImg[var]);
        }
    }
    if(!isCalculated)
    {
        QMessageBox::information(this, "Warning", "Please set ROI!", QMessageBox::Ok);
        return;
    }

    if(_isHighLuma)
    {
        CalGreenHighLumaROI(groupId,vecImg[_highLumaIndex]);
    }

    if(!isGreen)
    {
        CalculateWeightMap(groupId);
    }

    GetCalibRes(groupId,&_AWBData);

    _isCalculated=true;
    if(ui->modelCombo->currentIndex()!=2)
    {
        QMessageBox::information(this, "Success", "Finish Calculate!", QMessageBox::Ok);
    }
    else
    {
        calibrationOpen();
    }
}

void MainWindow::setHighLuma()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    if(_highLumaIndex!=-1)
    {
        if(_highLumaIndex==ui->thumbList->currentRow())
        {
            _isHighLuma=false;
            ui->thumbList->item(_highLumaIndex)->setForeground(Qt::black);
            _highLumaIndex=-1;
            ui->setLumaBtn->setText("SetHighLuma");
        }
        else
        {
            ui->thumbList->item(_highLumaIndex)->setForeground(Qt::black);
            _highLumaIndex=ui->thumbList->currentRow();
            ui->thumbList->item(_highLumaIndex)->setForeground(Qt::red);
            ui->setLumaBtn->setText("CancelHighLuma");
        }
    }
    else
    {
        _isHighLuma=true;
        _highLumaIndex=ui->thumbList->currentRow();
        ui->thumbList->item(_highLumaIndex)->setForeground(Qt::red);
        ui->setLumaBtn->setText("CancelHighLuma");
    }
}

void MainWindow::write2Reg()
{
    if(!_isCalculated)
    {
        QMessageBox::information(this, "Warning", "Please Calculate ROI!", QMessageBox::Ok);
        return;
    }

    switch (ui->modelCombo->currentIndex()) {
    case 0:
        normal2Reg();
        break;
    case 1:
        cwf2Reg();
        break;
    case 2:
        green2Reg();
        break;
    default:
        break;
    }
}

void MainWindow::applyAllBtn()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    int nIndex=ui->thumbList->currentRow();
    for (int var = 0; var < _vecRawInfo.size(); ++var) {
        if(var==nIndex){
            continue;
        }
        for (int i = 0; i < _vecRawInfo[nIndex].roiRect.size(); ++i) {
            checkRepeatRect(_vecRawInfo[var].roiRect,_vecRawInfo[nIndex].roiRect[i]);
        }
    }
    setGraphicName(ui->thumbList->currentRow());
}

void MainWindow::roiImport()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"),"",QStringLiteral("文本文件 (*.txt)"));
    if(fileName.isEmpty())
    {
        return;
    }
    int nIndex=0;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream in(&file);
    std::string strIn;
    while (!in.atEnd()) {
        strIn = in.readLine().toStdString();
        size_t pos = 0;
        pos = strIn.find(",");
        if(pos<0||pos>=strIn.size())
        {
            file.close();
            QMessageBox::information(this, "Warning", "config file format error!", QMessageBox::Ok);
            return;
        }
        nIndex=std::stoi(strIn.substr(0, pos));
        if(nIndex>=_vecRawInfo.size())
        {
            break;
        }
        strIn.erase(0, pos + 1);
        pos = strIn.find(",");
        if(pos<0||pos>=strIn.size())
        {
            file.close();
            QMessageBox::information(this, "Warning", "config file format error!", QMessageBox::Ok);
            return;
        }
        strIn.erase(0, pos + 1);
        pos = strIn.find(",");
        if(pos<0||pos>=strIn.size())
        {
            QMessageBox::information(this, "Warning", "config file format error!", QMessageBox::Ok);
            return;
        }
        double left=std::stod(strIn.substr(0, pos));
        strIn.erase(0, pos + 1);
        pos = strIn.find(",");
        if(pos<0||pos>=strIn.size())
        {
            file.close();
            QMessageBox::information(this, "Warning", "config file format error!", QMessageBox::Ok);
            return;
        }
        double top=std::stod(strIn.substr(0, pos));
        strIn.erase(0, pos + 1);
        pos = strIn.find(",");
        if(pos<0||pos>=strIn.size())
        {
            file.close();
            QMessageBox::information(this, "Warning", "config file format error!", QMessageBox::Ok);
            return;
        }
        double width=std::stod(strIn.substr(0, pos));
        strIn.erase(0, pos + 1);
        pos = strIn.find(",");
        double height=std::stod(strIn.substr(0, pos));
        QRectF rect(left,top,width,height);
        checkRepeatRect(_vecRawInfo[nIndex].roiRect, rect);
    }
    file.close();

    QMessageBox::information(this, "Success", "Import Finished!", QMessageBox::Ok);
    setGraphicName(ui->thumbList->currentRow());
    return;
}

void MainWindow::roiExport()
{
    if(_vecRawInfo.size()<1)
    {
        return;
    }
    QString srcDirPath=QFileDialog::getExistingDirectory(this, "选择目录","/", QFileDialog::ShowDirsOnly);
    if (srcDirPath.isEmpty())
    {
        return;
    }
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString strTime =current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
    srcDirPath+="/awbROIBackup"+strTime+".txt";
    std::ofstream outFile;
    outFile.open(srcDirPath.toStdString().c_str(),std::ios::out|std::ios::trunc);
    QString strWrite="";
    for (int i = 0; i < _vecRawInfo.size(); ++i) {
        for (int j = 0; j < _vecRawInfo[i].roiRect.size(); ++j) {
            strWrite=QString::number(i)+","+QString::number(j)+",";
            strWrite+=QString::number(_vecRawInfo[i].roiRect[j].left())+",";
            strWrite+=QString::number(_vecRawInfo[i].roiRect[j].top())+",";
            strWrite+=QString::number(_vecRawInfo[i].roiRect[j].width())+",";
            strWrite+=QString::number(_vecRawInfo[i].roiRect[j].height())+"\n";
            outFile.write(strWrite.toStdString().c_str(),strWrite.size());
        }
    }
    outFile.close();

    QMessageBox::information(this, "Success", "Export Finished!", QMessageBox::Ok);
    return;
}

void MainWindow::modelChanged()
{
    if(ui->modelCombo->currentIndex()==2)
    {
        ui->setLumaBtn->setVisible(true);
    }
    else
    {
        ui->setLumaBtn->setVisible(false);
        _isHighLuma=false;
        if(_highLumaIndex!=-1)
        {
            ui->thumbList->item(_highLumaIndex)->setForeground(Qt::black);
        }
        _highLumaIndex=-1;
    }
    _isCalculated=false;

    setGraphicName(ui->thumbList->currentRow());
}

void MainWindow::inputPathFolder()
{
    if(_picWidth==0 || _picHeight==0)
    {
        QMessageBox::information(this, "Warning", "Width or Height can't be empty or 0!", QMessageBox::Ok);
        return;
    }

    QString srcDirPath="/";
    if(ui->inputFolderEdit->text().size()>1)
    {
        srcDirPath=ui->inputFolderEdit->text();
    }
    BROWSEINFO bi;
    bi.hwndOwner = NULL;
    bi.pidlRoot = CSIDL_DESKTOP;//文件夹的根目录，此处为桌面
    bi.pszDisplayName = NULL;
    bi.lpszTitle = NULL;//显示位于对话框左上部的提示信息
    bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;//有新建文件夹按钮
    bi.lpfn = NULL;
    bi.iImage = 0;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);//调用选择对话框
    if (pidl == NULL)
    {
        return;
    }
    TCHAR strFolder[MAX_PATH];
    SHGetPathFromIDList(pidl, strFolder);
    srcDirPath=QString::fromWCharArray(strFolder);
    if (srcDirPath.isEmpty())
    {
        return;
    }
    ui->inputFolderEdit->setText(srcDirPath);
    ui->calibrationPathEdit->setText(srcDirPath);

    openSourcePicPath(srcDirPath);
    _highLumaIndex=-1;
    _isCalculated=false;
}

void MainWindow::inputPathFolderFromEdit()
{
    if(ui->inputFolderEdit->text().size()<1)
    {
        return;
    }
    openSourcePicPath(ui->inputFolderEdit->text());
    _highLumaIndex=-1;
    _isCalculated=false;
}

void MainWindow::isShowThumb()
{
    if(_picWidth==0 || _picHeight==0)
    {
        QMessageBox::information(this, "Warning", "Width or Height can't be empty or 0!", QMessageBox::Ok);
        return;
    }

    bool isShowPic=ui->thumbCheck->isChecked();
    int nIndex=ui->thumbList->currentRow();
    ui->thumbList->clear();
    for (int var = 0; var < _vecRawInfo.size(); ++var) {
        QListWidgetItem *pItem = new QListWidgetItem;
        if(isShowPic)
        {
            QPixmap pix;
            int nRet=rawToPixmap(pix,_vecRawInfo[var].rawFilePath.toLatin1().data(),_picWidth,_picHeight);
            if(nRet!=ERROR_NO_ERR)
            {
                return;
            }
            QIcon icon(pix);
            pItem->setIcon(icon);
        }
        QString str=_vecRawInfo[var].rawFilePath;
        str=str.right(str.size() - (str.lastIndexOf("/")+1));
        pItem->setText(autoWrap(str));
        ui->thumbList->addItem(pItem);
    }
    ui->thumbList->setCurrentRow(nIndex);

    if(_highLumaIndex!=-1)
    {
        ui->thumbList->item(_highLumaIndex)->setForeground(Qt::red);
    }
}


void MainWindow::outputImage(QString srcDirPath)
{
    QString directory = srcDirPath +"/"+ ui->modelCombo->currentText()+"_res_BMP";
    QDir().mkpath(directory);
    for(int i=0;i<vecImgName.size();++i){
        if( ui->modelCombo->currentIndex()==2){
            std::vector<uint16_t> rawDoubleData;
            openRawData(_vecRawInfo[i].rawFilePath.toStdString().c_str(),rawDoubleData,_picWidth,_picHeight);
            std::vector<uint8_t> rgb_data;
            rawToRGB888(rawDoubleData,rgb_data,_picWidth,_picHeight);
            QImage img((uint8_t*)&rgb_data[0],(int)_picWidth,(int)_picHeight,QImage::Format_BGR888);//转换图像
            img.save(directory+"/"+vecImgName[i].first+".bmp","BMP");
        }else{
            uint32_t size=(_picWidth*_picHeight);
            std::vector<uint16_t> double_data(size);
            GetImageAfterAWB(*(vecImgName[i].second),&double_data[0],size);
            std::vector<uint8_t> rgb_data;
            rawToRGB888(double_data,rgb_data,_picWidth,_picHeight);
            QImage img((uint8_t*)&rgb_data[0],(int)_picWidth,(int)_picHeight,QImage::Format_BGR888);//转换图像
            img.save(directory+"/"+vecImgName[i].first+".bmp","BMP");
        }
    }
}

void MainWindow::txtPathChanged()
{
    if(ui->calibrationPathEdit->text().size()<1)
    {
        QMessageBox::information(this, "Warning", "File Path is wrong!", QMessageBox::Ok);
        return;
    }
}

