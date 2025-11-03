#ifndef QCONFIGFILETRANS_H
#define QCONFIGFILETRANS_H

#include <QString>
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>

QString iniPathName();
QString getCurrentTime();
uint32_t getQStringToUint(QString value);
uint32_t getBitValue(uint32_t bitInfo, uint32_t value);
int readIniData(QString section,QString sectionKey,QString fileName,int defaultValue);
void writeIniData(QString section, QString sectionKey, QString fileName, int wrtieValue);
float readIniFloat(QString section,QString sectionKey,QString fileName,float defaultValue);
void writeIniFloat(QString section, QString sectionKey,QString fileName, float wrtieValue);
QString readIniString(QString section,QString sectionKey,QString fileName,QString defaultValue);
void writeIniString(QString section, QString sectionKey,QString fileName, QString wrtieValue);

#endif // QCONFIGFILETRANS_H
