#ifndef QCONFIGDIALOG_H
#define QCONFIGDIALOG_H

#include "Common/Others/qconfigfiletrans.h"
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include<QSpinBox>

namespace Ui {
class QConfigDialog;
}

//存储配置信息结构体
struct ConfigInfo{
    QString backGroundColor;
    QString fontColor;
    QString borderColor;
    int radius;
    ConfigInfo()
    {
        backGroundColor="";
        fontColor="";
        borderColor="";
        radius=-1;
    }
};

class QConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QConfigDialog(QWidget *parent = nullptr);
    ~QConfigDialog();
    void updateEdit();
    QString getConfigStyle(QString filePath=iniPathName() + "Config/M1_style.ini");
    void saveConfigStyle(QString filePath=iniPathName() + "Config/M1_style.ini");
    QString initStyle();

signals:
    void updateStyle(QString);
private slots:
    void showColorDialog();
    void showFontColorDialog();
    void showStyleConfig();
    void changeStyleForSlider();//专为滑条控件的更新样式并保存函数
    void showBorderColorDialog();
    void changePic();
private:
    Ui::QConfigDialog *ui;
    QPointer<QComboBox> box=nullptr;
    QPointer<QLineEdit> pathEdit=nullptr;
    QPointer<QToolButton> loadBtn=nullptr;
    QPointer<QToolButton> colorBtn=nullptr;
    QPointer<QLineEdit> colorEdit=nullptr;
    QPointer<QToolButton> fontColorBtn=nullptr;
    QPointer<QLineEdit> fontColorEdit=nullptr;
    //QPointer<QLineEdit> borderRadiusEdit=nullptr;
    //QPointer<QToolButton> borderRadiusBtn=nullptr;
    QPointer<QSlider> borderRadiusSlider=nullptr;
    QPointer<QSpinBox> borderRadiusSpinBox=nullptr;
    QPointer<QToolButton> borderColorBtn=nullptr;
    QPointer<QLineEdit> borderColorEdit=nullptr;
    QPointer<QLineEdit> imagePathEdit=nullptr;
    QPointer<QToolButton> imagePathBtn=nullptr;

    QMap<QString,ConfigInfo> uiInfoMap;

};

#endif // QCONFIGDIALOG_H
