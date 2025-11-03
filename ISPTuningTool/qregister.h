#ifndef QREGISTER_H
#define QREGISTER_H

#include <QDialog>
#include "QMessageBox"
#include <QFile>
#include "qcommondef.h"
#include "third_party/include/LicenseLib.h"

namespace Ui {
class qRegister;
}

class qRegister : public QDialog
{
    Q_OBJECT

public:
    explicit qRegister(uint* pLevel,QWidget *parent = nullptr);
    ~qRegister();

    //是否注册过
    static bool bIsAuthorized();
    //根据字符串等级编码，获取等级整型编码数据
    static int iGetUserLevelfromString(std::string strUserLevel);
    static int iGetUserLevel(std::string &strLevelEnable);
    static void vSetSensorID(UINT id);

signals:
    void updateLevel();

private:
    //初始化SerialNumber
    void initSerialNumber();

    void getSerialNumber(std::string& strSerialNumber);

private slots:
    void registerBtn();

private:
    Ui::qRegister *ui;

    static int gSensorID;
    static std::string m_strKey;
    uint* m_pLevel;
};

#endif // QREGISTER_H
