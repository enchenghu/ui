#include "qregister.h"
#include "ui_qregister.h"

qRegister::qRegister(uint *pLevel, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::qRegister)
    , m_pLevel(pLevel)
{
    ui->setupUi(this);
    this->setWindowTitle("Register");

    initSerialNumber();

    connect(ui->RegisterBtn, &QPushButton::clicked, this, &qRegister::registerBtn);
    connect(ui->KeyLineEdit, &QLineEdit::textChanged, this,
        [&]() { m_strKey = ui->KeyLineEdit->text().toStdString().c_str(); });
}

qRegister::~qRegister()
{
    delete ui;
}

bool qRegister::bIsAuthorized()
{
    bool iret;// = LicenseLib_IsAuthorized(); // 本地是否已经注册，License文件存在，且注册过
    char szKey[MAX_KEY_SIZE];
    char szLevelEnable[MAX_KEY_SIZE];
    iret = LicenseLib_IsAuthorizedRegisterTable(gSensorID,szKey,szLevelEnable);
    if (iret == false)                    // 本地文件不存在/未注册
    {
        // 判断注册表中是否已经注册
        /*if (gSensorID == 0) {
            return false;
        }*/
        QString strPath=QCoreApplication::applicationDirPath();
        strPath+="/LicenseKey.ini";
        QFile f(strPath);
        std::string lineStr="";
        lineStr.reserve(MAX_KEY_SIZE);
        if(f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream txtInput(&f);
            lineStr = txtInput.readLine().toStdString();
            f.close();
        }

        return true;
    } else // 本地已经注册
    {
        return iret;
    }
}

void qRegister::initSerialNumber()
{
    std::string strSerialNumber = "";
    getSerialNumber(strSerialNumber);

    ui->SerialNumberLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->SerialNumberLabel->setText(strSerialNumber.c_str());
}

void qRegister::getSerialNumber(std::string &strSerialNumber)
{
    char pSerialNumber[MAX_KEY_SIZE];
    LicenseLib_GetHardwareInfo(pSerialNumber,MAX_KEY_SIZE);
    strSerialNumber=pSerialNumber;
    strSerialNumber += std::to_string(gSensorID);
}

int qRegister::iGetUserLevelfromString(std::string strUserLevel)
{
    int iUserLev = 0;
    if (strUserLevel == "E5H8") {
        iUserLev = LEVEL_ADMINISTRATOR;
    } else if (strUserLevel == "M23K") {
        iUserLev = LEVEL_HIGHUSER;
    } else if (strUserLevel == "PSVI") {
        iUserLev = LEVEL_MIDDLEUSER;
    } else if (strUserLevel == "SS7T") {
        iUserLev = LEVEL_USER;
    } else {
        iUserLev = 0;
    }

    return iUserLev;
}

int qRegister::iGetUserLevel(std::string &strLevelEnable)
{
    std::string strTem;
    char cKey[MAX_KEY_SIZE];
    char cEnable[MAX_KEY_SIZE];
    LicenseLib_GetKeyEx(gSensorID, cKey, cEnable);
    strLevelEnable=cEnable;
    strTem=cKey;
    strTem=strTem.substr(0,4);
    int iUserLev = 0;
    if (strTem == "E5H8") {
        iUserLev = 1000;
    } else if (strTem == "M23K") {
        iUserLev = 2000;
    } else if (strTem == "PSVI") {
        iUserLev = 3000;
    } else if (strTem == "SS7T") {
        iUserLev = 4000;
    } else {
        iUserLev = 0;
    }

    return iUserLev;
}

void qRegister::vSetSensorID(UINT id)
{
    gSensorID=id;
}

void qRegister::registerBtn()
{
    if (ui->KeyLineEdit->text().size() < 1) {
        QMessageBox::information(this, "Warning", "Please enter the Key!", QMessageBox::Ok);
        return;
    }

    std::string strSerialTem = "";
    getSerialNumber(strSerialTem);
    std::string str          = ui->KeyLineEdit->text().toStdString();
    std::string     strUserLevel = str.substr(0, 4).c_str();
    int         iUserLev     = 0;
    iUserLev                 = iGetUserLevelfromString(strUserLevel);
    std::string strKey           = "";
    char cKey[MAX_KEY_SIZE];
    LicenseLib_GenerateLicense(iUserLev, strSerialTem.c_str(), cKey);
    strKey=cKey;
    if (strKey != str.substr(0, 20).c_str()) {
        QMessageBox::information(this, "Warning", "Register failed, key error!", QMessageBox::Ok);
        return;
    }
    // 注册
    int ret = LicenseLib_RegisterEx(gSensorID, cKey);
    if (ret) {
        //保存用户信息
        QString strPath=QCoreApplication::applicationDirPath();
        strPath+="/LicenseKey.ini";
        QFile f(strPath);
        if(f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream txtOutput(&f);
            txtOutput << QString::fromStdString(str);
            f.close();
        }
        // 获取用户等级信息
        std::string strUserLevel = str.substr(0, 4).c_str();
        *m_pLevel            = iGetUserLevelfromString(strUserLevel);
        emit updateLevel();
        QMessageBox::information(this, "Success", "Register Success!", QMessageBox::Ok);
        this->close();
        return;
    } else {
        QMessageBox::information(this, "Warning", "Register failed, please check key number!", QMessageBox::Ok);
        return;
    }
}
