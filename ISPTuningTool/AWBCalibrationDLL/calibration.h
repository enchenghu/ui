#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QDialog>
#include "QScatterSeries "
#include "chartviewcalibration.h"
#include "QMessageBox"
#include "QFileDialog"

namespace Ui {
class Calibration;
}

class Calibration : public QDialog
{
    Q_OBJECT

public:
    explicit Calibration(QWidget *parent = nullptr);
    ~Calibration();

    //传入点数组
    void setSourcePointList(std::vector<PointInfo> sourcePoint,PointInfo middlePt);
    //返回选中的点数组
    std::vector<PointInfo> getDestPointList();

private slots:
    void confirmBtn();
    void cancelBtn();
    void savePhoto();
private:
    Ui::Calibration *ui;
    ChartViewCalibration* _chart;

};

#endif // CALIBRATION_H
