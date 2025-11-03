#include "CMethod.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

extern SensorTab gstCurrentSensor;
extern string gstrFileName;
extern float garyfVolBuf[5];
extern vector<STRegNode> gvecRegList;

void OpenFileName()
{
	std::string folderPath;
	std::cout << "Enter the folder path: ";
	std::cin >> folderPath;
	std::vector<std::string> iniFiles;
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.path().extension() == ".ini")
		{
			iniFiles.push_back(entry.path().string());
		}
	}

	if (iniFiles.empty())
	{
		std::cout << "No ini files found in the folder." << std::endl;
		return;
	}

	std::cout << "Select an ini file:" << std::endl;
	for (size_t i = 0; i < iniFiles.size(); ++i)
	{
		std::cout << i + 1 << ". " << iniFiles[i] << std::endl;
	}

	int selection;
	std::cout << "Enter the file number: ";
	std::cin >> selection;
	std::cin.ignore(); // Ignore the newline character

	if (selection < 1 || selection > iniFiles.size())
	{
		std::cout << "Invalid file number." << std::endl;
		return;
	}

	std::string filePath = iniFiles[selection - 1];
	bLoadSensorConfig(filePath);
}

BOOL bLoadSensorConfig(string strFilePath)
{
    // 	memset(&gstCurrentSensor, 0, sizeof(SensorTab));
    string sPath = strFilePath;

    gstCurrentSensor.width = ReadIniData("Sensor", "width", sPath, 0);
    if (gstCurrentSensor.width == 0)
    {
        return FALSE;
    }
    gstCurrentSensor.height = ReadIniData("Sensor", "height", sPath, 0);
    gstCurrentSensor.type = ReadIniData("Sensor", "type", sPath, 2); // display format in demo
    gstCurrentSensor.port = ReadIniData("Sensor", "port", sPath, 0);
    gstCurrentSensor.pin = ReadIniData("Sensor", "pin", sPath, 0);
    gstCurrentSensor.lane = ReadIniData("Sensor", "lane", sPath, 4);
    gstCurrentSensor.avtime = ReadIniData("Sensor", "avtime", sPath, 2);
    gstCurrentSensor.dvtime = ReadIniData("Sensor", "dvtime", sPath, 1);
    gstCurrentSensor.dovtime = ReadIniData("Sensor", "dovtime", sPath, 0);
    gstCurrentSensor.SlaveID = ReadIniData("Sensor", "SlaveID", sPath, 0);
    gstCurrentSensor.mode = ReadIniData("Sensor", "mode", sPath, 0);
    if (0 == gstCurrentSensor.mode)
        gstCurrentSensor.mode = 1;
    gstCurrentSensor.FlagReg = ReadIniData("Sensor", "FlagReg", sPath, 0);
    gstCurrentSensor.FlagMask = ReadIniData("Sensor", "FlagMask", sPath, 0xff);
    gstCurrentSensor.FlagData = ReadIniData("Sensor", "FlagData", sPath, 0);
    gstCurrentSensor.FlagReg1 = ReadIniData("Sensor", "FlagReg1", sPath, 0);
    gstCurrentSensor.FlagMask1 = ReadIniData("Sensor", "FlagMask1", sPath, 0x0);
    gstCurrentSensor.FlagData1 = ReadIniData("Sensor", "FlagData1", sPath, 0);
    gstCurrentSensor.outformat = ReadIniData("Sensor", "outformat", sPath, 0x00);
    gstCurrentSensor.mclk = fReadIniFloat("Sensor", "mclk", sPath, 27.0);

    garyfVolBuf[0] = fReadIniFloat("Sensor", "avdd", sPath, -1.0f);
    if (garyfVolBuf[0] == -1.0f)
    {
        return FALSE;
    }
    garyfVolBuf[1] = fReadIniFloat("Sensor", "dovdd", sPath, -1.0f);
    if (garyfVolBuf[1] == -1.0f)
    {
        return FALSE;
    }
    garyfVolBuf[2] = fReadIniFloat("Sensor", "dvdd", sPath, -1.0f);
    if (garyfVolBuf[2] == -1.0f)
    {
        return FALSE;
    }
    garyfVolBuf[3] = fReadIniFloat("Sensor", "AFVCC", sPath, -1.0f);
    if (garyfVolBuf[3] == -1.0f)
    {
        return FALSE;
    }
    garyfVolBuf[4] = fReadIniFloat("Sensor", "VPP", sPath, -1.0f);
    gstCurrentSensor.dovdddelay = ReadIniData("Sensor", "dovdddelay", sPath, 0);
    gstCurrentSensor.dvdddelay = ReadIniData("Sensor", "dvdddelay", sPath, 0);
    gstCurrentSensor.avdddelay = ReadIniData("Sensor", "avdddelay", sPath, 0);
    gstCurrentSensor.avdd = garyfVolBuf[0];
    gstCurrentSensor.dovdd = garyfVolBuf[1];
    gstCurrentSensor.dvdd = garyfVolBuf[2];
    gstCurrentSensor.afvdd = garyfVolBuf[3];
    if (FALSE == bGetI2CDataFromLibFile(sPath))
    {
        return FALSE;
    }
    return TRUE;
}

// Helper function to trim leading and trailing whitespaces
string trim(const string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos)
    {
        return "";
    }
    return str.substr(first, last - first + 1);
}

// Helper function to parse hexadecimal string to integer
bool parseHex(const string &str, int &value)
{
    try
    {
        size_t pos;
        value = stoi(str, &pos, 16);
        return pos == str.length();
    }
    catch (const exception &)
    {
        return false;
    }
}

// Helper function to parse decimal string to integer
bool parseDec(const string &str, int &value)
{
    try
    {
        size_t pos;
        value = stoi(str, &pos, 10);
        return pos == str.length();
    }
    catch (const exception &)
    {
        return false;
    }
}

bool bGetI2CDataFromLibFile(string filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    vector<STRegNode> vecRegList;

    string line;
    while (getline(file, line))
    {
        // Remove comments
        size_t commentPos = line.find("//");
        if (commentPos != string::npos)
        {
            line = line.substr(0, commentPos);
        }

        // Remove leading and trailing whitespaces
        line = trim(line);

        if (line.empty())
        {
            continue;
        }

        // Split the line into register and value
        size_t commaPos = line.find(",");
        if (commaPos == string::npos)
        {
            continue;
        }

        string regStr = line.substr(0, commaPos);
        string valStr = line.substr(commaPos + 1);

        // Convert register and value to integers
        int reg = 0, val = 0;
        if (!parseHex(regStr, reg) && !parseDec(regStr, reg))
        {
            continue;
        }
        if (!parseHex(valStr, val) && !parseDec(valStr, val))
        {
            continue;
        }

        // Create STRegNode and add it to the vector
        STRegNode regNode;
        regNode.uReg = reg;
        regNode.uVal = val;
        vecRegList.push_back(regNode);
    }

    file.close();

    if (vecRegList.empty())
    {
        return false;
    }

    gvecRegList.swap(vecRegList);
    return true;
}

string sIniPathName()
{
    string strPath = "";
#ifdef _WIN32
    char exeFullPath[MAX_PATH];
    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
#endif
    strPath = exeFullPath;
    size_t pos = strPath.find_last_of('\\');
    if (pos != string::npos)
    {
        strPath = strPath.substr(0, pos + 1);
    }

    return strPath;
}

#ifndef WritePrivateProfileString
bool WritePrivateProfileString(const std::string& section, const std::string& key, const std::string& value, const std::string& fileName)
{
	std::map<std::string, std::map<std::string, std::string>> profileData;

	std::ifstream file(fileName);
	if (file)
	{
		std::string line;
		std::string currentSection;
		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#')
			{
				continue;
			}
			else if (line[0] == '[' && line[line.size() - 1] == ']')
			{
				currentSection = line.substr(1, line.size() - 2);
			}
			else
			{
				size_t separatorPos = line.find('=');
				if (separatorPos != std::string::npos)
				{
					std::string readKey = line.substr(0, separatorPos);
					std::string readValue = line.substr(separatorPos + 1);
					profileData[currentSection][readKey] = readValue;
				}
			}
		}
	}

	profileData[section][key] = value;

	std::ofstream fileOut(fileName);
	if (!fileOut)
	{
		return false;
	}

	for (const auto& sectionData : profileData)
	{
		fileOut << "[" << sectionData.first << "]" << std::endl;
		for (const auto& keyValuePair : sectionData.second)
		{
			fileOut << keyValuePair.first << "=" << keyValuePair.second << std::endl;
		}
		fileOut << std::endl;
	}

	return true;
}
#endif

#ifndef GetPrivateProfileString
std::string GetPrivateProfileString(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file)
	{
		return defaultValue;
	}

	std::string line;
	std::string currentSection;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			continue;
		}
		else if (line[0] == '[' && line[line.size() - 1] == ']')
		{
			currentSection = line.substr(1, line.size() - 2);
		}
		else
		{
			size_t separatorPos = line.find('=');
			if (separatorPos != std::string::npos)
			{
				std::string readKey = line.substr(0, separatorPos);
				std::string readValue = line.substr(separatorPos + 1);
				if (currentSection == section && readKey == key)
				{
					return readValue;
				}
			}
		}
	}

	return defaultValue;
}
#endif

#ifndef GetPrivateProfileInt
int GetPrivateProfileInt(const std::string& section, const std::string& key, int defaultValue, const std::string& fileName)
{
	std::string value = GetPrivateProfileString(section, key, std::to_string(defaultValue), fileName);
	return std::stoi(value);
}
#endif

bool WriteIniData(string sSection, string sSectionKey, string sFileName, int nValue)
{
    string sValue;
    sValue = to_string(nValue);
    return WritePrivateProfileString(sSection.c_str(), sSectionKey.c_str(), sValue.c_str(), sFileName.c_str());
}

// 获取配置文件中int格式的参数
int ReadIniData(string sSection, string sSectionKey, string sFileName, int nDefault)
{
    return GetPrivateProfileInt(sSection.c_str(), sSectionKey.c_str(), nDefault, sFileName.c_str());
}

string ReadIniString(string sSection, string sSectionKey, string sFileName, string sDefault)
{
    char strTem[MAX_PATH];
    GetPrivateProfileString(sSection.c_str(), sSectionKey.c_str(), sDefault.c_str(), strTem, MAX_PATH,
                            sFileName.c_str());
    string strResult(strTem);
    return strResult;
}

// int WriteRgb24ToBmpFile(unsigned char * pData, unsigned int nWidth, unsigned int nHeight, char *pFilePath)
int WriteRgb24ToBmpFile(unsigned char *pData, unsigned int nWidth, unsigned int nHeight, const char *pFilePath)
{
	std::ofstream file(pFilePath, std::ios::binary);
	if (!file)
	{
		return -1;
	}

	// Write BITMAPFILEHEADER
	uint32_t fileSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + nWidth * nHeight * 3;
	file.write(reinterpret_cast<const char*>(&fileSize), sizeof(uint32_t));
	uint32_t reserved = 0;
	file.write(reinterpret_cast<const char*>(&reserved), sizeof(uint32_t));
	uint32_t offset = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
	file.write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));

	// Write BITMAPINFOHEADER
	uint32_t headerSize = sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint16_t) + sizeof(uint16_t);
	file.write(reinterpret_cast<const char*>(&headerSize), sizeof(uint32_t));
	int32_t imageWidth = nWidth;
	file.write(reinterpret_cast<const char*>(&imageWidth), sizeof(int32_t));
	int32_t imageHeight = -static_cast<int32_t>(nHeight);
	file.write(reinterpret_cast<const char*>(&imageHeight), sizeof(int32_t));
	uint16_t planes = 1;
	file.write(reinterpret_cast<const char*>(&planes), sizeof(uint16_t));
	uint16_t bitCount = 24;
	file.write(reinterpret_cast<const char*>(&bitCount), sizeof(uint16_t));

	// Write image data
	file.write(reinterpret_cast<const char*>(pData), nWidth * nHeight * 3);

	return 0;
}

float fReadIniFloat(string sSection, string sKeyWord, string sFileName, float fDefault)
{
    char cTemRead[MAX_PATH];

    float fValue;
    string sTemp = to_string(fDefault);
    GetPrivateProfileString(sSection.c_str(), sKeyWord.c_str(), sTemp.c_str(), cTemRead, MAX_PATH, sFileName.c_str());
    sTemp = cTemRead;
    fValue = (float)atof(sTemp.c_str());
    return fValue;
}
