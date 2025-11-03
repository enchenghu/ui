#pragma once
#include "SenPara.h"
#include <string>

using namespace std;

string sIniPathName();
bool WriteIniData(string sSection, string sSectionKey, string sFileName, int nValue);
int ReadIniData(string sSection, string sSectionKey, string sFileName, int nDefault);
string ReadIniString(string sSection, string sSectionKey, string sFileName, string sDefault);
float fReadIniFloat(string sSection, string sKeyWord, string sFileName, float fDefault);

int WriteRgb24ToBmpFile(unsigned char *pData, unsigned int nWidth, unsigned int nHeight, const char *pFilePath);
bool bGetI2CDataFromLibFile(string filename);
void OpenFileName();
BOOL bLoadSensorConfig(string strFilePath);
