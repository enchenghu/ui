#include "qconfigfiletrans.h"

#include <QFile>

//获取程序根目录
QString iniPathName()
{
    QString  pathName;
    pathName = QCoreApplication::applicationDirPath() + "/";
    return pathName;
}

//获取当前时间
QString getCurrentTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString time =currentTime.toString("yyyyMMddhhmmsszzz");
    return time;
}

//将字符串转为uint32_t，自动识别十进制与十六进制
uint32_t getQStringToUint(QString value)
{
    uint32_t ret = 0;
    if((value.left(2) == "0x") || (value.left(2) == "0X")) {
        ret = value.toUInt(nullptr,16); //16进制
    } else {
        ret = value.toUInt(nullptr,10);  //10进制
    }
    return ret;
}

//根据bit信息自动获取实际生效值
uint32_t getBitValue(uint32_t bitInfo, uint32_t value)
{
    int index = 1;
    int offset = 0;
    while((bitInfo & index) == 0)
    {
        offset++;
        index = (index << 1);
    }

    return ((value & bitInfo) >> offset);
}

//获取配置文件中int格式的参数
int readIniData(QString section,QString sectionKey,QString fileName,int defaultValue)
{
    int ret = defaultValue;

    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    if(settings->contains(key)) {
        QString value = settings->value(key).toString();
        if((value.left(2) == "0x") || (value.left(2) == "0X")) {
            ret = value.toUInt(nullptr,16); //16进制（无符号）
        } else {
            ret = value.toInt(nullptr,10);  //10进制（有符号）
        }
    }
    delete settings;

    return ret;
}

//向配置文件中写入int格式的参数
void writeIniData(QString section, QString sectionKey, QString fileName, int wrtieValue)
{
    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    settings->setValue(key, wrtieValue);
    delete settings;
}

//获取配置文件中float格式的参数
float readIniFloat(QString section,QString sectionKey,QString fileName,float defaultValue)
{
    float ret = defaultValue;

    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    if(settings->contains(key))
        ret = settings->value(key).toFloat();
    delete settings;

    return ret;
}

//向配置文件中写入float格式的参数
void writeIniFloat(QString section, QString sectionKey,QString fileName, float wrtieValue)
{
    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    settings->setValue(key, wrtieValue);
    delete settings;
}

//获取配置文件中CString格式的参数
QString readIniString(QString section,QString sectionKey,QString fileName,QString defaultValue)
{
    QString ret = defaultValue;

    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    if(settings->contains(key))
        ret = settings->value(key).toString();
    delete settings;

    return ret;
}

//向配置文件中写入CString格式的参数
void writeIniString(QString section, QString sectionKey,QString fileName, QString wrtieValue)
{
    QString key = section + "/" + sectionKey;
    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    settings->setValue(key, wrtieValue);
    delete settings;
}

