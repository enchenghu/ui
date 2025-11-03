#include "calibration.h"
#include "ui_calibration.h"

Calibration::Calibration(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Calibration)
{
    ui->setupUi(this);
    //最大/小化按钮
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;
    setWindowFlags(windowFlag);

    connect(ui->ConfirmBtn,&QPushButton::clicked,this,&Calibration::confirmBtn);
    connect(ui->CancelBtn,&QPushButton::clicked,this,&Calibration::cancelBtn);
    connect(ui->savePhotoBtn,&QPushButton::clicked,this,&Calibration::savePhoto);
}

Calibration::~Calibration()
{
    delete ui;
}

void Calibration::setSourcePointList(std::vector<PointInfo> sourcePoint,PointInfo middlePt)
{
    _chart=new ChartViewCalibration(sourcePoint,middlePt);
    ui->graphicsView->setChart(_chart);
}

std::vector<PointInfo> Calibration::getDestPointList()
{
    return _chart->getDestPointList();
}

void Calibration::confirmBtn()
{
    if(_chart->getDestPointList().size()==0)
    {
        QMessageBox::information(nullptr, "Warning", "Selected points are less than 60%！", QMessageBox::Ok);
    }
    else
    {
        close();
    }
}

void Calibration::cancelBtn()
{
    close();
}

void Calibration::savePhoto()
{
    QString srcDirPath=QFileDialog::getExistingDirectory(this, "选择目录","/", QFileDialog::ShowDirsOnly);
    if (srcDirPath.isEmpty())
    {
        return;
    }
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString strTime =current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
    srcDirPath+="/awbCalibrationPhoto"+strTime+".bmp";

    QPixmap p = this->grab(ui->graphicsView->rect());
    p.save(srcDirPath,"bmp");
}


